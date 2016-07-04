#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/parser/JaniParser.h"
#include "src/storage/jani/Model.h"


TEST(JaniParser, DieTest) {
    std::string testFileInput = STORM_CPP_TESTS_BASE_PATH"/../examples/exported-jani-models/dice.jani";
    storm::jani::Model model = storm::parser::JaniParser::parse(testFileInput);
}

TEST(JaniParser, BrpTest) {
    std::string testFileInput = STORM_CPP_TESTS_BASE_PATH"/../examples/exported-jani-models/brp.jani";
    storm::jani::Model model = storm::parser::JaniParser::parse(testFileInput);
}

TEST(JaniParser, ConsensusTest) {
    std::string testFileInput = STORM_CPP_TESTS_BASE_PATH"/../examples/exported-jani-models/coin2.jani";
    storm::jani::Model model = storm::parser::JaniParser::parse(testFileInput);
}