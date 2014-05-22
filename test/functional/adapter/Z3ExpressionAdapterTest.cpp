#include "gtest/gtest.h"
#include "storm-config.h"

#include "z3++.h"
#include "src/adapters/Z3ExpressionAdapter.h"
#include "src/settings/Settings.h"

TEST(Z3ExpressionAdapter, StormToZ3Basic) {
#ifdef STORM_HAVE_Z3
	z3::context ctx;
	z3::solver s(ctx);
	z3::expr conjecture = ctx.bool_val(false);

	storm::adapters::Z3ExpressionAdapter adapter(ctx, {});

	storm::expressions::Expression exprTrue = storm::expressions::Expression::createTrue();
	z3::expr z3True = ctx.bool_val(true);
	ASSERT_NO_THROW(conjecture = !z3::expr(ctx, Z3_mk_iff(ctx, adapter.translateExpression(exprTrue), z3True)));
	s.add(conjecture);
	ASSERT_TRUE(s.check() == z3::unsat);
	s.reset();

	storm::expressions::Expression exprFalse = storm::expressions::Expression::createFalse();
	z3::expr z3False = ctx.bool_val(false);
	ASSERT_NO_THROW(conjecture = !z3::expr(ctx, Z3_mk_iff(ctx, adapter.translateExpression(exprFalse), z3False)));
	s.add(conjecture);
	ASSERT_TRUE(s.check() == z3::unsat);
	s.reset();

	storm::expressions::Expression exprConjunction = (storm::expressions::Expression::createBooleanVariable("x") && storm::expressions::Expression::createBooleanVariable("y"));
	z3::expr z3Conjunction = (ctx.bool_const("x") && ctx.bool_const("y"));
	ASSERT_THROW( adapter.translateExpression(exprConjunction, false), std::out_of_range ); //variables not yet created in adapter
	ASSERT_NO_THROW(conjecture = !z3::expr(ctx, Z3_mk_iff(ctx, adapter.translateExpression(exprConjunction, true), z3Conjunction)));
	s.add(conjecture);
	ASSERT_TRUE(s.check() == z3::unsat);
	s.reset();

	storm::expressions::Expression exprNor = !(storm::expressions::Expression::createBooleanVariable("x") || storm::expressions::Expression::createBooleanVariable("y"));
	z3::expr z3Nor = !(ctx.bool_const("x") || ctx.bool_const("y"));
	ASSERT_NO_THROW(adapter.translateExpression(exprNor, false)); //variables already created in adapter
	ASSERT_NO_THROW(conjecture = !z3::expr(ctx, Z3_mk_iff(ctx, adapter.translateExpression(exprNor, true), z3Nor)));
	s.add(conjecture);
	ASSERT_TRUE(s.check() == z3::unsat);
	s.reset();

#else
	ASSERT_TRUE(false) << "StoRM built without Z3 support.";
#endif
}

TEST(Z3ExpressionAdapter, StormToZ3Integer) {
#ifdef STORM_HAVE_Z3
	z3::context ctx;
	z3::solver s(ctx);
	z3::expr conjecture = ctx.bool_val(false);

	storm::adapters::Z3ExpressionAdapter adapter(ctx, {});

	storm::expressions::Expression exprAdd = (storm::expressions::Expression::createIntegerVariable("x") + storm::expressions::Expression::createIntegerVariable("y") < -storm::expressions::Expression::createIntegerVariable("y"));
	z3::expr z3Add = (ctx.int_const("x") + ctx.int_const("y") < -ctx.int_const("y"));
	ASSERT_NO_THROW(conjecture = !z3::expr(ctx, Z3_mk_iff(ctx, adapter.translateExpression(exprAdd, true), z3Add)));
	s.add(conjecture);
	ASSERT_TRUE(s.check() == z3::unsat);
	s.reset();

	storm::expressions::Expression exprMult = !(storm::expressions::Expression::createIntegerVariable("x") * storm::expressions::Expression::createIntegerVariable("y") == storm::expressions::Expression::createIntegerVariable("y"));
	z3::expr z3Mult = !(ctx.int_const("x") * ctx.int_const("y") == ctx.int_const("y"));
	ASSERT_NO_THROW(conjecture = !z3::expr(ctx, Z3_mk_iff(ctx, adapter.translateExpression(exprMult, true), z3Mult)));
	s.add(conjecture);
	ASSERT_TRUE(s.check() == z3::unsat);
	s.reset();

#else
	ASSERT_TRUE(false) << "StoRM built without Z3 support.";
#endif
}

