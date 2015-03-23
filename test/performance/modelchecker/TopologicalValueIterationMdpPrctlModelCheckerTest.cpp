#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "src/utility/solver.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "src/settings/SettingsManager.h"
#include "src/settings/SettingMemento.h"
#include "src/parser/AutoParser.h"

TEST(TopologicalValueIterationMdpPrctlModelCheckerTest, AsynchronousLeader) {
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.tra", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.lab", "", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.trans.rew")->as<storm::models::sparse::Mdp<double>>();

	ASSERT_EQ(mdp->getNumberOfStates(), 2095783ull);
	ASSERT_EQ(mdp->getNumberOfTransitions(), 7714385ull);

	storm::modelchecker::SparseMdpPrctlModelChecker<double> mc(*mdp, std::unique_ptr<storm::utility::solver::NondeterministicLinearEquationSolverFactory<double>>(new storm::utility::solver::TopologicalNondeterministicLinearEquationSolverFactory<double>()));

	auto apFormula = std::make_shared<storm::logic::AtomicLabelFormula>("elected");
	auto eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(apFormula);
	auto probFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Minimize, eventuallyFormula);

	std::unique_ptr<storm::modelchecker::CheckResult> result = mc.check(*probFormula);

	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 1.0),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
	probFormula.reset();

	apFormula = std::make_shared<storm::logic::AtomicLabelFormula>("elected");
	eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(apFormula);
	probFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Maximize, eventuallyFormula);

	result = mc.check(*probFormula);

	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 1.0),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
	probFormula.reset();

	apFormula = std::make_shared<storm::logic::AtomicLabelFormula>("elected");
	auto boundedEventuallyFormula = std::make_shared<storm::logic::BoundedUntilFormula>(std::make_shared<storm::logic::BooleanLiteralFormula>(true), apFormula, 25);
	probFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Minimize, boundedEventuallyFormula);

	result = mc.check(*probFormula);

	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 0.0),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
	probFormula.reset();

	apFormula = std::make_shared<storm::logic::AtomicLabelFormula>("elected");
	boundedEventuallyFormula = std::make_shared<storm::logic::BoundedUntilFormula>(std::make_shared<storm::logic::BooleanLiteralFormula>(true), apFormula, 25);
	probFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Maximize, boundedEventuallyFormula);

	result = mc.check(*probFormula);

	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 0.0),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
	probFormula.reset();

	apFormula = std::make_shared<storm::logic::AtomicLabelFormula>("elected");
	auto reachabilityRewardFormula = std::make_shared<storm::logic::ReachabilityRewardFormula>(apFormula);
	auto rewardFormula = std::make_shared<storm::logic::RewardOperatorFormula>(storm::logic::OptimalityType::Minimize, reachabilityRewardFormula);

	result = mc.check(*rewardFormula);

	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 6.172433512),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
	rewardFormula.reset();

	apFormula = std::make_shared<storm::logic::AtomicLabelFormula>("elected");
	reachabilityRewardFormula = std::make_shared<storm::logic::ReachabilityRewardFormula>(apFormula);
	rewardFormula = std::make_shared<storm::logic::RewardOperatorFormula>(storm::logic::OptimalityType::Maximize, reachabilityRewardFormula);

	result = mc.check(*rewardFormula);

	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 6.1724344),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
	rewardFormula.reset();
}

