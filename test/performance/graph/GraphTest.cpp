#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/utility/Settings.h"
#include "src/parser/AutoParser.h"
#include "src/utility/graph.h"

TEST(GraphTest, PerformProb01) {
	storm::parser::AutoParser<double> parser(STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds20_5.tra", STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds20_5.lab", "", "");

	std::shared_ptr<storm::models::Dtmc<double>> dtmc = parser.getModel<storm::models::Dtmc<double>>();

    LOG4CPLUS_WARN(logger, "Computing prob01 (3 times) for crowds/crowds20_5...");
    std::pair<storm::storage::BitVector, storm::storage::BitVector> prob01 = storm::utility::graph::performProb01(*dtmc, storm::storage::BitVector(dtmc->getNumberOfStates(), true), storm::storage::BitVector(dtmc->getLabeledStates("observe0Greater1")));
    
    ASSERT_EQ(prob01.first.getNumberOfSetBits(), 1724414u);
    ASSERT_EQ(prob01.second.getNumberOfSetBits(), 46046u);
    
    prob01 = storm::utility::graph::performProb01(*dtmc, storm::storage::BitVector(dtmc->getNumberOfStates(), true), storm::storage::BitVector(dtmc->getLabeledStates("observeIGreater1")));

    ASSERT_EQ(prob01.first.getNumberOfSetBits(), 574016u);
    ASSERT_EQ(prob01.second.getNumberOfSetBits(), 825797u);
    
    prob01 = storm::utility::graph::performProb01(*dtmc, storm::storage::BitVector(dtmc->getNumberOfStates(), true), storm::storage::BitVector(dtmc->getLabeledStates("observeOnlyTrueSender")));

    ASSERT_EQ(prob01.first.getNumberOfSetBits(), 1785309u);
    ASSERT_EQ(prob01.second.getNumberOfSetBits(), 40992u);
    LOG4CPLUS_WARN(logger, "Done.");
    
    dtmc = nullptr;
    
    storm::parser::AutoParser<double> parser2(STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader6_8.tra", STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader6_8.lab", "", "");
    
    std::shared_ptr<storm::models::Dtmc<double>> dtmc2 = parser2.getModel<storm::models::Dtmc<double>>();
    
    LOG4CPLUS_WARN(logger, "Computing prob01 for synchronous_leader/leader6_8...");
    prob01 = storm::utility::graph::performProb01(*dtmc2, storm::storage::BitVector(dtmc2->getNumberOfStates(), true), storm::storage::BitVector(dtmc2->getLabeledStates("elected")));
    LOG4CPLUS_WARN(logger, "Done.");

    ASSERT_EQ(prob01.first.getNumberOfSetBits(), 0u);
    ASSERT_EQ(prob01.second.getNumberOfSetBits(), 1312334u);
    
    dtmc2 = nullptr;
}

TEST(GraphTest, PerformProb01MinMax) {
    storm::parser::AutoParser<double> parser(STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.tra", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.lab", "", "");
	std::shared_ptr<storm::models::Mdp<double>> mdp = parser.getModel<storm::models::Mdp<double>>();
    
    LOG4CPLUS_WARN(logger, "Computing prob01min for asynchronous_leader/leader7...");
    std::pair<storm::storage::BitVector, storm::storage::BitVector> prob01 = storm::utility::graph::performProb01Min(*mdp, storm::storage::BitVector(mdp->getNumberOfStates(), true), mdp->getLabeledStates("elected"));
    LOG4CPLUS_WARN(logger, "Done.");
    
    ASSERT_EQ(prob01.first.getNumberOfSetBits(), 0u);
    ASSERT_EQ(prob01.second.getNumberOfSetBits(), 2095783u);
    
    LOG4CPLUS_WARN(logger, "Computing prob01max for asynchronous_leader/leader7...");
    prob01 = storm::utility::graph::performProb01Max(*mdp, storm::storage::BitVector(mdp->getNumberOfStates(), true), mdp->getLabeledStates("elected"));
    LOG4CPLUS_WARN(logger, "Done.");

    ASSERT_EQ(prob01.first.getNumberOfSetBits(), 0u);
    ASSERT_EQ(prob01.second.getNumberOfSetBits(), 2095783u);
    
    mdp = nullptr;

    storm::parser::AutoParser<double> parser2(STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.tra", STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.lab", "", "");
	std::shared_ptr<storm::models::Mdp<double>> mdp2 = parser2.getModel<storm::models::Mdp<double>>();

    LOG4CPLUS_WARN(logger, "Computing prob01min for consensus/coin4_6...");
    prob01 = storm::utility::graph::performProb01Min(*mdp2, storm::storage::BitVector(mdp2->getNumberOfStates(), true), mdp2->getLabeledStates("finished"));
    LOG4CPLUS_WARN(logger, "Done.");

    ASSERT_EQ(prob01.first.getNumberOfSetBits(), 0u);
    ASSERT_EQ(prob01.second.getNumberOfSetBits(), 63616u);

    LOG4CPLUS_WARN(logger, "Computing prob01max for consensus/coin4_6...");
    prob01 = storm::utility::graph::performProb01Max(*mdp2, storm::storage::BitVector(mdp2->getNumberOfStates(), true), mdp2->getLabeledStates("finished"));
    LOG4CPLUS_WARN(logger, "Done.");

    ASSERT_EQ(prob01.first.getNumberOfSetBits(), 0u);
    ASSERT_EQ(prob01.second.getNumberOfSetBits(), 63616u);
    
    mdp2 = nullptr;
}

