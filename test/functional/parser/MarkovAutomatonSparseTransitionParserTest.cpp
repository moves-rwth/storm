/*
 * MarkovAutomatonParserTest.cpp
 *
 *  Created on: 03.12.2013
 *      Author: Manuel Sascha Weiand
 */

#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/settings/Settings.h"

#include <vector>

#include "src/parser/MarkovAutomatonSparseTransitionParser.h"
#include "src/utility/cstring.h"
#include "src/parser/MarkovAutomatonParser.h"
#include "src/settings/InternalOptionMemento.h"
#include "src/exceptions/WrongFormatException.h"
#include "src/exceptions/FileIoException.h"

#define STATE_COUNT 6
#define CHOICE_COUNT 7

TEST(MarkovAutomatonSparseTransitionParserTest, NonExistingFile) {

	// No matter what happens, please do NOT create a file with the name "nonExistingFile.not"!
	ASSERT_THROW(storm::parser::MarkovAutomatonSparseTransitionParser::parseMarkovAutomatonTransitions(STORM_CPP_TESTS_BASE_PATH "/nonExistingFile.not"), storm::exceptions::FileIoException);
}

TEST(MarkovAutomatonSparseTransitionParserTest, BasicParsing) {

	// The file that will be used for the test.
	std::string filename = STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/ma_general.tra";

	// Execute the parser.
	storm::parser::MarkovAutomatonSparseTransitionParser::Result result = storm::parser::MarkovAutomatonSparseTransitionParser::parseMarkovAutomatonTransitions(filename);

	// Build the actual transition matrix.
	storm::storage::SparseMatrix<double> transitionMatrix(result.transitionMatrixBuilder.build(0,0));

	// Test all sizes and counts.
	ASSERT_EQ(transitionMatrix.getColumnCount(), STATE_COUNT);
	ASSERT_EQ(transitionMatrix.getRowCount(), CHOICE_COUNT);
	ASSERT_EQ(transitionMatrix.getEntryCount(), 12);
	ASSERT_EQ(transitionMatrix.getRowGroupCount(), 6);
	ASSERT_EQ(transitionMatrix.getRowGroupIndices().size(), 7);
	ASSERT_EQ(result.markovianChoices.size(), CHOICE_COUNT);
	ASSERT_EQ(result.markovianStates.size(), STATE_COUNT);
	ASSERT_EQ(result.markovianStates.getNumberOfSetBits(), 2);
	ASSERT_EQ(result.exitRates.size(), STATE_COUNT);

	// Test the general structure of the transition system (that will be an Markov automaton).

	// Test the mapping between states and transition matrix rows.
	ASSERT_EQ(transitionMatrix.getRowGroupIndices()[0], 0);
	ASSERT_EQ(transitionMatrix.getRowGroupIndices()[1], 1);
	ASSERT_EQ(transitionMatrix.getRowGroupIndices()[2], 2);
	ASSERT_EQ(transitionMatrix.getRowGroupIndices()[3], 3);
	ASSERT_EQ(transitionMatrix.getRowGroupIndices()[4], 4);
	ASSERT_EQ(transitionMatrix.getRowGroupIndices()[5], 6);
	ASSERT_EQ(transitionMatrix.getRowGroupIndices()[6], 7);

	// Test the Markovian states.
	ASSERT_TRUE(result.markovianStates.get(0));
	ASSERT_FALSE(result.markovianStates.get(1));
	ASSERT_TRUE(result.markovianStates.get(2));
	ASSERT_FALSE(result.markovianStates.get(3));
	ASSERT_FALSE(result.markovianStates.get(4));
	ASSERT_FALSE(result.markovianStates.get(5));

	// Test the exit rates. These have to be 0 for all non-Markovian states.
	ASSERT_EQ(result.exitRates[0], 2);
	ASSERT_EQ(result.exitRates[1], 0);
	ASSERT_EQ(result.exitRates[2], 15);
	ASSERT_EQ(result.exitRates[3], 0);
	ASSERT_EQ(result.exitRates[4], 0);
	ASSERT_EQ(result.exitRates[5], 0);

	// Finally, test the transition matrix itself.
	storm::storage::SparseMatrix<double>::const_iterator cIter = transitionMatrix.begin(0);

	ASSERT_EQ(cIter->second, 2);
	cIter++;
	ASSERT_EQ(cIter->second, 1);
	cIter++;
	ASSERT_EQ(cIter->second, 1);
	cIter++;
	ASSERT_EQ(cIter->second, 2);
	cIter++;
	ASSERT_EQ(cIter->second, 4);
	cIter++;
	ASSERT_EQ(cIter->second, 8);
	cIter++;
	ASSERT_EQ(cIter->second, 0.5);
	cIter++;
	ASSERT_EQ(cIter->second, 0.5);
	cIter++;
	ASSERT_EQ(cIter->second, 1);
	cIter++;
	ASSERT_EQ(cIter->second, 0.5);
	cIter++;
	ASSERT_EQ(cIter->second, 0.5);
	cIter++;
	ASSERT_EQ(cIter->second, 1);
	cIter++;
	ASSERT_EQ(transitionMatrix.end(), cIter);
}

