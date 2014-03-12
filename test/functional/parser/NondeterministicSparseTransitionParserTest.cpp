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

	storm::storage::SparseMatrix<double> nullInformation;
	ASSERT_THROW(storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitionRewards(STORM_CPP_TESTS_BASE_PATH "/nonExistingFile.not", nullInformation), storm::exceptions::FileIoException);
}


TEST(NondeterministicSparseTransitionParserTest, BasicTransitionsParsing) {

	// Parse a nondeterministic transitions file and test the result.
	storm::storage::SparseMatrix<double> result(storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitions(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/mdp_general.tra"));

	// Test the row mapping, i.e. at which row which state starts.
	ASSERT_EQ(6, result.getRowGroupCount());
	ASSERT_EQ(7, result.getRowGroupIndices().size());
	ASSERT_EQ(result.getRowGroupIndices()[0], 0);
	ASSERT_EQ(result.getRowGroupIndices()[1], 4);
	ASSERT_EQ(result.getRowGroupIndices()[2], 5);
	ASSERT_EQ(result.getRowGroupIndices()[3], 7);
	ASSERT_EQ(result.getRowGroupIndices()[4], 8);
	ASSERT_EQ(result.getRowGroupIndices()[5], 9);
	ASSERT_EQ(result.getRowGroupIndices()[6], 11);

	// Test the transition matrix.
	ASSERT_EQ(result.getColumnCount(), 6);
	ASSERT_EQ(result.getRowCount(), 11);
	ASSERT_EQ(result.getEntryCount(), 22);

	// Test every entry of the matrix.
	storm::storage::SparseMatrix<double>::const_iterator cIter = result.begin(0);

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
	storm::storage::SparseMatrix<double> modelInformation(storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitions(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/mdp_general.tra"));
	storm::storage::SparseMatrix<double> result(storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitionRewards(STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/mdp_general.trans.rew", modelInformation));

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
	storm::storage::SparseMatrix<double> correctResult(storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitions(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/mdp_general.tra"));
	storm::storage::SparseMatrix<double> whitespaceResult = storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitions(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/mdp_whitespaces.tra");
	ASSERT_EQ(correctResult.hash(), whitespaceResult.hash());
	ASSERT_EQ(correctResult.getRowGroupIndices().size(), whitespaceResult.getRowGroupIndices().size());
	for(uint_fast64_t i = 0; i < correctResult.getRowGroupIndices().size(); i++) {
		ASSERT_EQ(correctResult.getRowGroupIndices()[i], whitespaceResult.getRowGroupIndices()[i]);
	}

	// Do the same (minus the unused rowMapping) for the corresponding transition rewards file (with and without whitespaces)
	uint_fast64_t correctHash = storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitionRewards(STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/mdp_general.trans.rew", correctResult).hash();
	ASSERT_EQ(correctHash, storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitionRewards(STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/mdp_whitespaces.trans.rew", whitespaceResult).hash());
}

TEST(NondeterministicSparseTransitionParserTest, MixedTransitionOrder) {
	// Since the MatrixBuilder needs sequential input of new elements reordering of transitions or states should throw an exception.
	ASSERT_THROW(storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitions(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/mdp_mixedTransitionOrder.tra"), storm::exceptions::InvalidArgumentException);
	ASSERT_THROW(storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitions(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/mdp_mixedStateOrder.tra"), storm::exceptions::InvalidArgumentException);

	storm::storage::SparseMatrix<double> modelInformation = storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitions(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/mdp_general.tra");
	ASSERT_THROW(storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitionRewards(STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/mdp_mixedTransitionOrder.trans.rew", modelInformation), storm::exceptions::InvalidArgumentException);
	ASSERT_THROW(storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitionRewards(STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/mdp_mixedStateOrder.trans.rew", modelInformation), storm::exceptions::InvalidArgumentException);
}

TEST(NondeterministicSparseTransitionParserTest, FixDeadlocks) {
	// Set the fixDeadlocks flag temporarily. It is set to its old value once the deadlockOption object is destructed.
	storm::settings::InternalOptionMemento setDeadlockOption("fixDeadlocks", true);

	// Parse a transitions file with the fixDeadlocks Flag set and test if it works.
	storm::storage::SparseMatrix<double> result(storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitions(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/mdp_deadlock.tra"));

	ASSERT_EQ(result.getRowGroupIndices().size(), 8);
	ASSERT_EQ(result.getRowGroupIndices()[5], 9);
	ASSERT_EQ(result.getRowGroupIndices()[6], 10);
	ASSERT_EQ(result.getRowGroupIndices()[7], 12);

	ASSERT_EQ(result.getColumnCount(), 7);
	ASSERT_EQ(result.getRowCount(), 12);
	ASSERT_EQ(result.getEntryCount(), 23);

	storm::storage::SparseMatrix<double>::const_iterator cIter = result.begin(8);

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
	storm::storage::SparseMatrix<double> transitionResult = storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitions(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/mdp_general.tra");

	// There is a reward for a transition that does not exist in the transition matrix.
	ASSERT_THROW(storm::parser::NondeterministicSparseTransitionParser::parseNondeterministicTransitionRewards(STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/mdp_rewardForNonExTrans.trans.rew", transitionResult), storm::exceptions::WrongFormatException);
}
