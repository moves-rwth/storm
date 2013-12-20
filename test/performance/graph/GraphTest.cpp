#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/settings/Settings.h"
#include "src/parser/AutoParser.h"
#include "src/utility/graph.h"
#include "src/storage/StronglyConnectedComponentDecomposition.h"

TEST(GraphTest, PerformProb01) {
	storm::parser::AutoParser<double> parser(STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds20_5.tra", STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds20_5.lab", "", "");

	std::shared_ptr<storm::models::Dtmc<double>> dtmc = parser.getModel<storm::models::Dtmc<double>>();
	storm::storage::BitVector trueStates(dtmc->getNumberOfStates(), true);

    LOG4CPLUS_WARN(logger, "Computing prob01 (3 times) for crowds/crowds20_5...");
    
    std::pair<storm::storage::BitVector, storm::storage::BitVector> prob01(storm::utility::graph::performProb01(*dtmc, trueStates, storm::storage::BitVector(dtmc->getLabeledStates("observe0Greater1"))));
    
    ASSERT_EQ(prob01.first.getNumberOfSetBits(), 1724414ull);
    ASSERT_EQ(prob01.second.getNumberOfSetBits(), 46046ull);
    
    prob01 = storm::utility::graph::performProb01(*dtmc, trueStates, storm::storage::BitVector(dtmc->getLabeledStates("observeIGreater1")));

    ASSERT_EQ(prob01.first.getNumberOfSetBits(), 574016ull);
    ASSERT_EQ(prob01.second.getNumberOfSetBits(), 825797ull);
        
    prob01 = storm::utility::graph::performProb01(*dtmc, trueStates, storm::storage::BitVector(dtmc->getLabeledStates("observeOnlyTrueSender")));

    ASSERT_EQ(prob01.first.getNumberOfSetBits(), 1785309ull);
    ASSERT_EQ(prob01.second.getNumberOfSetBits(), 40992ull);
    LOG4CPLUS_WARN(logger, "Done.");
    
    dtmc = nullptr;
    
    storm::parser::AutoParser<double> parser2(STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader6_8.tra", STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader6_8.lab", "", "");
    
    std::shared_ptr<storm::models::Dtmc<double>> dtmc2 = parser2.getModel<storm::models::Dtmc<double>>();
    trueStates = storm::storage::BitVector(dtmc2->getNumberOfStates(), true);

    LOG4CPLUS_WARN(logger, "Computing prob01 for synchronous_leader/leader6_8...");
    prob01 = storm::utility::graph::performProb01(*dtmc2, trueStates, storm::storage::BitVector(dtmc2->getLabeledStates("elected")));
    LOG4CPLUS_WARN(logger, "Done.");

    ASSERT_EQ(prob01.first.getNumberOfSetBits(), 0ull);
    ASSERT_EQ(prob01.second.getNumberOfSetBits(), 1312334ull);
    
    dtmc2 = nullptr;
}

TEST(GraphTest, PerformProb01MinMax) {
    storm::parser::AutoParser<double> parser(STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.tra", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.lab", "", "");
	std::shared_ptr<storm::models::Mdp<double>> mdp = parser.getModel<storm::models::Mdp<double>>();
	storm::storage::BitVector trueStates(mdp->getNumberOfStates(), true);
    
    LOG4CPLUS_WARN(logger, "Computing prob01min for asynchronous_leader/leader7...");
    std::pair<storm::storage::BitVector, storm::storage::BitVector> prob01(storm::utility::graph::performProb01Min(*mdp, trueStates, mdp->getLabeledStates("elected")));
    LOG4CPLUS_WARN(logger, "Done.");
    
    ASSERT_EQ(prob01.first.getNumberOfSetBits(), 0ull);
    ASSERT_EQ(prob01.second.getNumberOfSetBits(), 2095783ull);
    
    LOG4CPLUS_WARN(logger, "Computing prob01max for asynchronous_leader/leader7...");
    prob01 = storm::utility::graph::performProb01Max(*mdp, trueStates, mdp->getLabeledStates("elected"));
    LOG4CPLUS_WARN(logger, "Done.");

    ASSERT_EQ(prob01.first.getNumberOfSetBits(), 0ull);
    ASSERT_EQ(prob01.second.getNumberOfSetBits(), 2095783ull);
    
    mdp = nullptr;

    storm::parser::AutoParser<double> parser2(STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.tra", STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.lab", "", "");
	std::shared_ptr<storm::models::Mdp<double>> mdp2 = parser2.getModel<storm::models::Mdp<double>>();
	trueStates = storm::storage::BitVector(mdp2->getNumberOfStates(), true);

    LOG4CPLUS_WARN(logger, "Computing prob01min for consensus/coin4_6...");
	prob01 = storm::utility::graph::performProb01Min(*mdp2, trueStates, mdp2->getLabeledStates("finished"));
    LOG4CPLUS_WARN(logger, "Done.");

    ASSERT_EQ(prob01.first.getNumberOfSetBits(), 0ull);
    ASSERT_EQ(prob01.second.getNumberOfSetBits(), 63616ull);

    LOG4CPLUS_WARN(logger, "Computing prob01max for consensus/coin4_6...");
    prob01 = storm::utility::graph::performProb01Max(*mdp2, trueStates, mdp2->getLabeledStates("finished"));
    LOG4CPLUS_WARN(logger, "Done.");

    ASSERT_EQ(prob01.first.getNumberOfSetBits(), 0ull);
    ASSERT_EQ(prob01.second.getNumberOfSetBits(), 63616ull);
    
    mdp2 = nullptr;
}