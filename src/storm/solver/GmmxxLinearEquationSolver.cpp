#include "GmmxxLinearEquationSolver.h"

#include <cmath>
#include <utility>

#include "storm/adapters/GmmxxAdapter.h"
#include "storm/settings/SettingsManager.h"
#include "storm/utility/vector.h"
#include "storm/utility/constants.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/settings/modules/GmmxxEquationSolverSettings.h"

#include "storm/solver/NativeLinearEquationSolver.h"

#include "storm/utility/gmm.h"
#include "storm/utility/vector.h"

namespace storm {
    namespace solver {
        template<typename ValueType>
        GmmxxLinearEquationSolverSettings<ValueType>::GmmxxLinearEquationSolverSettings() {
            // Get the settings object to customize linear solving.
            storm::settings::modules::GmmxxEquationSolverSettings const& settings = storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>();
            
            // Get appropriate settings.
            maximalNumberOfIterations = settings.getMaximalIterationCount();
            precision = settings.getPrecision();
            relative = settings.getConvergenceCriterion() == storm::settings::modules::GmmxxEquationSolverSettings::ConvergenceCriterion::Relative;
            restart = settings.getRestartIterationCount();
            
            // Determine the method to be used.
            storm::settings::modules::GmmxxEquationSolverSettings::LinearEquationMethod methodAsSetting = settings.getLinearEquationSystemMethod();
            if (methodAsSetting == storm::settings::modules::GmmxxEquationSolverSettings::LinearEquationMethod::Bicgstab) {
                method = SolutionMethod::Bicgstab;
            } else if (methodAsSetting == storm::settings::modules::GmmxxEquationSolverSettings::LinearEquationMethod::Qmr) {
                method = SolutionMethod::Qmr;
            } else if (methodAsSetting == storm::settings::modules::GmmxxEquationSolverSettings::LinearEquationMethod::Gmres) {
                method = SolutionMethod::Gmres;
            } else if (methodAsSetting == storm::settings::modules::GmmxxEquationSolverSettings::LinearEquationMethod::Jacobi) {
                method = SolutionMethod::Jacobi;
            }
            
            // Check which preconditioner to use.
            storm::settings::modules::GmmxxEquationSolverSettings::PreconditioningMethod preconditionAsSetting = settings.getPreconditioningMethod();
            if (preconditionAsSetting == storm::settings::modules::GmmxxEquationSolverSettings::PreconditioningMethod::Ilu) {
                preconditioner = Preconditioner::Ilu;
            } else if (preconditionAsSetting == storm::settings::modules::GmmxxEquationSolverSettings::PreconditioningMethod::Diagonal) {
                preconditioner = Preconditioner::Diagonal;
            } else if (preconditionAsSetting == storm::settings::modules::GmmxxEquationSolverSettings::PreconditioningMethod::None) {
                preconditioner = Preconditioner::None;
            }
        }
        
        template<typename ValueType>
        void GmmxxLinearEquationSolverSettings<ValueType>::setSolutionMethod(SolutionMethod const& method) {
            this->method = method;
        }
        
        template<typename ValueType>
        void GmmxxLinearEquationSolverSettings<ValueType>::setPreconditioner(Preconditioner const& preconditioner) {
            this->preconditioner = preconditioner;
        }
        
        template<typename ValueType>
        void GmmxxLinearEquationSolverSettings<ValueType>::setPrecision(ValueType precision) {
            this->precision = precision;
        }
        
        template<typename ValueType>
        void GmmxxLinearEquationSolverSettings<ValueType>::setMaximalNumberOfIterations(uint64_t maximalNumberOfIterations) {
            this->maximalNumberOfIterations = maximalNumberOfIterations;
        }
        
        template<typename ValueType>
        void GmmxxLinearEquationSolverSettings<ValueType>::setRelativeTerminationCriterion(bool value) {
            this->relative = value;
        }
        
        template<typename ValueType>
        void GmmxxLinearEquationSolverSettings<ValueType>::setNumberOfIterationsUntilRestart(uint64_t restart) {
            this->restart = restart;
        }
        
        template<typename ValueType>
        typename GmmxxLinearEquationSolverSettings<ValueType>::SolutionMethod GmmxxLinearEquationSolverSettings<ValueType>::getSolutionMethod() const {
            return method;
        }
        
        template<typename ValueType>
        typename GmmxxLinearEquationSolverSettings<ValueType>::Preconditioner GmmxxLinearEquationSolverSettings<ValueType>::getPreconditioner() const {
            return preconditioner;
        }
        
