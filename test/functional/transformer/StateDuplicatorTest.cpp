#include "gtest/gtest.h"

#include "src/storm/transformer/StateDuplicator.h"


TEST(StateDuplicator, SimpleModelTest) {
       
    storm::storage::SparseMatrix<double> matrix;
    storm::storage::SparseMatrixBuilder<double> builder(6, 4, 7, true, true, 4);
    ASSERT_NO_THROW(builder.newRowGroup(0));
    ASSERT_NO_THROW(builder.addNextValue(0, 0, 0.3));
    ASSERT_NO_THROW(builder.addNextValue(0, 1, 0.7));
    ASSERT_NO_THROW(builder.addNextValue(1, 3, 1.0));
    ASSERT_NO_THROW(builder.newRowGroup(2));
    ASSERT_NO_THROW(builder.addNextValue(2, 1, 1.0));
    ASSERT_NO_THROW(builder.newRowGroup(3));
    ASSERT_NO_THROW(builder.addNextValue(3, 0, 1.0));
    ASSERT_NO_THROW(builder.newRowGroup(4));
    ASSERT_NO_THROW(builder.addNextValue(4, 0, 1.0));
    ASSERT_NO_THROW(builder.addNextValue(5, 3, 1.0));
    ASSERT_NO_THROW(matrix = builder.build());
    
    storm::models::sparse::StateLabeling labeling(4);
    storm::storage::BitVector initStates(4);
    initStates.set(0);
    labeling.addLabel("init", initStates);
    storm::storage::BitVector gateStates(4);
    gateStates.set(3);
    labeling.addLabel("gate", gateStates);
    storm::storage::BitVector aStates(4);
    aStates.set(0);
    aStates.set(2);
    labeling.addLabel("a", aStates);
    storm::storage::BitVector bStates(4);
    bStates.set(1);
    bStates.set(3);
    labeling.addLabel("b", bStates);
    
    std::unordered_map<std::string, storm::models::sparse::StandardRewardModel<double>> rewardModels;
    std::vector<double> stateReward = {1.0, 2.0, 3.0, 4.0};
    std::vector<double> stateActionReward = {1.1, 1.2, 2.1, 3.1, 4.1, 4.2};
    rewardModels.insert(std::make_pair("rewards", storm::models::sparse::StandardRewardModel<double>(stateReward, stateActionReward)));
    
    storm::models::sparse::Mdp<double> model(matrix, labeling, rewardModels);
    
    auto res = storm::transformer::StateDuplicator<storm::models::sparse::Mdp<double>>::transform(model, gateStates);
    
    storm::storage::SparseMatrixBuilder<double> expectedBuilder(8, 5, 10, true, true, 5);
    ASSERT_NO_THROW(expectedBuilder.newRowGroup(0));
    ASSERT_NO_THROW(expectedBuilder.addNextValue(0, 0, 0.3));
    ASSERT_NO_THROW(expectedBuilder.addNextValue(0, 1, 0.7));
    ASSERT_NO_THROW(expectedBuilder.addNextValue(1, 2, 1.0));
    ASSERT_NO_THROW(expectedBuilder.newRowGroup(2));
    ASSERT_NO_THROW(expectedBuilder.addNextValue(2, 1, 1.0));
    ASSERT_NO_THROW(expectedBuilder.newRowGroup(3));
    ASSERT_NO_THROW(expectedBuilder.addNextValue(3, 3, 1.0));
    ASSERT_NO_THROW(expectedBuilder.addNextValue(4, 2, 1.0));
    ASSERT_NO_THROW(expectedBuilder.newRowGroup(5));
    ASSERT_NO_THROW(expectedBuilder.addNextValue(5, 3, 0.3));
    ASSERT_NO_THROW(expectedBuilder.addNextValue(5, 4, 0.7));
    ASSERT_NO_THROW(expectedBuilder.addNextValue(6, 2, 1.0));
    ASSERT_NO_THROW(expectedBuilder.newRowGroup(7));
    ASSERT_NO_THROW(expectedBuilder.addNextValue(7, 4, 1.0));
    ASSERT_NO_THROW(matrix = expectedBuilder.build());
    EXPECT_EQ(matrix, res.model->getTransitionMatrix());
    
    initStates.resize(5);
    EXPECT_EQ(initStates, res.model->getInitialStates());
    gateStates=storm::storage::BitVector(5);
    gateStates.set(2);
    EXPECT_EQ(gateStates, res.model->getStates("gate"));
    aStates = initStates;
    aStates.set(3);
    EXPECT_EQ(aStates, res.model->getStates("a"));
    bStates = ~aStates;
    EXPECT_EQ(bStates, res.model->getStates("b"));
    
    EXPECT_TRUE(res.model->hasRewardModel("rewards"));
    EXPECT_TRUE(res.model->getRewardModel("rewards").hasStateRewards());
    stateReward = {1.0, 2.0, 4.0, 1.0, 2.0};
    EXPECT_EQ(stateReward, res.model->getRewardModel("rewards").getStateRewardVector());
    EXPECT_TRUE(res.model->getRewardModel("rewards").hasStateActionRewards());
    stateActionReward = {1.1, 1.2, 2.1, 4.1, 4.2, 1.1, 1.2, 2.1};
    EXPECT_EQ(stateActionReward, res.model->getRewardModel("rewards").getStateActionRewardVector());
    
    storm::storage::BitVector firstCopy(5);
    firstCopy.set(0);
    firstCopy.set(1);
    EXPECT_EQ(firstCopy, res.firstCopy);
    EXPECT_EQ(~firstCopy, res.secondCopy);
    
    std::vector<uint_fast64_t> mapping = {0,1,3,0,1};
    EXPECT_EQ(mapping, res.newToOldStateIndexMapping);
    uint_fast64_t max = std::numeric_limits<uint_fast64_t>::max();
    mapping = {0, 1, max, max};
    EXPECT_EQ(mapping, res.firstCopyOldToNewStateIndexMapping);
    mapping = {3, 4, max, 2};
    EXPECT_EQ(mapping, res.secondCopyOldToNewStateIndexMapping);
    
}
