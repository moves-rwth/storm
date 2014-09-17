/*
#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/settings/SettingsManager.h"
#include "src/modelchecker/EigenDtmcPrctlModelChecker.h"
#include "src/parser/AutoParser.h"

TEST(EigenDtmcPrctlModelCheckerTest, Die) {
	storm::settings::Settings* s = storm::settings::Settings::getInstance();
	storm::settings::InternalOptionMemento deadlockOption("fixDeadlocks", true);
	ASSERT_TRUE(s->isSet("fixDeadlocks"));
	storm::parser::AutoParser<double> parser(STORM_CPP_TESTS_BASE_PATH "/functional/modelchecker/die/die.tra", STORM_CPP_TESTS_BASE_PATH "/functional/modelchecker/die/die.lab", "", STORM_CPP_TESTS_BASE_PATH "/functional/modelchecker/die/die.coin_flips.trans.rew");

	ASSERT_EQ(parser.getType(), storm::models::DTMC);

	std::shared_ptr<storm::models::Dtmc<double>> dtmc = parser.getModel<storm::models::Dtmc<double>>();

	ASSERT_EQ(dtmc->getNumberOfStates(), 14);
	ASSERT_EQ(dtmc->getNumberOfTransitions(), 28);

	storm::modelChecker::EigenDtmcPrctlModelChecker<double> mc(*dtmc);

	storm::properties::Ap<double>* apFormula = new storm::properties::Ap<double>("one");
	storm::properties::Eventually<double>* eventuallyFormula = new storm::properties::Eventually<double>(apFormula);
	storm::properties::ProbabilisticNoBoundOperator<double>* probFormula = new storm::properties::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	std::vector<double>* result = probFormula->check(mc);

	ASSERT_LT(std::abs((*result)[1] - ((double)1/6)), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete probFormula;
	delete result;

	apFormula = new storm::properties::Ap<double>("two");
	eventuallyFormula = new storm::properties::Eventually<double>(apFormula);
	probFormula = new storm::properties::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	result = probFormula->check(mc);

	ASSERT_LT(std::abs((*result)[1] - ((double)1/6)), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete probFormula;
	delete result;

	apFormula = new storm::properties::Ap<double>("three");
	eventuallyFormula = new storm::properties::Eventually<double>(apFormula);
	probFormula = new storm::properties::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	result = probFormula->check(mc);

	ASSERT_LT(std::abs((*result)[1] - ((double)1/6)), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete probFormula;
	delete result;

	storm::properties::Ap<double>* done = new storm::properties::Ap<double>("done");
	storm::properties::ReachabilityReward<double>* reachabilityRewardFormula = new storm::properties::ReachabilityReward<double>(done);
	storm::properties::RewardNoBoundOperator<double>* rewardFormula = new storm::properties::RewardNoBoundOperator<double>(reachabilityRewardFormula);

	result = rewardFormula->check(mc);


	ASSERT_LT(std::abs((*result)[1] - ((double)11/3)), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete rewardFormula;
	delete result;
}

TEST(EigenDtmcPrctlModelCheckerTest, Crowds) {
	storm::settings::Settings* s = storm::settings::Settings::getInstance();
	storm::settings::InternalOptionMemento deadlockOption("fixDeadlocks", true);
	ASSERT_TRUE(s->isSet("fixDeadlocks"));
	storm::parser::AutoParser<double> parser(STORM_CPP_TESTS_BASE_PATH "/functional/modelchecker/crowds/crowds5_5.tra", STORM_CPP_TESTS_BASE_PATH "/functional/modelchecker/crowds/crowds5_5.lab", "", "");

	ASSERT_EQ(parser.getType(), storm::models::DTMC);

	std::shared_ptr<storm::models::Dtmc<double>> dtmc = parser.getModel<storm::models::Dtmc<double>>();

	ASSERT_EQ(dtmc->getNumberOfStates(), 8608);
	ASSERT_EQ(dtmc->getNumberOfTransitions(), 22461);

	storm::modelChecker::EigenDtmcPrctlModelChecker<double> mc(*dtmc);

	storm::properties::Ap<double>* apFormula = new storm::properties::Ap<double>("observe0Greater1");
	storm::properties::Eventually<double>* eventuallyFormula = new storm::properties::Eventually<double>(apFormula);
	storm::properties::ProbabilisticNoBoundOperator<double>* probFormula = new storm::properties::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	std::vector<double>* result = probFormula->check(mc);

	ASSERT_LT(std::abs((*result)[1] - 0.3328800375801578281), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete probFormula;
	delete result;

	apFormula = new storm::properties::Ap<double>("observeIGreater1");
	eventuallyFormula = new storm::properties::Eventually<double>(apFormula);
	probFormula = new storm::properties::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	result = probFormula->check(mc);

	ASSERT_LT(std::abs((*result)[1] - 0.1522173670950556501), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete probFormula;
	delete result;

	apFormula = new storm::properties::Ap<double>("observeOnlyTrueSender");
	eventuallyFormula = new storm::properties::Eventually<double>(apFormula);
	probFormula = new storm::properties::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	result = probFormula->check(mc);

	ASSERT_LT(std::abs((*result)[1] - 0.32153724292835045), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete probFormula;
	delete result;
}

TEST(EigenDtmcPrctlModelCheckerTest, SynchronousLeader) {
	storm::settings::Settings* s = storm::settings::Settings::getInstance();
	storm::settings::InternalOptionMemento deadlockOption("fixDeadlocks", true);
	ASSERT_TRUE(s->isSet("fixDeadlocks"));
	storm::parser::AutoParser<double> parser(STORM_CPP_TESTS_BASE_PATH "/functional/modelchecker/synchronous_leader/leader4_8.tra", STORM_CPP_TESTS_BASE_PATH "/functional/modelchecker/synchronous_leader/leader4_8.lab", "", STORM_CPP_TESTS_BASE_PATH "/functional/modelchecker/synchronous_leader/leader4_8.pick.trans.rew");

	ASSERT_EQ(parser.getType(), storm::models::DTMC);

	std::shared_ptr<storm::models::Dtmc<double>> dtmc = parser.getModel<storm::models::Dtmc<double>>();

	ASSERT_EQ(dtmc->getNumberOfStates(), 12401);
	ASSERT_EQ(dtmc->getNumberOfTransitions(), 28895);

	storm::modelChecker::EigenDtmcPrctlModelChecker<double> mc(*dtmc);

	storm::properties::Ap<double>* apFormula = new storm::properties::Ap<double>("elected");
	storm::properties::Eventually<double>* eventuallyFormula = new storm::properties::Eventually<double>(apFormula);
	storm::properties::ProbabilisticNoBoundOperator<double>* probFormula = new storm::properties::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	std::vector<double>* result = probFormula->check(mc);

	ASSERT_LT(std::abs((*result)[1] - 1), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete probFormula;
	delete result;

	apFormula = new storm::properties::Ap<double>("elected");
	storm::properties::BoundedUntil<double>* boundedUntilFormula = new storm::properties::BoundedUntil<double>(new storm::properties::Ap<double>("true"), apFormula, 20);
	probFormula = new storm::properties::ProbabilisticNoBoundOperator<double>(boundedUntilFormula);

	result = probFormula->check(mc);

	ASSERT_LT(std::abs((*result)[1] - 0.9999965911265462636), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete probFormula;
	delete result;

	apFormula = new storm::properties::Ap<double>("elected");
	storm::properties::ReachabilityReward<double>* reachabilityRewardFormula = new storm::properties::ReachabilityReward<double>(apFormula);
	storm::properties::RewardNoBoundOperator<double>* rewardFormula = new storm::properties::RewardNoBoundOperator<double>(reachabilityRewardFormula);

	result = rewardFormula->check(mc);

	ASSERT_LT(std::abs((*result)[1] - 1.0448979591835938496), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete rewardFormula;
	delete result;
}
*/
