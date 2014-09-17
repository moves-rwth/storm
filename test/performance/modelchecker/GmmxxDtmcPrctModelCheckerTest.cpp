#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/settings/Settings.h"
#include "src/settings/InternalOptionMemento.h"
#include "src/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "src/solver/GmmxxLinearEquationSolver.h"
#include "src/parser/AutoParser.h"

TEST(GmmxxDtmcPrctlModelCheckerTest, Crowds) {
	storm::settings::SettingsManager* s = storm::settings::SettingsManager::getInstance();
	storm::settings::InternalOptionMemento deadlockOption("fixDeadlocks", true);
	ASSERT_TRUE(s->isSet("fixDeadlocks"));
	std::shared_ptr<storm::models::AbstractModel<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds20_5.tra", STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds20_5.lab", "", "");

	ASSERT_EQ(abstractModel->getType(), storm::models::DTMC);

	std::shared_ptr<storm::models::Dtmc<double>> dtmc = abstractModel->as<storm::models::Dtmc<double>>();

	ASSERT_EQ(2036647ull, dtmc->getNumberOfStates());
	ASSERT_EQ(7362293ull, dtmc->getNumberOfTransitions());

	storm::modelchecker::prctl::SparseDtmcPrctlModelChecker<double> mc(*dtmc, new storm::solver::GmmxxLinearEquationSolver<double>());

	auto apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("observe0Greater1");
	auto eventuallyFormula = std::make_shared<storm::properties::prctl::Eventually<double>>(apFormula);

    LOG4CPLUS_WARN(logger, "Model Checking P=? [F observe0Greater1] on crowds/crowds20_5...");
	std::vector<double> result = eventuallyFormula->check(mc, false);
    LOG4CPLUS_WARN(logger, "Done.");

	ASSERT_LT(std::abs(result[0] - 0.2296800237), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("observeIGreater1");
	eventuallyFormula = std::make_shared<storm::properties::prctl::Eventually<double>>(apFormula);

    LOG4CPLUS_WARN(logger, "Model Checking P=? [F observeIGreater1] on crowds/crowds20_5...");
    result = eventuallyFormula->check(mc, false);
    LOG4CPLUS_WARN(logger, "Done.");
    
	ASSERT_LT(std::abs(result[0] - 0.05073232193), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("observeOnlyTrueSender");
	eventuallyFormula = std::make_shared<storm::properties::prctl::Eventually<double>>(apFormula);

    LOG4CPLUS_WARN(logger, "Model Checking P=? [F observeOnlyTrueSender] on crowds/crowds20_5...");
    result = eventuallyFormula->check(mc, false);
    LOG4CPLUS_WARN(logger, "Done.");

	ASSERT_LT(std::abs(result[0] - 0.22742171078), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
}


TEST(GmmxxDtmcPrctlModelCheckerTest, SynchronousLeader) {
	storm::settings::SettingsManager* s = storm::settings::SettingsManager::getInstance();
	storm::settings::InternalOptionMemento deadlockOption("fixDeadlocks", true);
	ASSERT_TRUE(s->isSet("fixDeadlocks"));
	std::shared_ptr<storm::models::AbstractModel<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader6_8.tra", STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader6_8.lab", "", STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader6_8.pick.trans.rew");

	ASSERT_EQ(abstractModel->getType(), storm::models::DTMC);

	std::shared_ptr<storm::models::Dtmc<double>> dtmc = abstractModel->as<storm::models::Dtmc<double>>();

	ASSERT_EQ(1312334ull, dtmc->getNumberOfStates());
	ASSERT_EQ(1574477ull, dtmc->getNumberOfTransitions());

	storm::modelchecker::prctl::SparseDtmcPrctlModelChecker<double> mc(*dtmc, new storm::solver::GmmxxLinearEquationSolver<double>());

	auto apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("elected");
	auto eventuallyFormula = std::make_shared<storm::properties::prctl::Eventually<double>>(apFormula);

    LOG4CPLUS_WARN(logger, "Model Checking P=? [F elected] on synchronous_leader/leader6_8...");
	std::vector<double> result = eventuallyFormula->check(mc, false);
    LOG4CPLUS_WARN(logger, "Done.");

	ASSERT_LT(std::abs(result[0] - 1.0), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("elected");
	auto boundedUntilFormula = std::make_shared<storm::properties::prctl::BoundedUntil<double>>(std::make_shared<storm::properties::prctl::Ap<double>>("true"), apFormula, 20);

    LOG4CPLUS_WARN(logger, "Model Checking P=? [F<=20 elected] on synchronous_leader/leader6_8...");
    result = boundedUntilFormula->check(mc, false);
    LOG4CPLUS_WARN(logger, "Done.");

	ASSERT_LT(std::abs(result[0] - 0.9993949793), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("elected");
	auto reachabilityRewardFormula = std::make_shared<storm::properties::prctl::ReachabilityReward<double>>(apFormula);

    LOG4CPLUS_WARN(logger, "Model Checking R=? [F elected] on synchronous_leader/leader6_8...");
	result = reachabilityRewardFormula->check(mc, false);
    LOG4CPLUS_WARN(logger, "Done.");

	ASSERT_LT(std::abs(result[0] - 1.025106273), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
}
