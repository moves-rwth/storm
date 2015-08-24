#include "src/solver/GmmxxMinMaxLinearEquationSolver.h"

#include <utility>

#include "src/settings/SettingsManager.h"
#include "src/adapters/GmmxxAdapter.h"
#include "src/solver/GmmxxLinearEquationSolver.h"
#include "src/utility/vector.h"

#include "src/settings/modules/GeneralSettings.h"
#include "src/settings/modules/GmmxxEquationSolverSettings.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        GmmxxMinMaxLinearEquationSolver<ValueType>::GmmxxMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, MinMaxTechniqueSelection preferredTechnique, bool trackPolicy) : 
        MinMaxLinearEquationSolver<ValueType>(A, storm::settings::gmmxxEquationSolverSettings().getPrecision(), \
                                                storm::settings::gmmxxEquationSolverSettings().getConvergenceCriterion() == storm::settings::modules::GmmxxEquationSolverSettings::ConvergenceCriterion::Relative,\
                                                storm::settings::gmmxxEquationSolverSettings().getMaximalIterationCount(), trackPolicy, preferredTechnique),
            gmmxxMatrix(storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<ValueType>(A)), rowGroupIndices(A.getRowGroupIndices()) {
           
			
        }
        
        template<typename ValueType>
		GmmxxMinMaxLinearEquationSolver<ValueType>::GmmxxMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, double precision, uint_fast64_t maximalNumberOfIterations, MinMaxTechniqueSelection tech, bool relative, bool trackPolicy) : MinMaxLinearEquationSolver<ValueType>(A, precision, relative, maximalNumberOfIterations, trackPolicy, tech), gmmxxMatrix(storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<ValueType>(A)),  rowGroupIndices(A.getRowGroupIndices())  {
            // Intentionally left empty.
        }

        
        template<typename ValueType>
        void GmmxxMinMaxLinearEquationSolver<ValueType>::solveEquationSystem(bool minimize, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult, std::vector<ValueType>* newX) const {
			if (this->useValueIteration) {
				// Set up the environment for the power method. If scratch memory was not provided, we need to create it.
				bool multiplyResultMemoryProvided = true;
				if (multiplyResult == nullptr) {
					multiplyResult = new std::vector<ValueType>(b.size());
					multiplyResultMemoryProvided = false;
				}

				std::vector<ValueType>* currentX = &x;
				bool xMemoryProvided = true;
				if (newX == nullptr) {
					newX = new std::vector<ValueType>(x.size());
					xMemoryProvided = false;
				}
				uint_fast64_t iterations = 0;
				bool converged = false;

				// Keep track of which of the vectors for x is the auxiliary copy.
				std::vector<ValueType>* copyX = newX;

				// Proceed with the iterations as long as the method did not converge or reach the user-specified maximum number
				// of iterations.
				while (!converged && iterations < this->maximalNumberOfIterations) {
					// Compute x' = A*x + b.
					gmm::mult(*gmmxxMatrix, *currentX, *multiplyResult);
					gmm::add(b, *multiplyResult);

					// Reduce the vector x by applying min/max over all nondeterministic choices.
					storm::utility::vector::reduceVectorMinOrMax(minimize, *multiplyResult, *newX, rowGroupIndices);
					
					// Determine whether the method converged.
					converged = storm::utility::vector::equalModuloPrecision(*currentX, *newX, this->precision, this->relative);

					// Update environment variables.
					std::swap(currentX, newX);
					++iterations;
				}

				// Check if the solver converged and issue a warning otherwise.
				if (converged) {
					LOG4CPLUS_INFO(logger, "Iterative solver converged after " << iterations << " iterations.");
				} else {
					LOG4CPLUS_WARN(logger, "Iterative solver did not converge after " << iterations << " iterations.");
				}

				// If we performed an odd number of iterations, we need to swap the x and currentX, because the newest result
				// is currently stored in currentX, but x is the output vector.
				if (currentX == copyX) {
					std::swap(x, *currentX);
				}

				if (!xMemoryProvided) {
					delete copyX;
				}

				if (!multiplyResultMemoryProvided) {
					delete multiplyResult;
				}
			} else {
				// We will use Policy Iteration to solve the given system.
				// We first guess an initial choice resolution which will be refined after each iteration.
				this->policy = std::vector<storm::storage::sparse::state_type>(this->A.getRowGroupIndices().size() - 1);

				// Create our own multiplyResult for solving the deterministic sub-instances.
				std::vector<ValueType> deterministicMultiplyResult(rowGroupIndices.size() - 1);
				std::vector<ValueType> subB(rowGroupIndices.size() - 1);

				bool multiplyResultMemoryProvided = true;
				if (multiplyResult == nullptr) {
					multiplyResult = new std::vector<ValueType>(b.size());
					multiplyResultMemoryProvided = false;
				}

				std::vector<ValueType>* currentX = &x;
				bool xMemoryProvided = true;
				if (newX == nullptr) {
					newX = new std::vector<ValueType>(x.size());
					xMemoryProvided = false;
				}

				uint_fast64_t iterations = 0;
				bool converged = false;

				// Keep track of which of the vectors for x is the auxiliary copy.
				std::vector<ValueType>* copyX = newX;

				// Proceed with the iterations as long as the method did not converge or reach the user-specified maximum number
				// of iterations.
				while (!converged && iterations < this->maximalNumberOfIterations) {
					// Take the sub-matrix according to the current choices
					storm::storage::SparseMatrix<ValueType> submatrix = this->A.selectRowsFromRowGroups(this->policy, true);
					submatrix.convertToEquationSystem();
					
					GmmxxLinearEquationSolver<ValueType> gmmLinearEquationSolver(submatrix, storm::solver::GmmxxLinearEquationSolver<double>::SolutionMethod::Gmres, this->precision, this->maximalNumberOfIterations, storm::solver::GmmxxLinearEquationSolver<double>::Preconditioner::None, this->relative, 50);
					storm::utility::vector::selectVectorValues<ValueType>(subB, this->policy, rowGroupIndices, b);

					// Copy X since we will overwrite it
					std::copy(currentX->begin(), currentX->end(), newX->begin());

					// Solve the resulting linear equation system
					gmmLinearEquationSolver.solveEquationSystem(*newX, subB, &deterministicMultiplyResult);

					// Compute x' = A*x + b. This step is necessary to allow the choosing of the optimal policy for the next iteration.
					gmm::mult(*gmmxxMatrix, *newX, *multiplyResult);
					gmm::add(b, *multiplyResult);

					// Reduce the vector x by applying min/max over all nondeterministic choices.
					// Here, we capture which choice was taken in each state, thereby refining our initial guess.
					storm::utility::vector::reduceVectorMinOrMax(minimize, *multiplyResult, *newX, rowGroupIndices, &(this->policy));
					

					// Determine whether the method converged.
					converged = storm::utility::vector::equalModuloPrecision<ValueType>(*currentX, *newX, static_cast<ValueType>(this->precision), this->relative);

					// Update environment variables.
					std::swap(currentX, newX);
					++iterations;
				}

				// Check if the solver converged and issue a warning otherwise.
				if (converged) {
					LOG4CPLUS_INFO(logger, "Iterative solver converged after " << iterations << " iterations.");
				} else {
					LOG4CPLUS_WARN(logger, "Iterative solver did not converge after " << iterations << " iterations.");
				}

				// If we performed an odd number of iterations, we need to swap the x and currentX, because the newest result
				// is currently stored in currentX, but x is the output vector.
				if (currentX == copyX) {
					std::swap(x, *currentX);
				}

				if (!xMemoryProvided) {
					delete copyX;
				}

				if (!multiplyResultMemoryProvided) {
					delete multiplyResult;
				}
			}
        }
        
        template<typename ValueType>
        void GmmxxMinMaxLinearEquationSolver<ValueType>::performMatrixVectorMultiplication(bool minimize, std::vector<ValueType>& x, std::vector<ValueType>* b, uint_fast64_t n, std::vector<ValueType>* multiplyResult) const {
            bool multiplyResultMemoryProvided = true;
            if (multiplyResult == nullptr) {
                multiplyResult = new std::vector<ValueType>(gmmxxMatrix->nr);
                multiplyResultMemoryProvided = false;
            }
            
            // Now perform matrix-vector multiplication as long as we meet the bound of the formula.
            for (uint_fast64_t i = 0; i < n; ++i) {
                gmm::mult(*gmmxxMatrix, x, *multiplyResult);
                
                if (b != nullptr) {
                    gmm::add(*b, *multiplyResult);
                }
                
                storm::utility::vector::reduceVectorMinOrMax(minimize, *multiplyResult, x, rowGroupIndices);
                
            }
            
            if (!multiplyResultMemoryProvided) {
                delete multiplyResult;
            }
        }

        // Explicitly instantiate the solver.
        template class GmmxxMinMaxLinearEquationSolver<double>;
    } // namespace solver
} // namespace storm
