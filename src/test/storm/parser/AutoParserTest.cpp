#include "storm-config.h"
#include "test/storm_gtest.h"

#include "storm-parsers/parser/AutoParser.h"
#include "storm/exceptions/FileIoException.h"
#include "storm/exceptions/WrongFormatException.h"
#include "storm/models/sparse/StandardRewardModel.h"

TEST(AutoParserTest, NonExistingFile) {
    // No matter what happens, please do NOT create a file with the name "nonExistingFile.not"!
    STORM_SILENT_ASSERT_THROW(
        storm::parser::AutoParser<>::parseModel(STORM_TEST_RESOURCES_DIR "/nonExistingFile.not", STORM_TEST_RESOURCES_DIR "/nonExistingFile.not"),
        storm::exceptions::FileIoException);
}

TEST(AutoParserTest, BasicParsing) {
    // Parse model, which is a Dtmc.
    std::shared_ptr<storm::models::sparse::Model<double>> modelPtr =
        storm::parser::AutoParser<>::parseModel(STORM_TEST_RESOURCES_DIR "/tra/dtmc.tra", STORM_TEST_RESOURCES_DIR "/lab/autoParser.lab");

    // Test if parsed correctly.
    ASSERT_EQ(storm::models::ModelType::Dtmc, modelPtr->getType());
    ASSERT_EQ(12ul, modelPtr->getNumberOfStates());
    ASSERT_EQ(26ul, modelPtr->getNumberOfTransitions());
    ASSERT_EQ(1ul, modelPtr->getInitialStates().getNumberOfSetBits());
    ASSERT_TRUE(modelPtr->hasLabel("three"));
    ASSERT_FALSE(modelPtr->hasRewardModel());
}

TEST(AutoParserTest, WrongHint) {
    // The hint given describes the content but does not conform to the format.
    STORM_SILENT_ASSERT_THROW(
        storm::parser::AutoParser<>::parseModel(STORM_TEST_RESOURCES_DIR "/tra/wrongHint.tra", STORM_TEST_RESOURCES_DIR "/lab/autoParser.lab"),
        storm::exceptions::WrongFormatException);
}

TEST(AutoParserTest, NoHint) {
    // There is no hint contained in the given file, so the parser cannot decide which kind of model it is.
    STORM_SILENT_ASSERT_THROW(
        storm::parser::AutoParser<>::parseModel(STORM_TEST_RESOURCES_DIR "/tra/noHint.tra", STORM_TEST_RESOURCES_DIR "/lab/autoParser.lab"),
        storm::exceptions::WrongFormatException);
}

TEST(AutoParserTest, Decision) {
    // Test if the AutoParser recognizes each model kind and correctly parses it.

    // Dtmc
    std::shared_ptr<storm::models::sparse::Model<double>> modelPtr =
        storm::parser::AutoParser<>::parseModel(STORM_TEST_RESOURCES_DIR "/tra/dtmc.tra", STORM_TEST_RESOURCES_DIR "/lab/autoParser.lab");
    ASSERT_EQ(storm::models::ModelType::Dtmc, modelPtr->getType());
    ASSERT_EQ(12ul, modelPtr->getNumberOfStates());
    ASSERT_EQ(26ul, modelPtr->getNumberOfTransitions());

    // Ctmc
    modelPtr.reset();
    modelPtr = storm::parser::AutoParser<>::parseModel(STORM_TEST_RESOURCES_DIR "/tra/ctmc.tra", STORM_TEST_RESOURCES_DIR "/lab/autoParser.lab");
    ASSERT_EQ(storm::models::ModelType::Ctmc, modelPtr->getType());
    ASSERT_EQ(12ul, modelPtr->getNumberOfStates());
    ASSERT_EQ(26ul, modelPtr->getNumberOfTransitions());

    // Mdp
    modelPtr.reset();
    modelPtr = storm::parser::AutoParser<>::parseModel(STORM_TEST_RESOURCES_DIR "/tra/mdp.tra", STORM_TEST_RESOURCES_DIR "/lab/autoParser.lab");
    ASSERT_EQ(storm::models::ModelType::Mdp, modelPtr->getType());
    ASSERT_EQ(12ul, modelPtr->getNumberOfStates());
    ASSERT_EQ(28ul, modelPtr->getNumberOfTransitions());

    // MA
    modelPtr.reset();
    modelPtr = storm::parser::AutoParser<>::parseModel(STORM_TEST_RESOURCES_DIR "/tra/ma.tra", STORM_TEST_RESOURCES_DIR "/lab/autoParser.lab");
    ASSERT_EQ(storm::models::ModelType::MarkovAutomaton, modelPtr->getType());
    ASSERT_EQ(12ul, modelPtr->getNumberOfStates());
    ASSERT_EQ(27ul, modelPtr->getNumberOfTransitions());
}
