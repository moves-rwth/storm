#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/logic/Formulas.h"
#include "src/utility/solver.h"
#include "src/modelchecker/prctl/SymbolicMdpPrctlModelChecker.h"
#include "src/modelchecker/results/SymbolicQualitativeCheckResult.h"
#include "src/modelchecker/results/SymbolicQuantitativeCheckResult.h"
#include "src/parser/FormulaParser.h"
#include "src/parser/PrismParser.h"
#include "src/builder/DdPrismModelBuilder.h"
#include "src/models/symbolic/Dtmc.h"
#include "src/models/symbolic/StandardRewardModel.h"
#include "src/settings/SettingsManager.h"

#include "src/settings/modules/NativeEquationSolverSettings.h"

#include "src/settings/modules/GeneralSettings.h"

TEST(SymbolicMdpPrctlModelCheckerTest, AsynchronousLeader_Cudd) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/performance/builder/leader5.nm");
    
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
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().translateProgram(program, options);
    EXPECT_EQ(27299ul, model->getNumberOfStates());
    EXPECT_EQ(74365ul, model->getNumberOfTransitions());
    
    ASSERT_EQ(model->getType(), storm::models::ModelType::Mdp);
    
    std::shared_ptr<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>> mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>();
    
    storm::modelchecker::SymbolicMdpPrctlModelChecker<storm::dd::DdType::CUDD, double> checker(*mdp, std::unique_ptr<storm::utility::solver::SymbolicMinMaxLinearEquationSolverFactory<storm::dd::DdType::CUDD, double>>(new storm::utility::solver::SymbolicMinMaxLinearEquationSolverFactory<storm::dd::DdType::CUDD, double>()));
    
    std::shared_ptr<const storm::logic::Formula> formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F \"elected\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult1 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(1, quantitativeResult1.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(1, quantitativeResult1.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F<=25 \"elected\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult3 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(0, quantitativeResult3.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(0, quantitativeResult3.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Rmin=? [F \"elected\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult5 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(5.0348834996352601, quantitativeResult5.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(5.0348834996352601, quantitativeResult5.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
}

TEST(SymbolicMdpPrctlModelCheckerTest, AsynchronousLeader_Sylvan) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/performance/builder/leader5.nm");
    
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
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().translateProgram(program, options);
    EXPECT_EQ(27299ul, model->getNumberOfStates());
    EXPECT_EQ(74365ul, model->getNumberOfTransitions());
    
    ASSERT_EQ(model->getType(), storm::models::ModelType::Mdp);
    
    std::shared_ptr<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>> mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>();
    
    storm::modelchecker::SymbolicMdpPrctlModelChecker<storm::dd::DdType::Sylvan, double> checker(*mdp, std::unique_ptr<storm::utility::solver::SymbolicMinMaxLinearEquationSolverFactory<storm::dd::DdType::Sylvan, double>>(new storm::utility::solver::SymbolicMinMaxLinearEquationSolverFactory<storm::dd::DdType::Sylvan, double>()));
    
    std::shared_ptr<const storm::logic::Formula> formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F \"elected\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult1 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(1, quantitativeResult1.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(1, quantitativeResult1.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F<=25 \"elected\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult3 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(0, quantitativeResult3.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(0, quantitativeResult3.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Rmin=? [F \"elected\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult5 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(5.034920501133386, quantitativeResult5.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(5.034920501133386, quantitativeResult5.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
}

TEST(SymbolicMdpPrctlModelCheckerTest, CSMA_Cudd) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/performance/builder/csma3_2.nm");
    
    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser formulaParser(program);
    
    // Build the die model with its reward model.
#ifdef WINDOWS
    storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
#else
    typename storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
#endif
    options.buildAllRewardModels = false;
    options.rewardModelsToBuild.insert("time");
    
    storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD> builder;
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = builder.translateProgram(program, options);
    storm::prism::Program translatedProgram = builder.getTranslatedProgram();
    
    EXPECT_EQ(36850ul, model->getNumberOfStates());
    EXPECT_EQ(55862ul, model->getNumberOfTransitions());
    
    ASSERT_EQ(model->getType(), storm::models::ModelType::Mdp);
    
    std::shared_ptr<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>> mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>();
    
    storm::modelchecker::SymbolicMdpPrctlModelChecker<storm::dd::DdType::CUDD, double> checker(*mdp, std::unique_ptr<storm::utility::solver::SymbolicMinMaxLinearEquationSolverFactory<storm::dd::DdType::CUDD, double>>(new storm::utility::solver::SymbolicMinMaxLinearEquationSolverFactory<storm::dd::DdType::CUDD, double>()));
    
    std::shared_ptr<const storm::logic::Formula> formula = formulaParser.parseSingleFormulaFromString("Pmin=? [ !\"collision_max_backoff\" U \"all_delivered\" ]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult1 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(0.4349662650631545, quantitativeResult1.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(0.4349662650631545, quantitativeResult1.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Pmin=? [ F (min_backoff_after_success < 4) ]");
    formula = formula->substitute(translatedProgram.getConstantsSubstitution());
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult3 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(1, quantitativeResult3.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(1, quantitativeResult3.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Rmin=? [ F \"all_delivered\" ]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult5 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    EXPECT_NEAR(93.624085091252454, quantitativeResult5.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(93.624085091252454, quantitativeResult5.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
}

TEST(SymbolicMdpPrctlModelCheckerTest, CSMA_Sylvan) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/performance/builder/csma3_2.nm");
    
    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser formulaParser(program);
    
    // Build the die model with its reward model.
#ifdef WINDOWS
    storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>::Options options;
#else
    typename storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>::Options options;
#endif
    options.buildAllRewardModels = false;
    options.rewardModelsToBuild.insert("time");
    
    storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan> builder;
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = builder.translateProgram(program, options);
    storm::prism::Program translatedProgram = builder.getTranslatedProgram();
    
    EXPECT_EQ(36850ul, model->getNumberOfStates());
    EXPECT_EQ(55862ul, model->getNumberOfTransitions());
    
    ASSERT_EQ(model->getType(), storm::models::ModelType::Mdp);
    
    std::shared_ptr<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>> mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>();
    
    storm::modelchecker::SymbolicMdpPrctlModelChecker<storm::dd::DdType::Sylvan, double> checker(*mdp, std::unique_ptr<storm::utility::solver::SymbolicMinMaxLinearEquationSolverFactory<storm::dd::DdType::Sylvan, double>>(new storm::utility::solver::SymbolicMinMaxLinearEquationSolverFactory<storm::dd::DdType::Sylvan, double>()));
    
    std::shared_ptr<const storm::logic::Formula> formula = formulaParser.parseSingleFormulaFromString("Pmin=? [ !\"collision_max_backoff\" U \"all_delivered\" ]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult1 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(0.4349666248753522, quantitativeResult1.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(0.4349666248753522, quantitativeResult1.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Pmin=? [ F (min_backoff_after_success < 4) ]");
    formula = formula->substitute(translatedProgram.getConstantsSubstitution());

    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult3 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    EXPECT_NEAR(1, quantitativeResult3.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(1, quantitativeResult3.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Rmin=? [ F \"all_delivered\" ]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>& quantitativeResult5 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    
    // FIXME: not optimal precision.
    EXPECT_NEAR(93.624117712294478, quantitativeResult5.getMin(), 100 * storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(93.624117712294478, quantitativeResult5.getMax(), 100 * storm::settings::nativeEquationSolverSettings().getPrecision());
}