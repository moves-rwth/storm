#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/logic/Formulas.h"
#include "src/solver/GmmxxNondeterministicLinearEquationSolver.h"
#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "src/settings/SettingsManager.h"
#include "src/parser/AutoParser.h"

TEST(GmmxxMdpPrctlModelCheckerTest, Dice) {
    std::shared_ptr<storm::models::sparse::Model<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.tra", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.lab", "", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.trans.rew");
    
    ASSERT_EQ(abstractModel->getType(), storm::models::ModelType::Mdp);
    
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = abstractModel->as<storm::models::sparse::Mdp<double>>();
    
    ASSERT_EQ(mdp->getNumberOfStates(), 169ull);
    ASSERT_EQ(mdp->getNumberOfTransitions(), 436ull);
    
    storm::modelchecker::SparseMdpPrctlModelChecker<double> checker(*mdp, std::shared_ptr<storm::solver::GmmxxNondeterministicLinearEquationSolver<double>>(new storm::solver::GmmxxNondeterministicLinearEquationSolver<double>()));
    
    auto labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("two");
    auto eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(labelFormula);
    auto minProbabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Minimize, eventuallyFormula);
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*minProbabilityOperatorFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult1 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.0277777612209320068, quantitativeResult1[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    auto maxProbabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Maximize, eventuallyFormula);
    
    result = checker.check(*maxProbabilityOperatorFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult2 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.0277777612209320068, quantitativeResult2[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("three");
    eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(labelFormula);
    minProbabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Minimize, eventuallyFormula);
    
    result = checker.check(*minProbabilityOperatorFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult3 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.0555555224418640136, quantitativeResult3[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    maxProbabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Maximize, eventuallyFormula);
    
    result = checker.check(*maxProbabilityOperatorFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult4 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.0555555224418640136, quantitativeResult4[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("four");
    eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(labelFormula);
    minProbabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Minimize, eventuallyFormula);
    
    result = checker.check(*minProbabilityOperatorFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult5 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.083333283662796020508, quantitativeResult5[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    maxProbabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Maximize, eventuallyFormula);
    
    result = checker.check(*maxProbabilityOperatorFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult6 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.083333283662796020508, quantitativeResult6[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("done");
    auto reachabilityRewardFormula = std::make_shared<storm::logic::ReachabilityRewardFormula>(labelFormula);
    auto minRewardOperatorFormula = std::make_shared<storm::logic::RewardOperatorFormula>(storm::logic::OptimalityType::Minimize, reachabilityRewardFormula);
    
    result = checker.check(*minRewardOperatorFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult7 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(7.333329499, quantitativeResult7[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    auto maxRewardOperatorFormula = std::make_shared<storm::logic::RewardOperatorFormula>(storm::logic::OptimalityType::Maximize, reachabilityRewardFormula);
    
    result = checker.check(*maxRewardOperatorFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult8 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(7.333329499, quantitativeResult8[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.tra", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.lab", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.state.rew", "");
    
    ASSERT_EQ(abstractModel->getType(), storm::models::ModelType::Mdp);
    
    std::shared_ptr<storm::models::sparse::Mdp<double>> stateRewardMdp = abstractModel->as<storm::models::sparse::Mdp<double>>();
    
    storm::modelchecker::SparseMdpPrctlModelChecker<double> stateRewardModelChecker(*stateRewardMdp, std::shared_ptr<storm::solver::GmmxxNondeterministicLinearEquationSolver<double>>(new storm::solver::GmmxxNondeterministicLinearEquationSolver<double>()));
    
    labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("done");
    reachabilityRewardFormula = std::make_shared<storm::logic::ReachabilityRewardFormula>(labelFormula);
    minRewardOperatorFormula = std::make_shared<storm::logic::RewardOperatorFormula>(storm::logic::OptimalityType::Minimize, reachabilityRewardFormula);
    
    result = stateRewardModelChecker.check(*minRewardOperatorFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult9 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(7.333329499, quantitativeResult9[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    maxRewardOperatorFormula = std::make_shared<storm::logic::RewardOperatorFormula>(storm::logic::OptimalityType::Maximize, reachabilityRewardFormula);
    
    result = stateRewardModelChecker.check(*maxRewardOperatorFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult10 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(7.333329499, quantitativeResult10[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.tra", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.lab", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.state.rew", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.trans.rew");
    
    ASSERT_EQ(abstractModel->getType(), storm::models::ModelType::Mdp);
    
    std::shared_ptr<storm::models::sparse::Mdp<double>> stateAndTransitionRewardMdp = abstractModel->as<storm::models::sparse::Mdp<double>>();
    
    storm::modelchecker::SparseMdpPrctlModelChecker<double> stateAndTransitionRewardModelChecker(*stateAndTransitionRewardMdp, std::shared_ptr<storm::solver::GmmxxNondeterministicLinearEquationSolver<double>>(new storm::solver::GmmxxNondeterministicLinearEquationSolver<double>()));
    
    labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("done");
    reachabilityRewardFormula = std::make_shared<storm::logic::ReachabilityRewardFormula>(labelFormula);
    minRewardOperatorFormula = std::make_shared<storm::logic::RewardOperatorFormula>(storm::logic::OptimalityType::Minimize, reachabilityRewardFormula);
    
    result = stateAndTransitionRewardModelChecker.check(*minRewardOperatorFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult11 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(14.666658998, quantitativeResult11[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    maxRewardOperatorFormula = std::make_shared<storm::logic::RewardOperatorFormula>(storm::logic::OptimalityType::Maximize, reachabilityRewardFormula);
    
    result = stateAndTransitionRewardModelChecker.check(*maxRewardOperatorFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult12 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(14.666658998, quantitativeResult12[0], storm::settings::nativeEquationSolverSettings().getPrecision());
}

TEST(GmmxxMdpPrctlModelCheckerTest, AsynchronousLeader) {
    std::shared_ptr<storm::models::sparse::Model<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader4.tra", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader4.lab", "", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader4.trans.rew");
    
    ASSERT_EQ(storm::models::ModelType::Mdp, abstractModel->getType());
    
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = abstractModel->as<storm::models::sparse::Mdp<double>>();
    
    ASSERT_EQ(3172ull, mdp->getNumberOfStates());
    ASSERT_EQ(7144ull, mdp->getNumberOfTransitions());
    
    storm::modelchecker::SparseMdpPrctlModelChecker<double> checker(*mdp, std::shared_ptr<storm::solver::GmmxxNondeterministicLinearEquationSolver<double>>(new storm::solver::GmmxxNondeterministicLinearEquationSolver<double>()));
    
    auto labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("elected");
    auto eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(labelFormula);
    auto minProbabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Minimize, eventuallyFormula);
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*minProbabilityOperatorFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult1 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(1, quantitativeResult1[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    auto maxProbabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Maximize, eventuallyFormula);
    
    result = checker.check(*maxProbabilityOperatorFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult2 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(1, quantitativeResult2[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("elected");
    auto trueFormula = std::make_shared<storm::logic::BooleanLiteralFormula>(true);
    auto boundedUntilFormula = std::make_shared<storm::logic::BoundedUntilFormula>(trueFormula, labelFormula, 25);
    minProbabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Minimize, boundedUntilFormula);
    
    result = checker.check(*minProbabilityOperatorFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult3 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.0625, quantitativeResult3[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    maxProbabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Maximize, boundedUntilFormula);
    
    result = checker.check(*maxProbabilityOperatorFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult4 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.0625, quantitativeResult4[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("elected");
    auto reachabilityRewardFormula = std::make_shared<storm::logic::ReachabilityRewardFormula>(labelFormula);
    auto minRewardOperatorFormula = std::make_shared<storm::logic::RewardOperatorFormula>(storm::logic::OptimalityType::Minimize, reachabilityRewardFormula);
    
    result = checker.check(*minRewardOperatorFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult5 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(4.285689611, quantitativeResult5[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    auto maxRewardOperatorFormula = std::make_shared<storm::logic::RewardOperatorFormula>(storm::logic::OptimalityType::Maximize, reachabilityRewardFormula);
    
    result = checker.check(*maxRewardOperatorFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult6 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(4.285689611, quantitativeResult6[0], storm::settings::nativeEquationSolverSettings().getPrecision());
}
