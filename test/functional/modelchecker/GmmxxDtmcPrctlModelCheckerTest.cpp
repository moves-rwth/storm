#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/solver/GmmxxLinearEquationSolver.h"
#include "src/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "src/settings/Settings.h"
#include "src/settings/InternalOptionMemento.h"
#include "src/parser/AutoParser.h"

TEST(GmmxxDtmcPrctlModelCheckerTest, Die) {
	storm::settings::Settings* s = storm::settings::Settings::getInstance();
	storm::settings::InternalOptionMemento deadlockOption("fixDeadlocks", true);
	ASSERT_TRUE(s->isSet("fixDeadlocks"));
	std::shared_ptr<storm::models::AbstractModel<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/dtmc/die/die.tra", STORM_CPP_BASE_PATH "/examples/dtmc/die/die.lab", "", STORM_CPP_BASE_PATH "/examples/dtmc/die/die.coin_flips.trans.rew");

	ASSERT_EQ(abstractModel->getType(), storm::models::DTMC);

	std::shared_ptr<storm::models::Dtmc<double>> dtmc = abstractModel->as<storm::models::Dtmc<double>>();

	ASSERT_EQ(dtmc->getNumberOfStates(), 13ull);
	ASSERT_EQ(dtmc->getNumberOfTransitions(), 27ull);

	storm::modelchecker::prctl::SparseDtmcPrctlModelChecker<double> mc(*dtmc, new storm::solver::GmmxxLinearEquationSolver<double>());

	storm::property::prctl::Ap<double>* apFormula = new storm::property::prctl::Ap<double>("one");
	storm::property::prctl::Eventually<double>* eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	storm::property::prctl::ProbabilisticNoBoundOperator<double>* probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	std::vector<double> result = probFormula->check(mc);

	ASSERT_LT(std::abs(result[0] - ((double)1.0/6.0)), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete probFormula;

	apFormula = new storm::property::prctl::Ap<double>("two");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	result = probFormula->check(mc);

	ASSERT_LT(std::abs(result[0] - ((double)1.0/6.0)), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete probFormula;

	apFormula = new storm::property::prctl::Ap<double>("three");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	result = probFormula->check(mc);

	ASSERT_LT(std::abs(result[0] - ((double)1.0/6.0)), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete probFormula;

	storm::property::prctl::Ap<double>* done = new storm::property::prctl::Ap<double>("done");
	storm::property::prctl::ReachabilityReward<double>* reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(done);
	storm::property::prctl::RewardNoBoundOperator<double>* rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula);

	result = rewardFormula->check(mc);

	ASSERT_LT(std::abs(result[0] - ((double)11/3)), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete rewardFormula;
}

TEST(GmmxxDtmcPrctlModelCheckerTest, Crowds) {
	storm::settings::Settings* s = storm::settings::Settings::getInstance();
	storm::settings::InternalOptionMemento deadlockOption("fixDeadlocks", true);
	ASSERT_TRUE(s->isSet("fixDeadlocks"));
	std::shared_ptr<storm::models::AbstractModel<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds5_5.tra", STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds5_5.lab", "", "");

	ASSERT_EQ(abstractModel->getType(), storm::models::DTMC);

	std::shared_ptr<storm::models::Dtmc<double>> dtmc = abstractModel->as<storm::models::Dtmc<double>>();

	ASSERT_EQ(dtmc->getNumberOfStates(), 8607ull);
	ASSERT_EQ(dtmc->getNumberOfTransitions(), 22460ull);

	storm::modelchecker::prctl::SparseDtmcPrctlModelChecker<double> mc(*dtmc, new storm::solver::GmmxxLinearEquationSolver<double>());

	storm::property::prctl::Ap<double>* apFormula = new storm::property::prctl::Ap<double>("observe0Greater1");
	storm::property::prctl::Eventually<double>* eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	storm::property::prctl::ProbabilisticNoBoundOperator<double>* probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	std::vector<double> result = probFormula->check(mc);

	ASSERT_LT(std::abs(result[0] - 0.3328800375801578281), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete probFormula;

	apFormula = new storm::property::prctl::Ap<double>("observeIGreater1");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	result = probFormula->check(mc);

	ASSERT_LT(std::abs(result[0] - 0.1522194965), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete probFormula;

	apFormula = new storm::property::prctl::Ap<double>("observeOnlyTrueSender");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	result = probFormula->check(mc);

	ASSERT_LT(std::abs(result[0] - 0.32153724292835045), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete probFormula;
}

TEST(GmmxxDtmcPrctlModelCheckerTest, SynchronousLeader) {
	storm::settings::Settings* s = storm::settings::Settings::getInstance();
	storm::settings::InternalOptionMemento deadlockOption("fixDeadlocks", true);
	ASSERT_TRUE(s->isSet("fixDeadlocks"));
	std::shared_ptr<storm::models::AbstractModel<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader4_8.tra", STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader4_8.lab", "", STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader4_8.pick.trans.rew");

	ASSERT_EQ(abstractModel->getType(), storm::models::DTMC);
	std::shared_ptr<storm::models::Dtmc<double>> dtmc = abstractModel->as<storm::models::Dtmc<double>>();

	ASSERT_EQ(dtmc->getNumberOfStates(), 12400ull);
	ASSERT_EQ(dtmc->getNumberOfTransitions(), 28894ull);

	storm::modelchecker::prctl::SparseDtmcPrctlModelChecker<double> mc(*dtmc, new storm::solver::GmmxxLinearEquationSolver<double>());

	storm::property::prctl::Ap<double>* apFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::Eventually<double>* eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	storm::property::prctl::ProbabilisticNoBoundOperator<double>* probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula);

	std::vector<double> result = probFormula->check(mc);

	ASSERT_LT(std::abs(result[0] - 1.0), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete probFormula;

	apFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::BoundedUntil<double>* boundedUntilFormula = new storm::property::prctl::BoundedUntil<double>(new storm::property::prctl::Ap<double>("true"), apFormula, 20);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(boundedUntilFormula);

	result = probFormula->check(mc);

	ASSERT_LT(std::abs(result[0] - 0.9999965911265462636), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete probFormula;

	apFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::ReachabilityReward<double>* reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	storm::property::prctl::RewardNoBoundOperator<double>* rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula);

	result = rewardFormula->check(mc);

	ASSERT_LT(std::abs(result[0] - 1.044879046), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete rewardFormula;
}
