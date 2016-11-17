#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/storm/parser/FormulaParser.h"
#include "src/storm/logic/Formulas.h"
#include "src/storm/solver/NativeLinearEquationSolver.h"
#include "src/storm/storage/SymbolicModelDescription.h"
#include "src/storm/modelchecker/prctl/HybridDtmcPrctlModelChecker.h"
#include "src/storm/modelchecker/results/HybridQuantitativeCheckResult.h"
#include "src/storm/modelchecker/results/SymbolicQualitativeCheckResult.h"
#include "src/storm/modelchecker/results/SymbolicQuantitativeCheckResult.h"
#include "src/storm/parser/PrismParser.h"
#include "src/storm/builder/DdPrismModelBuilder.h"
#include "src/storm/models/symbolic/StandardRewardModel.h"
#include "src/storm/models/symbolic/Dtmc.h"
#include "src/storm/settings/SettingsManager.h"
#include "src/storm/settings/modules/GeneralSettings.h"
#include "src/storm/settings/modules/GmmxxEquationSolverSettings.h"
#include "src/storm/settings/modules/NativeEquationSolverSettings.h"

TEST(NativeHybridDtmcPrctlModelCheckerTest, Die_CUDD) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/die.pm");
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
    
    storm::modelchecker::HybridDtmcPrctlModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD, double>> checker(*dtmc, std::make_unique<storm::solver::NativeLinearEquationSolverFactory<double>>());
    
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("P=? [F \"one\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult1 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(1.0/6.0, quantitativeResult1.getMin(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(1.0/6.0, quantitativeResult1.getMax(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [F \"two\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult2 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(1.0/6.0, quantitativeResult2.getMin(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(1.0/6.0, quantitativeResult2.getMax(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [F \"three\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult3 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(1.0/6.0, quantitativeResult3.getMin(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(1.0/6.0, quantitativeResult3.getMax(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("R=? [F \"done\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult4 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(3.6666646003723145, quantitativeResult4.getMin(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(3.6666646003723145, quantitativeResult4.getMax(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
}

TEST(NativeHybridDtmcPrctlModelCheckerTest, Die_Sylvan) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/die.pm");
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
    
    storm::modelchecker::HybridDtmcPrctlModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan, double>> checker(*dtmc, std::make_unique<storm::solver::NativeLinearEquationSolverFactory<double>>());
    
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("P=? [F \"one\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult1 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(1.0/6.0, quantitativeResult1.getMin(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(1.0/6.0, quantitativeResult1.getMax(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [F \"two\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult2 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(1.0/6.0, quantitativeResult2.getMin(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(1.0/6.0, quantitativeResult2.getMax(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [F \"three\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult3 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(1.0/6.0, quantitativeResult3.getMin(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(1.0/6.0, quantitativeResult3.getMax(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("R=? [F \"done\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult4 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(3.6666646003723145, quantitativeResult4.getMin(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(3.6666646003723145, quantitativeResult4.getMax(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
}

TEST(NativeHybridDtmcPrctlModelCheckerTest, Crowds_CUDD) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/crowds-5-5.pm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    
    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser formulaParser;
    
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program);
    EXPECT_EQ(8607ul, model->getNumberOfStates());
    EXPECT_EQ(15113ul, model->getNumberOfTransitions());
    
    ASSERT_EQ(model->getType(), storm::models::ModelType::Dtmc);
    
    std::shared_ptr<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD>> dtmc = model->as<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD>>();
    
    storm::modelchecker::HybridDtmcPrctlModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD, double>> checker(*dtmc, std::make_unique<storm::solver::NativeLinearEquationSolverFactory<double>>());
    
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("P=? [F \"observe0Greater1\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult1 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(0.33288205191646525, quantitativeResult1.getMin(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.33288205191646525, quantitativeResult1.getMax(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [F \"observeIGreater1\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult2 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(0.15222066094730619, quantitativeResult2.getMin(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.15222066094730619, quantitativeResult2.getMax(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [F \"observeOnlyTrueSender\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult3 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(0.32153900158185761, quantitativeResult3.getMin(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.32153900158185761, quantitativeResult3.getMax(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
}

TEST(NativeHybridDtmcPrctlModelCheckerTest, Crowds_Sylvan) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/crowds-5-5.pm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    
    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser formulaParser;
    
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program);
    EXPECT_EQ(8607ul, model->getNumberOfStates());
    EXPECT_EQ(15113ul, model->getNumberOfTransitions());
    
    ASSERT_EQ(model->getType(), storm::models::ModelType::Dtmc);
    
    std::shared_ptr<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan>> dtmc = model->as<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan>>();
    
    storm::modelchecker::HybridDtmcPrctlModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan, double>> checker(*dtmc, std::make_unique<storm::solver::NativeLinearEquationSolverFactory<double>>());
    
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("P=? [F \"observe0Greater1\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult1 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(0.33288205191646525, quantitativeResult1.getMin(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.33288205191646525, quantitativeResult1.getMax(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [F \"observeIGreater1\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult2 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(0.15222066094730619, quantitativeResult2.getMin(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.15222066094730619, quantitativeResult2.getMax(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [F \"observeOnlyTrueSender\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult3 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(0.32153900158185761, quantitativeResult3.getMin(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.32153900158185761, quantitativeResult3.getMax(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
}

TEST(NativeHybridDtmcPrctlModelCheckerTest, SynchronousLeader_CUDD) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/leader-3-5.pm");
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
    
    storm::modelchecker::HybridDtmcPrctlModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD, double>> checker(*dtmc, std::make_unique<storm::solver::NativeLinearEquationSolverFactory<double>>());
    
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("P=? [F \"elected\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult1 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(1.0, quantitativeResult1.getMin(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(1.0, quantitativeResult1.getMax(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [F<=20 \"elected\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult2 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(0.99999989760000074, quantitativeResult2.getMin(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.99999989760000074, quantitativeResult2.getMax(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("R=? [F \"elected\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult3 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(1.0416666666666643, quantitativeResult3.getMin(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(1.0416666666666643, quantitativeResult3.getMax(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
}

TEST(NativeHybridDtmcPrctlModelCheckerTest, SynchronousLeader_Sylvan) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/leader-3-5.pm");
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
    
    storm::modelchecker::HybridDtmcPrctlModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan, double>> checker(*dtmc, std::make_unique<storm::solver::NativeLinearEquationSolverFactory<double>>());
    
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("P=? [F \"elected\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult1 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(1.0, quantitativeResult1.getMin(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(1.0, quantitativeResult1.getMax(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [F<=20 \"elected\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult2 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(0.99999989760000074, quantitativeResult2.getMin(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.99999989760000074, quantitativeResult2.getMax(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("R=? [F \"elected\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult3 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(1.0416666666666643, quantitativeResult3.getMin(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(1.0416666666666643, quantitativeResult3.getMax(), storm::settings::getModule<storm::settings::modules::GmmxxEquationSolverSettings>().getPrecision());
}
