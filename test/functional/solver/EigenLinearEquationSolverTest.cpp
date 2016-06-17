#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/solver/EigenLinearEquationSolver.h"
#include "src/settings/SettingsManager.h"

#include "src/settings/modules/GmmxxEquationSolverSettings.h"

TEST(EigenLinearEquationSolver, SolveWithStandardOptions) {
    ASSERT_NO_THROW(storm::storage::SparseMatrixBuilder<double> builder);
    storm::storage::SparseMatrixBuilder<double> builder;
    ASSERT_NO_THROW(builder.addNextValue(0, 0, 2));
    ASSERT_NO_THROW(builder.addNextValue(0, 1, 4));
    ASSERT_NO_THROW(builder.addNextValue(0, 2, -2));
    ASSERT_NO_THROW(builder.addNextValue(1, 0, 4));
    ASSERT_NO_THROW(builder.addNextValue(1, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(1, 2, 5));
    ASSERT_NO_THROW(builder.addNextValue(2, 0, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 2, 3));
    
    storm::storage::SparseMatrix<double> A;
    ASSERT_NO_THROW(A = builder.build());
    
    std::vector<double> x(3);
    std::vector<double> b = {16, -4, -7};
    
    ASSERT_NO_THROW(storm::solver::EigenLinearEquationSolver<double> solver(A));
    
    storm::solver::EigenLinearEquationSolver<double> solver(A);
    ASSERT_NO_THROW(solver.solveEquationSystem(x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
}

TEST(EigenLinearEquationSolver, SparseLU) {
    ASSERT_NO_THROW(storm::storage::SparseMatrixBuilder<double> builder);
    storm::storage::SparseMatrixBuilder<double> builder;
    ASSERT_NO_THROW(builder.addNextValue(0, 0, 2));
    ASSERT_NO_THROW(builder.addNextValue(0, 1, 4));
    ASSERT_NO_THROW(builder.addNextValue(0, 2, -2));
    ASSERT_NO_THROW(builder.addNextValue(1, 0, 4));
    ASSERT_NO_THROW(builder.addNextValue(1, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(1, 2, 5));
    ASSERT_NO_THROW(builder.addNextValue(2, 0, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 2, 3));
    
    storm::storage::SparseMatrix<double> A;
    ASSERT_NO_THROW(A = builder.build());
    
    std::vector<double> x(3);
    std::vector<double> b = {16, -4, -7};
    
    ASSERT_NO_THROW(storm::solver::EigenLinearEquationSolver<double> solver(A, storm::solver::EigenLinearEquationSolver<double>::SolutionMethod::SparseLU, 1e-6, 10000, storm::solver::EigenLinearEquationSolver<double>::Preconditioner::None));
    
    storm::solver::EigenLinearEquationSolver<double> solver(A, storm::solver::EigenLinearEquationSolver<double>::SolutionMethod::SparseLU, 1e-6, 10000, storm::solver::EigenLinearEquationSolver<double>::Preconditioner::None);
    ASSERT_NO_THROW(solver.solveEquationSystem(x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
}

TEST(EigenLinearEquationSolver, BiCGSTAB) {
    ASSERT_NO_THROW(storm::storage::SparseMatrixBuilder<double> builder);
    storm::storage::SparseMatrixBuilder<double> builder;
    ASSERT_NO_THROW(builder.addNextValue(0, 0, 2));
    ASSERT_NO_THROW(builder.addNextValue(0, 1, 4));
    ASSERT_NO_THROW(builder.addNextValue(0, 2, -2));
    ASSERT_NO_THROW(builder.addNextValue(1, 0, 4));
    ASSERT_NO_THROW(builder.addNextValue(1, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(1, 2, 5));
    ASSERT_NO_THROW(builder.addNextValue(2, 0, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 2, 3));
    
    storm::storage::SparseMatrix<double> A;
    ASSERT_NO_THROW(A = builder.build());
    
    std::vector<double> x(3);
    std::vector<double> b = {16, -4, -7};
    
    ASSERT_NO_THROW(storm::solver::EigenLinearEquationSolver<double> solver(A, storm::solver::EigenLinearEquationSolver<double>::SolutionMethod::Bicgstab, 1e-6, 10000, storm::solver::EigenLinearEquationSolver<double>::Preconditioner::None));
    
    storm::solver::EigenLinearEquationSolver<double> solver(A, storm::solver::EigenLinearEquationSolver<double>::SolutionMethod::Bicgstab, 1e-6, 10000, storm::solver::EigenLinearEquationSolver<double>::Preconditioner::None);
    ASSERT_NO_THROW(solver.solveEquationSystem(x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
}

TEST(EigenLinearEquationSolver, BiCGSTAB_Ilu) {
    ASSERT_NO_THROW(storm::storage::SparseMatrixBuilder<double> builder);
    storm::storage::SparseMatrixBuilder<double> builder;
    ASSERT_NO_THROW(builder.addNextValue(0, 0, 2));
    ASSERT_NO_THROW(builder.addNextValue(0, 1, 4));
    ASSERT_NO_THROW(builder.addNextValue(0, 2, -2));
    ASSERT_NO_THROW(builder.addNextValue(1, 0, 4));
    ASSERT_NO_THROW(builder.addNextValue(1, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(1, 2, 5));
    ASSERT_NO_THROW(builder.addNextValue(2, 0, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 2, 3));
    
    storm::storage::SparseMatrix<double> A;
    ASSERT_NO_THROW(A = builder.build());
    
    std::vector<double> x(3);
    std::vector<double> b = {16, -4, -7};
    
    ASSERT_NO_THROW(storm::solver::EigenLinearEquationSolver<double> solver(A, storm::solver::EigenLinearEquationSolver<double>::SolutionMethod::Bicgstab, 1e-6, 10000, storm::solver::EigenLinearEquationSolver<double>::Preconditioner::Ilu));
    
    storm::solver::EigenLinearEquationSolver<double> solver(A, storm::solver::EigenLinearEquationSolver<double>::SolutionMethod::Bicgstab, 1e-6, 10000, storm::solver::EigenLinearEquationSolver<double>::Preconditioner::Ilu);
    ASSERT_NO_THROW(solver.solveEquationSystem(x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
}

TEST(EigenLinearEquationSolver, BiCGSTAB_Diagonal) {
    ASSERT_NO_THROW(storm::storage::SparseMatrixBuilder<double> builder);
    storm::storage::SparseMatrixBuilder<double> builder;
    ASSERT_NO_THROW(builder.addNextValue(0, 0, 2));
    ASSERT_NO_THROW(builder.addNextValue(0, 1, 4));
    ASSERT_NO_THROW(builder.addNextValue(0, 2, -2));
    ASSERT_NO_THROW(builder.addNextValue(1, 0, 4));
    ASSERT_NO_THROW(builder.addNextValue(1, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(1, 2, 5));
    ASSERT_NO_THROW(builder.addNextValue(2, 0, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 2, 3));
    
    storm::storage::SparseMatrix<double> A;
    ASSERT_NO_THROW(A = builder.build());
    
    std::vector<double> x(3);
    std::vector<double> b = {16, -4, -7};
    
    ASSERT_NO_THROW(storm::solver::EigenLinearEquationSolver<double> solver(A, storm::solver::EigenLinearEquationSolver<double>::SolutionMethod::Bicgstab, 1e-6, 10000, storm::solver::EigenLinearEquationSolver<double>::Preconditioner::Diagonal));
    
    storm::solver::EigenLinearEquationSolver<double> solver(A, storm::solver::EigenLinearEquationSolver<double>::SolutionMethod::Bicgstab, 1e-6, 10000, storm::solver::EigenLinearEquationSolver<double>::Preconditioner::Diagonal);
    ASSERT_NO_THROW(solver.solveEquationSystem(x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
}

TEST(EigenLinearEquationSolver, MatrixVectorMultplication) {
    ASSERT_NO_THROW(storm::storage::SparseMatrixBuilder<double> builder);
    storm::storage::SparseMatrixBuilder<double> builder;
    ASSERT_NO_THROW(builder.addNextValue(0, 1, 0.5));
    ASSERT_NO_THROW(builder.addNextValue(0, 4, 0.5));
    ASSERT_NO_THROW(builder.addNextValue(1, 2, 0.5));
    ASSERT_NO_THROW(builder.addNextValue(1, 4, 0.5));
    ASSERT_NO_THROW(builder.addNextValue(2, 3, 0.5));
    ASSERT_NO_THROW(builder.addNextValue(2, 4, 0.5));
    ASSERT_NO_THROW(builder.addNextValue(3, 4, 1));
    ASSERT_NO_THROW(builder.addNextValue(4, 4, 1));
    
    storm::storage::SparseMatrix<double> A;
    ASSERT_NO_THROW(A = builder.build());
    
    std::vector<double> x(5);
    x[4] = 1;
    
    storm::solver::EigenLinearEquationSolver<double> solver(A);
    ASSERT_NO_THROW(solver.performMatrixVectorMultiplication(x, nullptr, 4));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
}