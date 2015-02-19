#include "gtest/gtest.h"
#include "storm-config.h"


#include "src/logic/Formulas.h"
#include "src/solver/NativeNondeterministicLinearEquationSolver.h"
#include "src/modelchecker/prctl/TopologicalValueIterationMdpPrctlModelChecker.h"
#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "src/modelchecker/ExplicitQuantitativeCheckResult.h"
#include "src/settings/SettingsManager.h"
#include "src/settings/SettingMemento.h"
#include "src/parser/AutoParser.h"

#include "storm-config.h"

TEST(TopologicalValueIterationMdpPrctlModelCheckerTest, Dice) {
	//storm::settings::Settings* s = storm::settings::Settings::getInstance();    
	std::shared_ptr<storm::models::Mdp<double>> mdp = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.tra", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.lab", "", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.trans.rew")->as<storm::models::Mdp<double>>();
    
	ASSERT_EQ(mdp->getNumberOfStates(), 169ull);
	ASSERT_EQ(mdp->getNumberOfTransitions(), 436ull);
    
	storm::modelchecker::prctl::TopologicalValueIterationMdpPrctlModelChecker<double> mc(*mdp);
    
	//storm::property::prctl::Ap<double>* apFormula = new storm::property::prctl::Ap<double>("two");
	auto apFormula = std::make_shared<storm::logic::AtomicLabelFormula>("two");
	//storm::property::prctl::Eventually<double>* eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	auto eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(apFormula);
	//storm::property::prctl::ProbabilisticNoBoundOperator<double>* probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);
	auto probabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Minimize, eventuallyFormula);
    
	//std::vector<double> result = mc.checkNoBoundOperator(*probFormula);
	std::unique_ptr<storm::modelchecker::CheckResult> result = mc.check(*probabilityOperatorFormula);
	
	//ASSERT_LT(std::abs(result[0] - 0.0277777612209320068), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 0.0277777612209320068),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());

	//delete probFormula;
	probabilityOperatorFormula.reset();

	//apFormula = new storm::property::prctl::Ap<double>("two");
	apFormula = std::make_shared<storm::logic::AtomicLabelFormula>("two");
	//eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(apFormula);
	//probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);
	probabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Maximize, eventuallyFormula);
    
	//result = mc.checkNoBoundOperator(*probFormula);
	result = mc.check(*probabilityOperatorFormula);
    
	//ASSERT_LT(std::abs(result[0] - 0.0277777612209320068), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 0.0277777612209320068),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());

	//delete probFormula;
	probabilityOperatorFormula.reset();
   
	// ---------------- test ap "three" ----------------
	apFormula = std::make_shared<storm::logic::AtomicLabelFormula>("three");
	eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(apFormula);
	probabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Minimize, eventuallyFormula);
    
	result = mc.check(*probabilityOperatorFormula);
    
	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 0.0555555224418640136),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
    
	probabilityOperatorFormula.reset();
    
	apFormula = std::make_shared<storm::logic::AtomicLabelFormula>("three");
	eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(apFormula);
	probabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Maximize, eventuallyFormula);
    
	result = mc.check(*probabilityOperatorFormula);
    
	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 0.0555555224418640136),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
    
	probabilityOperatorFormula.reset();
    
	// ---------------- test ap "four" ----------------
	apFormula = std::make_shared<storm::logic::AtomicLabelFormula>("four");
	eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(apFormula);
	probabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Minimize, eventuallyFormula);

	result = mc.check(*probabilityOperatorFormula);

	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 0.083333283662796020508),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());

	probabilityOperatorFormula.reset();

	apFormula = std::make_shared<storm::logic::AtomicLabelFormula>("four");
	eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(apFormula);
	probabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Maximize, eventuallyFormula);

	result = mc.check(*probabilityOperatorFormula);

	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 0.083333283662796020508),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());

	probabilityOperatorFormula.reset();


	// ---------------- test ap "done" ----------------
	apFormula = std::make_shared<storm::logic::AtomicLabelFormula>("done");
	auto reachabilityRewardFormula = std::make_shared<storm::logic::ReachabilityRewardFormula>(apFormula);
	auto rewardFormula = std::make_shared<storm::logic::RewardOperatorFormula>(storm::logic::OptimalityType::Minimize, reachabilityRewardFormula);
    
	result = mc.check(*rewardFormula);

#ifdef STORM_HAVE_CUDA
	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 7.333329499),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
#else
	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 7.33332904),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
#endif
	rewardFormula.reset();
    

	apFormula = std::make_shared<storm::logic::AtomicLabelFormula>("done");
	reachabilityRewardFormula = std::make_shared<storm::logic::ReachabilityRewardFormula>(apFormula);
	rewardFormula = std::make_shared<storm::logic::RewardOperatorFormula>(storm::logic::OptimalityType::Maximize, reachabilityRewardFormula);

	result = mc.check(*rewardFormula);

#ifdef STORM_HAVE_CUDA
	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 7.333329499),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
#else
	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 7.33333151),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
#endif
	rewardFormula.reset();

	// ------------- state rewards --------------
	std::shared_ptr<storm::models::Mdp<double>> stateRewardMdp = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.tra", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.lab", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.state.rew", "")->as<storm::models::Mdp<double>>();
    
	storm::modelchecker::prctl::TopologicalValueIterationMdpPrctlModelChecker<double> stateRewardModelChecker(*stateRewardMdp);


	apFormula = std::make_shared<storm::logic::AtomicLabelFormula>("done");
	reachabilityRewardFormula = std::make_shared<storm::logic::ReachabilityRewardFormula>(apFormula);
	rewardFormula = std::make_shared<storm::logic::RewardOperatorFormula>(storm::logic::OptimalityType::Minimize, reachabilityRewardFormula);

	result = stateRewardModelChecker.check(*rewardFormula);

