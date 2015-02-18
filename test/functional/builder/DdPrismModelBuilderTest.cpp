#include "gtest/gtest.h"
#include "storm-config.h"

#include <utility>

#include "src/storage/dd/CuddDd.h"
#include "src/parser/PrismParser.h"
#include "src/builder/DdPrismModelBuilder.h"

TEST(DdPrismModelBuilderTest, Dtmc) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/die.pm");
    
    std::pair<storm::dd::Dd<storm::dd::DdType::CUDD>, storm::dd::Dd<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::translateProgram(program);
    EXPECT_EQ(13, model.first.getNonZeroCount());
    EXPECT_EQ(20, model.second.getNonZeroCount());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/brp-16-2.pm");
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::translateProgram(program);
    EXPECT_EQ(677, model.first.getNonZeroCount());
    EXPECT_EQ(867, model.second.getNonZeroCount());

    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/crowds-5-5.pm");
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::translateProgram(program);
    EXPECT_EQ(8607, model.first.getNonZeroCount());
    EXPECT_EQ(15113, model.second.getNonZeroCount());

    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/leader-3-5.pm");
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::translateProgram(program);
    EXPECT_EQ(273, model.first.getNonZeroCount());
    EXPECT_EQ(397, model.second.getNonZeroCount());

    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/nand-5-2.pm");
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::translateProgram(program);
    EXPECT_EQ(1728, model.first.getNonZeroCount());
    EXPECT_EQ(2505, model.second.getNonZeroCount());
}

TEST(DdPrismModelBuilderTest, Mdp) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/two_dice.nm");
    std::pair<storm::dd::Dd<storm::dd::DdType::CUDD>, storm::dd::Dd<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::translateProgram(program);
    EXPECT_EQ(169, model.first.getNonZeroCount());
    EXPECT_EQ(436, model.second.getNonZeroCount());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/leader3.nm");
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::translateProgram(program);
    EXPECT_EQ(364, model.first.getNonZeroCount());
    EXPECT_EQ(654, model.second.getNonZeroCount());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/coin2-2.nm");
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::translateProgram(program);
    EXPECT_EQ(272, model.first.getNonZeroCount());
    EXPECT_EQ(492, model.second.getNonZeroCount());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/csma2-2.nm");
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::translateProgram(program);
    EXPECT_EQ(1038, model.first.getNonZeroCount());
    EXPECT_EQ(1282, model.second.getNonZeroCount());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/firewire3-0.5.nm");
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::translateProgram(program);
    EXPECT_EQ(4093, model.first.getNonZeroCount());
    EXPECT_EQ(5585, model.second.getNonZeroCount());

    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/wlan0-2-2.nm");
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::translateProgram(program);
    EXPECT_EQ(37, model.first.getNonZeroCount());
    EXPECT_EQ(59, model.second.getNonZeroCount());
}