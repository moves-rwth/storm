#include "storm-config.h"
#include "test/storm_gtest.h"

#include "storm-parsers/parser/DirectEncodingParser.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/StandardRewardModel.h"

TEST(DirectEncodingParserTest, DtmcParsing) {
    std::shared_ptr<storm::models::sparse::Model<double>> modelPtr =
        storm::parser::DirectEncodingParser<double>::parseModel(STORM_TEST_RESOURCES_DIR "/dtmc/crowds-5-5.drn");

    // Test if parsed correctly.
    ASSERT_EQ(storm::models::ModelType::Dtmc, modelPtr->getType());
    ASSERT_EQ(8607ul, modelPtr->getNumberOfStates());
    ASSERT_EQ(15113ul, modelPtr->getNumberOfTransitions());
    ASSERT_TRUE(modelPtr->hasLabel("init"));
    ASSERT_EQ(1ul, modelPtr->getInitialStates().getNumberOfSetBits());
    ASSERT_TRUE(modelPtr->hasLabel("observeIGreater1"));
    ASSERT_EQ(4650ul, modelPtr->getStates("observeIGreater1").getNumberOfSetBits());
    ASSERT_TRUE(modelPtr->hasLabel("observe0Greater1"));
    ASSERT_EQ(1260ul, modelPtr->getStates("observe0Greater1").getNumberOfSetBits());
}

TEST(DirectEncodingParserTest, MdpParsing) {
    std::shared_ptr<storm::models::sparse::Model<double>> modelPtr =
        storm::parser::DirectEncodingParser<double>::parseModel(STORM_TEST_RESOURCES_DIR "/mdp/two_dice.drn");

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
    ASSERT_EQ(1ul, modelPtr->getNumberOfRewardModels());
    ASSERT_TRUE(modelPtr->hasRewardModel("coinflips"));
    ASSERT_TRUE(!modelPtr->getRewardModel("coinflips").hasStateRewards());
    ASSERT_TRUE(modelPtr->getRewardModel("coinflips").hasStateActionRewards());
    ASSERT_TRUE(!modelPtr->getRewardModel("coinflips").isAllZero());
}

TEST(DirectEncodingParserTest, CtmcParsing) {
    std::shared_ptr<storm::models::sparse::Model<double>> modelPtr =
        storm::parser::DirectEncodingParser<double>::parseModel(STORM_TEST_RESOURCES_DIR "/ctmc/cluster2.drn");

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
    ASSERT_EQ(1ul, modelPtr->getNumberOfRewardModels());
    ASSERT_TRUE(modelPtr->hasRewardModel("num_repairs"));
    ASSERT_TRUE(!modelPtr->getRewardModel("num_repairs").hasStateRewards());
    ASSERT_TRUE(modelPtr->getRewardModel("num_repairs").hasStateActionRewards());
    ASSERT_TRUE(!modelPtr->getRewardModel("num_repairs").isAllZero());
}

TEST(DirectEncodingParserTest, MarkovAutomatonParsing) {
    std::shared_ptr<storm::models::sparse::Model<double>> modelPtr =
        storm::parser::DirectEncodingParser<double>::parseModel(STORM_TEST_RESOURCES_DIR "/ma/jobscheduler.drn");
    std::shared_ptr<storm::models::sparse::MarkovAutomaton<double>> ma = modelPtr->as<storm::models::sparse::MarkovAutomaton<double>>();

    // Test if parsed correctly.
    ASSERT_EQ(storm::models::ModelType::MarkovAutomaton, modelPtr->getType());
    ASSERT_EQ(17ul, ma->getNumberOfStates());
    ASSERT_EQ(25ul, ma->getNumberOfTransitions());
    ASSERT_EQ(19ul, ma->getNumberOfChoices());
    ASSERT_EQ(10ul, ma->getMarkovianStates().getNumberOfSetBits());
    ASSERT_EQ(5, ma->getMaximalExitRate());
    ASSERT_EQ(1ul, ma->getNumberOfRewardModels());
    ASSERT_TRUE(ma->hasRewardModel("avg_waiting_time"));
    ASSERT_TRUE(ma->getRewardModel("avg_waiting_time").hasStateRewards());
    ASSERT_TRUE(!ma->getRewardModel("avg_waiting_time").hasStateActionRewards());
    ASSERT_TRUE(!ma->getRewardModel("avg_waiting_time").isAllZero());
    ASSERT_TRUE(modelPtr->hasLabel("init"));
    ASSERT_EQ(1ul, modelPtr->getInitialStates().getNumberOfSetBits());
    ASSERT_TRUE(modelPtr->hasLabel("one_job_finished"));
    ASSERT_EQ(6ul, modelPtr->getStates("one_job_finished").getNumberOfSetBits());
}

TEST(DirectEncodingParserTest, IntervalDtmcTest) {
    std::shared_ptr<storm::models::sparse::Model<storm::Interval>> modelPtr =
        storm::parser::DirectEncodingParser<storm::Interval>::parseModel(STORM_TEST_RESOURCES_DIR "/idtmc/brp-16-2.drn");
    std::shared_ptr<storm::models::sparse::Dtmc<storm::Interval>> dtmc = modelPtr->as<storm::models::sparse::Dtmc<storm::Interval>>();
    ASSERT_EQ(storm::models::ModelType::Dtmc, modelPtr->getType());
    ASSERT_EQ(613, dtmc->getNumberOfStates());
    EXPECT_TRUE(modelPtr->hasUncertainty());
}
