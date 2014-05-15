#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/solver/GurobiLpSolver.h"
#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/InvalidAccessException.h"
#include "src/settings/Settings.h"

TEST(GurobiLpSolver, LPOptimizeMax) {
#ifdef STORM_HAVE_GUROBI
    storm::solver::GurobiLpSolver solver(storm::solver::LpSolver::ModelSense::Maximize);
    ASSERT_NO_THROW(solver.addBoundedContinuousVariable("x", 0, 1, -1));
    ASSERT_NO_THROW(solver.addLowerBoundedContinuousVariable("y", 0, 2));
    ASSERT_NO_THROW(solver.addLowerBoundedContinuousVariable("z", 0, 1));
    ASSERT_NO_THROW(solver.update());
    
    ASSERT_NO_THROW(solver.addConstraint("", storm::expressions::Expression::createDoubleVariable("x") + storm::expressions::Expression::createDoubleVariable("y") + storm::expressions::Expression::createDoubleVariable("z") <= storm::expressions::Expression::createDoubleLiteral(12)));
    ASSERT_NO_THROW(solver.addConstraint("", storm::expressions::Expression::createDoubleLiteral(0.5) * storm::expressions::Expression::createDoubleVariable("y") + storm::expressions::Expression::createDoubleVariable("z") - storm::expressions::Expression::createDoubleVariable("x") == storm::expressions::Expression::createDoubleLiteral(5)));
    ASSERT_NO_THROW(solver.addConstraint("", storm::expressions::Expression::createDoubleVariable("y") - storm::expressions::Expression::createDoubleVariable("x") <= storm::expressions::Expression::createDoubleLiteral(5.5)));
    ASSERT_NO_THROW(solver.update());

    ASSERT_NO_THROW(solver.optimize());
    ASSERT_TRUE(solver.isOptimal());
    ASSERT_FALSE(solver.isUnbounded());
    ASSERT_FALSE(solver.isInfeasible());
    double xValue = 0;
    ASSERT_NO_THROW(xValue = solver.getContinuousValue("x"));
    ASSERT_LT(std::abs(xValue - 1), storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    double yValue = 0;
    ASSERT_NO_THROW(yValue = solver.getContinuousValue("y"));
    ASSERT_LT(std::abs(yValue - 6.5), storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    double zValue = 0;
    ASSERT_NO_THROW(zValue = solver.getContinuousValue("z"));
    ASSERT_LT(std::abs(zValue - 2.75), storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    double objectiveValue = 0;
    ASSERT_NO_THROW(objectiveValue = solver.getObjectiveValue());
    ASSERT_LT(std::abs(objectiveValue - 14.75), storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
#else
    ASSERT_TRUE(false) << "StoRM built without Gurobi support.";
#endif
}

TEST(GurobiLpSolver, LPOptimizeMin) {
#ifdef STORM_HAVE_GUROBI
    storm::solver::GurobiLpSolver solver(storm::solver::LpSolver::ModelSense::Minimize);
    ASSERT_NO_THROW(solver.addBoundedContinuousVariable("x", 0, 1, -1));
    ASSERT_NO_THROW(solver.addLowerBoundedIntegerVariable("y", 0, 2));
    ASSERT_NO_THROW(solver.addBoundedContinuousVariable("z", 1, 5.7, -1));
    ASSERT_NO_THROW(solver.update());

    ASSERT_NO_THROW(solver.addConstraint("", storm::expressions::Expression::createDoubleVariable("x") + storm::expressions::Expression::createDoubleVariable("y") + storm::expressions::Expression::createDoubleVariable("z") <= storm::expressions::Expression::createDoubleLiteral(12)));
    ASSERT_NO_THROW(solver.addConstraint("", storm::expressions::Expression::createDoubleLiteral(0.5) * storm::expressions::Expression::createDoubleVariable("y") + storm::expressions::Expression::createDoubleVariable("z") - storm::expressions::Expression::createDoubleVariable("x") <= storm::expressions::Expression::createDoubleLiteral(5)));
    ASSERT_NO_THROW(solver.addConstraint("", storm::expressions::Expression::createDoubleVariable("y") - storm::expressions::Expression::createDoubleVariable("x") <= storm::expressions::Expression::createDoubleLiteral(5.5)));
    ASSERT_NO_THROW(solver.update());

    ASSERT_NO_THROW(solver.optimize());
    ASSERT_TRUE(solver.isOptimal());
    ASSERT_FALSE(solver.isUnbounded());
    ASSERT_FALSE(solver.isInfeasible());
    double xValue = 0;
    ASSERT_NO_THROW(xValue = solver.getContinuousValue("x"));
    ASSERT_LT(std::abs(xValue - 1), storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    double yValue = 0;
    ASSERT_NO_THROW(yValue = solver.getContinuousValue("y"));
    ASSERT_LT(std::abs(yValue - 0), storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    double zValue = 0;
    ASSERT_NO_THROW(zValue = solver.getContinuousValue("z"));
    ASSERT_LT(std::abs(zValue - 5.7), storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    double objectiveValue = 0;
    ASSERT_NO_THROW(objectiveValue = solver.getObjectiveValue());
    ASSERT_LT(std::abs(objectiveValue - (-6.7)), storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
#else
    ASSERT_TRUE(false) << "StoRM built without Gurobi support.";
#endif
}

TEST(GurobiLpSolver, MILPOptimizeMax) {
#ifdef STORM_HAVE_GUROBI
    storm::solver::GurobiLpSolver solver(storm::solver::LpSolver::ModelSense::Maximize);
    ASSERT_NO_THROW(solver.addBinaryVariable("x", -1));
    ASSERT_NO_THROW(solver.addLowerBoundedIntegerVariable("y", 0, 2));
    ASSERT_NO_THROW(solver.addLowerBoundedContinuousVariable("z", 0, 1));
    ASSERT_NO_THROW(solver.update());

    ASSERT_NO_THROW(solver.addConstraint("", storm::expressions::Expression::createDoubleVariable("x") + storm::expressions::Expression::createDoubleVariable("y") + storm::expressions::Expression::createDoubleVariable("z") <= storm::expressions::Expression::createDoubleLiteral(12)));
    ASSERT_NO_THROW(solver.addConstraint("", storm::expressions::Expression::createDoubleLiteral(0.5) * storm::expressions::Expression::createDoubleVariable("y") + storm::expressions::Expression::createDoubleVariable("z") - storm::expressions::Expression::createDoubleVariable("x") == storm::expressions::Expression::createDoubleLiteral(5)));
    ASSERT_NO_THROW(solver.addConstraint("", storm::expressions::Expression::createDoubleVariable("y") - storm::expressions::Expression::createDoubleVariable("x") <= storm::expressions::Expression::createDoubleLiteral(5.5)));
    ASSERT_NO_THROW(solver.update());

    ASSERT_NO_THROW(solver.optimize());
    ASSERT_TRUE(solver.isOptimal());
    ASSERT_FALSE(solver.isUnbounded());
    ASSERT_FALSE(solver.isInfeasible());
    bool xValue = false;
    ASSERT_NO_THROW(xValue = solver.getBinaryValue("x"));
    ASSERT_EQ(true, xValue);
    int_fast64_t yValue = 0;
    ASSERT_NO_THROW(yValue = solver.getIntegerValue("y"));
    ASSERT_EQ(6, yValue);
    double zValue = 0;
    ASSERT_NO_THROW(zValue = solver.getContinuousValue("z"));
    ASSERT_LT(std::abs(zValue - 3), storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    double objectiveValue = 0;
    ASSERT_NO_THROW(objectiveValue = solver.getObjectiveValue());
    ASSERT_LT(std::abs(objectiveValue - 14), storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
#else
    ASSERT_TRUE(false) << "StoRM built without Gurobi support.";
#endif
}

TEST(GurobiLpSolver, MILPOptimizeMin) {
#ifdef STORM_HAVE_GUROBI
    storm::solver::GurobiLpSolver solver(storm::solver::LpSolver::ModelSense::Minimize);
    ASSERT_NO_THROW(solver.addBinaryVariable("x", -1));
    ASSERT_NO_THROW(solver.addLowerBoundedIntegerVariable("y", 0, 2));
    ASSERT_NO_THROW(solver.addBoundedContinuousVariable("z", 0, 5, -1));
    ASSERT_NO_THROW(solver.update());

    ASSERT_NO_THROW(solver.addConstraint("", storm::expressions::Expression::createDoubleVariable("x") + storm::expressions::Expression::createDoubleVariable("y") + storm::expressions::Expression::createDoubleVariable("z") <= storm::expressions::Expression::createDoubleLiteral(12)));
    ASSERT_NO_THROW(solver.addConstraint("", storm::expressions::Expression::createDoubleLiteral(0.5) * storm::expressions::Expression::createDoubleVariable("y") + storm::expressions::Expression::createDoubleVariable("z") - storm::expressions::Expression::createDoubleVariable("x") <= storm::expressions::Expression::createDoubleLiteral(5)));
    ASSERT_NO_THROW(solver.addConstraint("", storm::expressions::Expression::createDoubleVariable("y") - storm::expressions::Expression::createDoubleVariable("x") <= storm::expressions::Expression::createDoubleLiteral(5.5)));
    ASSERT_NO_THROW(solver.update());

    ASSERT_NO_THROW(solver.optimize());
    ASSERT_TRUE(solver.isOptimal());
    ASSERT_FALSE(solver.isUnbounded());
    ASSERT_FALSE(solver.isInfeasible());
    bool xValue = false;
    ASSERT_NO_THROW(xValue = solver.getBinaryValue("x"));
    ASSERT_EQ(true, xValue);
    int_fast64_t yValue = 0;
    ASSERT_NO_THROW(yValue = solver.getIntegerValue("y"));
    ASSERT_EQ(0, yValue);
    double zValue = 0;
    ASSERT_NO_THROW(zValue = solver.getContinuousValue("z"));
    ASSERT_LT(std::abs(zValue - 5), storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    double objectiveValue = 0;
    ASSERT_NO_THROW(objectiveValue = solver.getObjectiveValue());
    ASSERT_LT(std::abs(objectiveValue - (-6)), storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
#else
    ASSERT_TRUE(false) << "StoRM built without Gurobi support.";
#endif
}

TEST(GurobiLpSolver, LPInfeasible) {
#ifdef STORM_HAVE_GUROBI
    storm::solver::GurobiLpSolver solver(storm::solver::LpSolver::ModelSense::Maximize);
    ASSERT_NO_THROW(solver.addBoundedContinuousVariable("x", 0, 1, -1));
    ASSERT_NO_THROW(solver.addLowerBoundedContinuousVariable("y", 0, 2));
    ASSERT_NO_THROW(solver.addLowerBoundedContinuousVariable("z", 0, 1));
    ASSERT_NO_THROW(solver.update());

    ASSERT_NO_THROW(solver.addConstraint("", storm::expressions::Expression::createDoubleVariable("x") + storm::expressions::Expression::createDoubleVariable("y") + storm::expressions::Expression::createDoubleVariable("z") <= storm::expressions::Expression::createDoubleLiteral(12)));
    ASSERT_NO_THROW(solver.addConstraint("", storm::expressions::Expression::createDoubleLiteral(0.5) * storm::expressions::Expression::createDoubleVariable("y") + storm::expressions::Expression::createDoubleVariable("z") - storm::expressions::Expression::createDoubleVariable("x") == storm::expressions::Expression::createDoubleLiteral(5)));
    ASSERT_NO_THROW(solver.addConstraint("", storm::expressions::Expression::createDoubleVariable("y") - storm::expressions::Expression::createDoubleVariable("x") <= storm::expressions::Expression::createDoubleLiteral(5.5)));
    ASSERT_NO_THROW(solver.addConstraint("", storm::expressions::Expression::createDoubleVariable("y") > storm::expressions::Expression::createDoubleLiteral(7)));
    ASSERT_NO_THROW(solver.update());

    ASSERT_NO_THROW(solver.optimize());
    ASSERT_FALSE(solver.isOptimal());
    ASSERT_FALSE(solver.isUnbounded());
    ASSERT_TRUE(solver.isInfeasible());
    double xValue = 0;
    ASSERT_THROW(xValue = solver.getContinuousValue("x"), storm::exceptions::InvalidAccessException);
    double yValue = 0;
    ASSERT_THROW(yValue = solver.getContinuousValue("y"), storm::exceptions::InvalidAccessException);
    double zValue = 0;
    ASSERT_THROW(zValue = solver.getContinuousValue("z"), storm::exceptions::InvalidAccessException);
    double objectiveValue = 0;
    ASSERT_THROW(objectiveValue = solver.getObjectiveValue(), storm::exceptions::InvalidAccessException);
#else
    ASSERT_TRUE(false) << "StoRM built without Gurobi support.";
#endif
}

TEST(GurobiLpSolver, MILPInfeasible) {
#ifdef STORM_HAVE_GUROBI
    storm::solver::GurobiLpSolver solver(storm::solver::LpSolver::ModelSense::Maximize);
    ASSERT_NO_THROW(solver.addBinaryVariable("x", -1));
    ASSERT_NO_THROW(solver.addLowerBoundedContinuousVariable("y", 0, 2));
    ASSERT_NO_THROW(solver.addLowerBoundedContinuousVariable("z", 0, 1));
    ASSERT_NO_THROW(solver.update());

    ASSERT_NO_THROW(solver.addConstraint("", storm::expressions::Expression::createDoubleVariable("x") + storm::expressions::Expression::createDoubleVariable("y") + storm::expressions::Expression::createDoubleVariable("z") <= storm::expressions::Expression::createDoubleLiteral(12)));
    ASSERT_NO_THROW(solver.addConstraint("", storm::expressions::Expression::createDoubleLiteral(0.5) * storm::expressions::Expression::createDoubleVariable("y") + storm::expressions::Expression::createDoubleVariable("z") - storm::expressions::Expression::createDoubleVariable("x") == storm::expressions::Expression::createDoubleLiteral(5)));
    ASSERT_NO_THROW(solver.addConstraint("", storm::expressions::Expression::createDoubleVariable("y") - storm::expressions::Expression::createDoubleVariable("x") <= storm::expressions::Expression::createDoubleLiteral(5.5)));
    ASSERT_NO_THROW(solver.addConstraint("", storm::expressions::Expression::createDoubleVariable("y") > storm::expressions::Expression::createDoubleLiteral(7)));
    ASSERT_NO_THROW(solver.update());

    ASSERT_NO_THROW(solver.optimize());
    ASSERT_FALSE(solver.isOptimal());
    ASSERT_FALSE(solver.isUnbounded());
    ASSERT_TRUE(solver.isInfeasible());
    bool xValue = false;
    ASSERT_THROW(xValue = solver.getBinaryValue("x"), storm::exceptions::InvalidAccessException);
    int_fast64_t yValue = 0;
    ASSERT_THROW(yValue = solver.getIntegerValue("y"), storm::exceptions::InvalidAccessException);
    double zValue = 0;
    ASSERT_THROW(zValue = solver.getContinuousValue("z"), storm::exceptions::InvalidAccessException);
    double objectiveValue = 0;
    ASSERT_THROW(objectiveValue = solver.getObjectiveValue(), storm::exceptions::InvalidAccessException);
#else
    ASSERT_TRUE(false) << "StoRM built without Gurobi support.";
#endif
}

TEST(GurobiLpSolver, LPUnbounded) {
#ifdef STORM_HAVE_GUROBI
    storm::solver::GurobiLpSolver solver(storm::solver::LpSolver::ModelSense::Maximize);
    ASSERT_NO_THROW(solver.addBoundedContinuousVariable("x", 0, 1, -1));
    ASSERT_NO_THROW(solver.addLowerBoundedContinuousVariable("y", 0, 2));
    ASSERT_NO_THROW(solver.addLowerBoundedContinuousVariable("z", 0, 1));
    ASSERT_NO_THROW(solver.update());

    ASSERT_NO_THROW(solver.addConstraint("", storm::expressions::Expression::createDoubleVariable("x") + storm::expressions::Expression::createDoubleVariable("y") - storm::expressions::Expression::createDoubleVariable("z") <= storm::expressions::Expression::createDoubleLiteral(12)));
    ASSERT_NO_THROW(solver.addConstraint("", storm::expressions::Expression::createDoubleVariable("y") - storm::expressions::Expression::createDoubleVariable("x") <= storm::expressions::Expression::createDoubleLiteral(5.5)));
    ASSERT_NO_THROW(solver.update());

    ASSERT_NO_THROW(solver.optimize());
    ASSERT_FALSE(solver.isOptimal());
    ASSERT_TRUE(solver.isUnbounded());
    ASSERT_FALSE(solver.isInfeasible());
    double xValue = 0;
    ASSERT_THROW(xValue = solver.getContinuousValue("x"), storm::exceptions::InvalidAccessException);
    double yValue = 0;
    ASSERT_THROW(yValue = solver.getContinuousValue("y"), storm::exceptions::InvalidAccessException);
    double zValue = 0;
    ASSERT_THROW(zValue = solver.getContinuousValue("z"), storm::exceptions::InvalidAccessException);
    double objectiveValue = 0;
    ASSERT_THROW(objectiveValue = solver.getObjectiveValue(), storm::exceptions::InvalidAccessException);
#else
    ASSERT_TRUE(false) << "StoRM built without Gurobi support.";
#endif
}

TEST(GurobiLpSolver, MILPUnbounded) {
#ifdef STORM_HAVE_GUROBI
    storm::solver::GurobiLpSolver solver(storm::solver::LpSolver::ModelSense::Maximize);
    ASSERT_NO_THROW(solver.addBinaryVariable("x", -1));
    ASSERT_NO_THROW(solver.addLowerBoundedContinuousVariable("y", 0, 2));
    ASSERT_NO_THROW(solver.addLowerBoundedContinuousVariable("z", 0, 1));
    ASSERT_NO_THROW(solver.update());

    ASSERT_NO_THROW(solver.addConstraint("", storm::expressions::Expression::createDoubleVariable("x") + storm::expressions::Expression::createDoubleVariable("y") - storm::expressions::Expression::createDoubleVariable("z") <= storm::expressions::Expression::createDoubleLiteral(12)));
    ASSERT_NO_THROW(solver.addConstraint("", storm::expressions::Expression::createDoubleVariable("y") - storm::expressions::Expression::createDoubleVariable("x") <= storm::expressions::Expression::createDoubleLiteral(5.5)));
    
    ASSERT_NO_THROW(solver.optimize());
    ASSERT_FALSE(solver.isOptimal());
    ASSERT_TRUE(solver.isUnbounded());
    ASSERT_FALSE(solver.isInfeasible());
    bool xValue = false;
    ASSERT_THROW(xValue = solver.getBinaryValue("x"), storm::exceptions::InvalidAccessException);
    int_fast64_t yValue = 0;
    ASSERT_THROW(yValue = solver.getIntegerValue("y"), storm::exceptions::InvalidAccessException);
    double zValue = 0;
    ASSERT_THROW(zValue = solver.getContinuousValue("z"), storm::exceptions::InvalidAccessException);
    double objectiveValue = 0;
    ASSERT_THROW(objectiveValue = solver.getObjectiveValue(), storm::exceptions::InvalidAccessException);
#else
    ASSERT_TRUE(false) << "StoRM built without Gurobi support.";
#endif
}
