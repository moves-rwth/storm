#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/storage/SparseMatrix.h"

#include "src/utility/solver.h"
#include "src/settings/SettingsManager.h"

#include "src/solver/GameSolver.h"
#include "src/settings/modules/NativeEquationSolverSettings.h"

TEST(GameSolverTest, Solve) {
    // Construct simple game. Start with player 2 matrix.
    storm::storage::SparseMatrixBuilder<double> player2MatrixBuilder(0, 0, 0, false, true);
    player2MatrixBuilder.newRowGroup(0);
    player2MatrixBuilder.addNextValue(0, 0, 0.4);
    player2MatrixBuilder.addNextValue(0, 1, 0.6);
    player2MatrixBuilder.addNextValue(1, 1, 0.2);
    player2MatrixBuilder.addNextValue(1, 2, 0.8);
    player2MatrixBuilder.newRowGroup(2);
    player2MatrixBuilder.addNextValue(2, 2, 0.5);
    player2MatrixBuilder.addNextValue(2, 3, 0.5);
    player2MatrixBuilder.addNextValue(3, 0, 1);
    player2MatrixBuilder.newRowGroup(4);
    player2MatrixBuilder.newRowGroup(5);
    player2MatrixBuilder.newRowGroup(6);
    storm::storage::SparseMatrix<double> player2Matrix = player2MatrixBuilder.build();

    // Now build player 1 matrix.
    storm::storage::SparseMatrixBuilder<storm::storage::sparse::state_type> player1MatrixBuilder(0, 0, 0, false, true);
    player1MatrixBuilder.newRowGroup(0);
    player1MatrixBuilder.addNextValue(0, 0, 1);
    player1MatrixBuilder.addNextValue(1, 1, 1);
    player1MatrixBuilder.newRowGroup(2);
    player1MatrixBuilder.addNextValue(2, 2, 1);
    player1MatrixBuilder.newRowGroup(3);
    player1MatrixBuilder.addNextValue(3, 3, 1);
    player1MatrixBuilder.newRowGroup(4);
    player1MatrixBuilder.addNextValue(4, 4, 1);
    storm::storage::SparseMatrix<storm::storage::sparse::state_type> player1Matrix = player1MatrixBuilder.build();

    std::unique_ptr<storm::utility::solver::GameSolverFactory<double>> solverFactory(new storm::utility::solver::GameSolverFactory<double>());
    std::unique_ptr<storm::solver::GameSolver<double>> solver = solverFactory->create(player1Matrix, player2Matrix);

    // Create solution and target state vector.
    std::vector<double> result(4);
    std::vector<double> b(7);
    b[4] = 1;
    b[6] = 1;

    // Now solve the game with different strategies for the players.
    solver->solveGame(storm::OptimizationDirection::Minimize, storm::OptimizationDirection::Minimize, result, b);
    EXPECT_NEAR(0, result[0], storm::settings::nativeEquationSolverSettings().getPrecision());

    result = std::vector<double>(4);

    solver->solveGame(storm::OptimizationDirection::Minimize, storm::OptimizationDirection::Maximize, result, b);
    EXPECT_NEAR(0.5, result[0], storm::settings::nativeEquationSolverSettings().getPrecision());

    result = std::vector<double>(4);

    solver->solveGame(storm::OptimizationDirection::Maximize, storm::OptimizationDirection::Minimize, result, b);
    EXPECT_NEAR(0.2, result[0], storm::settings::nativeEquationSolverSettings().getPrecision());

    result = std::vector<double>(4);

    solver->solveGame(storm::OptimizationDirection::Maximize, storm::OptimizationDirection::Maximize, result, b);
    EXPECT_NEAR(0.99999892625817599, result[0], storm::settings::nativeEquationSolverSettings().getPrecision());
}