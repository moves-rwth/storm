#include "gtest/gtest.h"
#include "storm-config.h"

#ifdef STORM_HAVE_MSAT
#include "src/solver/MathsatSmtSolver.h"

TEST(MathsatSmtSolver, CheckSat) {
	storm::solver::MathsatSmtSolver s;
	storm::solver::SmtSolver::CheckResult result = storm::solver::SmtSolver::CheckResult::Unknown;
    
	storm::expressions::Expression exprDeMorgan = !(storm::expressions::Expression::createBooleanVariable("x") && storm::expressions::Expression::createBooleanVariable("y")).iff((!storm::expressions::Expression::createBooleanVariable("x") || !storm::expressions::Expression::createBooleanVariable("y")));
    
	ASSERT_NO_THROW(s.add(exprDeMorgan));
	ASSERT_NO_THROW(result = s.check());
	ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::Sat);
	ASSERT_NO_THROW(s.reset());
    
	storm::expressions::Expression a = storm::expressions::Expression::createIntegerVariable("a");
	storm::expressions::Expression b = storm::expressions::Expression::createIntegerVariable("b");
	storm::expressions::Expression c = storm::expressions::Expression::createIntegerVariable("c");
	storm::expressions::Expression exprFormula = a >= storm::expressions::Expression::createIntegerLiteral(0)
    && a < storm::expressions::Expression::createIntegerLiteral(5)
    && b > storm::expressions::Expression::createIntegerLiteral(7)
    && c == (a * b)
    && b + a > c;
    
	ASSERT_NO_THROW(s.add(exprFormula));
	ASSERT_NO_THROW(result = s.check());
	ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::Sat);
	ASSERT_NO_THROW(s.reset());
}

TEST(MathsatSmtSolver, CheckUnsat) {
	storm::solver::MathsatSmtSolver s;
	storm::solver::SmtSolver::CheckResult result = storm::solver::SmtSolver::CheckResult::Unknown;
    
	storm::expressions::Expression exprDeMorgan = !(storm::expressions::Expression::createBooleanVariable("x") && storm::expressions::Expression::createBooleanVariable("y")).iff( (!storm::expressions::Expression::createBooleanVariable("x") || !storm::expressions::Expression::createBooleanVariable("y")));
    
	ASSERT_NO_THROW(s.add(!exprDeMorgan));
	ASSERT_NO_THROW(result = s.check());
	ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::Unsat);
	ASSERT_NO_THROW(s.reset());
    
	storm::expressions::Expression a = storm::expressions::Expression::createIntegerVariable("a");
	storm::expressions::Expression b = storm::expressions::Expression::createIntegerVariable("b");
	storm::expressions::Expression c = storm::expressions::Expression::createIntegerVariable("c");
	storm::expressions::Expression exprFormula = a >= storm::expressions::Expression::createIntegerLiteral(2)
    && a < storm::expressions::Expression::createIntegerLiteral(5)
    && b > storm::expressions::Expression::createIntegerLiteral(7)
    && c == (a + b + storm::expressions::Expression::createIntegerLiteral(1))
    && b + a > c;
    
	ASSERT_NO_THROW(s.add(exprFormula));
	ASSERT_NO_THROW(result = s.check());
	ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::Unsat);
}

TEST(MathsatSmtSolver, Backtracking) {
    storm::solver::MathsatSmtSolver s;
    storm::solver::SmtSolver::CheckResult result = storm::solver::SmtSolver::CheckResult::Unknown;
    
    storm::expressions::Expression expr1 = storm::expressions::Expression::createTrue();
    storm::expressions::Expression expr2 = storm::expressions::Expression::createFalse();
    storm::expressions::Expression expr3 = storm::expressions::Expression::createFalse();
    
    ASSERT_NO_THROW(s.add(expr1));
    ASSERT_NO_THROW(result = s.check());
    ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::Sat);
    ASSERT_NO_THROW(s.push());
    ASSERT_NO_THROW(s.add(expr2));
    ASSERT_NO_THROW(result = s.check());
    ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::Unsat);
    ASSERT_NO_THROW(s.pop());
    ASSERT_NO_THROW(result = s.check());
    ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::Sat);
    ASSERT_NO_THROW(s.push());
    ASSERT_NO_THROW(s.add(expr2));
    ASSERT_NO_THROW(result = s.check());
    ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::Unsat);
    ASSERT_NO_THROW(s.push());
    ASSERT_NO_THROW(s.add(expr3));
    ASSERT_NO_THROW(result = s.check());
    ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::Unsat);
    ASSERT_NO_THROW(s.pop(2));
    ASSERT_NO_THROW(result = s.check());
    ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::Sat);
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
    
    ASSERT_NO_THROW(s.add(exprFormula));
    ASSERT_NO_THROW(result = s.check());
    ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::Sat);
    ASSERT_NO_THROW(s.push());
    ASSERT_NO_THROW(s.add(exprFormula2));
    ASSERT_NO_THROW(result = s.check());
    ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::Unsat);
    ASSERT_NO_THROW(s.pop());
    ASSERT_NO_THROW(result = s.check());
    ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::Sat);
}

