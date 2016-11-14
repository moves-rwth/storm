#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/settings/SettingMemento.h"
#include "src/settings/SettingsManager.h"
#include "src/settings/modules/IOSettings.h"
#include "src/storage/SymbolicModelDescription.h"
#include "src/models/symbolic/Dtmc.h"
#include "src/models/symbolic/Ctmc.h"
#include "src/models/symbolic/Mdp.h"
#include "src/models/symbolic/StandardRewardModel.h"
#include "src/parser/PrismParser.h"
#include "src/builder/DdPrismModelBuilder.h"

TEST(DdPrismModelBuilderTest_Sylvan, Dtmc) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/die.pm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program);
    EXPECT_EQ(13ul, model->getNumberOfStates());
    EXPECT_EQ(20ul, model->getNumberOfTransitions());
    
    modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/brp-16-2.pm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program);
    EXPECT_EQ(677ul, model->getNumberOfStates());
    EXPECT_EQ(867ul, model->getNumberOfTransitions());
    
    modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/crowds-5-5.pm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program);
    EXPECT_EQ(8607ul, model->getNumberOfStates());
    EXPECT_EQ(15113ul, model->getNumberOfTransitions());

    modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/leader-3-5.pm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program);
    EXPECT_EQ(273ul, model->getNumberOfStates());
    EXPECT_EQ(397ul, model->getNumberOfTransitions());
    
    modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/nand-5-2.pm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program);
    EXPECT_EQ(1728ul, model->getNumberOfStates());
    EXPECT_EQ(2505ul, model->getNumberOfTransitions());
}

TEST(DdPrismModelBuilderTest_Cudd, Dtmc) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/die.pm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();

    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program);
    EXPECT_EQ(13ul, model->getNumberOfStates());
    EXPECT_EQ(20ul, model->getNumberOfTransitions());
        
    modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/brp-16-2.pm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program);
    EXPECT_EQ(677ul, model->getNumberOfStates());
    EXPECT_EQ(867ul, model->getNumberOfTransitions());

    modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/crowds-5-5.pm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program);
    EXPECT_EQ(8607ul, model->getNumberOfStates());
    EXPECT_EQ(15113ul, model->getNumberOfTransitions());
    
    modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/leader-3-5.pm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program);
    EXPECT_EQ(273ul, model->getNumberOfStates());
    EXPECT_EQ(397ul, model->getNumberOfTransitions());
    
    modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/nand-5-2.pm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program);
    EXPECT_EQ(1728ul, model->getNumberOfStates());
    EXPECT_EQ(2505ul, model->getNumberOfTransitions());
}

TEST(DdPrismModelBuilderTest_Sylvan, Ctmc) {
    // Set the PRISM compatibility mode temporarily. It is set to its old value once the returned object is destructed.
    std::unique_ptr<storm::settings::SettingMemento> enablePrismCompatibility = storm::settings::mutableIOSettings().overridePrismCompatibilityMode(true);
    
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/cluster2.sm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program);
    EXPECT_EQ(276ul, model->getNumberOfStates());
    EXPECT_EQ(1120ul, model->getNumberOfTransitions());
    
    modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/embedded2.sm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program);
    EXPECT_EQ(3478ul, model->getNumberOfStates());
    EXPECT_EQ(14639ul, model->getNumberOfTransitions());
    
    modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/polling2.sm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program);
    EXPECT_EQ(12ul, model->getNumberOfStates());
    EXPECT_EQ(22ul, model->getNumberOfTransitions());

    modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/fms2.sm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program);
    EXPECT_EQ(810ul, model->getNumberOfStates());
    EXPECT_EQ(3699ul, model->getNumberOfTransitions());
    
    modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/tandem5.sm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program);
    EXPECT_EQ(66ul, model->getNumberOfStates());
    EXPECT_EQ(189ul, model->getNumberOfTransitions());
}

TEST(DdPrismModelBuilderTest_Cudd, Ctmc) {
    // Set the PRISM compatibility mode temporarily. It is set to its old value once the returned object is destructed.
    std::unique_ptr<storm::settings::SettingMemento> enablePrismCompatibility = storm::settings::mutableIOSettings().overridePrismCompatibilityMode(true);

    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/cluster2.sm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program);
    EXPECT_EQ(276ul, model->getNumberOfStates());
    EXPECT_EQ(1120ul, model->getNumberOfTransitions());
    
    modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/embedded2.sm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program);
    EXPECT_EQ(3478ul, model->getNumberOfStates());
    EXPECT_EQ(14639ul, model->getNumberOfTransitions());
    
    modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/polling2.sm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program);
    EXPECT_EQ(12ul, model->getNumberOfStates());
    EXPECT_EQ(22ul, model->getNumberOfTransitions());
    
    modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/fms2.sm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program);
    EXPECT_EQ(810ul, model->getNumberOfStates());
    EXPECT_EQ(3699ul, model->getNumberOfTransitions());
    
    modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/tandem5.sm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program);
    EXPECT_EQ(66ul, model->getNumberOfStates());
    EXPECT_EQ(189ul, model->getNumberOfTransitions());
}

