#include "GmmxxLinearEquationSolver.h"

#include <cmath>
#include <utility>

#include "storm/adapters/GmmxxAdapter.h"

#include "storm/environment/solver/GmmxxSolverEnvironment.h"

#include "storm/utility/vector.h"
#include "storm/utility/constants.h"

#include "storm/utility/gmm.h"
#include "storm/utility/vector.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        GmmxxLinearEquationSolver<ValueType>::GmmxxLinearEquationSolver() {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        GmmxxLinearEquationSolver<ValueType>::GmmxxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A) {
            this->setMatrix(A);
        }

        template<typename ValueType>
        GmmxxLinearEquationSolver<ValueType>::GmmxxLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A) {
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
        GmmxxLinearEquationSolverMethod GmmxxLinearEquationSolver<ValueType>::getMethod(Environment const& env) const {
            STORM_LOG_ERROR_COND(!env.solver().isForceSoundness(), "This linear equation solver does not support sound computations. Using unsound methods now...");
            return env.solver().gmmxx().getMethod();
        }
        
        template<typename ValueType>
        bool GmmxxLinearEquationSolver<ValueType>::internalSolveEquations(Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            auto method = getMethod(env);
            auto preconditioner = env.solver().gmmxx().getPreconditioner();
            
            STORM_LOG_INFO("Solving linear equation system (" << x.size() << " rows) with Gmmxx linear equation solver with method '" << toString(method) << "' and preconditioner '" << toString(preconditioner) << "'.");
            
            if (method == GmmxxLinearEquationSolverMethod::Bicgstab || method == GmmxxLinearEquationSolverMethod::Qmr || method == GmmxxLinearEquationSolverMethod::Gmres) {
                // Make sure that the requested preconditioner is available
                if (preconditioner == GmmxxLinearEquationSolverPreconditioner::Ilu && !iluPreconditioner) {
                    iluPreconditioner = std::make_unique<gmm::ilu_precond<gmm::csr_matrix<ValueType>>>(*gmmxxA);
                } else if (preconditioner == GmmxxLinearEquationSolverPreconditioner::Diagonal) {
                    diagonalPreconditioner = std::make_unique<gmm::diagonal_precond<gmm::csr_matrix<ValueType>>>(*gmmxxA);
                }
                
                // Prepare an iteration object that determines the accuracy and the maximum number of iterations.
                gmm::iteration iter(storm::utility::convertNumber<ValueType>(env.solver().gmmxx().getPrecision()), 0, env.solver().gmmxx().getMaximalNumberOfIterations());
                
                // Invoke gmm with the corresponding settings
                if (method == GmmxxLinearEquationSolverMethod::Bicgstab) {
                    if (preconditioner == GmmxxLinearEquationSolverPreconditioner::Ilu) {
                        gmm::bicgstab(*gmmxxA, x, b, *iluPreconditioner, iter);
                    } else if (preconditioner == GmmxxLinearEquationSolverPreconditioner::Diagonal) {
                        gmm::bicgstab(*gmmxxA, x, b, *diagonalPreconditioner, iter);
                    } else if (preconditioner == GmmxxLinearEquationSolverPreconditioner::None) {
                        gmm::bicgstab(*gmmxxA, x, b, gmm::identity_matrix(), iter);
                    }
                } else if (method == GmmxxLinearEquationSolverMethod::Qmr) {
                    if (preconditioner == GmmxxLinearEquationSolverPreconditioner::Ilu) {
                        gmm::qmr(*gmmxxA, x, b, *iluPreconditioner, iter);
                    } else if (preconditioner == GmmxxLinearEquationSolverPreconditioner::Diagonal) {
                        gmm::qmr(*gmmxxA, x, b, *diagonalPreconditioner, iter);
                    } else if (preconditioner == GmmxxLinearEquationSolverPreconditioner::None) {
                        gmm::qmr(*gmmxxA, x, b, gmm::identity_matrix(), iter);
                    }
                } else if (method == GmmxxLinearEquationSolverMethod::Gmres) {
                    if (preconditioner == GmmxxLinearEquationSolverPreconditioner::Ilu) {
                        gmm::gmres(*gmmxxA, x, b, *iluPreconditioner, env.solver().gmmxx().getRestartThreshold(), iter);
                    } else if (preconditioner == GmmxxLinearEquationSolverPreconditioner::Diagonal) {
                        gmm::gmres(*gmmxxA, x, b, *diagonalPreconditioner, env.solver().gmmxx().getRestartThreshold(), iter);
                    } else if (preconditioner == GmmxxLinearEquationSolverPreconditioner::None) {
                        gmm::gmres(*gmmxxA, x, b, gmm::identity_matrix(), env.solver().gmmxx().getRestartThreshold(), iter);
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
                    STORM_LOG_WARN("Iterative solver did not converge after " << iter.get_iteration() << " iterations.");
                    return false;
                }
            }
            
            STORM_LOG_ERROR("Selected method is not available");
            return false;
        }
        
        template<typename ValueType>
        LinearEquationSolverProblemFormat GmmxxLinearEquationSolver<ValueType>::getEquationProblemFormat(Environment const& env) const {
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
        std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> GmmxxLinearEquationSolverFactory<ValueType>::create(Environment const& env) const {
            return std::make_unique<storm::solver::GmmxxLinearEquationSolver<ValueType>>();
        }
        
        template<typename ValueType>
        std::unique_ptr<LinearEquationSolverFactory<ValueType>> GmmxxLinearEquationSolverFactory<ValueType>::clone() const {
            return std::make_unique<GmmxxLinearEquationSolverFactory<ValueType>>(*this);
        }
        
        // Explicitly instantiate the solver.
        template class GmmxxLinearEquationSolver<double>;
        template class GmmxxLinearEquationSolverFactory<double>;
        
    }
}
