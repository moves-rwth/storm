#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/solver/Z3SmtSolver.h"
#include "src/settings/Settings.h"

TEST(Z3SmtSolver, CheckSat) {
#ifdef STORM_HAVE_Z3
	storm::solver::Z3SmtSolver s;
	storm::solver::Z3SmtSolver::CheckResult result;

	storm::expressions::Expression exprDeMorgan = !(storm::expressions::Expression::createBooleanVariable("x") && storm::expressions::Expression::createBooleanVariable("y")).iff((!storm::expressions::Expression::createBooleanVariable("x") || !storm::expressions::Expression::createBooleanVariable("y")));

	ASSERT_NO_THROW(s.assertExpression(exprDeMorgan));
	ASSERT_NO_THROW(result = s.check());
	ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::SAT);
	ASSERT_NO_THROW(s.reset());

	storm::expressions::Expression a = storm::expressions::Expression::createIntegerVariable("a");
	storm::expressions::Expression b = storm::expressions::Expression::createIntegerVariable("b");
	storm::expressions::Expression c = storm::expressions::Expression::createIntegerVariable("c");
	storm::expressions::Expression exprFormula = a >= storm::expressions::Expression::createIntegerLiteral(0)
		&& a < storm::expressions::Expression::createIntegerLiteral(5)
		&& b > storm::expressions::Expression::createIntegerLiteral(7)
		&& c == (a * b)
		&& b + a > c;

	ASSERT_NO_THROW(s.assertExpression(exprFormula));
	ASSERT_NO_THROW(result = s.check());
	ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::SAT);
	ASSERT_NO_THROW(s.reset());

#else
	ASSERT_TRUE(false) << "StoRM built without Z3 support.";
#endif
}

TEST(Z3SmtSolver, CheckUnsat) {
#ifdef STORM_HAVE_Z3
	storm::solver::Z3SmtSolver s;
	storm::solver::Z3SmtSolver::CheckResult result;

	storm::expressions::Expression exprDeMorgan = !(storm::expressions::Expression::createBooleanVariable("x") && storm::expressions::Expression::createBooleanVariable("y")).iff( (!storm::expressions::Expression::createBooleanVariable("x") || !storm::expressions::Expression::createBooleanVariable("y")));

	ASSERT_NO_THROW(s.assertExpression(!exprDeMorgan));
	ASSERT_NO_THROW(result = s.check());
	ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::UNSAT);
	ASSERT_NO_THROW(s.reset());


	storm::expressions::Expression a = storm::expressions::Expression::createIntegerVariable("a");
	storm::expressions::Expression b = storm::expressions::Expression::createIntegerVariable("b");
	storm::expressions::Expression c = storm::expressions::Expression::createIntegerVariable("c");
	storm::expressions::Expression exprFormula = a >= storm::expressions::Expression::createIntegerLiteral(2)
		&& a < storm::expressions::Expression::createIntegerLiteral(5)
		&& b > storm::expressions::Expression::createIntegerLiteral(7)
		&& c == (a * b)
		&& b + a > c;

	ASSERT_NO_THROW(s.assertExpression(exprFormula));
	ASSERT_NO_THROW(result = s.check());
	ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::UNSAT);

#else
    ASSERT_TRUE(false) << "StoRM built without Z3 support.";
#endif
}


