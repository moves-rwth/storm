#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/solver/Z3SmtSolver.h"
#include "src/settings/Settings.h"

TEST(Z3SmtSolver, CheckSatisfiability) {
#ifdef STORM_HAVE_Z3
	storm::solver::Z3SmtSolver s;

	storm::expressions::Expression exprDeMorgan = !(storm::expressions::Expression::createBooleanVariable("x") && storm::expressions::Expression::createBooleanVariable("y")).iff( (!storm::expressions::Expression::createBooleanVariable("x") || !storm::expressions::Expression::createBooleanVariable("y")));

	ASSERT_NO_THROW(s.assertExpression(!exprDeMorgan));
	storm::solver::Z3SmtSolver::CheckResult result;
	ASSERT_NO_THROW(result = s.check());
	ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::UNSAT);

#else
    ASSERT_TRUE(false) << "StoRM built without Z3 support.";
#endif
}
