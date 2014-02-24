#include "src/solver/TopologicalValueIterationNondeterministicLinearEquationSolver.h"

#include <utility>

#include "src/settings/Settings.h"
#include "src/utility/vector.h"
#include "src/utility/graph.h"
#include "src/models/PseudoModel.h"
#include "src/storage/StronglyConnectedComponentDecomposition.h"
#include "src/exceptions/IllegalArgumentException.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

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

			std::cout << "TopoSolver Input Matrix: " << A.getRowCount() << " x " << A.getColumnCount() << " with " << A.getEntryCount() << " Entries:" << std::endl;

			uint_fast64_t const rowCount = A.getRowCount();
			for (uint_fast64_t row = 0; row < rowCount; ++row) {
				std::cout << "Row " << row << ": ";
				auto const& rowElement = A.getRow(row);
				for (auto rowIt = rowElement.begin(); rowIt != rowElement.end(); ++rowIt) {
					std::cout << rowIt->first << " [" << rowIt->second << "], ";
				}
				std::cout << std::endl;
			}


			storm::models::NonDeterministicMatrixBasedPseudoModel<ValueType> pseudoModel(A, nondeterministicChoiceIndices);
			//storm::storage::StronglyConnectedComponentDecomposition<ValueType> sccDecomposition(*static_cast<storm::models::AbstractPseudoModel<ValueType>*>(&pseudoModel), false, false);
			storm::storage::StronglyConnectedComponentDecomposition<ValueType> sccDecomposition(pseudoModel, false, false);

			if (sccDecomposition.size() == 0) {
				LOG4CPLUS_ERROR(logger, "Can not solve given Equation System as the SCC Decomposition returned no SCCs.");
				throw storm::exceptions::IllegalArgumentException() << "Can not solve given Equation System as the SCC Decomposition returned no SCCs.";
			}

			storm::storage::SparseMatrix<ValueType> stronglyConnectedComponentsDependencyGraph = pseudoModel.extractPartitionDependencyGraph(sccDecomposition);
			std::vector<uint_fast64_t> topologicalSort = storm::utility::graph::getTopologicalSort(stronglyConnectedComponentsDependencyGraph);

			// Set up the environment for the power method.
