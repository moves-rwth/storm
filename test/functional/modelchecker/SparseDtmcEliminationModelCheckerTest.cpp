#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/logic/Formulas.h"
#include "src/solver/GmmxxLinearEquationSolver.h"
#include "src/modelchecker/reachability/SparseDtmcEliminationModelChecker.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "src/settings/SettingsManager.h"
#include "src/settings/SettingMemento.h"
#include "src/parser/AutoParser.h"

TEST(SparseDtmcEliminationModelCheckerTest, Die) {
    std::shared_ptr<storm::models::sparse::Model<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/dtmc/die/die.tra", STORM_CPP_BASE_PATH "/examples/dtmc/die/die.lab", "", STORM_CPP_BASE_PATH "/examples/dtmc/die/die.coin_flips.trans.rew");
    
    ASSERT_EQ(abstractModel->getType(), storm::models::ModelType::Dtmc);
    
    std::shared_ptr<storm::models::sparse::Dtmc<double>> dtmc = abstractModel->as<storm::models::sparse::Dtmc<double>>();
    
    ASSERT_EQ(dtmc->getNumberOfStates(), 13ull);
    ASSERT_EQ(dtmc->getNumberOfTransitions(), 20ull);
    
    storm::modelchecker::SparseDtmcEliminationModelChecker<double> checker(*dtmc);
    
    auto labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("one");
    auto eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(labelFormula);
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*eventuallyFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult1 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(1.0/6.0, quantitativeResult1[0], storm::settings::gmmxxEquationSolverSettings().getPrecision());
    
    labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("two");
    eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(labelFormula);
    
    result = checker.check(*eventuallyFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult2 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(1.0/6.0, quantitativeResult2[0], storm::settings::gmmxxEquationSolverSettings().getPrecision());
    
    labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("three");
    eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(labelFormula);
    
    result = checker.check(*eventuallyFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult3 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(1.0/6.0, quantitativeResult3[0], storm::settings::gmmxxEquationSolverSettings().getPrecision());
    
    auto done = std::make_shared<storm::logic::AtomicLabelFormula>("done");
    auto reachabilityRewardFormula = std::make_shared<storm::logic::ReachabilityRewardFormula>(done);
    
    result = checker.check(*reachabilityRewardFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult4 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(11.0/3.0, quantitativeResult4[0], storm::settings::gmmxxEquationSolverSettings().getPrecision());
}

TEST(SparseDtmcEliminationModelCheckerTest, Crowds) {
    std::shared_ptr<storm::models::sparse::Model<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds5_5.tra", STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds5_5.lab", "", "");
    
    ASSERT_EQ(abstractModel->getType(), storm::models::ModelType::Dtmc);
    
    std::shared_ptr<storm::models::sparse::Dtmc<double>> dtmc = abstractModel->as<storm::models::sparse::Dtmc<double>>();
    
    ASSERT_EQ(8607ull, dtmc->getNumberOfStates());
    ASSERT_EQ(15113ull, dtmc->getNumberOfTransitions());
    
    storm::modelchecker::SparseDtmcEliminationModelChecker<double> checker(*dtmc);
    
    auto labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("observe0Greater1");
    auto eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(labelFormula);
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*eventuallyFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult1 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.3328800375801578281, quantitativeResult1[0], storm::settings::gmmxxEquationSolverSettings().getPrecision());
    
    labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("observeIGreater1");
    eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(labelFormula);
    
    result = checker.check(*eventuallyFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult2 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.1522194965, quantitativeResult2[0], storm::settings::gmmxxEquationSolverSettings().getPrecision());
    
    labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("observeOnlyTrueSender");
    eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(labelFormula);
    
    result = checker.check(*eventuallyFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult3 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.32153724292835045, quantitativeResult3[0], storm::settings::gmmxxEquationSolverSettings().getPrecision());
    
    labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("observe0Greater1");
    eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(labelFormula);
    
    auto labelFormula2 = std::make_shared<storm::logic::AtomicLabelFormula>("observeIGreater1");
    auto eventuallyFormula2 = std::make_shared<storm::logic::EventuallyFormula>(labelFormula2);
    auto conditionalFormula = std::make_shared<storm::logic::ConditionalPathFormula>(eventuallyFormula, eventuallyFormula2);
    
    result = checker.check(*conditionalFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult4 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.15330064292476167, quantitativeResult4[0], storm::settings::gmmxxEquationSolverSettings().getPrecision());
    
    labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("observeOnlyTrueSender");
    eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(labelFormula);
    
    labelFormula2 = std::make_shared<storm::logic::AtomicLabelFormula>("observe0Greater1");
    eventuallyFormula2 = std::make_shared<storm::logic::EventuallyFormula>(labelFormula2);
    conditionalFormula = std::make_shared<storm::logic::ConditionalPathFormula>(eventuallyFormula, eventuallyFormula2);
    
    result = checker.check(*conditionalFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult5 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.96592521978041668, quantitativeResult5[0], storm::settings::gmmxxEquationSolverSettings().getPrecision());
}

TEST(SparseDtmcEliminationModelCheckerTest, SynchronousLeader) {
    std::shared_ptr<storm::models::sparse::Model<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader4_8.tra", STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader4_8.lab", "", STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader4_8.pick.trans.rew");
    
    ASSERT_EQ(abstractModel->getType(), storm::models::ModelType::Dtmc);
    std::shared_ptr<storm::models::sparse::Dtmc<double>> dtmc = abstractModel->as<storm::models::sparse::Dtmc<double>>();
    
    ASSERT_EQ(12400ull, dtmc->getNumberOfStates());
    ASSERT_EQ(16495ull, dtmc->getNumberOfTransitions());
    
    storm::modelchecker::SparseDtmcEliminationModelChecker<double> checker(*dtmc);
    
    auto labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("elected");
    auto eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(labelFormula);
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*eventuallyFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult1 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(1.0, quantitativeResult1[0], storm::settings::gmmxxEquationSolverSettings().getPrecision());
    
    labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("elected");
    auto reachabilityRewardFormula = std::make_shared<storm::logic::ReachabilityRewardFormula>(labelFormula);
    
    result = checker.check(*reachabilityRewardFormula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult3 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(1.0448979, quantitativeResult3[0], storm::settings::gmmxxEquationSolverSettings().getPrecision());
}
