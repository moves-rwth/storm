#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/parser/AutoParser.h"
#include "src/storage/MaximalEndComponentDecomposition.h"
#include "src/models/sparse/Mdp.h"
#include "src/models/sparse/StandardRewardModel.h"

TEST(MaximalEndComponentDecomposition, AsynchronousLeader) {
	std::shared_ptr<storm::models::sparse::Model<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.tra", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.lab", "", "");
	std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = abstractModel->as<storm::models::sparse::Mdp<double>>();
    
    storm::storage::MaximalEndComponentDecomposition<double> mecDecomposition;
    ASSERT_NO_THROW(mecDecomposition = storm::storage::MaximalEndComponentDecomposition<double>(*mdp));
    
    ASSERT_EQ(7ul, mecDecomposition.size());
    mdp = nullptr;
}

TEST(MaximalEndComponentDecomposition, Consensus) {
	std::shared_ptr<storm::models::sparse::Model<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin6_4.tra", STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin6_4.lab", "", "");
	std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = abstractModel->as<storm::models::sparse::Mdp<double>>();
    
    storm::storage::MaximalEndComponentDecomposition<double> mecDecomposition;
    ASSERT_NO_THROW(mecDecomposition = storm::storage::MaximalEndComponentDecomposition<double>(*mdp));
    
    ASSERT_EQ(384ul, mecDecomposition.size());
    mdp = nullptr;
}
