#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/settings/SettingsManager.h"
#include "src/settings/modules/GmmxxEquationSolverSettings.h"
#include "src/settings/modules/NativeEquationSolverSettings.h"
#include "src/settings/SettingMemento.h"
#include "src/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "src/utility/solver.h"
#include "src/parser/AutoParser.h"
#include "src/parser/FormulaParser.h"
#include "src/models/sparse/StandardRewardModel.h"

TEST(GmmxxDtmcPrctlModelCheckerTest, Crowds) {
	std::shared_ptr<storm::models::sparse::Model<double>> abstractModel = storm::parser::AutoParser<>::parseModel(STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds20_5.tra", STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds20_5.lab", "", "");

    ASSERT_EQ(abstractModel->getType(), storm::models::ModelType::Dtmc);

	std::shared_ptr<storm::models::sparse::Dtmc<double>> dtmc = abstractModel->as<storm::models::sparse::Dtmc<double>>();

	ASSERT_EQ(2036647ull, dtmc->getNumberOfStates());
	ASSERT_EQ(7362293ull, dtmc->getNumberOfTransitions());

    storm::modelchecker::SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<double>> checker(*dtmc, std::unique_ptr<storm::solver::LinearEquationSolverFactory<double>>(new storm::utility::solver::GmmxxLinearEquationSolverFactory<double>()));

    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser formulaParser;
    
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("P=? [F \"observe0Greater1\"]");

    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult1 = result->asExplicitQuantitativeCheckResult<double>();

    EXPECT_NEAR(0.2296800237, quantitativeResult1[0], storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("P=? [F \"observeIGreater1\"]");

    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult2 = result->asExplicitQuantitativeCheckResult<double>();
    
	EXPECT_NEAR(0.05073232193, quantitativeResult2[0], storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("P=? [F \"observeOnlyTrueSender\"]");

    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult3 = result->asExplicitQuantitativeCheckResult<double>();
    
	EXPECT_NEAR(0.22742171078, quantitativeResult3[0], storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
}


TEST(GmmxxDtmcPrctlModelCheckerTest, SynchronousLeader) {
	std::shared_ptr<storm::models::sparse::Model<double>> abstractModel = storm::parser::AutoParser<>::parseModel(STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader6_8.tra", STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader6_8.lab", "", STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader6_8.pick.trans.rew");

    ASSERT_EQ(abstractModel->getType(), storm::models::ModelType::Dtmc);

	std::shared_ptr<storm::models::sparse::Dtmc<double>> dtmc = abstractModel->as<storm::models::sparse::Dtmc<double>>();

	ASSERT_EQ(1312334ull, dtmc->getNumberOfStates());
	ASSERT_EQ(1574477ull, dtmc->getNumberOfTransitions());

	storm::modelchecker::SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<double>> checker(*dtmc, std::unique_ptr<storm::solver::LinearEquationSolverFactory<double>>(new storm::utility::solver::GmmxxLinearEquationSolverFactory<double>()));

    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser formulaParser;
    
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("P=? [F \"elected\"]");

    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult1 = result->asExplicitQuantitativeCheckResult<double>();
    
	EXPECT_NEAR(1.0, quantitativeResult1[0], storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("P=? [F<=20 \"elected\"]");

    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult2 = result->asExplicitQuantitativeCheckResult<double>();
    
	EXPECT_NEAR(0.9993949793, quantitativeResult2[0], storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("R=? [F \"elected\"]");

    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult3 = result->asExplicitQuantitativeCheckResult<double>();
    
	EXPECT_NEAR(1.025106273, quantitativeResult3[0], storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
}
