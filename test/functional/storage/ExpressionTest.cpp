#include <map>

#include "gtest/gtest.h"
#include "src/storage/expressions/Expression.h"
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
    EXPECT_NO_THROW(storm::expressions::Expression::createBooleanConstant("a"));
    EXPECT_NO_THROW(storm::expressions::Expression::createIntegerConstant("b"));
    EXPECT_NO_THROW(storm::expressions::Expression::createDoubleConstant("c"));
}

TEST(Expression, AccessorTest) {
    storm::expressions::Expression trueExpression;
    storm::expressions::Expression falseExpression;
    storm::expressions::Expression threeExpression;
    storm::expressions::Expression piExpression;
    storm::expressions::Expression boolVarExpression;
    storm::expressions::Expression intVarExpression;
    storm::expressions::Expression doubleVarExpression;
    storm::expressions::Expression boolConstExpression;
    storm::expressions::Expression intConstExpression;
    storm::expressions::Expression doubleConstExpression;
    
    ASSERT_NO_THROW(trueExpression = storm::expressions::Expression::createTrue());
    ASSERT_NO_THROW(falseExpression = storm::expressions::Expression::createFalse());
    ASSERT_NO_THROW(threeExpression = storm::expressions::Expression::createIntegerLiteral(3));
    ASSERT_NO_THROW(piExpression = storm::expressions::Expression::createDoubleLiteral(3.14));
    ASSERT_NO_THROW(boolVarExpression = storm::expressions::Expression::createBooleanVariable("x"));
    ASSERT_NO_THROW(intVarExpression = storm::expressions::Expression::createIntegerVariable("y"));
    ASSERT_NO_THROW(doubleVarExpression = storm::expressions::Expression::createDoubleVariable("z"));
    ASSERT_NO_THROW(boolConstExpression = storm::expressions::Expression::createBooleanConstant("a"));
    ASSERT_NO_THROW(intConstExpression = storm::expressions::Expression::createIntegerConstant("b"));
    ASSERT_NO_THROW(doubleConstExpression = storm::expressions::Expression::createDoubleConstant("c"));
    
    EXPECT_TRUE(trueExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    EXPECT_TRUE(trueExpression.isConstant());
    EXPECT_TRUE(trueExpression.isTrue());
    EXPECT_FALSE(trueExpression.isFalse());
    EXPECT_TRUE(trueExpression.getVariables() == std::set<std::string>());
    EXPECT_TRUE(trueExpression.getConstants() == std::set<std::string>());
    
    EXPECT_TRUE(falseExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    EXPECT_TRUE(falseExpression.isConstant());
    EXPECT_FALSE(falseExpression.isTrue());
    EXPECT_TRUE(falseExpression.isFalse());
    EXPECT_TRUE(falseExpression.getVariables() == std::set<std::string>());
    EXPECT_TRUE(falseExpression.getConstants() == std::set<std::string>());

    EXPECT_TRUE(threeExpression.getReturnType() == storm::expressions::ExpressionReturnType::Int);
    EXPECT_TRUE(threeExpression.isConstant());
    EXPECT_FALSE(threeExpression.isTrue());
    EXPECT_FALSE(threeExpression.isFalse());
    EXPECT_TRUE(threeExpression.getVariables() == std::set<std::string>());
    EXPECT_TRUE(threeExpression.getConstants() == std::set<std::string>());
    
    EXPECT_TRUE(piExpression.getReturnType() == storm::expressions::ExpressionReturnType::Double);
    EXPECT_TRUE(piExpression.isConstant());
    EXPECT_FALSE(piExpression.isTrue());
    EXPECT_FALSE(piExpression.isFalse());
    EXPECT_TRUE(piExpression.getVariables() == std::set<std::string>());
    EXPECT_TRUE(piExpression.getConstants() == std::set<std::string>());
    
    EXPECT_TRUE(boolVarExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    EXPECT_FALSE(boolVarExpression.isConstant());
    EXPECT_FALSE(boolVarExpression.isTrue());
    EXPECT_FALSE(boolVarExpression.isFalse());
    EXPECT_TRUE(boolVarExpression.getVariables() == std::set<std::string>({"x"}));
    EXPECT_TRUE(boolVarExpression.getConstants() == std::set<std::string>());

    EXPECT_TRUE(intVarExpression.getReturnType() == storm::expressions::ExpressionReturnType::Int);
    EXPECT_FALSE(intVarExpression.isConstant());
    EXPECT_FALSE(intVarExpression.isTrue());
    EXPECT_FALSE(intVarExpression.isFalse());
    EXPECT_TRUE(intVarExpression.getVariables() == std::set<std::string>({"y"}));
    EXPECT_TRUE(intVarExpression.getConstants() == std::set<std::string>());

    EXPECT_TRUE(doubleVarExpression.getReturnType() == storm::expressions::ExpressionReturnType::Double);
    EXPECT_FALSE(doubleVarExpression.isConstant());
    EXPECT_FALSE(doubleVarExpression.isTrue());
    EXPECT_FALSE(doubleVarExpression.isFalse());
    EXPECT_TRUE(doubleVarExpression.getVariables() == std::set<std::string>({"z"}));
    EXPECT_TRUE(doubleVarExpression.getConstants() == std::set<std::string>());

    EXPECT_TRUE(boolConstExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    EXPECT_FALSE(boolConstExpression.isConstant());
    EXPECT_FALSE(boolConstExpression.isTrue());
    EXPECT_FALSE(boolConstExpression.isFalse());
    EXPECT_TRUE(boolConstExpression.getVariables() == std::set<std::string>());
    EXPECT_TRUE(boolConstExpression.getConstants() == std::set<std::string>({"a"}));
    
    EXPECT_TRUE(intConstExpression.getReturnType() == storm::expressions::ExpressionReturnType::Int);
    EXPECT_FALSE(intConstExpression.isConstant());
    EXPECT_FALSE(intConstExpression.isTrue());
    EXPECT_FALSE(intConstExpression.isFalse());
    EXPECT_TRUE(intConstExpression.getVariables() == std::set<std::string>());
    EXPECT_TRUE(intConstExpression.getConstants() == std::set<std::string>({"b"}));

    EXPECT_TRUE(doubleConstExpression.getReturnType() == storm::expressions::ExpressionReturnType::Double);
    EXPECT_FALSE(doubleConstExpression.isConstant());
    EXPECT_FALSE(doubleConstExpression.isTrue());
    EXPECT_FALSE(doubleConstExpression.isFalse());
    EXPECT_TRUE(doubleConstExpression.getVariables() == std::set<std::string>());
    EXPECT_TRUE(doubleConstExpression.getConstants() == std::set<std::string>({"c"}));
}

TEST(Expression, OperatorTest) {
    storm::expressions::Expression trueExpression;
    storm::expressions::Expression falseExpression;
    storm::expressions::Expression threeExpression;
    storm::expressions::Expression piExpression;
    storm::expressions::Expression boolVarExpression;
    storm::expressions::Expression intVarExpression;
    storm::expressions::Expression doubleVarExpression;
    storm::expressions::Expression boolConstExpression;
    storm::expressions::Expression intConstExpression;
    storm::expressions::Expression doubleConstExpression;
    
    ASSERT_NO_THROW(trueExpression = storm::expressions::Expression::createTrue());
    ASSERT_NO_THROW(falseExpression = storm::expressions::Expression::createFalse());
    ASSERT_NO_THROW(threeExpression = storm::expressions::Expression::createIntegerLiteral(3));
    ASSERT_NO_THROW(piExpression = storm::expressions::Expression::createDoubleLiteral(3.14));
    ASSERT_NO_THROW(boolVarExpression = storm::expressions::Expression::createBooleanVariable("x"));
    ASSERT_NO_THROW(intVarExpression = storm::expressions::Expression::createIntegerVariable("y"));
    ASSERT_NO_THROW(doubleVarExpression = storm::expressions::Expression::createDoubleVariable("z"));
    ASSERT_NO_THROW(boolConstExpression = storm::expressions::Expression::createBooleanConstant("a"));
    ASSERT_NO_THROW(intConstExpression = storm::expressions::Expression::createIntegerConstant("b"));
    ASSERT_NO_THROW(doubleConstExpression = storm::expressions::Expression::createDoubleConstant("c"));
    
    storm::expressions::Expression tempExpression;

    ASSERT_THROW(tempExpression = trueExpression.ite(falseExpression, piExpression), storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = boolConstExpression.ite(threeExpression, doubleVarExpression));
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Double);
    ASSERT_NO_THROW(tempExpression = boolConstExpression.ite(threeExpression, intVarExpression));
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Int);
    ASSERT_NO_THROW(tempExpression = boolConstExpression.ite(trueExpression, falseExpression));
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    
    ASSERT_THROW(tempExpression = trueExpression + piExpression, storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = threeExpression + threeExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Int);
    ASSERT_NO_THROW(tempExpression = threeExpression + piExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Double);
    ASSERT_NO_THROW(tempExpression = doubleVarExpression + doubleConstExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Double);
    
    ASSERT_THROW(tempExpression = trueExpression - piExpression, storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = threeExpression - threeExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Int);
    ASSERT_NO_THROW(tempExpression = threeExpression - piExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Double);
    ASSERT_NO_THROW(tempExpression = doubleVarExpression - doubleConstExpression);
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
    ASSERT_NO_THROW(tempExpression = intVarExpression * intConstExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Int);

    ASSERT_THROW(tempExpression = trueExpression / piExpression, storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = threeExpression / threeExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Int);
    ASSERT_NO_THROW(tempExpression = threeExpression / piExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Double);
    ASSERT_NO_THROW(tempExpression = doubleVarExpression / intConstExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Double);

    ASSERT_THROW(tempExpression = trueExpression && piExpression, storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = trueExpression && falseExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    ASSERT_NO_THROW(tempExpression = boolVarExpression && boolConstExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);

    ASSERT_THROW(tempExpression = trueExpression || piExpression, storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = trueExpression || falseExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    ASSERT_NO_THROW(tempExpression = boolVarExpression || boolConstExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);

    ASSERT_THROW(tempExpression = !threeExpression, storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = !trueExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    ASSERT_NO_THROW(tempExpression = !boolVarExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);

    ASSERT_THROW(tempExpression = trueExpression == piExpression, storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = threeExpression == threeExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    ASSERT_NO_THROW(tempExpression = intVarExpression == doubleConstExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);

    ASSERT_THROW(tempExpression = trueExpression != piExpression, storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = threeExpression != threeExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    ASSERT_NO_THROW(tempExpression = intVarExpression != doubleConstExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);

    ASSERT_THROW(tempExpression = trueExpression > piExpression, storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = threeExpression > threeExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    ASSERT_NO_THROW(tempExpression = intVarExpression > doubleConstExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    
    ASSERT_THROW(tempExpression = trueExpression >= piExpression, storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = threeExpression >= threeExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    ASSERT_NO_THROW(tempExpression = intVarExpression >= doubleConstExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    
    ASSERT_THROW(tempExpression = trueExpression < piExpression, storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = threeExpression < threeExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    ASSERT_NO_THROW(tempExpression = intVarExpression < doubleConstExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    
    ASSERT_THROW(tempExpression = trueExpression <= piExpression, storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = threeExpression <= threeExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    ASSERT_NO_THROW(tempExpression = intVarExpression <= doubleConstExpression);
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    
    ASSERT_THROW(tempExpression = storm::expressions::Expression::minimum(trueExpression, piExpression), storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = storm::expressions::Expression::minimum(threeExpression, threeExpression));
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Int);
    ASSERT_NO_THROW(tempExpression = storm::expressions::Expression::minimum(intVarExpression, doubleConstExpression));
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Double);

    ASSERT_THROW(tempExpression = storm::expressions::Expression::maximum(trueExpression, piExpression), storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = storm::expressions::Expression::maximum(threeExpression, threeExpression));
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Int);
    ASSERT_NO_THROW(tempExpression = storm::expressions::Expression::maximum(intVarExpression, doubleConstExpression));
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Double);
    
    ASSERT_THROW(tempExpression = trueExpression.implies(piExpression), storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = trueExpression.implies(falseExpression));
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    ASSERT_NO_THROW(tempExpression = boolVarExpression.implies(boolConstExpression));
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    
    ASSERT_THROW(tempExpression = trueExpression.iff(piExpression), storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = trueExpression.iff(falseExpression));
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    ASSERT_NO_THROW(tempExpression = boolVarExpression.iff(boolConstExpression));
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Bool);
    
    ASSERT_THROW(tempExpression = trueExpression.floor(), storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = threeExpression.floor());
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Int);
    ASSERT_NO_THROW(tempExpression = doubleConstExpression.floor());
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Int);

    ASSERT_THROW(tempExpression = trueExpression.ceil(), storm::exceptions::InvalidTypeException);
    ASSERT_NO_THROW(tempExpression = threeExpression.ceil());
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Int);
    ASSERT_NO_THROW(tempExpression = doubleConstExpression.ceil());
    EXPECT_TRUE(tempExpression.getReturnType() == storm::expressions::ExpressionReturnType::Int);
}

TEST(Expression, SubstitutionTest) {
    storm::expressions::Expression trueExpression;
    storm::expressions::Expression falseExpression;
    storm::expressions::Expression threeExpression;
    storm::expressions::Expression piExpression;
    storm::expressions::Expression boolVarExpression;
    storm::expressions::Expression intVarExpression;
    storm::expressions::Expression doubleVarExpression;
    storm::expressions::Expression boolConstExpression;
    storm::expressions::Expression intConstExpression;
    storm::expressions::Expression doubleConstExpression;
    
    ASSERT_NO_THROW(trueExpression = storm::expressions::Expression::createTrue());
    ASSERT_NO_THROW(falseExpression = storm::expressions::Expression::createFalse());
    ASSERT_NO_THROW(threeExpression = storm::expressions::Expression::createIntegerLiteral(3));
    ASSERT_NO_THROW(piExpression = storm::expressions::Expression::createDoubleLiteral(3.14));
    ASSERT_NO_THROW(boolVarExpression = storm::expressions::Expression::createBooleanVariable("x"));
    ASSERT_NO_THROW(intVarExpression = storm::expressions::Expression::createIntegerVariable("y"));
    ASSERT_NO_THROW(doubleVarExpression = storm::expressions::Expression::createDoubleVariable("z"));
    ASSERT_NO_THROW(boolConstExpression = storm::expressions::Expression::createBooleanConstant("a"));
    ASSERT_NO_THROW(intConstExpression = storm::expressions::Expression::createIntegerConstant("b"));
    ASSERT_NO_THROW(doubleConstExpression = storm::expressions::Expression::createDoubleConstant("c"));
    
    storm::expressions::Expression tempExpression;
    ASSERT_NO_THROW(tempExpression = (intVarExpression < threeExpression || boolVarExpression) && boolConstExpression);

    std::map<std::string, storm::expressions::Expression> substution = { std::make_pair("y", doubleConstExpression), std::make_pair("x", storm::expressions::Expression::createTrue()), std::make_pair("a", storm::expressions::Expression::createTrue()) };
    storm::expressions::Expression substitutedExpression;
    ASSERT_NO_THROW(substitutedExpression = tempExpression.substitute<std::map>(substution));
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
    storm::expressions::Expression boolConstExpression;
    storm::expressions::Expression intConstExpression;
    storm::expressions::Expression doubleConstExpression;
    
    ASSERT_NO_THROW(trueExpression = storm::expressions::Expression::createTrue());
    ASSERT_NO_THROW(falseExpression = storm::expressions::Expression::createFalse());
    ASSERT_NO_THROW(threeExpression = storm::expressions::Expression::createIntegerLiteral(3));
    ASSERT_NO_THROW(piExpression = storm::expressions::Expression::createDoubleLiteral(3.14));
    ASSERT_NO_THROW(boolVarExpression = storm::expressions::Expression::createBooleanVariable("x"));
    ASSERT_NO_THROW(intVarExpression = storm::expressions::Expression::createIntegerVariable("y"));
    ASSERT_NO_THROW(doubleVarExpression = storm::expressions::Expression::createDoubleVariable("z"));
    ASSERT_NO_THROW(boolConstExpression = storm::expressions::Expression::createBooleanConstant("a"));
    ASSERT_NO_THROW(intConstExpression = storm::expressions::Expression::createIntegerConstant("b"));
    ASSERT_NO_THROW(doubleConstExpression = storm::expressions::Expression::createDoubleConstant("c"));
    
    storm::expressions::Expression tempExpression;
    
    ASSERT_NO_THROW(tempExpression = (intVarExpression < threeExpression || boolVarExpression) && boolConstExpression);
    
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
    EXPECT_TRUE(tempExpression.evaluateAsBool(&valuation));
    ASSERT_NO_THROW(valuation.setIntegerValue("y", 3));
    EXPECT_FALSE(tempExpression.evaluateAsBool(&valuation));
    
    ASSERT_NO_THROW(tempExpression = ((intVarExpression < threeExpression).ite(trueExpression, falseExpression)));
    ASSERT_THROW(tempExpression.evaluateAsDouble(&valuation), storm::exceptions::InvalidTypeException);
    ASSERT_THROW(tempExpression.evaluateAsInt(&valuation), storm::exceptions::InvalidTypeException);
    EXPECT_FALSE(tempExpression.evaluateAsBool(&valuation));
}