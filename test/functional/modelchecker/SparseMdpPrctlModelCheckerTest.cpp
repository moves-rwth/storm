#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/solver/NativeNondeterministicLinearEquationSolver.h"
#include "src/settings/SettingsManager.h"
#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "src/parser/AutoParser.h"

TEST(SparseMdpPrctlModelCheckerTest, Dice) {
	std::shared_ptr<storm::models::AbstractModel<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.tra", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.lab", "", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.trans.rew");
    
	ASSERT_EQ(abstractModel->getType(), storm::models::MDP);
    
	std::shared_ptr<storm::models::Mdp<double>> mdp = abstractModel->as<storm::models::Mdp<double>>();
    
	ASSERT_EQ(mdp->getNumberOfStates(), 169ull);
	ASSERT_EQ(mdp->getNumberOfTransitions(), 436ull);
    
	storm::modelchecker::prctl::SparseMdpPrctlModelChecker<double> mc(*mdp, std::shared_ptr<storm::solver::NativeNondeterministicLinearEquationSolver<double>>(new storm::solver::NativeNondeterministicLinearEquationSolver<double>()));
    
	auto apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("two");
	auto eventuallyFormula = std::make_shared<storm::properties::prctl::Eventually<double>>(apFormula);
    
	std::vector<double> result = mc.checkOptimizingOperator(*eventuallyFormula, true);
    
	ASSERT_LT(std::abs(result[0] - 0.0277777612209320068), storm::settings::nativeEquationSolverSettings().getPrecision());
    
	result = mc.checkOptimizingOperator(*eventuallyFormula, false);

	ASSERT_LT(std::abs(result[0] - 0.0277777612209320068), storm::settings::nativeEquationSolverSettings().getPrecision());
    
	apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("three");
	eventuallyFormula = std::make_shared<storm::properties::prctl::Eventually<double>>(apFormula);
    
	result = mc.checkOptimizingOperator(*eventuallyFormula, true);
    
	ASSERT_LT(std::abs(result[0] - 0.0555555224418640136), storm::settings::nativeEquationSolverSettings().getPrecision());
    
	result = mc.checkOptimizingOperator(*eventuallyFormula, false);

	ASSERT_LT(std::abs(result[0] - 0.0555555224418640136), storm::settings::nativeEquationSolverSettings().getPrecision());
    
	apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("four");
	eventuallyFormula = std::make_shared<storm::properties::prctl::Eventually<double>>(apFormula);
    
	result = mc.checkOptimizingOperator(*eventuallyFormula, true);
    
	ASSERT_LT(std::abs(result[0] - 0.083333283662796020508), storm::settings::nativeEquationSolverSettings().getPrecision());

	result = mc.checkOptimizingOperator(*eventuallyFormula, false);
    
	ASSERT_LT(std::abs(result[0] - 0.083333283662796020508), storm::settings::nativeEquationSolverSettings().getPrecision());

	apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("done");
	auto reachabilityRewardFormula = std::make_shared<storm::properties::prctl::ReachabilityReward<double>>(apFormula);

	result = mc.checkOptimizingOperator(*reachabilityRewardFormula, true);

	ASSERT_LT(std::abs(result[0] - 7.333329499), storm::settings::nativeEquationSolverSettings().getPrecision());

	result = mc.checkOptimizingOperator(*reachabilityRewardFormula, false);

	ASSERT_LT(std::abs(result[0] - 7.333329499), storm::settings::nativeEquationSolverSettings().getPrecision());
    
	abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.tra", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.lab", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.state.rew", "");
    
	ASSERT_EQ(abstractModel->getType(), storm::models::MDP);
    
	std::shared_ptr<storm::models::Mdp<double>> stateRewardMdp = abstractModel->as<storm::models::Mdp<double>>();
    
    storm::modelchecker::prctl::SparseMdpPrctlModelChecker<double> stateRewardModelChecker(*stateRewardMdp, std::shared_ptr<storm::solver::NativeNondeterministicLinearEquationSolver<double>>(new storm::solver::NativeNondeterministicLinearEquationSolver<double>()));

	apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("done");
	reachabilityRewardFormula = std::make_shared<storm::properties::prctl::ReachabilityReward<double>>(apFormula);

	result = stateRewardModelChecker.checkOptimizingOperator(*reachabilityRewardFormula, true);
    
	ASSERT_LT(std::abs(result[0] - 7.333329499), storm::settings::nativeEquationSolverSettings().getPrecision());

	result = stateRewardModelChecker.checkOptimizingOperator(*reachabilityRewardFormula, false);

	ASSERT_LT(std::abs(result[0] - 7.333329499), storm::settings::nativeEquationSolverSettings().getPrecision());
    
	abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.tra", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.lab", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.state.rew", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.trans.rew");
    
	ASSERT_EQ(abstractModel->getType(), storm::models::MDP);
    
	std::shared_ptr<storm::models::Mdp<double>> stateAndTransitionRewardMdp = abstractModel->as<storm::models::Mdp<double>>();
    
	storm::modelchecker::prctl::SparseMdpPrctlModelChecker<double> stateAndTransitionRewardModelChecker(*stateAndTransitionRewardMdp, std::shared_ptr<storm::solver::NativeNondeterministicLinearEquationSolver<double>>(new storm::solver::NativeNondeterministicLinearEquationSolver<double>()));
    
	apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("done");
	reachabilityRewardFormula = std::make_shared<storm::properties::prctl::ReachabilityReward<double>>(apFormula);
    
	result = stateAndTransitionRewardModelChecker.checkOptimizingOperator(*reachabilityRewardFormula, true);
    
	ASSERT_LT(std::abs(result[0] - 14.666658998), storm::settings::nativeEquationSolverSettings().getPrecision());

	result = stateAndTransitionRewardModelChecker.checkOptimizingOperator(*reachabilityRewardFormula, false);

	ASSERT_LT(std::abs(result[0] - 14.666658998), storm::settings::nativeEquationSolverSettings().getPrecision());
}

