#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/solver/NativeNondeterministicLinearEquationSolver.h"
#include "src/settings/Settings.h"
#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "src/modelchecker/prctl/TopologicalValueIterationMdpPrctlModelChecker.h"
#include "src/parser/AutoParser.h"

TEST(TopologicalValueIterationMdpPrctlModelCheckerTest, SmallLinEqSystem) {
	storm::storage::SparseMatrixBuilder<double> matrixBuilder(3, 3);
	ASSERT_NO_THROW(matrixBuilder.addNextValue(0, 2, 1.0));
	ASSERT_NO_THROW(matrixBuilder.addNextValue(1, 0, 4.0));
	ASSERT_NO_THROW(matrixBuilder.addNextValue(1, 1, 7.0));
	ASSERT_NO_THROW(matrixBuilder.addNextValue(2, 1, 7.0));
	ASSERT_NO_THROW(matrixBuilder.addNextValue(2, 2, -1.0));

	storm::storage::SparseMatrix<double> matrix;
	ASSERT_NO_THROW(matrix = matrixBuilder.build());

	ASSERT_EQ(3, matrix.getRowCount());
	ASSERT_EQ(3, matrix.getColumnCount());
	ASSERT_EQ(5, matrix.getEntryCount());
	
	// Solve the Linear Equation System
	storm::solver::TopologicalValueIterationNondeterministicLinearEquationSolver<double> topoSolver;

	std::vector<double> x(3);
	std::vector<double> b = { 5, 8, 2 };
	std::vector<uint_fast64_t> choices = { 0, 1, 2, 3 };

	ASSERT_NO_THROW(topoSolver.solveEquationSystem(true, matrix, x, b, choices));

	storm::settings::Settings* s = storm::settings::Settings::getInstance();
	ASSERT_LT(std::abs(x.at(0) - 0.25), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
	ASSERT_LT(std::abs(x.at(1) - 1.0), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
	ASSERT_LT(std::abs(x.at(2) - 5.0), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
}

TEST(TopologicalValueIterationMdpPrctlModelCheckerTest, Dice) {
	storm::settings::Settings* s = storm::settings::Settings::getInstance();
	//storm::parser::AutoParser<double> parser(STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.tra", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.lab", "", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.trans.rew");
	storm::parser::AutoParser<double> parser(STORM_CPP_BASE_PATH "/examples/mdp/scc/scc.tra", STORM_CPP_BASE_PATH "/examples/mdp/scc/scc.lab", "");

	ASSERT_EQ(parser.getType(), storm::models::MDP);
    
	std::shared_ptr<storm::models::Mdp<double>> mdp = parser.getModel<storm::models::Mdp<double>>();
    
	ASSERT_EQ(mdp->getNumberOfStates(), 11ull);
	ASSERT_EQ(mdp->getNumberOfTransitions(), 18ull);
    
	storm::modelchecker::prctl::TopologicalValueIterationMdpPrctlModelChecker<double> mc(*mdp);
    
	storm::property::prctl::Ap<double>* apFormula = new storm::property::prctl::Ap<double>("end");
	storm::property::prctl::Eventually<double>* eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	storm::property::prctl::ProbabilisticNoBoundOperator<double>* probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);
    
	std::vector<double> result = mc.checkNoBoundOperator(*probFormula);
    
	ASSERT_LT(std::abs(result[0] - 0.0277777612209320068), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    
	delete probFormula;
	/*
	apFormula = new storm::property::prctl::Ap<double>("two");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);
    
	result = mc.checkNoBoundOperator(*probFormula);
    
	ASSERT_LT(std::abs(result[0] - 0.0277777612209320068), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    
	delete probFormula;
    
	apFormula = new storm::property::prctl::Ap<double>("three");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);
    
	result = mc.checkNoBoundOperator(*probFormula);
    
	ASSERT_LT(std::abs(result[0] - 0.0555555224418640136), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    
	delete probFormula;
    
	apFormula = new storm::property::prctl::Ap<double>("three");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);
    
	result = mc.checkNoBoundOperator(*probFormula);
    
	ASSERT_LT(std::abs(result[0] - 0.0555555224418640136), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    
	delete probFormula;
    
	apFormula = new storm::property::prctl::Ap<double>("four");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);
    
	result = mc.checkNoBoundOperator(*probFormula);
    
	ASSERT_LT(std::abs(result[0] - 0.083333283662796020508), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    
	delete probFormula;
    
	apFormula = new storm::property::prctl::Ap<double>("four");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);
    
	result = mc.checkNoBoundOperator(*probFormula);
    
	ASSERT_LT(std::abs(result[0] - 0.083333283662796020508), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    
	delete probFormula;
    
	apFormula = new storm::property::prctl::Ap<double>("done");
	storm::property::prctl::ReachabilityReward<double>* reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	storm::property::prctl::RewardNoBoundOperator<double>* rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, true);
    
	result = mc.checkNoBoundOperator(*rewardFormula);
    
	ASSERT_LT(std::abs(result[0] - 7.333329499), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
	delete rewardFormula;
    
	apFormula = new storm::property::prctl::Ap<double>("done");
	reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, false);
    
	result = mc.checkNoBoundOperator(*rewardFormula);;
    
	ASSERT_LT(std::abs(result[0] - 7.333329499), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
	delete rewardFormula;
    
	storm::parser::AutoParser<double> stateRewardParser(STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.tra", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.lab", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.state.rew", "");
    
	ASSERT_EQ(stateRewardParser.getType(), storm::models::MDP);
    
	std::shared_ptr<storm::models::Mdp<double>> stateRewardMdp = stateRewardParser.getModel<storm::models::Mdp<double>>();
    
    storm::modelchecker::prctl::SparseMdpPrctlModelChecker<double> stateRewardModelChecker(*stateRewardMdp, std::shared_ptr<storm::solver::NativeNondeterministicLinearEquationSolver<double>>(new storm::solver::NativeNondeterministicLinearEquationSolver<double>()));

	apFormula = new storm::property::prctl::Ap<double>("done");
	reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, true);
    
	result = stateRewardModelChecker.checkNoBoundOperator(*rewardFormula);
    
	ASSERT_LT(std::abs(result[0] - 7.333329499), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
	delete rewardFormula;
    
	apFormula = new storm::property::prctl::Ap<double>("done");
	reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, false);
    
	result = stateRewardModelChecker.checkNoBoundOperator(*rewardFormula);
    
	ASSERT_LT(std::abs(result[0] - 7.333329499), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
	delete rewardFormula;
    
	storm::parser::AutoParser<double> stateAndTransitionRewardParser(STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.tra", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.lab", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.state.rew", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.trans.rew");
    
	ASSERT_EQ(stateAndTransitionRewardParser.getType(), storm::models::MDP);
    
	std::shared_ptr<storm::models::Mdp<double>> stateAndTransitionRewardMdp = stateAndTransitionRewardParser.getModel<storm::models::Mdp<double>>();
    
	storm::modelchecker::prctl::SparseMdpPrctlModelChecker<double> stateAndTransitionRewardModelChecker(*stateAndTransitionRewardMdp, std::shared_ptr<storm::solver::NativeNondeterministicLinearEquationSolver<double>>(new storm::solver::NativeNondeterministicLinearEquationSolver<double>()));
    
	apFormula = new storm::property::prctl::Ap<double>("done");
	reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, true);
    
	result = stateAndTransitionRewardModelChecker.checkNoBoundOperator(*rewardFormula);
    
	ASSERT_LT(std::abs(result[0] - 14.666658998), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
	delete rewardFormula;
    
	apFormula = new storm::property::prctl::Ap<double>("done");
	reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, false);
    
	result = stateAndTransitionRewardModelChecker.checkNoBoundOperator(*rewardFormula);
    
	ASSERT_LT(std::abs(result[0] - 14.666658998), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
	delete rewardFormula;*/
}

TEST(TopologicalValueIterationMdpPrctlModelCheckerTest, AsynchronousLeader) {
	/*storm::settings::Settings* s = storm::settings::Settings::getInstance();
	storm::parser::AutoParser<double> parser(STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader4.tra", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader4.lab", "", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader4.trans.rew");

	ASSERT_EQ(parser.getType(), storm::models::MDP);

	std::shared_ptr<storm::models::Mdp<double>> mdp = parser.getModel<storm::models::Mdp<double>>();

	ASSERT_EQ(mdp->getNumberOfStates(), 3172ull);
	ASSERT_EQ(mdp->getNumberOfTransitions(), 7144ull);

	storm::modelchecker::prctl::TopologicalValueIterationMdpPrctlModelChecker<double> mc(*mdp);

	storm::property::prctl::Ap<double>* apFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::Eventually<double>* eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	storm::property::prctl::ProbabilisticNoBoundOperator<double>* probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);

	std::vector<double> result = mc.checkNoBoundOperator(*probFormula);

	ASSERT_LT(std::abs(result[0] - 1), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete probFormula;

	apFormula = new storm::property::prctl::Ap<double>("elected");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);

	result = mc.checkNoBoundOperator(*probFormula);

	ASSERT_LT(std::abs(result[0] - 1), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete probFormula;

	apFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::BoundedEventually<double>* boundedEventuallyFormula = new storm::property::prctl::BoundedEventually<double>(apFormula, 25);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(boundedEventuallyFormula, false);

	result = mc.checkNoBoundOperator(*probFormula);

	ASSERT_LT(std::abs(result[0] - 0.0625), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete probFormula;

	apFormula = new storm::property::prctl::Ap<double>("elected");
	boundedEventuallyFormula = new storm::property::prctl::BoundedEventually<double>(apFormula, 25);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(boundedEventuallyFormula, true);

	result = mc.checkNoBoundOperator(*probFormula);

	ASSERT_LT(std::abs(result[0] - 0.0625), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete probFormula;

	apFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::ReachabilityReward<double>* reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	storm::property::prctl::RewardNoBoundOperator<double>* rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, true);

	result = mc.checkNoBoundOperator(*rewardFormula);;

	ASSERT_LT(std::abs(result[0] - 4.285689611), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
	delete rewardFormula;

	apFormula = new storm::property::prctl::Ap<double>("elected");
	reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, false);

	result = mc.checkNoBoundOperator(*rewardFormula);;

	ASSERT_LT(std::abs(result[0] - 4.285689611), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
	delete rewardFormula;*/
}
