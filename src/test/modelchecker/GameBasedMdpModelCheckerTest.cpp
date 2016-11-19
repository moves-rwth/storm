#include "gtest/gtest.h"
#include "storm-config.h"

#include "storm/parser/FormulaParser.h"
#include "storm/logic/Formulas.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/models/sparse/Model.h"
#include "storm/modelchecker/abstraction/GameBasedMdpModelChecker.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/modules/NativeEquationSolverSettings.h"

#include "storm/parser/AutoParser.h"
#include "storm/parser/PrismParser.h"

#include "utility/storm.h"

TEST(GameBasedMdpModelCheckerTest, Dice_Cudd) {
    std::string programFile = STORM_CPP_BASE_PATH "/builder/two_dice.nm";
    std::string formulaFile = STORM_CPP_BASE_PATH "/builder/two_dice.prctl";

    storm::prism::Program program = storm::parseProgram(programFile);
	
	// Build the die model
#ifdef WINDOWS
    storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
#else
    typename storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
#endif
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program, options);

    ASSERT_EQ(model->getNumberOfStates(), 169ull);
    ASSERT_EQ(model->getNumberOfTransitions(), 436ull);

	std::shared_ptr<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>> mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>();
	auto mdpModelchecker = std::make_shared<storm::modelchecker::GameBasedMdpModelChecker<storm::dd::DdType::CUDD, storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>>(program);
	
	// A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser formulaParser;
	
	std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F \"two\"]");
	storm::modelchecker::CheckTask<storm::logic::Formula, double> task(*formula, true);
	
    std::unique_ptr<storm::modelchecker::CheckResult> result = mdpModelchecker->check(task);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult1 = result->asExplicitQuantitativeCheckResult<double>();

    EXPECT_NEAR(0.0277777612209320068, quantitativeResult1[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("Pmax=? [F \"two\"]");
	task = storm::modelchecker::CheckTask<storm::logic::Formula, double>(*formula, true);

    result = mdpModelchecker->check(task);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult2 = result->asExplicitQuantitativeCheckResult<double>();

    EXPECT_NEAR(0.0277777612209320068, quantitativeResult2[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F \"three\"]");
	task = storm::modelchecker::CheckTask<storm::logic::Formula, double>(*formula, true);

    result = mdpModelchecker->check(task);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult3 = result->asExplicitQuantitativeCheckResult<double>();

    EXPECT_NEAR(0.0555555224418640136, quantitativeResult3[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("Pmax=? [F \"three\"]");
	task = storm::modelchecker::CheckTask<storm::logic::Formula, double>(*formula, true);

    result = mdpModelchecker->check(task);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult4 = result->asExplicitQuantitativeCheckResult<double>();

    EXPECT_NEAR(0.0555555224418640136, quantitativeResult4[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F \"four\"]");
	task = storm::modelchecker::CheckTask<storm::logic::Formula, double>(*formula, true);

    result = mdpModelchecker->check(task);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult5 = result->asExplicitQuantitativeCheckResult<double>();

    EXPECT_NEAR(0.083333283662796020508, quantitativeResult5[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("Pmax=? [F \"four\"]");
	task = storm::modelchecker::CheckTask<storm::logic::Formula, double>(*formula, true);

    result = mdpModelchecker->check(task);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult6 = result->asExplicitQuantitativeCheckResult<double>();

    EXPECT_NEAR(0.083333283662796020508, quantitativeResult6[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
}

TEST(GameBasedMdpModelCheckerTest, AsynchronousLeader_Cudd) {
    std::string programFile = STORM_CPP_BASE_PATH "/builder/leader4.nm";
    std::string formulaFile = STORM_CPP_BASE_PATH "/builder/leader.prctl";

    storm::prism::Program program = storm::parseProgram(programFile);
	// Build the die model
#ifdef WINDOWS
    storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
#else
    typename storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
#endif
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program, options);

    ASSERT_EQ(3172ull, model->getNumberOfStates());
    ASSERT_EQ(7144ull, model->getNumberOfTransitions());

	std::shared_ptr<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>> mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>();
	auto mdpModelchecker = std::make_shared<storm::modelchecker::GameBasedMdpModelChecker<storm::dd::DdType::CUDD, storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>>(program);

	// A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser formulaParser;
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F \"elected\"]");
	storm::modelchecker::CheckTask<storm::logic::Formula, double> task(*formula, true);

    std::unique_ptr<storm::modelchecker::CheckResult> result = mdpModelchecker->check(task);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult1 = result->asExplicitQuantitativeCheckResult<double>();

    EXPECT_NEAR(1, quantitativeResult1[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("Pmax=? [F \"elected\"]");
	task = storm::modelchecker::CheckTask<storm::logic::Formula, double>(*formula, true);

    result = mdpModelchecker->check(task);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult2 = result->asExplicitQuantitativeCheckResult<double>();

    EXPECT_NEAR(1, quantitativeResult2[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F<=25 \"elected\"]");
	task = storm::modelchecker::CheckTask<storm::logic::Formula, double>(*formula, true);

    result = mdpModelchecker->check(task);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult3 = result->asExplicitQuantitativeCheckResult<double>();

    EXPECT_NEAR(0.0625, quantitativeResult3[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("Pmax=? [F<=25 \"elected\"]");
	task = storm::modelchecker::CheckTask<storm::logic::Formula, double>(*formula, true);

    result = mdpModelchecker->check(task);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult4 = result->asExplicitQuantitativeCheckResult<double>();

    EXPECT_NEAR(0.0625, quantitativeResult4[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
}
