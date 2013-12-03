/*
 * MarkovAutomatonParserTest.cpp
 *
 *  Created on: 03.12.2013
 *      Author: Manuel Sascha Weiand
 */

#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/settings/Settings.h"

#include "src/parser/MarkovAutomatonSparseTransitionParser.h"
#include "src/parser/Parser.h"

#define STATE_COUNT 6
#define CHOICE_COUNT 7

TEST(MarkovAutomatonSparseTransitionParserTest, BasicParseTest) {

	// The file that will be used for the test.
	std::string filename = STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/ma_general_input_01.tra";

	storm::parser::MarkovAutomatonSparseTransitionParser::ResultType result = storm::parser::MarkovAutomatonSparseTransitionParser::parseMarkovAutomatonTransitions(filename);

	// Test all sizes and counts.
	ASSERT_EQ(result.transitionMatrix.getColumnCount(), STATE_COUNT);
	ASSERT_EQ(result.transitionMatrix.getRowCount(), CHOICE_COUNT);
	ASSERT_EQ(result.transitionMatrix.getNonZeroEntryCount(), 12);
	ASSERT_EQ(result.markovianChoices.getSize(), CHOICE_COUNT);
	ASSERT_EQ(result.markovianStates.getSize(), STATE_COUNT);
	ASSERT_EQ(result.markovianStates.getNumberOfSetBits(), 2);
	ASSERT_EQ(result.exitRates.size(), STATE_COUNT);
	ASSERT_EQ(result.nondeterministicChoiceIndices.size(), 7);

	// Test the general structure of the transition system (that will be an Markov automaton).

	// Test the mapping between states and transition matrix rows.
	ASSERT_EQ(result.nondeterministicChoiceIndices[0], 0);
	ASSERT_EQ(result.nondeterministicChoiceIndices[1], 1);
	ASSERT_EQ(result.nondeterministicChoiceIndices[2], 2);
	ASSERT_EQ(result.nondeterministicChoiceIndices[3], 3);
	ASSERT_EQ(result.nondeterministicChoiceIndices[4], 4);
	ASSERT_EQ(result.nondeterministicChoiceIndices[5], 6);
	ASSERT_EQ(result.nondeterministicChoiceIndices[6], 7);

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
	ASSERT_EQ(result.transitionMatrix.getValue(0,1), 2);
	ASSERT_EQ(result.transitionMatrix.getValue(1,2), 1);
	ASSERT_EQ(result.transitionMatrix.getValue(2,0), 1);
	ASSERT_EQ(result.transitionMatrix.getValue(2,1), 2);
	ASSERT_EQ(result.transitionMatrix.getValue(2,3), 4);
	ASSERT_EQ(result.transitionMatrix.getValue(2,4), 8);
	ASSERT_EQ(result.transitionMatrix.getValue(3,2), 0.5);
	ASSERT_EQ(result.transitionMatrix.getValue(3,3), 0.5);
	ASSERT_EQ(result.transitionMatrix.getValue(4,3), 1);
	ASSERT_EQ(result.transitionMatrix.getValue(5,1), 0.5);
	ASSERT_EQ(result.transitionMatrix.getValue(5,5), 0.5);
	ASSERT_EQ(result.transitionMatrix.getValue(6,5), 1);
}

