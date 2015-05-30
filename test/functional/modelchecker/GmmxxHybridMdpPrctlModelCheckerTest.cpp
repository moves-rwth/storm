#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/logic/Formulas.h"
#include "src/utility/solver.h"
#include "src/modelchecker/prctl/HybridMdpPrctlModelChecker.h"
#include "src/modelchecker/results/HybridQuantitativeCheckResult.h"
#include "src/modelchecker/results/SymbolicQualitativeCheckResult.h"
#include "src/modelchecker/results/SymbolicQuantitativeCheckResult.h"
#include "src/parser/PrismParser.h"
#include "src/builder/DdPrismModelBuilder.h"
#include "src/models/symbolic/Dtmc.h"
#include "src/settings/SettingsManager.h"

TEST(GmmxxHybridMdpPrctlModelCheckerTest, Dice) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/two_dice.nm");
    
    // Build the die model with its reward model.
    typename storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
    options.buildRewards = true;
    options.rewardModelName = "coinflips";
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::translateProgram(program, options);
    EXPECT_EQ(169, model->getNumberOfStates());
    EXPECT_EQ(436, model->getNumberOfTransitions());
    
    ASSERT_EQ(model->getType(), storm::models::ModelType::Mdp);
    
    std::shared_ptr<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>> mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>();
    
    storm::modelchecker::HybridMdpPrctlModelChecker<storm::dd::DdType::CUDD, double> checker(*mdp, std::unique_ptr<storm::utility::solver::MinMaxLinearEquationSolverFactory<double>>(new storm::utility::solver::GmmxxMinMaxLinearEquationSolverFactory<double>()));
    
    auto labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("two");
    auto eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(labelFormula);
    auto minProbabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Minimize, eventuallyFormula);
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*minProbabilityOperatorFormula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult1 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(0.0277777612209320068, quantitativeResult1.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(0.0277777612209320068, quantitativeResult1.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    auto maxProbabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Maximize, eventuallyFormula);
    
    result = checker.check(*maxProbabilityOperatorFormula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult2 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(0.0277777612209320068, quantitativeResult2.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(0.0277777612209320068, quantitativeResult2.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("three");
    eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(labelFormula);
    minProbabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Minimize, eventuallyFormula);
    
    result = checker.check(*minProbabilityOperatorFormula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult3 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(0.0555555224418640136, quantitativeResult3.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(0.0555555224418640136, quantitativeResult3.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    maxProbabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Maximize, eventuallyFormula);
    
    result = checker.check(*maxProbabilityOperatorFormula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult4 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(0.0555555224418640136, quantitativeResult4.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(0.0555555224418640136, quantitativeResult4.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("four");
    eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(labelFormula);
    minProbabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Minimize, eventuallyFormula);
    
    result = checker.check(*minProbabilityOperatorFormula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult5 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(0.083333283662796020508, quantitativeResult5.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(0.083333283662796020508, quantitativeResult5.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    maxProbabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Maximize, eventuallyFormula);
    
    result = checker.check(*maxProbabilityOperatorFormula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult6 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(0.083333283662796020508, quantitativeResult6.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(0.083333283662796020508, quantitativeResult6.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("done");
    auto reachabilityRewardFormula = std::make_shared<storm::logic::ReachabilityRewardFormula>(labelFormula);
    auto minRewardOperatorFormula = std::make_shared<storm::logic::RewardOperatorFormula>(storm::logic::OptimalityType::Minimize, reachabilityRewardFormula);
    
    result = checker.check(*minRewardOperatorFormula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult7 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(7.3333283960819244, quantitativeResult7.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(7.3333283960819244, quantitativeResult7.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    auto maxRewardOperatorFormula = std::make_shared<storm::logic::RewardOperatorFormula>(storm::logic::OptimalityType::Maximize, reachabilityRewardFormula);
    
    result = checker.check(*maxRewardOperatorFormula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult8 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(7.3333283960819244, quantitativeResult8.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(7.3333283960819244, quantitativeResult8.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
}

TEST(GmmxxHybridMdpPrctlModelCheckerTest, AsynchronousLeader) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/leader4.nm");
    
    // Build the die model with its reward model.
    typename storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
    options.buildRewards = true;
    options.rewardModelName = "rounds";
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::translateProgram(program, options);
    EXPECT_EQ(3172, model->getNumberOfStates());
    EXPECT_EQ(7144, model->getNumberOfTransitions());
    
    ASSERT_EQ(model->getType(), storm::models::ModelType::Mdp);
    
    std::shared_ptr<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>> mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>();
    
    storm::modelchecker::HybridMdpPrctlModelChecker<storm::dd::DdType::CUDD, double> checker(*mdp, std::unique_ptr<storm::utility::solver::MinMaxLinearEquationSolverFactory<double>>(new storm::utility::solver::GmmxxMinMaxLinearEquationSolverFactory<double>()));
    
    auto labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("elected");
    auto eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(labelFormula);
    auto minProbabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Minimize, eventuallyFormula);
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*minProbabilityOperatorFormula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult1 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(1, quantitativeResult1.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(1, quantitativeResult1.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    auto maxProbabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Maximize, eventuallyFormula);
    
    result = checker.check(*maxProbabilityOperatorFormula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult2 = result->asSymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(1, quantitativeResult2.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(1, quantitativeResult2.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("elected");
    auto trueFormula = std::make_shared<storm::logic::BooleanLiteralFormula>(true);
    auto boundedUntilFormula = std::make_shared<storm::logic::BoundedUntilFormula>(trueFormula, labelFormula, 25);
    minProbabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Minimize, boundedUntilFormula);
    
    result = checker.check(*minProbabilityOperatorFormula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult3 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(0.0625, quantitativeResult3.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(0.0625, quantitativeResult3.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    maxProbabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Maximize, boundedUntilFormula);
    
    result = checker.check(*maxProbabilityOperatorFormula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult4 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(0.0625, quantitativeResult4.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(0.0625, quantitativeResult4.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("elected");
    auto reachabilityRewardFormula = std::make_shared<storm::logic::ReachabilityRewardFormula>(labelFormula);
    auto minRewardOperatorFormula = std::make_shared<storm::logic::RewardOperatorFormula>(storm::logic::OptimalityType::Minimize, reachabilityRewardFormula);
    
    result = checker.check(*minRewardOperatorFormula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult5 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(4.2856925589077264, quantitativeResult5.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(4.2856925589077264, quantitativeResult5.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
    
    auto maxRewardOperatorFormula = std::make_shared<storm::logic::RewardOperatorFormula>(storm::logic::OptimalityType::Maximize, reachabilityRewardFormula);
    
    result = checker.check(*maxRewardOperatorFormula);
    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(model->getReachableStates(), model->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>& quantitativeResult6 = result->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    
    EXPECT_NEAR(4.2856953906798676, quantitativeResult6.getMin(), storm::settings::nativeEquationSolverSettings().getPrecision());
    EXPECT_NEAR(4.2856953906798676, quantitativeResult6.getMax(), storm::settings::nativeEquationSolverSettings().getPrecision());
}
