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
    
	auto apFormula = std::make_shared<storm::logic::AtomicLabelFormula>("two");
    //storm::property::prctl::Ap<double>* apFormula = new storm::property::prctl::Ap<double>("two");
	auto eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(apFormula);
	//storm::property::prctl::Eventually<double>* eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	//storm::property::prctl::ProbabilisticNoBoundOperator<double>* probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);
    
	std::unique_ptr<storm::modelchecker::CheckResult> result = mc.check(*eventuallyFormula);
	//std::vector<double> result = mc.checkNoBoundOperator(*probFormula);
	
	ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 0.0277777612209320068),
		storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
	//ASSERT_LT(std::abs(result[0] - 0.0277777612209320068), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	//apFormula = new storm::property::prctl::Ap<double>("two");
	//eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	//probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);
 //   
	//result = mc.checkNoBoundOperator(*probFormula);
 //   
	//ASSERT_LT(std::abs(result[0] - 0.0277777612209320068), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
 //   
	//delete probFormula;
 //   
	//apFormula = new storm::property::prctl::Ap<double>("three");
	//eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	//probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);
 //   
	//result = mc.checkNoBoundOperator(*probFormula);
 //   
	//ASSERT_LT(std::abs(result[0] - 0.0555555224418640136), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
 //   
	//delete probFormula;
 //   
	//apFormula = new storm::property::prctl::Ap<double>("three");
	//eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	//probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);
 //   
	//result = mc.checkNoBoundOperator(*probFormula);
 //   
	//ASSERT_LT(std::abs(result[0] - 0.0555555224418640136), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
 //   
	//delete probFormula;
 //   
	//apFormula = new storm::property::prctl::Ap<double>("four");
	//eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	//probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);
 //   
	//result = mc.checkNoBoundOperator(*probFormula);
 //   
	//ASSERT_LT(std::abs(result[0] - 0.083333283662796020508), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
 //   
	//delete probFormula;
 //   
	//apFormula = new storm::property::prctl::Ap<double>("four");
	//eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	//probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);
 //   
	//result = mc.checkNoBoundOperator(*probFormula);
 //   
	//ASSERT_LT(std::abs(result[0] - 0.083333283662796020508), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
 //   
	//delete probFormula;
 //   
	//apFormula = new storm::property::prctl::Ap<double>("done");
	//storm::property::prctl::ReachabilityReward<double>* reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	//storm::property::prctl::RewardNoBoundOperator<double>* rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, true);
 //   
	//result = mc.checkNoBoundOperator(*rewardFormula);

#ifdef STORM_HAVE_CUDAFORSTORM
	//ASSERT_LT(std::abs(result[0] - 7.333329499), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
#else
	//ASSERT_LT(std::abs(result[0] - 7.33332904), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
#endif
	//delete rewardFormula;
 //   
	//apFormula = new storm::property::prctl::Ap<double>("done");
	//reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	//rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, false);
 //   
	//result = mc.checkNoBoundOperator(*rewardFormula);;

#ifdef STORM_HAVE_CUDAFORSTORM
	//ASSERT_LT(std::abs(result[0] - 7.333329499), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
#else
	//ASSERT_LT(std::abs(result[0] - 7.33333151), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
#endif
	//delete rewardFormula;
	//std::shared_ptr<storm::models::Mdp<double>> stateRewardMdp = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.tra", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.lab", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.state.rew", "")->as<storm::models::Mdp<double>>();
 //   
	//storm::modelchecker::prctl::TopologicalValueIterationMdpPrctlModelChecker<double> stateRewardModelChecker(*stateRewardMdp);

	//apFormula = new storm::property::prctl::Ap<double>("done");
	//reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	//rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, true);
 //   
	//result = stateRewardModelChecker.checkNoBoundOperator(*rewardFormula);
   
#ifdef STORM_HAVE_CUDAFORSTORM
	//ASSERT_LT(std::abs(result[0] - 7.333329499), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
#else
	//ASSERT_LT(std::abs(result[0] - 7.33332904), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
#endif
	//delete rewardFormula;
 //   
	//apFormula = new storm::property::prctl::Ap<double>("done");
	//reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	//rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, false);
 //   
	//result = stateRewardModelChecker.checkNoBoundOperator(*rewardFormula);
    
#ifdef STORM_HAVE_CUDAFORSTORM
	//ASSERT_LT(std::abs(result[0] - 7.333329499), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
#else
	//ASSERT_LT(std::abs(result[0] - 7.33333151), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
