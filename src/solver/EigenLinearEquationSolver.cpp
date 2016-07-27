#include "src/solver/EigenLinearEquationSolver.h"

#include "src/adapters/EigenAdapter.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/modules/EigenEquationSolverSettings.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidSettingsException.h"

namespace storm {
    namespace solver {
     
        template<typename ValueType>
        EigenLinearEquationSolverSettings<ValueType>::EigenLinearEquationSolverSettings() {
            // Get the settings object to customize linear solving.
            storm::settings::modules::EigenEquationSolverSettings const& settings = storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>();
            
            // Get appropriate settings.
            maximalNumberOfIterations = settings.getMaximalIterationCount();
            precision = settings.getPrecision();
            restart = settings.getRestartIterationCount();
            
            // Determine the method to be used.
            storm::settings::modules::EigenEquationSolverSettings::LinearEquationMethod methodAsSetting = settings.getLinearEquationSystemMethod();
            if (methodAsSetting == storm::settings::modules::EigenEquationSolverSettings::LinearEquationMethod::BiCGSTAB) {
                method = SolutionMethod::BiCGSTAB;
            } else if (methodAsSetting == storm::settings::modules::EigenEquationSolverSettings::LinearEquationMethod::SparseLU) {
                method = SolutionMethod::SparseLU;
            } else if (methodAsSetting == storm::settings::modules::EigenEquationSolverSettings::LinearEquationMethod::DGMRES) {
                method = SolutionMethod::DGMRES;
            } else if (methodAsSetting == storm::settings::modules::EigenEquationSolverSettings::LinearEquationMethod::GMRES) {
                method = SolutionMethod::GMRES;
            }
            
            // Check which preconditioner to use.
            storm::settings::modules::EigenEquationSolverSettings::PreconditioningMethod preconditionAsSetting = settings.getPreconditioningMethod();
            if (preconditionAsSetting == storm::settings::modules::EigenEquationSolverSettings::PreconditioningMethod::Ilu) {
                preconditioner = Preconditioner::Ilu;
            } else if (preconditionAsSetting == storm::settings::modules::EigenEquationSolverSettings::PreconditioningMethod::Diagonal) {
                preconditioner = Preconditioner::Diagonal;
            } else if (preconditionAsSetting == storm::settings::modules::EigenEquationSolverSettings::PreconditioningMethod::None) {
                preconditioner = Preconditioner::None;
            }
        }
        
        template<typename ValueType>
        void EigenLinearEquationSolverSettings<ValueType>::setSolutionMethod(SolutionMethod const& method) {
            this->method = method;
        }
        
        template<typename ValueType>
        void EigenLinearEquationSolverSettings<ValueType>::setPreconditioner(Preconditioner const& preconditioner) {
            this->preconditioner = preconditioner;
        }
        
        template<typename ValueType>
        void EigenLinearEquationSolverSettings<ValueType>::setPrecision(ValueType precision) {
            this->precision = precision;
        }
        
        template<typename ValueType>
        void EigenLinearEquationSolverSettings<ValueType>::setMaximalNumberOfIterations(uint64_t maximalNumberOfIterations) {
            this->maximalNumberOfIterations = maximalNumberOfIterations;
        }
        
        template<typename ValueType>
        void EigenLinearEquationSolverSettings<ValueType>::setNumberOfIterationsUntilRestart(uint64_t restart) {
            this->restart = restart;
        }
        
        template<typename ValueType>
        typename EigenLinearEquationSolverSettings<ValueType>::SolutionMethod EigenLinearEquationSolverSettings<ValueType>::getSolutionMethod() const {
            return this->method;
        }
        
        template<typename ValueType>
        typename EigenLinearEquationSolverSettings<ValueType>::Preconditioner EigenLinearEquationSolverSettings<ValueType>::getPreconditioner() const {
            return this->preconditioner;
        }
        
        template<typename ValueType>
        ValueType EigenLinearEquationSolverSettings<ValueType>::getPrecision() const {
            return this->precision;
        }
        
        template<typename ValueType>
        uint64_t EigenLinearEquationSolverSettings<ValueType>::getMaximalNumberOfIterations() const {
            return this->maximalNumberOfIterations;
        }