TEST(DdPrismModelBuilderTest_Sylvan, Mdp) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/two_dice.nm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program);
    
    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    std::shared_ptr<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>> mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>();
    
    EXPECT_EQ(169ul, mdp->getNumberOfStates());
    EXPECT_EQ(436ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(254ul, mdp->getNumberOfChoices());
    
    modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/leader3.nm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program);
    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>();
    
    EXPECT_EQ(364ul, mdp->getNumberOfStates());
    EXPECT_EQ(654ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(573ul, mdp->getNumberOfChoices());
    
    modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/coin2-2.nm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program);
    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>();
    
    EXPECT_EQ(272ul, mdp->getNumberOfStates());
    EXPECT_EQ(492ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(400ul, mdp->getNumberOfChoices());
    
    modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/csma2-2.nm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program);
    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>();
    
    EXPECT_EQ(1038ul, mdp->getNumberOfStates());
    EXPECT_EQ(1282ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(1054ul, mdp->getNumberOfChoices());
    
    modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/firewire3-0.5.nm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program);
    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>();
    
    EXPECT_EQ(4093ul, mdp->getNumberOfStates());
    EXPECT_EQ(5585ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(5519ul, mdp->getNumberOfChoices());

    modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/wlan0-2-2.nm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program);
    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>();
    
    EXPECT_EQ(37ul, mdp->getNumberOfStates());
    EXPECT_EQ(59ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(59ul, mdp->getNumberOfChoices());
}

TEST(DdPrismModelBuilderTest_Cudd, Mdp) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/two_dice.nm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program);

    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    std::shared_ptr<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>> mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>();

    EXPECT_EQ(169ul, mdp->getNumberOfStates());
    EXPECT_EQ(436ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(254ul, mdp->getNumberOfChoices());
    
    modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/leader3.nm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program);
    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>();
    
    EXPECT_EQ(364ul, mdp->getNumberOfStates());
    EXPECT_EQ(654ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(573ul, mdp->getNumberOfChoices());
    
    modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/coin2-2.nm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program);
    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>();
    
    EXPECT_EQ(272ul, mdp->getNumberOfStates());
    EXPECT_EQ(492ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(400ul, mdp->getNumberOfChoices());
    
    modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/csma2-2.nm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program);
    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>();
    
    EXPECT_EQ(1038ul, mdp->getNumberOfStates());
    EXPECT_EQ(1282ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(1054ul, mdp->getNumberOfChoices());
    
    modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/firewire3-0.5.nm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program);
    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>();
    
    EXPECT_EQ(4093ul, mdp->getNumberOfStates());
    EXPECT_EQ(5585ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(5519ul, mdp->getNumberOfChoices());

    modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/wlan0-2-2.nm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program);
    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>();
    
    EXPECT_EQ(37ul, mdp->getNumberOfStates());
    EXPECT_EQ(59ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(59ul, mdp->getNumberOfChoices());
}

TEST(DdPrismModelBuilderTest_Sylvan, Composition) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/system_composition.nm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program);
    
    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    std::shared_ptr<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>> mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>();
    
    EXPECT_EQ(21ul, mdp->getNumberOfStates());
    EXPECT_EQ(61ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(61ul, mdp->getNumberOfChoices());
    
    modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/system_composition2.nm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program);
    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>();
    
    EXPECT_EQ(8ul, mdp->getNumberOfStates());
    EXPECT_EQ(21ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(21ul, mdp->getNumberOfChoices());
}

TEST(DdPrismModelBuilderTest_Cudd, Composition) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/system_composition.nm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program);
    
    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    std::shared_ptr<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>> mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>();
    
    EXPECT_EQ(21ul, mdp->getNumberOfStates());
    EXPECT_EQ(61ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(61ul, mdp->getNumberOfChoices());
    
    modelDescription = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/system_composition2.nm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program);
    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>();
    
    EXPECT_EQ(8ul, mdp->getNumberOfStates());
    EXPECT_EQ(21ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(21ul, mdp->getNumberOfChoices());
}

