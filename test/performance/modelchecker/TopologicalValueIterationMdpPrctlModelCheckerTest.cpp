#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/settings/Settings.h"
#include "src/modelchecker/prctl/TopologicalValueIterationMdpPrctlModelChecker.h"
#include "src/solver/NativeNondeterministicLinearEquationSolver.h"
#include "src/parser/AutoParser.h"

TEST(TopologicalValueIterationMdpPrctlModelCheckerTest, AsynchronousLeader) {
	storm::settings::Settings* s = storm::settings::Settings::getInstance();
	storm::parser::AutoParser<double> parser(STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.tra", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.lab", "", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.trans.rew");

	ASSERT_EQ(parser.getType(), storm::models::MDP);

	std::shared_ptr<storm::models::Mdp<double>> mdp = parser.getModel<storm::models::Mdp<double>>();

	ASSERT_EQ(mdp->getNumberOfStates(), 2095783ull);
	ASSERT_EQ(mdp->getNumberOfTransitions(), 7714385ull);

	storm::modelchecker::prctl::TopologicalValueIterationMdpPrctlModelChecker<double> mc(*mdp);

	storm::property::prctl::Ap<double>* apFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::Eventually<double>* eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	storm::property::prctl::ProbabilisticNoBoundOperator<double>* probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);

	std::vector<double> result = mc.checkNoBoundOperator(*probFormula);

	ASSERT_LT(std::abs(result[0] - 1.0), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
	delete probFormula;

	apFormula = new storm::property::prctl::Ap<double>("elected");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);

	result = mc.checkNoBoundOperator(*probFormula);

	ASSERT_LT(std::abs(result[0] - 1.0), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
	delete probFormula;

	apFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::BoundedEventually<double>* boundedEventuallyFormula = new storm::property::prctl::BoundedEventually<double>(apFormula, 25);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(boundedEventuallyFormula, true);

	result = mc.checkNoBoundOperator(*probFormula);

	ASSERT_LT(std::abs(result[0] - 0.0), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
	delete probFormula;

	apFormula = new storm::property::prctl::Ap<double>("elected");
	boundedEventuallyFormula = new storm::property::prctl::BoundedEventually<double>(apFormula, 25);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(boundedEventuallyFormula, false);

	result = mc.checkNoBoundOperator(*probFormula);

	ASSERT_LT(std::abs(result[0] - 0.0), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
	delete probFormula;

	apFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::ReachabilityReward<double>* reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	storm::property::prctl::RewardNoBoundOperator<double>* rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, true);

	result = mc.checkNoBoundOperator(*rewardFormula);

	ASSERT_LT(std::abs(result[0] - 6.172433512), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
	delete rewardFormula;

	apFormula = new storm::property::prctl::Ap<double>("elected");
	reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, false);

	result = mc.checkNoBoundOperator(*rewardFormula);

	ASSERT_LT(std::abs(result[0] - 6.1724344), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
	delete rewardFormula;
}

TEST(TopologicalValueIterationMdpPrctlModelCheckerTest, Consensus) {
	storm::settings::Settings* s = storm::settings::Settings::getInstance();
    // Increase the maximal number of iterations, because the solver does not converge otherwise.
	// This is done in the main cpp unit
    
	storm::parser::AutoParser<double> parser(STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.tra", STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.lab", STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.steps.state.rew", "");
    
	ASSERT_EQ(parser.getType(), storm::models::MDP);
    
	std::shared_ptr<storm::models::Mdp<double>> mdp = parser.getModel<storm::models::Mdp<double>>();
    
	ASSERT_EQ(mdp->getNumberOfStates(), 63616ull);
	ASSERT_EQ(mdp->getNumberOfTransitions(), 213472ull);
    
	storm::modelchecker::prctl::TopologicalValueIterationMdpPrctlModelChecker<double> mc(*mdp);
    
    storm::property::prctl::Ap<double>* apFormula = new storm::property::prctl::Ap<double>("finished");
	storm::property::prctl::Eventually<double>* eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	storm::property::prctl::ProbabilisticNoBoundOperator<double>* probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);
    
	std::vector<double> result = mc.checkNoBoundOperator(*probFormula);
    
	ASSERT_LT(std::abs(result[31168] - 1.0), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    delete probFormula;
    
    apFormula = new storm::property::prctl::Ap<double>("finished");
    storm::property::prctl::Ap<double>* apFormula2 = new storm::property::prctl::Ap<double>("all_coins_equal_0");
    storm::property::prctl::And<double>* andFormula = new storm::property::prctl::And<double>(apFormula, apFormula2);
	eventuallyFormula = new storm::property::prctl::Eventually<double>(andFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);
    
	result = mc.checkNoBoundOperator(*probFormula);

	ASSERT_LT(std::abs(result[31168] - 0.4374282832), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    delete probFormula;
    
    apFormula = new storm::property::prctl::Ap<double>("finished");
    apFormula2 = new storm::property::prctl::Ap<double>("all_coins_equal_1");
    andFormula = new storm::property::prctl::And<double>(apFormula, apFormula2);
    eventuallyFormula = new storm::property::prctl::Eventually<double>(andFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);
    
	result = mc.checkNoBoundOperator(*probFormula);
    
	ASSERT_LT(std::abs(result[31168] - 0.5293286369), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    delete probFormula;
    
    apFormula = new storm::property::prctl::Ap<double>("finished");
    apFormula2 = new storm::property::prctl::Ap<double>("agree");
    storm::property::prctl::Not<double>* notFormula = new storm::property::prctl::Not<double>(apFormula2);
    andFormula = new storm::property::prctl::And<double>(apFormula, notFormula);
    eventuallyFormula = new storm::property::prctl::Eventually<double>(andFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);
    
	result = mc.checkNoBoundOperator(*probFormula);
    
	ASSERT_LT(std::abs(result[31168] - 0.10414097), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    delete probFormula;
    
    apFormula = new storm::property::prctl::Ap<double>("finished");
	storm::property::prctl::BoundedEventually<double>* boundedEventuallyFormula = new storm::property::prctl::BoundedEventually<double>(apFormula, 50ull);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(boundedEventuallyFormula, true);
    
	result = mc.checkNoBoundOperator(*probFormula);
    
	ASSERT_LT(std::abs(result[31168] - 0.0), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    delete probFormula;
    
    apFormula = new storm::property::prctl::Ap<double>("finished");
	boundedEventuallyFormula = new storm::property::prctl::BoundedEventually<double>(apFormula, 50ull);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(boundedEventuallyFormula, false);
    
	result = mc.checkNoBoundOperator(*probFormula);
    
	ASSERT_LT(std::abs(result[31168] - 0.0), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    delete probFormula;

    apFormula = new storm::property::prctl::Ap<double>("finished");
	storm::property::prctl::ReachabilityReward<double>* reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	storm::property::prctl::RewardNoBoundOperator<double>* rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, true);
    
	result = mc.checkNoBoundOperator(*rewardFormula);
    
	ASSERT_LT(std::abs(result[31168] - 1725.593313), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
	delete rewardFormula;
    
	apFormula = new storm::property::prctl::Ap<double>("finished");
	reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, false);
    
	result = mc.checkNoBoundOperator(*rewardFormula);
    
	ASSERT_LT(std::abs(result[31168] - 2183.142422), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
	delete rewardFormula;
}