/*
#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/utility/Settings.h"
#include "src/modelchecker/EigenDtmcPrctlModelChecker.h"
#include "src/parser/AutoParser.h"

TEST(EigenDtmcPrctModelCheckerTest, Die) {
	storm::settings::Settings* s = storm::settings::instance();
	s->set("fix-deadlocks");
	storm::parser::AutoParser<double> parser(STORM_CPP_TESTS_BASE_PATH "/functional/die/die.tra", STORM_CPP_TESTS_BASE_PATH "/functional/die/die.lab", "", STORM_CPP_TESTS_BASE_PATH "/functional/die/die.coin_flips.trans.rew");

	ASSERT_EQ(parser.getType(), storm::models::DTMC);

	std::shared_ptr<storm::models::Dtmc<double>> dtmc = parser.getModel<storm::models::Dtmc<double>>();

	ASSERT_EQ(dtmc->getNumberOfStates(), 14);
	ASSERT_EQ(dtmc->getNumberOfTransitions(), 28);

	storm::modelChecker::EigenDtmcPrctlModelChecker<double> mc(*dtmc);

	storm::formula::Ap<double>* apFormula = new storm::formula::Ap<double>("one");
	storm::formula::Eventually<double>* eventuallyFormula = new storm::formula::Eventually<double>(apFormula);
	storm::formula::ProbabilisticNoBoundOperator<double>* probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	std::vector<double>* result = probFormula->check(mc);

	ASSERT_LT(std::abs((*result)[1] - ((double)1/6)), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::formula::Ap<double>("two");
	eventuallyFormula = new storm::formula::Eventually<double>(apFormula);
	probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	result = probFormula->check(mc);

	ASSERT_LT(std::abs((*result)[1] - ((double)1/6)), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::formula::Ap<double>("three");
	eventuallyFormula = new storm::formula::Eventually<double>(apFormula);
	probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	result = probFormula->check(mc);

	ASSERT_LT(std::abs((*result)[1] - ((double)1/6)), s->get<double>("precision"));

	delete probFormula;
	delete result;

	storm::formula::Ap<double>* done = new storm::formula::Ap<double>("done");
	storm::formula::ReachabilityReward<double>* reachabilityRewardFormula = new storm::formula::ReachabilityReward<double>(done);
	storm::formula::RewardNoBoundOperator<double>* rewardFormula = new storm::formula::RewardNoBoundOperator<double>(reachabilityRewardFormula);

	result = rewardFormula->check(mc);


	ASSERT_LT(std::abs((*result)[1] - ((double)11/3)), s->get<double>("precision"));

	delete rewardFormula;
	delete result;
}

TEST(EigenDtmcPrctModelCheckerTest, Crowds) {
	storm::settings::Settings* s = storm::settings::instance();
	s->set("fix-deadlocks");
	storm::parser::AutoParser<double> parser(STORM_CPP_TESTS_BASE_PATH "/functional/crowds/crowds5_5.tra", STORM_CPP_TESTS_BASE_PATH "/functional/crowds/crowds5_5.lab", "", "");

	ASSERT_EQ(parser.getType(), storm::models::DTMC);

	std::shared_ptr<storm::models::Dtmc<double>> dtmc = parser.getModel<storm::models::Dtmc<double>>();

	ASSERT_EQ(dtmc->getNumberOfStates(), 8608);
	ASSERT_EQ(dtmc->getNumberOfTransitions(), 22461);

	storm::modelChecker::EigenDtmcPrctlModelChecker<double> mc(*dtmc);

	storm::formula::Ap<double>* apFormula = new storm::formula::Ap<double>("observe0Greater1");
	storm::formula::Eventually<double>* eventuallyFormula = new storm::formula::Eventually<double>(apFormula);
	storm::formula::ProbabilisticNoBoundOperator<double>* probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	std::vector<double>* result = probFormula->check(mc);

	ASSERT_LT(std::abs((*result)[1] - 0.3328800375801578281), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::formula::Ap<double>("observeIGreater1");
	eventuallyFormula = new storm::formula::Eventually<double>(apFormula);
	probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	result = probFormula->check(mc);

	ASSERT_LT(std::abs((*result)[1] - 0.1522173670950556501), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::formula::Ap<double>("observeOnlyTrueSender");
	eventuallyFormula = new storm::formula::Eventually<double>(apFormula);
	probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	result = probFormula->check(mc);

	ASSERT_LT(std::abs((*result)[1] - 0.32153724292835045), s->get<double>("precision"));

	delete probFormula;
	delete result;
}

TEST(EigenDtmcPrctModelCheckerTest, SynchronousLeader) {
	storm::settings::Settings* s = storm::settings::instance();
	s->set("fix-deadlocks");
	storm::parser::AutoParser<double> parser(STORM_CPP_TESTS_BASE_PATH "/functional/synchronous_leader/leader4_8.tra", STORM_CPP_TESTS_BASE_PATH "/functional/synchronous_leader/leader4_8.lab", "", STORM_CPP_TESTS_BASE_PATH "/functional/synchronous_leader/leader4_8.pick.trans.rew");

	ASSERT_EQ(parser.getType(), storm::models::DTMC);

	std::shared_ptr<storm::models::Dtmc<double>> dtmc = parser.getModel<storm::models::Dtmc<double>>();

	ASSERT_EQ(dtmc->getNumberOfStates(), 12401);
	ASSERT_EQ(dtmc->getNumberOfTransitions(), 28895);

	storm::modelChecker::EigenDtmcPrctlModelChecker<double> mc(*dtmc);

	storm::formula::Ap<double>* apFormula = new storm::formula::Ap<double>("elected");
	storm::formula::Eventually<double>* eventuallyFormula = new storm::formula::Eventually<double>(apFormula);
	storm::formula::ProbabilisticNoBoundOperator<double>* probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	std::vector<double>* result = probFormula->check(mc);

	ASSERT_LT(std::abs((*result)[1] - 1), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::formula::Ap<double>("elected");
	storm::formula::BoundedUntil<double>* boundedUntilFormula = new storm::formula::BoundedUntil<double>(new storm::formula::Ap<double>("true"), apFormula, 20);
	probFormula = new storm::formula::ProbabilisticNoBoundOperator<double>(boundedUntilFormula);

	result = probFormula->check(mc);

	ASSERT_LT(std::abs((*result)[1] - 0.9999965911265462636), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::formula::Ap<double>("elected");
	storm::formula::ReachabilityReward<double>* reachabilityRewardFormula = new storm::formula::ReachabilityReward<double>(apFormula);
	storm::formula::RewardNoBoundOperator<double>* rewardFormula = new storm::formula::RewardNoBoundOperator<double>(reachabilityRewardFormula);

	result = rewardFormula->check(mc);

	ASSERT_LT(std::abs((*result)[1] - 1.0448979591835938496), s->get<double>("precision"));

	delete rewardFormula;
	delete result;
}
*/