TEST(MathsatSmtSolver, Assumptions) {
    storm::solver::MathsatSmtSolver s;
    storm::solver::SmtSolver::CheckResult result = storm::solver::SmtSolver::CheckResult::Unknown;
    
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
    
    ASSERT_NO_THROW(s.add(exprFormula));
    ASSERT_NO_THROW(result = s.check());
    ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::Sat);
    ASSERT_NO_THROW(s.add(exprFormula2));
    ASSERT_NO_THROW(result = s.check());
    ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::Sat);
    ASSERT_NO_THROW(result = s.checkWithAssumptions({f2}));
    ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::Unsat);
    ASSERT_NO_THROW(result = s.check());
    ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::Sat);
    ASSERT_NO_THROW(result = s.checkWithAssumptions({ !f2 }));
    ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::Sat);
}

TEST(MathsatSmtSolver, GenerateModel) {
    storm::solver::MathsatSmtSolver s;
    storm::solver::SmtSolver::CheckResult result;
    
    storm::expressions::Expression a = storm::expressions::Expression::createIntegerVariable("a");
    storm::expressions::Expression b = storm::expressions::Expression::createIntegerVariable("b");
    storm::expressions::Expression c = storm::expressions::Expression::createIntegerVariable("c");
    storm::expressions::Expression exprFormula = a > storm::expressions::Expression::createIntegerLiteral(0)
    && a < storm::expressions::Expression::createIntegerLiteral(5)
    && b > storm::expressions::Expression::createIntegerLiteral(7)
    && c == (a + b - storm::expressions::Expression::createIntegerLiteral(1))
    && b + a > c;
    
    s.add(exprFormula);
    result = s.check();
    ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::Sat);
    std::shared_ptr<storm::solver::SmtSolver::ModelReference> model = s.getModel();
    int_fast64_t aEval = model->getIntegerValue("a");
    int_fast64_t bEval = model->getIntegerValue("b");
    int_fast64_t cEval = model->getIntegerValue("c");
    ASSERT_TRUE(cEval == aEval + bEval - 1);
}

TEST(MathsatSmtSolver, AllSat) {
    storm::solver::MathsatSmtSolver s;
    storm::solver::SmtSolver::CheckResult result = storm::solver::SmtSolver::CheckResult::Unknown;
    
    storm::expressions::Expression a = storm::expressions::Expression::createIntegerVariable("a");
    storm::expressions::Expression b = storm::expressions::Expression::createIntegerVariable("b");
    storm::expressions::Expression x = storm::expressions::Expression::createBooleanVariable("x");
    storm::expressions::Expression y = storm::expressions::Expression::createBooleanVariable("y");
    storm::expressions::Expression z = storm::expressions::Expression::createBooleanVariable("z");
    storm::expressions::Expression exprFormula1 = x.implies(a > storm::expressions::Expression::createIntegerLiteral(5));
    storm::expressions::Expression exprFormula2 = y.implies(a < storm::expressions::Expression::createIntegerLiteral(5));
    storm::expressions::Expression exprFormula3 = z.implies(b < storm::expressions::Expression::createIntegerLiteral(5));
    
    s.add(exprFormula1);
    s.add(exprFormula2);
    s.add(exprFormula3);
    
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

TEST(MathsatSmtSolver, UnsatAssumptions) {
    storm::solver::MathsatSmtSolver s(storm::solver::MathsatSmtSolver::Options(false, true, false));
    storm::solver::SmtSolver::CheckResult result = storm::solver::SmtSolver::CheckResult::Unknown;
    
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
    
    s.add(exprFormula);
    s.add(exprFormula2);
    result = s.checkWithAssumptions({ f2 });
    ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::Unsat);
    std::vector<storm::expressions::Expression> unsatCore = s.getUnsatAssumptions();
    ASSERT_EQ(unsatCore.size(), 1);
    ASSERT_TRUE(unsatCore[0].isVariable());
    ASSERT_STREQ("f2", unsatCore[0].getIdentifier().c_str());
}

TEST(MathsatSmtSolver, InterpolationTest) {
    storm::solver::MathsatSmtSolver s(storm::solver::MathsatSmtSolver::Options(false, false, true));
    storm::solver::SmtSolver::CheckResult result = storm::solver::SmtSolver::CheckResult::Unknown;
    
    storm::expressions::Expression a = storm::expressions::Expression::createIntegerVariable("a");
    storm::expressions::Expression b = storm::expressions::Expression::createIntegerVariable("b");
    storm::expressions::Expression c = storm::expressions::Expression::createIntegerVariable("c");
    storm::expressions::Expression exprFormula = a > b;
    storm::expressions::Expression exprFormula2 = b > c;
    storm::expressions::Expression exprFormula3 = c > a;
    
    s.setInterpolationGroup(0);
    s.add(exprFormula);
    s.setInterpolationGroup(1);
    s.add(exprFormula2);
    s.setInterpolationGroup(2);
    s.add(exprFormula3);
    
    ASSERT_NO_THROW(result = s.check());
    ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::Unsat);
    
    storm::expressions::Expression interpol;
    ASSERT_NO_THROW(interpol = s.getInterpolant({0, 1}));
    
    storm::solver::MathsatSmtSolver s2;
    
    ASSERT_NO_THROW(s2.add(!(exprFormula && exprFormula2).implies(interpol)));
    ASSERT_NO_THROW(result = s2.check());
    ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::Unsat);
    
    ASSERT_NO_THROW(s2.reset());
    ASSERT_NO_THROW(s2.add(interpol && exprFormula3));
    ASSERT_NO_THROW(result = s2.check());
    ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::Unsat);
}

#endif