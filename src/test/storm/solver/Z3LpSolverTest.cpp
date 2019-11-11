#include "test/storm_gtest.h"
#include "storm-config.h"

#ifdef STORM_HAVE_Z3_OPTIMIZE
#include "storm/storage/expressions/Variable.h"
#include "storm/solver/Z3LpSolver.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/InvalidAccessException.h"
#include "storm/settings/SettingsManager.h"

#include "storm/settings/modules/GeneralSettings.h"

#include "storm/storage/expressions/Expressions.h"
#include "storm/solver/OptimizationDirection.h"

#include <cmath>

// Early versions sometimes have buggy features. We therefore might disable some tests.
#include <z3.h>
bool z3AtLeastVersion(unsigned expectedMajor, unsigned expectedMinor, unsigned expectedBuildNumber) {
    std::vector<unsigned> actual(4), expected({expectedMajor, expectedMinor, expectedBuildNumber, 0u});
    Z3_get_version(&actual[0], &actual[1], &actual[2], &actual[3]);
    for (uint64_t i = 0; i < 4; ++i) {
        if (actual[i] > expected[i]) {
            return true;
        }
        if (actual[i] < expected[i]) {
            return false;
        }
    }
    return true; // Equal versions
}

TEST(Z3LpSolver, LPOptimizeMax) {
    storm::solver::Z3LpSolver<double> solver(storm::OptimizationDirection::Maximize);
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
    ASSERT_LT(std::fabs(xValue - 1), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    double yValue = 0;
    ASSERT_NO_THROW(yValue = solver.getContinuousValue(y));
    ASSERT_LT(std::fabs(yValue - 6.5), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    double zValue = 0;
    ASSERT_NO_THROW(zValue = solver.getContinuousValue(z));
    ASSERT_LT(std::fabs(zValue - 2.75), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    double objectiveValue = 0;
    ASSERT_NO_THROW(objectiveValue = solver.getObjectiveValue());
    ASSERT_LT(std::fabs(objectiveValue - 14.75), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
}

TEST(Z3LpSolver, LPOptimizeMin) {
    storm::solver::Z3LpSolver<double> solver(storm::OptimizationDirection::Minimize);
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
    ASSERT_LT(std::fabs(xValue - 1), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    double yValue = 0;
    ASSERT_NO_THROW(yValue = solver.getContinuousValue(y));
    ASSERT_LT(std::fabs(yValue - 0), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    double zValue = 0;
    ASSERT_NO_THROW(zValue = solver.getContinuousValue(z));
    ASSERT_LT(std::fabs(zValue - 5.7), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    double objectiveValue = 0;
    ASSERT_NO_THROW(objectiveValue = solver.getObjectiveValue());
    ASSERT_LT(std::fabs(objectiveValue - (-6.7)), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
}

TEST(Z3LpSolver, MILPOptimizeMax) {
    storm::solver::Z3LpSolver<double> solver(storm::OptimizationDirection::Maximize);
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
    ASSERT_LT(std::fabs(zValue - 3), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    double objectiveValue = 0;
    ASSERT_NO_THROW(objectiveValue = solver.getObjectiveValue());
    ASSERT_LT(std::fabs(objectiveValue - 14), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
}

TEST(Z3LpSolver, MILPOptimizeMin) {
    storm::solver::Z3LpSolver<double> solver(storm::OptimizationDirection::Minimize);
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
    ASSERT_LT(std::fabs(zValue - 5), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    double objectiveValue = 0;
    ASSERT_NO_THROW(objectiveValue = solver.getObjectiveValue());
    ASSERT_LT(std::fabs(objectiveValue - (-6)), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
}

TEST(Z3LpSolver, LPInfeasible) {
    storm::solver::Z3LpSolver<double> solver(storm::OptimizationDirection::Maximize);
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
    STORM_SILENT_ASSERT_THROW(solver.getContinuousValue(x), storm::exceptions::InvalidAccessException);
    STORM_SILENT_ASSERT_THROW(solver.getContinuousValue(y), storm::exceptions::InvalidAccessException);
    STORM_SILENT_ASSERT_THROW(solver.getContinuousValue(z), storm::exceptions::InvalidAccessException);
    STORM_SILENT_ASSERT_THROW(solver.getObjectiveValue(), storm::exceptions::InvalidAccessException);
}

TEST(Z3LpSolver, MILPInfeasible) {
    storm::solver::Z3LpSolver<double> solver(storm::OptimizationDirection::Maximize);
    storm::expressions::Variable x;
    storm::expressions::Variable y;
    storm::expressions::Variable z;
    ASSERT_NO_THROW(x = solver.addBinaryVariable("x", -1));
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
    STORM_SILENT_ASSERT_THROW(solver.getBinaryValue(x), storm::exceptions::InvalidAccessException);
    STORM_SILENT_ASSERT_THROW(solver.getIntegerValue(y), storm::exceptions::InvalidAccessException);
    STORM_SILENT_ASSERT_THROW(solver.getContinuousValue(z), storm::exceptions::InvalidAccessException);
    STORM_SILENT_ASSERT_THROW(solver.getObjectiveValue(), storm::exceptions::InvalidAccessException);
}

TEST(Z3LpSolver, LPUnbounded) {
    storm::solver::Z3LpSolver<double> solver(storm::OptimizationDirection::Maximize);
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
    STORM_SILENT_ASSERT_THROW(solver.getContinuousValue(x), storm::exceptions::InvalidAccessException);
    STORM_SILENT_ASSERT_THROW(solver.getContinuousValue(y), storm::exceptions::InvalidAccessException);
    STORM_SILENT_ASSERT_THROW(solver.getContinuousValue(z), storm::exceptions::InvalidAccessException);
    STORM_SILENT_ASSERT_THROW(solver.getObjectiveValue(), storm::exceptions::InvalidAccessException);
}

TEST(Z3LpSolver, MILPUnbounded) {
    storm::solver::Z3LpSolver<double> solver(storm::OptimizationDirection::Maximize);
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
    STORM_SILENT_ASSERT_THROW(solver.getBinaryValue(x), storm::exceptions::InvalidAccessException);
    STORM_SILENT_ASSERT_THROW(solver.getIntegerValue(y), storm::exceptions::InvalidAccessException);
    STORM_SILENT_ASSERT_THROW(solver.getContinuousValue(z), storm::exceptions::InvalidAccessException);
    STORM_SILENT_ASSERT_THROW(solver.getObjectiveValue(), storm::exceptions::InvalidAccessException);
}

TEST(Z3LpSolver, Incremental) {
    storm::solver::Z3LpSolver<double> solver(storm::OptimizationDirection::Maximize);
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
    
    
    if (!z3AtLeastVersion(4,8,5)) {
        GTEST_SKIP() << "Test disabled since the z3 version is too old.";
    }
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
    ASSERT_FALSE(solver.isInfeasible());
    ASSERT_FALSE(solver.isUnbounded());
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