TEST(TopologicalValueIterationMdpPrctlModelCheckerTest, Consensus) {
    // Increase the maximal number of iterations, because the solver does not converge otherwise.
	// This is done in the main cpp unit
    
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.tra", STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.lab", STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.steps.state.rew", "")->as<storm::models::sparse::Mdp<double>>();
    
	ASSERT_EQ(mdp->getNumberOfStates(), 63616ull);
	ASSERT_EQ(mdp->getNumberOfTransitions(), 213472ull);
    
	storm::modelchecker::SparseMdpPrctlModelChecker<double> mc(*mdp, std::unique_ptr<storm::utility::solver::NondeterministicLinearEquationSolverFactory<double>>(new storm::utility::solver::TopologicalNondeterministicLinearEquationSolverFactory<double>()));

	auto apFormula = std::make_shared<storm::logic::AtomicLabelFormula>("finished");
	auto eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(apFormula);
	auto probFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Minimize, eventuallyFormula);
    
	auto result = mc.check(*probFormula);
    
	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[31168] - 1.0),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
	probFormula.reset();
    
	apFormula = std::make_shared<storm::logic::AtomicLabelFormula>("finished");
	auto apFormula2 = std::make_shared<storm::logic::AtomicLabelFormula>("all_coins_equal_0");
	auto andFormula = std::make_shared<storm::logic::BinaryBooleanStateFormula>(storm::logic::BinaryBooleanStateFormula::OperatorType::And, apFormula, apFormula2);
	eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(andFormula);
	probFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Minimize, eventuallyFormula);
    
	result = mc.check(*probFormula);

	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[31168] - 0.4374282832),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
	probFormula.reset();
    

	apFormula = std::make_shared<storm::logic::AtomicLabelFormula>("finished");
	apFormula2 = std::make_shared<storm::logic::AtomicLabelFormula>("all_coins_equal_1");
	andFormula = std::make_shared<storm::logic::BinaryBooleanStateFormula>(storm::logic::BinaryBooleanStateFormula::OperatorType::And, apFormula, apFormula2);
	eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(andFormula);
	probFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Maximize, eventuallyFormula);
    
	result = mc.check(*probFormula);
    
	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[31168] - 0.5293286369),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
	probFormula.reset();
    
	apFormula = std::make_shared<storm::logic::AtomicLabelFormula>("finished");
	apFormula2 = std::make_shared<storm::logic::AtomicLabelFormula>("agree");
	auto notFormula = std::make_shared<storm::logic::UnaryBooleanStateFormula>(storm::logic::UnaryBooleanStateFormula::OperatorType::Not, apFormula2);
	andFormula = std::make_shared<storm::logic::BinaryBooleanStateFormula>(storm::logic::BinaryBooleanStateFormula::OperatorType::And, apFormula, notFormula);
	eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(andFormula);
	probFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Maximize, eventuallyFormula);

	result = mc.check(*probFormula);
    
	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[31168] - 0.10414097),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
	probFormula.reset();

	apFormula = std::make_shared<storm::logic::AtomicLabelFormula>("finished");
	auto boundedEventuallyFormula = std::make_shared<storm::logic::BoundedUntilFormula>(std::make_shared<storm::logic::BooleanLiteralFormula>(true), apFormula, 50ull);
	probFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Minimize, eventuallyFormula);
    
	result = mc.check(*probFormula);
    
	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[31168] - 0.0),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
	probFormula.reset();
    
	apFormula = std::make_shared<storm::logic::AtomicLabelFormula>("finished");
	boundedEventuallyFormula = std::make_shared<storm::logic::BoundedUntilFormula>(std::make_shared<storm::logic::BooleanLiteralFormula>(true), apFormula, 50ull);
	probFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Maximize, eventuallyFormula);
    
	result = mc.check(*probFormula);

	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[31168] - 0.0),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
	probFormula.reset();

	apFormula = std::make_shared<storm::logic::AtomicLabelFormula>("finished");
	auto reachabilityRewardFormula = std::make_shared<storm::logic::ReachabilityRewardFormula>(apFormula);
	auto rewardFormula = std::make_shared<storm::logic::RewardOperatorFormula>(storm::logic::OptimalityType::Minimize, reachabilityRewardFormula);

	result = mc.check(*rewardFormula);
    
	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[31168] - 1725.593313),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
	probFormula.reset();
    
	apFormula = std::make_shared<storm::logic::AtomicLabelFormula>("finished");
	reachabilityRewardFormula = std::make_shared<storm::logic::ReachabilityRewardFormula>(apFormula);
	rewardFormula = std::make_shared<storm::logic::RewardOperatorFormula>(storm::logic::OptimalityType::Maximize, reachabilityRewardFormula);

	result = mc.check(*rewardFormula);

	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[31168] - 2183.142422),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
	probFormula.reset();
}