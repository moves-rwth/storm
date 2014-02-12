#include "src/solver/TopologicalValueIterationNondeterministicLinearEquationSolver.h"

#include <utility>

#include "src/settings/Settings.h"
#include "src/utility/vector.h"
#include "src/utility/graph.h"
#include "src/models/PseudoModel.h"
#include "src/storage/StronglyConnectedComponentDecomposition.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
		TopologicalValueIterationNondeterministicLinearEquationSolver<ValueType>::TopologicalValueIterationNondeterministicLinearEquationSolver() {
			// Get the settings object to customize solving.
			storm::settings::Settings* settings = storm::settings::Settings::getInstance();

			// Get appropriate settings.
			this->maximalNumberOfIterations = settings->getOptionByLongName("maxiter").getArgument(0).getValueAsUnsignedInteger();
			this->precision = settings->getOptionByLongName("precision").getArgument(0).getValueAsDouble();
			this->relative = !settings->isSet("absolute");
        }
        
        template<typename ValueType>
		TopologicalValueIterationNondeterministicLinearEquationSolver<ValueType>::TopologicalValueIterationNondeterministicLinearEquationSolver(double precision, uint_fast64_t maximalNumberOfIterations, bool relative) : NativeNondeterministicLinearEquationSolver<ValueType>(precision, maximalNumberOfIterations, relative) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
		NondeterministicLinearEquationSolver<ValueType>* TopologicalValueIterationNondeterministicLinearEquationSolver<ValueType>::clone() const {
			return new TopologicalValueIterationNondeterministicLinearEquationSolver<ValueType>(*this);
        }
        
        template<typename ValueType>
		void TopologicalValueIterationNondeterministicLinearEquationSolver<ValueType>::solveEquationSystem(bool minimize, storm::storage::SparseMatrix<ValueType> const& A, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, std::vector<ValueType>* multiplyResult, std::vector<ValueType>* newX) const {
            
			// Now, we need to determine the SCCs of the MDP and a topological sort.
			//std::vector<std::vector<uint_fast64_t>> stronglyConnectedComponents = storm::utility::graph::performSccDecomposition(this->getModel(), stronglyConnectedComponents, stronglyConnectedComponentsDependencyGraph);
			//storm::storage::SparseMatrix<T> stronglyConnectedComponentsDependencyGraph = this->getModel().extractSccDependencyGraph(stronglyConnectedComponents);

			storm::models::NonDeterministicMatrixBasedPseudoModel<ValueType> pseudoModel(A, nondeterministicChoiceIndices);
			storm::storage::StronglyConnectedComponentDecomposition<ValueType> sccDecomposition(*static_cast<storm::models::AbstractPseudoModel<ValueType>*>(&pseudoModel), false, false);
			storm::storage::SparseMatrix<ValueType> stronglyConnectedComponentsDependencyGraph = pseudoModel.extractPartitionDependencyGraph(sccDecomposition);
			

			std::vector<uint_fast64_t> topologicalSort = storm::utility::graph::getTopologicalSort(stronglyConnectedComponentsDependencyGraph);

			// Set up the environment for the power method.
			bool multiplyResultMemoryProvided = true;
			if (multiplyResult == nullptr) {
				multiplyResult = new std::vector<ValueType>(A.getRowCount());
				multiplyResultMemoryProvided = false;
			}
			std::vector<ValueType>* currentX = &x;
			bool xMemoryProvided = true;
			if (newX == nullptr) {
				newX = new std::vector<ValueType>(x.size());
				xMemoryProvided = false;
			}
			std::vector<ValueType>* swap = nullptr;
			uint_fast64_t currentMaxLocalIterations = 0;
			uint_fast64_t localIterations = 0;
			uint_fast64_t globalIterations = 0;
			bool converged = true;

			// Iterate over all SCCs of the MDP as specified by the topological sort. This guarantees that an SCC is only
			// solved after all SCCs it depends on have been solved.
			int counter = 0;
			std::cout << "Solving Equation System using the TopologicalValueIterationNon..." << std::endl;
			for (auto sccIndexIt = topologicalSort.begin(); sccIndexIt != topologicalSort.end() && converged; ++sccIndexIt) {
				storm::storage::StateBlock const& scc = sccDecomposition[*sccIndexIt];

				std::cout << "SCC " << counter << " contains:" << std::endl;
				for (auto sccIt = scc.cbegin(); sccIt != scc.cend(); ++sccIt) {
					std::cout << *sccIt << ", ";
				}
				std::cout << std::endl;

				// For the current SCC, we need to perform value iteration until convergence.
				localIterations = 0;
				converged = false;
				while (!converged && localIterations < this->maximalNumberOfIterations) {
					// Compute x' = A*x + b.
					//A.multiplyWithVector(scc, nondeterministicChoiceIndices, *currentX, multiplyResult);
					//storm::utility::addVectors(scc, nondeterministicChoiceIndices, multiplyResult, b);

					/*
					Versus:
					A.multiplyWithVector(*currentX, *multiplyResult);
					storm::utility::vector::addVectorsInPlace(*multiplyResult, b);					
					*/

					// Reduce the vector x' by applying min/max for all non-deterministic choices.
					if (minimize) {
						//storm::utility::reduceVectorMin(*multiplyResult, *newX, scc, nondeterministicChoiceIndices);
					}
					else {
						//storm::utility::reduceVectorMax(*multiplyResult, *newX, scc, nondeterministicChoiceIndices);
					}

					// Determine whether the method converged.
					// TODO: It seems that the equalModuloPrecision call that compares all values should have a higher
					// running time. In fact, it is faster. This has to be investigated.
					// converged = storm::utility::equalModuloPrecision(*currentX, *newX, scc, precision, relative);
					converged = storm::utility::vector::equalModuloPrecision<ValueType>(*currentX, *newX, this->precision, this->relative);

					// Update environment variables.
					swap = currentX;
					currentX = newX;
					newX = swap;
					++localIterations;
					++globalIterations;
				}

				// As the "number of iterations" of the full method is the maximum of the local iterations, we need to keep
				// track of the maximum.
				if (localIterations > currentMaxLocalIterations) {
					currentMaxLocalIterations = localIterations;
				}
			}

			// If we performed an odd number of global iterations, we need to swap the x and currentX, because the newest
			// result is currently stored in currentX, but x is the output vector.
			// TODO: Check whether this is correct or should be put into the for-loop over SCCs.
			if (globalIterations % 2 == 1) {
				std::swap(x, *currentX);
			}
			
			if (!xMemoryProvided) {
				delete newX;
			}

			if (!multiplyResultMemoryProvided) {
				delete multiplyResult;
			}

			// Check if the solver converged and issue a warning otherwise.
			if (converged) {
				LOG4CPLUS_INFO(logger, "Iterative solver converged after " << currentMaxLocalIterations << " iterations.");
			}
			else {
				LOG4CPLUS_WARN(logger, "Iterative solver did not converged after " << currentMaxLocalIterations << " iterations.");
			}
        }

        // Explicitly instantiate the solver.
		template class TopologicalValueIterationNondeterministicLinearEquationSolver<double>;
    } // namespace solver
} // namespace storm
