#include "gtest/gtest.h"
#include "storm-config.h"

#include "storm/logic/Formulas.h"
#include "storm/parser/FormulaParser.h"

#include "storm/utility/solver.h"
#include "storm/storage/SymbolicModelDescription.h"
#include "storm/modelchecker/prctl/SymbolicMdpPrctlModelChecker.h"
#include "storm/modelchecker/results/SymbolicQualitativeCheckResult.h"
#include "storm/modelchecker/results/SymbolicQuantitativeCheckResult.h"
#include "storm/parser/PrismParser.h"
#include "storm/builder/DdPrismModelBuilder.h"
#include "storm/models/symbolic/Dtmc.h"
#include "storm/models/symbolic/StandardRewardModel.h"
#include "storm/settings/SettingsManager.h"

#include "storm/settings/modules/NativeEquationSolverSettings.h"

#include "storm/settings/modules/GeneralSettings.h"

TEST(SymbolicMdpPrctlModelCheckerTest, Dice_Cudd) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/two_dice.nm");
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
    options.rewardModelsToBuild.insert("coinflips");
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program, options);
    EXPECT_EQ(169ul, model->getNumberOfStates());
    EXPECT_EQ(436ul, model->getNumberOfTransitions());
    
    ASSERT_EQ(model->getType(), storm::models::ModelType::Mdp);
    
    std::shared_ptr<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>> mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>();
    
    storm::modelchecker::SymbolicMdpPrctlModelChecker<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD, double>> checker(*mdp, std::unique_ptr<storm::solver::GeneralSymbolicMinMaxLinearEquationSolverFactory<storm::dd::DdType::CUDD, double>>(new storm::solver::GeneralSymbolicMinMaxLinearEquationSolverFactory<storm::dd::DdType::CUDD, double>()));
    
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F \"two\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult1 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(0.0277777612209320068, quantitativeResult1.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.0277777612209320068, quantitativeResult1.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Pmax=? [F \"two\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult2 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(0.0277777612209320068, quantitativeResult2.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.0277777612209320068, quantitativeResult2.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F \"three\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult3 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(0.0555555224418640136, quantitativeResult3.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.0555555224418640136, quantitativeResult3.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Pmax=? [F \"three\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult4 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(0.0555555224418640136, quantitativeResult4.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.0555555224418640136, quantitativeResult4.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F \"four\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult5 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(0.083333283662796020508, quantitativeResult5.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.083333283662796020508, quantitativeResult5.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Pmax=? [F \"four\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult6 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(0.083333283662796020508, quantitativeResult6.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.083333283662796020508, quantitativeResult6.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Rmin=? [F \"done\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult7 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(7.3333317041397095, quantitativeResult7.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(7.3333317041397095, quantitativeResult7.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Rmax=? [F \"done\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult8 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(7.3333294987678528, quantitativeResult8.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(7.3333294987678528, quantitativeResult8.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
}

TEST(SymbolicMdpPrctlModelCheckerTest, Dice_Sylvan) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/two_dice.nm");
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
    options.rewardModelsToBuild.insert("coinflips");
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program, options);
    EXPECT_EQ(169ul, model->getNumberOfStates());
    EXPECT_EQ(436ul, model->getNumberOfTransitions());
    
    ASSERT_EQ(model->getType(), storm::models::ModelType::Mdp);
    
    std::shared_ptr<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>> mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>();
    
    storm::modelchecker::SymbolicMdpPrctlModelChecker<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan, double>> checker(*mdp, std::unique_ptr<storm::solver::GeneralSymbolicMinMaxLinearEquationSolverFactory<storm::dd::DdType::Sylvan, double>>(new storm::solver::GeneralSymbolicMinMaxLinearEquationSolverFactory<storm::dd::DdType::Sylvan, double>()));
    
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F \"two\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult1 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(0.0277777612209320068, quantitativeResult1.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.0277777612209320068, quantitativeResult1.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Pmax=? [F \"two\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult2 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(0.0277777612209320068, quantitativeResult2.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.0277777612209320068, quantitativeResult2.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F \"three\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult3 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(0.0555555224418640136, quantitativeResult3.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.0555555224418640136, quantitativeResult3.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Pmax=? [F \"three\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult4 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(0.0555555224418640136, quantitativeResult4.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.0555555224418640136, quantitativeResult4.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F \"four\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult5 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(0.083333283662796020508, quantitativeResult5.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.083333283662796020508, quantitativeResult5.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Pmax=? [F \"four\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult6 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(0.083333283662796020508, quantitativeResult6.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.083333283662796020508, quantitativeResult6.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Rmin=? [F \"done\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult7 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(7.3333317041397095, quantitativeResult7.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(7.3333317041397095, quantitativeResult7.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Rmax=? [F \"done\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult8 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(7.3333294987678528, quantitativeResult8.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(7.3333294987678528, quantitativeResult8.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
}

