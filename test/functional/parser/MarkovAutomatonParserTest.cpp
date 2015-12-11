#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/parser/MarkovAutomatonParser.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/exceptions/FileIoException.h"
#include "src/exceptions/OutOfRangeException.h"

TEST(MarkovAutomatonParserTest, NonExistingFile) {
    // No matter what happens, please do NOT create a file with the name "nonExistingFile.not"!
    ASSERT_THROW(storm::parser::MarkovAutomatonParser<>::parseMarkovAutomaton(STORM_CPP_TESTS_BASE_PATH "/nonExistingFile.not", STORM_CPP_TESTS_BASE_PATH "/nonExistingFile.not", STORM_CPP_TESTS_BASE_PATH "/nonExistingFile.not"), storm::exceptions::FileIoException);
}

TEST(MarkovAutomatonParserTest, BasicParsing) {
    // Get the parsing result.
    storm::models::sparse::MarkovAutomaton<double> result = storm::parser::MarkovAutomatonParser<>::parseMarkovAutomaton(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/ma_general.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/ma_general.lab", STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/ma_general.state.rew");

    // Test sizes and counts.
    ASSERT_EQ(6ul, result.getNumberOfStates());
    ASSERT_EQ(7ul, result.getNumberOfChoices());
    ASSERT_EQ(12ul, result.getNumberOfTransitions());

    // Test the exit rates. These have to be 0 for all non-Markovian states.
    std::vector<double> rates = result.getExitRates();
    ASSERT_EQ(2, result.getExitRate(0));
    ASSERT_FALSE(result.isMarkovianState(1));
    ASSERT_EQ(0, result.getExitRate(1));
    ASSERT_EQ(15, result.getExitRate(2));
    ASSERT_FALSE(result.isMarkovianState(3));
    ASSERT_EQ(0, result.getExitRate(3));
    ASSERT_FALSE(result.isMarkovianState(4));
    ASSERT_EQ(0, result.getExitRate(4));
    ASSERT_FALSE(result.isMarkovianState(5));
    ASSERT_EQ(0, result.getExitRate(5));

    // Test the labeling.
    ASSERT_EQ(3ul, result.getStateLabeling().getNumberOfLabels());
    ASSERT_EQ(1ul, result.getInitialStates().getNumberOfSetBits());
    ASSERT_EQ(0ul, result.getLabelsOfState(4).size());
    ASSERT_EQ(1ul, result.getStateLabeling().getStates("goal").getNumberOfSetBits());

    // Test the state rewards.
    ASSERT_TRUE(result.hasRewardModel());
    double rewardSum = 0;
    for (uint_fast64_t i = 0; i < result.getRewardModel("").getStateRewardVector().size(); i++) {
        rewardSum += result.getRewardModel("").getStateRewardVector()[i];
    }
    ASSERT_EQ(1015.765099984, rewardSum);
    ASSERT_EQ(0, result.getRewardModel("").getStateRewardVector()[0]);

    // Test the transition rewards.
    ASSERT_FALSE(result.getRewardModel("").hasTransitionRewards());
}

TEST(MarkovAutomatonParserTest, MismatchedFiles) {

    // Test file combinations that do not match, i.e. differing number of states, transitions, etc.

    // The labeling file contains a label for a non existent state.
    ASSERT_THROW(storm::parser::MarkovAutomatonParser<>::parseMarkovAutomaton(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/ma_general.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/ma_mismatched.lab"), storm::exceptions::OutOfRangeException);

    // The state reward file contains a reward for a non existent state.
    ASSERT_THROW(storm::parser::MarkovAutomatonParser<>::parseMarkovAutomaton(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/ma_general.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/ma_general.lab", STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/ma_mismatched.state.rew"), storm::exceptions::OutOfRangeException);
}
