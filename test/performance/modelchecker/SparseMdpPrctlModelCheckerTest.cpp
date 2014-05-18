#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/settings/Settings.h"
#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "src/solver/NativeNondeterministicLinearEquationSolver.h"
#include "src/parser/AutoParser.h"

TEST(SparseMdpPrctlModelCheckerTest, AsynchronousLeader) {
	storm::settings::Settings* s = storm::settings::Settings::getInstance();
	std::shared_ptr<storm::models::AbstractModel<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.tra", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.lab", "", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.trans.rew");

	ASSERT_EQ(abstractModel->getType(), storm::models::MDP);

	std::shared_ptr<storm::models::Mdp<double>> mdp = abstractModel->as<storm::models::Mdp<double>>();

	ASSERT_EQ(2095783ull, mdp->getNumberOfStates());
	ASSERT_EQ(7714385ull, mdp->getNumberOfTransitions());

	storm::modelchecker::prctl::SparseMdpPrctlModelChecker<double> mc(*mdp, std::shared_ptr<storm::solver::NativeNondeterministicLinearEquationSolver<double>>(new storm::solver::NativeNondeterministicLinearEquationSolver<double>()));

	storm::property::prctl::Ap<double>* apFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::Eventually<double>* eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);

	std::vector<double> result = mc.checkMinMaxOperator(*eventuallyFormula, true);

	ASSERT_LT(std::abs(result[0] - 1.0), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	result = mc.checkMinMaxOperator(*eventuallyFormula, false);

	ASSERT_LT(std::abs(result[0] - 1.0), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete eventuallyFormula;

	apFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::BoundedEventually<double>* boundedEventuallyFormula = new storm::property::prctl::BoundedEventually<double>(apFormula, 25);

	result = mc.checkMinMaxOperator(*boundedEventuallyFormula, true);

	ASSERT_LT(std::abs(result[0] - 0.0), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	result = mc.checkMinMaxOperator(*boundedEventuallyFormula, false);

	ASSERT_LT(std::abs(result[0] - 0.0), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete boundedEventuallyFormula;

	apFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::ReachabilityReward<double>* reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);

	result = mc.checkMinMaxOperator(*reachabilityRewardFormula, true);

	ASSERT_LT(std::abs(result[0] - 6.172433512), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	result = mc.checkMinMaxOperator(*reachabilityRewardFormula, false);

	ASSERT_LT(std::abs(result[0] - 6.1724344), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete reachabilityRewardFormula;
}

TEST(SparseMdpPrctlModelCheckerTest, Consensus) {
	storm::settings::Settings* s = storm::settings::Settings::getInstance();
    // Increase the maximal number of iterations, because the solver does not converge otherwise.
	// This is done in the main cpp unit
    
	std::shared_ptr<storm::models::AbstractModel<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.tra", STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.lab", STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.steps.state.rew", "");
    
	ASSERT_EQ(abstractModel->getType(), storm::models::MDP);
    
	std::shared_ptr<storm::models::Mdp<double>> mdp = abstractModel->as<storm::models::Mdp<double>>();
    
	ASSERT_EQ(63616ull, mdp->getNumberOfStates());
	ASSERT_EQ(213472ull, mdp->getNumberOfTransitions());
    
	storm::modelchecker::prctl::SparseMdpPrctlModelChecker<double> mc(*mdp, std::shared_ptr<storm::solver::NativeNondeterministicLinearEquationSolver<double>>(new storm::solver::NativeNondeterministicLinearEquationSolver<double>()));
    
    storm::property::prctl::Ap<double>* apFormula = new storm::property::prctl::Ap<double>("finished");
	storm::property::prctl::Eventually<double>* eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
    
	std::vector<double> result = mc.checkMinMaxOperator(*eventuallyFormula, true);
    
	ASSERT_LT(std::abs(result[31168] - 1.0), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    
	delete eventuallyFormula;

    apFormula = new storm::property::prctl::Ap<double>("finished");
    storm::property::prctl::Ap<double>* apFormula2 = new storm::property::prctl::Ap<double>("all_coins_equal_0");
    storm::property::prctl::And<double>* andFormula = new storm::property::prctl::And<double>(apFormula, apFormula2);
	eventuallyFormula = new storm::property::prctl::Eventually<double>(andFormula);
    
	result = mc.checkMinMaxOperator(*eventuallyFormula, true);

	ASSERT_LT(std::abs(result[31168] - 0.4374282832), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

    delete eventuallyFormula;
    
    apFormula = new storm::property::prctl::Ap<double>("finished");
    apFormula2 = new storm::property::prctl::Ap<double>("all_coins_equal_1");
    andFormula = new storm::property::prctl::And<double>(apFormula, apFormula2);
    eventuallyFormula = new storm::property::prctl::Eventually<double>(andFormula);
    
	result = mc.checkMinMaxOperator(*eventuallyFormula, false);
    
	ASSERT_LT(std::abs(result[31168] - 0.5293286369), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    
	delete eventuallyFormula;

    apFormula = new storm::property::prctl::Ap<double>("finished");
    apFormula2 = new storm::property::prctl::Ap<double>("agree");
    storm::property::prctl::Not<double>* notFormula = new storm::property::prctl::Not<double>(apFormula2);
    andFormula = new storm::property::prctl::And<double>(apFormula, notFormula);
    eventuallyFormula = new storm::property::prctl::Eventually<double>(andFormula);
    
	result = mc.checkMinMaxOperator(*eventuallyFormula, false);
    
	ASSERT_LT(std::abs(result[31168] - 0.10414097), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    
	delete eventuallyFormula;

    apFormula = new storm::property::prctl::Ap<double>("finished");
	storm::property::prctl::BoundedEventually<double>* boundedEventuallyFormula = new storm::property::prctl::BoundedEventually<double>(apFormula, 50ull);
    
	result = mc.checkMinMaxOperator(*boundedEventuallyFormula, true);
    
	ASSERT_LT(std::abs(result[31168] - 0.0), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
    
	result = mc.checkMinMaxOperator(*boundedEventuallyFormula, false);

	ASSERT_LT(std::abs(result[31168] - 0.0), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete boundedEventuallyFormula;

    apFormula = new storm::property::prctl::Ap<double>("finished");
	storm::property::prctl::ReachabilityReward<double>* reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
    
	result = mc.checkMinMaxOperator(*reachabilityRewardFormula, true);
    
	ASSERT_LT(std::abs(result[31168] - 1725.593313), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	result = mc.checkMinMaxOperator(*reachabilityRewardFormula, false);

	ASSERT_LT(std::abs(result[31168] - 2183.142422), s->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

	delete reachabilityRewardFormula;
}