TEST(GraphTest, PerformSCCDecompositionAndGetDependencyGraph) {
    storm::parser::AutoParser<double> parser(STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds20_5.tra", STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds20_5.lab", "", "");
	std::shared_ptr<storm::models::Dtmc<double>> dtmc = parser.getModel<storm::models::Dtmc<double>>();
    
    LOG4CPLUS_WARN(logger, "Computing SCC decomposition of crowds/crowds20_5...");
    std::vector<std::vector<uint_fast64_t>> sccDecomposition = storm::utility::graph::performSccDecomposition(*dtmc);
    LOG4CPLUS_WARN(logger, "Done.");
    
    ASSERT_EQ(sccDecomposition.size(), 1290297u);
    
    LOG4CPLUS_WARN(logger, "Extracting SCC dependency graph of crowds/crowds20_5...");
    storm::storage::SparseMatrix<bool> sccDependencyGraph = dtmc->extractPartitionDependencyGraph(sccDecomposition);
    LOG4CPLUS_WARN(logger, "Done.");
    
    ASSERT_EQ(sccDependencyGraph.getNonZeroEntryCount(), 1371253u);
    
    dtmc = nullptr;
    
    storm::parser::AutoParser<double> parser2(STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader6_8.tra", STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader6_8.lab", "", "");
    std::shared_ptr<storm::models::Dtmc<double>> dtmc2 = parser2.getModel<storm::models::Dtmc<double>>();

    LOG4CPLUS_WARN(logger, "Computing SCC decomposition of synchronous_leader/leader6_8...");
    sccDecomposition = std::move(storm::utility::graph::performSccDecomposition(*dtmc2));
    LOG4CPLUS_WARN(logger, "Done.");

    ASSERT_EQ(sccDecomposition.size(), 1279673u);
    
    LOG4CPLUS_WARN(logger, "Extracting SCC dependency graph of synchronous_leader/leader6_8...");
    sccDependencyGraph = std::move(dtmc2->extractPartitionDependencyGraph(sccDecomposition));
    LOG4CPLUS_WARN(logger, "Done.");
    
    ASSERT_EQ(sccDependencyGraph.getNonZeroEntryCount(), 1535367u);
    
    dtmc2 = nullptr;
    
    storm::parser::AutoParser<double> parser3(STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader6.tra", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader6.lab", "", "");
	std::shared_ptr<storm::models::Mdp<double>> mdp = parser3.getModel<storm::models::Mdp<double>>();
    
    LOG4CPLUS_WARN(logger, "Computing SCC decomposition of asynchronous_leader/leader6...");
    sccDecomposition = std::move(storm::utility::graph::performSccDecomposition(*mdp));
    LOG4CPLUS_WARN(logger, "Done.");

    ASSERT_EQ(sccDecomposition.size(), 214675);
    
    LOG4CPLUS_WARN(logger, "Extracting SCC dependency graph of asynchronous_leader/leader6...");
    sccDependencyGraph = std::move(mdp->extractPartitionDependencyGraph(sccDecomposition));
    LOG4CPLUS_WARN(logger, "Done.");
    
    ASSERT_EQ(sccDependencyGraph.getNonZeroEntryCount(), 684093u);
    
    mdp = nullptr;
    
    storm::parser::AutoParser<double> parser4(STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.tra", STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.lab", "", "");
	std::shared_ptr<storm::models::Mdp<double>> mdp2 = parser4.getModel<storm::models::Mdp<double>>();

    LOG4CPLUS_WARN(logger, "Computing SCC decomposition of consensus/coin4_6...");
    sccDecomposition = std::move(storm::utility::graph::performSccDecomposition(*mdp2));
    LOG4CPLUS_WARN(logger, "Done.");

    ASSERT_EQ(sccDecomposition.size(), 63611u);
    
    LOG4CPLUS_WARN(logger, "Extracting SCC dependency graph of consensus/coin4_6...");
    sccDependencyGraph = std::move(mdp2->extractPartitionDependencyGraph(sccDecomposition));
    LOG4CPLUS_WARN(logger, "Done.");
    
    ASSERT_EQ(sccDependencyGraph.getNonZeroEntryCount(), 213400u);

    mdp2 = nullptr;
}