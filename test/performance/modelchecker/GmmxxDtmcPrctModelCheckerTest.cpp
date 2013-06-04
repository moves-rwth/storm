#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/utility/Settings.h"
#include "src/modelchecker/prctl/GmmxxDtmcPrctlModelChecker.h"
#include "src/parser/AutoParser.h"

TEST(GmmxxDtmcPrctlModelCheckerTest, Crowds) {
	storm::settings::Settings* s = storm::settings::instance();
	s->set("fix-deadlocks");
	storm::parser::AutoParser<double> parser(STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds20_5.tra", STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds20_5.lab", "", "");

	ASSERT_EQ(parser.getType(), storm::models::DTMC);

	std::shared_ptr<storm::models::Dtmc<double>> dtmc = parser.getModel<storm::models::Dtmc<double>>();

	ASSERT_EQ(dtmc->getNumberOfStates(), 2036647u);
	ASSERT_EQ(dtmc->getNumberOfTransitions(), 8973900u);

	storm::modelchecker::prctl::GmmxxDtmcPrctlModelChecker<double> mc(*dtmc);

	storm::property::prctl::Ap<double>* apFormula = new storm::property::prctl::Ap<double>("observe0Greater1");
	storm::property::prctl::Eventually<double>* eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	storm::property::prctl::ProbabilisticNoBoundOperator<double>* probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

    LOG4CPLUS_WARN(logger, "Model Checking P=? [F observe0Greater1] on crowds/crowds20_5...");
	std::vector<double>* result = probFormula->check(mc);
    LOG4CPLUS_WARN(logger, "Done.");

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - 0.2296803699), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::property::prctl::Ap<double>("observeIGreater1");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

    LOG4CPLUS_WARN(logger, "Model Checking P=? [F observeIGreater1] on crowds/crowds20_5...");
	result = probFormula->check(mc);
    LOG4CPLUS_WARN(logger, "Done.");
    
	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - 0.05072232915), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::property::prctl::Ap<double>("observeOnlyTrueSender");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

    LOG4CPLUS_WARN(logger, "Model Checking P=? [F observeOnlyTrueSender] on crowds/crowds20_5...");
	result = probFormula->check(mc);
    LOG4CPLUS_WARN(logger, "Done.");

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - 0.2274230551), s->get<double>("precision"));

	delete probFormula;
	delete result;
}


TEST(GmmxxDtmcPrctlModelCheckerTest, SynchronousLeader) {
	storm::settings::Settings* s = storm::settings::instance();
	s->set("fix-deadlocks");
	storm::parser::AutoParser<double> parser(STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader6_8.tra", STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader6_8.lab", "", STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader6_8.pick.trans.rew");

	ASSERT_EQ(parser.getType(), storm::models::DTMC);

	std::shared_ptr<storm::models::Dtmc<double>> dtmc = parser.getModel<storm::models::Dtmc<double>>();

	ASSERT_EQ(dtmc->getNumberOfStates(), 1312334u);
	ASSERT_EQ(dtmc->getNumberOfTransitions(), 2886810u);

	storm::modelchecker::prctl::GmmxxDtmcPrctlModelChecker<double> mc(*dtmc);

	storm::property::prctl::Ap<double>* apFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::Eventually<double>* eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	storm::property::prctl::ProbabilisticNoBoundOperator<double>* probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

    LOG4CPLUS_WARN(logger, "Model Checking P=? [F elected] on synchronous_leader/leader6_8...");
	std::vector<double>* result = probFormula->check(mc);
    LOG4CPLUS_WARN(logger, "Done.");

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - 1), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::BoundedUntil<double>* boundedUntilFormula = new storm::property::prctl::BoundedUntil<double>(new storm::property::prctl::Ap<double>("true"), apFormula, 20);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(boundedUntilFormula);

    LOG4CPLUS_WARN(logger, "Model Checking P=? [F<=20 elected] on synchronous_leader/leader6_8...");
	result = probFormula->check(mc);
    LOG4CPLUS_WARN(logger, "Done.");

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - 0.999394979327824395376467), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::ReachabilityReward<double>* reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	storm::property::prctl::RewardNoBoundOperator<double>* rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula);

    LOG4CPLUS_WARN(logger, "Model Checking R=? [F elected] on synchronous_leader/leader6_8...");
	result = rewardFormula->check(mc);
    LOG4CPLUS_WARN(logger, "Done.");

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - 1.02521744572240791626427), s->get<double>("precision"));

	delete rewardFormula;
	delete result;
}