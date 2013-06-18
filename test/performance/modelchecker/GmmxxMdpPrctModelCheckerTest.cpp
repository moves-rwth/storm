#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/utility/Settings.h"
#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "src/solver/GmmxxNondeterministicLinearEquationSolver.h"
#include "src/parser/AutoParser.h"

TEST(GmmxxMdpPrctlModelCheckerTest, AsynchronousLeader) {
	storm::settings::Settings* s = storm::settings::instance();
	storm::parser::AutoParser<double> parser(STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.tra", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.lab", "", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.trans.rew");

	ASSERT_EQ(parser.getType(), storm::models::MDP);

	std::shared_ptr<storm::models::Mdp<double>> mdp = parser.getModel<storm::models::Mdp<double>>();

	ASSERT_EQ(mdp->getNumberOfStates(), 2095783u);
	ASSERT_EQ(mdp->getNumberOfTransitions(), 7714385u);

	storm::modelchecker::prctl::SparseMdpPrctlModelChecker<double> mc(*mdp, new storm::solver::GmmxxNondeterministicLinearEquationSolver<double>());

	storm::property::prctl::Ap<double>* apFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::Eventually<double>* eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	storm::property::prctl::ProbabilisticNoBoundOperator<double>* probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);

    LOG4CPLUS_WARN(logger, "Model Checking Pmin=? [F elected] on asynchronous_leader/leader7...");
	std::vector<double>* result = mc.checkNoBoundOperator(*probFormula);
    LOG4CPLUS_WARN(logger, "Done.");

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - 1), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::property::prctl::Ap<double>("elected");
	eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);

    LOG4CPLUS_WARN(logger, "Model Checking Pmax=? [F elected] on asynchronous_leader/leader7...");
	result = mc.checkNoBoundOperator(*probFormula);
    LOG4CPLUS_WARN(logger, "Done.");

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - 1), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::BoundedEventually<double>* boundedEventuallyFormula = new storm::property::prctl::BoundedEventually<double>(apFormula, 25);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(boundedEventuallyFormula, true);

    LOG4CPLUS_WARN(logger, "Model Checking Pmin=? [F<=25 elected] on asynchronous_leader/leader7...");
	result = mc.checkNoBoundOperator(*probFormula);
    LOG4CPLUS_WARN(logger, "Done.");

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - 0), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::property::prctl::Ap<double>("elected");
	boundedEventuallyFormula = new storm::property::prctl::BoundedEventually<double>(apFormula, 25);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(boundedEventuallyFormula, false);

    LOG4CPLUS_WARN(logger, "Model Checking Pmax=? [F<=25 elected] on asynchronous_leader/leader7...");
	result = mc.checkNoBoundOperator(*probFormula);
    LOG4CPLUS_WARN(logger, "Done.");

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - 0), s->get<double>("precision"));

	delete probFormula;
	delete result;

	apFormula = new storm::property::prctl::Ap<double>("elected");
	storm::property::prctl::ReachabilityReward<double>* reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	storm::property::prctl::RewardNoBoundOperator<double>* rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, true);

    LOG4CPLUS_WARN(logger, "Model Checking Rmin=? [F elected] on asynchronous_leader/leader7...");
	result = mc.checkNoBoundOperator(*rewardFormula);
    LOG4CPLUS_WARN(logger, "Done.");

	ASSERT_LT(std::abs((*result)[0] - 6.172433512), s->get<double>("precision"));

	delete rewardFormula;
	delete result;

	apFormula = new storm::property::prctl::Ap<double>("elected");
	reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, false);

    LOG4CPLUS_WARN(logger, "Model Checking Rmax=? [F elected] on asynchronous_leader/leader7...");
	result = mc.checkNoBoundOperator(*rewardFormula);
    LOG4CPLUS_WARN(logger, "Done");

	ASSERT_NE(nullptr, result);

	ASSERT_LT(std::abs((*result)[0] - 6.1724344), s->get<double>("precision"));

	delete rewardFormula;
	delete result;
}

