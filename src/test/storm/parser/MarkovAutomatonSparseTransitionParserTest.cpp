/*
 * MarkovAutomatonParserTest.cpp
 *
 *  Created on: 03.12.2013
 *      Author: Manuel Sascha Weiand
 */

#include "storm-config.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/BuildSettings.h"
#include "test/storm_gtest.h"

#include <vector>

#include "storm-parsers/parser/MarkovAutomatonParser.h"
#include "storm-parsers/parser/MarkovAutomatonSparseTransitionParser.h"
#include "storm-parsers/util/cstring.h"
#include "storm/exceptions/FileIoException.h"
#include "storm/exceptions/WrongFormatException.h"
#include "storm/settings/SettingMemento.h"

#define STATE_COUNT 6ul
#define CHOICE_COUNT 7ul

TEST(MarkovAutomatonSparseTransitionParserTest, NonExistingFile) {
    // No matter what happens, please do NOT create a file with the name "nonExistingFile.not"!
    STORM_SILENT_ASSERT_THROW(
        storm::parser::MarkovAutomatonSparseTransitionParser<>::parseMarkovAutomatonTransitions(STORM_TEST_RESOURCES_DIR "/nonExistingFile.not"),
        storm::exceptions::FileIoException);
}

TEST(MarkovAutomatonSparseTransitionParserTest, BasicParsing) {
    // The file that will be used for the test.
    std::string filename = STORM_TEST_RESOURCES_DIR "/tra/ma_general.tra";

    // Execute the parser.
    typename storm::parser::MarkovAutomatonSparseTransitionParser<>::Result result =
        storm::parser::MarkovAutomatonSparseTransitionParser<>::parseMarkovAutomatonTransitions(filename);

    // Build the actual transition matrix.
    storm::storage::SparseMatrix<double> transitionMatrix(result.transitionMatrixBuilder.build(0, 0));

    // Test all sizes and counts.
    ASSERT_EQ(STATE_COUNT, transitionMatrix.getColumnCount());
    ASSERT_EQ(CHOICE_COUNT, transitionMatrix.getRowCount());
    ASSERT_EQ(12ul, transitionMatrix.getEntryCount());
    ASSERT_EQ(6ul, transitionMatrix.getRowGroupCount());
    ASSERT_EQ(7ul, transitionMatrix.getRowGroupIndices().size());
    ASSERT_EQ(CHOICE_COUNT, result.markovianChoices.size());
    ASSERT_EQ(STATE_COUNT, result.markovianStates.size());
    ASSERT_EQ(2ul, result.markovianStates.getNumberOfSetBits());
    ASSERT_EQ(STATE_COUNT, result.exitRates.size());

    // Test the general structure of the transition system (that will be an Markov automaton).

    // Test the mapping between states and transition matrix rows.
    ASSERT_EQ(0ul, transitionMatrix.getRowGroupIndices()[0]);
    ASSERT_EQ(1ul, transitionMatrix.getRowGroupIndices()[1]);
    ASSERT_EQ(2ul, transitionMatrix.getRowGroupIndices()[2]);
    ASSERT_EQ(3ul, transitionMatrix.getRowGroupIndices()[3]);
    ASSERT_EQ(4ul, transitionMatrix.getRowGroupIndices()[4]);
    ASSERT_EQ(6ul, transitionMatrix.getRowGroupIndices()[5]);
    ASSERT_EQ(7ul, transitionMatrix.getRowGroupIndices()[6]);

    // Test the Markovian states.
    ASSERT_TRUE(result.markovianStates.get(0));
    ASSERT_FALSE(result.markovianStates.get(1));
    ASSERT_TRUE(result.markovianStates.get(2));
    ASSERT_FALSE(result.markovianStates.get(3));
    ASSERT_FALSE(result.markovianStates.get(4));
    ASSERT_FALSE(result.markovianStates.get(5));

    // Test the exit rates. These have to be 0 for all non-Markovian states.
    ASSERT_EQ(2, result.exitRates[0]);
    ASSERT_EQ(0, result.exitRates[1]);
    ASSERT_EQ(15, result.exitRates[2]);
    ASSERT_EQ(0, result.exitRates[3]);
    ASSERT_EQ(0, result.exitRates[4]);
    ASSERT_EQ(0, result.exitRates[5]);

    // Finally, test the transition matrix itself.
    storm::storage::SparseMatrix<double>::const_iterator cIter = transitionMatrix.begin(0);

    ASSERT_EQ(2, cIter->getValue());
    cIter++;
    ASSERT_EQ(1, cIter->getValue());
    cIter++;
    ASSERT_EQ(1, cIter->getValue());
    cIter++;
    ASSERT_EQ(2, cIter->getValue());
    cIter++;
    ASSERT_EQ(4, cIter->getValue());
    cIter++;
    ASSERT_EQ(8, cIter->getValue());
    cIter++;
    ASSERT_EQ(0.5, cIter->getValue());
    cIter++;
    ASSERT_EQ(0.5, cIter->getValue());
    cIter++;
    ASSERT_EQ(1, cIter->getValue());
    cIter++;
    ASSERT_EQ(0.5, cIter->getValue());
    cIter++;
    ASSERT_EQ(0.5, cIter->getValue());
    cIter++;
    ASSERT_EQ(1, cIter->getValue());
    cIter++;
    ASSERT_EQ(transitionMatrix.end(), cIter);
}

