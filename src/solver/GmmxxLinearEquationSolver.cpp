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
        GmmxxLinearEquationSolver<ValueType>::GmmxxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, GmmxxLinearEquationSolverSettings<ValueType> const& settings) : localA(nullptr), A(A), gmmxxMatrix(storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<ValueType>(A)), settings(settings) {
            // Intentionally left empty.
        }

        template<typename ValueType>
        GmmxxLinearEquationSolver<ValueType>::GmmxxLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A, GmmxxLinearEquationSolverSettings<ValueType> const& settings) : localA(std::make_unique<storm::storage::SparseMatrix<ValueType>>(std::move(A))), A(*localA), gmmxxMatrix(storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<ValueType>(*localA)), settings(settings) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        void GmmxxLinearEquationSolver<ValueType>::solveEquationSystem(std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult) const {
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
                } else {
                    STORM_LOG_WARN("Iterative solver did not converge.");
                }
            } else if (method == GmmxxLinearEquationSolverSettings<ValueType>::SolutionMethod::Jacobi) {
                uint_fast64_t iterations = solveLinearEquationSystemWithJacobi(A, x, b, multiplyResult);
                
                // Check if the solver converged and issue a warning otherwise.
                if (iterations < this->getSettings().getMaximalNumberOfIterations()) {
                    STORM_LOG_INFO("Iterative solver converged after " << iterations << " iterations.");
                } else {
                    STORM_LOG_WARN("Iterative solver did not converge.");
                }
            }
        }
        
        template<typename ValueType>
        void GmmxxLinearEquationSolver<ValueType>::performMatrixVectorMultiplication(std::vector<ValueType>& x, std::vector<ValueType> const* b, uint_fast64_t n, std::vector<ValueType>* multiplyResult) const {
            // Set up some temporary variables so that we can just swap pointers instead of copying the result after
            // each iteration.
            std::vector<ValueType>* currentX = &x;

            bool multiplyResultProvided = true;
            std::vector<ValueType>* nextX = multiplyResult;
            if (nextX == nullptr) {
                nextX = new std::vector<ValueType>(x.size());
                multiplyResultProvided = false;
            }
            std::vector<ValueType> const* copyX = nextX;
            
            // Now perform matrix-vector multiplication as long as we meet the bound.
            for (uint_fast64_t i = 0; i < n; ++i) {
                gmm::mult(*gmmxxMatrix, *currentX, *nextX);
                std::swap(nextX, currentX);
                
                // If requested, add an offset to the current result vector.
                if (b != nullptr) {
                    gmm::add(*b, *currentX);
                }
            }
            
            // If we performed an odd number of repetitions, we need to swap the contents of currentVector and x,
            // because the output is supposed to be stored in the input vector x.
            if (currentX == copyX) {
                std::swap(x, *currentX);
            }
            
            // If the vector for the temporary multiplication result was not provided, we need to delete it.
            if (!multiplyResultProvided) {
                delete copyX;
            }
        }
        
        template<typename ValueType>
        uint_fast64_t GmmxxLinearEquationSolver<ValueType>::solveLinearEquationSystemWithJacobi(storm::storage::SparseMatrix<ValueType> const& A, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult) const {
            // Get a Jacobi decomposition of the matrix A.
            std::pair<storm::storage::SparseMatrix<ValueType>, std::vector<ValueType>> jacobiDecomposition = A.getJacobiDecomposition();
            
            // Convert the LU matrix to gmm++'s format.
            std::unique_ptr<gmm::csr_matrix<ValueType>> gmmLU = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<ValueType>(std::move(jacobiDecomposition.first));
        
            // To avoid copying the contents of the vector in the loop, we create a temporary x to swap with.
            bool multiplyResultProvided = true;
            std::vector<ValueType>* nextX = multiplyResult;
            if (nextX == nullptr) {
                nextX = new std::vector<ValueType>(x.size());
                multiplyResultProvided = false;
            }
            std::vector<ValueType> const* copyX = nextX;
            std::vector<ValueType>* currentX = &x;
            
            // Target vector for precision calculation.
            std::vector<ValueType> tmpX(x.size());
            
            // Set up additional environment variables.
            uint_fast64_t iterationCount = 0;
            bool converged = false;
            
            while (!converged && iterationCount < this->getSettings().getMaximalNumberOfIterations() && !(this->hasCustomTerminationCondition() && this->getTerminationCondition().terminateNow(*currentX))) {
                // Compute D^-1 * (b - LU * x) and store result in nextX.
                gmm::mult(*gmmLU, *currentX, tmpX);
                gmm::add(b, gmm::scaled(tmpX, -storm::utility::one<ValueType>()), tmpX);
                storm::utility::vector::multiplyVectorsPointwise(jacobiDecomposition.second, tmpX, *nextX);
                
                // Now check if the process already converged within our precision.
                converged = storm::utility::vector::equalModuloPrecision(*currentX, *nextX, this->getSettings().getPrecision(), this->getSettings().getRelativeTerminationCriterion());

                // Swap the two pointers as a preparation for the next iteration.
                std::swap(nextX, currentX);

                // Increase iteration count so we can abort if convergence is too slow.
                ++iterationCount;
            }
            
            // If the last iteration did not write to the original x we have to swap the contents, because the
            // output has to be written to the input parameter x.
            if (currentX == copyX) {
                std::swap(x, *currentX);
            }
            
            // If the vector for the temporary multiplication result was not provided, we need to delete it.
            if (!multiplyResultProvided) {
                delete copyX;
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
        std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> GmmxxLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType> const& matrix) const {
            return std::make_unique<storm::solver::GmmxxLinearEquationSolver<ValueType>>(matrix, settings);
        }
        
        template<typename ValueType>
        std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> GmmxxLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType>&& matrix) const {
            return std::make_unique<storm::solver::GmmxxLinearEquationSolver<ValueType>>(std::move(matrix), settings);
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