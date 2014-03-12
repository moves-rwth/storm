/*
 * AutoParserTest.cpp
 *
 *  Created on: Feb 10, 2014
 *      Author: Manuel Sascha Weiand
 */

#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/parser/AutoParser.h"
#include "src/exceptions/FileIoException.h"
#include "src/exceptions/WrongFormatException.h"

TEST(AutoParserTest, NonExistingFile) {
	// No matter what happens, please do NOT create a file with the name "nonExistingFile.not"!
	ASSERT_THROW(storm::parser::AutoParser::parseModel(STORM_CPP_TESTS_BASE_PATH "/nonExistingFile.not", STORM_CPP_TESTS_BASE_PATH "/nonExistingFile.not"), storm::exceptions::FileIoException);
}

TEST(AutoParserTest, BasicParsing) {
	// Parse model, which is a Dtmc.
	std::shared_ptr<storm::models::AbstractModel<double>> modelPtr = storm::parser::AutoParser::parseModel(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/autoParser/dtmc.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/autoParser.lab");

	// Test if parsed correctly.
	ASSERT_EQ(storm::models::DTMC, modelPtr->getType());
	ASSERT_EQ(12, modelPtr->getNumberOfStates());
	ASSERT_EQ(32, modelPtr->getNumberOfTransitions());
	ASSERT_EQ(1, modelPtr->getInitialStates().getNumberOfSetBits());
	ASSERT_TRUE(modelPtr->hasAtomicProposition("three"));
	ASSERT_FALSE(modelPtr->hasStateRewards());
	ASSERT_FALSE(modelPtr->hasTransitionRewards());
}

TEST(AutoParserTest, Whitespaces) {
	// Test different whitespace combinations by comparing the hash of the model parsed from files without whitespaces with the hash of the models parsed from files with whitespaces.
	uint_fast64_t correctHash = storm::parser::AutoParser::parseModel(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/autoParser/dtmc.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/autoParser.lab")->getHash();

	ASSERT_EQ(correctHash, storm::parser::AutoParser::parseModel(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/autoParser/dtmcWhitespaces1.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/autoParser.lab")->getHash());
	ASSERT_EQ(correctHash, storm::parser::AutoParser::parseModel(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/autoParser/dtmcWhitespaces2.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/autoParser.lab")->getHash());
}

TEST(AutoParserTest, WrongHint) {
	// The hint given describes the content but does not conform to the format.
	ASSERT_THROW(storm::parser::AutoParser::parseModel(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/autoParser/wrongHint.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/autoParser.lab"), storm::exceptions::WrongFormatException);
}

TEST(AutoParserTest, NoHint) {
	// There is no hint contained in the given file, so the parser cannot decide which kind of model it is.
	ASSERT_THROW(storm::parser::AutoParser::parseModel(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/autoParser/noHint.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/autoParser.lab"), storm::exceptions::WrongFormatException);
}

TEST(AutoParserTest, Decision) {
	// Test if the AutoParser recognizes each model kind and correctly parses it.

	// Dtmc
	std::shared_ptr<storm::models::AbstractModel<double>> modelPtr = storm::parser::AutoParser::parseModel(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/autoParser/dtmc.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/autoParser.lab");
	ASSERT_EQ(storm::models::DTMC, modelPtr->getType());
	ASSERT_EQ(12, modelPtr->getNumberOfStates());
	ASSERT_EQ(32, modelPtr->getNumberOfTransitions());

	// Ctmc
	modelPtr.reset();
	modelPtr = storm::parser::AutoParser::parseModel(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/autoParser/ctmc.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/autoParser.lab");
	ASSERT_EQ(storm::models::CTMC, modelPtr->getType());
	ASSERT_EQ(12, modelPtr->getNumberOfStates());
	ASSERT_EQ(31, modelPtr->getNumberOfTransitions());

	// Mdp
	modelPtr.reset();
	modelPtr = storm::parser::AutoParser::parseModel(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/autoParser/mdp.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/autoParser.lab");
	ASSERT_EQ(storm::models::MDP, modelPtr->getType());
	ASSERT_EQ(12, modelPtr->getNumberOfStates());
	ASSERT_EQ(36, modelPtr->getNumberOfTransitions());

	// Ctmdp
	// Note: For now we use the Mdp from above just given the ctmdp hint, since the implementation of the Ctmdp model seems not Quite right yet.
	//       We still do this test so that the code responsible for Ctmdps is executed at least once during testing.
	// TODO: Fix the Ctmdp implementation and use an actual Ctmdp for testing.
	modelPtr.reset();
	modelPtr = storm::parser::AutoParser::parseModel(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/autoParser/ctmdp.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/autoParser.lab");
	ASSERT_EQ(storm::models::CTMDP, modelPtr->getType());
	ASSERT_EQ(12, modelPtr->getNumberOfStates());
	ASSERT_EQ(36, modelPtr->getNumberOfTransitions());

	// MA
	modelPtr.reset();
	modelPtr = storm::parser::AutoParser::parseModel(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/autoParser/ma.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/autoParser.lab");
	ASSERT_EQ(storm::models::MA, modelPtr->getType());
	ASSERT_EQ(12, modelPtr->getNumberOfStates());
	ASSERT_EQ(35, modelPtr->getNumberOfTransitions());
}
