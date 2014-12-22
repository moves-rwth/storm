#include "gtest/gtest.h"
#include "storm-config.h"

#ifdef STORM_HAVE_MSAT
#include "mathsat.h"
#include "src/adapters/MathsatExpressionAdapter.h"
#include "src/settings/SettingsManager.h"

TEST(MathsatExpressionAdapter, StormToMathsatBasic) {
    msat_config config = msat_create_config();
    msat_env env = msat_create_env(config);
    ASSERT_FALSE(MSAT_ERROR_ENV(env));
    msat_destroy_config(config);

    storm::adapters::MathsatExpressionAdapter adapter(env);
    storm::adapters::MathsatExpressionAdapter adapter2(env, false);
    
    storm::expressions::Expression exprTrue = storm::expressions::Expression::createTrue();
    msat_term msatTrue = msat_make_true(env);
    msat_term conjecture;
    ASSERT_NO_THROW(conjecture = msat_make_not(env, msat_make_iff(env, msatTrue, adapter.translateExpression(exprTrue))));
    msat_assert_formula(env, conjecture);
    ASSERT_TRUE(msat_solve(env) == msat_result::MSAT_UNSAT);
    msat_reset_env(env);
    
    storm::expressions::Expression exprFalse = storm::expressions::Expression::createFalse();
    msat_term msatFalse = msat_make_false(env);
    ASSERT_NO_THROW(conjecture = msat_make_not(env, msat_make_iff(env, adapter.translateExpression(exprFalse), msatFalse)));
    msat_assert_formula(env, conjecture);
    ASSERT_TRUE(msat_solve(env) == msat_result::MSAT_UNSAT);
    msat_reset_env(env);
    
    storm::expressions::Expression exprConjunction = (storm::expressions::Expression::createBooleanVariable("x") && storm::expressions::Expression::createBooleanVariable("y"));
    msat_term msatConjunction = msat_make_and(env, msat_make_constant(env, msat_declare_function(env, "x", msat_get_bool_type(env))), msat_make_constant(env, msat_declare_function(env, "y", msat_get_bool_type(env))));
    
    // Variables not yet created in adapter.
    ASSERT_THROW(adapter2.translateExpression(exprConjunction), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(conjecture = msat_make_not(env, msat_make_iff(env, adapter.translateExpression(exprConjunction), msatConjunction)));
    msat_assert_formula(env, conjecture);
    ASSERT_TRUE(msat_solve(env) == msat_result::MSAT_UNSAT);
    msat_reset_env(env);
    
    storm::expressions::Expression exprNor = !(storm::expressions::Expression::createBooleanVariable("x") || storm::expressions::Expression::createBooleanVariable("y"));
    msat_term msatNor = msat_make_not(env, msat_make_or(env, msat_make_constant(env, msat_declare_function(env, "x", msat_get_bool_type(env))), msat_make_constant(env, msat_declare_function(env, "y", msat_get_bool_type(env)))));
    ASSERT_NO_THROW(adapter.translateExpression(exprNor)); // Variables already created in adapter.
    ASSERT_NO_THROW(conjecture = msat_make_not(env, msat_make_iff(env, adapter.translateExpression(exprNor), msatNor)));
    msat_assert_formula(env, conjecture);
    ASSERT_TRUE(msat_solve(env) == msat_result::MSAT_UNSAT);
    msat_reset_env(env);
}

TEST(MathsatExpressionAdapter, StormToMathsatInteger) {
    msat_config config = msat_create_config();
    msat_env env = msat_create_env(config);
    ASSERT_FALSE(MSAT_ERROR_ENV(env));
    msat_destroy_config(config);
    
    storm::adapters::MathsatExpressionAdapter adapter(env);
    
    storm::expressions::Expression exprAdd = (storm::expressions::Expression::createIntegerVariable("x") + storm::expressions::Expression::createIntegerVariable("y") < -storm::expressions::Expression::createIntegerVariable("y"));
    msat_decl xDecl = msat_declare_function(env, "x", msat_get_integer_type(env));
    msat_term x = msat_make_constant(env, xDecl);
    msat_decl yDecl = msat_declare_function(env, "y", msat_get_integer_type(env));
    msat_term y = msat_make_constant(env, yDecl);
    msat_term minusY = msat_make_times(env, msat_make_number(env, "-1"), y);
    msat_term msatAdd = msat_make_plus(env, x, y);
    msat_term msatLess = msat_make_and(env, msat_make_leq(env, msatAdd, minusY), msat_make_not(env, msat_make_equal(env, msatAdd, minusY)));
    msat_term conjecture;
    ASSERT_NO_THROW(conjecture = msat_make_not(env, msat_make_iff(env, adapter.translateExpression(exprAdd), msatLess)));
    msat_assert_formula(env, conjecture);
    ASSERT_TRUE(msat_solve(env) == msat_result::MSAT_UNSAT);
    msat_reset_env(env);
    
    storm::expressions::Expression exprMult = !(storm::expressions::Expression::createIntegerVariable("x") * storm::expressions::Expression::createIntegerVariable("y") == storm::expressions::Expression::createIntegerVariable("y"));
    msat_term msatTimes = msat_make_times(env, x, y);
    msat_term msatNotEqual = msat_make_not(env, msat_make_equal(env, msatTimes, y));
    ASSERT_NO_THROW(conjecture = msat_make_not(env, msat_make_iff(env, adapter.translateExpression(exprMult), msatNotEqual)));
    msat_assert_formula(env, conjecture);
    ASSERT_TRUE(msat_solve(env) == msat_result::MSAT_UNSAT);
    msat_reset_env(env);
}

TEST(MathsatExpressionAdapter, StormToMathsatReal) {
    msat_config config = msat_create_config();
    msat_env env = msat_create_env(config);
    ASSERT_FALSE(MSAT_ERROR_ENV(env));
    msat_destroy_config(config);
    
    storm::adapters::MathsatExpressionAdapter adapter(env);
    
    storm::expressions::Expression exprAdd = (storm::expressions::Expression::createDoubleVariable("x") + storm::expressions::Expression::createDoubleVariable("y") < -storm::expressions::Expression::createDoubleVariable("y"));
    msat_decl xDecl = msat_declare_function(env, "x", msat_get_rational_type(env));
    msat_term x = msat_make_constant(env, xDecl);
    msat_decl yDecl = msat_declare_function(env, "y", msat_get_rational_type(env));
    msat_term y = msat_make_constant(env, yDecl);
    msat_term minusY = msat_make_times(env, msat_make_number(env, "-1"), y);
    msat_term msatAdd = msat_make_plus(env, x, y);
    msat_term msatLess = msat_make_and(env, msat_make_leq(env, msatAdd, minusY), msat_make_not(env, msat_make_equal(env, msatAdd, minusY)));
    msat_term conjecture;
    ASSERT_NO_THROW(conjecture = msat_make_not(env, msat_make_iff(env, adapter.translateExpression(exprAdd), msatLess)));
    msat_assert_formula(env, conjecture);
    ASSERT_TRUE(msat_solve(env) == msat_result::MSAT_UNSAT);
    msat_reset_env(env);
    
    storm::expressions::Expression exprMult = !(storm::expressions::Expression::createDoubleVariable("x") * storm::expressions::Expression::createDoubleVariable("y") == storm::expressions::Expression::createDoubleVariable("y"));
    msat_term msatTimes = msat_make_times(env, x, y);
    msat_term msatNotEqual = msat_make_not(env, msat_make_equal(env, msatTimes, y));
    ASSERT_NO_THROW(conjecture = msat_make_not(env, msat_make_iff(env, adapter.translateExpression(exprMult), msatNotEqual)));
    msat_assert_formula(env, conjecture);
    ASSERT_TRUE(msat_solve(env) == msat_result::MSAT_UNSAT);
    msat_reset_env(env);
}

TEST(MathsatExpressionAdapter, StormToMathsatFloorCeil) {
    msat_config config = msat_create_config();
    msat_env env = msat_create_env(config);
    ASSERT_FALSE(MSAT_ERROR_ENV(env));
    msat_destroy_config(config);
    
    storm::adapters::MathsatExpressionAdapter adapter(env);
    
    storm::expressions::Expression exprFloor = ((storm::expressions::Expression::createDoubleVariable("d").floor()) == storm::expressions::Expression::createIntegerVariable("i") && storm::expressions::Expression::createDoubleVariable("d") > storm::expressions::Expression::createDoubleLiteral(4.1) && storm::expressions::Expression::createDoubleVariable("d") < storm::expressions::Expression::createDoubleLiteral(4.991));
    
    msat_decl iDecl = msat_declare_function(env, "i", msat_get_integer_type(env));
    msat_term i = msat_make_constant(env, iDecl);
    msat_term msatEqualsFour = msat_make_equal(env, msat_make_number(env, "4"), i);
    msat_term conjecture;
    msat_term translatedExpr = adapter.translateExpression(exprFloor);
    msat_term msatIff = msat_make_iff(env, translatedExpr, msatEqualsFour);
    ASSERT_NO_THROW(conjecture = msat_make_not(env, msatIff));
    msat_assert_formula(env, conjecture);
    
    // It is not logically equivalent, so this should be satisfiable.
    ASSERT_TRUE(msat_solve(env) == msat_result::MSAT_SAT);
    msat_reset_env(env);
    ASSERT_NO_THROW(conjecture = msat_make_not(env, msat_make_or(env, msat_make_not(env, adapter.translateExpression(exprFloor)), msatEqualsFour)));
    msat_assert_formula(env, conjecture);
    
    // However, the left part implies the right one, which is why this should be unsatisfiable.
    ASSERT_TRUE(msat_solve(env) == msat_result::MSAT_UNSAT);
    msat_reset_env(env);
    
    storm::expressions::Expression exprCeil = ((storm::expressions::Expression::createDoubleVariable("d").ceil()) == storm::expressions::Expression::createIntegerVariable("i") && storm::expressions::Expression::createDoubleVariable("d") > storm::expressions::Expression::createDoubleLiteral(4.1) && storm::expressions::Expression::createDoubleVariable("d") < storm::expressions::Expression::createDoubleLiteral(4.991));
    msat_term msatEqualsFive = msat_make_equal(env, msat_make_number(env, "5"), i);
    
    ASSERT_NO_THROW(conjecture = msat_make_not(env, msat_make_iff(env, adapter.translateExpression(exprFloor), msatEqualsFive)));
    msat_assert_formula(env, conjecture);
    
    // It is not logically equivalent, so this should be satisfiable.
    ASSERT_TRUE(msat_solve(env) == msat_result::MSAT_SAT);
    msat_reset_env(env);
    ASSERT_NO_THROW(conjecture = msat_make_or(env, msat_make_not(env, adapter.translateExpression(exprFloor)), msatEqualsFive));
    msat_assert_formula(env, conjecture);
    msat_reset_env(env);
}

TEST(MathsatExpressionAdapter, MathsatToStormBasic) {
    msat_config config = msat_create_config();
    msat_env env = msat_create_env(config);
    ASSERT_FALSE(MSAT_ERROR_ENV(env));
    msat_destroy_config(config);
    
    unsigned args = 2;
    
    storm::adapters::MathsatExpressionAdapter adapter(env);
    
    msat_term msatTrue = msat_make_true(env);
    storm::expressions::Expression exprTrue;
    ASSERT_NO_THROW(exprTrue = adapter.translateExpression(msatTrue));
    ASSERT_TRUE(exprTrue.isTrue());

    msat_term msatFalse = msat_make_false(env);
    storm::expressions::Expression exprFalse;
    ASSERT_NO_THROW(exprTrue = adapter.translateExpression(msatFalse));
    ASSERT_TRUE(exprTrue.isFalse());
    
    msat_decl xDecl = msat_declare_function(env, "x", msat_get_bool_type(env));
    msat_term x = msat_make_constant(env, xDecl);
    msat_decl yDecl = msat_declare_function(env, "y", msat_get_bool_type(env));
    msat_term y = msat_make_constant(env, yDecl);
    msat_term msatConjunction = msat_make_and(env, x, y);
    storm::expressions::Expression exprConjunction;
    ASSERT_NO_THROW(exprConjunction = adapter.translateExpression(msatConjunction));
    ASSERT_EQ(storm::expressions::OperatorType::And, exprConjunction.getOperator());
    ASSERT_TRUE(exprConjunction.getOperand(0).isVariable());
    ASSERT_EQ("x", exprConjunction.getOperand(0).getIdentifier());
    ASSERT_TRUE(exprConjunction.getOperand(1).isVariable());
    ASSERT_EQ("y", exprConjunction.getOperand(1).getIdentifier());

    msat_term msatNor = msat_make_not(env, msat_make_or(env, x, y));
    storm::expressions::Expression exprNor;
    ASSERT_NO_THROW(exprNor = adapter.translateExpression(msatNor));
    ASSERT_EQ(storm::expressions::OperatorType::Not, exprNor.getOperator());
    ASSERT_EQ(storm::expressions::OperatorType::Or, exprNor.getOperand(0).getOperator());
    ASSERT_TRUE(exprNor.getOperand(0).getOperand(0).isVariable());
    ASSERT_EQ("x", exprNor.getOperand(0).getOperand(0).getIdentifier());
    ASSERT_TRUE(exprNor.getOperand(0).getOperand(1).isVariable());
    ASSERT_EQ("y", exprNor.getOperand(0).getOperand(1).getIdentifier());
}
#endif