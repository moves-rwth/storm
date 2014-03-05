/*
 * NondeterministicSparseTransitionParserTest.cpp
 *
 *  Created on: Feb 26, 2014
 *      Author: Manuel Sascha Weiand
 */

#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/parser/NondeterministicSparseTransitionParser.h"
#include "src/storage/SparseMatrix.h"
#include "src/settings/InternalOptionMemento.h"
#include "src/exceptions/FileIoException.h"
#include "src/exceptions/WrongFormatException.h"

TEST(NondeterministicSparseTransitionParserTest, NonExistingFile) {

	// No matter what happens, please do NOT create a file with the name "nonExistingFile.not"!
	ASSERT_THROW(storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitions(STORM_CPP_TESTS_BASE_PATH "/nonExistingFile.not"), storm::exceptions::FileIoException);

	storm::parser::NondeterministicSparseTransitionParser::Result nullInformation;
	ASSERT_THROW(storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitionRewards(STORM_CPP_TESTS_BASE_PATH "/nonExistingFile.not", nullInformation), storm::exceptions::FileIoException);
}


TEST(NondeterministicSparseTransitionParserTest, BasicTransitionsParsing) {

	// Parse a nondeterministic transitions file and test the result.
	storm::parser::NondeterministicSparseTransitionParser::Result result(storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitions(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/mdp_general.tra"));

	// Test the row mapping, i.e. at which row which state starts.
	ASSERT_EQ(result.rowMapping.size(), 7);
	ASSERT_EQ(result.rowMapping[0], 0);
	ASSERT_EQ(result.rowMapping[1], 4);
	ASSERT_EQ(result.rowMapping[2], 5);
	ASSERT_EQ(result.rowMapping[3], 7);
	ASSERT_EQ(result.rowMapping[4], 8);
	ASSERT_EQ(result.rowMapping[5], 9);
	ASSERT_EQ(result.rowMapping[6], 11);

	// Test the transition matrix.
	ASSERT_EQ(result.transitionMatrix.getColumnCount(), 6);
	ASSERT_EQ(result.transitionMatrix.getRowCount(), 11);
	ASSERT_EQ(result.transitionMatrix.getEntryCount(), 22);

	// Test every entry of the matrix.
	storm::storage::SparseMatrix<double>::const_iterator cIter = result.transitionMatrix.begin(0);

	ASSERT_EQ(cIter->first, 0);
	ASSERT_EQ(cIter->second, 0.9);
	cIter++;
	ASSERT_EQ(cIter->first, 1);
	ASSERT_EQ(cIter->second, 0.1);
	cIter++;
	ASSERT_EQ(cIter->first, 1);
	ASSERT_EQ(cIter->second, 0.2);
	cIter++;
	ASSERT_EQ(cIter->first, 2);
	ASSERT_EQ(cIter->second, 0.2);
	cIter++;
	ASSERT_EQ(cIter->first, 3);
	ASSERT_EQ(cIter->second, 0.2);
	cIter++;
	ASSERT_EQ(cIter->first, 4);
	ASSERT_EQ(cIter->second, 0.2);
	cIter++;
	ASSERT_EQ(cIter->first, 5);
	ASSERT_EQ(cIter->second, 0.2);
	cIter++;
	ASSERT_EQ(cIter->first, 5);
	ASSERT_EQ(cIter->second, 1);
	cIter++;
	ASSERT_EQ(cIter->first, 0);
	ASSERT_EQ(cIter->second, 0.1);
	cIter++;
	ASSERT_EQ(cIter->first, 4);
	ASSERT_EQ(cIter->second, 0.9);
	cIter++;
	ASSERT_EQ(cIter->first, 2);
	ASSERT_EQ(cIter->second, 1);
	cIter++;
	ASSERT_EQ(cIter->first, 2);
	ASSERT_EQ(cIter->second, 0.5);
	cIter++;
	ASSERT_EQ(cIter->first, 3);
	ASSERT_EQ(cIter->second, 0.5);
	cIter++;
	ASSERT_EQ(cIter->first, 2);
	ASSERT_EQ(cIter->second, 1);
	cIter++;
	ASSERT_EQ(cIter->first, 2);
	ASSERT_EQ(cIter->second, 0.001);
	cIter++;
	ASSERT_EQ(cIter->first, 3);
	ASSERT_EQ(cIter->second, 0.999);
	cIter++;
	ASSERT_EQ(cIter->first, 1);
	ASSERT_EQ(cIter->second, 0.7);
	cIter++;
	ASSERT_EQ(cIter->first, 4);
	ASSERT_EQ(cIter->second, 0.3);
	cIter++;
	ASSERT_EQ(cIter->first, 1);
	ASSERT_EQ(cIter->second, 0.2);
	cIter++;
	ASSERT_EQ(cIter->first, 4);
	ASSERT_EQ(cIter->second, 0.2);
	cIter++;
	ASSERT_EQ(cIter->first, 5);
	ASSERT_EQ(cIter->second, 0.6);
	cIter++;
	ASSERT_EQ(cIter->first, 5);
	ASSERT_EQ(cIter->second, 1);
}

TEST(NondeterministicSparseTransitionParserTest, BasicTransitionsRewardsParsing) {
	// Parse a nondeterministic transitions file and test the result.
	storm::parser::NondeterministicSparseTransitionParser::Result modelInformation(storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitions(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/mdp_general.tra"));
	storm::storage::SparseMatrix<double> result(storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitionRewards(STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/mdp_general.trans.rew", modelInformation).transitionMatrix);

	// Test the transition matrix.
	ASSERT_EQ(result.getColumnCount(), 6);
	ASSERT_EQ(result.getRowCount(), 11);
	ASSERT_EQ(result.getEntryCount(), 17);

	// Test every entry of the matrix.
	storm::storage::SparseMatrix<double>::const_iterator cIter = result.begin(0);

	ASSERT_EQ(cIter->first, 0);
	ASSERT_EQ(cIter->second, 1);
	cIter++;
	ASSERT_EQ(cIter->first, 1);
	ASSERT_EQ(cIter->second, 30);
	cIter++;
	ASSERT_EQ(cIter->first, 1);
	ASSERT_EQ(cIter->second, 15.2);
	cIter++;
	ASSERT_EQ(cIter->first, 2);
	ASSERT_EQ(cIter->second, 75);
	cIter++;
	ASSERT_EQ(cIter->first, 5);
	ASSERT_EQ(cIter->second, 2.45);
	cIter++;
	ASSERT_EQ(cIter->first, 5);
	ASSERT_EQ(cIter->second, 1);
	cIter++;
	ASSERT_EQ(cIter->first, 0);
	ASSERT_EQ(cIter->second, 0.114);
	cIter++;
	ASSERT_EQ(cIter->first, 4);
	ASSERT_EQ(cIter->second, 90);
	cIter++;
	ASSERT_EQ(cIter->first, 2);
	ASSERT_EQ(cIter->second, 1);
	cIter++;
	ASSERT_EQ(cIter->first, 2);
	ASSERT_EQ(cIter->second, 55);
	cIter++;
	ASSERT_EQ(cIter->first, 3);
	ASSERT_EQ(cIter->second, 87);
	cIter++;
	ASSERT_EQ(cIter->first, 2);
	ASSERT_EQ(cIter->second, 13);
	cIter++;
	ASSERT_EQ(cIter->first, 3);
	ASSERT_EQ(cIter->second, 999);
	cIter++;
	ASSERT_EQ(cIter->first, 1);
	ASSERT_EQ(cIter->second, 0.7);
	cIter++;
	ASSERT_EQ(cIter->first, 4);
	ASSERT_EQ(cIter->second, 0.3);
	cIter++;
	ASSERT_EQ(cIter->first, 1);
	ASSERT_EQ(cIter->second, 0.1);
	cIter++;
	ASSERT_EQ(cIter->first, 5);
	ASSERT_EQ(cIter->second, 6);
}

TEST(NondeterministicSparseTransitionParserTest, Whitespaces) {
	// Test the resilience of the parser against whitespaces.
	// Do so by comparing the hashes of the transition matices and the rowMapping vectors element by element.
	storm::parser::NondeterministicSparseTransitionParser::Result correctResult(storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitions(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/mdp_general.tra"));
	storm::parser::NondeterministicSparseTransitionParser::Result whitespaceResult = storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitions(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/mdp_whitespaces.tra");
	ASSERT_EQ(correctResult.transitionMatrix.hash(), whitespaceResult.transitionMatrix.hash());
	ASSERT_EQ(correctResult.rowMapping.size(), whitespaceResult.rowMapping.size());
	for(uint_fast64_t i = 0; i < correctResult.rowMapping.size(); i++) {
		ASSERT_EQ(correctResult.rowMapping[i], whitespaceResult.rowMapping[i]);
	}

	// Do the same (minus the unused rowMapping) for the corresponding transition rewards file (with and without whitespaces)
	uint_fast64_t correctHash = storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitionRewards(STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/mdp_general.trans.rew", correctResult).transitionMatrix.hash();
	ASSERT_EQ(correctHash, storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitionRewards(STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/mdp_whitespaces.trans.rew", whitespaceResult).transitionMatrix.hash());
}

TEST(NondeterministicSparseTransitionParserTest, MixedTransitionOrder) {
	// Since the MatrixBuilder needs sequential input of new elements reordering of transitions or states should throw an exception.
	ASSERT_THROW(storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitions(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/mdp_mixedTransitionOrder.tra"), storm::exceptions::InvalidArgumentException);
	ASSERT_THROW(storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitions(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/mdp_mixedStateOrder.tra"), storm::exceptions::InvalidArgumentException);

	storm::parser::NondeterministicSparseTransitionParser::Result modelInformation = storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitions(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/mdp_general.tra");
	ASSERT_THROW(storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitionRewards(STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/mdp_mixedTransitionOrder.trans.rew", modelInformation), storm::exceptions::InvalidArgumentException);
	ASSERT_THROW(storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitionRewards(STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/mdp_mixedStateOrder.trans.rew", modelInformation), storm::exceptions::InvalidArgumentException);
}

TEST(NondeterministicSparseTransitionParserTest, FixDeadlocks) {
	// Set the fixDeadlocks flag temporarily. It is set to its old value once the deadlockOption object is destructed.
	storm::settings::InternalOptionMemento setDeadlockOption("fixDeadlocks", true);

	// Parse a transitions file with the fixDeadlocks Flag set and test if it works.
	storm::parser::NondeterministicSparseTransitionParser::Result result(storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitions(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/mdp_deadlock.tra"));

	ASSERT_EQ(result.rowMapping.size(), 8);
	ASSERT_EQ(result.rowMapping[5], 9);
	ASSERT_EQ(result.rowMapping[6], 10);
	ASSERT_EQ(result.rowMapping[7], 12);

	ASSERT_EQ(result.transitionMatrix.getColumnCount(), 7);
	ASSERT_EQ(result.transitionMatrix.getRowCount(), 12);
	ASSERT_EQ(result.transitionMatrix.getEntryCount(), 23);

	storm::storage::SparseMatrix<double>::const_iterator cIter = result.transitionMatrix.begin(8);

	ASSERT_EQ(cIter->first, 1);
	ASSERT_EQ(cIter->second, 0.7);
	cIter++;
	ASSERT_EQ(cIter->first, 4);
	ASSERT_EQ(cIter->second, 0.3);
	cIter++;
	ASSERT_EQ(cIter->first, 5);
	ASSERT_EQ(cIter->second, 1);
	cIter++;
	ASSERT_EQ(cIter->first, 1);
	ASSERT_EQ(cIter->second, 0.2);
	cIter++;
	ASSERT_EQ(cIter->first, 4);
	ASSERT_EQ(cIter->second, 0.2);
	cIter++;
	ASSERT_EQ(cIter->first, 5);
	ASSERT_EQ(cIter->second, 0.6);
	cIter++;
	ASSERT_EQ(cIter->first, 5);
	ASSERT_EQ(cIter->second, 1);

}

TEST(NondeterministicSparseTransitionParserTest, DontFixDeadlocks) {
	// Try to parse a transitions file containing a deadlock state with the fixDeadlocksFlag unset. This should throw an exception.
	storm::settings::InternalOptionMemento unsetDeadlockOption("fixDeadlocks", false);

	ASSERT_THROW(storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitions(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/mdp_deadlock.tra"), storm::exceptions::WrongFormatException);
}

TEST(NondeterministicSparseTransitionParserTest, DoubledLines) {
	// There is a redundant line in the transition file. As the transition already exists this should throw an exception.
	ASSERT_THROW(storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitions(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/mdp_doubledLines.tra"), storm::exceptions::InvalidArgumentException);
}

TEST(NondeterministicSparseTransitionParserTest, RewardForNonExistentTransition) {

	// First parse a transition file. Then parse a transition reward file for the resulting transition matrix.
	storm::parser::NondeterministicSparseTransitionParser::Result transitionResult = storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitions(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/mdp_general.tra");

	// There is a reward for a transition that does not exist in the transition matrix.
	ASSERT_THROW(storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitionRewards(STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/mdp_rewardForNonExTrans.trans.rew", transitionResult), storm::exceptions::WrongFormatException);
}
