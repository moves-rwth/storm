#include "GmmxxLinearEquationSolver.h"

#include <cmath>
#include <utility>

#include "src/adapters/GmmxxAdapter.h"
#include "src/settings/SettingsManager.h"
#include "src/utility/vector.h"
#include "src/utility/constants.h"
#include "src/exceptions/InvalidStateException.h"
#include "src/settings/modules/GmmxxEquationSolverSettings.h"

#include "src/solver/NativeLinearEquationSolver.h"

#include "src/utility/gmm.h"

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
        GmmxxLinearEquationSolver<ValueType>::GmmxxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, GmmxxLinearEquationSolverSettings<ValueType> const& settings) : localA(nullptr), A(nullptr), gmmxxMatrix(nullptr), settings(settings), auxiliaryJacobiMemory(nullptr) {
            this->setMatrix(A);
        }

        template<typename ValueType>
        GmmxxLinearEquationSolver<ValueType>::GmmxxLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A, GmmxxLinearEquationSolverSettings<ValueType> const& settings) : localA(nullptr), A(nullptr), gmmxxMatrix(nullptr), settings(settings), auxiliaryJacobiMemory(nullptr) {
            this->setMatrix(std::move(A));
        }
        
        template<typename ValueType>
        void GmmxxLinearEquationSolver<ValueType>::setMatrix(storm::storage::SparseMatrix<ValueType> const& A) {
            localA.reset();
            this->A = &A;
            gmmxxMatrix = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<ValueType>(A);
        }
        
        template<typename ValueType>
        void GmmxxLinearEquationSolver<ValueType>::setMatrix(storm::storage::SparseMatrix<ValueType>&& A) {
            localA = std::make_unique<storm::storage::SparseMatrix<ValueType>>(std::move(A));
            this->A = localA.get();
            gmmxxMatrix = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<ValueType>(*localA);
        }
        
        template<typename ValueType>
        bool GmmxxLinearEquationSolver<ValueType>::solveEquations(std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            auto method = this->getSettings().getSolutionMethod();
            auto preconditioner = this->getSettings().getPreconditioner();
            STORM_LOG_INFO("Using method '" << method << "' with preconditioner '" << preconditioner << "' (max. " << this->getSettings().getMaximalNumberOfIterations() << " iterations).");
            if (method == GmmxxLinearEquationSolverSettings<ValueType>::SolutionMethod::Jacobi && preconditioner != GmmxxLinearEquationSolverSettings<ValueType>::Preconditioner::None) {
                STORM_LOG_WARN("Jacobi method currently does not support preconditioners. The requested preconditioner will be ignored.");
            }
            
            if (method == GmmxxLinearEquationSolverSettings<ValueType>::SolutionMethod::Bicgstab || method == GmmxxLinearEquationSolverSettings<ValueType>::SolutionMethod::Qmr || method == GmmxxLinearEquationSolverSettings<ValueType>::SolutionMethod::Gmres) {
                // Prepare an iteration object that determines the accuracy and the maximum number of iterations.
                gmm::iteration iter(this->getSettings().getPrecision(), 0, this->getSettings().getMaximalNumberOfIterations());
                
                if (method == GmmxxLinearEquationSolverSettings<ValueType>::SolutionMethod::Bicgstab) {
                    if (preconditioner == GmmxxLinearEquationSolverSettings<ValueType>::Preconditioner::Ilu) {
                        gmm::bicgstab(*gmmxxMatrix, x, b, gmm::ilu_precond<gmm::csr_matrix<ValueType>>(*gmmxxMatrix), iter);
                    } else if (preconditioner == GmmxxLinearEquationSolverSettings<ValueType>::Preconditioner::Diagonal) {
                        gmm::bicgstab(*gmmxxMatrix, x, b, gmm::diagonal_precond<gmm::csr_matrix<ValueType>>(*gmmxxMatrix), iter);
                    } else if (preconditioner == GmmxxLinearEquationSolverSettings<ValueType>::Preconditioner::None) {
                        gmm::bicgstab(*gmmxxMatrix, x, b, gmm::identity_matrix(), iter);
                    }
                } else if (method == GmmxxLinearEquationSolverSettings<ValueType>::SolutionMethod::Qmr) {
                    if (preconditioner == GmmxxLinearEquationSolverSettings<ValueType>::Preconditioner::Ilu) {
                        gmm::qmr(*gmmxxMatrix, x, b, gmm::ilu_precond<gmm::csr_matrix<ValueType>>(*gmmxxMatrix), iter);
                    } else if (preconditioner == GmmxxLinearEquationSolverSettings<ValueType>::Preconditioner::Diagonal) {
                        gmm::qmr(*gmmxxMatrix, x, b, gmm::diagonal_precond<gmm::csr_matrix<ValueType>>(*gmmxxMatrix), iter);
                    } else if (preconditioner == GmmxxLinearEquationSolverSettings<ValueType>::Preconditioner::None) {
                        gmm::qmr(*gmmxxMatrix, x, b, gmm::identity_matrix(), iter);
                    }
                } else if (method == GmmxxLinearEquationSolverSettings<ValueType>::SolutionMethod::Gmres) {
                    if (preconditioner == GmmxxLinearEquationSolverSettings<ValueType>::Preconditioner::Ilu) {
                        gmm::gmres(*gmmxxMatrix, x, b, gmm::ilu_precond<gmm::csr_matrix<ValueType>>(*gmmxxMatrix), this->getSettings().getNumberOfIterationsUntilRestart(), iter);
                    } else if (preconditioner == GmmxxLinearEquationSolverSettings<ValueType>::Preconditioner::Diagonal) {
                        gmm::gmres(*gmmxxMatrix, x, b, gmm::diagonal_precond<gmm::csr_matrix<ValueType>>(*gmmxxMatrix), this->getSettings().getNumberOfIterationsUntilRestart(), iter);
                    } else if (preconditioner == GmmxxLinearEquationSolverSettings<ValueType>::Preconditioner::None) {
                        gmm::gmres(*gmmxxMatrix, x, b, gmm::identity_matrix(), this->getSettings().getNumberOfIterationsUntilRestart(), iter);
                    }
                }
                
                // Check if the solver converged and issue a warning otherwise.
                if (iter.converged()) {
                    STORM_LOG_INFO("Iterative solver converged after " << iter.get_iteration() << " iterations.");
                    return true;
                } else {
                    STORM_LOG_WARN("Iterative solver did not converge.");
                    return false;
                }
            } else if (method == GmmxxLinearEquationSolverSettings<ValueType>::SolutionMethod::Jacobi) {
                uint_fast64_t iterations = solveLinearEquationSystemWithJacobi(*A, x, b);
                
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
            if (b) {
                gmm::mult_add(*gmmxxMatrix, x, *b, result);
            } else {
                gmm::mult(*gmmxxMatrix, x, result);
            }
        }
        
        template<typename ValueType>
        uint_fast64_t GmmxxLinearEquationSolver<ValueType>::solveLinearEquationSystemWithJacobi(storm::storage::SparseMatrix<ValueType> const& A, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            bool allocatedAuxMemory = !this->hasAuxMemory(LinearEquationSolverOperation::SolveEquations);
            if (allocatedAuxMemory) {
                this->allocateAuxMemory(LinearEquationSolverOperation::SolveEquations);
            }
            
            // Get a Jacobi decomposition of the matrix A.
            std::pair<storm::storage::SparseMatrix<ValueType>, std::vector<ValueType>> jacobiDecomposition = A.getJacobiDecomposition();
            
            // Convert the LU matrix to gmm++'s format.
            std::unique_ptr<gmm::csr_matrix<ValueType>> gmmLU = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<ValueType>(std::move(jacobiDecomposition.first));
        
            std::vector<ValueType>* currentX = &x;
            std::vector<ValueType>* nextX = auxiliaryJacobiMemory.get();
            
            // Set up additional environment variables.
            uint_fast64_t iterationCount = 0;
            bool converged = false;
            
            while (!converged && iterationCount < this->getSettings().getMaximalNumberOfIterations() && !(this->hasCustomTerminationCondition() && this->getTerminationCondition().terminateNow(*currentX))) {
                // Compute D^-1 * (b - LU * x) and store result in nextX.
                gmm::mult_add(*gmmLU, gmm::scaled(*currentX, -storm::utility::one<ValueType>()), b, *nextX);
                storm::utility::vector::multiplyVectorsPointwise(jacobiDecomposition.second, *nextX, *nextX);
                
                // Now check if the process already converged within our precision.
                converged = storm::utility::vector::equalModuloPrecision(*currentX, *nextX, this->getSettings().getPrecision(), this->getSettings().getRelativeTerminationCriterion());

                // Swap the two pointers as a preparation for the next iteration.
                std::swap(nextX, currentX);

                // Increase iteration count so we can abort if convergence is too slow.
                ++iterationCount;
            }
            
            // If the last iteration did not write to the original x we have to swap the contents, because the
            // output has to be written to the input parameter x.
            if (currentX == auxiliaryJacobiMemory.get()) {
                std::swap(x, *currentX);
            }
            
            // If we allocated auxiliary memory, we need to dispose of it now.
            if (allocatedAuxMemory) {
                this->deallocateAuxMemory(LinearEquationSolverOperation::SolveEquations);
            }
            
            return iterationCount;
        }
        
        template<typename ValueType>
        GmmxxLinearEquationSolverSettings<ValueType>& GmmxxLinearEquationSolver<ValueType>::getSettings() {
            return settings;
        }

        template<typename ValueType>
        GmmxxLinearEquationSolverSettings<ValueType> const& GmmxxLinearEquationSolver<ValueType>::getSettings() const {
            return settings;
        }
        
        template<typename ValueType>
        bool GmmxxLinearEquationSolver<ValueType>::allocateAuxMemory(LinearEquationSolverOperation operation) const {
            bool result = false;
            if (operation == LinearEquationSolverOperation::SolveEquations) {
                if (this->getSettings().getSolutionMethod() == GmmxxLinearEquationSolverSettings<ValueType>::SolutionMethod::Jacobi) {
                    if (!auxiliaryJacobiMemory) {
                        auxiliaryJacobiMemory = std::make_unique<std::vector<ValueType>>(this->getMatrixRowCount());
                        result = true;
                    }
                }
            }
            result |= LinearEquationSolver<ValueType>::allocateAuxMemory(operation);
            return result;
        }
        
        template<typename ValueType>
        bool GmmxxLinearEquationSolver<ValueType>::deallocateAuxMemory(LinearEquationSolverOperation operation) const {
            bool result = false;
            if (operation == LinearEquationSolverOperation::SolveEquations) {
                if (auxiliaryJacobiMemory) {
                    result = true;
                    auxiliaryJacobiMemory.reset();
                }
            }
            result |= LinearEquationSolver<ValueType>::deallocateAuxMemory(operation);
            return result;
        }
        
        template<typename ValueType>
        bool GmmxxLinearEquationSolver<ValueType>::reallocateAuxMemory(LinearEquationSolverOperation operation) const {
            bool result = false;
            if (operation == LinearEquationSolverOperation::SolveEquations) {
                if (auxiliaryJacobiMemory) {
                    result = auxiliaryJacobiMemory->size() != this->getMatrixColumnCount();
                    auxiliaryJacobiMemory->resize(this->getMatrixRowCount());
                }
            }
            result |= LinearEquationSolver<ValueType>::reallocateAuxMemory(operation);
            return result;
        }
        
        template<typename ValueType>
        bool GmmxxLinearEquationSolver<ValueType>::hasAuxMemory(LinearEquationSolverOperation operation) const {
            bool result = false;
            if (operation == LinearEquationSolverOperation::SolveEquations) {
                result |= static_cast<bool>(auxiliaryJacobiMemory);
            }
            result |= LinearEquationSolver<ValueType>::hasAuxMemory(operation);
            return result;
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
