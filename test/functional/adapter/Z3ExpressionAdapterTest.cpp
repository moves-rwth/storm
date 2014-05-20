#include "gtest/gtest.h"
#include "storm-config.h"

#include "z3++.h"
#include "src/adapters/Z3ExpressionAdapter.h"
#include "src/settings/Settings.h"

TEST(Z3ExpressionAdapter, StormToZ3) {
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

