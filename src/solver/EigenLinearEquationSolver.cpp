#include "src/solver/EigenLinearEquationSolver.h"

#include "src/adapters/EigenAdapter.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/modules/EigenEquationSolverSettings.h"

namespace storm {
    namespace solver {
     
        template<typename ValueType>
        EigenLinearEquationSolver<ValueType>::EigenLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, SolutionMethod method, double precision, uint64_t maximalNumberOfIterations, Preconditioner preconditioner) : originalA(&A), eigenA(storm::adapters::EigenAdapter::toEigenSparseMatrix<ValueType>(A)), precision(precision), maximalNumberOfIterations(maximalNumberOfIterations), preconditioner(preconditioner) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        EigenLinearEquationSolver<ValueType>::EigenLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A) : originalA(&A), eigenA(storm::adapters::EigenAdapter::toEigenSparseMatrix<ValueType>(A)) {
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
        void EigenLinearEquationSolver<ValueType>::solveEquationSystem(std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult) const {
            // Translate the vectors x and b into Eigen's format.
            Eigen::VectorXd eigenB(b.size());
            for (uint64_t index = 0; index < b.size(); ++index) {
                eigenB[index] = b[index];
            }
            
            Eigen::VectorXd eigenX(x.size());
            for (uint64_t index = 0; index < x.size(); ++index) {
                eigenX[index] = x[index];
            }
            
            if (method == SolutionMethod::SparseLU) {
                Eigen::SparseLU<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>> solver;
                solver.compute(*eigenA);
                solver._solve(eigenB, eigenX);
            } else {
                if (preconditioner == Preconditioner::Ilu) {
                    Eigen::BiCGSTAB<Eigen::SparseMatrix<double>, Eigen::IncompleteLUT<double>> solver;
                    solver.compute(*eigenA);
                    solver._solve(eigenB, eigenX);
                } else if (preconditioner == Preconditioner::Diagonal) {
                    Eigen::BiCGSTAB<Eigen::SparseMatrix<double>, Eigen::DiagonalPreconditioner<double>> solver;
                    solver.compute(*eigenA);
                    solver._solve(eigenB, eigenX);
                } else {
                    Eigen::BiCGSTAB<Eigen::SparseMatrix<double>, Eigen::IdentityPreconditioner> solver;
                    solver.compute(*eigenA);
                    solver._solve(eigenB, eigenX);
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
            std::unique_ptr<Eigen::VectorXd> eigenB;
            if (b != nullptr) {
                eigenB = std::make_unique<Eigen::VectorXd>(b->size());
                for (uint64_t index = 0; index < b->size(); ++index) {
                    (*eigenB)[index] = (*b)[index];
                }
            }
            Eigen::VectorXd eigenX(x.size());
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
        
        template class EigenLinearEquationSolver<double>;
        
        
    }
}