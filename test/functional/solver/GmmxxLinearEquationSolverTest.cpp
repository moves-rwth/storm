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
    
    ASSERT_NO_THROW(storm::solver::GmmxxLinearEquationSolver<double> solver);
    
    storm::solver::GmmxxLinearEquationSolver<double> solver;
    ASSERT_NO_THROW(solver.solveEquationSystem(A, x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::SettingsManager::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::SettingsManager::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::SettingsManager::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
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
    
    ASSERT_NO_THROW(storm::solver::GmmxxLinearEquationSolver<double> solver(storm::solver::GmmxxLinearEquationSolver<double>::GMRES, 1e-6, 10000, storm::solver::GmmxxLinearEquationSolver<double>::NONE, true, 50));
    
    storm::solver::GmmxxLinearEquationSolver<double> solver(storm::solver::GmmxxLinearEquationSolver<double>::GMRES, 1e-6, 10000, storm::solver::GmmxxLinearEquationSolver<double>::NONE, true, 50);
    ASSERT_NO_THROW(solver.solveEquationSystem(A, x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::SettingsManager::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::SettingsManager::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::SettingsManager::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
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
    
    ASSERT_NO_THROW(storm::solver::GmmxxLinearEquationSolver<double> solver(storm::solver::GmmxxLinearEquationSolver<double>::QMR, 1e-6, 10000, storm::solver::GmmxxLinearEquationSolver<double>::NONE));
    
    storm::solver::GmmxxLinearEquationSolver<double> solver(storm::solver::GmmxxLinearEquationSolver<double>::QMR, 1e-6, 10000, storm::solver::GmmxxLinearEquationSolver<double>::NONE, true, 50);
    ASSERT_NO_THROW(solver.solveEquationSystem(A, x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::SettingsManager::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::SettingsManager::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::SettingsManager::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
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
    
    ASSERT_NO_THROW(storm::solver::GmmxxLinearEquationSolver<double> solver(storm::solver::GmmxxLinearEquationSolver<double>::BICGSTAB, 1e-6, 10000, storm::solver::GmmxxLinearEquationSolver<double>::NONE));
    
    storm::solver::GmmxxLinearEquationSolver<double> solver(storm::solver::GmmxxLinearEquationSolver<double>::BICGSTAB, 1e-6, 10000, storm::solver::GmmxxLinearEquationSolver<double>::NONE);
    ASSERT_NO_THROW(solver.solveEquationSystem(A, x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::SettingsManager::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::SettingsManager::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::SettingsManager::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
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
    
    ASSERT_NO_THROW(storm::solver::GmmxxLinearEquationSolver<double> solver(storm::solver::GmmxxLinearEquationSolver<double>::JACOBI, 1e-6, 10000, storm::solver::GmmxxLinearEquationSolver<double>::NONE));
    
    storm::solver::GmmxxLinearEquationSolver<double> solver(storm::solver::GmmxxLinearEquationSolver<double>::JACOBI, 1e-6, 10000, storm::solver::GmmxxLinearEquationSolver<double>::NONE);
    ASSERT_NO_THROW(solver.solveEquationSystem(A, x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::SettingsManager::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::SettingsManager::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::SettingsManager::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
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
    
    ASSERT_NO_THROW(storm::solver::GmmxxLinearEquationSolver<double> solver(storm::solver::GmmxxLinearEquationSolver<double>::GMRES, 1e-6, 10000, storm::solver::GmmxxLinearEquationSolver<double>::ILU, true, 50));
    
    storm::solver::GmmxxLinearEquationSolver<double> solver(storm::solver::GmmxxLinearEquationSolver<double>::GMRES, 1e-6, 10000, storm::solver::GmmxxLinearEquationSolver<double>::NONE, true, 50);
    ASSERT_NO_THROW(solver.solveEquationSystem(A, x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::SettingsManager::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::SettingsManager::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::SettingsManager::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
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
    
    ASSERT_NO_THROW(storm::solver::GmmxxLinearEquationSolver<double> solver(storm::solver::GmmxxLinearEquationSolver<double>::GMRES, 1e-6, 10000, storm::solver::GmmxxLinearEquationSolver<double>::DIAGONAL, true, 50));
    
    storm::solver::GmmxxLinearEquationSolver<double> solver(storm::solver::GmmxxLinearEquationSolver<double>::GMRES, 1e-6, 10000, storm::solver::GmmxxLinearEquationSolver<double>::NONE, true, 50);
    ASSERT_NO_THROW(solver.solveEquationSystem(A, x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::SettingsManager::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::SettingsManager::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::SettingsManager::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
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
    
    storm::solver::GmmxxLinearEquationSolver<double> solver;
    ASSERT_NO_THROW(solver.performMatrixVectorMultiplication(A, x, nullptr, 4));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::SettingsManager::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
}