#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/solver/GmmxxMinMaxLinearEquationSolver.h"
#include "src/settings/SettingsManager.h"
#include "src/settings/modules/GmmxxEquationSolverSettings.h"
#include "src/storage/SparseMatrix.h"

TEST(GmmxxMinMaxLinearEquationSolver, SolveWithStandardOptions) {
    storm::storage::SparseMatrixBuilder<double> builder(0, 0, 0, false, true);
    ASSERT_NO_THROW(builder.newRowGroup(0));
    ASSERT_NO_THROW(builder.addNextValue(0, 0, 0.9));

    storm::storage::SparseMatrix<double> A;
    ASSERT_NO_THROW(A = builder.build(2));
    
    std::vector<double> x(1);
    std::vector<double> b = {0.099, 0.5};
        
    storm::solver::GmmxxMinMaxLinearEquationSolver<double> solver(A);
    ASSERT_NO_THROW(solver.solveEquationSystem(storm::OptimizationDirection::Minimize, x, b));
    ASSERT_LT(std::abs(x[0] - 0.5), storm::settings::gmmxxEquationSolverSettings().getPrecision());
    
    ASSERT_NO_THROW(solver.solveEquationSystem(storm::OptimizationDirection::Maximize, x, b));
    ASSERT_LT(std::abs(x[0] - 0.989991), storm::settings::gmmxxEquationSolverSettings().getPrecision());
}

// TODO add better tests here.
TEST(GmmxxMinMaxLinearEquationSolver, SolveWithStandardOptionsAndEarlyTermination) {
    storm::storage::SparseMatrixBuilder<double> builder(0, 0, 0, false, true);
    ASSERT_NO_THROW(builder.newRowGroup(0));
    ASSERT_NO_THROW(builder.addNextValue(0, 0, 0.9));

    storm::storage::SparseMatrix<double> A;
    ASSERT_NO_THROW(A = builder.build(2));
    
    std::vector<double> x(1);
    std::vector<double> b = {0.099, 0.5};
        
    double bound = 0.8;
    storm::solver::GmmxxMinMaxLinearEquationSolver<double> solver(A);
    solver.setEarlyTerminationCriterion(std::make_unique<storm::solver::TerminateAfterFilteredExtremumPassesThresholdValue<double>>(storm::storage::BitVector(1, true), bound, true));
    ASSERT_NO_THROW(solver.solveEquationSystem(storm::OptimizationDirection::Minimize, x, b));
    ASSERT_LT(std::abs(x[0] - 0.5), storm::settings::gmmxxEquationSolverSettings().getPrecision());
    
    ASSERT_NO_THROW(solver.solveEquationSystem(storm::OptimizationDirection::Maximize, x, b));
    ASSERT_LT(std::abs(x[0] - 0.989991), 0.989991 - bound - storm::settings::gmmxxEquationSolverSettings().getPrecision());
    
    bound = 0.6;
    solver.setEarlyTerminationCriterion(std::make_unique<storm::solver::TerminateAfterFilteredExtremumPassesThresholdValue<double>>(storm::storage::BitVector(1, true), bound, true));
    
    ASSERT_NO_THROW(solver.solveEquationSystem(storm::OptimizationDirection::Maximize, x, b));
    ASSERT_LT(std::abs(x[0] - 0.989991), 0.989991 - bound - storm::settings::gmmxxEquationSolverSettings().getPrecision());
    
}