TEST(MarkovAutomatonSparseTransitionParserTest, WhiteSpaceTest) {
	// The file that will be used for the test.
	std::string filename = STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/ma_whitespace_input_01.tra";

	storm::parser::MarkovAutomatonSparseTransitionParser::ResultType result = storm::parser::MarkovAutomatonSparseTransitionParser::parseMarkovAutomatonTransitions(filename);

	// Test all sizes and counts.
	ASSERT_EQ(result.transitionMatrix.getColumnCount(), STATE_COUNT);
	ASSERT_EQ(result.transitionMatrix.getRowCount(), CHOICE_COUNT);
	ASSERT_EQ(result.transitionMatrix.getNonZeroEntryCount(), 12);
	ASSERT_EQ(result.markovianChoices.getSize(), CHOICE_COUNT);
	ASSERT_EQ(result.markovianStates.getSize(), STATE_COUNT);
	ASSERT_EQ(result.markovianStates.getNumberOfSetBits(), 2);
	ASSERT_EQ(result.exitRates.size(), STATE_COUNT);
	ASSERT_EQ(result.nondeterministicChoiceIndices.size(), 7);

	// Test the general structure of the transition system (that will be an Markov automaton).

	// Test the mapping between states and transition matrix rows.
	ASSERT_EQ(result.nondeterministicChoiceIndices[0], 0);
	ASSERT_EQ(result.nondeterministicChoiceIndices[1], 1);
	ASSERT_EQ(result.nondeterministicChoiceIndices[2], 2);
	ASSERT_EQ(result.nondeterministicChoiceIndices[3], 3);
	ASSERT_EQ(result.nondeterministicChoiceIndices[4], 4);
	ASSERT_EQ(result.nondeterministicChoiceIndices[5], 6);
	ASSERT_EQ(result.nondeterministicChoiceIndices[6], 7);

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
	ASSERT_EQ(result.transitionMatrix.getValue(0,1), 2);
	ASSERT_EQ(result.transitionMatrix.getValue(1,2), 1);
	ASSERT_EQ(result.transitionMatrix.getValue(2,0), 1);
	ASSERT_EQ(result.transitionMatrix.getValue(2,1), 2);
	ASSERT_EQ(result.transitionMatrix.getValue(2,3), 4);
	ASSERT_EQ(result.transitionMatrix.getValue(2,4), 8);
	ASSERT_EQ(result.transitionMatrix.getValue(3,2), 0.5);
	ASSERT_EQ(result.transitionMatrix.getValue(3,3), 0.5);
	ASSERT_EQ(result.transitionMatrix.getValue(4,3), 1);
	ASSERT_EQ(result.transitionMatrix.getValue(5,1), 0.5);
	ASSERT_EQ(result.transitionMatrix.getValue(5,5), 0.5);
	ASSERT_EQ(result.transitionMatrix.getValue(6,5), 1);
}

//TODO: Deadlock Test. I am quite sure that the deadlock state handling does not behave quite right.
//                     Find a way to test this by manipulating the fixDeadlocks flag of the settings.

/*
TEST(MarkovAutomatonSparseTransitionParserTest, DeadlockTest) {
	// Save the fixDeadlocks flag, since it will be manipulated during the test.
	bool fixDeadlocks = storm::settings::Settings::getInstance()->isSet("fixDeadlocks");
	storm::settings::Settings::getInstance()->set("fixDeadlocks");


	// The file that will be used for the test.
	std::string filename = "/functional/parser/tra_files/ma_general_input_01.tra";

	storm::parser::MarkovAutomatonSparseTransitionParser::ResultType result = storm::parser::MarkovAutomatonSparseTransitionParser::parseMarkovAutomatonTransitions(filename);

	// Test if the result of the first pass has been transfered correctly
	ASSERT_EQ(result.transitionMatrix.colCount, 7);
	ASSERT_EQ(result.transitionMatrix.nonZeroEntryCount, 8);
	ASSERT_EQ(result.markovianChoices.getNumberOfSetBits(), 2);

	// Test the general structure of the transition system (that will be an Markov automaton).
	//TODO

	//Do the test again but this time without the fixDeadlock flag. This should throw an exception.
	storm::settings::Settings::getInstance()->unset("fixDeadlocks");

	bool thrown = false;

	try {
		// Parse the file, again.
		result = storm::parser::MarkovAutomatonSparseTransitionParser::parseMarkovAutomatonTransitions(filename);
	} catch(Exception &exception) {
		// Print the exception and remember that it was thrown.
		exception.print(std::cout);
		thrown = true;
	}

	ASSERT_TRUE(thrown);

	// Reset the fixDeadlocks flag to its original value.
	if(fixDeadlocks) {
		storm::settings::Settings::getInstance()->set("fixDeadlocks");
	}
}*/