        template<typename ValueType>
        ValueType GmmxxLinearEquationSolverSettings<ValueType>::getPrecision() const {
            return precision;
        }
        
        template<typename ValueType>
        uint64_t GmmxxLinearEquationSolverSettings<ValueType>::getMaximalNumberOfIterations() const {
            return maximalNumberOfIterations;
        }
        
        template<typename ValueType>
        bool GmmxxLinearEquationSolverSettings<ValueType>::getRelativeTerminationCriterion() const {
            return relative;
        }
        
        template<typename ValueType>
        uint64_t GmmxxLinearEquationSolverSettings<ValueType>::getNumberOfIterationsUntilRestart() const {
            return restart;
        }
        
        template<typename ValueType>
        GmmxxLinearEquationSolver<ValueType>::GmmxxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, GmmxxLinearEquationSolverSettings<ValueType> const& settings) : localA(nullptr), A(nullptr), settings(settings) {
            this->setMatrix(A);
        }

        template<typename ValueType>
        GmmxxLinearEquationSolver<ValueType>::GmmxxLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A, GmmxxLinearEquationSolverSettings<ValueType> const& settings) : localA(nullptr), A(nullptr), settings(settings) {
            this->setMatrix(std::move(A));
        }
        
        template<typename ValueType>
        void GmmxxLinearEquationSolver<ValueType>::setMatrix(storm::storage::SparseMatrix<ValueType> const& A) {
            localA.reset();
            this->A = &A;
            clearCache();
        }
        
        template<typename ValueType>
        void GmmxxLinearEquationSolver<ValueType>::setMatrix(storm::storage::SparseMatrix<ValueType>&& A) {
            localA = std::make_unique<storm::storage::SparseMatrix<ValueType>>(std::move(A));
            this->A = localA.get();
            clearCache();
        }
        
        template<typename ValueType>
        bool GmmxxLinearEquationSolver<ValueType>::solveEquations(std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            auto method = this->getSettings().getSolutionMethod();
            auto preconditioner = this->getSettings().getPreconditioner();
            STORM_LOG_INFO("Solving linear equation system (" << x.size() << " rows) with Gmmxx linear equation solver with method '" << method << "' and preconditioner '" << preconditioner << "' (max. " << this->getSettings().getMaximalNumberOfIterations() << " iterations).");
            if (method == GmmxxLinearEquationSolverSettings<ValueType>::SolutionMethod::Jacobi && preconditioner != GmmxxLinearEquationSolverSettings<ValueType>::Preconditioner::None) {
                STORM_LOG_WARN("Jacobi method currently does not support preconditioners. The requested preconditioner will be ignored.");
            }
            
            if (method == GmmxxLinearEquationSolverSettings<ValueType>::SolutionMethod::Bicgstab || method == GmmxxLinearEquationSolverSettings<ValueType>::SolutionMethod::Qmr || method == GmmxxLinearEquationSolverSettings<ValueType>::SolutionMethod::Gmres) {
                
                // Translate the matrix into gmm++ format (if not already done)
                if(!gmmxxA) {
                    gmmxxA = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<ValueType>(*A);
                }
                
                // Make sure that the requested preconditioner is available
                if (preconditioner == GmmxxLinearEquationSolverSettings<ValueType>::Preconditioner::Ilu && !iluPreconditioner) {
                    iluPreconditioner = std::make_unique<gmm::ilu_precond<gmm::csr_matrix<ValueType>>>(*gmmxxA);
                } else if (preconditioner == GmmxxLinearEquationSolverSettings<ValueType>::Preconditioner::Diagonal) {
                    diagonalPreconditioner = std::make_unique<gmm::diagonal_precond<gmm::csr_matrix<ValueType>>>(*gmmxxA);
                }
                
                // Prepare an iteration object that determines the accuracy and the maximum number of iterations.
                gmm::iteration iter(this->getSettings().getPrecision(), 0, this->getSettings().getMaximalNumberOfIterations());
                
                // Invoke gmm with the corresponding settings
                if (method == GmmxxLinearEquationSolverSettings<ValueType>::SolutionMethod::Bicgstab) {
                    if (preconditioner == GmmxxLinearEquationSolverSettings<ValueType>::Preconditioner::Ilu) {
                        gmm::bicgstab(*gmmxxA, x, b, *iluPreconditioner, iter);
                    } else if (preconditioner == GmmxxLinearEquationSolverSettings<ValueType>::Preconditioner::Diagonal) {
                        gmm::bicgstab(*gmmxxA, x, b, *diagonalPreconditioner, iter);
                    } else if (preconditioner == GmmxxLinearEquationSolverSettings<ValueType>::Preconditioner::None) {
                        gmm::bicgstab(*gmmxxA, x, b, gmm::identity_matrix(), iter);
                    }
                } else if (method == GmmxxLinearEquationSolverSettings<ValueType>::SolutionMethod::Qmr) {
                    if (preconditioner == GmmxxLinearEquationSolverSettings<ValueType>::Preconditioner::Ilu) {
                        gmm::qmr(*gmmxxA, x, b, *iluPreconditioner, iter);
                    } else if (preconditioner == GmmxxLinearEquationSolverSettings<ValueType>::Preconditioner::Diagonal) {
                        gmm::qmr(*gmmxxA, x, b, *diagonalPreconditioner, iter);
                    } else if (preconditioner == GmmxxLinearEquationSolverSettings<ValueType>::Preconditioner::None) {
                        gmm::qmr(*gmmxxA, x, b, gmm::identity_matrix(), iter);
                    }
                } else if (method == GmmxxLinearEquationSolverSettings<ValueType>::SolutionMethod::Gmres) {
                    if (preconditioner == GmmxxLinearEquationSolverSettings<ValueType>::Preconditioner::Ilu) {
                        gmm::gmres(*gmmxxA, x, b, *iluPreconditioner, this->getSettings().getNumberOfIterationsUntilRestart(), iter);
                    } else if (preconditioner == GmmxxLinearEquationSolverSettings<ValueType>::Preconditioner::Diagonal) {
                        gmm::gmres(*gmmxxA, x, b, *diagonalPreconditioner, this->getSettings().getNumberOfIterationsUntilRestart(), iter);
                    } else if (preconditioner == GmmxxLinearEquationSolverSettings<ValueType>::Preconditioner::None) {
                        gmm::gmres(*gmmxxA, x, b, gmm::identity_matrix(), this->getSettings().getNumberOfIterationsUntilRestart(), iter);
                    }
                }
                
                if (!this->isCachingEnabled()) {
                    clearCache();
                }
                
                // Make sure that all results conform to the bounds.
                storm::utility::vector::clip(x, this->lowerBound, this->upperBound);
                
                // Check if the solver converged and issue a warning otherwise.
                if (iter.converged()) {
                    STORM_LOG_INFO("Iterative solver converged after " << iter.get_iteration() << " iterations.");
                    return true;
                } else {
                    STORM_LOG_WARN("Iterative solver did not converge.");
                    return false;
                }
            } else if (method == GmmxxLinearEquationSolverSettings<ValueType>::SolutionMethod::Jacobi) {
                uint_fast64_t iterations = solveLinearEquationSystemWithJacobi(x, b);
                
                // Make sure that all results conform to the bounds.
                storm::utility::vector::clip(x, this->lowerBound, this->upperBound);
                
                // Check if the solver converged and issue a warning otherwise.
                if (iterations < this->getSettings().getMaximalNumberOfIterations()) {
                    STORM_LOG_INFO("Iterative solver converged after " << iterations << " iterations.");
                    return true;
                } else {
                    STORM_LOG_WARN("Iterative solver did not converge.");
                    return false;
                }
            }
            STORM_LOG_ERROR("Selected method is not available");
            return false;
        }
        
        template<typename ValueType>
        void GmmxxLinearEquationSolver<ValueType>::multiply(std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<ValueType>& result) const {
            if (!gmmxxA) {
                gmmxxA = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<ValueType>(*A);
            }
            if (b) {
                gmm::mult_add(*gmmxxA, x, *b, result);
            } else {
                gmm::mult(*gmmxxA, x, result);
            }
            
            if(!this->isCachingEnabled()) {
                clearCache();
            }
        }
        
        template<typename ValueType>
        uint_fast64_t GmmxxLinearEquationSolver<ValueType>::solveLinearEquationSystemWithJacobi(std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            
            // Get a Jacobi decomposition of the matrix A (if not already available).
            if (!jacobiDecomposition) {
                std::pair<storm::storage::SparseMatrix<ValueType>, std::vector<ValueType>> nativeJacobiDecomposition = A->getJacobiDecomposition();
                // Convert the LU matrix to gmm++'s format.
                jacobiDecomposition = std::make_unique<std::pair<gmm::csr_matrix<ValueType>, std::vector<ValueType>>>(*storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<ValueType>(std::move(nativeJacobiDecomposition.first)), std::move(nativeJacobiDecomposition.second));
            }
            gmm::csr_matrix<ValueType> const& jacobiLU = jacobiDecomposition->first;
            std::vector<ValueType> const& jacobiD = jacobiDecomposition->second;
        
            std::vector<ValueType>* currentX = &x;
            
            if (!this->cachedRowVector) {
                this->cachedRowVector = std::make_unique<std::vector<ValueType>>(getMatrixRowCount());
            }
            std::vector<ValueType>* nextX = this->cachedRowVector.get();
            
            // Set up additional environment variables.
            uint_fast64_t iterationCount = 0;
            bool converged = false;
            
            while (!converged && iterationCount < this->getSettings().getMaximalNumberOfIterations() && !(this->hasCustomTerminationCondition() && this->getTerminationCondition().terminateNow(*currentX))) {
                // Compute D^-1 * (b - LU * x) and store result in nextX.
                gmm::mult_add(jacobiLU, gmm::scaled(*currentX, -storm::utility::one<ValueType>()), b, *nextX);
                storm::utility::vector::multiplyVectorsPointwise(jacobiD, *nextX, *nextX);
                
                // Now check if the process already converged within our precision.
                converged = storm::utility::vector::equalModuloPrecision(*currentX, *nextX, this->getSettings().getPrecision(), this->getSettings().getRelativeTerminationCriterion());

                // Swap the two pointers as a preparation for the next iteration.
                std::swap(nextX, currentX);

                // Increase iteration count so we can abort if convergence is too slow.
                ++iterationCount;
            }
            
            // If the last iteration did not write to the original x we have to swap the contents, because the
            // output has to be written to the input parameter x.
            if (currentX == this->cachedRowVector.get()) {
                std::swap(x, *currentX);
            }
            
            if(!this->isCachingEnabled()) {
                clearCache();
            }
            
            return iterationCount;
        }
        
        template<typename ValueType>
        void GmmxxLinearEquationSolver<ValueType>::setSettings(GmmxxLinearEquationSolverSettings<ValueType> const& newSettings) {
            settings = newSettings;
        }

        template<typename ValueType>
        GmmxxLinearEquationSolverSettings<ValueType> const& GmmxxLinearEquationSolver<ValueType>::getSettings() const {
            return settings;
        }
        
        template<typename ValueType>
        void GmmxxLinearEquationSolver<ValueType>::clearCache() const {
            gmmxxA.reset();
            iluPreconditioner.reset();
            diagonalPreconditioner.reset();
            jacobiDecomposition.reset();
            LinearEquationSolver<ValueType>::clearCache();
        }
        
        template<typename ValueType>
        uint64_t GmmxxLinearEquationSolver<ValueType>::getMatrixRowCount() const {
            return this->A->getRowCount();
        }
        
        template<typename ValueType>
        uint64_t GmmxxLinearEquationSolver<ValueType>::getMatrixColumnCount() const {
            return this->A->getColumnCount();
        }
        
        template<typename ValueType>
        std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> GmmxxLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType> const& matrix) const {
            return std::make_unique<storm::solver::GmmxxLinearEquationSolver<ValueType>>(matrix, settings);
        }
        
        template<typename ValueType>
        std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> GmmxxLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType>&& matrix) const {
            return std::make_unique<storm::solver::GmmxxLinearEquationSolver<ValueType>>(std::move(matrix), settings);
        }
        
        template<typename ValueType>
        std::unique_ptr<LinearEquationSolverFactory<ValueType>> GmmxxLinearEquationSolverFactory<ValueType>::clone() const {
            return std::make_unique<GmmxxLinearEquationSolverFactory<ValueType>>(*this);
        }
        
        template<typename ValueType>
        GmmxxLinearEquationSolverSettings<ValueType>& GmmxxLinearEquationSolverFactory<ValueType>::getSettings() {
            return settings;
        }
        
        template<typename ValueType>
        GmmxxLinearEquationSolverSettings<ValueType> const& GmmxxLinearEquationSolverFactory<ValueType>::getSettings() const {
            return settings;
        }
        
        // Explicitly instantiate the solver.
        template class GmmxxLinearEquationSolverSettings<double>;
        template class GmmxxLinearEquationSolver<double>;
        template class GmmxxLinearEquationSolverFactory<double>;
        
    }
}
