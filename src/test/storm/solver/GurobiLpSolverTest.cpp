#include "storm-config.h"
#include "test/storm_gtest.h"

#ifdef STORM_HAVE_GUROBI
#include "storm/exceptions/InvalidAccessException.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/solver/GurobiLpSolver.h"
#include "storm/storage/expressions/Expressions.h"
#include "storm/storage/expressions/Variable.h"
#include "storm/utility/solver.h"

TEST(GurobiLpSolver, LPOptimizeMax) {
    auto solverPtr = storm::utility::solver::GurobiLpSolverFactory<double>().create("");
    auto& solver = static_cast<storm::solver::GurobiLpSolver<double>&>(*solverPtr);
    solver.setOptimizationDirection(storm::OptimizationDirection::Maximize);
    storm::expressions::Variable x;
    storm::expressions::Variable y;
    storm::expressions::Variable z;
    ASSERT_NO_THROW(x = solver.addBoundedContinuousVariable("x", 0, 1, -1));
    ASSERT_NO_THROW(y = solver.addLowerBoundedContinuousVariable("y", 0, 2));
    ASSERT_NO_THROW(z = solver.addLowerBoundedContinuousVariable("z", 0, 1));
    ASSERT_NO_THROW(solver.update());

    ASSERT_NO_THROW(solver.addConstraint("", x + y + z <= solver.getConstant(12)));
    ASSERT_NO_THROW(solver.addConstraint("", solver.getConstant(0.5) * y + z - x == solver.getConstant(5)));
    ASSERT_NO_THROW(solver.addConstraint("", y - x <= solver.getConstant(5.5)));
    ASSERT_NO_THROW(solver.update());

    ASSERT_NO_THROW(solver.optimize());
    ASSERT_TRUE(solver.isOptimal());
    ASSERT_FALSE(solver.isUnbounded());
    ASSERT_FALSE(solver.isInfeasible());
    double xValue = 0;
    ASSERT_NO_THROW(xValue = solver.getContinuousValue(x));
    ASSERT_LT(std::abs(xValue - 1), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    double yValue = 0;
    ASSERT_NO_THROW(yValue = solver.getContinuousValue(y));
    ASSERT_LT(std::abs(yValue - 6.5), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    double zValue = 0;
    ASSERT_NO_THROW(zValue = solver.getContinuousValue(z));
    ASSERT_LT(std::abs(zValue - 2.75), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    double objectiveValue = 0;
    ASSERT_NO_THROW(objectiveValue = solver.getObjectiveValue());
    ASSERT_LT(std::abs(objectiveValue - 14.75), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
}

TEST(GurobiLpSolver, LPOptimizeMin) {
    auto solverPtr = storm::utility::solver::GurobiLpSolverFactory<double>().create("");
    auto& solver = static_cast<storm::solver::GurobiLpSolver<double>&>(*solverPtr);
    solver.setOptimizationDirection(storm::OptimizationDirection::Minimize);
    storm::expressions::Variable x;
    storm::expressions::Variable y;
    storm::expressions::Variable z;
    ASSERT_NO_THROW(x = solver.addBoundedContinuousVariable("x", 0, 1, -1));
    ASSERT_NO_THROW(y = solver.addLowerBoundedContinuousVariable("y", 0, 2));
    ASSERT_NO_THROW(z = solver.addBoundedContinuousVariable("z", 1, 5.7, -1));
    ASSERT_NO_THROW(solver.update());

    ASSERT_NO_THROW(solver.addConstraint("", x + y + z <= solver.getConstant(12)));
    ASSERT_NO_THROW(solver.addConstraint("", solver.getConstant(0.5) * y + z - x <= solver.getConstant(5)));
    ASSERT_NO_THROW(solver.addConstraint("", y - x <= solver.getConstant(5.5)));
    ASSERT_NO_THROW(solver.update());

    ASSERT_NO_THROW(solver.optimize());
    ASSERT_TRUE(solver.isOptimal());
    ASSERT_FALSE(solver.isUnbounded());
    ASSERT_FALSE(solver.isInfeasible());
    double xValue = 0;
    ASSERT_NO_THROW(xValue = solver.getContinuousValue(x));
    ASSERT_LT(std::abs(xValue - 1), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    double yValue = 0;
    ASSERT_NO_THROW(yValue = solver.getContinuousValue(y));
    ASSERT_LT(std::abs(yValue - 0), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    double zValue = 0;
    ASSERT_NO_THROW(zValue = solver.getContinuousValue(z));
    ASSERT_LT(std::abs(zValue - 5.7), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    double objectiveValue = 0;
    ASSERT_NO_THROW(objectiveValue = solver.getObjectiveValue());
    ASSERT_LT(std::abs(objectiveValue - (-6.7)), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
}

TEST(GurobiLpSolver, MILPOptimizeMax) {
    auto solverPtr = storm::utility::solver::GurobiLpSolverFactory<double>().create("");
    auto& solver = static_cast<storm::solver::GurobiLpSolver<double>&>(*solverPtr);
    solver.setOptimizationDirection(storm::OptimizationDirection::Maximize);
    storm::expressions::Variable x;
    storm::expressions::Variable y;
    storm::expressions::Variable z;
    ASSERT_NO_THROW(x = solver.addBinaryVariable("x", -1));
    ASSERT_NO_THROW(y = solver.addLowerBoundedIntegerVariable("y", 0, 2));
    ASSERT_NO_THROW(z = solver.addLowerBoundedContinuousVariable("z", 0, 1));
    ASSERT_NO_THROW(solver.update());

    ASSERT_NO_THROW(solver.addConstraint("", x + y + z <= solver.getConstant(12)));
    ASSERT_NO_THROW(solver.addConstraint("", solver.getConstant(0.5) * y + z - x == solver.getConstant(5)));
    ASSERT_NO_THROW(solver.addConstraint("", y - x <= solver.getConstant(5.5)));
    ASSERT_NO_THROW(solver.update());

    ASSERT_NO_THROW(solver.optimize());
    ASSERT_TRUE(solver.isOptimal());
    ASSERT_FALSE(solver.isUnbounded());
    ASSERT_FALSE(solver.isInfeasible());
    bool xValue = false;
    ASSERT_NO_THROW(xValue = solver.getBinaryValue(x));
    ASSERT_EQ(true, xValue);
    int_fast64_t yValue = 0;
    ASSERT_NO_THROW(yValue = solver.getIntegerValue(y));
    ASSERT_EQ(6, yValue);
    double zValue = 0;
    ASSERT_NO_THROW(zValue = solver.getContinuousValue(z));
    ASSERT_LT(std::abs(zValue - 3), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    double objectiveValue = 0;
    ASSERT_NO_THROW(objectiveValue = solver.getObjectiveValue());
    ASSERT_LT(std::abs(objectiveValue - 14), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
}

TEST(GurobiLpSolver, MILPOptimizeMin) {
    auto solverPtr = storm::utility::solver::GurobiLpSolverFactory<double>().create("");
    auto& solver = static_cast<storm::solver::GurobiLpSolver<double>&>(*solverPtr);
    solver.setOptimizationDirection(storm::OptimizationDirection::Minimize);
    storm::expressions::Variable x;
    storm::expressions::Variable y;
    storm::expressions::Variable z;
    ASSERT_NO_THROW(x = solver.addBinaryVariable("x", -1));
    ASSERT_NO_THROW(y = solver.addLowerBoundedIntegerVariable("y", 0, 2));
    ASSERT_NO_THROW(z = solver.addBoundedContinuousVariable("z", 0, 5, -1));
    ASSERT_NO_THROW(solver.update());

    ASSERT_NO_THROW(solver.addConstraint("", x + y + z <= solver.getConstant(12)));
    ASSERT_NO_THROW(solver.addConstraint("", solver.getConstant(0.5) * y + z - x <= solver.getConstant(5)));
    ASSERT_NO_THROW(solver.addConstraint("", y - x <= solver.getConstant(5.5)));
    ASSERT_NO_THROW(solver.update());

    ASSERT_NO_THROW(solver.optimize());
    ASSERT_TRUE(solver.isOptimal());
    ASSERT_FALSE(solver.isUnbounded());
    ASSERT_FALSE(solver.isInfeasible());
    bool xValue = false;
    ASSERT_NO_THROW(xValue = solver.getBinaryValue(x));
    ASSERT_EQ(true, xValue);
    int_fast64_t yValue = 0;
    ASSERT_NO_THROW(yValue = solver.getIntegerValue(y));
    ASSERT_EQ(0, yValue);
    double zValue = 0;
    ASSERT_NO_THROW(zValue = solver.getContinuousValue(z));
    ASSERT_LT(std::abs(zValue - 5), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    double objectiveValue = 0;
    ASSERT_NO_THROW(objectiveValue = solver.getObjectiveValue());
    ASSERT_LT(std::abs(objectiveValue - (-6)), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
}

TEST(GurobiLpSolver, LPInfeasible) {
    auto solverPtr = storm::utility::solver::GurobiLpSolverFactory<double>().create("");
    auto& solver = static_cast<storm::solver::GurobiLpSolver<double>&>(*solverPtr);
    solver.setOptimizationDirection(storm::OptimizationDirection::Maximize);
    storm::expressions::Variable x;
    storm::expressions::Variable y;
    storm::expressions::Variable z;
    ASSERT_NO_THROW(x = solver.addBoundedContinuousVariable("x", 0, 1, -1));
    ASSERT_NO_THROW(y = solver.addLowerBoundedContinuousVariable("y", 0, 2));
    ASSERT_NO_THROW(z = solver.addLowerBoundedContinuousVariable("z", 0, 1));
    ASSERT_NO_THROW(solver.update());

    ASSERT_NO_THROW(solver.addConstraint("", x + y + z <= solver.getConstant(12)));
    ASSERT_NO_THROW(solver.addConstraint("", solver.getConstant(0.5) * y + z - x == solver.getConstant(5)));
    ASSERT_NO_THROW(solver.addConstraint("", y - x <= solver.getConstant(5.5)));
    ASSERT_NO_THROW(solver.addConstraint("", y > solver.getConstant(7)));
    ASSERT_NO_THROW(solver.update());

    ASSERT_NO_THROW(solver.optimize());
    ASSERT_FALSE(solver.isOptimal());
    ASSERT_FALSE(solver.isUnbounded());
    ASSERT_TRUE(solver.isInfeasible());
    double xValue = 0;
    STORM_SILENT_ASSERT_THROW(xValue = solver.getContinuousValue(x), storm::exceptions::InvalidAccessException);
    double yValue = 0;
    STORM_SILENT_ASSERT_THROW(yValue = solver.getContinuousValue(y), storm::exceptions::InvalidAccessException);
    double zValue = 0;
    STORM_SILENT_ASSERT_THROW(zValue = solver.getContinuousValue(z), storm::exceptions::InvalidAccessException);
    double objectiveValue = 0;
    STORM_SILENT_ASSERT_THROW(objectiveValue = solver.getObjectiveValue(), storm::exceptions::InvalidAccessException);
}

TEST(GurobiLpSolver, MILPInfeasible) {
    auto solverPtr = storm::utility::solver::GurobiLpSolverFactory<double>().create("");
    auto& solver = static_cast<storm::solver::GurobiLpSolver<double>&>(*solverPtr);
    solver.setOptimizationDirection(storm::OptimizationDirection::Maximize);
    storm::expressions::Variable x;
    storm::expressions::Variable y;
    storm::expressions::Variable z;
    ASSERT_NO_THROW(x = solver.addBoundedContinuousVariable("x", 0, 1, -1));
    ASSERT_NO_THROW(y = solver.addLowerBoundedContinuousVariable("y", 0, 2));
    ASSERT_NO_THROW(z = solver.addLowerBoundedContinuousVariable("z", 0, 1));
    ASSERT_NO_THROW(solver.update());

    ASSERT_NO_THROW(solver.addConstraint("", x + y + z <= solver.getConstant(12)));
    ASSERT_NO_THROW(solver.addConstraint("", solver.getConstant(0.5) * y + z - x == solver.getConstant(5)));
    ASSERT_NO_THROW(solver.addConstraint("", y - x <= solver.getConstant(5.5)));
    ASSERT_NO_THROW(solver.addConstraint("", y > solver.getConstant(7)));
    ASSERT_NO_THROW(solver.update());

    ASSERT_NO_THROW(solver.optimize());
    ASSERT_FALSE(solver.isOptimal());
    ASSERT_FALSE(solver.isUnbounded());
    ASSERT_TRUE(solver.isInfeasible());
    double xValue = 0;
    STORM_SILENT_ASSERT_THROW(xValue = solver.getContinuousValue(x), storm::exceptions::InvalidAccessException);
    double yValue = 0;
    STORM_SILENT_ASSERT_THROW(yValue = solver.getContinuousValue(y), storm::exceptions::InvalidAccessException);
    double zValue = 0;
    STORM_SILENT_ASSERT_THROW(zValue = solver.getContinuousValue(z), storm::exceptions::InvalidAccessException);
    double objectiveValue = 0;
    STORM_SILENT_ASSERT_THROW(objectiveValue = solver.getObjectiveValue(), storm::exceptions::InvalidAccessException);
}

TEST(GurobiLpSolver, LPUnbounded) {
    auto solverPtr = storm::utility::solver::GurobiLpSolverFactory<double>().create("");
    auto& solver = static_cast<storm::solver::GurobiLpSolver<double>&>(*solverPtr);
    solver.setOptimizationDirection(storm::OptimizationDirection::Maximize);
    storm::expressions::Variable x;
    storm::expressions::Variable y;
    storm::expressions::Variable z;
    ASSERT_NO_THROW(x = solver.addBoundedContinuousVariable("x", 0, 1, -1));
    ASSERT_NO_THROW(y = solver.addLowerBoundedContinuousVariable("y", 0, 2));
    ASSERT_NO_THROW(z = solver.addLowerBoundedContinuousVariable("z", 0, 1));
    ASSERT_NO_THROW(solver.update());

    ASSERT_NO_THROW(solver.addConstraint("", x + y - z <= solver.getConstant(12)));
    ASSERT_NO_THROW(solver.addConstraint("", y - x <= solver.getConstant(5.5)));
    ASSERT_NO_THROW(solver.update());

    ASSERT_NO_THROW(solver.optimize());
    ASSERT_FALSE(solver.isOptimal());
    ASSERT_TRUE(solver.isUnbounded());
    ASSERT_FALSE(solver.isInfeasible());
    double xValue = 0;
    STORM_SILENT_ASSERT_THROW(xValue = solver.getContinuousValue(x), storm::exceptions::InvalidAccessException);
    double yValue = 0;
    STORM_SILENT_ASSERT_THROW(yValue = solver.getContinuousValue(y), storm::exceptions::InvalidAccessException);
    double zValue = 0;
    STORM_SILENT_ASSERT_THROW(zValue = solver.getContinuousValue(z), storm::exceptions::InvalidAccessException);
    double objectiveValue = 0;
    STORM_SILENT_ASSERT_THROW(objectiveValue = solver.getObjectiveValue(), storm::exceptions::InvalidAccessException);
}

TEST(GurobiLpSolver, MILPUnbounded) {
    auto solverPtr = storm::utility::solver::GurobiLpSolverFactory<double>().create("");
    auto& solver = static_cast<storm::solver::GurobiLpSolver<double>&>(*solverPtr);
    solver.setOptimizationDirection(storm::OptimizationDirection::Maximize);
    storm::expressions::Variable x;
    storm::expressions::Variable y;
    storm::expressions::Variable z;
    ASSERT_NO_THROW(x = solver.addBinaryVariable("x", -1));
    ASSERT_NO_THROW(y = solver.addLowerBoundedContinuousVariable("y", 0, 2));
    ASSERT_NO_THROW(z = solver.addLowerBoundedContinuousVariable("z", 0, 1));
    ASSERT_NO_THROW(solver.update());

    ASSERT_NO_THROW(solver.addConstraint("", x + y - z <= solver.getConstant(12)));
    ASSERT_NO_THROW(solver.addConstraint("", y - x <= solver.getConstant(5.5)));
    ASSERT_NO_THROW(solver.update());

    ASSERT_NO_THROW(solver.optimize());
    ASSERT_FALSE(solver.isOptimal());
    ASSERT_TRUE(solver.isUnbounded());
    ASSERT_FALSE(solver.isInfeasible());
    bool xValue = false;
    STORM_SILENT_ASSERT_THROW(xValue = solver.getBinaryValue(x), storm::exceptions::InvalidAccessException);
    int_fast64_t yValue = 0;
    STORM_SILENT_ASSERT_THROW(yValue = solver.getIntegerValue(y), storm::exceptions::InvalidAccessException);
    double zValue = 0;
    STORM_SILENT_ASSERT_THROW(zValue = solver.getContinuousValue(z), storm::exceptions::InvalidAccessException);
    double objectiveValue = 0;
    STORM_SILENT_ASSERT_THROW(objectiveValue = solver.getObjectiveValue(), storm::exceptions::InvalidAccessException);
}

TEST(GurobiLpSolver, Incremental) {
    auto solverPtr = storm::utility::solver::GurobiLpSolverFactory<double>().create("");
    auto& solver = static_cast<storm::solver::GurobiLpSolver<double>&>(*solverPtr);
    solver.setOptimizationDirection(storm::OptimizationDirection::Maximize);
    storm::expressions::Variable x, y, z;
    ASSERT_NO_THROW(x = solver.addUnboundedContinuousVariable("x", 1));

    solver.push();
    ASSERT_NO_THROW(solver.addConstraint("", x <= solver.getConstant(12)));
    ASSERT_NO_THROW(solver.optimize());
    // max x s.t. x<=12
    ASSERT_TRUE(solver.isOptimal());
    EXPECT_EQ(12.0, solver.getContinuousValue(x));

    solver.push();
    ASSERT_NO_THROW(y = solver.addUnboundedContinuousVariable("y"));
    ASSERT_NO_THROW(solver.addConstraint("", y <= solver.getConstant(6)));
    ASSERT_NO_THROW(solver.addConstraint("", x <= y));
    // max x s.t. x<=12 and y <= 6 and x <= y
    ASSERT_NO_THROW(solver.optimize());
    ASSERT_TRUE(solver.isOptimal());
    EXPECT_EQ(6.0, solver.getContinuousValue(x));
    EXPECT_EQ(6.0, solver.getContinuousValue(y));

    solver.pop();
    ASSERT_NO_THROW(solver.optimize());
    // max x s.t. x<=12
    ASSERT_TRUE(solver.isOptimal());
    EXPECT_EQ(12.0, solver.getContinuousValue(x));

    solver.push();
    ASSERT_NO_THROW(y = solver.addUnboundedContinuousVariable("y", 10));
    ASSERT_NO_THROW(solver.addConstraint("", y <= solver.getConstant(20)));
    ASSERT_NO_THROW(solver.addConstraint("", y <= -x));
    // max x+10y s.t. x<=12 and y<=20 and y<=-x
    ASSERT_NO_THROW(solver.optimize());
    ASSERT_TRUE(solver.isOptimal());
    EXPECT_EQ(-20, solver.getContinuousValue(x));
    EXPECT_EQ(20, solver.getContinuousValue(y));

    solver.pop();
    ASSERT_NO_THROW(solver.optimize());
    // max x s.t. x<=12
    ASSERT_TRUE(solver.isOptimal());
    EXPECT_EQ(12.0, solver.getContinuousValue(x));

    solver.push();
    ASSERT_NO_THROW(z = solver.addUnboundedIntegerVariable("z"));
    ASSERT_NO_THROW(solver.addConstraint("", z <= solver.getConstant(6)));
    ASSERT_NO_THROW(solver.addConstraint("", x <= z));
    ASSERT_NO_THROW(solver.optimize());
    // max x s.t. x<=12 and z <= 6 and x <= z
    ASSERT_TRUE(solver.isOptimal());
    EXPECT_EQ(6.0, solver.getContinuousValue(x));
    EXPECT_EQ(6, solver.getIntegerValue(z));

    solver.pop();
    solver.pop();
    // max x s.t. true
    ASSERT_NO_THROW(solver.optimize());
    ASSERT_FALSE(solver.isOptimal());
    ASSERT_TRUE(solver.isUnbounded());
    ASSERT_FALSE(solver.isInfeasible());
}

#endif