TEST(GmmxxMinMaxLinearEquationSolver, MatrixVectorMultiplication) {
    storm::storage::SparseMatrixBuilder<double> builder(0, 0, 0, false, true);
    ASSERT_NO_THROW(builder.newRowGroup(0));
    ASSERT_NO_THROW(builder.addNextValue(0, 0, 0.9));
    ASSERT_NO_THROW(builder.addNextValue(0, 1, 0.099));
    ASSERT_NO_THROW(builder.addNextValue(0, 2, 0.001));
    ASSERT_NO_THROW(builder.addNextValue(1, 1, 0.5));
    ASSERT_NO_THROW(builder.addNextValue(1, 2, 0.5));
    ASSERT_NO_THROW(builder.newRowGroup(2));
    ASSERT_NO_THROW(builder.addNextValue(2, 1, 1));
    ASSERT_NO_THROW(builder.newRowGroup(3));
    ASSERT_NO_THROW(builder.addNextValue(3, 2, 1));
    
    storm::storage::SparseMatrix<double> A;
    ASSERT_NO_THROW(A = builder.build());
    
    std::vector<double> x = {0, 1, 0};
    
    ASSERT_NO_THROW(storm::solver::GmmxxMinMaxLinearEquationSolver<double> solver(A));
    
    storm::solver::GmmxxMinMaxLinearEquationSolver<double> solver(A);
    ASSERT_NO_THROW(solver.performMatrixVectorMultiplication(storm::OptimizationDirection::Minimize, x, nullptr, 1));
    ASSERT_LT(std::abs(x[0] - 0.099), storm::settings::gmmxxEquationSolverSettings().getPrecision());
    
    x = {0, 1, 0};
    ASSERT_NO_THROW(solver.performMatrixVectorMultiplication(storm::OptimizationDirection::Minimize, x, nullptr, 2));
    ASSERT_LT(std::abs(x[0] - 0.1881), storm::settings::gmmxxEquationSolverSettings().getPrecision());
    
    x = {0, 1, 0};
    ASSERT_NO_THROW(solver.performMatrixVectorMultiplication(storm::OptimizationDirection::Minimize, x, nullptr, 20));
    ASSERT_LT(std::abs(x[0] - 0.5), storm::settings::gmmxxEquationSolverSettings().getPrecision());
    
    x = {0, 1, 0};
    ASSERT_NO_THROW(solver.performMatrixVectorMultiplication(storm::OptimizationDirection::Maximize, x, nullptr, 1));
    ASSERT_LT(std::abs(x[0] - 0.5), storm::settings::gmmxxEquationSolverSettings().getPrecision());
    
    x = {0, 1, 0};
    ASSERT_NO_THROW(solver.performMatrixVectorMultiplication(storm::OptimizationDirection::Maximize, x, nullptr, 20));
    ASSERT_LT(std::abs(x[0] - 0.9238082658), storm::settings::gmmxxEquationSolverSettings().getPrecision());
}

TEST(GmmxxMinMaxLinearEquationSolver, SolveWithPolicyIteration) {
	storm::storage::SparseMatrixBuilder<double> builder(0, 0, 0, false, true);
	ASSERT_NO_THROW(builder.newRowGroup(0));
	ASSERT_NO_THROW(builder.addNextValue(0, 0, 0.9));

	storm::storage::SparseMatrix<double> A;
	ASSERT_NO_THROW(A = builder.build(2));

	std::vector<double> x(1);
	std::vector<double> b = { 0.099, 0.5 };

	storm::settings::modules::GmmxxEquationSolverSettings const& settings = storm::settings::gmmxxEquationSolverSettings();
	storm::solver::GmmxxMinMaxLinearEquationSolver<double> solver(A, settings.getPrecision(), settings.getMaximalIterationCount(), storm::solver::MinMaxTechniqueSelection::PolicyIteration, settings.getConvergenceCriterion() == storm::settings::modules::GmmxxEquationSolverSettings::ConvergenceCriterion::Relative);
	ASSERT_NO_THROW(solver.solveEquationSystem(storm::OptimizationDirection::Minimize, x, b));
	ASSERT_LT(std::abs(x[0] - 0.5), storm::settings::gmmxxEquationSolverSettings().getPrecision());

	ASSERT_NO_THROW(solver.solveEquationSystem(storm::OptimizationDirection::Maximize, x, b));
	ASSERT_LT(std::abs(x[0] - 0.99), storm::settings::gmmxxEquationSolverSettings().getPrecision());
}