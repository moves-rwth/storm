#include "storm/solver/EigenLinearEquationSolver.h"

#include "storm/adapters/EigenAdapter.h"

#include "storm/environment/solver/EigenSolverEnvironment.h"

#include "storm/utility/vector.h"
#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidSettingsException.h"

namespace storm {
    namespace solver {

        template<typename ValueType>
        EigenLinearEquationSolver<ValueType>::EigenLinearEquationSolver() {
            // Intentionally left empty.
        }

        template<typename ValueType>
        EigenLinearEquationSolver<ValueType>::EigenLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A) {
            this->setMatrix(A);
        }

        template<typename ValueType>
        EigenLinearEquationSolver<ValueType>::EigenLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A) {
            this->setMatrix(std::move(A));
        }
        
        template<typename ValueType>
        void EigenLinearEquationSolver<ValueType>::setMatrix(storm::storage::SparseMatrix<ValueType> const& A) {
            eigenA = storm::adapters::EigenAdapter::toEigenSparseMatrix<ValueType>(A);
            this->clearCache();
        }
        
        template<typename ValueType>
        void EigenLinearEquationSolver<ValueType>::setMatrix(storm::storage::SparseMatrix<ValueType>&& A) {
            // Take ownership of the matrix so it is destroyed after we have translated it to Eigen's format.
            storm::storage::SparseMatrix<ValueType> localA(std::move(A));
            this->setMatrix(localA);
            this->clearCache();
        }
        
        template<typename ValueType>
        EigenLinearEquationSolverMethod EigenLinearEquationSolver<ValueType>::getMethod(Environment const& env, bool isExactMode) const {
            // Adjust the method if none was specified and we are using rational numbers.
            auto method = env.solver().eigen().getMethod();
            
            if (isExactMode && method != EigenLinearEquationSolverMethod::SparseLU) {
                if (env.solver().eigen().isMethodSetFromDefault()) {
                    STORM_LOG_INFO("Selecting 'SparseLU' as the solution technique to guarantee exact results.");
                } else {
                    STORM_LOG_WARN("The selected solution method does not guarantee exact results. Falling back to SparseLU.");
                }
                method = EigenLinearEquationSolverMethod::SparseLU;
            } else if (env.solver().isForceSoundness() && method != EigenLinearEquationSolverMethod::SparseLU) {
                if (env.solver().eigen().isMethodSetFromDefault()) {
                    STORM_LOG_INFO("Selecting 'SparseLU' as the solution technique to guarantee sound results. If you want to override this, please explicitly specify a different method.");
                    method = EigenLinearEquationSolverMethod::SparseLU;
                } else {
                    STORM_LOG_WARN("The selected solution method does not guarantee sound results.");
                }
            }
            return method;
        }
        
        #ifdef STORM_HAVE_CARL
        // Specialization for storm::RationalNumber
        template<>
        bool EigenLinearEquationSolver<storm::RationalNumber>::internalSolveEquations(Environment const& env, std::vector<storm::RationalNumber>& x, std::vector<storm::RationalNumber> const& b) const {
            auto solutionMethod = getMethod(env, true);
            STORM_LOG_WARN_COND(solutionMethod == EigenLinearEquationSolverMethod::SparseLU, "Switching method to SparseLU.");
            STORM_LOG_INFO("Solving linear equation system (" << x.size() << " rows) with with rational numbers using LU factorization (Eigen library).");
            
            // Map the input vectors to Eigen's format.
            auto eigenX = StormEigen::Matrix<storm::RationalNumber, StormEigen::Dynamic, 1>::Map(x.data(), x.size());
            auto eigenB = StormEigen::Matrix<storm::RationalNumber, StormEigen::Dynamic, 1>::Map(b.data(), b.size());
            
            StormEigen::SparseLU<StormEigen::SparseMatrix<storm::RationalNumber>, StormEigen::COLAMDOrdering<int>> solver;
            solver.compute(*eigenA);
            solver._solve_impl(eigenB, eigenX);
            
            return solver.info() == StormEigen::ComputationInfo::Success;
        }
        
        // Specialization for storm::RationalFunction
        template<>
        bool EigenLinearEquationSolver<storm::RationalFunction>::internalSolveEquations(Environment const& env, std::vector<storm::RationalFunction>& x, std::vector<storm::RationalFunction> const& b) const {
            auto solutionMethod = getMethod(env, true);
            STORM_LOG_WARN_COND(solutionMethod == EigenLinearEquationSolverMethod::SparseLU, "Switching method to SparseLU.");
            STORM_LOG_INFO("Solving linear equation system (" << x.size() << " rows) with rational functions using LU factorization (Eigen library).");

            // Map the input vectors to Eigen's format.
            auto eigenX = StormEigen::Matrix<storm::RationalFunction, StormEigen::Dynamic, 1>::Map(x.data(), x.size());
            auto eigenB = StormEigen::Matrix<storm::RationalFunction, StormEigen::Dynamic, 1>::Map(b.data(), b.size());
            
            StormEigen::SparseLU<StormEigen::SparseMatrix<storm::RationalFunction>, StormEigen::COLAMDOrdering<int>> solver;
            solver.compute(*eigenA);
            solver._solve_impl(eigenB, eigenX);
            return solver.info() == StormEigen::ComputationInfo::Success;
        }
