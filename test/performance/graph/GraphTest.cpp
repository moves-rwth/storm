#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/settings/SettingsManager.h"
#include "src/parser/AutoParser.h"
#include "src/utility/graph.h"
#include "src/storage/StronglyConnectedComponentDecomposition.h"
#include "src/models/sparse/Mdp.h"
#include "src/models/sparse/Dtmc.h"
#include "src/models/sparse/StandardRewardModel.h"

TEST(GraphTest, ExplicitProb01) {
	std::shared_ptr<storm::models::sparse::Model<double>> abstractModel = storm::parser::AutoParser<>::parseModel(STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds20_5.tra", STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds20_5.lab", "", "");

    std::shared_ptr<storm::models::sparse::Dtmc<double>> dtmc = abstractModel->as<storm::models::sparse::Dtmc<double>>();
	storm::storage::BitVector trueStates(dtmc->getNumberOfStates(), true);
    
    std::pair<storm::storage::BitVector, storm::storage::BitVector> prob01(storm::utility::graph::performProb01(*dtmc, trueStates, dtmc->getStates("observe0Greater1")));
    
    ASSERT_EQ(prob01.first.getNumberOfSetBits(), 1724414ull);
    ASSERT_EQ(prob01.second.getNumberOfSetBits(), 46046ull);
    
    prob01 = storm::utility::graph::performProb01(*dtmc, trueStates, dtmc->getStates("observeIGreater1"));

    ASSERT_EQ(prob01.first.getNumberOfSetBits(), 574016ull);
    ASSERT_EQ(prob01.second.getNumberOfSetBits(), 825797ull);
        
    prob01 = storm::utility::graph::performProb01(*dtmc, trueStates, dtmc->getStates("observeOnlyTrueSender"));

    ASSERT_EQ(prob01.first.getNumberOfSetBits(), 1785309ull);
    ASSERT_EQ(prob01.second.getNumberOfSetBits(), 40992ull);
    
    dtmc = nullptr;
    
    abstractModel = storm::parser::AutoParser<>::parseModel(STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader6_8.tra", STORM_CPP_BASE_PATH "/examples/dtmc/synchronous_leader/leader6_8.lab", "", "");
    
    std::shared_ptr<storm::models::sparse::Dtmc<double>> dtmc2 = abstractModel->as<storm::models::sparse::Dtmc<double>>();
    trueStates = storm::storage::BitVector(dtmc2->getNumberOfStates(), true);

    prob01 = storm::utility::graph::performProb01(*dtmc2, trueStates, storm::storage::BitVector(dtmc2->getStates("elected")));

    ASSERT_EQ(prob01.first.getNumberOfSetBits(), 0ull);
    ASSERT_EQ(prob01.second.getNumberOfSetBits(), 1312334ull);
    
    dtmc2 = nullptr;
}

TEST(GraphTest, PerformProb01MinMax) {
	std::shared_ptr<storm::models::sparse::Model<double>> abstractModel = storm::parser::AutoParser<>::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.tra", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader7.lab", "", "");
	std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = abstractModel->as<storm::models::sparse::Mdp<double>>();
	storm::storage::BitVector trueStates(mdp->getNumberOfStates(), true);
    
    std::pair<storm::storage::BitVector, storm::storage::BitVector> prob01(storm::utility::graph::performProb01Min(*mdp, trueStates, mdp->getStates("elected")));
    
    ASSERT_EQ(prob01.first.getNumberOfSetBits(), 0ull);
    ASSERT_EQ(prob01.second.getNumberOfSetBits(), 2095783ull);
    
    prob01 = storm::utility::graph::performProb01Max(*mdp, trueStates, mdp->getStates("elected"));

    ASSERT_EQ(prob01.first.getNumberOfSetBits(), 0ull);
    ASSERT_EQ(prob01.second.getNumberOfSetBits(), 2095783ull);
    
    mdp = nullptr;

    abstractModel = storm::parser::AutoParser<>::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.tra", STORM_CPP_BASE_PATH "/examples/mdp/consensus/coin4_6.lab", "", "");
	std::shared_ptr<storm::models::sparse::Mdp<double>> mdp2 = abstractModel->as<storm::models::sparse::Mdp<double>>();
	trueStates = storm::storage::BitVector(mdp2->getNumberOfStates(), true);

	prob01 = storm::utility::graph::performProb01Min(*mdp2, trueStates, mdp2->getStates("finished"));

    ASSERT_EQ(prob01.first.getNumberOfSetBits(), 0ull);
    ASSERT_EQ(prob01.second.getNumberOfSetBits(), 63616ull);

    prob01 = storm::utility::graph::performProb01Max(*mdp2, trueStates, mdp2->getStates("finished"));

    ASSERT_EQ(prob01.first.getNumberOfSetBits(), 0ull);
    ASSERT_EQ(prob01.second.getNumberOfSetBits(), 63616ull);
    
    mdp2 = nullptr;
}
