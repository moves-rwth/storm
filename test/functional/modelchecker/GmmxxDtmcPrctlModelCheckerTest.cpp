#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/solver/GmmxxLinearEquationSolver.h"
#include "src/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "src/settings/SettingsManager.h"
#include "src/settings/SettingMemento.h"
#include "src/parser/AutoParser.h"

TEST(GmmxxDtmcPrctlModelCheckerTest, Die) {
	std::shared_ptr<storm::models::AbstractModel<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/dtmc/die/die.tra", STORM_CPP_BASE_PATH "/examples/dtmc/die/die.lab", "", STORM_CPP_BASE_PATH "/examples/dtmc/die/die.coin_flips.trans.rew");

	ASSERT_EQ(abstractModel->getType(), storm::models::DTMC);

	std::shared_ptr<storm::models::Dtmc<double>> dtmc = abstractModel->as<storm::models::Dtmc<double>>();

	ASSERT_EQ(dtmc->getNumberOfStates(), 13ull);
	ASSERT_EQ(dtmc->getNumberOfTransitions(), 20ull);

	storm::modelchecker::prctl::SparseDtmcPrctlModelChecker<double> mc(*dtmc, std::unique_ptr<storm::solver::LinearEquationSolver<double>>(new storm::solver::GmmxxLinearEquationSolver<double>()));

	auto apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("one");
	auto eventuallyFormula = std::make_shared<storm::properties::prctl::Eventually<double>>(apFormula);

	std::vector<double> result = eventuallyFormula->check(mc, false);

	ASSERT_LT(std::abs(result[0] - ((double)1.0/6.0)), storm::settings::gmmxxEquationSolverSettings().getPrecision());

	apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("two");
	eventuallyFormula = std::make_shared<storm::properties::prctl::Eventually<double>>(apFormula);

	result = eventuallyFormula->check(mc, false);

	ASSERT_LT(std::abs(result[0] - ((double)1.0/6.0)), storm::settings::gmmxxEquationSolverSettings().getPrecision());

	apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("three");
	eventuallyFormula = std::make_shared<storm::properties::prctl::Eventually<double>>(apFormula);

	result = eventuallyFormula->check(mc, false);

	ASSERT_LT(std::abs(result[0] - ((double)1.0/6.0)), storm::settings::gmmxxEquationSolverSettings().getPrecision());

	auto done = std::make_shared<storm::properties::prctl::Ap<double>>("done");
	auto reachabilityRewardFormula = std::make_shared<storm::properties::prctl::ReachabilityReward<double>>(done);

	result = reachabilityRewardFormula->check(mc, false);

	ASSERT_LT(std::abs(result[0] - ((double)11/3)), storm::settings::gmmxxEquationSolverSettings().getPrecision());
}

TEST(GmmxxDtmcPrctlModelCheckerTest, Crowds) {
	std::shared_ptr<storm::models::AbstractModel<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds5_5.tra", STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds5_5.lab", "", "");

	ASSERT_EQ(abstractModel->getType(), storm::models::DTMC);

	std::shared_ptr<storm::models::Dtmc<double>> dtmc = abstractModel->as<storm::models::Dtmc<double>>();

	ASSERT_EQ(8607ull, dtmc->getNumberOfStates());
	ASSERT_EQ(15113ull, dtmc->getNumberOfTransitions());

	storm::modelchecker::prctl::SparseDtmcPrctlModelChecker<double> mc(*dtmc, std::unique_ptr<storm::solver::LinearEquationSolver<double>>(new storm::solver::GmmxxLinearEquationSolver<double>()));

	auto apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("observe0Greater1");
	auto eventuallyFormula = std::make_shared<storm::properties::prctl::Eventually<double>>(apFormula);

	std::vector<double> result = eventuallyFormula->check(mc, false);

	ASSERT_LT(std::abs(result[0] - 0.3328800375801578281), storm::settings::gmmxxEquationSolverSettings().getPrecision());

	apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("observeIGreater1");
	eventuallyFormula = std::make_shared<storm::properties::prctl::Eventually<double>>(apFormula);

	result = eventuallyFormula->check(mc, false);

	ASSERT_LT(std::abs(result[0] - 0.1522194965), storm::settings::gmmxxEquationSolverSettings().getPrecision());

	apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("observeOnlyTrueSender");
	eventuallyFormula = std::make_shared<storm::properties::prctl::Eventually<double>>(apFormula);

	result = eventuallyFormula->check(mc, false);

	ASSERT_LT(std::abs(result[0] - 0.32153724292835045), storm::settings::gmmxxEquationSolverSettings().getPrecision());
}

TEST(GmmxxDtmcPrctlModelCheckerTest, SynchronousLeader) {
	std::shared_ptr<storm::models::AbstractModel<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader4_8.tra", STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader4_8.lab", "", STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader4_8.pick.trans.rew");

	ASSERT_EQ(abstractModel->getType(), storm::models::DTMC);
	std::shared_ptr<storm::models::Dtmc<double>> dtmc = abstractModel->as<storm::models::Dtmc<double>>();

	ASSERT_EQ(12400ull, dtmc->getNumberOfStates());
	ASSERT_EQ(16495ull, dtmc->getNumberOfTransitions());

	storm::modelchecker::prctl::SparseDtmcPrctlModelChecker<double> mc(*dtmc, std::unique_ptr<storm::solver::LinearEquationSolver<double>>(new storm::solver::GmmxxLinearEquationSolver<double>()));

	auto apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("elected");
	auto eventuallyFormula = std::make_shared<storm::properties::prctl::Eventually<double>>(apFormula);

	std::vector<double> result = eventuallyFormula->check(mc, false);

	ASSERT_LT(std::abs(result[0] - 1.0), storm::settings::gmmxxEquationSolverSettings().getPrecision());

	apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("elected");
	auto boundedUntilFormula = std::make_shared<storm::properties::prctl::BoundedUntil<double>>(std::make_shared<storm::properties::prctl::Ap<double>>("true"), apFormula, 20);

	result = boundedUntilFormula->check(mc, false);

	ASSERT_LT(std::abs(result[0] - 0.9999965911265462636), storm::settings::gmmxxEquationSolverSettings().getPrecision());

	apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("elected");
	auto reachabilityRewardFormula = std::make_shared<storm::properties::prctl::ReachabilityReward<double>>(apFormula);

	result = reachabilityRewardFormula->check(mc, false);

	ASSERT_LT(std::abs(result[0] - 1.044879046), storm::settings::gmmxxEquationSolverSettings().getPrecision());
}