TEST(GmmxxMdpPrctlModelCheckerTest, Consensus) {
	storm::settings::Settings* s = storm::settings::instance();
    s->set<unsigned>("maxiter", 20000);
    
	storm::parser::AutoParser<double> parser(STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.tra", STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.lab", STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.steps.state.rew", "");
    
	ASSERT_EQ(parser.getType(), storm::models::MDP);
    
	std::shared_ptr<storm::models::Mdp<double>> mdp = parser.getModel<storm::models::Mdp<double>>();
    
	ASSERT_EQ(mdp->getNumberOfStates(), 63616u);
	ASSERT_EQ(mdp->getNumberOfTransitions(), 213472u);
    
	storm::modelchecker::prctl::SparseMdpPrctlModelChecker<double> mc(*mdp, new storm::solver::GmmxxNondeterministicLinearEquationSolver<double>());
    
    storm::property::prctl::Ap<double>* apFormula = new storm::property::prctl::Ap<double>("finished");
	storm::property::prctl::Eventually<double>* eventuallyFormula = new storm::property::prctl::Eventually<double>(apFormula);
	storm::property::prctl::ProbabilisticNoBoundOperator<double>* probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);
    
    LOG4CPLUS_WARN(logger, "Model Checking Pmin=? [F finished] on consensus/coin4_6...");
	std::vector<double>* result = mc.checkNoBoundOperator(*probFormula);
    LOG4CPLUS_WARN(logger, "Done.");
    
	ASSERT_NE(nullptr, result);
    
	ASSERT_LT(std::abs((*result)[31168] - 1), s->get<double>("precision"));
    
    delete probFormula;
    delete result;
    
    apFormula = new storm::property::prctl::Ap<double>("finished");
    storm::property::prctl::Ap<double>* apFormula2 = new storm::property::prctl::Ap<double>("all_coins_equal_0");
    storm::property::prctl::And<double>* andFormula = new storm::property::prctl::And<double>(apFormula, apFormula2);
	eventuallyFormula = new storm::property::prctl::Eventually<double>(andFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, true);
    
    LOG4CPLUS_WARN(logger, "Model Checking Pmin=? [F finished & all_coins_equal_0] on consensus/coin4_6...");
	result = mc.checkNoBoundOperator(*probFormula);
    LOG4CPLUS_WARN(logger, "Done.");
    
	ASSERT_NE(nullptr, result);
    
	ASSERT_LT(std::abs((*result)[31168] - 0.43742828319177884388579), s->get<double>("precision"));
    
    delete probFormula;
    delete result;
    
    apFormula = new storm::property::prctl::Ap<double>("finished");
    apFormula2 = new storm::property::prctl::Ap<double>("all_coins_equal_1");
    andFormula = new storm::property::prctl::And<double>(apFormula, apFormula2);
    eventuallyFormula = new storm::property::prctl::Eventually<double>(andFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);
    
    LOG4CPLUS_WARN(logger, "Model Checking Pmax=? [F finished & all_coins_equal_1] on consensus/coin4_6...");
	result = mc.checkNoBoundOperator(*probFormula);
    LOG4CPLUS_WARN(logger, "Done.");
    
	ASSERT_NE(nullptr, result);    
	ASSERT_LT(std::abs((*result)[31168] - 0.52932863686144482340267813924583606421947479248047), s->get<double>("precision"));
    
    delete probFormula;
    delete result;
    
    apFormula = new storm::property::prctl::Ap<double>("finished");
    apFormula2 = new storm::property::prctl::Ap<double>("agree");
    storm::property::prctl::Not<double>* notFormula = new storm::property::prctl::Not<double>(apFormula2);
    andFormula = new storm::property::prctl::And<double>(apFormula, notFormula);
    eventuallyFormula = new storm::property::prctl::Eventually<double>(andFormula);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(eventuallyFormula, false);
    
    LOG4CPLUS_WARN(logger, "Model Checking Pmax=? [F finished & !agree] on consensus/coin4_6...");
	result = mc.checkNoBoundOperator(*probFormula);
    LOG4CPLUS_WARN(logger, "Done.");
    
	ASSERT_NE(nullptr, result);
	ASSERT_LT(std::abs((*result)[31168] - 0.1041409700076474653673841), s->get<double>("precision"));
    
    delete probFormula;
    delete result;
    
    apFormula = new storm::property::prctl::Ap<double>("finished");
	storm::property::prctl::BoundedEventually<double>* boundedEventuallyFormula = new storm::property::prctl::BoundedEventually<double>(apFormula, 50);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(boundedEventuallyFormula, true);
    
    LOG4CPLUS_WARN(logger, "Model Checking Pmin=? [F<=50 finished] on consensus/coin4_6...");
	result = mc.checkNoBoundOperator(*probFormula);
    LOG4CPLUS_WARN(logger, "Done.");
    
	ASSERT_NE(nullptr, result);
	ASSERT_LT(std::abs((*result)[31168] - 0), s->get<double>("precision"));
    
    delete probFormula;
    delete result;
    
    apFormula = new storm::property::prctl::Ap<double>("finished");
	boundedEventuallyFormula = new storm::property::prctl::BoundedEventually<double>(apFormula, 50);
	probFormula = new storm::property::prctl::ProbabilisticNoBoundOperator<double>(boundedEventuallyFormula, false);
    
    LOG4CPLUS_WARN(logger, "Model Checking Pmax=? [F<=50 finished] on consensus/coin4_6...");
	result = mc.checkNoBoundOperator(*probFormula);
    LOG4CPLUS_WARN(logger, "Done.");
    
	ASSERT_NE(nullptr, result);
	ASSERT_LT(std::abs((*result)[31168] - 0), s->get<double>("precision"));
    
    delete probFormula;
    delete result;
    
    apFormula = new storm::property::prctl::Ap<double>("finished");
	storm::property::prctl::ReachabilityReward<double>* reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	storm::property::prctl::RewardNoBoundOperator<double>* rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, true);
    
    LOG4CPLUS_WARN(logger, "Model Checking Rmin=? [F finished] on consensus/coin4_6...");
	result = mc.checkNoBoundOperator(*rewardFormula);
    LOG4CPLUS_WARN(logger, "Done.");
    
    ASSERT_NE(nullptr, result);
	ASSERT_LT(std::abs((*result)[31168] - 1725.5933133943854045), s->get<double>("precision"));
    
	delete rewardFormula;
	delete result;
    
	apFormula = new storm::property::prctl::Ap<double>("finished");
	reachabilityRewardFormula = new storm::property::prctl::ReachabilityReward<double>(apFormula);
	rewardFormula = new storm::property::prctl::RewardNoBoundOperator<double>(reachabilityRewardFormula, false);
    
    LOG4CPLUS_WARN(logger, "Model Checking Rmax=? [F finished] on consensus/coin4_6...");
	result = mc.checkNoBoundOperator(*rewardFormula);
    LOG4CPLUS_WARN(logger, "Done.");
    
	ASSERT_NE(nullptr, result);
	ASSERT_LT(std::abs((*result)[31168] - 2183.1424220082612919213715), s->get<double>("precision"));
    
	delete rewardFormula;
	delete result;

}