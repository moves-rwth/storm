#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/parser/NondeterministicModelParser.h"
#include "src/models/sparse/Mdp.h"
#include "src/exceptions/FileIoException.h"

#include "src/exceptions/OutOfRangeException.h"
#include "src/exceptions/InvalidArgumentException.h"

TEST(NondeterministicModelParserTest, NonExistingFile) {
	// No matter what happens, please do NOT create a file with the name "nonExistingFile.not"!
	ASSERT_THROW(storm::parser::NondeterministicModelParser::parseMdp(STORM_CPP_TESTS_BASE_PATH "/nonExistingFile.not", STORM_CPP_TESTS_BASE_PATH "/nonExistingFile.not"), storm::exceptions::FileIoException);
}

TEST(NondeterministicModelParserTest, BasicMdpParsing) {

	// Parse a Mdp and check the result.
	storm::models::sparse::Mdp<double> mdp(storm::parser::NondeterministicModelParser::parseMdp(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/mdp_general.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/mdp_general.lab", STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/mdp_general.state.rew", STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/mdp_general.trans.rew"));

	ASSERT_EQ(6ul, mdp.getNumberOfStates());
	ASSERT_EQ(22ul, mdp.getNumberOfTransitions());
	ASSERT_EQ(11ul, mdp.getNumberOfChoices());

	ASSERT_EQ(2ul, mdp.getInitialStates().getNumberOfSetBits());
	ASSERT_TRUE(mdp.getInitialStates().get(0));
	ASSERT_TRUE(mdp.getInitialStates().get(4));
	ASSERT_EQ(4ul, mdp.getStateLabeling().getNumberOfLabels());
	ASSERT_EQ(3ul, mdp.getLabelsOfState(0).size());

	ASSERT_TRUE(mdp.hasStateRewards());
	ASSERT_EQ(0, mdp.getStateRewardVector()[0]);
	ASSERT_EQ(42, mdp.getStateRewardVector()[4]);
	double rewardSum = 0;
	for(uint_fast64_t i = 0; i < mdp.getStateRewardVector().size(); i++) {
		rewardSum += mdp.getStateRewardVector()[i];
	}
	ASSERT_EQ(158.32, rewardSum);

	ASSERT_TRUE(mdp.hasTransitionRewards());
	ASSERT_EQ(17ul, mdp.getTransitionRewardMatrix().getEntryCount());
	rewardSum = 0;
	for(uint_fast64_t i = 0; i < mdp.getTransitionRewardMatrix().getRowCount(); i++) {
			rewardSum += mdp.getTransitionRewardMatrix().getRowSum(i);
	}
	ASSERT_EQ(1376.864, rewardSum);
}

TEST(NondeterministicModelParserTest, MismatchedFiles) {
	// Test file combinations that do not match, i.e. differing number of states, transitions, etc.

	// The labeling file contains a label for a non existent state.
	ASSERT_THROW(storm::parser::NondeterministicModelParser::parseMdp(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/mdp_mismatched.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/mdp_general.lab"), storm::exceptions::OutOfRangeException);

	// The state reward file contains a reward for a non existent state.
	ASSERT_THROW(storm::parser::NondeterministicModelParser::parseMdp(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/mdp_mismatched.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/mdp_mismatched.lab", STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/mdp_general.state.rew"), storm::exceptions::OutOfRangeException);

	// The transition reward file contains rewards for a non existent state.
	ASSERT_THROW(storm::parser::NondeterministicModelParser::parseMdp(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/mdp_mismatched.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/mdp_mismatched.lab", "", STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/mdp_general.trans.rew"), storm::exceptions::OutOfRangeException);

	// The transition reward file contains rewards for a non existent transition
	ASSERT_THROW(storm::parser::NondeterministicModelParser::parseMdp(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/mdp_mismatched.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/mdp_mismatched.lab", "", STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/mdp_mismatched.trans.rew"), storm::exceptions::OutOfRangeException);
}
