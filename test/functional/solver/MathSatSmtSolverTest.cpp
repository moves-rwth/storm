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

TEST(MathSatSmtSolver, Backtracking) {
    storm::solver::MathSatSmtSolver s;
    storm::solver::SmtSolver::CheckResult result = storm::solver::SmtSolver::CheckResult::UNKNOWN;
    
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
    && c == (a + b - storm::expressions::Expression::createIntegerLiteral(1))
    && b + a > c;
    storm::expressions::Expression exprFormula2 = c > a + b + storm::expressions::Expression::createIntegerLiteral(1);
    
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
}

TEST(MathSatSmtSolver, Assumptions) {
    storm::solver::MathSatSmtSolver s;
    storm::solver::SmtSolver::CheckResult result = storm::solver::SmtSolver::CheckResult::UNKNOWN;
    
    storm::expressions::Expression a = storm::expressions::Expression::createIntegerVariable("a");
    storm::expressions::Expression b = storm::expressions::Expression::createIntegerVariable("b");
    storm::expressions::Expression c = storm::expressions::Expression::createIntegerVariable("c");
    storm::expressions::Expression exprFormula = a >= storm::expressions::Expression::createIntegerLiteral(0)
    && a < storm::expressions::Expression::createIntegerLiteral(5)
    && b > storm::expressions::Expression::createIntegerLiteral(7)
    && c == (a + b - storm::expressions::Expression::createIntegerLiteral(1))
    && b + a > c;
    storm::expressions::Expression f2 = storm::expressions::Expression::createBooleanVariable("f2");
    storm::expressions::Expression exprFormula2 = f2.implies(c > a + b + storm::expressions::Expression::createIntegerLiteral(1));
    
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
}

TEST(MathSatSmtSolver, GenerateModel) {
    storm::solver::MathSatSmtSolver s;
    storm::solver::SmtSolver::CheckResult result;
    
    storm::expressions::Expression a = storm::expressions::Expression::createIntegerVariable("a");
    storm::expressions::Expression b = storm::expressions::Expression::createIntegerVariable("b");
    storm::expressions::Expression c = storm::expressions::Expression::createIntegerVariable("c");
    storm::expressions::Expression exprFormula = a > storm::expressions::Expression::createIntegerLiteral(0)
    && a < storm::expressions::Expression::createIntegerLiteral(5)
    && b > storm::expressions::Expression::createIntegerLiteral(7)
    && c == (a + b - storm::expressions::Expression::createIntegerLiteral(1))
    && b + a > c;
    
    (s.assertExpression(exprFormula));
    (result = s.check());
    ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::SAT);
    storm::expressions::SimpleValuation model;
    (model = s.getModel());
    int_fast64_t a_eval = model.getIntegerValue("a");
    int_fast64_t b_eval = model.getIntegerValue("b");
    int_fast64_t c_eval = model.getIntegerValue("c");
    ASSERT_TRUE(c_eval == a_eval + b_eval - 1);
}

TEST(MathSatSmtSolver, AllSat) {
    storm::solver::MathSatSmtSolver s;
    storm::solver::SmtSolver::CheckResult result = storm::solver::SmtSolver::CheckResult::UNKNOWN;
    
    storm::expressions::Expression a = storm::expressions::Expression::createIntegerVariable("a");
    storm::expressions::Expression b = storm::expressions::Expression::createIntegerVariable("b");
    storm::expressions::Expression x = storm::expressions::Expression::createBooleanVariable("x");
    storm::expressions::Expression y = storm::expressions::Expression::createBooleanVariable("y");
    storm::expressions::Expression z = storm::expressions::Expression::createBooleanVariable("z");
    storm::expressions::Expression exprFormula1 = x.implies(a > storm::expressions::Expression::createIntegerLiteral(5));
    storm::expressions::Expression exprFormula2 = y.implies(a < storm::expressions::Expression::createIntegerLiteral(5));
    storm::expressions::Expression exprFormula3 = z.implies(b < storm::expressions::Expression::createIntegerLiteral(5));
    
    (s.assertExpression(exprFormula1));
    (s.assertExpression(exprFormula2));
    (s.assertExpression(exprFormula3));
    
    std::vector<storm::expressions::SimpleValuation> valuations = s.allSat({x,y});
    
    ASSERT_TRUE(valuations.size() == 3);
    for (int i = 0; i < valuations.size(); ++i) {
        ASSERT_EQ(valuations[i].getNumberOfIdentifiers(), 2);
        ASSERT_TRUE(valuations[i].containsBooleanIdentifier("x"));
        ASSERT_TRUE(valuations[i].containsBooleanIdentifier("y"));
    }
    for (int i = 0; i < valuations.size(); ++i) {
        ASSERT_FALSE(valuations[i].getBooleanValue("x") && valuations[i].getBooleanValue("y"));
        
        for (int j = i+1; j < valuations.size(); ++j) {
            ASSERT_TRUE((valuations[i].getBooleanValue("x") != valuations[j].getBooleanValue("x")) || (valuations[i].getBooleanValue("y") != valuations[j].getBooleanValue("y")));
        }
    }
}

TEST(MathSatSmtSolver, UnsatAssumptions) {
    storm::solver::MathSatSmtSolver s;
    storm::solver::SmtSolver::CheckResult result = storm::solver::SmtSolver::CheckResult::UNKNOWN;
    
    storm::expressions::Expression a = storm::expressions::Expression::createIntegerVariable("a");
    storm::expressions::Expression b = storm::expressions::Expression::createIntegerVariable("b");
    storm::expressions::Expression c = storm::expressions::Expression::createIntegerVariable("c");
    storm::expressions::Expression exprFormula = a >= storm::expressions::Expression::createIntegerLiteral(0)
    && a < storm::expressions::Expression::createIntegerLiteral(5)
    && b > storm::expressions::Expression::createIntegerLiteral(7)
    && c == (a + b - storm::expressions::Expression::createIntegerLiteral(1))
    && b + a > c;
    storm::expressions::Expression f2 = storm::expressions::Expression::createBooleanVariable("f2");
    storm::expressions::Expression exprFormula2 = f2.implies(c > a + b + storm::expressions::Expression::createIntegerLiteral(1));
    
    (s.assertExpression(exprFormula));
    (s.assertExpression(exprFormula2));
    (result = s.checkWithAssumptions({ f2 }));
    ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::UNSAT);
    std::vector<storm::expressions::Expression> unsatCore = s.getUnsatAssumptions();
    ASSERT_EQ(unsatCore.size(), 1);
    ASSERT_TRUE(unsatCore[0].isVariable());
    ASSERT_STREQ("f2", unsatCore[0].getIdentifier().c_str());
}

#endif