TEST(MarkovAutomatonSparseTransitionParserTest, Whitespaces) {
    // The file that will be used for the test.
    std::string filename = STORM_TEST_RESOURCES_DIR "/tra/ma_whitespaces.tra";

    // Execute the parser.
    typename storm::parser::MarkovAutomatonSparseTransitionParser<>::Result result =
        storm::parser::MarkovAutomatonSparseTransitionParser<>::parseMarkovAutomatonTransitions(filename);

    // Build the actual transition matrix.
    storm::storage::SparseMatrix<double> transitionMatrix(result.transitionMatrixBuilder.build());

    // Test all sizes and counts.
    ASSERT_EQ(STATE_COUNT, transitionMatrix.getColumnCount());
    ASSERT_EQ(CHOICE_COUNT, transitionMatrix.getRowCount());
    ASSERT_EQ(12ul, transitionMatrix.getEntryCount());
    ASSERT_EQ(6ul, transitionMatrix.getRowGroupCount());
    ASSERT_EQ(7ul, transitionMatrix.getRowGroupIndices().size());
    ASSERT_EQ(CHOICE_COUNT, result.markovianChoices.size());
    ASSERT_EQ(STATE_COUNT, result.markovianStates.size());
    ASSERT_EQ(2ul, result.markovianStates.getNumberOfSetBits());
    ASSERT_EQ(STATE_COUNT, result.exitRates.size());

    // Test the general structure of the transition system (that will be an Markov automaton).

    // Test the mapping between states and transition matrix rows.
    ASSERT_EQ(0ul, transitionMatrix.getRowGroupIndices()[0]);
    ASSERT_EQ(1ul, transitionMatrix.getRowGroupIndices()[1]);
    ASSERT_EQ(2ul, transitionMatrix.getRowGroupIndices()[2]);
    ASSERT_EQ(3ul, transitionMatrix.getRowGroupIndices()[3]);
    ASSERT_EQ(4ul, transitionMatrix.getRowGroupIndices()[4]);
    ASSERT_EQ(6ul, transitionMatrix.getRowGroupIndices()[5]);
    ASSERT_EQ(7ul, transitionMatrix.getRowGroupIndices()[6]);

    // Test the Markovian states.
    ASSERT_TRUE(result.markovianStates.get(0));
    ASSERT_FALSE(result.markovianStates.get(1));
    ASSERT_TRUE(result.markovianStates.get(2));
    ASSERT_FALSE(result.markovianStates.get(3));
    ASSERT_FALSE(result.markovianStates.get(4));
    ASSERT_FALSE(result.markovianStates.get(5));

    // Test the exit rates. These have to be 0 for all non-Markovian states.
    ASSERT_EQ(2, result.exitRates[0]);
    ASSERT_EQ(0, result.exitRates[1]);
    ASSERT_EQ(15, result.exitRates[2]);
    ASSERT_EQ(0, result.exitRates[3]);
    ASSERT_EQ(0, result.exitRates[4]);
    ASSERT_EQ(0, result.exitRates[5]);

    // Finally, test the transition matrix itself.
    storm::storage::SparseMatrix<double>::const_iterator cIter = transitionMatrix.begin(0);

    ASSERT_EQ(2, cIter->getValue());
    cIter++;
    ASSERT_EQ(1, cIter->getValue());
    cIter++;
    ASSERT_EQ(1, cIter->getValue());
    cIter++;
    ASSERT_EQ(2, cIter->getValue());
    cIter++;
    ASSERT_EQ(4, cIter->getValue());
    cIter++;
    ASSERT_EQ(8, cIter->getValue());
    cIter++;
    ASSERT_EQ(0.5, cIter->getValue());
    cIter++;
    ASSERT_EQ(0.5, cIter->getValue());
    cIter++;
    ASSERT_EQ(1, cIter->getValue());
    cIter++;
    ASSERT_EQ(0.5, cIter->getValue());
    cIter++;
    ASSERT_EQ(0.5, cIter->getValue());
    cIter++;
    ASSERT_EQ(1, cIter->getValue());
    cIter++;
    ASSERT_EQ(transitionMatrix.end(), cIter);
}

