/*
 * DeterministicModelParserTest.cpp
 *
 *  Created on: Feb 24, 2014
 *      Author: Manuel Sascha Weiand
 */

#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/parser/DeterministicModelParser.h"
#include "src/models/Dtmc.h"
#include "src/models/Ctmc.h"
#include "src/exceptions/FileIoException.h"

TEST(DeterministicModelParserTest, NonExistingFile) {
	// No matter what happens, please do NOT create a file with the name "nonExistingFile.not"!
	ASSERT_THROW(storm::parser::DeterministicModelParser::parseDtmc(STORM_CPP_TESTS_BASE_PATH "/nonExistingFile.not", STORM_CPP_TESTS_BASE_PATH "/nonExistingFile.not"), storm::exceptions::FileIoException);

	ASSERT_THROW(storm::parser::DeterministicModelParser::parseCtmc(STORM_CPP_TESTS_BASE_PATH "/nonExistingFile.not", STORM_CPP_TESTS_BASE_PATH "/nonExistingFile.not"), storm::exceptions::FileIoException);
}

TEST(DeterministicModelParserTest, BasicDtmcParsing) {

	// Parse a Dtmc and check the result.
	storm::models::Dtmc<double> dtmc(storm::parser::DeterministicModelParser::parseDtmc(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/dtmc_general_input.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/dtmc_general_input.lab", STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/dtmc_general_input.state.rew", STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/dtmc_general_input.trans.rew"));

	ASSERT_EQ(dtmc.getNumberOfStates(), 8);
	ASSERT_EQ(dtmc.getNumberOfTransitions(), 21);

	ASSERT_EQ(dtmc.getInitialStates().getNumberOfSetBits(), 2);
	ASSERT_TRUE(dtmc.getInitialStates().get(0));
	ASSERT_TRUE(dtmc.getInitialStates().get(7));
	ASSERT_EQ(dtmc.getStateLabeling().getNumberOfAtomicPropositions(), 5);
	ASSERT_EQ(dtmc.getLabelsForState(6).size(), 2);

	ASSERT_TRUE(dtmc.hasStateRewards());
	ASSERT_EQ(dtmc.getStateRewardVector()[7], 42);
	double rewardSum = 0;
	for(uint_fast64_t i = 0; i < dtmc.getStateRewardVector().size(); i++) {
		rewardSum += dtmc.getStateRewardVector()[i];
	}
	ASSERT_EQ(rewardSum, 263.32);

	ASSERT_TRUE(dtmc.hasTransitionRewards());
	ASSERT_EQ(dtmc.getTransitionRewardMatrix().getEntryCount(), 17);
	rewardSum = 0;
	for(uint_fast64_t i = 0; i < dtmc.getTransitionRewardMatrix().getRowCount(); i++) {
			rewardSum += dtmc.getTransitionRewardMatrix().getRowSum(i);
	}
	ASSERT_EQ(rewardSum, 125.4);
}


TEST(DeterministicModelParserTest, BasicCtmcParsing) {

	// Parse a Ctmc and check the result.
	storm::models::Ctmc<double> ctmc(storm::parser::DeterministicModelParser::parseCtmc(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/dtmc_general_input.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/dtmc_general_input.lab", STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/dtmc_general_input.state.rew", STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/dtmc_general_input.trans.rew"));

	ASSERT_EQ(ctmc.getNumberOfStates(), 8);
	ASSERT_EQ(ctmc.getNumberOfTransitions(), 21);

	ASSERT_EQ(ctmc.getInitialStates().getNumberOfSetBits(), 2);
	ASSERT_TRUE(ctmc.getInitialStates().get(0));
	ASSERT_TRUE(ctmc.getInitialStates().get(7));
	ASSERT_EQ(ctmc.getStateLabeling().getNumberOfAtomicPropositions(), 5);
	ASSERT_EQ(ctmc.getLabelsForState(6).size(), 2);

	ASSERT_TRUE(ctmc.hasStateRewards());
	ASSERT_EQ(ctmc.getStateRewardVector()[7], 42);
	double rewardSum = 0;
	for(uint_fast64_t i = 0; i < ctmc.getStateRewardVector().size(); i++) {
		rewardSum += ctmc.getStateRewardVector()[i];
	}
	ASSERT_EQ(rewardSum, 263.32);

	ASSERT_TRUE(ctmc.hasTransitionRewards());
	ASSERT_EQ(ctmc.getTransitionRewardMatrix().getEntryCount(), 17);
	rewardSum = 0;
	for(uint_fast64_t i = 0; i < ctmc.getTransitionRewardMatrix().getRowCount(); i++) {
			rewardSum += ctmc.getTransitionRewardMatrix().getRowSum(i);
	}
	ASSERT_EQ(rewardSum, 125.4);
}

TEST(DeterministicModelParserTest, UnmatchedFiles) {

	// Test file combinations that do not match, i.e. differing number of states, transitions, etc.

	// The labeling file contains a label for a non existent state.
	ASSERT_THROW(storm::parser::DeterministicModelParser::parseDtmc(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/dtmc_unmatched.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/dtmc_general_input.lab"), storm::exceptions::OutOfRangeException);

	// The state reward file contains a reward for a non existent state.
	ASSERT_THROW(storm::parser::DeterministicModelParser::parseDtmc(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/dtmc_unmatched.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/dtmc_unmatched.lab", STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/dtmc_general_input.state.rew"), storm::exceptions::OutOfRangeException);

	// The transition reward file contains rewards for a non existent state.
	ASSERT_THROW(storm::parser::DeterministicModelParser::parseDtmc(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/dtmc_unmatched.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/dtmc_unmatched.lab", "", STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/dtmc_general_input.trans.rew"), storm::exceptions::OutOfRangeException);

	// The transition reward file contains rewards for a non existent transition
	ASSERT_THROW(storm::parser::DeterministicModelParser::parseDtmc(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/dtmc_unmatched.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/dtmc_unmatched.lab", "", STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/dtmc_unmatched.trans.rew"), storm::exceptions::OutOfRangeException);
}
