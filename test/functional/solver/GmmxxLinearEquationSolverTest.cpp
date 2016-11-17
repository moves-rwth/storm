#include "gtest/gtest.h"
#include "storm-config.h"

#include "storm/solver/GmmxxLinearEquationSolver.h"
#include "storm/settings/SettingsManager.h"

#include "storm/settings/modules/GmmxxEquationSolverSettings.h"

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
    ASSERT_NO_THROW(solver.solveEquations(x, b));
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
    auto settings = solver.getSettings();
    settings.setSolutionMethod(storm::solver::GmmxxLinearEquationSolverSettings<double>::SolutionMethod::Gmres);
    settings.setPrecision(1e-6);
    settings.setMaximalNumberOfIterations(10000);
    settings.setPreconditioner(storm::solver::GmmxxLinearEquationSolverSettings<double>::Preconditioner::None);
    settings.setNumberOfIterationsUntilRestart(50);
    solver.setSettings(settings);
    
    ASSERT_NO_THROW(solver.solveEquations(x, b));
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
    auto settings = solver.getSettings();
    settings.setSolutionMethod(storm::solver::GmmxxLinearEquationSolverSettings<double>::SolutionMethod::Qmr);
    settings.setPrecision(1e-6);
    settings.setMaximalNumberOfIterations(10000);
    settings.setPreconditioner(storm::solver::GmmxxLinearEquationSolverSettings<double>::Preconditioner::None);
    solver.setSettings(settings);
    
    storm::solver::GmmxxLinearEquationSolver<double> solver2(A);
    auto settings2 = solver2.getSettings();
    settings2.setSolutionMethod(storm::solver::GmmxxLinearEquationSolverSettings<double>::SolutionMethod::Qmr);
    settings2.setPrecision(1e-6);
    settings2.setMaximalNumberOfIterations(10000);
    settings2.setPreconditioner(storm::solver::GmmxxLinearEquationSolverSettings<double>::Preconditioner::None);
    solver2.setSettings(settings2);
    
    ASSERT_NO_THROW(solver2.solveEquations(x, b));
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
    auto settings = solver.getSettings();
    settings.setSolutionMethod(storm::solver::GmmxxLinearEquationSolverSettings<double>::SolutionMethod::Bicgstab);
    settings.setPrecision(1e-6);
    settings.setMaximalNumberOfIterations(10000);
    settings.setPreconditioner(storm::solver::GmmxxLinearEquationSolverSettings<double>::Preconditioner::None);
    solver.setSettings(settings);
    
    ASSERT_NO_THROW(solver.solveEquations(x, b));
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
    auto settings = solver.getSettings();
    settings.setSolutionMethod(storm::solver::GmmxxLinearEquationSolverSettings<double>::SolutionMethod::Jacobi);
    settings.setPrecision(1e-6);
    settings.setMaximalNumberOfIterations(10000);
    settings.setPreconditioner(storm::solver::GmmxxLinearEquationSolverSettings<double>::Preconditioner::None);
    solver.setSettings(settings);
    ASSERT_NO_THROW(solver.solveEquations(x, b));
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
    auto settings = solver.getSettings();
    settings.setSolutionMethod(storm::solver::GmmxxLinearEquationSolverSettings<double>::SolutionMethod::Gmres);
    settings.setPrecision(1e-6);
    settings.setMaximalNumberOfIterations(10000);
    settings.setPreconditioner(storm::solver::GmmxxLinearEquationSolverSettings<double>::Preconditioner::Ilu);
    settings.setNumberOfIterationsUntilRestart(50);
    solver.setSettings(settings);
    ASSERT_NO_THROW(solver.solveEquations(x, b));
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
    auto settings = solver.getSettings();
    settings.setSolutionMethod(storm::solver::GmmxxLinearEquationSolverSettings<double>::SolutionMethod::Gmres);
    settings.setPrecision(1e-6);
    settings.setMaximalNumberOfIterations(10000);
    settings.setPreconditioner(storm::solver::GmmxxLinearEquationSolverSettings<double>::Preconditioner::Diagonal);
    settings.setNumberOfIterationsUntilRestart(50);
    solver.setSettings(settings);

    ASSERT_NO_THROW(solver.solveEquations(x, b));
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
    ASSERT_NO_THROW(solver.repeatedMultiply(x, nullptr, 4));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
}
