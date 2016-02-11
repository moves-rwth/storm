#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/modules/NativeEquationSolverSettings.h"
#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "src/utility/solver.h"
#include "src/parser/AutoParser.h"
#include "src/models/sparse/StandardRewardModel.h"

#include "src/parser/FormulaParser.h"

TEST(GmxxMdpPrctlModelCheckerTest, AsynchronousLeader) {
    std::shared_ptr<storm::models::sparse::Model<double>> abstractModel = storm::parser::AutoParser<>::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.tra", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.lab", "", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.trans.rew");
    
    ASSERT_EQ(abstractModel->getType(), storm::models::ModelType::Mdp);
    
    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser formulaParser;
    
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = abstractModel->as<storm::models::sparse::Mdp<double>>();
    
    ASSERT_EQ(2095783ull, mdp->getNumberOfStates());
    ASSERT_EQ(7714385ull, mdp->getNumberOfTransitions());
    
    storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>> checker(*mdp, std::unique_ptr<storm::utility::solver::MinMaxLinearEquationSolverFactory<double>>(new storm::utility::solver::MinMaxLinearEquationSolverFactory<double>(storm::solver::EquationSolverTypeSelection::Gmmxx)));
    
    std::shared_ptr<const storm::logic::Formula> formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F \"elected\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult1 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(1.0, quantitativeResult1[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Pmax=? [F \"elected\"]");
    
    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult2 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(1.0, quantitativeResult2[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F<=25 \"elected\"]");
    
    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult3 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.0, quantitativeResult3[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Pmax=? [F<=25 \"elected\"]");
    
    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult4 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.0, quantitativeResult4[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Rmin=? [F \"elected\"]");
    
    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult5 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(6.172433512, quantitativeResult5[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Rmax=? [F \"elected\"]");
    
    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult6 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(6.1724344, quantitativeResult6[0], storm::settings::nativeEquationSolverSettings().getPrecision());
}

TEST(GmxxMdpPrctlModelCheckerTest, Consensus) {
    std::shared_ptr<storm::models::sparse::Model<double>> abstractModel = storm::parser::AutoParser<>::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.tra", STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.lab", STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.steps.state.rew", "");
    
    ASSERT_EQ(abstractModel->getType(), storm::models::ModelType::Mdp);
    
    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser formulaParser;
    
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = abstractModel->as<storm::models::sparse::Mdp<double>>();
    
    ASSERT_EQ(63616ull, mdp->getNumberOfStates());
    ASSERT_EQ(213472ull, mdp->getNumberOfTransitions());
    
    storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>> checker(*mdp, std::unique_ptr<storm::utility::solver::MinMaxLinearEquationSolverFactory<double>>(new storm::utility::solver::MinMaxLinearEquationSolverFactory<double>(storm::solver::EquationSolverTypeSelection::Gmmxx)));
    
    std::shared_ptr<const storm::logic::Formula> formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F \"finished\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult1 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(1.0, quantitativeResult1[31168], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F \"finished\" & \"all_coins_equal_0\"]");
    
    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult2 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.4374282832, quantitativeResult2[31168], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Pmax=? [F \"finished\" & \"all_coins_equal_1\"]");
    
    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult3 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.5293286369, quantitativeResult3[31168], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Pmax=? [F \"finished\" & !\"agree\"]");
    
    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult4 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.10414097, quantitativeResult4[31168], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F<=50 \"finished\"]");
    
    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult5 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.0, quantitativeResult5[31168], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Pmax=? [F<=50 \"finished\"]");
    
    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult6 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.0, quantitativeResult6[31168], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Rmin=? [F \"finished\"]");
    
    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult7 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(1725.593313, quantitativeResult7[31168], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("Rmax=? [F \"finished\"]");
    
    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult8 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(2183.142422, quantitativeResult8[31168], storm::settings::nativeEquationSolverSettings().getPrecision());
}
