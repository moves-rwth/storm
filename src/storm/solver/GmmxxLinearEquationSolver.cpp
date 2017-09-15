#include "GmmxxLinearEquationSolver.h"

#include <cmath>
#include <utility>

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/modules/GmmxxEquationSolverSettings.h"

#include "storm/adapters/GmmxxAdapter.h"

#include "storm/solver/GmmxxMultiplier.h"
#include "storm/solver/NativeLinearEquationSolver.h"

#include "storm/utility/vector.h"
#include "storm/utility/constants.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/InvalidSolverSettingsException.h"

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
            restart = settings.getRestartIterationCount();
            
            // Determine the method to be used.
            storm::settings::modules::GmmxxEquationSolverSettings::LinearEquationMethod methodAsSetting = settings.getLinearEquationSystemMethod();
            if (methodAsSetting == storm::settings::modules::GmmxxEquationSolverSettings::LinearEquationMethod::Bicgstab) {
                method = SolutionMethod::Bicgstab;
            } else if (methodAsSetting == storm::settings::modules::GmmxxEquationSolverSettings::LinearEquationMethod::Qmr) {
                method = SolutionMethod::Qmr;
            } else if (methodAsSetting == storm::settings::modules::GmmxxEquationSolverSettings::LinearEquationMethod::Gmres) {
                method = SolutionMethod::Gmres;
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
            
            // Finally force soundness and potentially overwrite some other settings.
            this->setForceSoundness(storm::settings::getModule<storm::settings::modules::GeneralSettings>().isSoundSet());
        }
        
        template<typename ValueType>
        void GmmxxLinearEquationSolverSettings<ValueType>::setSolutionMethod(SolutionMethod const& method) {
            this->method = method;
            
            // Make sure we switch the method if we have to guarantee soundness.
            this->setForceSoundness(forceSoundness);
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
        void GmmxxLinearEquationSolverSettings<ValueType>::setNumberOfIterationsUntilRestart(uint64_t restart) {
            this->restart = restart;
        }
        
        template<typename ValueType>
        void GmmxxLinearEquationSolverSettings<ValueType>::setForceSoundness(bool value) {
            STORM_LOG_THROW(!value, storm::exceptions::InvalidSolverSettingsException, "Solver cannot guarantee soundness, please choose a different equation solver.");
            forceSoundness = value;
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
        uint64_t GmmxxLinearEquationSolverSettings<ValueType>::getNumberOfIterationsUntilRestart() const {
            return restart;
        }
        
        template<typename ValueType>
        bool GmmxxLinearEquationSolverSettings<ValueType>::getForceSoundness() const {
            return forceSoundness;
        }
        
        template<typename ValueType>
        GmmxxLinearEquationSolver<ValueType>::GmmxxLinearEquationSolver(GmmxxLinearEquationSolverSettings<ValueType> const& settings) : settings(settings) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        GmmxxLinearEquationSolver<ValueType>::GmmxxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, GmmxxLinearEquationSolverSettings<ValueType> const& settings) : settings(settings) {
            this->setMatrix(A);
        }

        template<typename ValueType>
        GmmxxLinearEquationSolver<ValueType>::GmmxxLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A, GmmxxLinearEquationSolverSettings<ValueType> const& settings) : settings(settings) {
            this->setMatrix(std::move(A));
        }
        
        template<typename ValueType>
        void GmmxxLinearEquationSolver<ValueType>::setMatrix(storm::storage::SparseMatrix<ValueType> const& A) {
            gmmxxA = storm::adapters::GmmxxAdapter<ValueType>::toGmmxxSparseMatrix(A);
            clearCache();
        }
        
        template<typename ValueType>
        void GmmxxLinearEquationSolver<ValueType>::setMatrix(storm::storage::SparseMatrix<ValueType>&& A) {
            gmmxxA = storm::adapters::GmmxxAdapter<ValueType>::toGmmxxSparseMatrix(A);
            clearCache();
        }
        
        template<typename ValueType>
        bool GmmxxLinearEquationSolver<ValueType>::internalSolveEquations(std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            auto method = this->getSettings().getSolutionMethod();
            auto preconditioner = this->getSettings().getPreconditioner();
            STORM_LOG_INFO("Solving linear equation system (" << x.size() << " rows) with Gmmxx linear equation solver with method '" << method << "' and preconditioner '" << preconditioner << "' (max. " << this->getSettings().getMaximalNumberOfIterations() << " iterations).");
            
            if (method == GmmxxLinearEquationSolverSettings<ValueType>::SolutionMethod::Bicgstab || method == GmmxxLinearEquationSolverSettings<ValueType>::SolutionMethod::Qmr || method == GmmxxLinearEquationSolverSettings<ValueType>::SolutionMethod::Gmres) {
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
            }
            
            STORM_LOG_ERROR("Selected method is not available");
            return false;
        }
        
        template<typename ValueType>
        void GmmxxLinearEquationSolver<ValueType>::multiply(std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<ValueType>& result) const {
            multiplier.multAdd(*gmmxxA, x, b, result);
            
            if (!this->isCachingEnabled()) {
                clearCache();
            }
        }
        
        template<typename ValueType>
        void GmmxxLinearEquationSolver<ValueType>::multiplyAndReduce(OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<ValueType>& result, std::vector<uint_fast64_t>* choices) const {
            multiplier.multAddReduce(dir, rowGroupIndices, *gmmxxA, x, b, result, choices);
        }
        
        template<typename ValueType>
        bool GmmxxLinearEquationSolver<ValueType>::supportsGaussSeidelMultiplication() const {
            return true;
        }
        
        template<typename ValueType>
        void GmmxxLinearEquationSolver<ValueType>::multiplyGaussSeidel(std::vector<ValueType>& x, std::vector<ValueType> const* b) const {
            multiplier.multAddGaussSeidelBackward(*gmmxxA, x, b);
        }

        template<typename ValueType>
        void GmmxxLinearEquationSolver<ValueType>::multiplyAndReduceGaussSeidel(OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<uint64_t>* choices) const {
            multiplier.multAddReduceGaussSeidel(dir, rowGroupIndices, *gmmxxA, x, b, choices);
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
        LinearEquationSolverProblemFormat GmmxxLinearEquationSolver<ValueType>::getEquationProblemFormat() const {
            return LinearEquationSolverProblemFormat::EquationSystem;
        }
        
        template<typename ValueType>
        void GmmxxLinearEquationSolver<ValueType>::clearCache() const {
            iluPreconditioner.reset();
            diagonalPreconditioner.reset();
            LinearEquationSolver<ValueType>::clearCache();
        }
        
        template<typename ValueType>
        uint64_t GmmxxLinearEquationSolver<ValueType>::getMatrixRowCount() const {
            return gmmxxA->nr;
        }
        
        template<typename ValueType>
        uint64_t GmmxxLinearEquationSolver<ValueType>::getMatrixColumnCount() const {
            return gmmxxA->nc;
        }
        
        template<typename ValueType>
        std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> GmmxxLinearEquationSolverFactory<ValueType>::create() const {
            return std::make_unique<storm::solver::GmmxxLinearEquationSolver<ValueType>>(settings);
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
