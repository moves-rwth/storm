#include "gtest/gtest.h"
#include "storm-config.h"

#ifdef STORM_HAVE_MSAT
#include "src/solver/MathSatSmtSolver.h"

TEST(MathSatSmtSolver, CheckSat) {
	storm::solver::MathSatSmtSolver s;
	storm::solver::SmtSolver::CheckResult result = storm::solver::SmtSolver::CheckResult::UNKNOWN;
    
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
}

TEST(MathSatSmtSolver, CheckUnsat) {
	storm::solver::MathSatSmtSolver s;
	storm::solver::SmtSolver::CheckResult result = storm::solver::SmtSolver::CheckResult::UNKNOWN;
    
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
    && c == (a + b + storm::expressions::Expression::createIntegerLiteral(1))
    && b + a > c;
    
	ASSERT_NO_THROW(s.assertExpression(exprFormula));
	ASSERT_NO_THROW(result = s.check());
	ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::UNSAT);
}

#endif