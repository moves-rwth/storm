#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/parser/DeterministicModelParser.h"
#include "src/models/sparse/Dtmc.h"
#include "src/models/sparse/Ctmc.h"
#include "src/exceptions/FileIoException.h"

TEST(DeterministicModelParserTest, NonExistingFile) {
	// No matter what happens, please do NOT create a file with the name "nonExistingFile.not"!
	ASSERT_THROW(storm::parser::DeterministicModelParser::parseDtmc(STORM_CPP_TESTS_BASE_PATH "/nonExistingFile.not", STORM_CPP_TESTS_BASE_PATH "/nonExistingFile.not"), storm::exceptions::FileIoException);

	ASSERT_THROW(storm::parser::DeterministicModelParser::parseCtmc(STORM_CPP_TESTS_BASE_PATH "/nonExistingFile.not", STORM_CPP_TESTS_BASE_PATH "/nonExistingFile.not"), storm::exceptions::FileIoException);
}

TEST(DeterministicModelParserTest, BasicDtmcParsing) {

	// Parse a Dtmc and check the result.
    storm::models::sparse::Dtmc<double> dtmc(storm::parser::DeterministicModelParser::parseDtmc(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/dtmc_general.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/dtmc_general.lab", STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/dtmc_general.state.rew", STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/dtmc_general.trans.rew"));

	ASSERT_EQ(8, dtmc.getNumberOfStates());
	ASSERT_EQ(16, dtmc.getNumberOfTransitions());

	ASSERT_EQ(2, dtmc.getInitialStates().getNumberOfSetBits());
	ASSERT_TRUE(dtmc.getInitialStates().get(0));
	ASSERT_TRUE(dtmc.getInitialStates().get(7));
	ASSERT_EQ(5, dtmc.getStateLabeling().getNumberOfLabels());
	ASSERT_EQ(2, dtmc.getLabelsOfState(6).size());

	ASSERT_TRUE(dtmc.hasStateRewards());
	ASSERT_EQ(42, dtmc.getStateRewardVector()[7]);
	double rewardSum = 0;
	for(uint_fast64_t i = 0; i < dtmc.getStateRewardVector().size(); i++) {
		rewardSum += dtmc.getStateRewardVector()[i];
	}
	ASSERT_EQ(263.32, rewardSum);

	ASSERT_TRUE(dtmc.hasTransitionRewards());
	ASSERT_EQ(17, dtmc.getTransitionRewardMatrix().getEntryCount());
	rewardSum = 0;
	for(uint_fast64_t i = 0; i < dtmc.getTransitionRewardMatrix().getRowCount(); i++) {
			rewardSum += dtmc.getTransitionRewardMatrix().getRowSum(i);
	}
	ASSERT_EQ(125.4, rewardSum);
}


TEST(DeterministicModelParserTest, BasicCtmcParsing) {

	// Parse a Ctmc and check the result.
	storm::models::sparse::Ctmc<double> ctmc(storm::parser::DeterministicModelParser::parseCtmc(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/dtmc_general.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/dtmc_general.lab", STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/dtmc_general.state.rew", STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/dtmc_general.trans.rew"));

	ASSERT_EQ(8, ctmc.getNumberOfStates());
	ASSERT_EQ(16, ctmc.getNumberOfTransitions());

	ASSERT_EQ(2, ctmc.getInitialStates().getNumberOfSetBits());
	ASSERT_TRUE(ctmc.getInitialStates().get(0));
	ASSERT_TRUE(ctmc.getInitialStates().get(7));
	ASSERT_EQ(5, ctmc.getStateLabeling().getNumberOfLabels());
	ASSERT_EQ(2, ctmc.getLabelsOfState(6).size());

	ASSERT_TRUE(ctmc.hasStateRewards());
	ASSERT_EQ(42, ctmc.getStateRewardVector()[7]);
	double rewardSum = 0;
	for(uint_fast64_t i = 0; i < ctmc.getStateRewardVector().size(); i++) {
		rewardSum += ctmc.getStateRewardVector()[i];
	}
	ASSERT_EQ(263.32, rewardSum);

	ASSERT_TRUE(ctmc.hasTransitionRewards());
	ASSERT_EQ(17, ctmc.getTransitionRewardMatrix().getEntryCount());
	rewardSum = 0;
	for(uint_fast64_t i = 0; i < ctmc.getTransitionRewardMatrix().getRowCount(); i++) {
			rewardSum += ctmc.getTransitionRewardMatrix().getRowSum(i);
	}
	ASSERT_EQ(125.4, rewardSum);
}

TEST(DeterministicModelParserTest, MismatchedFiles) {

	// Test file combinations that do not match, i.e. differing number of states, transitions, etc.

	// The labeling file contains a label for a non existent state.
	ASSERT_THROW(storm::parser::DeterministicModelParser::parseDtmc(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/dtmc_mismatched.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/dtmc_general.lab"), storm::exceptions::OutOfRangeException);

	// The state reward file contains a reward for a non existent state.
	ASSERT_THROW(storm::parser::DeterministicModelParser::parseDtmc(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/dtmc_mismatched.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/dtmc_mismatched.lab", STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/dtmc_general.state.rew"), storm::exceptions::OutOfRangeException);

	// The transition reward file contains rewards for a non existent state.
	ASSERT_THROW(storm::parser::DeterministicModelParser::parseDtmc(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/dtmc_mismatched.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/dtmc_mismatched.lab", "", STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/dtmc_general.trans.rew"), storm::exceptions::OutOfRangeException);

	// The transition reward file contains rewards for a non existent transition
	ASSERT_THROW(storm::parser::DeterministicModelParser::parseDtmc(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/dtmc_mismatched.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/dtmc_mismatched.lab", "", STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/dtmc_mismatched.trans.rew"), storm::exceptions::OutOfRangeException);
}
