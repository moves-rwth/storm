#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/utility/Settings.h"
#include "src/modelchecker/prctl/GmmxxMdpPrctlModelChecker.h"
#include "src/parser/AutoParser.h"

TEST(GmmxxMdpPrctlModelCheckerTest, Dice) {
	storm::settings::Settings* s = storm::settings::instance();
	storm::parser::AutoParser<double> parser(STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.tra", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.lab", "", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.trans.rew");
    
	ASSERT_EQ(parser.getType(), storm::models::MDP);

	std::shared_ptr<storm::models::Mdp<double>> mdp = parser.getModel<storm::models::Mdp<double>>();
    
	ASSERT_EQ(mdp->getNumberOfStates(), 169u);
	ASSERT_EQ(mdp->getNumberOfTransitions(), 436u);

	storm::modelchecker::prctl::GmmxxMdpPrctlModelChecker<double> mc(*mdp);

	storm::property::prctl::Ap<double>* apFormula = new storm::property::prctl::Ap<double>("two");
	storm::property::prctl::Eventually<double>* eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	storm::property::prctl::ProbabilisticNoBoundOperator<double>* probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);
    
	std::vector<double>* result = mc.checkNoBoundOperator(*probFormula);
    
	ASSERT_NE(nullptr, result);
    
	ASSERT_LT(std::abs((*result)[0] - 0.0277777612209320068), s->get<double>("precision"));
    
	delete probFormula;
	delete result;
    
	apFormula = new storm::property::prctl::Ap<double>("two");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);
    
	result = mc.checkNoBoundOperator(*probFormula);
    
	ASSERT_LT(std::abs((*result)[0] - 0.0277777612209320068), s->get<double>("precision"));
    
	delete probFormula;
	delete result;
    
	apFormula = new storm::property::prctl::Ap<double>("three");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);
    
	result = mc.checkNoBoundOperator(*probFormula);
    
	ASSERT_LT(std::abs((*result)[0] - 0.0555555224418640136), s->get<double>("precision"));
    
	delete probFormula;
	delete result;
    
	apFormula = new storm::property::prctl::Ap<double>("three");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);
    
	result = mc.checkNoBoundOperator(*probFormula);
    
	ASSERT_LT(std::abs((*result)[0] - 0.0555555224418640136), s->get<double>("precision"));
    
	delete probFormula;
	delete result;
    
	apFormula = new storm::property::prctl::Ap<double>("four");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);
    
	result = mc.checkNoBoundOperator(*probFormula);
    
	ASSERT_LT(std::abs((*result)[0] - 0.083333283662796020508), s->get<double>("precision"));
    
	delete probFormula;
	delete result;
    
	apFormula = new storm::property::prctl::Ap<double>("four");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);
    
	result = mc.checkNoBoundOperator(*probFormula);
    
	ASSERT_LT(std::abs((*result)[0] - 0.083333283662796020508), s->get<double>("precision"));
    
	delete probFormula;
	delete result;
    
	apFormula = new storm::property::prctl::Ap<double>("done");
	storm::property::prctl::ReachabilityReward<double>* reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	storm::property::prctl::RewardNoBoundOperator<double>* rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, true);
    
	result = mc.checkNoBoundOperator(*rewardFormula);
    
	ASSERT_LT(std::abs((*result)[0] - 7.3333294987678527832), s->get<double>("precision"));
    
	delete rewardFormula;
	delete result;
    
	apFormula = new storm::property::prctl::Ap<double>("done");
	reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, false);
    
	result = mc.checkNoBoundOperator(*rewardFormula);;
    
	ASSERT_LT(std::abs((*result)[0] - 7.3333294987678527832), s->get<double>("precision"));
    
	delete rewardFormula;
	delete result;
    
	storm::parser::AutoParser<double> stateRewardParser(STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.tra", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.lab", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.state.rew", "");
    
	ASSERT_EQ(stateRewardParser.getType(), storm::models::MDP);
    
	std::shared_ptr<storm::models::Mdp<double>> stateRewardMdp = stateRewardParser.getModel<storm::models::Mdp<double>>();
    
	storm::modelchecker::prctl::GmmxxMdpPrctlModelChecker<double> stateRewardModelChecker(*stateRewardMdp);
    
	apFormula = new storm::property::prctl::Ap<double>("done");
	reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, true);
    
	result = stateRewardModelChecker.checkNoBoundOperator(*rewardFormula);
    
	ASSERT_LT(std::abs((*result)[0] - 7.3333294987678527832), s->get<double>("precision"));
    
	delete rewardFormula;
	delete result;
    
	apFormula = new storm::property::prctl::Ap<double>("done");
	reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, false);
    
	result = stateRewardModelChecker.checkNoBoundOperator(*rewardFormula);
    
	ASSERT_LT(std::abs((*result)[0] - 7.3333294987678527832), s->get<double>("precision"));
    
	delete rewardFormula;
	delete result;
    
	storm::parser::AutoParser<double> stateAndTransitionRewardParser(STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.tra", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.lab", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.state.rew", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.trans.rew");
    
	ASSERT_EQ(stateAndTransitionRewardParser.getType(), storm::models::MDP);
    
	std::shared_ptr<storm::models::Mdp<double>> stateAndTransitionRewardMdp = stateAndTransitionRewardParser.getModel<storm::models::Mdp<double>>();
    
	storm::modelchecker::prctl::GmmxxMdpPrctlModelChecker<double> stateAndTransitionRewardModelChecker(*stateAndTransitionRewardMdp);
    
	apFormula = new storm::property::prctl::Ap<double>("done");
	reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, true);
    
	result = stateAndTransitionRewardModelChecker.checkNoBoundOperator(*rewardFormula);
    
	ASSERT_LT(std::abs((*result)[0] - (2 * 7.3333294987678527832)), s->get<double>("precision"));
    
	delete rewardFormula;
	delete result;
    
	apFormula = new storm::property::prctl::Ap<double>("done");
	reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, false);
    
	result = stateAndTransitionRewardModelChecker.checkNoBoundOperator(*rewardFormula);
    
	ASSERT_LT(std::abs((*result)[0] - (2 * 7.3333294987678527832)), s->get<double>("precision"));
    
	delete rewardFormula;
	delete result;
}

