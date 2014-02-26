/*
 * DeterministicSparseTransitionParserTest.cpp
 *
 *  Created on: Feb 24, 2014
 *      Author: Manuel Sascha Weiand
 */

#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/parser/DeterministicSparseTransitionParser.h"
#include "src/storage/SparseMatrix.h"
#include "src/settings/InternalOptionMemento.h"
#include "src/exceptions/FileIoException.h"
#include "src/exceptions/WrongFormatException.h"

TEST(DeterministicSparseTransitionParserTest, NonExistingFile) {

	// No matter what happens, please do NOT create a file with the name "nonExistingFile.not"!
	ASSERT_THROW(storm::parser::DeterministicSparseTransitionParser::parseDeterministicTransitions(STORM_CPP_TESTS_BASE_PATH "/nonExistingFile.not"), storm::exceptions::FileIoException);

	storm::storage::SparseMatrix<double> nullMatrix;
	ASSERT_THROW(storm::parser::DeterministicSparseTransitionParser::parseDeterministicTransitionRewards(STORM_CPP_TESTS_BASE_PATH "/nonExistingFile.not", nullMatrix), storm::exceptions::FileIoException);
}


TEST(DeterministicSparseTransitionParserTest, BasicTransitionsParsing) {

	// Parse a deterministic transitions file and test the resulting matrix.
	storm::storage::SparseMatrix<double> transitionMatrix = storm::parser::DeterministicSparseTransitionParser::parseDeterministicTransitions(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/dtmc_general_input.tra");

	ASSERT_EQ(transitionMatrix.getColumnCount(), 8);
	ASSERT_EQ(transitionMatrix.getEntryCount(), 21);

	// Test every entry of the matrix.
	storm::storage::SparseMatrix<double>::const_iterator cIter = transitionMatrix.begin(0);

	ASSERT_EQ(cIter->first, 0);
	ASSERT_EQ(cIter->second, 0);
	cIter++;
	ASSERT_EQ(cIter->first, 1);
	ASSERT_EQ(cIter->second, 1);
	cIter++;
	ASSERT_EQ(cIter->first, 1);
	ASSERT_EQ(cIter->second, 0);
	cIter++;
	ASSERT_EQ(cIter->first, 2);
	ASSERT_EQ(cIter->second, 0.5);
	cIter++;
	ASSERT_EQ(cIter->first, 3);
	ASSERT_EQ(cIter->second, 0.5);
	cIter++;
	ASSERT_EQ(cIter->first, 2);
	ASSERT_EQ(cIter->second, 0);
	cIter++;
	ASSERT_EQ(cIter->first, 3);
	ASSERT_EQ(cIter->second, 0.4);
	cIter++;
	ASSERT_EQ(cIter->first, 4);
	ASSERT_EQ(cIter->second, 0.4);
	cIter++;
	ASSERT_EQ(cIter->first, 5);
	ASSERT_EQ(cIter->second, 0.2);
	cIter++;
	ASSERT_EQ(cIter->first, 3);
	ASSERT_EQ(cIter->second, 1);
	cIter++;
	ASSERT_EQ(cIter->first, 3);
	ASSERT_EQ(cIter->second, 1);
	cIter++;
	ASSERT_EQ(cIter->first, 4);
	ASSERT_EQ(cIter->second, 0);
	cIter++;
	ASSERT_EQ(cIter->first, 3);
	ASSERT_EQ(cIter->second, 0.1);
	cIter++;
	ASSERT_EQ(cIter->first, 4);
	ASSERT_EQ(cIter->second, 0.1);
	cIter++;
	ASSERT_EQ(cIter->first, 5);
	ASSERT_EQ(cIter->second, 0.1);
	cIter++;
	ASSERT_EQ(cIter->first, 6);
	ASSERT_EQ(cIter->second, 0.7);
	cIter++;
	ASSERT_EQ(cIter->first, 0);
	ASSERT_EQ(cIter->second, 0.9);
	cIter++;
	ASSERT_EQ(cIter->first, 5);
	ASSERT_EQ(cIter->second, 0);
	cIter++;
	ASSERT_EQ(cIter->first, 6);
	ASSERT_EQ(cIter->second, 0.1);
	cIter++;
	ASSERT_EQ(cIter->first, 6);
	ASSERT_EQ(cIter->second, 0.224653);
	cIter++;
	ASSERT_EQ(cIter->first, 7);
	ASSERT_EQ(cIter->second, 0.775347);
}

TEST(DeterministicSparseTransitionParserTest, BasicTransitionsRewardsParsing) {

	// First parse a transition file. Then parse a transition reward file for the resulting transitiion matrix.
	storm::storage::SparseMatrix<double> transitionMatrix = storm::parser::DeterministicSparseTransitionParser::parseDeterministicTransitions(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/dtmc_general_input.tra");

	storm::storage::SparseMatrix<double> rewardMatrix = storm::parser::DeterministicSparseTransitionParser::parseDeterministicTransitionRewards(STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/dtmc_general_input.trans.rew", transitionMatrix);

	ASSERT_EQ(rewardMatrix.getColumnCount(), 8);
	ASSERT_EQ(rewardMatrix.getEntryCount(), 17);

	// Test every entry of the matrix.
	storm::storage::SparseMatrix<double>::const_iterator cIter = rewardMatrix.begin(0);

	ASSERT_EQ(cIter->first, 1);
	ASSERT_EQ(cIter->second, 10);
	cIter++;
	ASSERT_EQ(cIter->first, 2);
	ASSERT_EQ(cIter->second, 5);
	cIter++;
	ASSERT_EQ(cIter->first, 3);
	ASSERT_EQ(cIter->second, 5.5);
	cIter++;
	ASSERT_EQ(cIter->first, 3);
	ASSERT_EQ(cIter->second, 21.4);
	cIter++;
	ASSERT_EQ(cIter->first, 4);
	ASSERT_EQ(cIter->second, 4);
	cIter++;
	ASSERT_EQ(cIter->first, 5);
	ASSERT_EQ(cIter->second, 2);
	cIter++;
	ASSERT_EQ(cIter->first, 3);
	ASSERT_EQ(cIter->second, 1);
	cIter++;
	ASSERT_EQ(cIter->first, 3);
	ASSERT_EQ(cIter->second, 1);
	cIter++;
	ASSERT_EQ(cIter->first, 3);
	ASSERT_EQ(cIter->second, 0.1);
	cIter++;
	ASSERT_EQ(cIter->first, 4);
	ASSERT_EQ(cIter->second, 1.1);
	cIter++;
	ASSERT_EQ(cIter->first, 5);
	ASSERT_EQ(cIter->second, 9.5);
	cIter++;
	ASSERT_EQ(cIter->first, 6);
	ASSERT_EQ(cIter->second, 6.7);
	cIter++;
	ASSERT_EQ(cIter->first, 0);
	ASSERT_EQ(cIter->second, 1);
	cIter++;
	ASSERT_EQ(cIter->first, 5);
	ASSERT_EQ(cIter->second, 0);
	cIter++;
	ASSERT_EQ(cIter->first, 6);
	ASSERT_EQ(cIter->second, 12);
	cIter++;
	ASSERT_EQ(cIter->first, 6);
	ASSERT_EQ(cIter->second, 35.224653);
	cIter++;
	ASSERT_EQ(cIter->first, 7);
	ASSERT_EQ(cIter->second, 9.875347);
}


TEST(DeterministicSparseTransitionParserTest, Whitespaces) {

	// Test the resilience of the parser against whitespaces.
	// Do so by comparing the hash of the matrix resulting from the file without whitespaces with the hash of the matrix resulting from the file with whitespaces.
	uint_fast64_t correctHash = storm::parser::DeterministicSparseTransitionParser::parseDeterministicTransitions(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/dtmc_general_input.tra").hash();
	storm::storage::SparseMatrix<double> transitionMatrix = storm::parser::DeterministicSparseTransitionParser::parseDeterministicTransitions(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/dtmc_whitespaces_input.tra");
	ASSERT_EQ(correctHash, transitionMatrix.hash());

	// Do the same for the corresponding transition rewards file (with and without whitespaces)
	correctHash = storm::parser::DeterministicSparseTransitionParser::parseDeterministicTransitionRewards(STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/dtmc_general_input.trans.rew", transitionMatrix).hash();
	ASSERT_EQ(correctHash, storm::parser::DeterministicSparseTransitionParser::parseDeterministicTransitionRewards(STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/dtmc_whitespaces_input.trans.rew", transitionMatrix).hash());
}

TEST(DeterministicSparseTransitionParserTest, MixedTransitionOrder) {

	// Since the MatrixBuilder needs sequential input of new elements reordering of transitions or states should throw an exception.
	ASSERT_THROW(storm::parser::DeterministicSparseTransitionParser::parseDeterministicTransitions(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/dtmc_mixedTransitionOrder_input.tra"), storm::exceptions::InvalidArgumentException);
	ASSERT_THROW(storm::parser::DeterministicSparseTransitionParser::parseDeterministicTransitions(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/dtmc_mixedStateOrder_input.tra"), storm::exceptions::InvalidArgumentException);

	storm::storage::SparseMatrix<double> transitionMatrix = storm::parser::DeterministicSparseTransitionParser::parseDeterministicTransitions(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/dtmc_general_input.tra");
	ASSERT_THROW(storm::parser::DeterministicSparseTransitionParser::parseDeterministicTransitionRewards(STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/dtmc_mixedTransitionOrder_input.trans.rew", transitionMatrix), storm::exceptions::InvalidArgumentException);
	ASSERT_THROW(storm::parser::DeterministicSparseTransitionParser::parseDeterministicTransitionRewards(STORM_CPP_TESTS_BASE_PATH "/functional/parser/rew_files/dtmc_mixedStateOrder_input.trans.rew", transitionMatrix), storm::exceptions::InvalidArgumentException);
}

TEST(DeterministicSparseTransitionParserTest, FixDeadlocks) {

	// Set the fixDeadlocks flag temporarily. It is set to its old value once the deadlockOption object is destructed.
	storm::settings::InternalOptionMemento setDeadlockOption("fixDeadlocks", true);

	// Parse a transitions file with the fixDeadlocks Flag set and test if it works.
	storm::storage::SparseMatrix<double> transitionMatrix = storm::parser::DeterministicSparseTransitionParser::parseDeterministicTransitions(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/dtmc_deadlock_input.tra");

	ASSERT_EQ(transitionMatrix.getColumnCount(), 9);
	ASSERT_EQ(transitionMatrix.getEntryCount(), 23);

	storm::storage::SparseMatrix<double>::const_iterator cIter = transitionMatrix.begin(7);
	ASSERT_EQ(cIter->first, 7);
	ASSERT_EQ(cIter->second, 1);
	cIter++;
	ASSERT_EQ(cIter->first, 6);
	ASSERT_EQ(cIter->second, 0.224653);
	cIter++;
	ASSERT_EQ(cIter->first, 7);
	ASSERT_EQ(cIter->second, 0.775347);
	cIter++;
	ASSERT_EQ(cIter->first, 8);
	ASSERT_EQ(cIter->second, 0);
}

TEST(DeterministicSparseTransitionParserTest, DontFixDeadlocks) {

	// Try to parse a transitions file containing a deadlock state with the fixDeadlocksFlag unset. This should throw an exception.
	storm::settings::InternalOptionMemento unsetDeadlockOption("fixDeadlocks", false);

	ASSERT_THROW(storm::parser::DeterministicSparseTransitionParser::parseDeterministicTransitions(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/dtmc_deadlock_input.tra"), storm::exceptions::WrongFormatException);
}

TEST(DeterministicSparseTransitionParserTest, DoubledLines) {
	// There is a redundant line in the transition file. As the transition already exists this should throw an exception.
	// Note: If two consecutive lines are doubled no exception is thrown.
	ASSERT_THROW(storm::parser::DeterministicSparseTransitionParser::parseDeterministicTransitions(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/dtmc_doubledLines_input.tra"), storm::exceptions::InvalidArgumentException);
}
