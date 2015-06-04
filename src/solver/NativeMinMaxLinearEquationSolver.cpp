#include "src/solver/NativeMinMaxLinearEquationSolver.h"

#include <utility>

#include "src/settings/SettingsManager.h"
#include "src/utility/vector.h"
#include "src/solver/NativeLinearEquationSolver.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        NativeMinMaxLinearEquationSolver<ValueType>::NativeMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A) : A(A) {
            // Get the settings object to customize solving.
            storm::settings::modules::NativeEquationSolverSettings const& settings = storm::settings::nativeEquationSolverSettings();
			storm::settings::modules::GeneralSettings const& generalSettings = storm::settings::generalSettings();
            
            // Get appropriate settings.
			precision = settings.getPrecision();
			relative = settings.getConvergenceCriterion() == storm::settings::modules::NativeEquationSolverSettings::ConvergenceCriterion::Relative;
            maximalNumberOfIterations = settings.getMaximalIterationCount();
			useValueIteration = (generalSettings.getMinMaxEquationSolvingTechnique() == storm::settings::modules::GeneralSettings::MinMaxTechnique::ValueIteration);
        }
        
        template<typename ValueType>
		NativeMinMaxLinearEquationSolver<ValueType>::NativeMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, double precision, uint_fast64_t maximalNumberOfIterations, bool useValueIteration, bool relative) : A(A), precision(precision), relative(relative), maximalNumberOfIterations(maximalNumberOfIterations), useValueIteration(useValueIteration) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        void NativeMinMaxLinearEquationSolver<ValueType>::solveEquationSystem(bool minimize, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult, std::vector<ValueType>* newX) const {
			if (useValueIteration) {
				// Set up the environment for the power method. If scratch memory was not provided, we need to create it.
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
				uint_fast64_t iterations = 0;
				bool converged = false;

				// Keep track of which of the vectors for x is the auxiliary copy.
				std::vector<ValueType>* copyX = newX;

				// Proceed with the iterations as long as the method did not converge or reach the
				// user-specified maximum number of iterations.
				while (!converged && iterations < maximalNumberOfIterations) {
					// Compute x' = A*x + b.
					A.multiplyWithVector(*currentX, *multiplyResult);
					storm::utility::vector::addVectors(*multiplyResult, b, *multiplyResult);

					// Reduce the vector x' by applying min/max for all non-deterministic choices as given by the topmost
					// element of the min/max operator stack.
					if (minimize) {
						storm::utility::vector::reduceVectorMin(*multiplyResult, *newX, A.getRowGroupIndices());
					} else {
						storm::utility::vector::reduceVectorMax(*multiplyResult, *newX, A.getRowGroupIndices());
					}

					// Determine whether the method converged.
					converged = storm::utility::vector::equalModuloPrecision<ValueType>(*currentX, *newX, static_cast<ValueType>(precision), relative);

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
				std::vector<typename storm::storage::SparseMatrix<ValueType>::index_type> choiceVector(A.getRowGroupIndices().size() - 1);

				// Create our own multiplyResult for solving the deterministic sub-instances.
				std::vector<ValueType> deterministicMultiplyResult(A.getRowGroupIndices().size() - 1);
				std::vector<ValueType> subB(A.getRowGroupIndices().size() - 1);

				// Check whether intermediate storage was provided and create it otherwise.
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
				while (!converged && iterations < maximalNumberOfIterations) {
					// Take the sub-matrix according to the current choices
					storm::storage::SparseMatrix<ValueType> submatrix = A.selectRowsFromRowGroups(choiceVector, true);
					submatrix.convertToEquationSystem();

					NativeLinearEquationSolver<ValueType> nativeLinearEquationSolver(submatrix);
					storm::utility::vector::selectVectorValues<ValueType>(subB, choiceVector, A.getRowGroupIndices(), b);

					// Copy X since we will overwrite it
					std::copy(currentX->begin(), currentX->end(), newX->begin());

					// Solve the resulting linear equation system of the sub-instance for x under the current choices
					nativeLinearEquationSolver.solveEquationSystem(*newX, subB, &deterministicMultiplyResult);

					// Compute x' = A*x + b. This step is necessary to allow the choosing of the optimal policy for the next iteration.
					A.multiplyWithVector(*newX, *multiplyResult);
					storm::utility::vector::addVectors(*multiplyResult, b, *multiplyResult);

					// Reduce the vector x by applying min/max over all nondeterministic choices.
					// Here, we capture which choice was taken in each state, thereby refining our initial guess.
					if (minimize) {
						storm::utility::vector::reduceVectorMin(*multiplyResult, *newX, A.getRowGroupIndices(), &choiceVector);
					} else {
						storm::utility::vector::reduceVectorMax(*multiplyResult, *newX, A.getRowGroupIndices(), &choiceVector);
					}

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
        void NativeMinMaxLinearEquationSolver<ValueType>::performMatrixVectorMultiplication(bool minimize, std::vector<ValueType>& x, std::vector<ValueType>* b, uint_fast64_t n, std::vector<ValueType>* multiplyResult) const {
            
            // If scratch memory was not provided, we need to create it.
            bool multiplyResultMemoryProvided = true;
            if (multiplyResult == nullptr) {
                multiplyResult = new std::vector<ValueType>(A.getRowCount());
                multiplyResultMemoryProvided = false;
            }

            // Now perform matrix-vector multiplication as long as we meet the bound of the formula.
            for (uint_fast64_t i = 0; i < n; ++i) {
                A.multiplyWithVector(x, *multiplyResult);
                
                // Add b if it is non-null.
                if (b != nullptr) {
                    storm::utility::vector::addVectors(*multiplyResult, *b, *multiplyResult);
                }
                
                // Reduce the vector x' by applying min/max for all non-deterministic choices as given by the topmost
                // element of the min/max operator stack.
                if (minimize) {
                    storm::utility::vector::reduceVectorMin(*multiplyResult, x, A.getRowGroupIndices());
                } else {
                    storm::utility::vector::reduceVectorMax(*multiplyResult, x, A.getRowGroupIndices());
                }
            }
            
            if (!multiplyResultMemoryProvided) {
                delete multiplyResult;
            }
        }
        
        // Explicitly instantiate the solver.
        template class NativeMinMaxLinearEquationSolver<double>;
		template class NativeMinMaxLinearEquationSolver<float>;
    } // namespace solver
} // namespace storm
