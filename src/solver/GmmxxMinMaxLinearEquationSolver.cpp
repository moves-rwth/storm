#include "src/solver/GmmxxMinMaxLinearEquationSolver.h"

#include <utility>

#include "src/settings/SettingsManager.h"
#include "src/adapters/GmmxxAdapter.h"
#include "src/solver/GmmxxLinearEquationSolver.h"
#include "src/utility/vector.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        GmmxxMinMaxLinearEquationSolver<ValueType>::GmmxxMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A) : gmmxxMatrix(storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<ValueType>(A)), stormMatrix(A), rowGroupIndices(A.getRowGroupIndices()) {
            // Get the settings object to customize solving.
            storm::settings::modules::GmmxxEquationSolverSettings const& settings = storm::settings::gmmxxEquationSolverSettings();
			storm::settings::modules::GeneralSettings const& generalSettings = storm::settings::generalSettings();
            
            // Get appropriate settings.
            maximalNumberOfIterations = settings.getMaximalIterationCount();
            precision = settings.getPrecision();
            relative = settings.getConvergenceCriterion() == storm::settings::modules::GmmxxEquationSolverSettings::ConvergenceCriterion::Relative;
			useValueIteration = (generalSettings.getMinMaxEquationSolvingTechnique() == storm::settings::modules::GeneralSettings::MinMaxTechnique::ValueIteration);
        }
        
        template<typename ValueType>
		GmmxxMinMaxLinearEquationSolver<ValueType>::GmmxxMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, double precision, uint_fast64_t maximalNumberOfIterations, bool useValueIteration, bool relative) : gmmxxMatrix(storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<ValueType>(A)), stormMatrix(A), rowGroupIndices(A.getRowGroupIndices()), precision(precision), maximalNumberOfIterations(maximalNumberOfIterations), useValueIteration(useValueIteration), relative(relative) {
            // Intentionally left empty.
        }

        
        template<typename ValueType>
        void GmmxxMinMaxLinearEquationSolver<ValueType>::solveEquationSystem(bool minimize, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult, std::vector<ValueType>* newX) const {
			if (useValueIteration) {
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
				while (!converged && iterations < maximalNumberOfIterations) {
					// Compute x' = A*x + b.
					gmm::mult(*gmmxxMatrix, *currentX, *multiplyResult);
					gmm::add(b, *multiplyResult);

					// Reduce the vector x by applying min/max over all nondeterministic choices.
					if (minimize) {
						storm::utility::vector::reduceVectorMin(*multiplyResult, *newX, rowGroupIndices);
					} else {
						storm::utility::vector::reduceVectorMax(*multiplyResult, *newX, rowGroupIndices);
					}

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
				// We first define an initial choice resolution which will be refined after each iteration.
				std::vector<storm::storage::SparseMatrix<ValueType>::index_type> choiceVector(rowGroupIndices.size() - 1);

				// Create our own multiplyResult for solving the deterministic instances.
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
				while (!converged && iterations < maximalNumberOfIterations) {
					// Take the sub-matrix according to the current choices
					storm::storage::SparseMatrix<ValueType> submatrix = stormMatrix.selectRowsFromRowGroups(choiceVector, true);
					submatrix.convertToEquationSystem();
					
					GmmxxLinearEquationSolver<ValueType> gmmLinearEquationSolver(submatrix, storm::solver::GmmxxLinearEquationSolver<double>::SolutionMethod::Gmres, precision, maximalNumberOfIterations, storm::solver::GmmxxLinearEquationSolver<double>::Preconditioner::None, relative, 50);
					storm::utility::vector::selectVectorValues<ValueType>(subB, choiceVector, rowGroupIndices, b);

					// Copy X since we will overwrite it
					std::copy(currentX->begin(), currentX->end(), newX->begin());

					// Solve the resulting linear equation system
					gmmLinearEquationSolver.solveEquationSystem(*newX, subB, &deterministicMultiplyResult);

					// Compute x' = A*x + b. This step is necessary to allow the choosing of the optimal policy for the next iteration.
					gmm::mult(*gmmxxMatrix, *newX, *multiplyResult);
					gmm::add(b, *multiplyResult);

					// Reduce the vector x by applying min/max over all nondeterministic choices.
					if (minimize) {
						storm::utility::vector::reduceVectorMin(*multiplyResult, *newX, rowGroupIndices, &choiceVector);
					} else {
						storm::utility::vector::reduceVectorMax(*multiplyResult, *newX, rowGroupIndices, &choiceVector);
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
                
                if (minimize) {
                    storm::utility::vector::reduceVectorMin(*multiplyResult, x, rowGroupIndices);
                } else {
                    storm::utility::vector::reduceVectorMax(*multiplyResult, x, rowGroupIndices);
                }
            }
            
            if (!multiplyResultMemoryProvided) {
                delete multiplyResult;
            }
        }

        // Explicitly instantiate the solver.
        template class GmmxxMinMaxLinearEquationSolver<double>;
    } // namespace solver
} // namespace storm
