#include "src/solver/GmmxxMinMaxLinearEquationSolver.h"

#include <utility>

#include "src/settings/SettingsManager.h"
#include "src/adapters/GmmxxAdapter.h"
#include "src/utility/vector.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        GmmxxMinMaxLinearEquationSolver<ValueType>::GmmxxMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A) : gmmxxMatrix(storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<ValueType>(A)), rowGroupIndices(A.getRowGroupIndices()) {
            // Get the settings object to customize solving.
            storm::settings::modules::GmmxxEquationSolverSettings const& settings = storm::settings::gmmxxEquationSolverSettings();
            
            // Get appropriate settings.
            maximalNumberOfIterations = settings.getMaximalIterationCount();
            precision = settings.getPrecision();
            relative = settings.getConvergenceCriterion() == storm::settings::modules::GmmxxEquationSolverSettings::ConvergenceCriterion::Relative;
        }
        
        template<typename ValueType>
        GmmxxMinMaxLinearEquationSolver<ValueType>::GmmxxMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, double precision, uint_fast64_t maximalNumberOfIterations, bool relative) : gmmxxMatrix(storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<ValueType>(A)), rowGroupIndices(A.getRowGroupIndices()), precision(precision), relative(relative), maximalNumberOfIterations(maximalNumberOfIterations) {
            // Intentionally left empty.
        }

        
        template<typename ValueType>
        void GmmxxMinMaxLinearEquationSolver<ValueType>::solveEquationSystem(bool minimize, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult, std::vector<ValueType>* newX) const {
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
