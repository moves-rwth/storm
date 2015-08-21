#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/logic/Formulas.h"
#include "src/utility/solver.h"
#include "src/modelchecker/prctl/HybridMdpPrctlModelChecker.h"
#include "src/modelchecker/results/HybridQuantitativeCheckResult.h"
#include "src/modelchecker/results/SymbolicQualitativeCheckResult.h"
#include "src/modelchecker/results/SymbolicQuantitativeCheckResult.h"
#include "src/parser/PrismParser.h"
#include "src/parser/FormulaParser.h"
#include "src/builder/DdPrismModelBuilder.h"
#include "src/models/symbolic/Dtmc.h"
#include "src/models/symbolic/StandardRewardModel.h"
#include "src/settings/SettingsManager.h"

#include "src/settings/modules/GeneralSettings.h"

#include "src/settings/modules/GmmxxEquationSolverSettings.h"

TEST(GmmxxHybridMdpPrctlModelCheckerTest, Dice) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/two_dice.nm");
    
    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser parser;
    
    // Build the die model with its reward model.
#ifdef WINDOWS
	storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
#else
	typename storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
#endif
    options.buildAllRewardModels = false;
    options.rewardModelsToBuild.insert("coinflips");
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::translateProgram(program, options);
    EXPECT_EQ(169ul, model->getNumberOfStates());
    EXPECT_EQ(436ul, model->getNumberOfTransitions());
    
    ASSERT_EQ(model->getType(), storm::models::ModelType::Mdp);
    
    std::shared_ptr<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>> mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>();
    
    storm::modelchecker::HybridMdpPrctlModelChecker<storm::dd::DdType::CUDD, double> checker(*mdp, std::unique_ptr<storm::utility::solver::MinMaxLinearEquationSolverFactory<double>>(new storm::utility::solver::GmmxxMinMaxLinearEquationSolverFactory<double>()));
    
    std::shared_ptr<storm::logic::Formula> formula = parser.parseFromString("Pmin=? [F \"two\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult1 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(0.0277777612209320068, quantitativeResult1.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(0.0277777612209320068, quantitativeResult1.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("Pmax=? [F \"two\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult2 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(0.0277777612209320068, quantitativeResult2.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(0.0277777612209320068, quantitativeResult2.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("Pmin=? [F \"three\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult3 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(0.0555555224418640136, quantitativeResult3.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(0.0555555224418640136, quantitativeResult3.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("Pmax=? [F \"three\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult4 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(0.0555555224418640136, quantitativeResult4.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(0.0555555224418640136, quantitativeResult4.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("Pmin=? [F \"four\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult5 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(0.083333283662796020508, quantitativeResult5.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(0.083333283662796020508, quantitativeResult5.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("Pmax=? [F \"four\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult6 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(0.083333283662796020508, quantitativeResult6.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(0.083333283662796020508, quantitativeResult6.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("Rmin=? [F \"done\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult7 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(7.3333283960819244, quantitativeResult7.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(7.3333283960819244, quantitativeResult7.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("Rmax=? [F \"done\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult8 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(7.3333283960819244, quantitativeResult8.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(7.3333283960819244, quantitativeResult8.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
}

TEST(GmmxxHybridMdpPrctlModelCheckerTest, AsynchronousLeader) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/leader4.nm");
    
    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser parser;
    
	// Build the die model with its reward model.
#ifdef WINDOWS
	storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
#else
	typename storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
#endif
    options.buildAllRewardModels = false;
    options.rewardModelsToBuild.insert("rounds");
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::translateProgram(program, options);
    EXPECT_EQ(3172ul, model->getNumberOfStates());
    EXPECT_EQ(7144ul, model->getNumberOfTransitions());
    
    ASSERT_EQ(model->getType(), storm::models::ModelType::Mdp);
    
    std::shared_ptr<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>> mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>();
    
    storm::modelchecker::HybridMdpPrctlModelChecker<storm::dd::DdType::CUDD, double> checker(*mdp, std::unique_ptr<storm::utility::solver::MinMaxLinearEquationSolverFactory<double>>(new storm::utility::solver::GmmxxMinMaxLinearEquationSolverFactory<double>()));
    
    std::shared_ptr<storm::logic::Formula> formula = parser.parseFromString("Pmin=? [F \"elected\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult1 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(1, quantitativeResult1.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(1, quantitativeResult1.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("Pmax=? [F \"elected\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult2 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(1, quantitativeResult2.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(1, quantitativeResult2.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("Pmin=? [F<=25 \"elected\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult3 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(0.0625, quantitativeResult3.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(0.0625, quantitativeResult3.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("Pmax=? [F<=25 \"elected\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult4 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(0.0625, quantitativeResult4.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(0.0625, quantitativeResult4.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("Rmin=? [F \"elected\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult5 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(4.2856925589077264, quantitativeResult5.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(4.2856925589077264, quantitativeResult5.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    formula = parser.parseFromString("Rmax=? [F \"elected\"]");
    
    result = checker.check(*formula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult6 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(4.2856953906798676, quantitativeResult6.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(4.2856953906798676, quantitativeResult6.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
}
