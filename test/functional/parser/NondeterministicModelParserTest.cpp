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
	storm::models::Mdp<double> mdp(storm::parser::NondeterministicModelParser::parseMdp(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/mdp_general.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/mdp_general.lab", STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/mdp_general.state.rew", STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/mdp_general.trans.rew"));

	ASSERT_EQ(6, mdp.getNumberOfStates());
	ASSERT_EQ(22, mdp.getNumberOfTransitions());
	ASSERT_EQ(11, mdp.getNumberOfChoices());

	ASSERT_EQ(2, mdp.getInitialStates().getNumberOfSetBits());
	ASSERT_TRUE(mdp.getInitialStates().get(0));
	ASSERT_TRUE(mdp.getInitialStates().get(4));
	ASSERT_EQ(4, mdp.getStateLabeling().getNumberOfAtomicPropositions());
	ASSERT_EQ(3, mdp.getLabelsForState(0).size());

	ASSERT_TRUE(mdp.hasStateRewards());
	ASSERT_EQ(0, mdp.getStateRewardVector()[0]);
	ASSERT_EQ(42, mdp.getStateRewardVector()[4]);
	double rewardSum = 0;
	for(uint_fast64_t i = 0; i < mdp.getStateRewardVector().size(); i++) {
		rewardSum += mdp.getStateRewardVector()[i];
	}
	ASSERT_EQ(158.32, rewardSum);

	ASSERT_TRUE(mdp.hasTransitionRewards());
	ASSERT_EQ(17, mdp.getTransitionRewardMatrix().getEntryCount());
	rewardSum = 0;
	for(uint_fast64_t i = 0; i < mdp.getTransitionRewardMatrix().getRowCount(); i++) {
			rewardSum += mdp.getTransitionRewardMatrix().getRowSum(i);
	}
	ASSERT_EQ(1376.864, rewardSum);
}


TEST(NondeterministicModelParserTest, BasicCtmdpParsing) {
	// Parse a Ctmdp and check the result.
	storm::models::Ctmdp<double> ctmdp(storm::parser::NondeterministicModelParser::parseCtmdp(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/mdp_general.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/mdp_general.lab", STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/mdp_general.state.rew", STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/mdp_general.trans.rew"));

	ASSERT_EQ(6, ctmdp.getNumberOfStates());
	ASSERT_EQ(22, ctmdp.getNumberOfTransitions());
	ASSERT_EQ(11, ctmdp.getNumberOfChoices());

	ASSERT_EQ(2, ctmdp.getInitialStates().getNumberOfSetBits());
	ASSERT_TRUE(ctmdp.getInitialStates().get(0));
	ASSERT_TRUE(ctmdp.getInitialStates().get(4));
	ASSERT_EQ(4, ctmdp.getStateLabeling().getNumberOfAtomicPropositions());
	ASSERT_EQ(3, ctmdp.getLabelsForState(0).size());

	ASSERT_TRUE(ctmdp.hasStateRewards());
	ASSERT_EQ(0, ctmdp.getStateRewardVector()[0]);
	ASSERT_EQ(42, ctmdp.getStateRewardVector()[4]);
	double rewardSum = 0;
	for(uint_fast64_t i = 0; i < ctmdp.getStateRewardVector().size(); i++) {
		rewardSum += ctmdp.getStateRewardVector()[i];
	}
	ASSERT_EQ(158.32, rewardSum);

	ASSERT_TRUE(ctmdp.hasTransitionRewards());
	ASSERT_EQ(ctmdp.getTransitionRewardMatrix().getEntryCount(), 17);
	rewardSum = 0;
	for(uint_fast64_t i = 0; i < ctmdp.getTransitionRewardMatrix().getRowCount(); i++) {
			rewardSum += ctmdp.getTransitionRewardMatrix().getRowSum(i);
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