TEST(Z3SmtSolver, Backtracking) {
#ifdef STORM_HAVE_Z3
	storm::solver::Z3SmtSolver s;
	storm::solver::Z3SmtSolver::CheckResult result;

	storm::expressions::Expression expr1 = storm::expressions::Expression::createTrue();
	storm::expressions::Expression expr2 = storm::expressions::Expression::createFalse();
	storm::expressions::Expression expr3 = storm::expressions::Expression::createFalse();

	ASSERT_NO_THROW(s.assertExpression(expr1));
	ASSERT_NO_THROW(result = s.check());
	ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::SAT);
	ASSERT_NO_THROW(s.push());
	ASSERT_NO_THROW(s.assertExpression(expr2));
	ASSERT_NO_THROW(result = s.check());
	ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::UNSAT);
	ASSERT_NO_THROW(s.pop());
	ASSERT_NO_THROW(result = s.check());
	ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::SAT);
	ASSERT_NO_THROW(s.push());
	ASSERT_NO_THROW(s.assertExpression(expr2));
	ASSERT_NO_THROW(result = s.check());
	ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::UNSAT);
	ASSERT_NO_THROW(s.push());
	ASSERT_NO_THROW(s.assertExpression(expr3));
	ASSERT_NO_THROW(result = s.check());
	ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::UNSAT);
	ASSERT_NO_THROW(s.pop(2));
	ASSERT_NO_THROW(result = s.check());
	ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::SAT);
	ASSERT_NO_THROW(s.reset());


	storm::expressions::Expression a = storm::expressions::Expression::createIntegerVariable("a");
	storm::expressions::Expression b = storm::expressions::Expression::createIntegerVariable("b");
	storm::expressions::Expression c = storm::expressions::Expression::createIntegerVariable("c");
	storm::expressions::Expression exprFormula = a >= storm::expressions::Expression::createIntegerLiteral(0)
		&& a < storm::expressions::Expression::createIntegerLiteral(5)
		&& b > storm::expressions::Expression::createIntegerLiteral(7)
		&& c == (a * b)
		&& b + a > c;
	storm::expressions::Expression exprFormula2 = a >= storm::expressions::Expression::createIntegerLiteral(2);

	ASSERT_NO_THROW(s.assertExpression(exprFormula));
	ASSERT_NO_THROW(result = s.check());
	ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::SAT);
	ASSERT_NO_THROW(s.push());
	ASSERT_NO_THROW(s.assertExpression(exprFormula2));
	ASSERT_NO_THROW(result = s.check());
	ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::UNSAT);
	ASSERT_NO_THROW(s.pop());
	ASSERT_NO_THROW(result = s.check());
	ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::SAT);

#else
	ASSERT_TRUE(false) << "StoRM built without Z3 support.";
#endif
}

TEST(Z3SmtSolver, Assumptions) {
#ifdef STORM_HAVE_Z3
	storm::solver::Z3SmtSolver s;
	storm::solver::Z3SmtSolver::CheckResult result;

	storm::expressions::Expression a = storm::expressions::Expression::createIntegerVariable("a");
	storm::expressions::Expression b = storm::expressions::Expression::createIntegerVariable("b");
	storm::expressions::Expression c = storm::expressions::Expression::createIntegerVariable("c");
	storm::expressions::Expression exprFormula = a >= storm::expressions::Expression::createIntegerLiteral(0)
		&& a < storm::expressions::Expression::createIntegerLiteral(5)
		&& b > storm::expressions::Expression::createIntegerLiteral(7)
		&& c == (a * b)
		&& b + a > c;
	storm::expressions::Expression f2 = storm::expressions::Expression::createBooleanVariable("f2");
	storm::expressions::Expression exprFormula2 = f2.implies(a >= storm::expressions::Expression::createIntegerLiteral(2));

	ASSERT_NO_THROW(s.assertExpression(exprFormula));
	ASSERT_NO_THROW(result = s.check());
	ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::SAT);
	ASSERT_NO_THROW(s.assertExpression(exprFormula2));
	ASSERT_NO_THROW(result = s.check());
	ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::SAT);
	ASSERT_NO_THROW(result = s.checkWithAssumptions({f2}));
	ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::UNSAT);
	ASSERT_NO_THROW(result = s.check());
	ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::SAT);
	ASSERT_NO_THROW(result = s.checkWithAssumptions({ !f2 }));
	ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::SAT);

#else
	ASSERT_TRUE(false) << "StoRM built without Z3 support.";
#endif
}

TEST(Z3SmtSolver, GenerateModel) {
#ifdef STORM_HAVE_Z3
	storm::solver::Z3SmtSolver s;
	storm::solver::Z3SmtSolver::CheckResult result;

	storm::expressions::Expression a = storm::expressions::Expression::createIntegerVariable("a");
	storm::expressions::Expression b = storm::expressions::Expression::createIntegerVariable("b");
	storm::expressions::Expression c = storm::expressions::Expression::createIntegerVariable("c");
	storm::expressions::Expression exprFormula = a >= storm::expressions::Expression::createIntegerLiteral(0)
		&& a < storm::expressions::Expression::createIntegerLiteral(5)
		&& b > storm::expressions::Expression::createIntegerLiteral(7)
		&& c == (a * b)
		&& b + a > c;

	(s.assertExpression(exprFormula));
	(result = s.check());
	ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::SAT);
	storm::expressions::SimpleValuation model;
	(model = s.getModel());
	int_fast64_t a_eval;
	(a_eval = model.getIntegerValue("a"));
	ASSERT_EQ(1, a_eval);

#else
	ASSERT_TRUE(false) << "StoRM built without Z3 support.";
#endif
}