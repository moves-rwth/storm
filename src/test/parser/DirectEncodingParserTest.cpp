#include "gtest/gtest.h"
#include "storm-config.h"

#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/parser/DirectEncodingParser.h"

TEST(DirectEncodingParserTest, CtmcParsing) {
    std::shared_ptr<storm::models::sparse::Model<double>> modelPtr = storm::parser::DirectEncodingParser<double>::parseModel(STORM_TEST_RESOURCES_DIR "/ctmc/cluster2.drn");

    // Test if parsed correctly.
    ASSERT_EQ(storm::models::ModelType::Ctmc, modelPtr->getType());
    ASSERT_EQ(276ul, modelPtr->getNumberOfStates());
    ASSERT_EQ(1120ul, modelPtr->getNumberOfTransitions());
    ASSERT_TRUE(modelPtr->hasLabel("init"));
    ASSERT_EQ(1ul, modelPtr->getInitialStates().getNumberOfSetBits());
    ASSERT_TRUE(modelPtr->hasLabel("premium"));
    ASSERT_EQ(64ul, modelPtr->getStates("premium").getNumberOfSetBits());
    ASSERT_TRUE(modelPtr->hasLabel("minimum"));
    ASSERT_EQ(132ul, modelPtr->getStates("minimum").getNumberOfSetBits());
}

TEST(DirectEncodingParserTest, MdpParsing) {
    std::shared_ptr<storm::models::sparse::Model<double>> modelPtr = storm::parser::DirectEncodingParser<double>::parseModel(STORM_TEST_RESOURCES_DIR "/mdp/two_dice.drn");

    // Test if parsed correctly.
    ASSERT_EQ(storm::models::ModelType::Mdp, modelPtr->getType());
    ASSERT_EQ(169ul, modelPtr->getNumberOfStates());
    ASSERT_EQ(436ul, modelPtr->getNumberOfTransitions());
    ASSERT_EQ(254ul, modelPtr->as<storm::models::sparse::Mdp<double>>()->getNumberOfChoices());
    ASSERT_TRUE(modelPtr->hasLabel("init"));
    ASSERT_EQ(1ul, modelPtr->getInitialStates().getNumberOfSetBits());
    ASSERT_TRUE(modelPtr->hasLabel("six"));
    ASSERT_EQ(5ul, modelPtr->getStates("six").getNumberOfSetBits());
    ASSERT_TRUE(modelPtr->hasLabel("eleven"));
    ASSERT_EQ(2ul, modelPtr->getStates("eleven").getNumberOfSetBits());
}