TEST(SymbolicMdpPrctlModelCheckerTest, AsynchronousLeader_Cudd) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/leader4.nm");
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
    options.rewardModelsToBuild.insert("rounds");
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program, options);
    EXPECT_EQ(3172ul, model->getNumberOfStates());
    EXPECT_EQ(7144ul, model->getNumberOfTransitions());
    
    ASSERT_EQ(model->getType(), storm::models::ModelType::Mdp);
    
    std::shared_ptr<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>> mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>();
    
    storm::modelchecker::SymbolicMdpPrctlModelChecker<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD, double>> checker(*mdp, std::unique_ptr<storm::solver::GeneralSymbolicMinMaxLinearEquationSolverFactory<storm::dd::DdType::CUDD, double>>(new storm::solver::GeneralSymbolicMinMaxLinearEquationSolverFactory<storm::dd::DdType::CUDD, double>()));
    
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F \"elected\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult1 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(1, quantitativeResult1.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(1, quantitativeResult1.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Pmax=? [F \"elected\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult2 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(1, quantitativeResult2.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(1, quantitativeResult2.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F<=25 \"elected\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult3 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(0.0625, quantitativeResult3.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.0625, quantitativeResult3.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Pmax=? [F<=25 \"elected\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult4 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(0.0625, quantitativeResult4.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.0625, quantitativeResult4.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Rmin=? [F \"elected\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult5 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(4.2856904569131631, quantitativeResult5.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(4.2856904569131631, quantitativeResult5.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Rmax=? [F \"elected\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult6 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(4.2856904354441401, quantitativeResult6.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(4.2856904354441401, quantitativeResult6.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
}

TEST(SymbolicMdpPrctlModelCheckerTest, AsynchronousLeader_Sylvan) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/leader4.nm");
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
    options.rewardModelsToBuild.insert("rounds");
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program, options);
    EXPECT_EQ(3172ul, model->getNumberOfStates());
    EXPECT_EQ(7144ul, model->getNumberOfTransitions());
    
    ASSERT_EQ(model->getType(), storm::models::ModelType::Mdp);
    
    std::shared_ptr<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>> mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>();
    
    storm::modelchecker::SymbolicMdpPrctlModelChecker<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan, double>> checker(*mdp, std::unique_ptr<storm::solver::GeneralSymbolicMinMaxLinearEquationSolverFactory<storm::dd::DdType::Sylvan, double>>(new storm::solver::GeneralSymbolicMinMaxLinearEquationSolverFactory<storm::dd::DdType::Sylvan, double>()));
    
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F \"elected\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult1 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(1, quantitativeResult1.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(1, quantitativeResult1.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Pmax=? [F \"elected\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult2 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(1, quantitativeResult2.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(1, quantitativeResult2.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F<=25 \"elected\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult3 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(0.0625, quantitativeResult3.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.0625, quantitativeResult3.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Pmax=? [F<=25 \"elected\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult4 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(0.0625, quantitativeResult4.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.0625, quantitativeResult4.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Rmin=? [F \"elected\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult5 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    // FIXME: this precision bound is not really good.
    EXPECT_NEAR(4.2857, quantitativeResult5.getMin(), 100 * storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(4.2857, quantitativeResult5.getMax(), 100 * storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Rmax=? [F \"elected\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult6 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    // FIXME: this precision bound is not really good.
    EXPECT_NEAR(4.2857, quantitativeResult6.getMin(), 100 * storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(4.2857, quantitativeResult6.getMax(), 100 * storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
}
