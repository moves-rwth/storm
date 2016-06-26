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
        
        EigenLinearEquationSolverSettings<storm::RationalNumber>::EigenLinearEquationSolverSettings() {
            // Intentionally left empty.
        }

        EigenLinearEquationSolverSettings<storm::RationalFunction>::EigenLinearEquationSolverSettings() {
            // Intentionally left empty.
        }

        template<typename ValueType>
        EigenLinearEquationSolver<ValueType>::EigenLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, EigenLinearEquationSolverSettings<ValueType> const& settings) : eigenA(storm::adapters::EigenAdapter::toEigenSparseMatrix<ValueType>(A)), settings(settings) {
            // Intentionally left empty.
        }

        template<typename ValueType>
        EigenLinearEquationSolver<ValueType>::EigenLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A, EigenLinearEquationSolverSettings<ValueType> const& settings) : settings(settings) {
            storm::storage::SparseMatrix<ValueType> localA(std::move(A));
            eigenA = storm::adapters::EigenAdapter::toEigenSparseMatrix<ValueType>(localA);
        }
        
        template<typename ValueType>
        void EigenLinearEquationSolver<ValueType>::solveEquationSystem(std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult) const {
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
        void EigenLinearEquationSolver<ValueType>::performMatrixVectorMultiplication(std::vector<ValueType>& x, std::vector<ValueType> const* b, uint_fast64_t n, std::vector<ValueType>* multiplyResult) const {
            // Typedef the map-type so we don't have to spell it out.
            typedef decltype(Eigen::Matrix<ValueType, Eigen::Dynamic, 1>::Map(b->data(), b->size())) MapType;
            
            bool multiplyResultProvided = multiplyResult != nullptr;
            if (!multiplyResult) {
                multiplyResult = new std::vector<ValueType>(eigenA->cols());
            }
            auto eigenMultiplyResult = Eigen::Matrix<ValueType, Eigen::Dynamic, 1>::Map(multiplyResult->data(), multiplyResult->size());
            
            // Map the input vectors x and b to Eigen's format.
            std::unique_ptr<MapType> eigenB;
            if (b != nullptr) {
                eigenB = std::make_unique<MapType>(Eigen::Matrix<ValueType, Eigen::Dynamic, 1>::Map(b->data(), b->size()));
            }
            auto eigenX = Eigen::Matrix<ValueType, Eigen::Dynamic, 1>::Map(x.data(), x.size());
            
            // Perform n matrix-vector multiplications.
            auto currentX = &eigenX;
            auto nextX = &eigenMultiplyResult;
            for (uint64_t iteration = 0; iteration < n; ++iteration) {
                if (eigenB) {
                    nextX->noalias() = *eigenA * *currentX + *eigenB;
                } else {
                    nextX->noalias() = *eigenA * *currentX;
                }
                std::swap(nextX, currentX);
            }
            
            // If the last result we obtained is not the one in the input vector x, we swap the result there.
            if (currentX != &eigenX) {
                std::swap(*nextX, *currentX);
            }
            
            if (!multiplyResultProvided) {
                delete multiplyResult;
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
        
        // Specialization form storm::RationalNumber
        
        template<>
        void EigenLinearEquationSolver<storm::RationalNumber>::solveEquationSystem(std::vector<storm::RationalNumber>& x, std::vector<storm::RationalNumber> const& b, std::vector<storm::RationalNumber>* multiplyResult) const {
            // Map the input vectors to Eigen's format.
            auto eigenX = Eigen::Matrix<storm::RationalNumber, Eigen::Dynamic, 1>::Map(x.data(), x.size());
            auto eigenB = Eigen::Matrix<storm::RationalNumber, Eigen::Dynamic, 1>::Map(b.data(), b.size());
                        
            Eigen::SparseLU<Eigen::SparseMatrix<storm::RationalNumber>, Eigen::COLAMDOrdering<int>> solver;
            solver.compute(*eigenA);
            solver._solve_impl(eigenB, eigenX);
        }
        
        template<>
        void EigenLinearEquationSolver<storm::RationalNumber>::performMatrixVectorMultiplication(std::vector<storm::RationalNumber>& x, std::vector<storm::RationalNumber> const* b, uint_fast64_t n, std::vector<storm::RationalNumber>* multiplyResult) const {
            // Typedef the map-type so we don't have to spell it out.
            typedef decltype(Eigen::Matrix<storm::RationalNumber, Eigen::Dynamic, 1>::Map(b->data(), b->size())) MapType;

            bool multiplyResultProvided = multiplyResult != nullptr;
            if (!multiplyResult) {
                multiplyResult = new std::vector<storm::RationalNumber>(eigenA->cols());
            }
            auto eigenMultiplyResult = Eigen::Matrix<storm::RationalNumber, Eigen::Dynamic, 1>::Map(multiplyResult->data(), multiplyResult->size());
            
            // Map the input vectors x and b to Eigen's format.
            std::unique_ptr<MapType> eigenB;
            if (b != nullptr) {
                eigenB = std::make_unique<MapType>(Eigen::Matrix<storm::RationalNumber, Eigen::Dynamic, 1>::Map(b->data(), b->size()));
            }
            auto eigenX = Eigen::Matrix<storm::RationalNumber, Eigen::Dynamic, 1>::Map(x.data(), x.size());
            
            // Perform n matrix-vector multiplications.
            auto currentX = &eigenX;
            auto nextX = &eigenMultiplyResult;
            for (uint64_t iteration = 0; iteration < n; ++iteration) {
                if (eigenB) {
                    nextX->noalias() = *eigenA * *currentX + *eigenB;
                } else {
                    nextX->noalias() = *eigenA * *currentX;
                }
            }
            
            // If the last result we obtained is not the one in the input vector x, we swap the result there.
            if (currentX != &eigenX) {
                std::swap(*nextX, *currentX);
            }
            
            if (!multiplyResultProvided) {
                delete multiplyResult;
            }
        }
        
        // Specialization form storm::RationalFunction
        
        template<>
        void EigenLinearEquationSolver<storm::RationalFunction>::solveEquationSystem(std::vector<storm::RationalFunction>& x, std::vector<storm::RationalFunction> const& b, std::vector<storm::RationalFunction>* multiplyResult) const {
            // Map the input vectors to Eigen's format.
            auto eigenX = Eigen::Matrix<storm::RationalFunction, Eigen::Dynamic, 1>::Map(x.data(), x.size());
            auto eigenB = Eigen::Matrix<storm::RationalFunction, Eigen::Dynamic, 1>::Map(b.data(), b.size());
            
            Eigen::SparseLU<Eigen::SparseMatrix<storm::RationalFunction>, Eigen::COLAMDOrdering<int>> solver;
            solver.compute(*eigenA);
            solver._solve_impl(eigenB, eigenX);
        }
        
        template<>
        void EigenLinearEquationSolver<storm::RationalFunction>::performMatrixVectorMultiplication(std::vector<storm::RationalFunction>& x, std::vector<storm::RationalFunction> const* b, uint_fast64_t n, std::vector<storm::RationalFunction>* multiplyResult) const {
            // Typedef the map-type so we don't have to spell it out.
            typedef decltype(Eigen::Matrix<storm::RationalFunction, Eigen::Dynamic, 1>::Map(b->data(), b->size())) MapType;

            bool multiplyResultProvided = multiplyResult != nullptr;
            if (!multiplyResult) {
                multiplyResult = new std::vector<storm::RationalFunction>(eigenA->cols());
            }
            auto eigenMultiplyResult = Eigen::Matrix<storm::RationalFunction, Eigen::Dynamic, 1>::Map(multiplyResult->data(), multiplyResult->size());
            
            // Map the input vectors x and b to Eigen's format.
            std::unique_ptr<MapType> eigenB;
            if (b != nullptr) {
                eigenB = std::make_unique<MapType>(Eigen::Matrix<storm::RationalFunction, Eigen::Dynamic, 1>::Map(b->data(), b->size()));
            }
            auto eigenX = Eigen::Matrix<storm::RationalFunction, Eigen::Dynamic, 1>::Map(x.data(), x.size());
            
            // Perform n matrix-vector multiplications.
            auto currentX = &eigenX;
            auto nextX = &eigenMultiplyResult;
            for (uint64_t iteration = 0; iteration < n; ++iteration) {
                if (eigenB) {
                    nextX->noalias() = *eigenA * *currentX + *eigenB;
                } else {
                    nextX->noalias() = *eigenA * *currentX;
                }
            }
            
            // If the last result we obtained is not the one in the input vector x, we swap the result there.
            if (currentX != &eigenX) {
                std::swap(*nextX, *currentX);
            }
            
            if (!multiplyResultProvided) {
                delete multiplyResult;
            }
        }
        
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
        template class EigenLinearEquationSolverSettings<storm::RationalNumber>;
        template class EigenLinearEquationSolverSettings<storm::RationalFunction>;
        
        template class EigenLinearEquationSolver<double>;
        template class EigenLinearEquationSolver<storm::RationalNumber>;
        template class EigenLinearEquationSolver<storm::RationalFunction>;
        
        template class EigenLinearEquationSolverFactory<double>;
        template class EigenLinearEquationSolverFactory<storm::RationalNumber>;
        template class EigenLinearEquationSolverFactory<storm::RationalFunction>;
        
    }
}