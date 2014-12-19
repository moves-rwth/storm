#include "gtest/gtest.h"
#include "storm-config.h"

#ifdef STORM_HAVE_Z3
#include "src/solver/Z3SmtSolver.h"
#include "src/settings/SettingsManager.h"

TEST(Z3SmtSolver, CheckSat) {
	storm::solver::Z3SmtSolver s;
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

TEST(Z3SmtSolver, CheckUnsat) {
	storm::solver::Z3SmtSolver s;
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
    && c == (a * b)
    && b + a > c;
    
	ASSERT_NO_THROW(s.add(exprFormula));
	ASSERT_NO_THROW(result = s.check());
	ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::Unsat);
}


TEST(Z3SmtSolver, Backtracking) {
	storm::solver::Z3SmtSolver s;
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
    && c == (a * b)
    && b + a > c;
	storm::expressions::Expression exprFormula2 = a >= storm::expressions::Expression::createIntegerLiteral(2);
    
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

TEST(Z3SmtSolver, Assumptions) {
	storm::solver::Z3SmtSolver s;
	storm::solver::SmtSolver::CheckResult result = storm::solver::SmtSolver::CheckResult::Unknown;
    
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

TEST(Z3SmtSolver, GenerateModel) {
	storm::solver::Z3SmtSolver s;
	storm::solver::SmtSolver::CheckResult result;
    
	storm::expressions::Expression a = storm::expressions::Expression::createIntegerVariable("a");
	storm::expressions::Expression b = storm::expressions::Expression::createIntegerVariable("b");
	storm::expressions::Expression c = storm::expressions::Expression::createIntegerVariable("c");
	storm::expressions::Expression exprFormula = a > storm::expressions::Expression::createIntegerLiteral(0)
    && a < storm::expressions::Expression::createIntegerLiteral(5)
    && b > storm::expressions::Expression::createIntegerLiteral(7)
    && c == (a * b)
    && b + a > c;
    
	s.add(exprFormula);
	result = s.check();
	ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::Sat);
    std::shared_ptr<storm::solver::SmtSolver::ModelReference> model = s.getModel();
    int_fast64_t aEval = model->getIntegerValue("a");
	ASSERT_EQ(1, aEval);
}


TEST(Z3SmtSolver, AllSat) {
	storm::solver::Z3SmtSolver s;
	storm::solver::SmtSolver::CheckResult result;
    
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

TEST(Z3SmtSolver, UnsatAssumptions) {
	storm::solver::Z3SmtSolver s;
	storm::solver::SmtSolver::CheckResult result;
    
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
	
	s.add(exprFormula);
	s.add(exprFormula2);
	result = s.checkWithAssumptions({ f2 });
	ASSERT_TRUE(result == storm::solver::SmtSolver::CheckResult::Unsat);
	std::vector<storm::expressions::Expression> unsatCore = s.getUnsatAssumptions();
	ASSERT_EQ(unsatCore.size(), 1);
	ASSERT_TRUE(unsatCore[0].isVariable());
	ASSERT_STREQ("f2", unsatCore[0].getIdentifier().c_str());
}

#endif