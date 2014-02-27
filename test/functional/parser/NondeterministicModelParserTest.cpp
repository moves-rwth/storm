/*
 * NondeterministicModelParserTest.cpp
 *
 *  Created on: Feb 26, 2014
 *      Author: Manuel Sascha Weiand
 */

#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/parser/NondeterministicModelParser.h"
#include "src/models/Mdp.h"
#include "src/models/Ctmdp.h"
#include "src/exceptions/FileIoException.h"

TEST(NondeterministicModelParserTest, NonExistingFile) {
	// No matter what happens, please do NOT create a file with the name "nonExistingFile.not"!
	ASSERT_THROW(storm::parser::NondeterministicModelParser::parseMdp(STORM_CPP_TESTS_BASE_PATH "/nonExistingFile.not", STORM_CPP_TESTS_BASE_PATH "/nonExistingFile.not"), storm::exceptions::FileIoException);

	ASSERT_THROW(storm::parser::NondeterministicModelParser::parseCtmdp(STORM_CPP_TESTS_BASE_PATH "/nonExistingFile.not", STORM_CPP_TESTS_BASE_PATH "/nonExistingFile.not"), storm::exceptions::FileIoException);
}

TEST(NondeterministicModelParserTest, BasicMdpParsing) {

	// Parse a Mdp and check the result.
	storm::models::Mdp<double> mdp(storm::parser::NondeterministicModelParser::parseMdp(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/mdp_general_input.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/mdp_general_input.lab", STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/mdp_general_input.state.rew", STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/mdp_general_input.trans.rew"));

	ASSERT_EQ(mdp.getNumberOfStates(), 6);
	ASSERT_EQ(mdp.getNumberOfTransitions(), 22);
	ASSERT_EQ(mdp.getNumberOfChoices(), 11);

	ASSERT_EQ(mdp.getInitialStates().getNumberOfSetBits(), 2);
	ASSERT_TRUE(mdp.getInitialStates().get(0));
	ASSERT_TRUE(mdp.getInitialStates().get(4));
	ASSERT_EQ(mdp.getStateLabeling().getNumberOfAtomicPropositions(), 4);
	ASSERT_EQ(mdp.getLabelsForState(0).size(), 3);

	ASSERT_TRUE(mdp.hasStateRewards());
	ASSERT_EQ(mdp.getStateRewardVector()[0], 0);
	ASSERT_EQ(mdp.getStateRewardVector()[4], 42);
	double rewardSum = 0;
	for(uint_fast64_t i = 0; i < mdp.getStateRewardVector().size(); i++) {
		rewardSum += mdp.getStateRewardVector()[i];
	}
	ASSERT_EQ(rewardSum, 158.32);

	ASSERT_TRUE(mdp.hasTransitionRewards());
	ASSERT_EQ(mdp.getTransitionRewardMatrix().getEntryCount(), 17);
	rewardSum = 0;
	for(uint_fast64_t i = 0; i < mdp.getTransitionRewardMatrix().getRowCount(); i++) {
			rewardSum += mdp.getTransitionRewardMatrix().getRowSum(i);
	}
	ASSERT_EQ(rewardSum, 1376.864);
}


TEST(NondeterministicModelParserTest, BasicCtmdpParsing) {
	// Parse a Ctmdp and check the result.
	storm::models::Ctmdp<double> ctmdp(storm::parser::NondeterministicModelParser::parseCtmdp(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/mdp_general_input.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/mdp_general_input.lab", STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/mdp_general_input.state.rew", STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/mdp_general_input.trans.rew"));

	ASSERT_EQ(ctmdp.getNumberOfStates(), 6);
	ASSERT_EQ(ctmdp.getNumberOfTransitions(), 22);
	ASSERT_EQ(ctmdp.getNumberOfChoices(), 11);

	ASSERT_EQ(ctmdp.getInitialStates().getNumberOfSetBits(), 2);
	ASSERT_TRUE(ctmdp.getInitialStates().get(0));
	ASSERT_TRUE(ctmdp.getInitialStates().get(4));
	ASSERT_EQ(ctmdp.getStateLabeling().getNumberOfAtomicPropositions(), 4);
	ASSERT_EQ(ctmdp.getLabelsForState(0).size(), 3);

	ASSERT_TRUE(ctmdp.hasStateRewards());
	ASSERT_EQ(ctmdp.getStateRewardVector()[0], 0);
	ASSERT_EQ(ctmdp.getStateRewardVector()[4], 42);
	double rewardSum = 0;
	for(uint_fast64_t i = 0; i < ctmdp.getStateRewardVector().size(); i++) {
		rewardSum += ctmdp.getStateRewardVector()[i];
	}
	ASSERT_EQ(rewardSum, 158.32);

	ASSERT_TRUE(ctmdp.hasTransitionRewards());
	ASSERT_EQ(ctmdp.getTransitionRewardMatrix().getEntryCount(), 17);
	rewardSum = 0;
	for(uint_fast64_t i = 0; i < ctmdp.getTransitionRewardMatrix().getRowCount(); i++) {
			rewardSum += ctmdp.getTransitionRewardMatrix().getRowSum(i);
	}
	ASSERT_EQ(rewardSum, 1376.864);
}

TEST(NondeterministicModelParserTest, UnmatchedFiles) {
	// Test file combinations that do not match, i.e. differing number of states, transitions, etc.

	// The labeling file contains a label for a non existent state.
	ASSERT_THROW(storm::parser::NondeterministicModelParser::parseMdp(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/mdp_unmatched.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/mdp_general_input.lab"), storm::exceptions::OutOfRangeException);

	// The state reward file contains a reward for a non existent state.
	ASSERT_THROW(storm::parser::NondeterministicModelParser::parseMdp(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/mdp_unmatched.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/mdp_unmatched.lab", STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/mdp_general_input.state.rew"), storm::exceptions::OutOfRangeException);

	// The transition reward file contains rewards for a non existent state.
	ASSERT_THROW(storm::parser::NondeterministicModelParser::parseMdp(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/mdp_unmatched.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/mdp_unmatched.lab", "", STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/mdp_general_input.trans.rew"), storm::exceptions::OutOfRangeException);

	// The transition reward file contains rewards for a non existent transition
	ASSERT_THROW(storm::parser::NondeterministicModelParser::parseMdp(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/mdp_unmatched.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/mdp_unmatched.lab", "", STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/mdp_unmatched.trans.rew"), storm::exceptions::OutOfRangeException);
}
