#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/solver/GmmxxLinearEquationSolver.h"
#include "src/settings/SettingsManager.h"

TEST(GmmxxLinearEquationSolver, SolveWithStandardOptions) {
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
    
    ASSERT_NO_THROW(storm::solver::GmmxxLinearEquationSolver<double> solver(A));
    
    storm::solver::GmmxxLinearEquationSolver<double> solver(A);
    ASSERT_NO_THROW(solver.solveEquationSystem(x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::gmmxxEquationSolverSettings().getPrecision());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::gmmxxEquationSolverSettings().getPrecision());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::gmmxxEquationSolverSettings().getPrecision());
}

TEST(GmmxxLinearEquationSolver, gmres) {
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
    
    ASSERT_NO_THROW(storm::solver::GmmxxLinearEquationSolver<double> solver(A, storm::solver::GmmxxLinearEquationSolver<double>::SolutionMethod::Gmres, 1e-6, 10000, storm::solver::GmmxxLinearEquationSolver<double>::Preconditioner::None, true, 50));
    
    storm::solver::GmmxxLinearEquationSolver<double> solver(A, storm::solver::GmmxxLinearEquationSolver<double>::SolutionMethod::Gmres, 1e-6, 10000, storm::solver::GmmxxLinearEquationSolver<double>::Preconditioner::None, true, 50);
    ASSERT_NO_THROW(solver.solveEquationSystem(x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::gmmxxEquationSolverSettings().getPrecision());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::gmmxxEquationSolverSettings().getPrecision());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::gmmxxEquationSolverSettings().getPrecision());
}

TEST(GmmxxLinearEquationSolver, qmr) {
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
    
    ASSERT_NO_THROW(storm::solver::GmmxxLinearEquationSolver<double> solver(A, storm::solver::GmmxxLinearEquationSolver<double>::SolutionMethod::Qmr, 1e-6, 10000, storm::solver::GmmxxLinearEquationSolver<double>::Preconditioner::None));
    
    storm::solver::GmmxxLinearEquationSolver<double> solver(A, storm::solver::GmmxxLinearEquationSolver<double>::SolutionMethod::Qmr, 1e-6, 10000, storm::solver::GmmxxLinearEquationSolver<double>::Preconditioner::None, true, 50);
    ASSERT_NO_THROW(solver.solveEquationSystem(x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::gmmxxEquationSolverSettings().getPrecision());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::gmmxxEquationSolverSettings().getPrecision());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::gmmxxEquationSolverSettings().getPrecision());
}

TEST(GmmxxLinearEquationSolver, bicgstab) {
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
    
    ASSERT_NO_THROW(storm::solver::GmmxxLinearEquationSolver<double> solver(A, storm::solver::GmmxxLinearEquationSolver<double>::SolutionMethod::Bicgstab, 1e-6, 10000, storm::solver::GmmxxLinearEquationSolver<double>::Preconditioner::None));
    
    storm::solver::GmmxxLinearEquationSolver<double> solver(A, storm::solver::GmmxxLinearEquationSolver<double>::SolutionMethod::Bicgstab, 1e-6, 10000, storm::solver::GmmxxLinearEquationSolver<double>::Preconditioner::None);
    ASSERT_NO_THROW(solver.solveEquationSystem(x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::gmmxxEquationSolverSettings().getPrecision());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::gmmxxEquationSolverSettings().getPrecision());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::gmmxxEquationSolverSettings().getPrecision());
}

TEST(GmmxxLinearEquationSolver, jacobi) {
    ASSERT_NO_THROW(storm::storage::SparseMatrixBuilder<double> builder);
    storm::storage::SparseMatrixBuilder<double> builder;
    ASSERT_NO_THROW(builder.addNextValue(0, 0, 4));
    ASSERT_NO_THROW(builder.addNextValue(0, 1, 2));
    ASSERT_NO_THROW(builder.addNextValue(0, 2, -1));
    ASSERT_NO_THROW(builder.addNextValue(1, 0, 1));
    ASSERT_NO_THROW(builder.addNextValue(1, 1, -5));
    ASSERT_NO_THROW(builder.addNextValue(1, 2, 2));
    ASSERT_NO_THROW(builder.addNextValue(2, 0, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 1, 2));
    ASSERT_NO_THROW(builder.addNextValue(2, 2, 4));

    storm::storage::SparseMatrix<double> A;
    ASSERT_NO_THROW(A = builder.build());
    
    std::vector<double> x(3);
    std::vector<double> b = {11, -16, 1};
    
    ASSERT_NO_THROW(storm::solver::GmmxxLinearEquationSolver<double> solver(A, storm::solver::GmmxxLinearEquationSolver<double>::SolutionMethod::Jacobi, 1e-6, 10000, storm::solver::GmmxxLinearEquationSolver<double>::Preconditioner::None));
    
    storm::solver::GmmxxLinearEquationSolver<double> solver(A, storm::solver::GmmxxLinearEquationSolver<double>::SolutionMethod::Jacobi, 1e-6, 10000, storm::solver::GmmxxLinearEquationSolver<double>::Preconditioner::None);
    ASSERT_NO_THROW(solver.solveEquationSystem(x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::gmmxxEquationSolverSettings().getPrecision());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::gmmxxEquationSolverSettings().getPrecision());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::gmmxxEquationSolverSettings().getPrecision());
}

TEST(GmmxxLinearEquationSolver, gmresilu) {
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
    
    ASSERT_NO_THROW(storm::solver::GmmxxLinearEquationSolver<double> solver(A, storm::solver::GmmxxLinearEquationSolver<double>::SolutionMethod::Gmres, 1e-6, 10000, storm::solver::GmmxxLinearEquationSolver<double>::Preconditioner::Ilu, true, 50));
    
    storm::solver::GmmxxLinearEquationSolver<double> solver(A, storm::solver::GmmxxLinearEquationSolver<double>::SolutionMethod::Gmres, 1e-6, 10000, storm::solver::GmmxxLinearEquationSolver<double>::Preconditioner::None, true, 50);
    ASSERT_NO_THROW(solver.solveEquationSystem(x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::gmmxxEquationSolverSettings().getPrecision());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::gmmxxEquationSolverSettings().getPrecision());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::gmmxxEquationSolverSettings().getPrecision());
}

TEST(GmmxxLinearEquationSolver, gmresdiag) {
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
    
    ASSERT_NO_THROW(storm::solver::GmmxxLinearEquationSolver<double> solver(A, storm::solver::GmmxxLinearEquationSolver<double>::SolutionMethod::Gmres, 1e-6, 10000, storm::solver::GmmxxLinearEquationSolver<double>::Preconditioner::Diagonal, true, 50));
    
    storm::solver::GmmxxLinearEquationSolver<double> solver(A, storm::solver::GmmxxLinearEquationSolver<double>::SolutionMethod::Gmres, 1e-6, 10000, storm::solver::GmmxxLinearEquationSolver<double>::Preconditioner::None, true, 50);
    ASSERT_NO_THROW(solver.solveEquationSystem(x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::gmmxxEquationSolverSettings().getPrecision());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::gmmxxEquationSolverSettings().getPrecision());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::gmmxxEquationSolverSettings().getPrecision());
}

TEST(GmmxxLinearEquationSolver, MatrixVectorMultplication) {
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
    
    storm::solver::GmmxxLinearEquationSolver<double> solver(A);
    ASSERT_NO_THROW(solver.performMatrixVectorMultiplication(x, nullptr, 4));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::gmmxxEquationSolverSettings().getPrecision());
}