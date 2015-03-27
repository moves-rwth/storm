#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/settings/SettingsManager.h"
#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "src/utility/solver.h"
#include "src/parser/AutoParser.h"

TEST(GmxxMdpPrctlModelCheckerTest, AsynchronousLeader) {
    std::shared_ptr<storm::models::sparse::Model<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.tra", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.lab", "", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.trans.rew");
    
    ASSERT_EQ(abstractModel->getType(), storm::models::ModelType::Mdp);
    
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = abstractModel->as<storm::models::sparse::Mdp<double>>();
    
    ASSERT_EQ(2095783ull, mdp->getNumberOfStates());
    ASSERT_EQ(7714385ull, mdp->getNumberOfTransitions());
    
    storm::modelchecker::SparseMdpPrctlModelChecker<double> checker(*mdp, std::unique_ptr<storm::utility::solver::MinMaxLinearEquationSolverFactory<double>>(new storm::utility::solver::GmmxxMinMaxLinearEquationSolverFactory<double>()));
    
    auto labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("elected");
    auto eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(labelFormula);
    auto minProbabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Minimize, eventuallyFormula);
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*minProbabilityOperatorFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult1 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(1.0, quantitativeResult1[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    auto maxProbabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Maximize, eventuallyFormula);
    
    result = checker.check(*maxProbabilityOperatorFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult2 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(1.0, quantitativeResult2[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("elected");
    auto trueFormula = std::make_shared<storm::logic::BooleanLiteralFormula>(true);
    auto boundedUntilFormula = std::make_shared<storm::logic::BoundedUntilFormula>(trueFormula, labelFormula, 25);
    minProbabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Minimize, boundedUntilFormula);
    
    result = checker.check(*minProbabilityOperatorFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult3 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.0, quantitativeResult3[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    maxProbabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Maximize, boundedUntilFormula);
    
    result = checker.check(*maxProbabilityOperatorFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult4 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.0, quantitativeResult4[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("elected");
    auto reachabilityRewardFormula = std::make_shared<storm::logic::ReachabilityRewardFormula>(labelFormula);
    auto minRewardOperatorFormula = std::make_shared<storm::logic::RewardOperatorFormula>(storm::logic::OptimalityType::Minimize, reachabilityRewardFormula);
    
    result = checker.check(*minRewardOperatorFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult5 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(6.172433512, quantitativeResult5[0], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    auto maxRewardOperatorFormula = std::make_shared<storm::logic::RewardOperatorFormula>(storm::logic::OptimalityType::Maximize, reachabilityRewardFormula);
    
    result = checker.check(*maxRewardOperatorFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult6 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(6.1724344, quantitativeResult6[0], storm::settings::nativeEquationSolverSettings().getPrecision());
}

TEST(GmxxMdpPrctlModelCheckerTest, Consensus) {
    std::shared_ptr<storm::models::sparse::Model<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.tra", STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.lab", STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.steps.state.rew", "");
    
    ASSERT_EQ(abstractModel->getType(), storm::models::ModelType::Mdp);
    
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = abstractModel->as<storm::models::sparse::Mdp<double>>();
    
    ASSERT_EQ(63616ull, mdp->getNumberOfStates());
    ASSERT_EQ(213472ull, mdp->getNumberOfTransitions());
    
    storm::modelchecker::SparseMdpPrctlModelChecker<double> checker(*mdp, std::unique_ptr<storm::utility::solver::MinMaxLinearEquationSolverFactory<double>>(new storm::utility::solver::GmmxxMinMaxLinearEquationSolverFactory<double>()));
    
    auto labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("finished");
    auto eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(labelFormula);
    auto minProbabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Minimize, eventuallyFormula);
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*minProbabilityOperatorFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult1 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(1.0, quantitativeResult1[31168], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("finished");
    auto labelFormula2 = std::make_shared<storm::logic::AtomicLabelFormula>("all_coins_equal_0");
    auto andFormula = std::make_shared<storm::logic::BinaryBooleanStateFormula>(storm::logic::BinaryBooleanStateFormula::OperatorType::And, labelFormula, labelFormula2);
    eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(andFormula);
    minProbabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Minimize, eventuallyFormula);
    
    result = checker.check(*minProbabilityOperatorFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult2 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.4374282832, quantitativeResult2[31168], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("finished");
    labelFormula2 = std::make_shared<storm::logic::AtomicLabelFormula>("all_coins_equal_1");
    andFormula = std::make_shared<storm::logic::BinaryBooleanStateFormula>(storm::logic::BinaryBooleanStateFormula::OperatorType::And, labelFormula, labelFormula2);
    eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(andFormula);
    auto maxProbabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Maximize, eventuallyFormula);
    
    result = checker.check(*maxProbabilityOperatorFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult3 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.5293286369, quantitativeResult3[31168], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("finished");
    labelFormula2 = std::make_shared<storm::logic::AtomicLabelFormula>("agree");
    auto notFormula = std::make_shared<storm::logic::UnaryBooleanStateFormula>(storm::logic::UnaryBooleanStateFormula::OperatorType::Not, labelFormula2);
    andFormula = std::make_shared<storm::logic::BinaryBooleanStateFormula>(storm::logic::BinaryBooleanStateFormula::OperatorType::And, labelFormula, notFormula);
    eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(andFormula);
    maxProbabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Maximize, eventuallyFormula);
    
    result = checker.check(*maxProbabilityOperatorFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult4 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.10414097, quantitativeResult4[31168], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("finished");
    auto trueFormula = std::make_shared<storm::logic::BooleanLiteralFormula>(true);
    auto boundedUntilFormula = std::make_shared<storm::logic::BoundedUntilFormula>(trueFormula, labelFormula, 50);
    minProbabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Minimize, boundedUntilFormula);
    
    result = checker.check(*minProbabilityOperatorFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult5 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.0, quantitativeResult5[31168], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    maxProbabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Maximize, boundedUntilFormula);
    
    result = checker.check(*maxProbabilityOperatorFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult6 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.0, quantitativeResult6[31168], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("finished");
    auto reachabilityRewardFormula = std::make_shared<storm::logic::ReachabilityRewardFormula>(labelFormula);
    auto minRewardOperatorFormula = std::make_shared<storm::logic::RewardOperatorFormula>(storm::logic::OptimalityType::Minimize, reachabilityRewardFormula);
    
    result = checker.check(*minRewardOperatorFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult7 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(1725.593313, quantitativeResult7[31168], storm::settings::nativeEquationSolverSettings().getPrecision());
    
    auto maxRewardOperatorFormula = std::make_shared<storm::logic::RewardOperatorFormula>(storm::logic::OptimalityType::Maximize, reachabilityRewardFormula);
    
    result = checker.check(*maxRewardOperatorFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeResult8 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(2183.142422, quantitativeResult8[31168], storm::settings::nativeEquationSolverSettings().getPrecision());
}
