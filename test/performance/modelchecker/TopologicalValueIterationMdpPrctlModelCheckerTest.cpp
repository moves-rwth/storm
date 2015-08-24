#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "src/utility/solver.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "src/settings/SettingsManager.h"
#include "src/settings/modules/TopologicalValueIterationEquationSolverSettings.h"
#include "src/settings/SettingMemento.h"
#include "src/parser/AutoParser.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/parser/FormulaParser.h"

TEST(DISABLED_TopologicalValueIterationMdpPrctlModelCheckerTest, AsynchronousLeader) {
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.tra", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.lab", "", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.trans.rew")->as<storm::models::sparse::Mdp<double>>();

    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser parser;
    
	ASSERT_EQ(mdp->getNumberOfStates(), 2095783ull);
	ASSERT_EQ(mdp->getNumberOfTransitions(), 7714385ull);

    storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>> checker(*mdp, std::unique_ptr<storm::utility::solver::MinMaxLinearEquationSolverFactory<double>>(new storm::utility::solver::TopologicalMinMaxLinearEquationSolverFactory<double>()));

    std::shared_ptr<storm::logic::Formula> formula = parser.parseFromString("Pmin=? [F \"elected\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);

	ASSERT_NEAR(1.0, result->asExplicitQuantitativeCheckResult<double>()[0], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());

    formula = parser.parseFromString("Pmax=? [F \"elected\"]");
    
    result = checker.check(*formula);
    ASSERT_NEAR(1.0, result->asExplicitQuantitativeCheckResult<double>()[0], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());

    formula = parser.parseFromString("Pmin=? [F<=25 \"elected\"]");
    
    result = checker.check(*formula);

	ASSERT_NEAR(0.0, result->asExplicitQuantitativeCheckResult<double>()[0], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());

    formula = parser.parseFromString("Pmax=? [F<=25 \"elected\"]");
    
    result = checker.check(*formula);
	ASSERT_NEAR(0.0, result->asExplicitQuantitativeCheckResult<double>()[0], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());

    formula = parser.parseFromString("Rmin=? [F \"elected\"]");
    
    result = checker.check(*formula);
	ASSERT_NEAR(6.172433512, result->asExplicitQuantitativeCheckResult<double>()[0], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());

    formula = parser.parseFromString("Rmax=? [F \"elected\"]");
    
    result = checker.check(*formula);
	ASSERT_NEAR(6.1724344, result->asExplicitQuantitativeCheckResult<double>()[0], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
}

TEST(DISABLED_TopologicalValueIterationMdpPrctlModelCheckerTest, Consensus) {
    // Increase the maximal number of iterations, because the solver does not converge otherwise.
	// This is done in the main cpp unit
    
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.tra", STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.lab", STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.steps.state.rew", "")->as<storm::models::sparse::Mdp<double>>();
    
    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser parser;
    
	ASSERT_EQ(mdp->getNumberOfStates(), 63616ull);
	ASSERT_EQ(mdp->getNumberOfTransitions(), 213472ull);
    
	storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>> checker(*mdp, std::unique_ptr<storm::utility::solver::MinMaxLinearEquationSolverFactory<double>>(new storm::utility::solver::TopologicalMinMaxLinearEquationSolverFactory<double>()));

    std::shared_ptr<storm::logic::Formula> formula = parser.parseFromString("Pmin=? [F \"finished\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    
	ASSERT_NEAR(1.0, result->asExplicitQuantitativeCheckResult<double>()[31168], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("Pmin=? [F \"finished\" & \"all_coins_equal_0\"]");
    
    result = checker.check(*formula);
    ASSERT_NEAR(0.4374282832, result->asExplicitQuantitativeCheckResult<double>()[31168], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("Pmax=? [F \"finished\" & \"all_coins_equal_1\"]");
    
    result = checker.check(*formula);
	ASSERT_NEAR(0.5293286369, result->asExplicitQuantitativeCheckResult<double>()[31168], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("Pmax=? [F \"finished\" & !\"agree\"]");
    
    result = checker.check(*formula);
	ASSERT_NEAR(0.10414097, result->asExplicitQuantitativeCheckResult<double>()[31168], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());

    formula = parser.parseFromString("Pmin=? [F<=50 \"finished\"]");
    
    result = checker.check(*formula);    
	ASSERT_NEAR(0.0, result->asExplicitQuantitativeCheckResult<double>()[31168], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("Pmax=? [F<=50 \"finished\"]");
    
    result = checker.check(*formula);
    ASSERT_NEAR(0.0, result->asExplicitQuantitativeCheckResult<double>()[31168], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());

    formula = parser.parseFromString("Rmin=? [F \"finished\"]");
    
    result = checker.check(*formula);
    ASSERT_NEAR(1725.593313, result->asExplicitQuantitativeCheckResult<double>()[31168], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("Rmax=? [F \"finished\"\"]");
    
    result = checker.check(*formula);
    ASSERT_NEAR(2183.142422, result->asExplicitQuantitativeCheckResult<double>()[31168], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
}