TEST(MarkovAutomatonSparseTransitionParserTest, Whitespaces) {
	// The file that will be used for the test.
	std::string filename = STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/ma_whitespaces.tra";

	// Execute the parser.
	storm::parser::MarkovAutomatonSparseTransitionParser::Result result = storm::parser::MarkovAutomatonSparseTransitionParser::parseMarkovAutomatonTransitions(filename);

	// Build the actual transition matrix.
	storm::storage::SparseMatrix<double> transitionMatrix(result.transitionMatrixBuilder.build(0,0));

	// Test all sizes and counts.
	ASSERT_EQ(transitionMatrix.getColumnCount(), STATE_COUNT);
	ASSERT_EQ(transitionMatrix.getRowCount(), CHOICE_COUNT);
	ASSERT_EQ(transitionMatrix.getEntryCount(), 12);
	ASSERT_EQ(transitionMatrix.getRowGroupCount(), 6);
	ASSERT_EQ(transitionMatrix.getRowGroupIndices().size(), 7);
	ASSERT_EQ(result.markovianChoices.size(), CHOICE_COUNT);
	ASSERT_EQ(result.markovianStates.size(), STATE_COUNT);
	ASSERT_EQ(result.markovianStates.getNumberOfSetBits(), 2);
	ASSERT_EQ(result.exitRates.size(), STATE_COUNT);

	// Test the general structure of the transition system (that will be an Markov automaton).

	// Test the mapping between states and transition matrix rows.
	ASSERT_EQ(transitionMatrix.getRowGroupIndices()[0], 0);
	ASSERT_EQ(transitionMatrix.getRowGroupIndices()[1], 1);
	ASSERT_EQ(transitionMatrix.getRowGroupIndices()[2], 2);
	ASSERT_EQ(transitionMatrix.getRowGroupIndices()[3], 3);
	ASSERT_EQ(transitionMatrix.getRowGroupIndices()[4], 4);
	ASSERT_EQ(transitionMatrix.getRowGroupIndices()[5], 6);
	ASSERT_EQ(transitionMatrix.getRowGroupIndices()[6], 7);

	// Test the Markovian states.
	ASSERT_TRUE(result.markovianStates.get(0));
	ASSERT_FALSE(result.markovianStates.get(1));
	ASSERT_TRUE(result.markovianStates.get(2));
	ASSERT_FALSE(result.markovianStates.get(3));
	ASSERT_FALSE(result.markovianStates.get(4));
	ASSERT_FALSE(result.markovianStates.get(5));

	// Test the exit rates. These have to be 0 for all non-Markovian states.
	ASSERT_EQ(result.exitRates[0], 2);
	ASSERT_EQ(result.exitRates[1], 0);
	ASSERT_EQ(result.exitRates[2], 15);
	ASSERT_EQ(result.exitRates[3], 0);
	ASSERT_EQ(result.exitRates[4], 0);
	ASSERT_EQ(result.exitRates[5], 0);

	// Finally, test the transition matrix itself.
	storm::storage::SparseMatrix<double>::const_iterator cIter = transitionMatrix.begin(0);

	ASSERT_EQ(cIter->second, 2);
	cIter++;
	ASSERT_EQ(cIter->second, 1);
	cIter++;
	ASSERT_EQ(cIter->second, 1);
	cIter++;
	ASSERT_EQ(cIter->second, 2);
	cIter++;
	ASSERT_EQ(cIter->second, 4);
	cIter++;
	ASSERT_EQ(cIter->second, 8);
	cIter++;
	ASSERT_EQ(cIter->second, 0.5);
	cIter++;
	ASSERT_EQ(cIter->second, 0.5);
	cIter++;
	ASSERT_EQ(cIter->second, 1);
	cIter++;
	ASSERT_EQ(cIter->second, 0.5);
	cIter++;
	ASSERT_EQ(cIter->second, 0.5);
	cIter++;
	ASSERT_EQ(cIter->second, 1);
	cIter++;
	ASSERT_EQ(transitionMatrix.end(), cIter);
}

TEST(MarkovAutomatonSparseTransitionParserTest, FixDeadlocks) {
	// Set the fixDeadlocks flag temporarily. It is set to its old value once the deadlockOption object is destructed.
	storm::settings::InternalOptionMemento setDeadlockOption("fixDeadlocks", true);

	// Parse a Markov Automaton transition file with the fixDeadlocks Flag set and test if it works.
	storm::parser::MarkovAutomatonSparseTransitionParser::Result result = storm::parser::MarkovAutomatonSparseTransitionParser::parseMarkovAutomatonTransitions(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/ma_deadlock.tra");

	// Test if the result is consistent with the parsed Markov Automaton.
	storm::storage::SparseMatrix<double> resultMatrix(result.transitionMatrixBuilder.build(0,0));
	ASSERT_EQ(resultMatrix.getColumnCount(), STATE_COUNT + 1);
	ASSERT_EQ(resultMatrix.getEntryCount(), 13);
	ASSERT_EQ(resultMatrix.getRowGroupCount(), 7);
	ASSERT_EQ(resultMatrix.getRowGroupIndices().size(), 8);
	ASSERT_EQ(result.markovianChoices.size(), CHOICE_COUNT +1);
	ASSERT_EQ(result.markovianStates.size(), STATE_COUNT +1);
	ASSERT_EQ(result.markovianStates.getNumberOfSetBits(), 2);
	ASSERT_EQ(result.exitRates.size(), STATE_COUNT + 1);
}

TEST(MarkovAutomatonSparseTransitionParserTest, DontFixDeadlocks) {
	// Try to parse a Markov Automaton transition file containing a deadlock state with the fixDeadlocksFlag unset. This should throw an exception.
	storm::settings::InternalOptionMemento unsetDeadlockOption("fixDeadlocks", false);

	ASSERT_THROW(storm::parser::MarkovAutomatonSparseTransitionParser::parseMarkovAutomatonTransitions(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/ma_deadlock.tra"), storm::exceptions::WrongFormatException);
}
