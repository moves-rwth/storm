#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/parser/FormulaParser.h"
#include "src/logic/Formulas.h"
#include "src/utility/solver.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "src/settings/SettingsManager.h"
#include "src/settings/modules/GeneralSettings.h"

#include "src/settings/modules/GmmxxEquationSolverSettings.h"

#include "src/settings/modules/NativeEquationSolverSettings.h"
#include "src/parser/AutoParser.h"

TEST(GmmxxMdpPrctlModelCheckerTest, Dice) {
    std::shared_ptr<storm::models::sparse::Model<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.tra", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.lab", "", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.trans.rew");
    
    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser parser;
    
    ASSERT_EQ(abstractModel->getType(), storm::models::ModelType::Mdp);
    
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = abstractModel->as<storm::models::sparse::Mdp<double>>();
    
    ASSERT_EQ(mdp->getNumberOfStates(), 169ull);
    ASSERT_EQ(mdp->getNumberOfTransitions(), 436ull);
    
    storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>> checker(*mdp, std::unique_ptr<storm::utility::solver::MinMaxLinearEquationSolverFactory<double>>(new storm::utility::solver::GmmxxMinMaxLinearEquationSolverFactory<double>()));
    
    std::shared_ptr<storm::logic::Formula> formula = parser.parseFromString("Pmin=? [F \"two\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult1 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.0277777612209320068, quantitativeResult1[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("Pmax=? [F \"two\"]");
    
    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult2 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.0277777612209320068, quantitativeResult2[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("Pmin=? [F \"three\"]");
    
    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult3 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.0555555224418640136, quantitativeResult3[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("Pmax=? [F \"three\"]");
    
    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult4 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.0555555224418640136, quantitativeResult4[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("Pmin=? [F \"four\"]");
    
    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult5 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.083333283662796020508, quantitativeResult5[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("Pmax=? [F \"four\"]");
    
    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult6 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.083333283662796020508, quantitativeResult6[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("Rmin=? [F \"done\"]");
    
    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult7 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(7.333329499, quantitativeResult7[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("Rmax=? [F \"done\"]");
    
    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult8 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(7.333329499, quantitativeResult8[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.tra", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.lab", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.state.rew", "");
    
    ASSERT_EQ(abstractModel->getType(), storm::models::ModelType::Mdp);
    
    std::shared_ptr<storm::models::sparse::Mdp<double>> stateRewardMdp = abstractModel->as<storm::models::sparse::Mdp<double>>();
    
    storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>> stateRewardModelChecker(*mdp, std::unique_ptr<storm::utility::solver::MinMaxLinearEquationSolverFactory<double>>(new storm::utility::solver::GmmxxMinMaxLinearEquationSolverFactory<double>()));
    
    formula = parser.parseFromString("Rmin=? [F \"done\"]");
    
    result = stateRewardModelChecker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult9 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(7.333329499, quantitativeResult9[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("Rmax=? [F \"done\"]");
    
    result = stateRewardModelChecker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult10 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(7.333329499, quantitativeResult10[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.tra", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.lab", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.state.rew", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.trans.rew");
    
    ASSERT_EQ(abstractModel->getType(), storm::models::ModelType::Mdp);
    
    std::shared_ptr<storm::models::sparse::Mdp<double>> stateAndTransitionRewardMdp = abstractModel->as<storm::models::sparse::Mdp<double>>();
    
    storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>> stateAndTransitionRewardModelChecker(*stateAndTransitionRewardMdp, std::unique_ptr<storm::utility::solver::MinMaxLinearEquationSolverFactory<double>>(new storm::utility::solver::GmmxxMinMaxLinearEquationSolverFactory<double>()));
    
    formula = parser.parseFromString("Rmin=? [F \"done\"]");
    
    result = stateAndTransitionRewardModelChecker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult11 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(14.666658998, quantitativeResult11[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("Rmax=? [F \"done\"]");
    
    result = stateAndTransitionRewardModelChecker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult12 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(14.666658998, quantitativeResult12[0], storm::settings::nativeEquationSolverSettings().getPrecision());
}

TEST(GmmxxMdpPrctlModelCheckerTest, AsynchronousLeader) {
    std::shared_ptr<storm::models::sparse::Model<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader4.tra", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader4.lab", "", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader4.trans.rew");
    
    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser parser;
    
    ASSERT_EQ(storm::models::ModelType::Mdp, abstractModel->getType());
    
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = abstractModel->as<storm::models::sparse::Mdp<double>>();
    
    ASSERT_EQ(3172ull, mdp->getNumberOfStates());
    ASSERT_EQ(7144ull, mdp->getNumberOfTransitions());
    
    storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>> checker(*mdp, std::unique_ptr<storm::utility::solver::MinMaxLinearEquationSolverFactory<double>>(new storm::utility::solver::GmmxxMinMaxLinearEquationSolverFactory<double>()));
    
    std::shared_ptr<storm::logic::Formula> formula = parser.parseFromString("Pmin=? [F \"elected\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult1 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(1, quantitativeResult1[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("Pmax=? [F \"elected\"]");
    
    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult2 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(1, quantitativeResult2[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("Pmin=? [F<=25 \"elected\"]");
    
    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult3 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.0625, quantitativeResult3[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("Pmax=? [F<=25 \"elected\"]");
    
    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult4 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.0625, quantitativeResult4[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("Rmin=? [F \"elected\"]");
    
    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult5 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(4.285689611, quantitativeResult5[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("Rmax=? [F \"elected\"]");
    
    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult6 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(4.285689611, quantitativeResult6[0], storm::settings::nativeEquationSolverSettings().getPrecision());
}
