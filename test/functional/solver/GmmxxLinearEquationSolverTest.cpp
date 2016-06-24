#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/solver/GmmxxLinearEquationSolver.h"
#include "src/settings/SettingsManager.h"

#include "src/settings/modules/GmmxxEquationSolverSettings.h"

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
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
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
    
    storm::solver::GmmxxLinearEquationSolver<double> solver(A);
    solver.getSettings().setSolutionMethod(storm::solver::GmmxxLinearEquationSolverSettings<double>::SolutionMethod::Gmres);
    solver.getSettings().setPrecision(1e-6);
    solver.getSettings().setMaximalNumberOfIterations(10000);
    solver.getSettings().setPreconditioner(storm::solver::GmmxxLinearEquationSolverSettings<double>::Preconditioner::None);
    solver.getSettings().setNumberOfIterationsUntilRestart(50);
    
    ASSERT_NO_THROW(solver.solveEquationSystem(x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
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
    
    storm::solver::GmmxxLinearEquationSolver<double> solver(A);
    solver.getSettings().setSolutionMethod(storm::solver::GmmxxLinearEquationSolverSettings<double>::SolutionMethod::Qmr);
    solver.getSettings().setPrecision(1e-6);
    solver.getSettings().setMaximalNumberOfIterations(10000);
    solver.getSettings().setPreconditioner(storm::solver::GmmxxLinearEquationSolverSettings<double>::Preconditioner::None);
    
    storm::solver::GmmxxLinearEquationSolver<double> solver2(A);
    solver2.getSettings().setSolutionMethod(storm::solver::GmmxxLinearEquationSolverSettings<double>::SolutionMethod::Qmr);
    solver2.getSettings().setPrecision(1e-6);
    solver2.getSettings().setMaximalNumberOfIterations(10000);
    solver2.getSettings().setPreconditioner(storm::solver::GmmxxLinearEquationSolverSettings<double>::Preconditioner::None);
    
    ASSERT_NO_THROW(solver2.solveEquationSystem(x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
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
    
    storm::solver::GmmxxLinearEquationSolver<double> solver(A);
    solver.getSettings().setSolutionMethod(storm::solver::GmmxxLinearEquationSolverSettings<double>::SolutionMethod::Bicgstab);
    solver.getSettings().setPrecision(1e-6);
    solver.getSettings().setMaximalNumberOfIterations(10000);
    solver.getSettings().setPreconditioner(storm::solver::GmmxxLinearEquationSolverSettings<double>::Preconditioner::None);
    
    ASSERT_NO_THROW(solver.solveEquationSystem(x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
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
    
    storm::solver::GmmxxLinearEquationSolver<double> solver(A);
    solver.getSettings().setSolutionMethod(storm::solver::GmmxxLinearEquationSolverSettings<double>::SolutionMethod::Jacobi);
    solver.getSettings().setPrecision(1e-6);
    solver.getSettings().setMaximalNumberOfIterations(10000);
    solver.getSettings().setPreconditioner(storm::solver::GmmxxLinearEquationSolverSettings<double>::Preconditioner::None);
    ASSERT_NO_THROW(solver.solveEquationSystem(x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
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
    
    storm::solver::GmmxxLinearEquationSolver<double> solver(A);
    solver.getSettings().setSolutionMethod(storm::solver::GmmxxLinearEquationSolverSettings<double>::SolutionMethod::Gmres);
    solver.getSettings().setPrecision(1e-6);
    solver.getSettings().setMaximalNumberOfIterations(10000);
    solver.getSettings().setPreconditioner(storm::solver::GmmxxLinearEquationSolverSettings<double>::Preconditioner::Ilu);
    solver.getSettings().setNumberOfIterationsUntilRestart(50);
    ASSERT_NO_THROW(solver.solveEquationSystem(x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
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
    
    storm::solver::GmmxxLinearEquationSolver<double> solver(A);
    solver.getSettings().setSolutionMethod(storm::solver::GmmxxLinearEquationSolverSettings<double>::SolutionMethod::Gmres);
    solver.getSettings().setPrecision(1e-6);
    solver.getSettings().setMaximalNumberOfIterations(10000);
    solver.getSettings().setPreconditioner(storm::solver::GmmxxLinearEquationSolverSettings<double>::Preconditioner::Diagonal);
    solver.getSettings().setNumberOfIterationsUntilRestart(50);

    ASSERT_NO_THROW(solver.solveEquationSystem(x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
}

TEST(GmmxxLinearEquationSolver, MatrixVectorMultiplication) {
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
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
}