TEST(MarkovAutomatonSparseTransitionParserTest, FixDeadlocks) {
    // Set the fixDeadlocks flag temporarily. It is set to its old value once the deadlockOption object is destructed.
    std::unique_ptr<storm::settings::SettingMemento> fixDeadlocks = storm::settings::mutableBuildSettings().overrideDontFixDeadlocksSet(false);

    // Parse a Markov Automaton transition file with the fixDeadlocks Flag set and test if it works.
    typename storm::parser::MarkovAutomatonSparseTransitionParser<>::Result result =
        storm::parser::MarkovAutomatonSparseTransitionParser<>::parseMarkovAutomatonTransitions(STORM_TEST_RESOURCES_DIR "/tra/ma_deadlock.tra");

    // Test if the result is consistent with the parsed Markov Automaton.
    storm::storage::SparseMatrix<double> resultMatrix(result.transitionMatrixBuilder.build());
    ASSERT_EQ(STATE_COUNT + 1, resultMatrix.getColumnCount());
    ASSERT_EQ(13ul, resultMatrix.getEntryCount());
    ASSERT_EQ(7ul, resultMatrix.getRowGroupCount());
    ASSERT_EQ(8ul, resultMatrix.getRowGroupIndices().size());
    ASSERT_EQ(CHOICE_COUNT + 1, result.markovianChoices.size());
    ASSERT_EQ(STATE_COUNT + 1, result.markovianStates.size());
    ASSERT_EQ(2ul, result.markovianStates.getNumberOfSetBits());
    ASSERT_EQ(STATE_COUNT + 1, result.exitRates.size());
}

TEST(MarkovAutomatonSparseTransitionParserTest, DontFixDeadlocks) {
    // Try to parse a Markov Automaton transition file containing a deadlock state with the fixDeadlocksFlag unset. This should throw an exception.
    std::unique_ptr<storm::settings::SettingMemento> dontFixDeadlocks = storm::settings::mutableBuildSettings().overrideDontFixDeadlocksSet(true);

    STORM_SILENT_ASSERT_THROW(
        storm::parser::MarkovAutomatonSparseTransitionParser<>::parseMarkovAutomatonTransitions(STORM_TEST_RESOURCES_DIR "/tra/ma_deadlock.tra"),
        storm::exceptions::WrongFormatException);
}
