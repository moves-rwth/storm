#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/parser/JaniParser.h"
#include "src/storage/jani/Model.h"


TEST(JaniParser, DieTest) {
    std::string testFileInput = STORM_CPP_TESTS_BASE_PATH"/../examples/jani-examples/dice.jani";
    storm::jani::Model model = storm::parser::JaniParser::parse(testFileInput);
}

TEST(JaniParser, BrpTest) {
    std::string testFileInput = STORM_CPP_TESTS_BASE_PATH"/../examples/jani-examples/brp.jani";
    storm::jani::Model model = storm::parser::JaniParser::parse(testFileInput);
}

TEST(JaniParser, ConsensusTest) {
    std::string testFileInput = STORM_CPP_TESTS_BASE_PATH"/../examples/jani-examples/consensus-6.jani";
    storm::jani::Model model = storm::parser::JaniParser::parse(testFileInput);
}