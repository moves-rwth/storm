#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/utility/Settings.h"
#include "src/modelchecker/GmmxxMdpPrctlModelChecker.h"
#include "src/parser/AutoParser.h"

TEST(GmmxxMdpPrctModelCheckerTest, Dice) {
	storm::settings::Settings* s = storm::settings::instance();
	storm::parser::AutoParser<double> parser(STORM_CPP_TESTS_BASE_PATH "/functional/two_dice/two_dice.tra", STORM_CPP_TESTS_BASE_PATH "/functional/two_dice/two_dice.lab", "", STORM_CPP_TESTS_BASE_PATH "/functional/two_dice/two_dice.flip.trans.rew");

	ASSERT_EQ(parser.getType(), storm::models::MDP);

	std::shared_ptr<storm::models::Mdp<double>> mdp = parser.getModel<storm::models::Mdp<double>>();

	ASSERT_EQ(mdp->getNumberOfStates(), 169);
	ASSERT_EQ(mdp->getNumberOfTransitions(), 436);

	storm::modelchecker::GmmxxMdpPrctlModelChecker<double> mc(*mdp);

	storm::formula::prctl::Ap<double>* apFormula = new storm::formula::prctl::Ap<double>("two");
	storm::formula::prctl::Eventually<double>* eventuallyFormula = new storm::formula::prctl::Eventually<double>(apFormula);
	storm::formula::prctl::ProbabilisticNoBoundOperator<double>* probFormula = new storm::formula::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);

	std::vector<double>* result = mc.checkNoBoundOperator(*probFormula);

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - 0.0277777612209320068), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::formula::prctl::Ap<double>("two");
	eventuallyFormula = new storm::formula::prctl::Eventually<double>(apFormula);
	probFormula = new storm::formula::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);

	result = mc.checkNoBoundOperator(*probFormula);

	ASSERT_LT(std::abs((*result)[0] - 0.0277777612209320068), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::formula::prctl::Ap<double>("three");
	eventuallyFormula = new storm::formula::prctl::Eventually<double>(apFormula);
	probFormula = new storm::formula::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);

	result = mc.checkNoBoundOperator(*probFormula);

	ASSERT_LT(std::abs((*result)[0] - 0.0555555224418640136), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::formula::prctl::Ap<double>("three");
	eventuallyFormula = new storm::formula::prctl::Eventually<double>(apFormula);
	probFormula = new storm::formula::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);

	result = mc.checkNoBoundOperator(*probFormula);

	ASSERT_LT(std::abs((*result)[0] - 0.0555555224418640136), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::formula::prctl::Ap<double>("four");
	eventuallyFormula = new storm::formula::prctl::Eventually<double>(apFormula);
	probFormula = new storm::formula::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);

	result = mc.checkNoBoundOperator(*probFormula);

	ASSERT_LT(std::abs((*result)[0] - 0.083333283662796020508), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::formula::prctl::Ap<double>("four");
	eventuallyFormula = new storm::formula::prctl::Eventually<double>(apFormula);
	probFormula = new storm::formula::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);

	result = mc.checkNoBoundOperator(*probFormula);

	ASSERT_LT(std::abs((*result)[0] - 0.083333283662796020508), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::formula::prctl::Ap<double>("done");
	storm::formula::prctl::ReachabilityReward<double>* reachabilityRewardFormula = new storm::formula::prctl::ReachabilityReward<double>(apFormula);
	storm::formula::prctl::RewardNoBoundOperator<double>* rewardFormula = new storm::formula::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, true);

	result = mc.checkNoBoundOperator(*rewardFormula);

	ASSERT_LT(std::abs((*result)[0] - 7.3333294987678527832), s->get<double>("precision"));

	delete rewardFormula;
	delete result;

	apFormula = new storm::formula::prctl::Ap<double>("done");
	reachabilityRewardFormula = new storm::formula::prctl::ReachabilityReward<double>(apFormula);
	rewardFormula = new storm::formula::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, false);

	result = mc.checkNoBoundOperator(*rewardFormula);;

	ASSERT_LT(std::abs((*result)[0] - 7.3333294987678527832), s->get<double>("precision"));

	delete rewardFormula;
	delete result;

	storm::parser::AutoParser<double> stateRewardParser(STORM_CPP_TESTS_BASE_PATH "/functional/two_dice/two_dice.tra", STORM_CPP_TESTS_BASE_PATH "/functional/two_dice/two_dice.lab", STORM_CPP_TESTS_BASE_PATH "/functional/two_dice/two_dice.flip.state.rew", "");

	ASSERT_EQ(stateRewardParser.getType(), storm::models::MDP);

	std::shared_ptr<storm::models::Mdp<double>> stateRewardMdp = stateRewardParser.getModel<storm::models::Mdp<double>>();

	storm::modelchecker::GmmxxMdpPrctlModelChecker<double> stateRewardModelChecker(*stateRewardMdp);

	apFormula = new storm::formula::prctl::Ap<double>("done");
	reachabilityRewardFormula = new storm::formula::prctl::ReachabilityReward<double>(apFormula);
	rewardFormula = new storm::formula::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, true);

	result = stateRewardModelChecker.checkNoBoundOperator(*rewardFormula);

	ASSERT_LT(std::abs((*result)[0] - 7.3333294987678527832), s->get<double>("precision"));

	delete rewardFormula;
	delete result;

	apFormula = new storm::formula::prctl::Ap<double>("done");
	reachabilityRewardFormula = new storm::formula::prctl::ReachabilityReward<double>(apFormula);
	rewardFormula = new storm::formula::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, false);

	result = stateRewardModelChecker.checkNoBoundOperator(*rewardFormula);

	ASSERT_LT(std::abs((*result)[0] - 7.3333294987678527832), s->get<double>("precision"));

	delete rewardFormula;
	delete result;

	storm::parser::AutoParser<double> stateAndTransitionRewardParser(STORM_CPP_TESTS_BASE_PATH "/functional/two_dice/two_dice.tra", STORM_CPP_TESTS_BASE_PATH "/functional/two_dice/two_dice.lab", STORM_CPP_TESTS_BASE_PATH "/functional/two_dice/two_dice.flip.state.rew", STORM_CPP_TESTS_BASE_PATH "/functional/two_dice/two_dice.flip.trans.rew");

	ASSERT_EQ(stateAndTransitionRewardParser.getType(), storm::models::MDP);

	std::shared_ptr<storm::models::Mdp<double>> stateAndTransitionRewardMdp = stateAndTransitionRewardParser.getModel<storm::models::Mdp<double>>();

	storm::modelchecker::GmmxxMdpPrctlModelChecker<double> stateAndTransitionRewardModelChecker(*stateAndTransitionRewardMdp);

	apFormula = new storm::formula::prctl::Ap<double>("done");
	reachabilityRewardFormula = new storm::formula::prctl::ReachabilityReward<double>(apFormula);
	rewardFormula = new storm::formula::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, true);

	result = stateAndTransitionRewardModelChecker.checkNoBoundOperator(*rewardFormula);

	ASSERT_LT(std::abs((*result)[0] - (2 * 7.3333294987678527832)), s->get<double>("precision"));

	delete rewardFormula;
	delete result;

	apFormula = new storm::formula::prctl::Ap<double>("done");
	reachabilityRewardFormula = new storm::formula::prctl::ReachabilityReward<double>(apFormula);
	rewardFormula = new storm::formula::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, false);

	result = stateAndTransitionRewardModelChecker.checkNoBoundOperator(*rewardFormula);

	ASSERT_LT(std::abs((*result)[0] - (2 * 7.3333294987678527832)), s->get<double>("precision"));

	delete rewardFormula;
	delete result;
}