        template<typename ValueType>
        uint64_t EigenLinearEquationSolverSettings<ValueType>::getNumberOfIterationsUntilRestart() const {
            return restart;
        }
        
#ifdef STORM_HAVE_CARL
        EigenLinearEquationSolverSettings<storm::RationalNumber>::EigenLinearEquationSolverSettings() {
            // Intentionally left empty.
        }

        EigenLinearEquationSolverSettings<storm::RationalFunction>::EigenLinearEquationSolverSettings() {
            // Intentionally left empty.
        }
#endif

        template<typename ValueType>
        EigenLinearEquationSolver<ValueType>::EigenLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, EigenLinearEquationSolverSettings<ValueType> const& settings) : eigenA(storm::adapters::EigenAdapter::toEigenSparseMatrix<ValueType>(A)), settings(settings) {
            // Intentionally left empty.
        }

        template<typename ValueType>
        EigenLinearEquationSolver<ValueType>::EigenLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A, EigenLinearEquationSolverSettings<ValueType> const& settings) : settings(settings) {
            this->setMatrix(std::move(A));
        }
        
        template<typename ValueType>
        void EigenLinearEquationSolver<ValueType>::setMatrix(storm::storage::SparseMatrix<ValueType> const& A) {
            eigenA = storm::adapters::EigenAdapter::toEigenSparseMatrix<ValueType>(A);
        }
        
        template<typename ValueType>
        void EigenLinearEquationSolver<ValueType>::setMatrix(storm::storage::SparseMatrix<ValueType>&& A) {
            // Take ownership of the matrix so it is destroyed after we have translated it to Eigen's format.
            storm::storage::SparseMatrix<ValueType> localA(std::move(A));
            this->setMatrix(localA);
        }
        
        template<typename ValueType>
        void EigenLinearEquationSolver<ValueType>::solveEquations(std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            // Map the input vectors to Eigen's format.
            auto eigenX = Eigen::Matrix<ValueType, Eigen::Dynamic, 1>::Map(x.data(), x.size());
            auto eigenB = Eigen::Matrix<ValueType, Eigen::Dynamic, 1>::Map(b.data(), b.size());

            typename EigenLinearEquationSolverSettings<ValueType>::SolutionMethod solutionMethod = this->getSettings().getSolutionMethod();
            if (solutionMethod == EigenLinearEquationSolverSettings<ValueType>::SolutionMethod::SparseLU) {
                Eigen::SparseLU<Eigen::SparseMatrix<ValueType>, Eigen::COLAMDOrdering<int>> solver;
                solver.compute(*this->eigenA);
                solver._solve_impl(eigenB, eigenX);
            } else {
                typename EigenLinearEquationSolverSettings<ValueType>::Preconditioner preconditioner = this->getSettings().getPreconditioner();
                if (solutionMethod == EigenLinearEquationSolverSettings<ValueType>::SolutionMethod::BiCGSTAB) {
                    if (preconditioner == EigenLinearEquationSolverSettings<ValueType>::Preconditioner::Ilu) {
                        Eigen::BiCGSTAB<Eigen::SparseMatrix<ValueType>, Eigen::IncompleteLUT<ValueType>> solver;
                        solver.compute(*this->eigenA);
                        solver.setTolerance(this->getSettings().getPrecision());
                        solver.setMaxIterations(this->getSettings().getMaximalNumberOfIterations());
                        eigenX = solver.solveWithGuess(eigenB, eigenX);
                    } else if (preconditioner == EigenLinearEquationSolverSettings<ValueType>::Preconditioner::Diagonal) {
                        Eigen::BiCGSTAB<Eigen::SparseMatrix<ValueType>, Eigen::DiagonalPreconditioner<ValueType>> solver;
                        solver.setTolerance(this->getSettings().getPrecision());
                        solver.setMaxIterations(this->getSettings().getMaximalNumberOfIterations());
                        solver.compute(*this->eigenA);
                        eigenX = solver.solveWithGuess(eigenB, eigenX);
                    } else {
                        Eigen::BiCGSTAB<Eigen::SparseMatrix<ValueType>, Eigen::IdentityPreconditioner> solver;
                        solver.setTolerance(this->getSettings().getPrecision());
                        solver.setMaxIterations(this->getSettings().getMaximalNumberOfIterations());
                        solver.compute(*this->eigenA);
                        eigenX = solver.solveWithGuess(eigenB, eigenX);
                    }
                } else if (solutionMethod == EigenLinearEquationSolverSettings<ValueType>::SolutionMethod::DGMRES) {
                    if (preconditioner == EigenLinearEquationSolverSettings<ValueType>::Preconditioner::Ilu) {
                        Eigen::DGMRES<Eigen::SparseMatrix<ValueType>, Eigen::IncompleteLUT<ValueType>> solver;
                        solver.setTolerance(this->getSettings().getPrecision());
                        solver.setMaxIterations(this->getSettings().getMaximalNumberOfIterations());
                        solver.set_restart(this->getSettings().getNumberOfIterationsUntilRestart());
                        solver.compute(*this->eigenA);
                        eigenX = solver.solveWithGuess(eigenB, eigenX);
                    } else if (preconditioner == EigenLinearEquationSolverSettings<ValueType>::Preconditioner::Diagonal) {
                        Eigen::DGMRES<Eigen::SparseMatrix<ValueType>, Eigen::DiagonalPreconditioner<ValueType>> solver;
                        solver.setTolerance(this->getSettings().getPrecision());
                        solver.setMaxIterations(this->getSettings().getMaximalNumberOfIterations());
                        solver.set_restart(this->getSettings().getNumberOfIterationsUntilRestart());
                        solver.compute(*this->eigenA);
                        eigenX = solver.solveWithGuess(eigenB, eigenX);
                    } else {
                        Eigen::DGMRES<Eigen::SparseMatrix<ValueType>, Eigen::IdentityPreconditioner> solver;
                        solver.setTolerance(this->getSettings().getPrecision());
                        solver.setMaxIterations(this->getSettings().getMaximalNumberOfIterations());
                        solver.set_restart(this->getSettings().getNumberOfIterationsUntilRestart());
                        solver.compute(*this->eigenA);
                        eigenX = solver.solveWithGuess(eigenB, eigenX);
                    }
                } else if (solutionMethod == EigenLinearEquationSolverSettings<ValueType>::SolutionMethod::GMRES) {
                    if (preconditioner == EigenLinearEquationSolverSettings<ValueType>::Preconditioner::Ilu) {
                        Eigen::GMRES<Eigen::SparseMatrix<ValueType>, Eigen::IncompleteLUT<ValueType>> solver;
                        solver.setTolerance(this->getSettings().getPrecision());
                        solver.setMaxIterations(this->getSettings().getMaximalNumberOfIterations());
                        solver.set_restart(this->getSettings().getNumberOfIterationsUntilRestart());
                        solver.compute(*this->eigenA);
                        eigenX = solver.solveWithGuess(eigenB, eigenX);
                    } else if (preconditioner == EigenLinearEquationSolverSettings<ValueType>::Preconditioner::Diagonal) {
                        Eigen::GMRES<Eigen::SparseMatrix<ValueType>, Eigen::DiagonalPreconditioner<ValueType>> solver;
                        solver.setTolerance(this->getSettings().getPrecision());
                        solver.setMaxIterations(this->getSettings().getMaximalNumberOfIterations());
                        solver.set_restart(this->getSettings().getNumberOfIterationsUntilRestart());
                        solver.compute(*this->eigenA);
                        eigenX = solver.solveWithGuess(eigenB, eigenX);
                    } else {
                        Eigen::GMRES<Eigen::SparseMatrix<ValueType>, Eigen::IdentityPreconditioner> solver;
                        solver.setTolerance(this->getSettings().getPrecision());
                        solver.setMaxIterations(this->getSettings().getMaximalNumberOfIterations());
                        solver.set_restart(this->getSettings().getNumberOfIterationsUntilRestart());
                        solver.compute(*this->eigenA);
                        eigenX = solver.solveWithGuess(eigenB, eigenX);
                    }
                }
            }
        }
        
        template<typename ValueType>
        void EigenLinearEquationSolver<ValueType>::multiply(std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<ValueType>& result) const {
            // Typedef the map-type so we don't have to spell it out.
            typedef decltype(Eigen::Matrix<ValueType, Eigen::Dynamic, 1>::Map(b->data(), b->size())) MapType;

            auto eigenX = Eigen::Matrix<ValueType, Eigen::Dynamic, 1>::Map(x.data(), x.size());
            auto eigenResult = Eigen::Matrix<ValueType, Eigen::Dynamic, 1>::Map(result.data(), result.size());

            std::unique_ptr<MapType> eigenB;
            if (b != nullptr) {
                eigenB = std::make_unique<MapType>(Eigen::Matrix<ValueType, Eigen::Dynamic, 1>::Map(b->data(), b->size()));
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
        EigenLinearEquationSolverSettings<ValueType>& EigenLinearEquationSolver<ValueType>::getSettings() {
            return settings;
        }
        
        template<typename ValueType>
        EigenLinearEquationSolverSettings<ValueType> const& EigenLinearEquationSolver<ValueType>::getSettings() const {
            return settings;
        }
        
        template<typename ValueType>
        uint64_t EigenLinearEquationSolver<ValueType>::getMatrixRowCount() const {
            return eigenA->rows();
        }
        
        template<typename ValueType>
        uint64_t EigenLinearEquationSolver<ValueType>::getMatrixColumnCount() const {
            return eigenA->cols();
        }
        
#ifdef STORM_HAVE_CARL
        // Specialization for storm::RationalNumber
        template<>
        void EigenLinearEquationSolver<storm::RationalNumber>::solveEquations(std::vector<storm::RationalNumber>& x, std::vector<storm::RationalNumber> const& b) const {
            // Map the input vectors to Eigen's format.
            auto eigenX = Eigen::Matrix<storm::RationalNumber, Eigen::Dynamic, 1>::Map(x.data(), x.size());
            auto eigenB = Eigen::Matrix<storm::RationalNumber, Eigen::Dynamic, 1>::Map(b.data(), b.size());
                        
            Eigen::SparseLU<Eigen::SparseMatrix<storm::RationalNumber>, Eigen::COLAMDOrdering<int>> solver;
            solver.compute(*eigenA);
            solver._solve_impl(eigenB, eigenX);
        }
        
        // Specialization for storm::RationalFunction
        template<>
        void EigenLinearEquationSolver<storm::RationalFunction>::solveEquations(std::vector<storm::RationalFunction>& x, std::vector<storm::RationalFunction> const& b) const {
            // Map the input vectors to Eigen's format.
            auto eigenX = Eigen::Matrix<storm::RationalFunction, Eigen::Dynamic, 1>::Map(x.data(), x.size());
            auto eigenB = Eigen::Matrix<storm::RationalFunction, Eigen::Dynamic, 1>::Map(b.data(), b.size());
            
            Eigen::SparseLU<Eigen::SparseMatrix<storm::RationalFunction>, Eigen::COLAMDOrdering<int>> solver;
            solver.compute(*eigenA);
            solver._solve_impl(eigenB, eigenX);
        }
#endif

        template<typename ValueType>
        std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> EigenLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType> const& matrix) const {
            return std::make_unique<storm::solver::EigenLinearEquationSolver<ValueType>>(matrix, settings);
        }
        
        template<typename ValueType>
        std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> EigenLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType>&& matrix) const {
            return std::make_unique<storm::solver::EigenLinearEquationSolver<ValueType>>(std::move(matrix), settings);
        }
        
        template<typename ValueType>
        EigenLinearEquationSolverSettings<ValueType>& EigenLinearEquationSolverFactory<ValueType>::getSettings() {
            return settings;
        }
        
        template<typename ValueType>
        EigenLinearEquationSolverSettings<ValueType> const& EigenLinearEquationSolverFactory<ValueType>::getSettings() const {
            return settings;
        }
        
        template<typename ValueType>
        std::unique_ptr<LinearEquationSolverFactory<ValueType>> EigenLinearEquationSolverFactory<ValueType>::clone() const {
            return std::make_unique<EigenLinearEquationSolverFactory<ValueType>>(*this);
        }
        
        template class EigenLinearEquationSolverSettings<double>;
        template class EigenLinearEquationSolver<double>;
        template class EigenLinearEquationSolverFactory<double>;

#ifdef STORM_HAVE_CARL
        template class EigenLinearEquationSolverSettings<storm::RationalNumber>;
        template class EigenLinearEquationSolverSettings<storm::RationalFunction>;
        
        template class EigenLinearEquationSolver<storm::RationalNumber>;
        template class EigenLinearEquationSolver<storm::RationalFunction>;
        
        template class EigenLinearEquationSolverFactory<storm::RationalNumber>;
        template class EigenLinearEquationSolverFactory<storm::RationalFunction>;
#endif
    }
}
