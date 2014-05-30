#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/solver/NativeNondeterministicLinearEquationSolver.h"
#include "src/settings/Settings.h"
#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "src/parser/AutoParser.h"

TEST(SparseMdpPrctlModelCheckerTest, Dice) {
	storm::settings::Settings* s = storm::settings::Settings::getInstance();
	std::shared_ptr<storm::models::AbstractModel<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.tra", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.lab", "", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.trans.rew");
    
	ASSERT_EQ(abstractModel->getType(), storm::models::MDP);
    
	std::shared_ptr<storm::models::Mdp<double>> mdp = abstractModel->as<storm::models::Mdp<double>>();
    
	ASSERT_EQ(mdp->getNumberOfStates(), 169ull);
	ASSERT_EQ(mdp->getNumberOfTransitions(), 436ull);
    
	storm::modelchecker::prctl::SparseMdpPrctlModelChecker<double> mc(*mdp, std::shared_ptr<storm::solver::NativeNondeterministicLinearEquationSolver<double>>(new storm::solver::NativeNondeterministicLinearEquationSolver<double>()));
    
	storm::property::prctl::Ap<double>* apFormula = new storm::property::prctl::Ap<double>("two");
	storm::property::prctl::Eventually<double>* eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
    
	std::vector<double> result = mc.checkOptimizingOperator(*eventuallyFormula, true);
    
	ASSERT_LT(std::abs(result[0] - 0.0277777612209320068), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    
	result = mc.checkOptimizingOperator(*eventuallyFormula, false);

	ASSERT_LT(std::abs(result[0] - 0.0277777612209320068), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete eventuallyFormula;
    
	apFormula = new storm::property::prctl::Ap<double>("three");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
    
	result = mc.checkOptimizingOperator(*eventuallyFormula, true);
    
	ASSERT_LT(std::abs(result[0] - 0.0555555224418640136), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    
	result = mc.checkOptimizingOperator(*eventuallyFormula, false);

	ASSERT_LT(std::abs(result[0] - 0.0555555224418640136), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete eventuallyFormula;
    
	apFormula = new storm::property::prctl::Ap<double>("four");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
    
	result = mc.checkOptimizingOperator(*eventuallyFormula, true);
    
	ASSERT_LT(std::abs(result[0] - 0.083333283662796020508), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	result = mc.checkOptimizingOperator(*eventuallyFormula, false);
    
	ASSERT_LT(std::abs(result[0] - 0.083333283662796020508), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete eventuallyFormula;

	apFormula = new storm::property::prctl::Ap<double>("done");
	storm::property::prctl::ReachabilityReward<double>* reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);

	result = mc.checkOptimizingOperator(*reachabilityRewardFormula, true);

	ASSERT_LT(std::abs(result[0] - 7.333329499), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	result = mc.checkOptimizingOperator(*reachabilityRewardFormula, false);

	ASSERT_LT(std::abs(result[0] - 7.333329499), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete reachabilityRewardFormula;
    
	abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.tra", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.lab", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.state.rew", "");
    
	ASSERT_EQ(abstractModel->getType(), storm::models::MDP);
    
	std::shared_ptr<storm::models::Mdp<double>> stateRewardMdp = abstractModel->as<storm::models::Mdp<double>>();
    
    storm::modelchecker::prctl::SparseMdpPrctlModelChecker<double> stateRewardModelChecker(*stateRewardMdp, std::shared_ptr<storm::solver::NativeNondeterministicLinearEquationSolver<double>>(new storm::solver::NativeNondeterministicLinearEquationSolver<double>()));

	apFormula = new storm::property::prctl::Ap<double>("done");
	reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);

	result = stateRewardModelChecker.checkOptimizingOperator(*reachabilityRewardFormula, true);
    
	ASSERT_LT(std::abs(result[0] - 7.333329499), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	result = stateRewardModelChecker.checkOptimizingOperator(*reachabilityRewardFormula, false);

	ASSERT_LT(std::abs(result[0] - 7.333329499), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete reachabilityRewardFormula;
    
	abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.tra", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.lab", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.state.rew", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.trans.rew");
    
	ASSERT_EQ(abstractModel->getType(), storm::models::MDP);
    
	std::shared_ptr<storm::models::Mdp<double>> stateAndTransitionRewardMdp = abstractModel->as<storm::models::Mdp<double>>();
    
	storm::modelchecker::prctl::SparseMdpPrctlModelChecker<double> stateAndTransitionRewardModelChecker(*stateAndTransitionRewardMdp, std::shared_ptr<storm::solver::NativeNondeterministicLinearEquationSolver<double>>(new storm::solver::NativeNondeterministicLinearEquationSolver<double>()));
    
	apFormula = new storm::property::prctl::Ap<double>("done");
	reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
    
	result = stateAndTransitionRewardModelChecker.checkOptimizingOperator(*reachabilityRewardFormula, true);
    
	ASSERT_LT(std::abs(result[0] - 14.666658998), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	result = stateAndTransitionRewardModelChecker.checkOptimizingOperator(*reachabilityRewardFormula, false);

	ASSERT_LT(std::abs(result[0] - 14.666658998), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete reachabilityRewardFormula;
}

TEST(SparseMdpPrctlModelCheckerTest, AsynchronousLeader) {
	storm::settings::Settings* s = storm::settings::Settings::getInstance();
	std::shared_ptr<storm::models::AbstractModel<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader4.tra", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader4.lab", "", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader4.trans.rew");

	ASSERT_EQ(storm::models::MDP, abstractModel->getType());

	std::shared_ptr<storm::models::Mdp<double>> mdp = abstractModel->as<storm::models::Mdp<double>>();

	ASSERT_EQ(3172ull, mdp->getNumberOfStates());
	ASSERT_EQ(7144ull, mdp->getNumberOfTransitions());

	storm::modelchecker::prctl::SparseMdpPrctlModelChecker<double> mc(*mdp, std::shared_ptr<storm::solver::NativeNondeterministicLinearEquationSolver<double>>(new storm::solver::NativeNondeterministicLinearEquationSolver<double>()));

	storm::property::prctl::Ap<double>* apFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::Eventually<double>* eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);

	std::vector<double> result = mc.checkOptimizingOperator(*eventuallyFormula, true);

	ASSERT_LT(std::abs(result[0] - 1), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	result = mc.checkOptimizingOperator(*eventuallyFormula, false);

	ASSERT_LT(std::abs(result[0] - 1), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete eventuallyFormula;

	apFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::BoundedEventually<double>* boundedEventuallyFormula = new storm::property::prctl::BoundedEventually<double>(apFormula, 25);

	result = mc.checkOptimizingOperator(*boundedEventuallyFormula, true);

	ASSERT_LT(std::abs(result[0] - 0.0625), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	result = mc.checkOptimizingOperator(*boundedEventuallyFormula, false);

	ASSERT_LT(std::abs(result[0] - 0.0625), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete boundedEventuallyFormula;

	apFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::ReachabilityReward<double>* reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);

	result = mc.checkOptimizingOperator(*reachabilityRewardFormula, true);

	ASSERT_LT(std::abs(result[0] - 4.285689611), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	result = mc.checkOptimizingOperator(*reachabilityRewardFormula, false);

	ASSERT_LT(std::abs(result[0] - 4.285689611), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete reachabilityRewardFormula;
}
