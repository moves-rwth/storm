#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/parser/AutoParser.h"
#include "src/storage/StronglyConnectedComponentDecomposition.h"

TEST(StronglyConnectedComponentDecomposition, Crowds) {
    storm::parser::AutoParser<double> parser(STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds20_5.tra", STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds20_5.lab", "", "");
	std::shared_ptr<storm::models::Dtmc<double>> dtmc = parser.getModel<storm::models::Dtmc<double>>();
    
    storm::storage::StronglyConnectedComponentDecomposition<double> sccDecomposition;
    ASSERT_NO_THROW(sccDecomposition = storm::storage::StronglyConnectedComponentDecomposition<double>(*dtmc));
    
    ASSERT_EQ(sccDecomposition.size(), 1290297ull);
    dtmc = nullptr;
}

TEST(StronglyConnectedComponentDecomposition, SynchronousLeader) {
    storm::parser::AutoParser<double> parser(STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader6_8.tra", STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader6_8.lab", "", "");
    std::shared_ptr<storm::models::Dtmc<double>> dtmc = parser.getModel<storm::models::Dtmc<double>>();
    
    storm::storage::StronglyConnectedComponentDecomposition<double> sccDecomposition;
    ASSERT_NO_THROW(sccDecomposition = storm::storage::StronglyConnectedComponentDecomposition<double>(*dtmc));

    ASSERT_EQ(sccDecomposition.size(), 1279673ull);
    dtmc = nullptr;
}

TEST(StronglyConnectedComponentDecomposition, AsynchronousLeader) {
    storm::parser::AutoParser<double> parser(STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader6.tra", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader6.lab", "", "");
	std::shared_ptr<storm::models::Mdp<double>> mdp = parser.getModel<storm::models::Mdp<double>>();
    
    storm::storage::StronglyConnectedComponentDecomposition<double> sccDecomposition;
    ASSERT_NO_THROW(sccDecomposition = storm::storage::StronglyConnectedComponentDecomposition<double>(*mdp));
    
    ASSERT_EQ(sccDecomposition.size(), 146844ull);
    mdp = nullptr;
}

TEST(StronglyConnectedComponentDecomposition, Consensus) {
    storm::parser::AutoParser<double> parser(STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.tra", STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.lab", "", "");
	std::shared_ptr<storm::models::Mdp<double>> mdp = parser.getModel<storm::models::Mdp<double>>();
    
    storm::storage::StronglyConnectedComponentDecomposition<double> sccDecomposition;
    ASSERT_NO_THROW(sccDecomposition = storm::storage::StronglyConnectedComponentDecomposition<double>(*mdp));
    
    ASSERT_EQ(sccDecomposition.size(), 2611ull);
    mdp = nullptr;
}