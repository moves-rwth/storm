#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/parser/PrismParser.h"
#include "src/builder/ExplicitPrismModelBuilder.h"

TEST(ExplicitPrismModelBuilderTest, Dtmc) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/die.pm");
    
    std::unique_ptr<storm::models::AbstractModel<double>> model = storm::builder::ExplicitPrismModelBuilder<double>::translateProgram(program);
    EXPECT_EQ(13, model->getNumberOfStates());
    EXPECT_EQ(20, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/brp-16-2.pm");
    model = storm::builder::ExplicitPrismModelBuilder<double>::translateProgram(program);
    EXPECT_EQ(677, model->getNumberOfStates());
    EXPECT_EQ(867, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/crowds-5-5.pm");
    model = storm::builder::ExplicitPrismModelBuilder<double>::translateProgram(program);
    EXPECT_EQ(8607, model->getNumberOfStates());
    EXPECT_EQ(15113, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/leader-3-5.pm");
    model = storm::builder::ExplicitPrismModelBuilder<double>::translateProgram(program);
    EXPECT_EQ(273, model->getNumberOfStates());
    EXPECT_EQ(397, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/nand-5-2.pm");
    model = storm::builder::ExplicitPrismModelBuilder<double>::translateProgram(program);
    EXPECT_EQ(1728, model->getNumberOfStates());
    EXPECT_EQ(2505, model->getNumberOfTransitions());
}
