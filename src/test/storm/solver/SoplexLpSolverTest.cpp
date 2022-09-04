#include "storm-config.h"
#include "test/storm_gtest.h"

#ifdef STORM_HAVE_SOPLEX
#include "storm/exceptions/InvalidAccessException.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/solver/SoplexLpSolver.h"
#include "storm/storage/expressions/Expressions.h"
#include "storm/storage/expressions/Variable.h"
#include "storm/utility/solver.h"

TEST(SoplexLpSolver, LPOptimizeMax) {
    auto solverPtr = storm::utility::solver::SoplexLpSolverFactory<double>().create("");
    auto& solver = static_cast<storm::solver::SoplexLpSolver<double>&>(*solverPtr);
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

TEST(SoplexLpSolver, LPOptimizeMaxExact) {
    auto solverPtr = storm::utility::solver::SoplexLpSolverFactory<storm::RationalNumber>().create("");
    auto& solver = static_cast<storm::solver::SoplexLpSolver<storm::RationalNumber>&>(*solverPtr);
    solver.setOptimizationDirection(storm::OptimizationDirection::Maximize);
    storm::expressions::Variable x;
    storm::expressions::Variable y;
    storm::expressions::Variable z;
    ASSERT_NO_THROW(x = solver.addBoundedContinuousVariable("x", 0, 1, -1));
    ASSERT_NO_THROW(y = solver.addLowerBoundedContinuousVariable("y", 0, 2));
    ASSERT_NO_THROW(z = solver.addLowerBoundedContinuousVariable("z", 0, 1));
    ASSERT_NO_THROW(solver.update());

    ASSERT_NO_THROW(solver.addConstraint("", x + y + z <= solver.getConstant(12)));
    ASSERT_NO_THROW(solver.addConstraint("", solver.getConstant(1) * y + solver.getConstant(2) * z - solver.getConstant(2) * x == solver.getConstant(10)));
    ASSERT_NO_THROW(solver.addConstraint("", solver.getConstant(2) * (y - x) <= solver.getConstant(11)));
    ASSERT_NO_THROW(solver.update());

    ASSERT_NO_THROW(solver.optimize());
    ASSERT_TRUE(solver.isOptimal());
    ASSERT_FALSE(solver.isUnbounded());
    ASSERT_FALSE(solver.isInfeasible());
    storm::RationalNumber xValue = 0;
    ASSERT_NO_THROW(xValue = solver.getContinuousValue(x));
    ASSERT_LT(std::abs(xValue - 1), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    storm::RationalNumber yValue = 0;
    ASSERT_NO_THROW(yValue = solver.getContinuousValue(y));
    ASSERT_LT(std::abs(yValue - 6.5), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    storm::RationalNumber zValue = 0;
    ASSERT_NO_THROW(zValue = solver.getContinuousValue(z));
    ASSERT_LT(std::abs(zValue - 2.75), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    storm::RationalNumber objectiveValue = 0;
    ASSERT_NO_THROW(objectiveValue = solver.getObjectiveValue());
    ASSERT_LT(std::abs(objectiveValue - 14.75), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
}

#endif
