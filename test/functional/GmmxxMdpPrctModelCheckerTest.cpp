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

	storm::modelChecker::GmmxxMdpPrctlModelChecker<double> mc(*mdp);

	storm::formula::Ap<double>* apFormula = new storm::formula::Ap<double>("two");
	storm::formula::Eventually<double>* eventuallyFormula = new storm::formula::Eventually<double>(apFormula);
	storm::formula::ProbabilisticNoBoundOperator<double>* probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);

	std::vector<double>* result = probFormula->check(mc);

	ASSERT_LT(std::abs((*result)[0] - 0.0277777612209320068), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::formula::Ap<double>("two");
	eventuallyFormula = new storm::formula::Eventually<double>(apFormula);
	probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);

	result = probFormula->check(mc);

	ASSERT_LT(std::abs((*result)[0] - 0.0277777612209320068), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::formula::Ap<double>("three");
	eventuallyFormula = new storm::formula::Eventually<double>(apFormula);
	probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);

	result = probFormula->check(mc);

	ASSERT_LT(std::abs((*result)[0] - 0.0555555224418640136), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::formula::Ap<double>("three");
	eventuallyFormula = new storm::formula::Eventually<double>(apFormula);
	probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);

	result = probFormula->check(mc);

	ASSERT_LT(std::abs((*result)[0] - 0.0555555224418640136), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::formula::Ap<double>("four");
	eventuallyFormula = new storm::formula::Eventually<double>(apFormula);
	probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);

	result = probFormula->check(mc);

	ASSERT_LT(std::abs((*result)[0] - 0.083333283662796020508), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::formula::Ap<double>("four");
	eventuallyFormula = new storm::formula::Eventually<double>(apFormula);
	probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);

	result = probFormula->check(mc);

	ASSERT_LT(std::abs((*result)[0] - 0.083333283662796020508), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::formula::Ap<double>("done");
	storm::formula::ReachabilityReward<double>* reachabilityRewardFormula = new storm::formula::ReachabilityReward<double>(apFormula);
	storm::formula::RewardNoBoundOperator<double>* rewardFormula = new storm::formula::RewardNoBoundOperator<double>(reachabilityRewardFormula, true);

	result = rewardFormula->check(mc);

	ASSERT_LT(std::abs((*result)[0] - 7.3333272933959960938), s->get<double>("precision"));

	delete rewardFormula;
	delete result;

	apFormula = new storm::formula::Ap<double>("done");
	reachabilityRewardFormula = new storm::formula::ReachabilityReward<double>(apFormula);
	rewardFormula = new storm::formula::RewardNoBoundOperator<double>(reachabilityRewardFormula, false);

	result = rewardFormula->check(mc);

	ASSERT_LT(std::abs((*result)[0] - 7.3333272933959960938), s->get<double>("precision"));

	delete rewardFormula;
	delete result;

	storm::parser::AutoParser<double> stateRewardParser(STORM_CPP_TESTS_BASE_PATH "/functional/two_dice/two_dice.tra", STORM_CPP_TESTS_BASE_PATH "/functional/two_dice/two_dice.lab", STORM_CPP_TESTS_BASE_PATH "/functional/two_dice/two_dice.flip.state.rew", "");

	ASSERT_EQ(stateRewardParser.getType(), storm::models::MDP);

	std::shared_ptr<storm::models::Mdp<double>> stateRewardMdp = stateRewardParser.getModel<storm::models::Mdp<double>>();

	storm::modelChecker::GmmxxMdpPrctlModelChecker<double> stateRewardModelChecker(*stateRewardMdp);

	apFormula = new storm::formula::Ap<double>("done");
	reachabilityRewardFormula = new storm::formula::ReachabilityReward<double>(apFormula);
	rewardFormula = new storm::formula::RewardNoBoundOperator<double>(reachabilityRewardFormula, true);

	result = rewardFormula->check(stateRewardModelChecker);

	ASSERT_LT(std::abs((*result)[0] - 7.3333272933959960938), s->get<double>("precision"));

	delete rewardFormula;
	delete result;

	apFormula = new storm::formula::Ap<double>("done");
	reachabilityRewardFormula = new storm::formula::ReachabilityReward<double>(apFormula);
	rewardFormula = new storm::formula::RewardNoBoundOperator<double>(reachabilityRewardFormula, false);

	result = rewardFormula->check(stateRewardModelChecker);

	ASSERT_LT(std::abs((*result)[0] - 7.3333272933959960938), s->get<double>("precision"));

	delete rewardFormula;
	delete result;

	storm::parser::AutoParser<double> stateAndTransitionRewardParser(STORM_CPP_TESTS_BASE_PATH "/functional/two_dice/two_dice.tra", STORM_CPP_TESTS_BASE_PATH "/functional/two_dice/two_dice.lab", STORM_CPP_TESTS_BASE_PATH "/functional/two_dice/two_dice.flip.state.rew", STORM_CPP_TESTS_BASE_PATH "/functional/two_dice/two_dice.flip.trans.rew");

	ASSERT_EQ(stateAndTransitionRewardParser.getType(), storm::models::MDP);

	std::shared_ptr<storm::models::Mdp<double>> stateAndTransitionRewardMdp = stateAndTransitionRewardParser.getModel<storm::models::Mdp<double>>();

	storm::modelChecker::GmmxxMdpPrctlModelChecker<double> stateAndTransitionRewardModelChecker(*stateAndTransitionRewardMdp);

	apFormula = new storm::formula::Ap<double>("done");
	reachabilityRewardFormula = new storm::formula::ReachabilityReward<double>(apFormula);
	rewardFormula = new storm::formula::RewardNoBoundOperator<double>(reachabilityRewardFormula, true);

	result = rewardFormula->check(stateAndTransitionRewardModelChecker);

	ASSERT_LT(std::abs((*result)[0] - (2 * 7.3333272933959960938)), s->get<double>("precision"));

	delete rewardFormula;
	delete result;

	apFormula = new storm::formula::Ap<double>("done");
	reachabilityRewardFormula = new storm::formula::ReachabilityReward<double>(apFormula);
	rewardFormula = new storm::formula::RewardNoBoundOperator<double>(reachabilityRewardFormula, false);

	result = rewardFormula->check(stateAndTransitionRewardModelChecker);

	ASSERT_LT(std::abs((*result)[0] - (2 * 7.3333272933959960938)), s->get<double>("precision"));

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

	storm::modelChecker::GmmxxMdpPrctlModelChecker<double> mc(*mdp);

	storm::formula::Ap<double>* apFormula = new storm::formula::Ap<double>("elected");
	storm::formula::Eventually<double>* eventuallyFormula = new storm::formula::Eventually<double>(apFormula);
	storm::formula::ProbabilisticNoBoundOperator<double>* probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);

	std::vector<double>* result = probFormula->check(mc);

	ASSERT_LT(std::abs((*result)[0] - 1), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::formula::Ap<double>("elected");
	eventuallyFormula = new storm::formula::Eventually<double>(apFormula);
	probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);

	result = probFormula->check(mc);

	ASSERT_LT(std::abs((*result)[0] - 1), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::formula::Ap<double>("elected");
	storm::formula::BoundedEventually<double>* boundedEventuallyFormula = new storm::formula::BoundedEventually<double>(apFormula, 25);
	probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(boundedEventuallyFormula, true);

	result = probFormula->check(mc);

	ASSERT_LT(std::abs((*result)[0] - 0.0625), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::formula::Ap<double>("elected");
	boundedEventuallyFormula = new storm::formula::BoundedEventually<double>(apFormula, 25);
	probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(boundedEventuallyFormula, false);

	result = probFormula->check(mc);

	ASSERT_LT(std::abs((*result)[0] - 0.0625), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::formula::Ap<double>("elected");
	storm::formula::ReachabilityReward<double>* reachabilityRewardFormula = new storm::formula::ReachabilityReward<double>(apFormula);
	storm::formula::RewardNoBoundOperator<double>* rewardFormula = new storm::formula::RewardNoBoundOperator<double>(reachabilityRewardFormula, true);

	result = rewardFormula->check(mc);

	ASSERT_LT(std::abs((*result)[0] - 4.28568908480604982), s->get<double>("precision"));

	delete rewardFormula;
	delete result;

	apFormula = new storm::formula::Ap<double>("elected");
	reachabilityRewardFormula = new storm::formula::ReachabilityReward<double>(apFormula);
	rewardFormula = new storm::formula::RewardNoBoundOperator<double>(reachabilityRewardFormula, false);

	result = rewardFormula->check(mc);

	ASSERT_LT(std::abs((*result)[0] - 4.2856904354441400784), s->get<double>("precision"));

	delete rewardFormula;
	delete result;
}
