#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/parser/FormulaParser.h"
#include "src/logic/Formulas.h"
#include "src/utility/solver.h"
#include "src/modelchecker/prctl/SymbolicDtmcPrctlModelChecker.h"
#include "src/modelchecker/results/SymbolicQualitativeCheckResult.h"
#include "src/modelchecker/results/SymbolicQuantitativeCheckResult.h"
#include "src/parser/PrismParser.h"
#include "src/builder/DdPrismModelBuilder.h"
#include "src/models/symbolic/StandardRewardModel.h"
#include "src/models/symbolic/Dtmc.h"
#include "src/settings/SettingsManager.h"

#include "src/settings/modules/NativeEquationSolverSettings.h"

#include "src/settings/modules/GeneralSettings.h"

TEST(SymbolicDtmcPrctlModelCheckerTest, SynchronousLeader_Cudd) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/performance/builder/leader5_8.pm");
    
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
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().translateProgram(program, options);
    EXPECT_EQ(131521ul, model->getNumberOfStates());
    EXPECT_EQ(164288ul, model->getNumberOfTransitions());
    
    ASSERT_EQ(model->getType(), storm::models::ModelType::Dtmc);
    
    std::shared_ptr<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD>> dtmc = model->as<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD>>();
    
    storm::modelchecker::SymbolicDtmcPrctlModelChecker<storm::dd::DdType::CUDD, double> checker(*dtmc, std::unique_ptr<storm::utility::solver::SymbolicLinearEquationSolverFactory<storm::dd::DdType::CUDD, double>>(new storm::utility::solver::SymbolicLinearEquationSolverFactory<storm::dd::DdType::CUDD, double>()));
    
    std::shared_ptr<const storm::logic::Formula> formula = formulaParser.parseSingleFormulaFromString("P=? [F \"elected\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult1 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(1.0, quantitativeResult1.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(1.0, quantitativeResult1.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [F<=20 \"elected\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult2 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(0.9999947917094687, quantitativeResult2.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.9999947917094687, quantitativeResult2.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("R=? [F \"elected\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult3 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(1.0176397951004841, quantitativeResult3.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(1.0176397951004841, quantitativeResult3.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
}

TEST(SymbolicDtmcPrctlModelCheckerTest, SynchronousLeader_Sylvan) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/performance/builder/leader5_8.pm");
    
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
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().translateProgram(program, options);
    EXPECT_EQ(131521ul, model->getNumberOfStates());
    EXPECT_EQ(164288ul, model->getNumberOfTransitions());
    
    ASSERT_EQ(model->getType(), storm::models::ModelType::Dtmc);
    
    std::shared_ptr<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan>> dtmc = model->as<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan>>();
    
    storm::modelchecker::SymbolicDtmcPrctlModelChecker<storm::dd::DdType::Sylvan, double> checker(*dtmc, std::unique_ptr<storm::utility::solver::SymbolicLinearEquationSolverFactory<storm::dd::DdType::Sylvan, double>>(new storm::utility::solver::SymbolicLinearEquationSolverFactory<storm::dd::DdType::Sylvan, double>()));
    
    std::shared_ptr<const storm::logic::Formula> formula = formulaParser.parseSingleFormulaFromString("P=? [F \"elected\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult1 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(1.0, quantitativeResult1.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(1.0, quantitativeResult1.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [F<=20 \"elected\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult2 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(0.9999947917094687, quantitativeResult2.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.9999947917094687, quantitativeResult2.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("R=? [F \"elected\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult3 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(1.0176397951004841, quantitativeResult3.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(1.0176397951004841, quantitativeResult3.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
}

TEST(SymbolicDtmcPrctlModelCheckerTest, Crowds_Cudd) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/performance/builder/crowds15_5.pm");
    
    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser formulaParser;
    
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().translateProgram(program);
    EXPECT_EQ(586242ul, model->getNumberOfStates());
    EXPECT_EQ(1753883ul, model->getNumberOfTransitions());
    
    ASSERT_EQ(model->getType(), storm::models::ModelType::Dtmc);
    
    std::shared_ptr<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD>> dtmc = model->as<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD>>();
    
    storm::modelchecker::SymbolicDtmcPrctlModelChecker<storm::dd::DdType::CUDD, double> checker(*dtmc, std::unique_ptr<storm::utility::solver::SymbolicLinearEquationSolverFactory<storm::dd::DdType::CUDD, double>>(new storm::utility::solver::SymbolicLinearEquationSolverFactory<storm::dd::DdType::CUDD, double>()));
    
    std::shared_ptr<const storm::logic::Formula> formula = formulaParser.parseSingleFormulaFromString("P=? [F \"observe0Greater1\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult1 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(0.24084538502812078, quantitativeResult1.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.24084538502812078, quantitativeResult1.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [F \"observeIGreater1\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult2 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(0.065569806085001583, quantitativeResult2.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.065569806085001583, quantitativeResult2.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [F \"observeOnlyTrueSender\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult3 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(0.23773283919051694, quantitativeResult3.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.23773283919051694, quantitativeResult3.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
}

TEST(SymbolicDtmcPrctlModelCheckerTest, Crowds_Sylvan) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/performance/builder/crowds15_5.pm");
    
    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser formulaParser;
    
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().translateProgram(program);
    EXPECT_EQ(586242ul, model->getNumberOfStates());
    EXPECT_EQ(1753883ul, model->getNumberOfTransitions());
    
    ASSERT_EQ(model->getType(), storm::models::ModelType::Dtmc);
    
    std::shared_ptr<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan>> dtmc = model->as<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan>>();
    
    storm::modelchecker::SymbolicDtmcPrctlModelChecker<storm::dd::DdType::Sylvan, double> checker(*dtmc, std::unique_ptr<storm::utility::solver::SymbolicLinearEquationSolverFactory<storm::dd::DdType::Sylvan, double>>(new storm::utility::solver::SymbolicLinearEquationSolverFactory<storm::dd::DdType::Sylvan, double>()));
    
    std::shared_ptr<const storm::logic::Formula> formula = formulaParser.parseSingleFormulaFromString("P=? [F \"observe0Greater1\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult1 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(0.24084538502812078, quantitativeResult1.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.24084538502812078, quantitativeResult1.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [F \"observeIGreater1\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult2 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(0.065569806085001583, quantitativeResult2.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.065569806085001583, quantitativeResult2.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [F \"observeOnlyTrueSender\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult3 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(0.23773283919051694, quantitativeResult3.getMin(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    EXPECT_NEAR(0.23773283919051694, quantitativeResult3.getMax(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
}