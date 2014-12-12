#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/settings/SettingsManager.h"
#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "src/solver/NativeNondeterministicLinearEquationSolver.h"
#include "src/parser/AutoParser.h"

TEST(SparseMdpPrctlModelCheckerTest, AsynchronousLeader) {
	std::shared_ptr<storm::models::AbstractModel<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.tra", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.lab", "", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.trans.rew");

	ASSERT_EQ(abstractModel->getType(), storm::models::MDP);

	std::shared_ptr<storm::models::Mdp<double>> mdp = abstractModel->as<storm::models::Mdp<double>>();

	ASSERT_EQ(2095783ull, mdp->getNumberOfStates());
	ASSERT_EQ(7714385ull, mdp->getNumberOfTransitions());

	storm::modelchecker::prctl::SparseMdpPrctlModelChecker<double> mc(*mdp, std::shared_ptr<storm::solver::NativeNondeterministicLinearEquationSolver<double>>(new storm::solver::NativeNondeterministicLinearEquationSolver<double>()));

	auto apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("elected");
	auto eventuallyFormula = std::make_shared<storm::properties::prctl::Eventually<double>>(apFormula);

	std::vector<double> result = mc.checkOptimizingOperator(*eventuallyFormula, true);

    ASSERT_LT(std::abs(result[0] - 1.0), storm::settings::nativeEquationSolverSettings().getPrecision());

	result = mc.checkOptimizingOperator(*eventuallyFormula, false);

	ASSERT_LT(std::abs(result[0] - 1.0), storm::settings::nativeEquationSolverSettings().getPrecision());

	apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("elected");
	auto boundedEventuallyFormula = std::make_shared<storm::properties::prctl::BoundedEventually<double>>(apFormula, 25);

	result = mc.checkOptimizingOperator(*boundedEventuallyFormula, true);

	ASSERT_LT(std::abs(result[0] - 0.0), storm::settings::nativeEquationSolverSettings().getPrecision());

	result = mc.checkOptimizingOperator(*boundedEventuallyFormula, false);

	ASSERT_LT(std::abs(result[0] - 0.0), storm::settings::nativeEquationSolverSettings().getPrecision());

	apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("elected");
	auto reachabilityRewardFormula = std::make_shared<storm::properties::prctl::ReachabilityReward<double>>(apFormula);

	result = mc.checkOptimizingOperator(*reachabilityRewardFormula, true);

	ASSERT_LT(std::abs(result[0] - 6.172433512), storm::settings::nativeEquationSolverSettings().getPrecision());

	result = mc.checkOptimizingOperator(*reachabilityRewardFormula, false);

	ASSERT_LT(std::abs(result[0] - 6.1724344), storm::settings::nativeEquationSolverSettings().getPrecision());
}

TEST(SparseMdpPrctlModelCheckerTest, Consensus) {
    // Increase the maximal number of iterations, because the solver does not converge otherwise.
	// This is done in the main cpp unit
    
	std::shared_ptr<storm::models::AbstractModel<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.tra", STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.lab", STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.steps.state.rew", "");
    
	ASSERT_EQ(abstractModel->getType(), storm::models::MDP);
    
	std::shared_ptr<storm::models::Mdp<double>> mdp = abstractModel->as<storm::models::Mdp<double>>();
    
	ASSERT_EQ(63616ull, mdp->getNumberOfStates());
	ASSERT_EQ(213472ull, mdp->getNumberOfTransitions());
    
	storm::modelchecker::prctl::SparseMdpPrctlModelChecker<double> mc(*mdp, std::shared_ptr<storm::solver::NativeNondeterministicLinearEquationSolver<double>>(new storm::solver::NativeNondeterministicLinearEquationSolver<double>()));
    
    auto apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("finished");
	auto eventuallyFormula = std::make_shared<storm::properties::prctl::Eventually<double>>(apFormula);
    
	std::vector<double> result = mc.checkOptimizingOperator(*eventuallyFormula, true);
    
	ASSERT_LT(std::abs(result[31168] - 1.0), storm::settings::nativeEquationSolverSettings().getPrecision());

    apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("finished");
    auto apFormula2 = std::make_shared<storm::properties::prctl::Ap<double>>("all_coins_equal_0");
    auto andFormula = std::make_shared<storm::properties::prctl::And<double>>(apFormula, apFormula2);
	eventuallyFormula = std::make_shared<storm::properties::prctl::Eventually<double>>(andFormula);
    
	result = mc.checkOptimizingOperator(*eventuallyFormula, true);

	ASSERT_LT(std::abs(result[31168] - 0.4374282832), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("finished");
    apFormula2 = std::make_shared<storm::properties::prctl::Ap<double>>("all_coins_equal_1");
    andFormula = std::make_shared<storm::properties::prctl::And<double>>(apFormula, apFormula2);
    eventuallyFormula = std::make_shared<storm::properties::prctl::Eventually<double>>(andFormula);
    
	result = mc.checkOptimizingOperator(*eventuallyFormula, false);
    
	ASSERT_LT(std::abs(result[31168] - 0.5293286369), storm::settings::nativeEquationSolverSettings().getPrecision());

    apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("finished");
    apFormula2 = std::make_shared<storm::properties::prctl::Ap<double>>("agree");
    auto notFormula = std::make_shared<storm::properties::prctl::Not<double>>(apFormula2);
    andFormula = std::make_shared<storm::properties::prctl::And<double>>(apFormula, notFormula);
    eventuallyFormula = std::make_shared<storm::properties::prctl::Eventually<double>>(andFormula);
    
	result = mc.checkOptimizingOperator(*eventuallyFormula, false);
    
	ASSERT_LT(std::abs(result[31168] - 0.10414097), storm::settings::nativeEquationSolverSettings().getPrecision());

    apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("finished");
	auto boundedEventuallyFormula = std::make_shared<storm::properties::prctl::BoundedEventually<double>>(apFormula, 50ull);
    
	result = mc.checkOptimizingOperator(*boundedEventuallyFormula, true);
    
	ASSERT_LT(std::abs(result[31168] - 0.0), storm::settings::nativeEquationSolverSettings().getPrecision());
    
	result = mc.checkOptimizingOperator(*boundedEventuallyFormula, false);

	ASSERT_LT(std::abs(result[31168] - 0.0), storm::settings::nativeEquationSolverSettings().getPrecision());

    apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("finished");
	auto reachabilityRewardFormula = std::make_shared<storm::properties::prctl::ReachabilityReward<double>>(apFormula);
    
	result = mc.checkOptimizingOperator(*reachabilityRewardFormula, true);
    
	ASSERT_LT(std::abs(result[31168] - 1725.593313), storm::settings::nativeEquationSolverSettings().getPrecision());

	result = mc.checkOptimizingOperator(*reachabilityRewardFormula, false);

	ASSERT_LT(std::abs(result[31168] - 2183.142422), storm::settings::nativeEquationSolverSettings().getPrecision());
}
