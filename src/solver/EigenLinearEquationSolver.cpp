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
            
            // Determine the method to be used.
            storm::settings::modules::EigenEquationSolverSettings::LinearEquationMethod methodAsSetting = settings.getLinearEquationSystemMethod();
            if (methodAsSetting == storm::settings::modules::EigenEquationSolverSettings::LinearEquationMethod::Bicgstab) {
                method = SolutionMethod::Bicgstab;
            } else if (methodAsSetting == storm::settings::modules::EigenEquationSolverSettings::LinearEquationMethod::SparseLU) {
                method = SolutionMethod::SparseLU;
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
            // Translate the vectors x and b into Eigen's format.
            Eigen::Matrix<ValueType, Eigen::Dynamic, 1> eigenB(b.size());
            for (uint64_t index = 0; index < b.size(); ++index) {
                eigenB[index] = b[index];
            }
            
            Eigen::Matrix<ValueType, Eigen::Dynamic, 1> eigenX(x.size());
            for (uint64_t index = 0; index < x.size(); ++index) {
                eigenX[index] = x[index];
            }
            
            if (this->getSettings().getSolutionMethod() == EigenLinearEquationSolverSettings<ValueType>::SolutionMethod::SparseLU) {
                Eigen::SparseLU<Eigen::SparseMatrix<ValueType>, Eigen::COLAMDOrdering<int>> solver;
                solver.compute(*this->eigenA);
                if (solver.info() != Eigen::Success) {
                    std::cout << solver.lastErrorMessage() << std::endl;
                }
                solver._solve_impl(eigenB, eigenX);
            } else {
                if (this->getSettings().getPreconditioner() == EigenLinearEquationSolverSettings<ValueType>::Preconditioner::Ilu) {
                    Eigen::BiCGSTAB<Eigen::SparseMatrix<ValueType>, Eigen::IncompleteLUT<ValueType>> solver;
                    solver.compute(*this->eigenA);
                    solver.solveWithGuess(eigenB, eigenX);
                } else if (this->getSettings().getPreconditioner() == EigenLinearEquationSolverSettings<ValueType>::Preconditioner::Diagonal) {
                    Eigen::BiCGSTAB<Eigen::SparseMatrix<ValueType>, Eigen::DiagonalPreconditioner<ValueType>> solver;
                    solver.compute(*this->eigenA);
                    solver.solveWithGuess(eigenB, eigenX);
                } else {
                    Eigen::BiCGSTAB<Eigen::SparseMatrix<ValueType>, Eigen::IdentityPreconditioner> solver;
                    solver.compute(*this->eigenA);
                    solver.solveWithGuess(eigenB, eigenX);
                }
            }

            // Translate the solution from Eigen's format into our representation.
            for (uint64_t index = 0; index < eigenX.size(); ++index) {
                x[index] = eigenX[index];
            }
        }
        
        template<typename ValueType>
        void EigenLinearEquationSolver<ValueType>::performMatrixVectorMultiplication(std::vector<ValueType>& x, std::vector<ValueType> const* b, uint_fast64_t n, std::vector<ValueType>* multiplyResult) const {
            // Translate the vectors x and b into Eigen's format.
            std::unique_ptr<Eigen::Matrix<ValueType, Eigen::Dynamic, 1>> eigenB;
            if (b != nullptr) {
                eigenB = std::make_unique<Eigen::Matrix<ValueType, Eigen::Dynamic, 1>>(b->size());
                for (uint64_t index = 0; index < b->size(); ++index) {
                    (*eigenB)[index] = (*b)[index];
                }
            }
            Eigen::Matrix<ValueType, Eigen::Dynamic, 1> eigenX(x.size());
            for (uint64_t index = 0; index < x.size(); ++index) {
                eigenX[index] = x[index];
            }
            
            // Perform n matrix-vector multiplications.
            for (uint64_t iteration = 0; iteration < n; ++iteration) {
                eigenX = *this->eigenA * eigenX;
                if (eigenB != nullptr) {
                    eigenX += *eigenB;
                }
            }
            
            // Translate the solution from Eigen's format into our representation.
            for (uint64_t index = 0; index < eigenX.size(); ++index) {
                x[index] = eigenX[index];
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
            // Translate the vectors x and b into Eigen's format.
            Eigen::Matrix<storm::RationalNumber, Eigen::Dynamic, 1> eigenB(b.size());
            for (uint64_t index = 0; index < b.size(); ++index) {
                eigenB[index] = b[index];
            }
            
            Eigen::Matrix<storm::RationalNumber, Eigen::Dynamic, 1> eigenX(x.size());
            for (uint64_t index = 0; index < x.size(); ++index) {
                eigenX[index] = x[index];
            }
            
            Eigen::SparseLU<Eigen::SparseMatrix<storm::RationalNumber>, Eigen::COLAMDOrdering<int>> solver;
            solver.compute(*eigenA);
            solver._solve_impl(eigenB, eigenX);
            
            // Translate the solution from Eigen's format into our representation.
            for (uint64_t index = 0; index < eigenX.size(); ++index) {
                x[index] = eigenX[index];
            }
        }
        
        template<>
        void EigenLinearEquationSolver<storm::RationalNumber>::performMatrixVectorMultiplication(std::vector<storm::RationalNumber>& x, std::vector<storm::RationalNumber> const* b, uint_fast64_t n, std::vector<storm::RationalNumber>* multiplyResult) const {
            // Translate the vectors x and b into Eigen's format.
            std::unique_ptr<Eigen::Matrix<storm::RationalNumber, Eigen::Dynamic, 1>> eigenB;
            if (b != nullptr) {
                eigenB = std::make_unique<Eigen::Matrix<storm::RationalNumber, Eigen::Dynamic, 1>>(b->size());
                for (uint64_t index = 0; index < b->size(); ++index) {
                    (*eigenB)[index] = (*b)[index];
                }
            }
            Eigen::Matrix<storm::RationalNumber, Eigen::Dynamic, 1> eigenX(x.size());
            for (uint64_t index = 0; index < x.size(); ++index) {
                eigenX[index] = x[index];
            }
            
            // Perform n matrix-vector multiplications.
            for (uint64_t iteration = 0; iteration < n; ++iteration) {
                eigenX = *eigenA * eigenX;
                if (eigenB != nullptr) {
                    eigenX += *eigenB;
                }
            }
            
            // Translate the solution from Eigen's format into our representation.
            for (uint64_t index = 0; index < eigenX.size(); ++index) {
                x[index] = eigenX[index];
            }
        }
        
        // Specialization form storm::RationalFunction
        
        template<>
        void EigenLinearEquationSolver<storm::RationalFunction>::solveEquationSystem(std::vector<storm::RationalFunction>& x, std::vector<storm::RationalFunction> const& b, std::vector<storm::RationalFunction>* multiplyResult) const {
            // Translate the vectors x and b into Eigen's format.
            Eigen::Matrix<storm::RationalFunction, Eigen::Dynamic, 1> eigenB(b.size());
            for (uint64_t index = 0; index < b.size(); ++index) {
                eigenB[index] = b[index];
            }
            
            Eigen::Matrix<storm::RationalFunction, Eigen::Dynamic, 1> eigenX(x.size());
            for (uint64_t index = 0; index < x.size(); ++index) {
                eigenX[index] = x[index];
            }
            
            Eigen::SparseLU<Eigen::SparseMatrix<storm::RationalFunction>, Eigen::COLAMDOrdering<int>> solver;
            solver.compute(*eigenA);
            solver._solve_impl(eigenB, eigenX);
            
            // Translate the solution from Eigen's format into our representation.
            for (uint64_t index = 0; index < eigenX.size(); ++index) {
                x[index] = eigenX[index];
            }
        }
        
        template<>
        void EigenLinearEquationSolver<storm::RationalFunction>::performMatrixVectorMultiplication(std::vector<storm::RationalFunction>& x, std::vector<storm::RationalFunction> const* b, uint_fast64_t n, std::vector<storm::RationalFunction>* multiplyResult) const {
            // Translate the vectors x and b into Eigen's format.
            std::unique_ptr<Eigen::Matrix<storm::RationalFunction, Eigen::Dynamic, 1>> eigenB;
            if (b != nullptr) {
                eigenB = std::make_unique<Eigen::Matrix<storm::RationalFunction, Eigen::Dynamic, 1>>(b->size());
                for (uint64_t index = 0; index < b->size(); ++index) {
                    (*eigenB)[index] = (*b)[index];
                }
            }
            Eigen::Matrix<storm::RationalFunction, Eigen::Dynamic, 1> eigenX(x.size());
            for (uint64_t index = 0; index < x.size(); ++index) {
                eigenX[index] = x[index];
            }
            
            // Perform n matrix-vector multiplications.
            for (uint64_t iteration = 0; iteration < n; ++iteration) {
                eigenX = *eigenA * eigenX;
                if (eigenB != nullptr) {
                    eigenX += *eigenB;
                }
            }
            
            // Translate the solution from Eigen's format into our representation.
            for (uint64_t index = 0; index < eigenX.size(); ++index) {
                x[index] = eigenX[index];
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