TEST(SparseMdpPrctlModelCheckerTest, AsynchronousLeader) {
	std::shared_ptr<storm::models::AbstractModel<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader4.tra", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader4.lab", "", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader4.trans.rew");

	ASSERT_EQ(storm::models::MDP, abstractModel->getType());

	std::shared_ptr<storm::models::Mdp<double>> mdp = abstractModel->as<storm::models::Mdp<double>>();

	ASSERT_EQ(3172ull, mdp->getNumberOfStates());
	ASSERT_EQ(7144ull, mdp->getNumberOfTransitions());

	storm::modelchecker::prctl::SparseMdpPrctlModelChecker<double> mc(*mdp, std::shared_ptr<storm::solver::NativeNondeterministicLinearEquationSolver<double>>(new storm::solver::NativeNondeterministicLinearEquationSolver<double>()));

	auto apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("elected");
	auto eventuallyFormula = std::make_shared<storm::properties::prctl::Eventually<double>>(apFormula);

	std::vector<double> result = mc.checkOptimizingOperator(*eventuallyFormula, true);

	ASSERT_LT(std::abs(result[0] - 1), storm::settings::nativeEquationSolverSettings().getPrecision());

	result = mc.checkOptimizingOperator(*eventuallyFormula, false);

	ASSERT_LT(std::abs(result[0] - 1), storm::settings::nativeEquationSolverSettings().getPrecision());

	apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("elected");
	auto boundedEventuallyFormula = std::make_shared<storm::properties::prctl::BoundedEventually<double>>(apFormula, 25);

	result = mc.checkOptimizingOperator(*boundedEventuallyFormula, true);

	ASSERT_LT(std::abs(result[0] - 0.0625), storm::settings::nativeEquationSolverSettings().getPrecision());

	result = mc.checkOptimizingOperator(*boundedEventuallyFormula, false);

	ASSERT_LT(std::abs(result[0] - 0.0625), storm::settings::nativeEquationSolverSettings().getPrecision());

	apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("elected");
	auto reachabilityRewardFormula = std::make_shared<storm::properties::prctl::ReachabilityReward<double>>(apFormula);

	result = mc.checkOptimizingOperator(*reachabilityRewardFormula, true);

	ASSERT_LT(std::abs(result[0] - 4.285689611), storm::settings::nativeEquationSolverSettings().getPrecision());

	result = mc.checkOptimizingOperator(*reachabilityRewardFormula, false);

	ASSERT_LT(std::abs(result[0] - 4.285689611), storm::settings::nativeEquationSolverSettings().getPrecision());
}
