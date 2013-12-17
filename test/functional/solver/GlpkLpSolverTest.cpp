#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/solver/GlpkLpSolver.h"
#include "src/exceptions/InvalidStateException.h"
#include "src/settings/Settings.h"

TEST(GlpkLpSolver, LPOptimizeMax) {
#ifdef STORM_HAVE_GLPK
    storm::solver::GlpkLpSolver solver(storm::solver::LpSolver::MAXIMIZE);
    uint_fast64_t xIndex;
    ASSERT_NO_THROW(xIndex = solver.createContinuousVariable("x", storm::solver::LpSolver::VariableType::BOUNDED, 0, 1, -1));
    uint_fast64_t yIndex;
    ASSERT_NO_THROW(yIndex = solver.createContinuousVariable("y", storm::solver::LpSolver::VariableType::LOWER_BOUND, 0, 0, 2));
    uint_fast64_t zIndex;
    ASSERT_NO_THROW(zIndex = solver.createContinuousVariable("z", storm::solver::LpSolver::VariableType::LOWER_BOUND, 0, 0, 1));
    
    ASSERT_NO_THROW(solver.addConstraint("", {xIndex, yIndex, zIndex}, {1, 1, 1}, storm::solver::LpSolver::BoundType::LESS_EQUAL, 12));
    ASSERT_NO_THROW(solver.addConstraint("", {yIndex, zIndex, xIndex}, {0.5, 1, -1}, storm::solver::LpSolver::BoundType::EQUAL, 5));
    ASSERT_NO_THROW(solver.addConstraint("", {yIndex, xIndex}, {1, -1}, storm::solver::LpSolver::BoundType::LESS_EQUAL, 5.5));
    
    ASSERT_NO_THROW(solver.optimize());
    ASSERT_TRUE(solver.isOptimal());
    ASSERT_FALSE(solver.isUnbounded());
    ASSERT_FALSE(solver.isInfeasible());
    double xValue = 0;
    ASSERT_NO_THROW(xValue = solver.getContinuousValue(xIndex));
    ASSERT_LT(std::abs(xValue - 1), storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    double yValue = 0;
    ASSERT_NO_THROW(yValue = solver.getContinuousValue(yIndex));
    ASSERT_LT(std::abs(yValue - 6.5), storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    double zValue = 0;
    ASSERT_NO_THROW(zValue = solver.getContinuousValue(zIndex));
    ASSERT_LT(std::abs(zValue - 2.75), storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    double objectiveValue = 0;
    ASSERT_NO_THROW(objectiveValue = solver.getObjectiveValue());
    ASSERT_LT(std::abs(objectiveValue - 14.75), storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
#else
    ASSERT_TRUE(false, "StoRM built without glpk support.");
#endif
}

TEST(GlpkLpSolver, LPOptimizeMin) {
#ifdef STORM_HAVE_GLPK
    storm::solver::GlpkLpSolver solver(storm::solver::LpSolver::MINIMIZE);
    uint_fast64_t xIndex;
    ASSERT_NO_THROW(xIndex = solver.createContinuousVariable("x", storm::solver::LpSolver::VariableType::BOUNDED, 0, 1, -1));
    uint_fast64_t yIndex;
    ASSERT_NO_THROW(yIndex = solver.createContinuousVariable("y", storm::solver::LpSolver::VariableType::LOWER_BOUND, 0, 0, 2));
    uint_fast64_t zIndex;
    ASSERT_NO_THROW(zIndex = solver.createContinuousVariable("z", storm::solver::LpSolver::VariableType::BOUNDED, 1, 5.7, -1));
    
    ASSERT_NO_THROW(solver.addConstraint("", {xIndex, yIndex, zIndex}, {1, 1, 1}, storm::solver::LpSolver::BoundType::LESS_EQUAL, 12));
    ASSERT_NO_THROW(solver.addConstraint("", {yIndex, zIndex, xIndex}, {0.5, 1, -1}, storm::solver::LpSolver::BoundType::LESS_EQUAL, 5));
    ASSERT_NO_THROW(solver.addConstraint("", {yIndex, xIndex}, {1, -1}, storm::solver::LpSolver::BoundType::LESS_EQUAL, 5.5));
    
    ASSERT_NO_THROW(solver.optimize());
    ASSERT_TRUE(solver.isOptimal());
    ASSERT_FALSE(solver.isUnbounded());
    ASSERT_FALSE(solver.isInfeasible());
    double xValue = 0;
    ASSERT_NO_THROW(xValue = solver.getContinuousValue(xIndex));
    ASSERT_LT(std::abs(xValue - 1), storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    double yValue = 0;
    ASSERT_NO_THROW(yValue = solver.getContinuousValue(yIndex));
    ASSERT_LT(std::abs(yValue - 0), storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    double zValue = 0;
    ASSERT_NO_THROW(zValue = solver.getContinuousValue(zIndex));
    ASSERT_LT(std::abs(zValue - 5.7), storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    double objectiveValue = 0;
    ASSERT_NO_THROW(objectiveValue = solver.getObjectiveValue());
    ASSERT_LT(std::abs(objectiveValue - (-6.7)), storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
#else
    ASSERT_TRUE(false, "StoRM built without glpk support.");
#endif
}

TEST(GlpkLpSolver, MILPOptimizeMax) {
#ifdef STORM_HAVE_GLPK
    storm::solver::GlpkLpSolver solver(storm::solver::LpSolver::MAXIMIZE);
    uint_fast64_t xIndex;
    ASSERT_NO_THROW(xIndex = solver.createBinaryVariable("x", -1));
    uint_fast64_t yIndex;
    ASSERT_NO_THROW(yIndex = solver.createIntegerVariable("y", storm::solver::LpSolver::VariableType::LOWER_BOUND, 0, 0, 2));
    uint_fast64_t zIndex;
    ASSERT_NO_THROW(zIndex = solver.createContinuousVariable("z", storm::solver::LpSolver::VariableType::LOWER_BOUND, 0, 0, 1));
    
    ASSERT_NO_THROW(solver.addConstraint("", {xIndex, yIndex, zIndex}, {1, 1, 1}, storm::solver::LpSolver::BoundType::LESS_EQUAL, 12));
    ASSERT_NO_THROW(solver.addConstraint("", {yIndex, zIndex, xIndex}, {0.5, 1, -1}, storm::solver::LpSolver::BoundType::EQUAL, 5));
    ASSERT_NO_THROW(solver.addConstraint("", {yIndex, xIndex}, {1, -1}, storm::solver::LpSolver::BoundType::LESS_EQUAL, 5.5));
    
    ASSERT_NO_THROW(solver.optimize());
    ASSERT_TRUE(solver.isOptimal());
    ASSERT_FALSE(solver.isUnbounded());
    ASSERT_FALSE(solver.isInfeasible());
    bool xValue = false;
    ASSERT_NO_THROW(xValue = solver.getBinaryValue(xIndex));
    ASSERT_EQ(true, xValue);
    int_fast64_t yValue = 0;
    ASSERT_NO_THROW(yValue = solver.getIntegerValue(yIndex));
    ASSERT_EQ(6, yValue);
    double zValue = 0;
    ASSERT_NO_THROW(zValue = solver.getContinuousValue(zIndex));
    ASSERT_LT(std::abs(zValue - 3), storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    double objectiveValue = 0;
    ASSERT_NO_THROW(objectiveValue = solver.getObjectiveValue());
    ASSERT_LT(std::abs(objectiveValue - 14), storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
#else
    ASSERT_TRUE(false, "StoRM built without glpk support.");
#endif
}

TEST(GlpkLpSolver, MILPOptimizeMin) {
#ifdef STORM_HAVE_GLPK
    storm::solver::GlpkLpSolver solver(storm::solver::LpSolver::MINIMIZE);
    uint_fast64_t xIndex;
    ASSERT_NO_THROW(xIndex = solver.createBinaryVariable("x", -1));
    uint_fast64_t yIndex;
    ASSERT_NO_THROW(yIndex = solver.createContinuousVariable("y", storm::solver::LpSolver::VariableType::LOWER_BOUND, 0, 0, 2));
    uint_fast64_t zIndex;
    ASSERT_NO_THROW(zIndex = solver.createIntegerVariable("z", storm::solver::LpSolver::VariableType::BOUNDED, 0, 5, -1));
    
    ASSERT_NO_THROW(solver.addConstraint("", {xIndex, yIndex, zIndex}, {1, 1, 1}, storm::solver::LpSolver::BoundType::LESS_EQUAL, 12));
    ASSERT_NO_THROW(solver.addConstraint("", {yIndex, zIndex, xIndex}, {0.5, 1, -1}, storm::solver::LpSolver::BoundType::LESS_EQUAL, 5));
    ASSERT_NO_THROW(solver.addConstraint("", {yIndex, xIndex}, {1, -1}, storm::solver::LpSolver::BoundType::LESS_EQUAL, 5.5));
    
    ASSERT_NO_THROW(solver.optimize());
    ASSERT_TRUE(solver.isOptimal());
    ASSERT_FALSE(solver.isUnbounded());
    ASSERT_FALSE(solver.isInfeasible());
    bool xValue = false;
    ASSERT_NO_THROW(xValue = solver.getBinaryValue(xIndex));
    ASSERT_EQ(true, xValue);
    int_fast64_t yValue = 0;
    ASSERT_NO_THROW(yValue = solver.getIntegerValue(yIndex));
    ASSERT_EQ(0, yValue);
    double zValue = 0;
    ASSERT_NO_THROW(zValue = solver.getContinuousValue(zIndex));
    ASSERT_LT(std::abs(zValue - 5), storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    double objectiveValue = 0;
    ASSERT_NO_THROW(objectiveValue = solver.getObjectiveValue());
    ASSERT_LT(std::abs(objectiveValue - (-6)), storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
#else
    ASSERT_TRUE(false, "StoRM built without glpk support.");
#endif
}

TEST(GlpkLpSolver, LPInfeasible) {
#ifdef STORM_HAVE_GLPK
    storm::solver::GlpkLpSolver solver(storm::solver::LpSolver::MAXIMIZE);
    uint_fast64_t xIndex;
    ASSERT_NO_THROW(xIndex = solver.createContinuousVariable("x", storm::solver::LpSolver::VariableType::BOUNDED, 0, 1, -1));
    uint_fast64_t yIndex;
    ASSERT_NO_THROW(yIndex = solver.createContinuousVariable("y", storm::solver::LpSolver::VariableType::LOWER_BOUND, 0, 0, 2));
    uint_fast64_t zIndex;
    ASSERT_NO_THROW(zIndex = solver.createContinuousVariable("z", storm::solver::LpSolver::VariableType::LOWER_BOUND, 0, 0, 1));
    
    ASSERT_NO_THROW(solver.addConstraint("", {xIndex, yIndex, zIndex}, {1, 1, 1}, storm::solver::LpSolver::BoundType::LESS_EQUAL, 12));
    ASSERT_NO_THROW(solver.addConstraint("", {yIndex, zIndex, xIndex}, {0.5, 1, -1}, storm::solver::LpSolver::BoundType::EQUAL, 5));
    ASSERT_NO_THROW(solver.addConstraint("", {yIndex, xIndex}, {1, -1}, storm::solver::LpSolver::BoundType::LESS_EQUAL, 5.5));
    ASSERT_NO_THROW(solver.addConstraint("", {yIndex}, {1}, storm::solver::LpSolver::BoundType::GREATER_EQUAL, 7));
    
    ASSERT_NO_THROW(solver.optimize());
    ASSERT_FALSE(solver.isOptimal());
    ASSERT_FALSE(solver.isUnbounded());
    ASSERT_TRUE(solver.isInfeasible());
    double xValue = 0;
    ASSERT_THROW(xValue = solver.getContinuousValue(xIndex), storm::exceptions::InvalidStateException);
    double yValue = 0;
    ASSERT_THROW(yValue = solver.getContinuousValue(yIndex), storm::exceptions::InvalidStateException);
    double zValue = 0;
    ASSERT_THROW(zValue = solver.getContinuousValue(zIndex), storm::exceptions::InvalidStateException);
    double objectiveValue = 0;
    ASSERT_THROW(objectiveValue = solver.getObjectiveValue(), storm::exceptions::InvalidStateException);
#else
    ASSERT_TRUE(false, "StoRM built without glpk support.");
#endif
}

TEST(GlpkLpSolver, MILPInfeasible) {
#ifdef STORM_HAVE_GLPK
    storm::solver::GlpkLpSolver solver(storm::solver::LpSolver::MAXIMIZE);
    uint_fast64_t xIndex;
    ASSERT_NO_THROW(xIndex = solver.createBinaryVariable("x", -1));
    uint_fast64_t yIndex;
    ASSERT_NO_THROW(yIndex = solver.createIntegerVariable("y", storm::solver::LpSolver::VariableType::LOWER_BOUND, 0, 0, 2));
    uint_fast64_t zIndex;
    ASSERT_NO_THROW(zIndex = solver.createContinuousVariable("z", storm::solver::LpSolver::VariableType::LOWER_BOUND, 0, 0, 1));
    
    ASSERT_NO_THROW(solver.addConstraint("", {xIndex, yIndex, zIndex}, {1, 1, 1}, storm::solver::LpSolver::BoundType::LESS_EQUAL, 12));
    ASSERT_NO_THROW(solver.addConstraint("", {yIndex, zIndex, xIndex}, {0.5, 1, -1}, storm::solver::LpSolver::BoundType::EQUAL, 5));
    ASSERT_NO_THROW(solver.addConstraint("", {yIndex, xIndex}, {1, -1}, storm::solver::LpSolver::BoundType::LESS_EQUAL, 5.5));
    ASSERT_NO_THROW(solver.addConstraint("", {yIndex}, {1}, storm::solver::LpSolver::BoundType::GREATER_EQUAL, 7));
    
    ASSERT_NO_THROW(solver.optimize());
    ASSERT_FALSE(solver.isOptimal());
    ASSERT_FALSE(solver.isUnbounded());
    ASSERT_TRUE(solver.isInfeasible());
    bool xValue = false;
    ASSERT_THROW(xValue = solver.getBinaryValue(xIndex), storm::exceptions::InvalidStateException);
    int_fast64_t yValue = 0;
    ASSERT_THROW(yValue = solver.getIntegerValue(yIndex), storm::exceptions::InvalidStateException);
    double zValue = 0;
    ASSERT_THROW(zValue = solver.getContinuousValue(zIndex), storm::exceptions::InvalidStateException);
    double objectiveValue = 0;
    ASSERT_THROW(objectiveValue = solver.getObjectiveValue(), storm::exceptions::InvalidStateException);
#else
    ASSERT_TRUE(false, "StoRM built without glpk support.");
#endif
}

TEST(GlpkLpSolver, LPUnbounded) {
#ifdef STORM_HAVE_GLPK
    storm::solver::GlpkLpSolver solver(storm::solver::LpSolver::MAXIMIZE);
    uint_fast64_t xIndex;
    ASSERT_NO_THROW(xIndex = solver.createContinuousVariable("x", storm::solver::LpSolver::VariableType::BOUNDED, 0, 1, -1));
    uint_fast64_t yIndex;
    ASSERT_NO_THROW(yIndex = solver.createContinuousVariable("y", storm::solver::LpSolver::VariableType::LOWER_BOUND, 0, 0, 2));
    uint_fast64_t zIndex;
    ASSERT_NO_THROW(zIndex = solver.createContinuousVariable("z", storm::solver::LpSolver::VariableType::LOWER_BOUND, 0, 0, 1));
    
    ASSERT_NO_THROW(solver.addConstraint("", {xIndex, yIndex, zIndex}, {1, 1, -1}, storm::solver::LpSolver::BoundType::LESS_EQUAL, 12));
    ASSERT_NO_THROW(solver.addConstraint("", {yIndex, xIndex}, {1, -1}, storm::solver::LpSolver::BoundType::LESS_EQUAL, 5.5));
    
    ASSERT_NO_THROW(solver.optimize());
    ASSERT_FALSE(solver.isOptimal());
    ASSERT_TRUE(solver.isUnbounded());
    ASSERT_FALSE(solver.isInfeasible());
    double xValue = 0;
    ASSERT_THROW(xValue = solver.getContinuousValue(xIndex), storm::exceptions::InvalidStateException);
    double yValue = 0;
    ASSERT_THROW(yValue = solver.getContinuousValue(yIndex), storm::exceptions::InvalidStateException);
    double zValue = 0;
    ASSERT_THROW(zValue = solver.getContinuousValue(zIndex), storm::exceptions::InvalidStateException);
    double objectiveValue = 0;
    ASSERT_THROW(objectiveValue = solver.getObjectiveValue(), storm::exceptions::InvalidStateException);
#else
    ASSERT_TRUE(false, "StoRM built without glpk support.");
#endif
}

TEST(GlpkLpSolver, MILPUnbounded) {
#ifdef STORM_HAVE_GLPK
    storm::solver::GlpkLpSolver solver(storm::solver::LpSolver::MAXIMIZE);
    uint_fast64_t xIndex;
    ASSERT_NO_THROW(xIndex = solver.createBinaryVariable("x", -1));
    uint_fast64_t yIndex;
    ASSERT_NO_THROW(yIndex = solver.createIntegerVariable("y", storm::solver::LpSolver::VariableType::LOWER_BOUND, 0, 0, 2));
    uint_fast64_t zIndex;
    ASSERT_NO_THROW(zIndex = solver.createContinuousVariable("z", storm::solver::LpSolver::VariableType::LOWER_BOUND, 0, 0, 1));
    
    ASSERT_NO_THROW(solver.addConstraint("", {xIndex, yIndex, zIndex}, {1, 1, -1}, storm::solver::LpSolver::BoundType::LESS_EQUAL, 12));
    ASSERT_NO_THROW(solver.addConstraint("", {yIndex, xIndex}, {1, -1}, storm::solver::LpSolver::BoundType::LESS_EQUAL, 5.5));
    
    ASSERT_NO_THROW(solver.optimize());
    ASSERT_FALSE(solver.isOptimal());
    ASSERT_TRUE(solver.isUnbounded());
    ASSERT_FALSE(solver.isInfeasible());
    bool xValue = false;
    ASSERT_THROW(xValue = solver.getBinaryValue(xIndex), storm::exceptions::InvalidStateException);
    int_fast64_t yValue = 0;
    ASSERT_THROW(yValue = solver.getIntegerValue(yIndex), storm::exceptions::InvalidStateException);
    double zValue = 0;
    ASSERT_THROW(zValue = solver.getContinuousValue(zIndex), storm::exceptions::InvalidStateException);
    double objectiveValue = 0;
    ASSERT_THROW(objectiveValue = solver.getObjectiveValue(), storm::exceptions::InvalidStateException);
#else
    ASSERT_TRUE(false, "StoRM built without glpk support.");
#endif
}