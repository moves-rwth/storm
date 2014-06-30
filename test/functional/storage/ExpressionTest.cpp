#include <map>
#include <string>

#include "gtest/gtest.h"
#include "src/storage/expressions/Expression.h"
#include "src/storage/expressions/LinearityCheckVisitor.h"
#include "src/storage/expressions/SimpleValuation.h"
#include "src/exceptions/InvalidTypeException.h"

TEST(Expression, FactoryMethodTest) {
    EXPECT_NO_THROW(storm::expressions::Expression::createBooleanLiteral(true));
    EXPECT_NO_THROW(storm::expressions::Expression::createTrue());
    EXPECT_NO_THROW(storm::expressions::Expression::createFalse());
    EXPECT_NO_THROW(storm::expressions::Expression::createIntegerLiteral(3));
    EXPECT_NO_THROW(storm::expressions::Expression::createDoubleLiteral(3.14));
    EXPECT_NO_THROW(storm::expressions::Expression::createBooleanVariable("x"));
    EXPECT_NO_THROW(storm::expressions::Expression::createIntegerVariable("y"));
    EXPECT_NO_THROW(storm::expressions::Expression::createDoubleVariable("z"));
}

TEST(Expression, AccessorTest) {
    storm::expressions::Expression trueExpression;
    storm::expressions::Expression falseExpression;
    storm::expressions::Expression threeExpression;
    storm::expressions::Expression piExpression;
    storm::expressions::Expression boolVarExpression;
    storm::expressions::Expression intVarExpression;
    storm::expressions::Expression doubleVarExpression;
    
    ASSERT_NO_THROW(trueExpression = storm::expressions::Expression::createTrue());
    ASSERT_NO_THROW(falseExpression = storm::expressions::Expression::createFalse());
    ASSERT_NO_THROW(threeExpression = storm::expressions::Expression::createIntegerLiteral(3));
    ASSERT_NO_THROW(piExpression = storm::expressions::Expression::createDoubleLiteral(3.14));
    ASSERT_NO_THROW(boolVarExpression = storm::expressions::Expression::createBooleanVariable("x"));
    ASSERT_NO_THROW(intVarExpression = storm::expressions::Expression::createIntegerVariable("y"));
    ASSERT_NO_THROW(doubleVarExpression = storm::expressions::Expression::createDoubleVariable("z"));
    
    EXPECT_TRUE(trueExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    EXPECT_TRUE(trueExpression.isLiteral());
    EXPECT_TRUE(trueExpression.isTrue());
    EXPECT_FALSE(trueExpression.isFalse());
    EXPECT_TRUE(trueExpression.getVariables() == std::set<std::string>());
    
    EXPECT_TRUE(falseExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    EXPECT_TRUE(falseExpression.isLiteral());
    EXPECT_FALSE(falseExpression.isTrue());
    EXPECT_TRUE(falseExpression.isFalse());
    EXPECT_TRUE(falseExpression.getVariables() == std::set<std::string>());

    EXPECT_TRUE(threeExpression.getReturnType() == storm::expressions::ExpressionReturnType::Int);
    EXPECT_TRUE(threeExpression.isLiteral());
    EXPECT_FALSE(threeExpression.isTrue());
    EXPECT_FALSE(threeExpression.isFalse());
    EXPECT_TRUE(threeExpression.getVariables() == std::set<std::string>());
    
    EXPECT_TRUE(piExpression.getReturnType() == storm::expressions::ExpressionReturnType::Double);
    EXPECT_TRUE(piExpression.isLiteral());
    EXPECT_FALSE(piExpression.isTrue());
    EXPECT_FALSE(piExpression.isFalse());
    EXPECT_TRUE(piExpression.getVariables() == std::set<std::string>());
    
    EXPECT_TRUE(boolVarExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    EXPECT_FALSE(boolVarExpression.isLiteral());
    EXPECT_FALSE(boolVarExpression.isTrue());
    EXPECT_FALSE(boolVarExpression.isFalse());
    EXPECT_TRUE(boolVarExpression.getVariables() == std::set<std::string>({"x"}));

    EXPECT_TRUE(intVarExpression.getReturnType() == storm::expressions::ExpressionReturnType::Int);
    EXPECT_FALSE(intVarExpression.isLiteral());
    EXPECT_FALSE(intVarExpression.isTrue());
    EXPECT_FALSE(intVarExpression.isFalse());
    EXPECT_TRUE(intVarExpression.getVariables() == std::set<std::string>({"y"}));

    EXPECT_TRUE(doubleVarExpression.getReturnType() == storm::expressions::ExpressionReturnType::Double);
    EXPECT_FALSE(doubleVarExpression.isLiteral());
    EXPECT_FALSE(doubleVarExpression.isTrue());
    EXPECT_FALSE(doubleVarExpression.isFalse());
    EXPECT_TRUE(doubleVarExpression.getVariables() == std::set<std::string>({"z"}));
}

TEST(Expression, OperatorTest) {
    storm::expressions::Expression trueExpression;
    storm::expressions::Expression falseExpression;
    storm::expressions::Expression threeExpression;
    storm::expressions::Expression piExpression;
    storm::expressions::Expression boolVarExpression;
    storm::expressions::Expression intVarExpression;
    storm::expressions::Expression doubleVarExpression;
    
    ASSERT_NO_THROW(trueExpression = storm::expressions::Expression::createTrue());
    ASSERT_NO_THROW(falseExpression = storm::expressions::Expression::createFalse());
    ASSERT_NO_THROW(threeExpression = storm::expressions::Expression::createIntegerLiteral(3));
    ASSERT_NO_THROW(piExpression = storm::expressions::Expression::createDoubleLiteral(3.14));
    ASSERT_NO_THROW(boolVarExpression = storm::expressions::Expression::createBooleanVariable("x"));
    ASSERT_NO_THROW(intVarExpression = storm::expressions::Expression::createIntegerVariable("y"));
    ASSERT_NO_THROW(doubleVarExpression = storm::expressions::Expression::createDoubleVariable("z"));
    
    storm::expressions::Expression tempExpression;

    ASSERT_THROW(tempExpression = trueExpression.ite(falseExpression, piExpression), storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = boolVarExpression.ite(threeExpression, doubleVarExpression));
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Double);
    ASSERT_NO_THROW(tempExpression = boolVarExpression.ite(threeExpression, intVarExpression));
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Int);
    ASSERT_NO_THROW(tempExpression = boolVarExpression.ite(trueExpression, falseExpression));
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    
    ASSERT_THROW(tempExpression = trueExpression + piExpression, storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = threeExpression + threeExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Int);
    ASSERT_NO_THROW(tempExpression = threeExpression + piExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Double);
    ASSERT_NO_THROW(tempExpression = doubleVarExpression + doubleVarExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Double);
    
    ASSERT_THROW(tempExpression = trueExpression - piExpression, storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = threeExpression - threeExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Int);
    ASSERT_NO_THROW(tempExpression = threeExpression - piExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Double);
    ASSERT_NO_THROW(tempExpression = doubleVarExpression - doubleVarExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Double);
    
    ASSERT_THROW(tempExpression = -trueExpression, storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = -threeExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Int);
    ASSERT_NO_THROW(tempExpression = -piExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Double);
    ASSERT_NO_THROW(tempExpression = -doubleVarExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Double);

    ASSERT_THROW(tempExpression = trueExpression * piExpression, storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = threeExpression * threeExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Int);
    ASSERT_NO_THROW(tempExpression = threeExpression * piExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Double);
    ASSERT_NO_THROW(tempExpression = intVarExpression * intVarExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Int);

    ASSERT_THROW(tempExpression = trueExpression / piExpression, storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = threeExpression / threeExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Int);
    ASSERT_NO_THROW(tempExpression = threeExpression / piExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Double);
    ASSERT_NO_THROW(tempExpression = doubleVarExpression / intVarExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Double);

    ASSERT_THROW(tempExpression = trueExpression && piExpression, storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = trueExpression && falseExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    ASSERT_NO_THROW(tempExpression = boolVarExpression && boolVarExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);

    ASSERT_THROW(tempExpression = trueExpression || piExpression, storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = trueExpression || falseExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    ASSERT_NO_THROW(tempExpression = boolVarExpression || boolVarExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);

    ASSERT_THROW(tempExpression = !threeExpression, storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = !trueExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    ASSERT_NO_THROW(tempExpression = !boolVarExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);

    ASSERT_THROW(tempExpression = trueExpression == piExpression, storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = threeExpression == threeExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    ASSERT_NO_THROW(tempExpression = intVarExpression == doubleVarExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);

    ASSERT_THROW(tempExpression = trueExpression != piExpression, storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = threeExpression != threeExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    ASSERT_NO_THROW(tempExpression = intVarExpression != doubleVarExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);

    ASSERT_THROW(tempExpression = trueExpression > piExpression, storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = threeExpression > threeExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    ASSERT_NO_THROW(tempExpression = intVarExpression > doubleVarExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    
    ASSERT_THROW(tempExpression = trueExpression >= piExpression, storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = threeExpression >= threeExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    ASSERT_NO_THROW(tempExpression = intVarExpression >= doubleVarExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    
    ASSERT_THROW(tempExpression = trueExpression < piExpression, storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = threeExpression < threeExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    ASSERT_NO_THROW(tempExpression = intVarExpression < doubleVarExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    
    ASSERT_THROW(tempExpression = trueExpression <= piExpression, storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = threeExpression <= threeExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    ASSERT_NO_THROW(tempExpression = intVarExpression <= doubleVarExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    
    ASSERT_THROW(tempExpression = storm::expressions::Expression::minimum(trueExpression, piExpression), storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = storm::expressions::Expression::minimum(threeExpression, threeExpression));
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Int);
    ASSERT_NO_THROW(tempExpression = storm::expressions::Expression::minimum(intVarExpression, doubleVarExpression));
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Double);

    ASSERT_THROW(tempExpression = storm::expressions::Expression::maximum(trueExpression, piExpression), storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = storm::expressions::Expression::maximum(threeExpression, threeExpression));
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Int);
    ASSERT_NO_THROW(tempExpression = storm::expressions::Expression::maximum(intVarExpression, doubleVarExpression));
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Double);
    
    ASSERT_THROW(tempExpression = trueExpression.implies(piExpression), storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = trueExpression.implies(falseExpression));
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    ASSERT_NO_THROW(tempExpression = boolVarExpression.implies(boolVarExpression));
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    
    ASSERT_THROW(tempExpression = trueExpression.iff(piExpression), storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = trueExpression.iff(falseExpression));
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    ASSERT_NO_THROW(tempExpression = boolVarExpression.iff(boolVarExpression));
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    
    ASSERT_THROW(tempExpression = trueExpression != piExpression, storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = trueExpression != falseExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    ASSERT_NO_THROW(tempExpression = boolVarExpression != boolVarExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    
    ASSERT_THROW(tempExpression = trueExpression.floor(), storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = threeExpression.floor());
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Int);
    ASSERT_NO_THROW(tempExpression = doubleVarExpression.floor());
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Int);

    ASSERT_THROW(tempExpression = trueExpression.ceil(), storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = threeExpression.ceil());
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Int);
    ASSERT_NO_THROW(tempExpression = doubleVarExpression.ceil());
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Int);

    ASSERT_THROW(tempExpression = trueExpression ^ piExpression, storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = threeExpression ^ threeExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Int);
    ASSERT_NO_THROW(tempExpression = intVarExpression ^ doubleVarExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Double);
}

TEST(Expression, SubstitutionTest) {
    storm::expressions::Expression trueExpression;
    storm::expressions::Expression falseExpression;
    storm::expressions::Expression threeExpression;
    storm::expressions::Expression piExpression;
    storm::expressions::Expression boolVarExpression;
    storm::expressions::Expression intVarExpression;
    storm::expressions::Expression doubleVarExpression;
    
    ASSERT_NO_THROW(trueExpression = storm::expressions::Expression::createTrue());
    ASSERT_NO_THROW(falseExpression = storm::expressions::Expression::createFalse());
    ASSERT_NO_THROW(threeExpression = storm::expressions::Expression::createIntegerLiteral(3));
    ASSERT_NO_THROW(piExpression = storm::expressions::Expression::createDoubleLiteral(3.14));
    ASSERT_NO_THROW(boolVarExpression = storm::expressions::Expression::createBooleanVariable("x"));
    ASSERT_NO_THROW(intVarExpression = storm::expressions::Expression::createIntegerVariable("y"));
    ASSERT_NO_THROW(doubleVarExpression = storm::expressions::Expression::createDoubleVariable("z"));
    
    storm::expressions::Expression tempExpression;
    ASSERT_NO_THROW(tempExpression = (intVarExpression < threeExpression || boolVarExpression) && boolVarExpression);

    std::map<std::string, storm::expressions::Expression> substution = { std::make_pair("y", doubleVarExpression), std::make_pair("x", storm::expressions::Expression::createTrue()), std::make_pair("a", storm::expressions::Expression::createTrue()) };
    storm::expressions::Expression substitutedExpression;
    ASSERT_NO_THROW(substitutedExpression = tempExpression.substitute(substution));
    EXPECT_TRUE(substitutedExpression.simplify().isTrue());
}

TEST(Expression, SimplificationTest) {
    storm::expressions::Expression trueExpression;
    storm::expressions::Expression falseExpression;
    storm::expressions::Expression threeExpression;
    storm::expressions::Expression intVarExpression;
    
    ASSERT_NO_THROW(trueExpression = storm::expressions::Expression::createTrue());
    ASSERT_NO_THROW(falseExpression = storm::expressions::Expression::createFalse());
    ASSERT_NO_THROW(threeExpression = storm::expressions::Expression::createIntegerLiteral(3));
    ASSERT_NO_THROW(intVarExpression = storm::expressions::Expression::createIntegerVariable("y"));
    
    storm::expressions::Expression tempExpression;
    storm::expressions::Expression simplifiedExpression;

    ASSERT_NO_THROW(tempExpression = trueExpression || intVarExpression > threeExpression);
    ASSERT_NO_THROW(simplifiedExpression = tempExpression.simplify());
    EXPECT_TRUE(simplifiedExpression.isTrue());

    ASSERT_NO_THROW(tempExpression = falseExpression && intVarExpression > threeExpression);
    ASSERT_NO_THROW(simplifiedExpression = tempExpression.simplify());
    EXPECT_TRUE(simplifiedExpression.isFalse());
}

TEST(Expression, SimpleEvaluationTest) {
    storm::expressions::Expression trueExpression;
    storm::expressions::Expression falseExpression;
    storm::expressions::Expression threeExpression;
    storm::expressions::Expression piExpression;
    storm::expressions::Expression boolVarExpression;
    storm::expressions::Expression intVarExpression;
    storm::expressions::Expression doubleVarExpression;
    
    ASSERT_NO_THROW(trueExpression = storm::expressions::Expression::createTrue());
    ASSERT_NO_THROW(falseExpression = storm::expressions::Expression::createFalse());
    ASSERT_NO_THROW(threeExpression = storm::expressions::Expression::createIntegerLiteral(3));
    ASSERT_NO_THROW(piExpression = storm::expressions::Expression::createDoubleLiteral(3.14));
    ASSERT_NO_THROW(boolVarExpression = storm::expressions::Expression::createBooleanVariable("x"));
    ASSERT_NO_THROW(intVarExpression = storm::expressions::Expression::createIntegerVariable("y"));
    ASSERT_NO_THROW(doubleVarExpression = storm::expressions::Expression::createDoubleVariable("z"));
    
    storm::expressions::Expression tempExpression;
    
    ASSERT_NO_THROW(tempExpression = (intVarExpression < threeExpression || boolVarExpression) && boolVarExpression);
    
    ASSERT_NO_THROW(storm::expressions::SimpleValuation valuation);
    storm::expressions::SimpleValuation valuation;
    ASSERT_NO_THROW(valuation.addBooleanIdentifier("x"));
    ASSERT_NO_THROW(valuation.addBooleanIdentifier("a"));
    ASSERT_NO_THROW(valuation.addIntegerIdentifier("y"));
    ASSERT_NO_THROW(valuation.addIntegerIdentifier("b"));
    ASSERT_NO_THROW(valuation.addDoubleIdentifier("z"));
    ASSERT_NO_THROW(valuation.addDoubleIdentifier("c"));
    
    ASSERT_THROW(tempExpression.evaluateAsDouble(&valuation), storm::exceptions::InvalidTypeException);
    ASSERT_THROW(tempExpression.evaluateAsInt(&valuation), storm::exceptions::InvalidTypeException);
    EXPECT_FALSE(tempExpression.evaluateAsBool(&valuation));
    ASSERT_NO_THROW(valuation.setBooleanValue("a", true));
    EXPECT_FALSE(tempExpression.evaluateAsBool(&valuation));
    ASSERT_NO_THROW(valuation.setIntegerValue("y", 3));
    EXPECT_FALSE(tempExpression.evaluateAsBool(&valuation));
    
    ASSERT_NO_THROW(tempExpression = ((intVarExpression < threeExpression).ite(trueExpression, falseExpression)));
    ASSERT_THROW(tempExpression.evaluateAsDouble(&valuation), storm::exceptions::InvalidTypeException);
    ASSERT_THROW(tempExpression.evaluateAsInt(&valuation), storm::exceptions::InvalidTypeException);
    EXPECT_FALSE(tempExpression.evaluateAsBool(&valuation));
}

TEST(Expression, VisitorTest) {
    storm::expressions::Expression threeExpression;
    storm::expressions::Expression piExpression;
    storm::expressions::Expression intVarExpression;
    storm::expressions::Expression doubleVarExpression;
    
    ASSERT_NO_THROW(threeExpression = storm::expressions::Expression::createIntegerLiteral(3));
    ASSERT_NO_THROW(piExpression = storm::expressions::Expression::createDoubleLiteral(3.14));
    ASSERT_NO_THROW(intVarExpression = storm::expressions::Expression::createIntegerVariable("y"));
    ASSERT_NO_THROW(doubleVarExpression = storm::expressions::Expression::createDoubleVariable("z"));
    
    storm::expressions::Expression tempExpression = intVarExpression + doubleVarExpression * threeExpression;
    storm::expressions::LinearityCheckVisitor visitor;
    EXPECT_TRUE(visitor.check(tempExpression));
}