#endif

        
        template<typename ValueType>
        bool EigenLinearEquationSolver<ValueType>::internalSolveEquations(Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            // Map the input vectors to Eigen's format.
            auto eigenX = StormEigen::Matrix<ValueType, StormEigen::Dynamic, 1>::Map(x.data(), x.size());
            auto eigenB = StormEigen::Matrix<ValueType, StormEigen::Dynamic, 1>::Map(b.data(), b.size());

            auto solutionMethod = getMethod(env, false);
            if (solutionMethod == EigenLinearEquationSolverMethod::SparseLU) {
                STORM_LOG_INFO("Solving linear equation system (" << x.size() << " rows) with sparse LU factorization (Eigen library).");
                StormEigen::SparseLU<StormEigen::SparseMatrix<ValueType>, StormEigen::COLAMDOrdering<int>> solver;
                solver.compute(*this->eigenA);
                solver._solve_impl(eigenB, eigenX);
            } else {
                bool converged = false;
                uint64_t numberOfIterations = 0;
                uint64_t maxIter = env.solver().eigen().getMaximalNumberOfIterations();
                uint64_t restartThreshold = env.solver().eigen().getRestartThreshold();
                ValueType precision = storm::utility::convertNumber<ValueType>(env.solver().eigen().getPrecision());
                EigenLinearEquationSolverPreconditioner preconditioner = env.solver().eigen().getPreconditioner();
                if (solutionMethod == EigenLinearEquationSolverMethod::Bicgstab) {
                    if (preconditioner == EigenLinearEquationSolverPreconditioner::Ilu) {
                        STORM_LOG_INFO("Solving linear equation system (" << x.size() << " rows) with BiCGSTAB with Ilu preconditioner (Eigen library).");

                        StormEigen::BiCGSTAB<StormEigen::SparseMatrix<ValueType>, StormEigen::IncompleteLUT<ValueType>> solver;
                        solver.compute(*this->eigenA);
                        solver.setTolerance(precision);
                        solver.setMaxIterations(maxIter);
                        eigenX = solver.solveWithGuess(eigenB, eigenX);
                        converged = solver.info() == StormEigen::ComputationInfo::Success;
                        numberOfIterations = solver.iterations();
                    } else if (preconditioner == EigenLinearEquationSolverPreconditioner::Diagonal) {
                        STORM_LOG_INFO("Solving linear equation system (" << x.size() << " rows) with BiCGSTAB with Diagonal preconditioner (Eigen library).");

                        StormEigen::BiCGSTAB<StormEigen::SparseMatrix<ValueType>, StormEigen::DiagonalPreconditioner<ValueType>> solver;
                        solver.setTolerance(precision);
                        solver.setMaxIterations(maxIter);
                        solver.compute(*this->eigenA);
                        eigenX = solver.solveWithGuess(eigenB, eigenX);
                        converged = solver.info() == StormEigen::ComputationInfo::Success;
                        numberOfIterations = solver.iterations();
                    } else {
                        STORM_LOG_INFO("Solving linear equation system (" << x.size() << " rows) with BiCGSTAB with identity preconditioner (Eigen library).");

                        StormEigen::BiCGSTAB<StormEigen::SparseMatrix<ValueType>, StormEigen::IdentityPreconditioner> solver;
                        solver.setTolerance(precision);
                        solver.setMaxIterations(maxIter);
                        solver.compute(*this->eigenA);
                        eigenX = solver.solveWithGuess(eigenB, eigenX);
                        numberOfIterations = solver.iterations();
                        converged = solver.info() == StormEigen::ComputationInfo::Success;
                    }
                } else if (solutionMethod == EigenLinearEquationSolverMethod::DGmres) {
                    if (preconditioner == EigenLinearEquationSolverPreconditioner::Ilu) {
                        STORM_LOG_INFO("Solving linear equation system (" << x.size() << " rows) with DGMRES with Ilu preconditioner (Eigen library).");

                        StormEigen::DGMRES<StormEigen::SparseMatrix<ValueType>, StormEigen::IncompleteLUT<ValueType>> solver;
                        solver.setTolerance(precision);
                        solver.setMaxIterations(maxIter);
                        solver.set_restart(restartThreshold);
                        solver.compute(*this->eigenA);
                        eigenX = solver.solveWithGuess(eigenB, eigenX);
                        converged = solver.info() == StormEigen::ComputationInfo::Success;
                        numberOfIterations = solver.iterations();
                    } else if (preconditioner == EigenLinearEquationSolverPreconditioner::Diagonal) {
                        STORM_LOG_INFO("Solving linear equation system (" << x.size() << " rows) with DGMRES with Diagonal preconditioner (Eigen library).");

                        StormEigen::DGMRES<StormEigen::SparseMatrix<ValueType>, StormEigen::DiagonalPreconditioner<ValueType>> solver;
                        solver.setTolerance(precision);
                        solver.setMaxIterations(maxIter);
                        solver.set_restart(restartThreshold);
                        solver.compute(*this->eigenA);
                        eigenX = solver.solveWithGuess(eigenB, eigenX);
                        converged = solver.info() == StormEigen::ComputationInfo::Success;
                        numberOfIterations = solver.iterations();
                    } else {
                        STORM_LOG_INFO("Solving linear equation system (" << x.size() << " rows) with DGMRES with identity preconditioner (Eigen library).");

                        StormEigen::DGMRES<StormEigen::SparseMatrix<ValueType>, StormEigen::IdentityPreconditioner> solver;
                        solver.setTolerance(precision);
                        solver.setMaxIterations(maxIter);
                        solver.set_restart(restartThreshold);
                        solver.compute(*this->eigenA);
                        eigenX = solver.solveWithGuess(eigenB, eigenX);
                        converged = solver.info() == StormEigen::ComputationInfo::Success;
                        numberOfIterations = solver.iterations();
                    }
                } else if (solutionMethod == EigenLinearEquationSolverMethod::Gmres) {
                    if (preconditioner == EigenLinearEquationSolverPreconditioner::Ilu) {
                        STORM_LOG_INFO("Solving linear equation system (" << x.size() << " rows) with GMRES with Ilu preconditioner (Eigen library).");

                        StormEigen::GMRES<StormEigen::SparseMatrix<ValueType>, StormEigen::IncompleteLUT<ValueType>> solver;
                        solver.setTolerance(precision);
                        solver.setMaxIterations(maxIter);
                        solver.set_restart(restartThreshold);
                        solver.compute(*this->eigenA);
                        eigenX = solver.solveWithGuess(eigenB, eigenX);
                        converged = solver.info() == StormEigen::ComputationInfo::Success;
                        numberOfIterations = solver.iterations();
                    } else if (preconditioner == EigenLinearEquationSolverPreconditioner::Diagonal) {
                        STORM_LOG_INFO("Solving linear equation system (" << x.size() << " rows) with GMRES with Diagonal preconditioner (Eigen library).");

                        StormEigen::GMRES<StormEigen::SparseMatrix<ValueType>, StormEigen::DiagonalPreconditioner<ValueType>> solver;
                        solver.setTolerance(precision);
                        solver.setMaxIterations(maxIter);
                        solver.set_restart(restartThreshold);
                        solver.compute(*this->eigenA);
                        eigenX = solver.solveWithGuess(eigenB, eigenX);
                        converged = solver.info() == StormEigen::ComputationInfo::Success;
                        numberOfIterations = solver.iterations();
                    } else {
                        STORM_LOG_INFO("Solving linear equation system (" << x.size() << " rows) with GMRES with identity preconditioner (Eigen library).");

                        StormEigen::GMRES<StormEigen::SparseMatrix<ValueType>, StormEigen::IdentityPreconditioner> solver;
                        solver.setTolerance(precision);
                        solver.setMaxIterations(maxIter);
                        solver.set_restart(restartThreshold);
                        solver.compute(*this->eigenA);
                        eigenX = solver.solveWithGuess(eigenB, eigenX);
                        converged = solver.info() == StormEigen::ComputationInfo::Success;
                        numberOfIterations = solver.iterations();
                    }
                }
                
                // Make sure that all results conform to the (global) bounds.
                storm::utility::vector::clip(x, this->lowerBound, this->upperBound);
                
                // Check if the solver converged and issue a warning otherwise.
                if (converged) {
                    STORM_LOG_INFO("Iterative solver converged after " << numberOfIterations << " iterations.");
                    return true;
                } else {
                    STORM_LOG_WARN("Iterative solver did not converge.");
                    return false;
                }
            }
            
            return true;
        }
        
        template<typename ValueType>
        void EigenLinearEquationSolver<ValueType>::multiply(std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<ValueType>& result) const {
            // Typedef the map-type so we don't have to spell it out.
            typedef decltype(StormEigen::Matrix<ValueType, StormEigen::Dynamic, 1>::Map(b->data(), b->size())) MapType;

            auto eigenX = StormEigen::Matrix<ValueType, StormEigen::Dynamic, 1>::Map(x.data(), x.size());
            auto eigenResult = StormEigen::Matrix<ValueType, StormEigen::Dynamic, 1>::Map(result.data(), result.size());

            std::unique_ptr<MapType> eigenB;
            if (b != nullptr) {
                eigenB = std::make_unique<MapType>(StormEigen::Matrix<ValueType, StormEigen::Dynamic, 1>::Map(b->data(), b->size()));
            }
            
            if (&x != &result) {
                if (b != nullptr) {
                    eigenResult.noalias() = *eigenA * eigenX + *eigenB;
                } else {
                    eigenResult.noalias() = *eigenA * eigenX;
                }
            } else {
                if (b != nullptr) {
                    eigenResult = *eigenA * eigenX + *eigenB;
                } else {
                    eigenResult = *eigenA * eigenX;
                }
            }
        }
        
        template<typename ValueType>
        LinearEquationSolverProblemFormat EigenLinearEquationSolver<ValueType>::getEquationProblemFormat(Environment const& env) const {
            return LinearEquationSolverProblemFormat::EquationSystem;
        }
        
        template<typename ValueType>
        uint64_t EigenLinearEquationSolver<ValueType>::getMatrixRowCount() const {
            return eigenA->rows();
        }
        
        template<typename ValueType>
        uint64_t EigenLinearEquationSolver<ValueType>::getMatrixColumnCount() const {
            return eigenA->cols();
        }
        
        template<typename ValueType>
        std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> EigenLinearEquationSolverFactory<ValueType>::create(Environment const& env) const {
            return std::make_unique<storm::solver::EigenLinearEquationSolver<ValueType>>();
        }
        
        template<typename ValueType>
        std::unique_ptr<LinearEquationSolverFactory<ValueType>> EigenLinearEquationSolverFactory<ValueType>::clone() const {
            return std::make_unique<EigenLinearEquationSolverFactory<ValueType>>(*this);
        }
        
        template class EigenLinearEquationSolver<double>;
        template class EigenLinearEquationSolverFactory<double>;

#ifdef STORM_HAVE_CARL
        template class EigenLinearEquationSolver<storm::RationalNumber>;
        template class EigenLinearEquationSolver<storm::RationalFunction>;
        
        template class EigenLinearEquationSolverFactory<storm::RationalNumber>;
        template class EigenLinearEquationSolverFactory<storm::RationalFunction>;
#endif
    }
}