TEST(GmmxxMdpPrctModelCheckerTest, AsynchronousLeader) {
	storm::settings::Settings* s = storm::settings::instance();
	storm::parser::AutoParser<double> parser(STORM_CPP_TESTS_BASE_PATH "/functional/asynchronous_leader/leader4.tra", STORM_CPP_TESTS_BASE_PATH "/functional/asynchronous_leader/leader4.lab", "", STORM_CPP_TESTS_BASE_PATH "/functional/asynchronous_leader/leader4.trans.rew");

	ASSERT_EQ(parser.getType(), storm::models::MDP);

	std::shared_ptr<storm::models::Mdp<double>> mdp = parser.getModel<storm::models::Mdp<double>>();

	ASSERT_EQ(mdp->getNumberOfStates(), 3172);
	ASSERT_EQ(mdp->getNumberOfTransitions(), 7144);

	storm::modelchecker::GmmxxMdpPrctlModelChecker<double> mc(*mdp);

	storm::formula::prctl::Ap<double>* apFormula = new storm::formula::prctl::Ap<double>("elected");
	storm::formula::prctl::Eventually<double>* eventuallyFormula = new storm::formula::prctl::Eventually<double>(apFormula);
	storm::formula::prctl::ProbabilisticNoBoundOperator<double>* probFormula = new storm::formula::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);

	std::vector<double>* result = mc.checkNoBoundOperator(*probFormula);

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - 1), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::formula::prctl::Ap<double>("elected");
	eventuallyFormula = new storm::formula::prctl::Eventually<double>(apFormula);
	probFormula = new storm::formula::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);

	result = mc.checkNoBoundOperator(*probFormula);

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - 1), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::formula::prctl::Ap<double>("elected");
	storm::formula::prctl::BoundedEventually<double>* boundedEventuallyFormula = new storm::formula::prctl::BoundedEventually<double>(apFormula, 25);
	probFormula = new storm::formula::prctl::ProbabilisticNoBoundOperator<double>(boundedEventuallyFormula, true);

	result = mc.checkNoBoundOperator(*probFormula);

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - 0.0625), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::formula::prctl::Ap<double>("elected");
	boundedEventuallyFormula = new storm::formula::prctl::BoundedEventually<double>(apFormula, 25);
	probFormula = new storm::formula::prctl::ProbabilisticNoBoundOperator<double>(boundedEventuallyFormula, false);

	result = mc.checkNoBoundOperator(*probFormula);

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - 0.0625), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::formula::prctl::Ap<double>("elected");
	storm::formula::prctl::ReachabilityReward<double>* reachabilityRewardFormula = new storm::formula::prctl::ReachabilityReward<double>(apFormula);
	storm::formula::prctl::RewardNoBoundOperator<double>* rewardFormula = new storm::formula::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, true);

	result = mc.checkNoBoundOperator(*rewardFormula);;

	ASSERT_LT(std::abs((*result)[0] - 4.28568908480604982), s->get<double>("precision"));

	delete rewardFormula;
	delete result;

	apFormula = new storm::formula::prctl::Ap<double>("elected");
	reachabilityRewardFormula = new storm::formula::prctl::ReachabilityReward<double>(apFormula);
	rewardFormula = new storm::formula::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, false);

	result = mc.checkNoBoundOperator(*rewardFormula);;

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - 4.2856904354441400784), s->get<double>("precision"));

	delete rewardFormula;
	delete result;
}