#endif
	//delete rewardFormula;
	//std::shared_ptr<storm::models::Mdp<double>> stateAndTransitionRewardMdp = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.tra", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.lab", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.state.rew", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.trans.rew")->as<storm::models::Mdp<double>>();
 //   
	//storm::modelchecker::prctl::TopologicalValueIterationMdpPrctlModelChecker<double> stateAndTransitionRewardModelChecker(*stateAndTransitionRewardMdp);
 //   
	//apFormula = new storm::property::prctl::Ap<double>("done");
	//reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	//rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, true);
 //   
	//result = stateAndTransitionRewardModelChecker.checkNoBoundOperator(*rewardFormula);
    
#ifdef STORM_HAVE_CUDAFORSTORM
	//ASSERT_LT(std::abs(result[0] - 14.666658998), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
#else
	//ASSERT_LT(std::abs(result[0] - 14.6666581), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
#endif
	//delete rewardFormula;
 //   
	//apFormula = new storm::property::prctl::Ap<double>("done");
	//reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	//rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, false);
 //   
	//result = stateAndTransitionRewardModelChecker.checkNoBoundOperator(*rewardFormula);
    
#ifdef STORM_HAVE_CUDAFORSTORM
	//ASSERT_LT(std::abs(result[0] - 14.666658998), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
#else
	//ASSERT_LT(std::abs(result[0] - 14.666663), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
#endif
	//delete rewardFormula;
}

TEST(TopologicalValueIterationMdpPrctlModelCheckerTest, AsynchronousLeader) {
	//storm::settings::Settings* s = storm::settings::Settings::getInstance();
	std::shared_ptr<storm::models::Mdp<double>> mdp = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader4.tra", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader4.lab", "", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader4.trans.rew")->as<storm::models::Mdp<double>>();

	//ASSERT_EQ(mdp->getNumberOfStates(), 3172ull);
	//ASSERT_EQ(mdp->getNumberOfTransitions(), 7144ull);

	//storm::modelchecker::prctl::TopologicalValueIterationMdpPrctlModelChecker<double> mc(*mdp);

	//storm::property::prctl::Ap<double>* apFormula = new storm::property::prctl::Ap<double>("elected");
	//storm::property::prctl::Eventually<double>* eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	//storm::property::prctl::ProbabilisticNoBoundOperator<double>* probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);

	//std::vector<double> result = mc.checkNoBoundOperator(*probFormula);

	//ASSERT_LT(std::abs(result[0] - 1), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	//delete probFormula;

	//apFormula = new storm::property::prctl::Ap<double>("elected");
	//eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	//probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);

	//result = mc.checkNoBoundOperator(*probFormula);

	//ASSERT_LT(std::abs(result[0] - 1), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	//delete probFormula;

	//apFormula = new storm::property::prctl::Ap<double>("elected");
	//storm::property::prctl::BoundedEventually<double>* boundedEventuallyFormula = new storm::property::prctl::BoundedEventually<double>(apFormula, 25);
	//probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(boundedEventuallyFormula, false);

	//result = mc.checkNoBoundOperator(*probFormula);

	//ASSERT_LT(std::abs(result[0] - 0.0625), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	//delete probFormula;

	//apFormula = new storm::property::prctl::Ap<double>("elected");
	//boundedEventuallyFormula = new storm::property::prctl::BoundedEventually<double>(apFormula, 25);
	//probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(boundedEventuallyFormula, true);

	//result = mc.checkNoBoundOperator(*probFormula);

	//ASSERT_LT(std::abs(result[0] - 0.0625), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	//delete probFormula;

	//apFormula = new storm::property::prctl::Ap<double>("elected");
	//storm::property::prctl::ReachabilityReward<double>* reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	//storm::property::prctl::RewardNoBoundOperator<double>* rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, true);

	//result = mc.checkNoBoundOperator(*rewardFormula);

#ifdef STORM_HAVE_CUDAFORSTORM
	//ASSERT_LT(std::abs(result[0] - 4.285689611), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
#else
	//ASSERT_LT(std::abs(result[0] - 4.285701547), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
#endif
	//delete rewardFormula;

	//apFormula = new storm::property::prctl::Ap<double>("elected");
	//reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	//rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, false);

	//result = mc.checkNoBoundOperator(*rewardFormula);

#ifdef STORM_HAVE_CUDAFORSTORM
	//ASSERT_LT(std::abs(result[0] - 4.285689611), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
#else
	//ASSERT_LT(std::abs(result[0] - 4.285703591), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
#endif
	//delete rewardFormula;
}