#ifdef STORM_HAVE_CUDA
	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 7.333329499),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
#else
	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 7.33332904),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
#endif
	rewardFormula.reset();
   
	apFormula = std::make_shared<storm::logic::AtomicLabelFormula>("done");
	reachabilityRewardFormula = std::make_shared<storm::logic::ReachabilityRewardFormula>(apFormula);
	rewardFormula = std::make_shared<storm::logic::RewardOperatorFormula>(storm::logic::OptimalityType::Maximize, reachabilityRewardFormula);

	result = stateRewardModelChecker.check(*rewardFormula);

#ifdef STORM_HAVE_CUDA
	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 7.333329499),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
#else
	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 7.33333151),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
#endif
	rewardFormula.reset();
	
	// -------------------------------- state and transition reward ------------------------
	std::shared_ptr<storm::models::Mdp<double>> stateAndTransitionRewardMdp = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.tra", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.lab", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.state.rew", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.trans.rew")->as<storm::models::Mdp<double>>();
    
	storm::modelchecker::prctl::TopologicalValueIterationMdpPrctlModelChecker<double> stateAndTransitionRewardModelChecker(*stateAndTransitionRewardMdp);
    

	apFormula = std::make_shared<storm::logic::AtomicLabelFormula>("done");
	reachabilityRewardFormula = std::make_shared<storm::logic::ReachabilityRewardFormula>(apFormula);
	rewardFormula = std::make_shared<storm::logic::RewardOperatorFormula>(storm::logic::OptimalityType::Minimize, reachabilityRewardFormula);

	result = stateAndTransitionRewardModelChecker.check(*rewardFormula);

#ifdef STORM_HAVE_CUDA
	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 14.666658998),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
#else
	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 14.6666581),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
#endif
	rewardFormula.reset();

	apFormula = std::make_shared<storm::logic::AtomicLabelFormula>("done");
	reachabilityRewardFormula = std::make_shared<storm::logic::ReachabilityRewardFormula>(apFormula);
	rewardFormula = std::make_shared<storm::logic::RewardOperatorFormula>(storm::logic::OptimalityType::Maximize, reachabilityRewardFormula);

	result = stateAndTransitionRewardModelChecker.check(*rewardFormula);

#ifdef STORM_HAVE_CUDA
	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 14.666658998),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
#else
	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 14.666663),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
#endif
	rewardFormula.reset();
}

TEST(TopologicalValueIterationMdpPrctlModelCheckerTest, AsynchronousLeader) {
	std::shared_ptr<storm::models::Mdp<double>> mdp = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader4.tra", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader4.lab", "", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader4.trans.rew")->as<storm::models::Mdp<double>>();

	ASSERT_EQ(mdp->getNumberOfStates(), 3172ull);
	ASSERT_EQ(mdp->getNumberOfTransitions(), 7144ull);

	storm::modelchecker::prctl::TopologicalValueIterationMdpPrctlModelChecker<double> mc(*mdp);

	auto apFormula = std::make_shared<storm::logic::AtomicLabelFormula>("elected");
	auto eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(apFormula);
	auto probabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Minimize, eventuallyFormula);
	
	std::unique_ptr<storm::modelchecker::CheckResult> result = mc.check(*probabilityOperatorFormula);

	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 1),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());

	probabilityOperatorFormula.reset();

	apFormula = std::make_shared<storm::logic::AtomicLabelFormula>("elected");
	eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(apFormula);
	probabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Maximize, eventuallyFormula);

	result = mc.check(*probabilityOperatorFormula);

	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 1),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());

	probabilityOperatorFormula.reset();

	apFormula = std::make_shared<storm::logic::AtomicLabelFormula>("elected");
	auto boundedEventuallyFormula = std::make_shared<storm::logic::BoundedUntilFormula>(std::make_shared<storm::logic::BooleanLiteralFormula>(true), apFormula, 25);
	probabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Maximize, boundedEventuallyFormula);

	result = mc.check(*probabilityOperatorFormula);

	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 0.0625),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());

	probabilityOperatorFormula.reset();

	apFormula = std::make_shared<storm::logic::AtomicLabelFormula>("elected");
	boundedEventuallyFormula = std::make_shared<storm::logic::BoundedUntilFormula>(std::make_shared<storm::logic::BooleanLiteralFormula>(true), apFormula, 25);
	probabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(storm::logic::OptimalityType::Minimize, boundedEventuallyFormula);

	result = mc.check(*probabilityOperatorFormula);

	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 0.0625),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());

	probabilityOperatorFormula.reset();

	apFormula = std::make_shared<storm::logic::AtomicLabelFormula>("elected");
	auto reachabilityRewardFormula = std::make_shared<storm::logic::ReachabilityRewardFormula>(apFormula);
	auto rewardFormula = std::make_shared<storm::logic::RewardOperatorFormula>(storm::logic::OptimalityType::Minimize, reachabilityRewardFormula);

	result = mc.check(*rewardFormula);

#ifdef STORM_HAVE_CUDA
	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 4.285689611),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
#else
	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 4.285701547),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
#endif

	probabilityOperatorFormula.reset();

	apFormula = std::make_shared<storm::logic::AtomicLabelFormula>("elected");
	reachabilityRewardFormula = std::make_shared<storm::logic::ReachabilityRewardFormula>(apFormula);
	rewardFormula = std::make_shared<storm::logic::RewardOperatorFormula>(storm::logic::OptimalityType::Maximize, reachabilityRewardFormula);

	result = mc.check(*rewardFormula);

#ifdef STORM_HAVE_CUDA
	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 4.285689611),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
#else
	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 4.285703591),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
#endif

	probabilityOperatorFormula.reset();
}
