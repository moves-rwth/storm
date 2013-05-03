#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/utility/Settings.h"
#include "src/modelchecker/GmmxxDtmcPrctlModelChecker.h"
#include "src/parser/AutoParser.h"

TEST(GmmxxDtmcPrctModelCheckerTest, Die) {
	storm::settings::Settings* s = storm::settings::instance();
	s->set("fix-deadlocks");
	storm::parser::AutoParser<double> parser(STORM_CPP_TESTS_BASE_PATH "/functional/die/die.tra", STORM_CPP_TESTS_BASE_PATH "/functional/die/die.lab", "", STORM_CPP_TESTS_BASE_PATH "/functional/die/die.coin_flips.trans.rew");

	ASSERT_EQ(parser.getType(), storm::models::DTMC);

	std::shared_ptr<storm::models::Dtmc<double>> dtmc = parser.getModel<storm::models::Dtmc<double>>();

	ASSERT_EQ(dtmc->getNumberOfStates(), 13);
	ASSERT_EQ(dtmc->getNumberOfTransitions(), 27);

	storm::modelchecker::GmmxxDtmcPrctlModelChecker<double> mc(*dtmc);

	storm::property::prctl::Ap<double>* apFormula = new storm::property::prctl::Ap<double>("one");
	storm::property::prctl::Eventually<double>* eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	storm::property::prctl::ProbabilisticNoBoundOperator<double>* probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	std::vector<double>* result = probFormula->check(mc);

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - ((double)1/6)), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::property::prctl::Ap<double>("two");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	result = probFormula->check(mc);

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - ((double)1/6)), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::property::prctl::Ap<double>("three");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	result = probFormula->check(mc);

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - ((double)1/6)), s->get<double>("precision"));

	delete probFormula;
	delete result;

	storm::property::prctl::Ap<double>* done = new storm::property::prctl::Ap<double>("done");
	storm::property::prctl::ReachabilityReward<double>* reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(done);
	storm::property::prctl::RewardNoBoundOperator<double>* rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula);

	result = rewardFormula->check(mc);

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - ((double)11/3)), s->get<double>("precision"));

	delete rewardFormula;
	delete result;
}

TEST(GmmxxDtmcPrctModelCheckerTest, Crowds) {
	storm::settings::Settings* s = storm::settings::instance();
	s->set("fix-deadlocks");
	storm::parser::AutoParser<double> parser(STORM_CPP_TESTS_BASE_PATH "/functional/crowds/crowds5_5.tra", STORM_CPP_TESTS_BASE_PATH "/functional/crowds/crowds5_5.lab", "", "");

	ASSERT_EQ(parser.getType(), storm::models::DTMC);

	std::shared_ptr<storm::models::Dtmc<double>> dtmc = parser.getModel<storm::models::Dtmc<double>>();

	ASSERT_EQ(dtmc->getNumberOfStates(), 8607);
	ASSERT_EQ(dtmc->getNumberOfTransitions(), 22460);

	storm::modelchecker::GmmxxDtmcPrctlModelChecker<double> mc(*dtmc);

	storm::property::prctl::Ap<double>* apFormula = new storm::property::prctl::Ap<double>("observe0Greater1");
	storm::property::prctl::Eventually<double>* eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	storm::property::prctl::ProbabilisticNoBoundOperator<double>* probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	std::vector<double>* result = probFormula->check(mc);

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - 0.3328800375801578281), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::property::prctl::Ap<double>("observeIGreater1");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	result = probFormula->check(mc);

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - 0.1522173670950556501), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::property::prctl::Ap<double>("observeOnlyTrueSender");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	result = probFormula->check(mc);

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - 0.32153724292835045), s->get<double>("precision"));

	delete probFormula;
	delete result;
}

TEST(GmmxxDtmcPrctModelCheckerTest, SynchronousLeader) {
	storm::settings::Settings* s = storm::settings::instance();
	s->set("fix-deadlocks");
	storm::parser::AutoParser<double> parser(STORM_CPP_TESTS_BASE_PATH "/functional/synchronous_leader/leader4_8.tra", STORM_CPP_TESTS_BASE_PATH "/functional/synchronous_leader/leader4_8.lab", "", STORM_CPP_TESTS_BASE_PATH "/functional/synchronous_leader/leader4_8.pick.trans.rew");

	ASSERT_EQ(parser.getType(), storm::models::DTMC);

	std::shared_ptr<storm::models::Dtmc<double>> dtmc = parser.getModel<storm::models::Dtmc<double>>();

	ASSERT_EQ(dtmc->getNumberOfStates(), 12400);
	ASSERT_EQ(dtmc->getNumberOfTransitions(), 28894);

	storm::modelchecker::GmmxxDtmcPrctlModelChecker<double> mc(*dtmc);

	storm::property::prctl::Ap<double>* apFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::Eventually<double>* eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	storm::property::prctl::ProbabilisticNoBoundOperator<double>* probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	std::vector<double>* result = probFormula->check(mc);

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - 1), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::BoundedUntil<double>* boundedUntilFormula = new storm::property::prctl::BoundedUntil<double>(new storm::property::prctl::Ap<double>("true"), apFormula, 20);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(boundedUntilFormula);

	result = probFormula->check(mc);

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - 0.9999965911265462636), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::ReachabilityReward<double>* reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	storm::property::prctl::RewardNoBoundOperator<double>* rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula);

	result = rewardFormula->check(mc);

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - 1.0448979591835938496), s->get<double>("precision"));

	delete rewardFormula;
	delete result;
}
