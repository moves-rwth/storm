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
	std::shared_ptr<storm::models::sparse::Model<double>> modelPtr = storm::parser::AutoParser::parseModel(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/autoParser/dtmc.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/autoParser.lab");

	// Test if parsed correctly.
    ASSERT_EQ(storm::models::ModelType::Dtmc, modelPtr->getType());
	ASSERT_EQ(12, modelPtr->getNumberOfStates());
	ASSERT_EQ(26, modelPtr->getNumberOfTransitions());
	ASSERT_EQ(1, modelPtr->getInitialStates().getNumberOfSetBits());
	ASSERT_TRUE(modelPtr->hasLabel("three"));
	ASSERT_FALSE(modelPtr->hasStateRewards());
	ASSERT_FALSE(modelPtr->hasTransitionRewards());
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
	std::shared_ptr<storm::models::sparse::Model<double>> modelPtr = storm::parser::AutoParser::parseModel(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/autoParser/dtmc.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/autoParser.lab");
	ASSERT_EQ(storm::models::ModelType::Dtmc, modelPtr->getType());
	ASSERT_EQ(12, modelPtr->getNumberOfStates());
	ASSERT_EQ(26, modelPtr->getNumberOfTransitions());

	// Ctmc
	modelPtr.reset();
	modelPtr = storm::parser::AutoParser::parseModel(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/autoParser/ctmc.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/autoParser.lab");
	ASSERT_EQ(storm::models::ModelType::Ctmc, modelPtr->getType());
	ASSERT_EQ(12, modelPtr->getNumberOfStates());
	ASSERT_EQ(26, modelPtr->getNumberOfTransitions());

	// Mdp
	modelPtr.reset();
	modelPtr = storm::parser::AutoParser::parseModel(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/autoParser/mdp.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/autoParser.lab");
	ASSERT_EQ(storm::models::ModelType::Mdp, modelPtr->getType());
	ASSERT_EQ(12, modelPtr->getNumberOfStates());
	ASSERT_EQ(28, modelPtr->getNumberOfTransitions());

	// MA
	modelPtr.reset();
	modelPtr = storm::parser::AutoParser::parseModel(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/autoParser/ma.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/autoParser.lab");
	ASSERT_EQ(storm::models::ModelType::MarkovAutomaton, modelPtr->getType());
	ASSERT_EQ(12, modelPtr->getNumberOfStates());
	ASSERT_EQ(27, modelPtr->getNumberOfTransitions());
}
