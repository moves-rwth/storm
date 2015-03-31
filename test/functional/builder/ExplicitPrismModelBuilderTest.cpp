#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/settings/SettingMemento.h"
#include "src/parser/PrismParser.h"
#include "src/builder/ExplicitPrismModelBuilder.h"

TEST(ExplicitPrismModelBuilderTest, Dtmc) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/die.pm");
    
    std::shared_ptr<storm::models::sparse::Model<double>> model = storm::builder::ExplicitPrismModelBuilder<double>::translateProgram(program);
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

TEST(ExplicitPrismModelBuilderTest, Ctmc) {
    // Set the PRISM compatibility mode temporarily. It is set to its old value once the returned object is destructed.
    std::unique_ptr<storm::settings::SettingMemento> enablePrismCompatibility = storm::settings::mutableGeneralSettings().overridePrismCompatibilityMode(true);

    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/cluster2.sm");

    std::shared_ptr<storm::models::sparse::Model<double>> model = storm::builder::ExplicitPrismModelBuilder<double>::translateProgram(program);
    EXPECT_EQ(276, model->getNumberOfStates());
    EXPECT_EQ(1120, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/embedded2.sm");
    model = storm::builder::ExplicitPrismModelBuilder<double>::translateProgram(program);
    EXPECT_EQ(3478, model->getNumberOfStates());
    EXPECT_EQ(14639, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/polling2.sm");
    model = storm::builder::ExplicitPrismModelBuilder<double>::translateProgram(program);
    EXPECT_EQ(12, model->getNumberOfStates());
    EXPECT_EQ(22, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/fms2.sm");
    model = storm::builder::ExplicitPrismModelBuilder<double>::translateProgram(program);
    EXPECT_EQ(810, model->getNumberOfStates());
    EXPECT_EQ(3699, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/tandem5.sm");
    model = storm::builder::ExplicitPrismModelBuilder<double>::translateProgram(program);
    EXPECT_EQ(66, model->getNumberOfStates());
    EXPECT_EQ(189, model->getNumberOfTransitions());
}

TEST(ExplicitPrismModelBuilderTest, Mdp) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/two_dice.nm");
    
    std::shared_ptr<storm::models::sparse::Model<double>> model = storm::builder::ExplicitPrismModelBuilder<double>::translateProgram(program);
    EXPECT_EQ(169, model->getNumberOfStates());
    EXPECT_EQ(436, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/leader3.nm");
    model = storm::builder::ExplicitPrismModelBuilder<double>::translateProgram(program);
    EXPECT_EQ(364, model->getNumberOfStates());
    EXPECT_EQ(654, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/coin2-2.nm");
    model = storm::builder::ExplicitPrismModelBuilder<double>::translateProgram(program);
    EXPECT_EQ(272, model->getNumberOfStates());
    EXPECT_EQ(492, model->getNumberOfTransitions());

    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/csma2-2.nm");
    model = storm::builder::ExplicitPrismModelBuilder<double>::translateProgram(program);
    EXPECT_EQ(1038, model->getNumberOfStates());
    EXPECT_EQ(1282, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/firewire3-0.5.nm");
    model = storm::builder::ExplicitPrismModelBuilder<double>::translateProgram(program);
    EXPECT_EQ(4093, model->getNumberOfStates());
    EXPECT_EQ(5585, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/wlan0-2-2.nm");
    model = storm::builder::ExplicitPrismModelBuilder<double>::translateProgram(program);
    EXPECT_EQ(37, model->getNumberOfStates());
    EXPECT_EQ(59, model->getNumberOfTransitions());
}