#include "gtest/gtest.h"
#include "storm-config.h"

#include "storm/solver/StandardMinMaxLinearEquationSolver.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GmmxxEquationSolverSettings.h"
#include "storm/storage/SparseMatrix.h"

TEST(GmmxxMinMaxLinearEquationSolver, SolveWithStandardOptions) {
    storm::storage::SparseMatrixBuilder<double> builder(0, 0, 0, false, true);
    ASSERT_NO_THROW(builder.newRowGroup(0));
    ASSERT_NO_THROW(builder.addNextValue(0, 0, 0.9));

    storm::storage::SparseMatrix<double> A;
    ASSERT_NO_THROW(A = builder.build(2));
    
    std::vector<double> x(1);
    std::vector<double> b = {0.099, 0.5};
    
    auto factory = storm::solver::GmmxxMinMaxLinearEquationSolverFactory<double>();
    auto solver = factory.create(A);
    solver->setHasUniqueSolution();
    ASSERT_NO_THROW(solver->solveEquations(storm::OptimizationDirection::Minimize, x, b));
    ASSERT_LT(std::abs(x[0] - 0.5), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    
    ASSERT_NO_THROW(solver->solveEquations(storm::OptimizationDirection::Maximize, x, b));
    ASSERT_LT(std::abs(x[0] - 0.989991), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
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
    
    auto factory = storm::solver::GmmxxMinMaxLinearEquationSolverFactory<double>();
    auto solver = factory.create(A);
    solver->setLowerBound(0.0);
    solver->setUpperBound(1.0);
    
    solver->setTerminationCondition(std::make_unique<storm::solver::TerminateIfFilteredExtremumExceedsThreshold<double>>(storm::storage::BitVector(A.getRowGroupCount(), true), false, bound, true));
    ASSERT_NO_THROW(solver->solveEquations(storm::OptimizationDirection::Minimize, x, b));
    ASSERT_LT(std::abs(x[0] - 0.5), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    
    ASSERT_NO_THROW(solver->solveEquations(storm::OptimizationDirection::Maximize, x, b));
    ASSERT_LT(std::abs(x[0] - 0.989991), 0.989991 - bound - storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    
    bound = 0.6;
    solver->setTerminationCondition(std::make_unique<storm::solver::TerminateIfFilteredExtremumExceedsThreshold<double>>(storm::storage::BitVector(A.getRowGroupCount(), true), false, bound, true));
    
    ASSERT_NO_THROW(solver->solveEquations(storm::OptimizationDirection::Maximize, x, b));
    ASSERT_LT(std::abs(x[0] - 0.989991), 0.989991 - bound - storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
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
    
    auto factory = storm::solver::GmmxxMinMaxLinearEquationSolverFactory<double>();
    auto solver = factory.create(A);
    
    ASSERT_NO_THROW(solver->repeatedMultiply(storm::OptimizationDirection::Minimize, x, nullptr, 1));
    ASSERT_LT(std::abs(x[0] - 0.099), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    
    x = {0, 1, 0};
    ASSERT_NO_THROW(solver->repeatedMultiply(storm::OptimizationDirection::Minimize, x, nullptr, 2));
    ASSERT_LT(std::abs(x[0] - 0.1881), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    
    x = {0, 1, 0};
    ASSERT_NO_THROW(solver->repeatedMultiply(storm::OptimizationDirection::Minimize, x, nullptr, 20));
    ASSERT_LT(std::abs(x[0] - 0.5), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    
    x = {0, 1, 0};
    ASSERT_NO_THROW(solver->repeatedMultiply(storm::OptimizationDirection::Maximize, x, nullptr, 1));
    ASSERT_LT(std::abs(x[0] - 0.5), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    
    x = {0, 1, 0};
    ASSERT_NO_THROW(solver->repeatedMultiply(storm::OptimizationDirection::Maximize, x, nullptr, 20));
    ASSERT_LT(std::abs(x[0] - 0.9238082658), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
}

TEST(GmmxxMinMaxLinearEquationSolver, SolveWithPolicyIteration) {
	storm::storage::SparseMatrixBuilder<double> builder(0, 0, 0, false, true);
	ASSERT_NO_THROW(builder.newRowGroup(0));
	ASSERT_NO_THROW(builder.addNextValue(0, 0, 0.9));

	storm::storage::SparseMatrix<double> A;
	ASSERT_NO_THROW(A = builder.build(2));

	std::vector<double> x(1);
	std::vector<double> b = { 0.099, 0.5 };

    auto factory = storm::solver::GmmxxMinMaxLinearEquationSolverFactory<double>(storm::solver::MinMaxMethodSelection::PolicyIteration);
    auto solver = factory.create(A);
    solver->setHasUniqueSolution();
    
	ASSERT_NO_THROW(solver->solveEquations(storm::OptimizationDirection::Minimize, x, b));
	ASSERT_LT(std::abs(x[0] - 0.5), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());

	ASSERT_NO_THROW(solver->solveEquations(storm::OptimizationDirection::Maximize, x, b));
	ASSERT_LT(std::abs(x[0] - 0.99), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
}
