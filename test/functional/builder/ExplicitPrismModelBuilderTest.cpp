#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/settings/SettingMemento.h"
#include "src/parser/PrismParser.h"
#include "src/builder/ExplicitPrismModelBuilder.h"

#include "src/settings/modules/IOSettings.h"

TEST(ExplicitPrismModelBuilderTest, Dtmc) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/die.pm");
    
    std::shared_ptr<storm::models::sparse::Model<double>> model = storm::builder::ExplicitPrismModelBuilder<double>(program).translate();
    EXPECT_EQ(13ul, model->getNumberOfStates());
    EXPECT_EQ(20ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/brp-16-2.pm");
    model = storm::builder::ExplicitPrismModelBuilder<double>(program).translate();
    EXPECT_EQ(677ul, model->getNumberOfStates());
    EXPECT_EQ(867ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/crowds-5-5.pm");
    model = storm::builder::ExplicitPrismModelBuilder<double>(program).translate();
    EXPECT_EQ(8607ul, model->getNumberOfStates());
    EXPECT_EQ(15113ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/leader-3-5.pm");
    model = storm::builder::ExplicitPrismModelBuilder<double>(program).translate();
    EXPECT_EQ(273ul, model->getNumberOfStates());
    EXPECT_EQ(397ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/nand-5-2.pm");
    model = storm::builder::ExplicitPrismModelBuilder<double>(program).translate();
    EXPECT_EQ(1728ul, model->getNumberOfStates());
    EXPECT_EQ(2505ul, model->getNumberOfTransitions());
}

TEST(ExplicitPrismModelBuilderTest, Ctmc) {
    // Set the PRISM compatibility mode temporarily. It is set to its old value once the returned object is destructed.
    std::unique_ptr<storm::settings::SettingMemento> enablePrismCompatibility = storm::settings::mutableIOSettings().overridePrismCompatibilityMode(true);

    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/cluster2.sm");

    std::shared_ptr<storm::models::sparse::Model<double>> model = storm::builder::ExplicitPrismModelBuilder<double>(program).translate();
    EXPECT_EQ(276ul, model->getNumberOfStates());
    EXPECT_EQ(1120ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/embedded2.sm");
    model = storm::builder::ExplicitPrismModelBuilder<double>(program).translate();
    EXPECT_EQ(3478ul, model->getNumberOfStates());
    EXPECT_EQ(14639ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/polling2.sm");
    model = storm::builder::ExplicitPrismModelBuilder<double>(program).translate();
    EXPECT_EQ(12ul, model->getNumberOfStates());
    EXPECT_EQ(22ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/fms2.sm");
    model = storm::builder::ExplicitPrismModelBuilder<double>(program).translate();
    EXPECT_EQ(810ul, model->getNumberOfStates());
    EXPECT_EQ(3699ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/tandem5.sm");
    model = storm::builder::ExplicitPrismModelBuilder<double>(program).translate();
    EXPECT_EQ(66ul, model->getNumberOfStates());
    EXPECT_EQ(189ul, model->getNumberOfTransitions());
}

TEST(ExplicitPrismModelBuilderTest, Mdp) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/two_dice.nm");
    
    std::shared_ptr<storm::models::sparse::Model<double>> model = storm::builder::ExplicitPrismModelBuilder<double>(program).translate();
    EXPECT_EQ(169ul, model->getNumberOfStates());
    EXPECT_EQ(436ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/leader3.nm");
    model = storm::builder::ExplicitPrismModelBuilder<double>(program).translate();
    EXPECT_EQ(364ul, model->getNumberOfStates());
    EXPECT_EQ(654ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/coin2-2.nm");
    model = storm::builder::ExplicitPrismModelBuilder<double>(program).translate();
    EXPECT_EQ(272ul, model->getNumberOfStates());
    EXPECT_EQ(492ul, model->getNumberOfTransitions());

    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/csma2-2.nm");
    model = storm::builder::ExplicitPrismModelBuilder<double>(program).translate();
    EXPECT_EQ(1038ul, model->getNumberOfStates());
    EXPECT_EQ(1282ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/firewire3-0.5.nm");
    model = storm::builder::ExplicitPrismModelBuilder<double>(program).translate();
    EXPECT_EQ(4093ul, model->getNumberOfStates());
    EXPECT_EQ(5585ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/wlan0-2-2.nm");
    model = storm::builder::ExplicitPrismModelBuilder<double>(program).translate();
    EXPECT_EQ(37ul, model->getNumberOfStates());
    EXPECT_EQ(59ul, model->getNumberOfTransitions());
}

TEST(ExplicitPrismModelBuilderTest, Fail) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/system_composition.nm");

    ASSERT_THROW(storm::builder::ExplicitPrismModelBuilder<double>(program).translate(), storm::exceptions::WrongFormatException);
}
