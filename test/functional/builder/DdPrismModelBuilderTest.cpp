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