TEST(Z3ExpressionAdapter, StormToZ3Real) {
#ifdef STORM_HAVE_Z3
	z3::context ctx;
	z3::solver s(ctx);
	z3::expr conjecture = ctx.bool_val(false);

	storm::adapters::Z3ExpressionAdapter adapter(ctx, {});

	storm::expressions::Expression exprAdd = (storm::expressions::Expression::createDoubleVariable("x") + storm::expressions::Expression::createDoubleVariable("y") < -storm::expressions::Expression::createDoubleVariable("y"));
	z3::expr z3Add = (ctx.real_const("x") + ctx.real_const("y") < -ctx.real_const("y"));
	ASSERT_NO_THROW(conjecture = !z3::expr(ctx, Z3_mk_iff(ctx, adapter.translateExpression(exprAdd, true), z3Add)));
	s.add(conjecture);
	ASSERT_TRUE(s.check() == z3::unsat);
	s.reset();

	storm::expressions::Expression exprMult = !(storm::expressions::Expression::createDoubleVariable("x") * storm::expressions::Expression::createDoubleVariable("y") == storm::expressions::Expression::createDoubleVariable("y"));
	z3::expr z3Mult = !(ctx.real_const("x") * ctx.real_const("y") == ctx.real_const("y"));
	ASSERT_NO_THROW(conjecture = !z3::expr(ctx, Z3_mk_iff(ctx, adapter.translateExpression(exprMult, true), z3Mult)));
	s.add(conjecture);
	ASSERT_TRUE(s.check() == z3::unsat);
	s.reset();

#else
	ASSERT_TRUE(false) << "StoRM built without Z3 support.";
#endif
}

TEST(Z3ExpressionAdapter, StormToZ3TypeErrors) {
#ifdef STORM_HAVE_Z3
	z3::context ctx;
	z3::solver s(ctx);
	z3::expr conjecture = ctx.bool_val(false);

	storm::adapters::Z3ExpressionAdapter adapter(ctx, {});

	storm::expressions::Expression exprFail1 = (storm::expressions::Expression::createDoubleVariable("x") + storm::expressions::Expression::createIntegerVariable("y") < -storm::expressions::Expression::createDoubleVariable("y"));
	ASSERT_THROW(conjecture = adapter.translateExpression(exprFail1, true), storm::exceptions::InvalidTypeException);

#else
	ASSERT_TRUE(false) << "StoRM built without Z3 support.";
#endif
}

