#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/utility/Settings.h"
#include "src/parser/AutoParser.h"
#include "src/utility/GraphAnalyzer.h"

TEST(GraphAnalyzerTest, PerformProb01) {
	storm::parser::AutoParser<double> parser(STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds20_5.tra", STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds20_5.lab", "", "");

	std::shared_ptr<storm::models::Dtmc<double>> dtmc = parser.getModel<storm::models::Dtmc<double>>();

    LOG4CPLUS_INFO(logger, "Computing prob01 (3 times) for crowds/crowds20_5.");
    std::pair<storm::storage::BitVector, storm::storage::BitVector> prob01 = storm::utility::GraphAnalyzer::performProb01(*dtmc, storm::storage::BitVector(dtmc->getNumberOfStates(), true), storm::storage::BitVector(dtmc->getLabeledStates("observe0Greater1")));
    
    ASSERT_EQ(prob01.first.getNumberOfSetBits(), 1724414u);
    ASSERT_EQ(prob01.second.getNumberOfSetBits(), 46046u);
    
    prob01 = storm::utility::GraphAnalyzer::performProb01(*dtmc, storm::storage::BitVector(dtmc->getNumberOfStates(), true), storm::storage::BitVector(dtmc->getLabeledStates("observeIGreater1")));

    ASSERT_EQ(prob01.first.getNumberOfSetBits(), 574016u);
    ASSERT_EQ(prob01.second.getNumberOfSetBits(), 825797u);
    
    prob01 = storm::utility::GraphAnalyzer::performProb01(*dtmc, storm::storage::BitVector(dtmc->getNumberOfStates(), true), storm::storage::BitVector(dtmc->getLabeledStates("observeOnlyTrueSender")));

    ASSERT_EQ(prob01.first.getNumberOfSetBits(), 1785309u);
    ASSERT_EQ(prob01.second.getNumberOfSetBits(), 40992u);
    LOG4CPLUS_INFO(logger, "Done computing prob01 (3 times) for crowds/crowds20_5.");
    
    storm::parser::AutoParser<double> parser2(STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader6_8.tra", STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader6_8.lab", "", STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader6_8.pick.trans.rew");
    
    std::shared_ptr<storm::models::Dtmc<double>> dtmc2 = parser2.getModel<storm::models::Dtmc<double>>();
    
    LOG4CPLUS_INFO(logger, "Computing prob01 for synchronous_leader/leader6_8");
    prob01 = storm::utility::GraphAnalyzer::performProb01(*dtmc2, storm::storage::BitVector(dtmc2->getNumberOfStates(), true), storm::storage::BitVector(dtmc2->getLabeledStates("elected")));
    LOG4CPLUS_INFO(logger, "Done computing prob01 for synchronous_leader/leader6_8");

    ASSERT_EQ(prob01.first.getNumberOfSetBits(), 0u);
    ASSERT_EQ(prob01.second.getNumberOfSetBits(), 1312334u);
}

TEST(GraphAnalyzerTest, PerformProb01MinMax) {
    storm::parser::AutoParser<double> parser(STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.tra", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.lab", "", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.trans.rew");
	std::shared_ptr<storm::models::Mdp<double>> mdp = parser.getModel<storm::models::Mdp<double>>();
    
    LOG4CPLUS_INFO(logger, "Computing prob01min for asynchronous_leader/leader7");
    std::pair<storm::storage::BitVector, storm::storage::BitVector> prob01 = storm::utility::GraphAnalyzer::performProb01Min(*mdp, storm::storage::BitVector(mdp->getNumberOfStates(), true), mdp->getLabeledStates("elected"));
    LOG4CPLUS_INFO(logger, "Done computing prob01min for asynchronous_leader/leader7");
    
    ASSERT_EQ(prob01.first.getNumberOfSetBits(), 0u);
    ASSERT_EQ(prob01.second.getNumberOfSetBits(), 2095783u);
    
    LOG4CPLUS_INFO(logger, "Computing prob01max for asynchronous_leader/leader7");
    prob01 = storm::utility::GraphAnalyzer::performProb01Max(*mdp, storm::storage::BitVector(mdp->getNumberOfStates(), true), mdp->getLabeledStates("elected"));
    LOG4CPLUS_INFO(logger, "Done computing prob01max for asynchronous_leader/leader7");

    ASSERT_EQ(prob01.first.getNumberOfSetBits(), 0u);
    ASSERT_EQ(prob01.second.getNumberOfSetBits(), 2095783u);

    storm::parser::AutoParser<double> parser2(STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.tra", STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.lab", STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.steps.state.rew", "");
	std::shared_ptr<storm::models::Mdp<double>> mdp2 = parser2.getModel<storm::models::Mdp<double>>();

    LOG4CPLUS_INFO(logger, "Computing prob01min for consensus/coin4_6");
    prob01 = storm::utility::GraphAnalyzer::performProb01Min(*mdp2, storm::storage::BitVector(mdp2->getNumberOfStates(), true), mdp2->getLabeledStates("finished"));
    LOG4CPLUS_INFO(logger, "Done computing prob01min for consensus/coin4_6");

    ASSERT_EQ(prob01.first.getNumberOfSetBits(), 0u);
    ASSERT_EQ(prob01.second.getNumberOfSetBits(), 63616u);

    LOG4CPLUS_INFO(logger, "Computing prob01max for consensus/coin4_6");
    prob01 = storm::utility::GraphAnalyzer::performProb01Max(*mdp2, storm::storage::BitVector(mdp2->getNumberOfStates(), true), mdp2->getLabeledStates("finished"));
    LOG4CPLUS_INFO(logger, "Done computing prob01max for consensus/coin4_6");

    ASSERT_EQ(prob01.first.getNumberOfSetBits(), 0u);
    ASSERT_EQ(prob01.second.getNumberOfSetBits(), 63616u);
}

TEST(GraphAnalyzerTest, PerformSCCDecomposition) {
    storm::parser::AutoParser<double> parser(STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds20_5.tra", STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds20_5.lab", "", "");
	std::shared_ptr<storm::models::Dtmc<double>> dtmc = parser.getModel<storm::models::Dtmc<double>>();
    
    LOG4CPLUS_INFO(logger, "Computing SCC decomposition of crowds/crowds20_5.");
    std::pair<std::vector<std::vector<uint_fast64_t>>, storm::storage::SparseMatrix<bool>> sccDecomposition = storm::utility::GraphAnalyzer::performSccDecomposition(*dtmc);
    LOG4CPLUS_INFO(logger, "Done computing SCC decomposition of crowds/crowds20_5.");
    
    ASSERT_EQ(sccDecomposition.first.size(), 1290297u);
    ASSERT_EQ(sccDecomposition.second.getNonZeroEntryCount(), 1371253u);
    
    storm::parser::AutoParser<double> parser2(STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader6_8.tra", STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader6_8.lab", "", STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader6_8.pick.trans.rew");
    std::shared_ptr<storm::models::Dtmc<double>> dtmc2 = parser2.getModel<storm::models::Dtmc<double>>();

    LOG4CPLUS_INFO(logger, "Computing SCC decomposition of synchronous_leader/leader6_8");
    sccDecomposition = storm::utility::GraphAnalyzer::performSccDecomposition(*dtmc2);
    LOG4CPLUS_INFO(logger, "Computing SCC decomposition of synchronous_leader/leader6_8.");

    ASSERT_EQ(sccDecomposition.first.size(), 1279673u);
    ASSERT_EQ(sccDecomposition.second.getNonZeroEntryCount(), 1535367u);
    
    storm::parser::AutoParser<double> parser3(STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.tra", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.lab", "", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.trans.rew");
	std::shared_ptr<storm::models::Mdp<double>> mdp = parser3.getModel<storm::models::Mdp<double>>();
    
    LOG4CPLUS_INFO(logger, "Computing SCC decomposition of asynchronous_leader/leader7");
    sccDecomposition = storm::utility::GraphAnalyzer::performSccDecomposition(*mdp);
    LOG4CPLUS_INFO(logger, "Done computing SCC decomposition of asynchronous_leader/leader7");

    ASSERT_EQ(sccDecomposition.first.size(), 1914691u);
    ASSERT_EQ(sccDecomposition.second.getNonZeroEntryCount(), 7023587u);
    
    storm::parser::AutoParser<double> parser4(STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.tra", STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.lab", STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.steps.state.rew", "");
	std::shared_ptr<storm::models::Mdp<double>> mdp2 = parser4.getModel<storm::models::Mdp<double>>();

    LOG4CPLUS_INFO(logger, "Computing SCC decomposition of consensus/coin4_6");
    sccDecomposition = storm::utility::GraphAnalyzer::performSccDecomposition(*mdp2);
    LOG4CPLUS_INFO(logger, "Computing SCC decomposition of consensus/coin4_6");

    ASSERT_EQ(sccDecomposition.first.size(), 63611u);
    ASSERT_EQ(sccDecomposition.second.getNonZeroEntryCount(), 213400u);
}