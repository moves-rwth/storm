#include "gtest/gtest.h"
#include "storm-config.h"

#include "storm/parser/FormulaParser.h"
#include "storm/logic/Formulas.h"
#include "storm/utility/solver.h"
#include "storm/storage/SymbolicModelDescription.h"
#include "storm/modelchecker/prctl/SymbolicDtmcPrctlModelChecker.h"
#include "storm/modelchecker/results/SymbolicQualitativeCheckResult.h"
#include "storm/modelchecker/results/SymbolicQuantitativeCheckResult.h"
#include "storm/parser/PrismParser.h"
#include "storm/builder/DdPrismModelBuilder.h"
#include "storm/models/symbolic/StandardRewardModel.h"
#include "storm/models/symbolic/Dtmc.h"
#include "storm/settings/SettingsManager.h"

#include "storm/settings/modules/NativeEquationSolverSettings.h"

#include "storm/settings/modules/GeneralSettings.h"

TEST(SymbolicDtmcPrctlModelCheckerTest, Die_Cudd) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/builder/die.pm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    
    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser formulaParser;

    // Build the die model with its reward model.
#ifdef WINDOWS
    storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
#else
    typename storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
#endif
    options.buildAllRewardModels = false;
    options.rewardModelsToBuild.insert("coin_flips");
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program, options);
    EXPECT_EQ(13ul, model->getNumberOfStates());
    EXPECT_EQ(20ul, model->getNumberOfTransitions());
    
    ASSERT_EQ(model->getType(), storm::models::ModelType::Dtmc);
    
    std::shared_ptr<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD>> dtmc = model->as<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD>>();
    
    storm::modelchecker::SymbolicDtmcPrctlModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD, double>> checker(*dtmc, std::unique_ptr<storm::utility::solver::SymbolicLinearEquationSolverFactory<storm::dd::DdType::CUDD, double>>(new storm::utility::solver::SymbolicLinearEquationSolverFactory<storm::dd::DdType::CUDD, double>()));
    
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("P=? [F \"one\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult1 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(1.0/6.0, quantitativeResult1.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(1.0/6.0, quantitativeResult1.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [F \"two\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult2 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(1.0/6.0, quantitativeResult2.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(1.0/6.0, quantitativeResult2.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [F \"three\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult3 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(1.0/6.0, quantitativeResult3.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(1.0/6.0, quantitativeResult3.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("R=? [F \"done\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult4 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(3.6666622161865234, quantitativeResult4.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(3.6666622161865234, quantitativeResult4.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
}

TEST(SymbolicDtmcPrctlModelCheckerTest, Die_Sylvan) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/builder/die.pm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    
    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser formulaParser;
    
    // Build the die model with its reward model.
#ifdef WINDOWS
    storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>::Options options;
#else
    typename storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>::Options options;
#endif
    options.buildAllRewardModels = false;
    options.rewardModelsToBuild.insert("coin_flips");
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program, options);
    EXPECT_EQ(13ul, model->getNumberOfStates());
    EXPECT_EQ(20ul, model->getNumberOfTransitions());
    
    ASSERT_EQ(model->getType(), storm::models::ModelType::Dtmc);
    
    std::shared_ptr<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan>> dtmc = model->as<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan>>();
    
    storm::modelchecker::SymbolicDtmcPrctlModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan, double>> checker(*dtmc, std::unique_ptr<storm::utility::solver::SymbolicLinearEquationSolverFactory<storm::dd::DdType::Sylvan, double>>(new storm::utility::solver::SymbolicLinearEquationSolverFactory<storm::dd::DdType::Sylvan, double>()));
    
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("P=? [F \"one\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult1 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(1.0/6.0, quantitativeResult1.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(1.0/6.0, quantitativeResult1.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [F \"two\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult2 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(1.0/6.0, quantitativeResult2.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(1.0/6.0, quantitativeResult2.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [F \"three\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult3 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(1.0/6.0, quantitativeResult3.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(1.0/6.0, quantitativeResult3.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("R=? [F \"done\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult4 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    // FIXME: precision is not optimal.
    EXPECT_NEAR(3.6666622161865234, quantitativeResult4.getMin(), 10 * storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(3.6666622161865234, quantitativeResult4.getMax(), 10 * storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
}

TEST(SymbolicDtmcPrctlModelCheckerTest, Crowds_Cudd) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/builder/crowds-5-5.pm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    
    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser formulaParser;
    
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program);
    EXPECT_EQ(8607ul, model->getNumberOfStates());
    EXPECT_EQ(15113ul, model->getNumberOfTransitions());
    
    ASSERT_EQ(model->getType(), storm::models::ModelType::Dtmc);
    
    std::shared_ptr<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD>> dtmc = model->as<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD>>();
    
    storm::modelchecker::SymbolicDtmcPrctlModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD, double>> checker(*dtmc, std::unique_ptr<storm::utility::solver::SymbolicLinearEquationSolverFactory<storm::dd::DdType::CUDD, double>>(new storm::utility::solver::SymbolicLinearEquationSolverFactory<storm::dd::DdType::CUDD, double>()));
    
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("P=? [F \"observe0Greater1\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult1 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(0.33288236360191303, quantitativeResult1.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.33288236360191303, quantitativeResult1.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [F \"observeIGreater1\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult2 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(0.15222081144084315, quantitativeResult2.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.15222081144084315, quantitativeResult2.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [F \"observeOnlyTrueSender\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult3 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(0.3215392962289586, quantitativeResult3.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.3215392962289586, quantitativeResult3.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
}

TEST(SymbolicDtmcPrctlModelCheckerTest, Crowds_Sylvan) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/builder/crowds-5-5.pm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    
    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser formulaParser;
    
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program);
    EXPECT_EQ(8607ul, model->getNumberOfStates());
    EXPECT_EQ(15113ul, model->getNumberOfTransitions());
    
    ASSERT_EQ(model->getType(), storm::models::ModelType::Dtmc);
    
    std::shared_ptr<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan>> dtmc = model->as<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan>>();
    
    storm::modelchecker::SymbolicDtmcPrctlModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan, double>> checker(*dtmc, std::unique_ptr<storm::utility::solver::SymbolicLinearEquationSolverFactory<storm::dd::DdType::Sylvan, double>>(new storm::utility::solver::SymbolicLinearEquationSolverFactory<storm::dd::DdType::Sylvan, double>()));
    
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("P=? [F \"observe0Greater1\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult1 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    // FIXME: precision not optimal.
    EXPECT_NEAR(0.33288236360191303, quantitativeResult1.getMin(), 10 * storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.33288236360191303, quantitativeResult1.getMax(), 10 * storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [F \"observeIGreater1\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult2 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    // FIXME: precision not optimal.
    EXPECT_NEAR(0.15222081144084315, quantitativeResult2.getMin(), 10 * storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.15222081144084315, quantitativeResult2.getMax(), 10 * storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [F \"observeOnlyTrueSender\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult3 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    // FIXME: precision not optimal.
    EXPECT_NEAR(0.3215392962289586, quantitativeResult3.getMin(), 10 * storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.3215392962289586, quantitativeResult3.getMax(), 10 * storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
}

TEST(SymbolicDtmcPrctlModelCheckerTest, SynchronousLeader_Cudd) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/builder/leader-3-5.pm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    
    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser formulaParser;
    
    // Build the die model with its reward model.
#ifdef WINDOWS
    storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
#else
    typename storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
#endif
    options.buildAllRewardModels = false;
    options.rewardModelsToBuild.insert("num_rounds");
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program, options);
    EXPECT_EQ(273ul, model->getNumberOfStates());
    EXPECT_EQ(397ul, model->getNumberOfTransitions());
    
    ASSERT_EQ(model->getType(), storm::models::ModelType::Dtmc);
    
    std::shared_ptr<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD>> dtmc = model->as<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD>>();
    
    storm::modelchecker::SymbolicDtmcPrctlModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD, double>> checker(*dtmc, std::unique_ptr<storm::utility::solver::SymbolicLinearEquationSolverFactory<storm::dd::DdType::CUDD, double>>(new storm::utility::solver::SymbolicLinearEquationSolverFactory<storm::dd::DdType::CUDD, double>()));
    
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("P=? [F \"elected\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult1 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(1.0, quantitativeResult1.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(1.0, quantitativeResult1.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [F<=20 \"elected\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult2 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(0.99999989760000074, quantitativeResult2.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.99999989760000074, quantitativeResult2.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("R=? [F \"elected\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult3 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(1.0416666666666643, quantitativeResult3.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(1.0416666666666643, quantitativeResult3.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
}

TEST(SymbolicDtmcPrctlModelCheckerTest, SynchronousLeader_Sylvan) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/builder/leader-3-5.pm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    
    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser formulaParser;
    
    // Build the die model with its reward model.
#ifdef WINDOWS
    storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>::Options options;
#else
    typename storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>::Options options;
#endif
    options.buildAllRewardModels = false;
    options.rewardModelsToBuild.insert("num_rounds");
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program, options);
    EXPECT_EQ(273ul, model->getNumberOfStates());
    EXPECT_EQ(397ul, model->getNumberOfTransitions());
    
    ASSERT_EQ(model->getType(), storm::models::ModelType::Dtmc);
    
    std::shared_ptr<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan>> dtmc = model->as<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan>>();
    
    storm::modelchecker::SymbolicDtmcPrctlModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan, double>> checker(*dtmc, std::unique_ptr<storm::utility::solver::SymbolicLinearEquationSolverFactory<storm::dd::DdType::Sylvan, double>>(new storm::utility::solver::SymbolicLinearEquationSolverFactory<storm::dd::DdType::Sylvan, double>()));
    
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("P=? [F \"elected\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult1 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(1.0, quantitativeResult1.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(1.0, quantitativeResult1.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [F<=20 \"elected\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult2 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(0.99999989760000074, quantitativeResult2.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.99999989760000074, quantitativeResult2.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("R=? [F \"elected\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult3 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(1.0416666666666643, quantitativeResult3.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(1.0416666666666643, quantitativeResult3.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
}