TEST(Z3ExpressionAdapter, StormToZ3FloorCeil) {
#ifdef STORM_HAVE_Z3
	z3::context ctx;
	z3::solver s(ctx);
	z3::expr conjecture = ctx.bool_val(false);

	storm::adapters::Z3ExpressionAdapter adapter(ctx, {});

	storm::expressions::Expression exprFloor = ((storm::expressions::Expression::createDoubleVariable("d").floor()) == storm::expressions::Expression::createIntegerVariable("i") && storm::expressions::Expression::createDoubleVariable("d") > storm::expressions::Expression::createDoubleLiteral(4.1) && storm::expressions::Expression::createDoubleVariable("d") < storm::expressions::Expression::createDoubleLiteral(4.991));
	z3::expr z3Floor = ctx.int_val(4) == ctx.int_const("i");

	//try { adapter.translateExpression(exprFloor, true); }
	//catch (std::exception &e) { std::cout << e.what() << std::endl; }

	ASSERT_NO_THROW(conjecture = !z3::expr(ctx, Z3_mk_iff(ctx, adapter.translateExpression(exprFloor, true), z3Floor)));
	s.add(conjecture);
	ASSERT_TRUE(s.check() == z3::sat); //it is NOT logical equivalent
	s.reset();
	ASSERT_NO_THROW(conjecture = !z3::expr(ctx, Z3_mk_implies(ctx, adapter.translateExpression(exprFloor, true), z3Floor)));
	s.add(conjecture);
	ASSERT_TRUE(s.check() == z3::unsat); //it is NOT logical equivalent
	s.reset();


	storm::expressions::Expression exprCeil = ((storm::expressions::Expression::createDoubleVariable("d").ceil()) == storm::expressions::Expression::createIntegerVariable("i") && storm::expressions::Expression::createDoubleVariable("d") > storm::expressions::Expression::createDoubleLiteral(4.1) && storm::expressions::Expression::createDoubleVariable("d") < storm::expressions::Expression::createDoubleLiteral(4.991));
	z3::expr z3Ceil = ctx.int_val(5) == ctx.int_const("i");

	//try { adapter.translateExpression(exprFloor, true); }
	//catch (std::exception &e) { std::cout << e.what() << std::endl; }

	ASSERT_NO_THROW(conjecture = !z3::expr(ctx, Z3_mk_iff(ctx, adapter.translateExpression(exprCeil, true), z3Ceil)));
	s.add(conjecture);
	ASSERT_TRUE(s.check() == z3::sat); //it is NOT logical equivalent
	s.reset();
	ASSERT_NO_THROW(conjecture = !z3::expr(ctx, Z3_mk_implies(ctx, adapter.translateExpression(exprCeil, true), z3Ceil)));
	s.add(conjecture);
	ASSERT_TRUE(s.check() == z3::unsat); //it is NOT logical equivalent
	s.reset();
#else
    ASSERT_TRUE(false) << "StoRM built without Z3 support.";
#endif
}

TEST(Z3ExpressionAdapter, Z3ToStormBasic) {
#ifdef STORM_HAVE_Z3
	z3::context ctx;

	unsigned args = 2;

	storm::adapters::Z3ExpressionAdapter adapter(ctx, {});

	z3::expr z3True = ctx.bool_val(true);
	storm::expressions::Expression exprTrue;
	ASSERT_NO_THROW(exprTrue = adapter.translateExpression(z3True));
	ASSERT_TRUE(exprTrue.isTrue());

	z3::expr z3False = ctx.bool_val(false);
	storm::expressions::Expression exprFalse;
	ASSERT_NO_THROW(exprFalse = adapter.translateExpression(z3False));
	ASSERT_TRUE(exprFalse.isFalse());

	z3::expr z3Conjunction = (ctx.bool_const("x") && ctx.bool_const("y"));
	storm::expressions::Expression exprConjunction;
	ASSERT_NO_THROW(exprConjunction = adapter.translateExpression(z3Conjunction));
	ASSERT_EQ(storm::expressions::OperatorType::And, exprConjunction.getOperator());
	ASSERT_TRUE(exprConjunction.getOperand(0).isVariable());
	ASSERT_EQ("x", exprConjunction.getOperand(0).getIdentifier());
	ASSERT_TRUE(exprConjunction.getOperand(1).isVariable());
	ASSERT_EQ("y", exprConjunction.getOperand(1).getIdentifier());

	z3::expr z3Nor = !(ctx.bool_const("x") || ctx.bool_const("y"));
	storm::expressions::Expression exprNor;
	ASSERT_NO_THROW(exprNor = adapter.translateExpression(z3Nor));
	ASSERT_EQ(storm::expressions::OperatorType::Not, exprNor.getOperator());
	ASSERT_EQ(storm::expressions::OperatorType::Or, exprNor.getOperand(0).getOperator());
	ASSERT_TRUE(exprNor.getOperand(0).getOperand(0).isVariable());
	ASSERT_EQ("x", exprNor.getOperand(0).getOperand(0).getIdentifier());
	ASSERT_TRUE(exprNor.getOperand(0).getOperand(1).isVariable());
	ASSERT_EQ("y", exprNor.getOperand(0).getOperand(1).getIdentifier());

#else
	ASSERT_TRUE(false) << "StoRM built without Z3 support.";
#endif
}