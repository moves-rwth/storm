#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/models/symbolic/Dtmc.h"
#include "src/models/symbolic/Ctmc.h"
#include "src/models/symbolic/Mdp.h"

#include "src/storage/dd/Add.h"
#include "src/storage/dd/Bdd.h"

#include "src/models/symbolic/StandardRewardModel.h"
#include "src/parser/PrismParser.h"
#include "src/builder/DdJaniModelBuilder.h"

#include "src/settings/SettingMemento.h"
#include "src/settings/SettingsManager.h"
#include "src/settings/modules/IOSettings.h"

TEST(DdJaniModelBuilderTest_Sylvan, Dtmc) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/die.pm");
    storm::jani::Model janiModel = program.toJani(true);
    
    storm::builder::DdJaniModelBuilder<storm::dd::DdType::Sylvan, double> builder(janiModel);
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = builder.translate();
    EXPECT_EQ(13ul, model->getNumberOfStates());
    EXPECT_EQ(20ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/brp-16-2.pm");
    janiModel = program.toJani(true);
    builder = storm::builder::DdJaniModelBuilder<storm::dd::DdType::Sylvan, double>(janiModel);
    model = builder.translate();
    EXPECT_EQ(677ul, model->getNumberOfStates());
    EXPECT_EQ(867ul, model->getNumberOfTransitions());
        
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/crowds-5-5.pm");
    janiModel = program.toJani(true);
    builder = storm::builder::DdJaniModelBuilder<storm::dd::DdType::Sylvan, double>(janiModel);
    model = builder.translate();
    EXPECT_EQ(8607ul, model->getNumberOfStates());
    EXPECT_EQ(15113ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/leader-3-5.pm");
    janiModel = program.toJani(true);
    builder = storm::builder::DdJaniModelBuilder<storm::dd::DdType::Sylvan, double>(janiModel);
    model = builder.translate();
    EXPECT_EQ(273ul, model->getNumberOfStates());
    EXPECT_EQ(397ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/nand-5-2.pm");
    janiModel = program.toJani(true);
    
    builder = storm::builder::DdJaniModelBuilder<storm::dd::DdType::Sylvan, double>(janiModel);
    model = builder.translate();
    EXPECT_EQ(1728ul, model->getNumberOfStates());
    EXPECT_EQ(2505ul, model->getNumberOfTransitions());
}

TEST(DdJaniModelBuilderTest_Cudd, Dtmc) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/die.pm");
    storm::jani::Model janiModel = program.toJani(true);
    
    storm::builder::DdJaniModelBuilder<storm::dd::DdType::CUDD, double> builder(janiModel);
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = builder.translate();
    EXPECT_EQ(13ul, model->getNumberOfStates());
    EXPECT_EQ(20ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/brp-16-2.pm");
    janiModel = program.toJani(true);
    builder = storm::builder::DdJaniModelBuilder<storm::dd::DdType::CUDD, double>(janiModel);
    model = builder.translate();
    EXPECT_EQ(677ul, model->getNumberOfStates());
    EXPECT_EQ(867ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/crowds-5-5.pm");
    janiModel = program.toJani(true);
    builder = storm::builder::DdJaniModelBuilder<storm::dd::DdType::CUDD, double>(janiModel);
    model = builder.translate();
    EXPECT_EQ(8607ul, model->getNumberOfStates());
    EXPECT_EQ(15113ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/leader-3-5.pm");
    janiModel = program.toJani(true);
    builder = storm::builder::DdJaniModelBuilder<storm::dd::DdType::CUDD, double>(janiModel);
    model = builder.translate();
    EXPECT_EQ(273ul, model->getNumberOfStates());
    EXPECT_EQ(397ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/nand-5-2.pm");
    janiModel = program.toJani(true);
    builder = storm::builder::DdJaniModelBuilder<storm::dd::DdType::CUDD, double>(janiModel);
    model = builder.translate();
    EXPECT_EQ(1728ul, model->getNumberOfStates());
    EXPECT_EQ(2505ul, model->getNumberOfTransitions());
}

TEST(DdJaniModelBuilderTest_Sylvan, Ctmc) {
    // Set the PRISM compatibility mode temporarily. It is set to its old value once the returned object is destructed.
    std::unique_ptr<storm::settings::SettingMemento> enablePrismCompatibility = storm::settings::mutableIOSettings().overridePrismCompatibilityMode(true);
    
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/cluster2.sm");
    storm::jani::Model janiModel = program.toJani(true);
    storm::builder::DdJaniModelBuilder<storm::dd::DdType::Sylvan, double> builder(janiModel);
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = builder.translate();
    EXPECT_EQ(276ul, model->getNumberOfStates());
    EXPECT_EQ(1120ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/embedded2.sm");
    janiModel = program.toJani(true);
    builder = storm::builder::DdJaniModelBuilder<storm::dd::DdType::Sylvan, double>(janiModel);
    model = builder.translate();
    EXPECT_EQ(3478ul, model->getNumberOfStates());
    EXPECT_EQ(14639ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/polling2.sm");
    janiModel = program.toJani(true);
    builder = storm::builder::DdJaniModelBuilder<storm::dd::DdType::Sylvan, double>(janiModel);
    model = builder.translate();
    EXPECT_EQ(12ul, model->getNumberOfStates());
    EXPECT_EQ(22ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/fms2.sm");
    janiModel = program.toJani(true);
    builder = storm::builder::DdJaniModelBuilder<storm::dd::DdType::Sylvan, double>(janiModel);
    model = builder.translate();
    EXPECT_EQ(810ul, model->getNumberOfStates());
    EXPECT_EQ(3699ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/tandem5.sm");
    janiModel = program.toJani(true);
    builder = storm::builder::DdJaniModelBuilder<storm::dd::DdType::Sylvan, double>(janiModel);
    model = builder.translate();
    EXPECT_EQ(66ul, model->getNumberOfStates());
    EXPECT_EQ(189ul, model->getNumberOfTransitions());
}

TEST(DdJaniModelBuilderTest_Cudd, Ctmc) {
    // Set the PRISM compatibility mode temporarily. It is set to its old value once the returned object is destructed.
    std::unique_ptr<storm::settings::SettingMemento> enablePrismCompatibility = storm::settings::mutableIOSettings().overridePrismCompatibilityMode(true);
    
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/cluster2.sm");
    storm::jani::Model janiModel = program.toJani(true);
    storm::builder::DdJaniModelBuilder<storm::dd::DdType::CUDD, double> builder(janiModel);
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = builder.translate();
    EXPECT_EQ(276ul, model->getNumberOfStates());
    EXPECT_EQ(1120ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/embedded2.sm");
    janiModel = program.toJani(true);
    builder = storm::builder::DdJaniModelBuilder<storm::dd::DdType::CUDD, double>(janiModel);
    model = builder.translate();
    EXPECT_EQ(3478ul, model->getNumberOfStates());
    EXPECT_EQ(14639ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/polling2.sm");
    janiModel = program.toJani(true);
    builder = storm::builder::DdJaniModelBuilder<storm::dd::DdType::CUDD, double>(janiModel);
    model = builder.translate();
    EXPECT_EQ(12ul, model->getNumberOfStates());
    EXPECT_EQ(22ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/fms2.sm");
    janiModel = program.toJani(true);
    builder = storm::builder::DdJaniModelBuilder<storm::dd::DdType::CUDD, double>(janiModel);
    model = builder.translate();
    EXPECT_EQ(810ul, model->getNumberOfStates());
    EXPECT_EQ(3699ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/tandem5.sm");
    janiModel = program.toJani(true);
    builder = storm::builder::DdJaniModelBuilder<storm::dd::DdType::CUDD, double>(janiModel);
    model = builder.translate();
    EXPECT_EQ(66ul, model->getNumberOfStates());
    EXPECT_EQ(189ul, model->getNumberOfTransitions());
}