//			bool multiplyResultMemoryProvided = true;
//			if (multiplyResult == nullptr) {
//				multiplyResult = new std::vector<ValueType>(A.getRowCount());
//				multiplyResultMemoryProvided = false;
//			}
			std::vector<ValueType>* currentX = nullptr;
			//bool xMemoryProvided = true;
			//if (newX == nullptr) {
			//	newX = new std::vector<ValueType>(x.size());
			//	xMemoryProvided = false;
			//}
			std::vector<ValueType>* swap = nullptr;
			uint_fast64_t currentMaxLocalIterations = 0;
			uint_fast64_t localIterations = 0;
			uint_fast64_t globalIterations = 0;
			bool converged = true;

			// Iterate over all SCCs of the MDP as specified by the topological sort. This guarantees that an SCC is only
			// solved after all SCCs it depends on have been solved.
			int counter = 0;
			std::cout << "Solving Equation System using the TopologicalValueIterationNon..." << std::endl;
			std::cout << "Found " << sccDecomposition.size() << " SCCs." << std::endl;

			for (auto sccIndexIt = topologicalSort.begin(); sccIndexIt != topologicalSort.end() && converged; ++sccIndexIt) {
				storm::storage::StateBlock const& scc = sccDecomposition[*sccIndexIt];

				std::cout << "SCC " << counter << " from Index " << *sccIndexIt << " contains:" << std::endl;
				++counter;
				for (auto state : scc) {
					std::cout << state << ", ";
				}
				std::cout << std::endl;

				// Generate a submatrix
				storm::storage::BitVector subMatrixIndices(A.getColumnCount(), scc.cbegin(), scc.cend());
				storm::storage::SparseMatrix<ValueType> sccSubmatrix = A.getSubmatrix(subMatrixIndices, nondeterministicChoiceIndices);
				std::vector<ValueType> sccSubB(sccSubmatrix.getRowCount());
				storm::utility::vector::selectVectorValues<ValueType>(sccSubB, subMatrixIndices, nondeterministicChoiceIndices, b);
				std::vector<ValueType> sccSubX(sccSubmatrix.getColumnCount());
				std::vector<ValueType> sccSubXSwap(sccSubmatrix.getColumnCount());
                std::vector<ValueType> sccMultiplyResult(sccSubmatrix.getRowCount());
                
				// Prepare the pointers for swapping in the calculation
				currentX = &sccSubX;
				swap = &sccSubXSwap;

				storm::utility::vector::selectVectorValues<ValueType>(sccSubX, subMatrixIndices, x); // x is getCols() large, where as b and multiplyResult are getRows() (nondet. choices times states)
				std::vector<uint_fast64_t> sccSubNondeterministicChoiceIndices(sccSubmatrix.getColumnCount() + 1);
				sccSubNondeterministicChoiceIndices.at(0) = 0;

				// Preprocess all dependant states
				// Remove outgoing transitions and create the ChoiceIndices
				uint_fast64_t innerIndex = 0;
                uint_fast64_t outerIndex = 0;
				for (uint_fast64_t state: scc) {
					// Choice Indices
					sccSubNondeterministicChoiceIndices.at(outerIndex + 1) = sccSubNondeterministicChoiceIndices.at(outerIndex) + (nondeterministicChoiceIndices[state + 1] - nondeterministicChoiceIndices[state]);

					for (auto rowGroupIt = nondeterministicChoiceIndices[state]; rowGroupIt != nondeterministicChoiceIndices[state + 1]; ++rowGroupIt) {
						typename storm::storage::SparseMatrix<ValueType>::const_rows row = A.getRow(rowGroupIt);
						for (auto rowIt = row.begin(); rowIt != row.end(); ++rowIt) {
							if (!subMatrixIndices.get(rowIt->first)) {
								// This is an outgoing transition of a state in the SCC to a state not included in the SCC
								// Subtracting Pr(tau) * x_other from b fixes that
								sccSubB.at(innerIndex) = sccSubB.at(innerIndex) + (rowIt->second * x.at(rowIt->first));
							}
						}
                        ++innerIndex;
					}
                    ++outerIndex;
				}

				// For the current SCC, we need to perform value iteration until convergence.
				localIterations = 0;
				converged = false;
				while (!converged && localIterations < this->maximalNumberOfIterations) {
					// Compute x' = A*x + b.
					sccSubmatrix.multiplyWithVector(*currentX, sccMultiplyResult);
					storm::utility::vector::addVectorsInPlace<ValueType>(sccMultiplyResult, sccSubB);

					//A.multiplyWithVector(scc, nondeterministicChoiceIndices, *currentX, multiplyResult);
					//storm::utility::addVectors(scc, nondeterministicChoiceIndices, multiplyResult, b);

					/*
					Versus:
					A.multiplyWithVector(*currentX, *multiplyResult);
					storm::utility::vector::addVectorsInPlace(*multiplyResult, b);					
					*/

					// Reduce the vector x' by applying min/max for all non-deterministic choices.
					if (minimize) {
						storm::utility::vector::reduceVectorMin<ValueType>(sccMultiplyResult, *swap, sccSubNondeterministicChoiceIndices);
					}
					else {
						storm::utility::vector::reduceVectorMax<ValueType>(sccMultiplyResult, *swap, sccSubNondeterministicChoiceIndices);
					}

					// Determine whether the method converged.
					// TODO: It seems that the equalModuloPrecision call that compares all values should have a higher
					// running time. In fact, it is faster. This has to be investigated.
					// converged = storm::utility::equalModuloPrecision(*currentX, *newX, scc, precision, relative);
					converged = storm::utility::vector::equalModuloPrecision<ValueType>(*currentX, *swap, this->precision, this->relative);

					// Update environment variables.
					std::swap(currentX, swap);

					++localIterations;
					++globalIterations;
				}

				// The Result of this SCC has to be taken back into the main result vector
				innerIndex = 0;
				for (uint_fast64_t state: scc) {
					x.at(state) = currentX->at(innerIndex);
					++innerIndex;
				}

				// Since the pointers for swapping in the calculation point to temps they should not be valide anymore
				currentX = nullptr;
				swap = nullptr;

				// As the "number of iterations" of the full method is the maximum of the local iterations, we need to keep
				// track of the maximum.
				if (localIterations > currentMaxLocalIterations) {
					currentMaxLocalIterations = localIterations;
				}
			}
			
			//if (!xMemoryProvided) {
			//	delete newX;
			//}

//			if (!multiplyResultMemoryProvided) {
//				delete multiplyResult;
//			}

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
