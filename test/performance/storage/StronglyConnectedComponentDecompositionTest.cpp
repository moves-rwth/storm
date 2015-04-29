#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/parser/AutoParser.h"
#include "src/storage/StronglyConnectedComponentDecomposition.h"
#include "src/models/sparse/Mdp.h"
#include "src/models/sparse/Dtmc.h"

TEST(StronglyConnectedComponentDecomposition, Crowds) {
	std::shared_ptr<storm::models::sparse::Model<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds20_5.tra", STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds20_5.lab", "", "");
	std::shared_ptr<storm::models::sparse::Dtmc<double>> dtmc = abstractModel->as<storm::models::sparse::Dtmc<double>>();
    
    storm::storage::StronglyConnectedComponentDecomposition<double> sccDecomposition;

    ASSERT_NO_THROW(sccDecomposition = storm::storage::StronglyConnectedComponentDecomposition<double>(*dtmc));
    ASSERT_EQ(1290297ull, sccDecomposition.size());

    ASSERT_NO_THROW(sccDecomposition = storm::storage::StronglyConnectedComponentDecomposition<double>(*dtmc, true));
    ASSERT_EQ(437690, sccDecomposition.size());

    ASSERT_NO_THROW(sccDecomposition = storm::storage::StronglyConnectedComponentDecomposition<double>(*dtmc, true, true));
    ASSERT_EQ(425040, sccDecomposition.size());

    dtmc = nullptr;
}

TEST(StronglyConnectedComponentDecomposition, SynchronousLeader) {
	std::shared_ptr<storm::models::sparse::Model<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader6_9.tra", STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader6_9.lab", "", "");
    std::shared_ptr<storm::models::sparse::Dtmc<double>> dtmc = abstractModel->as<storm::models::sparse::Dtmc<double>>();
    
    storm::storage::StronglyConnectedComponentDecomposition<double> sccDecomposition;

    ASSERT_NO_THROW(sccDecomposition = storm::storage::StronglyConnectedComponentDecomposition<double>(*dtmc));
    ASSERT_EQ(2611835, sccDecomposition.size());

    ASSERT_NO_THROW(sccDecomposition = storm::storage::StronglyConnectedComponentDecomposition<double>(*dtmc, true));
    ASSERT_EQ(2, sccDecomposition.size());

    ASSERT_NO_THROW(sccDecomposition = storm::storage::StronglyConnectedComponentDecomposition<double>(*dtmc, true, true));
    ASSERT_EQ(1, sccDecomposition.size());

    dtmc = nullptr;
}

TEST(StronglyConnectedComponentDecomposition, AsynchronousLeader) {
	std::shared_ptr<storm::models::sparse::Model<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.tra", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.lab", "", "");
	std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = abstractModel->as<storm::models::sparse::Mdp<double>>();
    
    storm::storage::StronglyConnectedComponentDecomposition<double> sccDecomposition;
    
    ASSERT_NO_THROW(sccDecomposition = storm::storage::StronglyConnectedComponentDecomposition<double>(*mdp));
    ASSERT_EQ(1461930, sccDecomposition.size());

    ASSERT_NO_THROW(sccDecomposition = storm::storage::StronglyConnectedComponentDecomposition<double>(*mdp, true));
    ASSERT_EQ(127, sccDecomposition.size());

    ASSERT_NO_THROW(sccDecomposition = storm::storage::StronglyConnectedComponentDecomposition<double>(*mdp, true, true));
    ASSERT_EQ(7, sccDecomposition.size());

    mdp = nullptr;
}

TEST(StronglyConnectedComponentDecomposition, Consensus) {
	std::shared_ptr<storm::models::sparse::Model<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin6_4.tra", STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin6_4.lab", "", "");
	std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = abstractModel->as<storm::models::sparse::Mdp<double>>();
    
    storm::storage::StronglyConnectedComponentDecomposition<double> sccDecomposition;
    
    ASSERT_NO_THROW(sccDecomposition = storm::storage::StronglyConnectedComponentDecomposition<double>(*mdp));
    ASSERT_EQ(121251, sccDecomposition.size());

    ASSERT_NO_THROW(sccDecomposition = storm::storage::StronglyConnectedComponentDecomposition<double>(*mdp, true));
    ASSERT_EQ(1049, sccDecomposition.size());

    ASSERT_NO_THROW(sccDecomposition = storm::storage::StronglyConnectedComponentDecomposition<double>(*mdp, true, true));
    ASSERT_EQ(384, sccDecomposition.size());

    mdp = nullptr;
}