TEST(GmmxxMdpPrctlModelCheckerTest, AsynchronousLeader) {
	storm::settings::Settings* s = storm::settings::instance();
	storm::parser::AutoParser<double> parser(STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader4.tra", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader4.lab", "", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader4.trans.rew");

	ASSERT_EQ(parser.getType(), storm::models::MDP);

	std::shared_ptr<storm::models::Mdp<double>> mdp = parser.getModel<storm::models::Mdp<double>>();

	ASSERT_EQ(mdp->getNumberOfStates(), 3172u);
	ASSERT_EQ(mdp->getNumberOfTransitions(), 7144u);

	storm::modelchecker::prctl::GmmxxMdpPrctlModelChecker<double> mc(*mdp);

	storm::property::prctl::Ap<double>* apFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::Eventually<double>* eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	storm::property::prctl::ProbabilisticNoBoundOperator<double>* probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);

	std::vector<double>* result = mc.checkNoBoundOperator(*probFormula);

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - 1), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::property::prctl::Ap<double>("elected");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);

	result = mc.checkNoBoundOperator(*probFormula);

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - 1), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::BoundedEventually<double>* boundedEventuallyFormula = new storm::property::prctl::BoundedEventually<double>(apFormula, 25);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(boundedEventuallyFormula, true);

	result = mc.checkNoBoundOperator(*probFormula);

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - 0.0625), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::property::prctl::Ap<double>("elected");
	boundedEventuallyFormula = new storm::property::prctl::BoundedEventually<double>(apFormula, 25);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(boundedEventuallyFormula, false);

	result = mc.checkNoBoundOperator(*probFormula);

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - 0.0625), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::ReachabilityReward<double>* reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	storm::property::prctl::RewardNoBoundOperator<double>* rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, true);

	result = mc.checkNoBoundOperator(*rewardFormula);;

	ASSERT_LT(std::abs((*result)[0] - 4.28568908480604982), s->get<double>("precision"));

	delete rewardFormula;
	delete result;

	apFormula = new storm::property::prctl::Ap<double>("elected");
	reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, false);

	result = mc.checkNoBoundOperator(*rewardFormula);;

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - 4.2856904354441400784), s->get<double>("precision"));

	delete rewardFormula;
	delete result;
}
