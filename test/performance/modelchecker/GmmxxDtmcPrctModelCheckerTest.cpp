#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/utility/Settings.h"
#include "src/modelchecker/GmmxxDtmcPrctlModelChecker.h"
#include "src/parser/AutoParser.h"

TEST(GmmxxDtmcPrctModelCheckerTest, Crowds) {
	storm::settings::Settings* s = storm::settings::instance();
	s->set("fix-deadlocks");
	storm::parser::AutoParser<double> parser(STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds20_5.tra", STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds20_5.lab", "", "");

	ASSERT_EQ(parser.getType(), storm::models::DTMC);

	std::shared_ptr<storm::models::Dtmc<double>> dtmc = parser.getModel<storm::models::Dtmc<double>>();

	ASSERT_EQ(dtmc->getNumberOfStates(), 2036647u);
	ASSERT_EQ(dtmc->getNumberOfTransitions(), 8973900u);

	storm::modelchecker::GmmxxDtmcPrctlModelChecker<double> mc(*dtmc);

	storm::property::prctl::Ap<double>* apFormula = new storm::property::prctl::Ap<double>("observe0Greater1");
	storm::property::prctl::Eventually<double>* eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	storm::property::prctl::ProbabilisticNoBoundOperator<double>* probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	std::vector<double>* result = probFormula->check(mc);

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - 0.2296803699), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::property::prctl::Ap<double>("observeIGreater1");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	result = probFormula->check(mc);

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - 0.05072232915), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::property::prctl::Ap<double>("observeOnlyTrueSender");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	result = probFormula->check(mc);

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - 0.2274230551), s->get<double>("precision"));

	delete probFormula;
	delete result;
}


TEST(GmmxxDtmcPrctModelCheckerTest, SynchronousLeader) {
	storm::settings::Settings* s = storm::settings::instance();
	s->set("fix-deadlocks");
	storm::parser::AutoParser<double> parser(STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader6_8.tra", STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader6_8.lab", "", STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader6_8.pick.trans.rew");

	ASSERT_EQ(parser.getType(), storm::models::DTMC);

	std::shared_ptr<storm::models::Dtmc<double>> dtmc = parser.getModel<storm::models::Dtmc<double>>();

	ASSERT_EQ(dtmc->getNumberOfStates(), 1312334u);
	ASSERT_EQ(dtmc->getNumberOfTransitions(), 2886810u);

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

	ASSERT_LT(std::abs((*result)[0] - 0.9999996339), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::ReachabilityReward<double>* reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	storm::property::prctl::RewardNoBoundOperator<double>* rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula);

	result = rewardFormula->check(mc);

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - 1.025217446), s->get<double>("precision"));

	delete rewardFormula;
	delete result;
}
