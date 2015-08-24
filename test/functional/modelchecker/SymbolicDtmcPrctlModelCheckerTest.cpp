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

TEST(SymbolicDtmcPrctlModelCheckerTest, Die) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/die.pm");
    
    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser parser;

    // Build the die model with its reward model.
#ifdef WINDOWS
    storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
#else
    typename storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
#endif
    options.buildAllRewardModels = false;
    options.rewardModelsToBuild.insert("coin_flips");
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::translateProgram(program, options);
    EXPECT_EQ(13ul, model->getNumberOfStates());
    EXPECT_EQ(20ul, model->getNumberOfTransitions());
    
    ASSERT_EQ(model->getType(), storm::models::ModelType::Dtmc);
    
    std::shared_ptr<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD>> dtmc = model->as<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD>>();
    
    storm::modelchecker::SymbolicDtmcPrctlModelChecker<storm::dd::DdType::CUDD, double> checker(*dtmc, std::unique_ptr<storm::utility::solver::SymbolicLinearEquationSolverFactory<storm::dd::DdType::CUDD, double>>(new storm::utility::solver::SymbolicLinearEquationSolverFactory<storm::dd::DdType::CUDD, double>()));
    
    std::shared_ptr<storm::logic::Formula> formula = parser.parseFromString("P=? [F \"one\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult1 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(1.0/6.0, quantitativeResult1.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(1.0/6.0, quantitativeResult1.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("P=? [F \"two\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult2 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(1.0/6.0, quantitativeResult2.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(1.0/6.0, quantitativeResult2.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("P=? [F \"three\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult3 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(1.0/6.0, quantitativeResult3.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(1.0/6.0, quantitativeResult3.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("R=? [F \"done\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult4 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(3.6666622161865234, quantitativeResult4.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(3.6666622161865234, quantitativeResult4.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
}

TEST(SymbolicDtmcPrctlModelCheckerTest, Crowds) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/crowds-5-5.pm");
    
    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser parser;
    
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::translateProgram(program);
    EXPECT_EQ(8607ul, model->getNumberOfStates());
    EXPECT_EQ(15113ul, model->getNumberOfTransitions());
    
    ASSERT_EQ(model->getType(), storm::models::ModelType::Dtmc);
    
    std::shared_ptr<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD>> dtmc = model->as<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD>>();
    
    storm::modelchecker::SymbolicDtmcPrctlModelChecker<storm::dd::DdType::CUDD, double> checker(*dtmc, std::unique_ptr<storm::utility::solver::SymbolicLinearEquationSolverFactory<storm::dd::DdType::CUDD, double>>(new storm::utility::solver::SymbolicLinearEquationSolverFactory<storm::dd::DdType::CUDD, double>()));
    
    std::shared_ptr<storm::logic::Formula> formula = parser.parseFromString("P=? [F \"observe0Greater1\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult1 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(0.33288236360191303, quantitativeResult1.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(0.33288236360191303, quantitativeResult1.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("P=? [F \"observeIGreater1\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult2 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(0.15222081144084315, quantitativeResult2.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(0.15222081144084315, quantitativeResult2.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("P=? [F \"observeOnlyTrueSender\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult3 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(0.3215392962289586, quantitativeResult3.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(0.3215392962289586, quantitativeResult3.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
}

TEST(SymbolicDtmcPrctlModelCheckerTest, SynchronousLeader) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/leader-3-5.pm");
    
    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser parser;
    
    // Build the die model with its reward model.
#ifdef WINDOWS
    storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
#else
    typename storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
#endif
    options.buildAllRewardModels = false;
    options.rewardModelsToBuild.insert("num_rounds");
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::translateProgram(program, options);
    EXPECT_EQ(273ul, model->getNumberOfStates());
    EXPECT_EQ(397ul, model->getNumberOfTransitions());
    
    ASSERT_EQ(model->getType(), storm::models::ModelType::Dtmc);
    
    std::shared_ptr<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD>> dtmc = model->as<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD>>();
    
    storm::modelchecker::SymbolicDtmcPrctlModelChecker<storm::dd::DdType::CUDD, double> checker(*dtmc, std::unique_ptr<storm::utility::solver::SymbolicLinearEquationSolverFactory<storm::dd::DdType::CUDD, double>>(new storm::utility::solver::SymbolicLinearEquationSolverFactory<storm::dd::DdType::CUDD, double>()));
    
    std::shared_ptr<storm::logic::Formula> formula = parser.parseFromString("P=? [F \"elected\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult1 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(1.0, quantitativeResult1.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(1.0, quantitativeResult1.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("P=? [F<=20 \"elected\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult2 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(0.99999989760000074, quantitativeResult2.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(0.99999989760000074, quantitativeResult2.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("R=? [F \"elected\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult3 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(1.0416666666666643, quantitativeResult3.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(1.0416666666666643, quantitativeResult3.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
}

