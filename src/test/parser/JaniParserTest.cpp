#include "gtest/gtest.h"
#include "storm-config.h"
#include "storm/parser/JaniParser.h"
#include "storm/storage/jani/Model.h"
#include "storm/storage/jani/Property.h"


TEST(JaniParser, DieTest) {
    std::string testFileInput = STORM_TEST_RESOURCES_DIR"/../examples/exported-jani-models/dice.jani";
    storm::jani::Model model = storm::parser::JaniParser::parse(testFileInput).first;
}

TEST(JaniParser, BrpTest) {
    std::string testFileInput = STORM_TEST_RESOURCES_DIR"/../examples/exported-jani-models/brp.jani";
    storm::jani::Model model = storm::parser::JaniParser::parse(testFileInput).first;
}

TEST(JaniParser, ConsensusTest) {
    std::string testFileInput = STORM_TEST_RESOURCES_DIR"/../examples/exported-jani-models/coin2.jani";
    storm::jani::Model model = storm::parser::JaniParser::parse(testFileInput).first;
}
