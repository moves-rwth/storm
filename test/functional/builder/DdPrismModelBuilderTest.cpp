#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/settings/SettingMemento.h"
#include "src/settings/SettingsManager.h"
#include "src/settings/modules/IOSettings.h"
#include "src/models/symbolic/Dtmc.h"
#include "src/models/symbolic/Ctmc.h"
#include "src/models/symbolic/Mdp.h"
#include "src/models/symbolic/StandardRewardModel.h"
#include "src/parser/PrismParser.h"
#include "src/builder/DdPrismModelBuilder.h"

TEST(DdPrismModelBuilderTest_Sylvan, Dtmc) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/die.pm");
    
    auto t1 = std::chrono::high_resolution_clock::now();
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().translateProgram(program);
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << "die: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << std::endl;
    EXPECT_EQ(13ul, model->getNumberOfStates());
    EXPECT_EQ(20ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/brp-16-2.pm");
    t1 = std::chrono::high_resolution_clock::now();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().translateProgram(program);
    t2 = std::chrono::high_resolution_clock::now();
    std::cout << "brp: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << std::endl;
    EXPECT_EQ(677ul, model->getNumberOfStates());
    EXPECT_EQ(867ul, model->getNumberOfTransitions());

    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/crowds-5-5.pm");
    t1 = std::chrono::high_resolution_clock::now();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().translateProgram(program);
    t2 = std::chrono::high_resolution_clock::now();
    std::cout << "crowds: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << std::endl;
    EXPECT_EQ(8607ul, model->getNumberOfStates());
    EXPECT_EQ(15113ul, model->getNumberOfTransitions());

    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/leader-3-5.pm");
    t1 = std::chrono::high_resolution_clock::now();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().translateProgram(program);
    t2 = std::chrono::high_resolution_clock::now();
    std::cout << "lead: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << std::endl;
    EXPECT_EQ(273ul, model->getNumberOfStates());
    EXPECT_EQ(397ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/nand-5-2.pm");
    t1 = std::chrono::high_resolution_clock::now();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().translateProgram(program);
    t2 = std::chrono::high_resolution_clock::now();
    std::cout << "nand: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << std::endl;
    EXPECT_EQ(1728ul, model->getNumberOfStates());
    EXPECT_EQ(2505ul, model->getNumberOfTransitions());
}

TEST(DdPrismModelBuilderTest_Cudd, Dtmc) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/die.pm");
    
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().translateProgram(program);
    model->getTransitionMatrix().exportToDot("trans_prism.dot");
    std::cout << "nnz: " << model->getTransitionMatrix().getNonZeroCount() << std::endl;
    std::cout << "nodes: " << model->getTransitionMatrix().getNodeCount() << std::endl;
    std::cout << "vars: " << model->getTransitionMatrix().getContainedMetaVariables().size() << std::endl;
    EXPECT_EQ(13ul, model->getNumberOfStates());
    EXPECT_EQ(20ul, model->getNumberOfTransitions());
        
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/brp-16-2.pm");
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().translateProgram(program);
    EXPECT_EQ(677ul, model->getNumberOfStates());
    EXPECT_EQ(867ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/crowds-5-5.pm");
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().translateProgram(program);
    EXPECT_EQ(8607ul, model->getNumberOfStates());
    EXPECT_EQ(15113ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/leader-3-5.pm");
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().translateProgram(program);
    EXPECT_EQ(273ul, model->getNumberOfStates());
    EXPECT_EQ(397ul, model->getNumberOfTransitions());
        
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/nand-5-2.pm");
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().translateProgram(program);
    EXPECT_EQ(1728ul, model->getNumberOfStates());
    EXPECT_EQ(2505ul, model->getNumberOfTransitions());
}

TEST(DdPrismModelBuilderTest_Sylvan, Ctmc) {
    // Set the PRISM compatibility mode temporarily. It is set to its old value once the returned object is destructed.
    std::unique_ptr<storm::settings::SettingMemento> enablePrismCompatibility = storm::settings::mutableIOSettings().overridePrismCompatibilityMode(true);
    
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/cluster2.sm");
    
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().translateProgram(program);
    EXPECT_EQ(276ul, model->getNumberOfStates());
    EXPECT_EQ(1120ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/embedded2.sm");
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().translateProgram(program);
    EXPECT_EQ(3478ul, model->getNumberOfStates());
    EXPECT_EQ(14639ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/polling2.sm");
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().translateProgram(program);
    EXPECT_EQ(12ul, model->getNumberOfStates());
    EXPECT_EQ(22ul, model->getNumberOfTransitions());

    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/fms2.sm");
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().translateProgram(program);
    EXPECT_EQ(810ul, model->getNumberOfStates());
    EXPECT_EQ(3699ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/tandem5.sm");
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().translateProgram(program);
    EXPECT_EQ(66ul, model->getNumberOfStates());
    EXPECT_EQ(189ul, model->getNumberOfTransitions());
}

TEST(DdPrismModelBuilderTest_Cudd, Ctmc) {
    // Set the PRISM compatibility mode temporarily. It is set to its old value once the returned object is destructed.
    std::unique_ptr<storm::settings::SettingMemento> enablePrismCompatibility = storm::settings::mutableIOSettings().overridePrismCompatibilityMode(true);

    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/cluster2.sm");
    
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().translateProgram(program);
    EXPECT_EQ(276ul, model->getNumberOfStates());
    EXPECT_EQ(1120ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/embedded2.sm");
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().translateProgram(program);
    EXPECT_EQ(3478ul, model->getNumberOfStates());
    EXPECT_EQ(14639ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/polling2.sm");
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().translateProgram(program);
    EXPECT_EQ(12ul, model->getNumberOfStates());
    EXPECT_EQ(22ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/fms2.sm");
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().translateProgram(program);
    EXPECT_EQ(810ul, model->getNumberOfStates());
    EXPECT_EQ(3699ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/tandem5.sm");
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().translateProgram(program);
    EXPECT_EQ(66ul, model->getNumberOfStates());
    EXPECT_EQ(189ul, model->getNumberOfTransitions());
}

TEST(DdPrismModelBuilderTest_Sylvan, Mdp) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/two_dice.nm");
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().translateProgram(program);
    
    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    std::shared_ptr<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>> mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>();
    
    EXPECT_EQ(169ul, mdp->getNumberOfStates());
    EXPECT_EQ(436ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(254ul, mdp->getNumberOfChoices());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/leader3.nm");
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().translateProgram(program);
    
    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>();
    
    EXPECT_EQ(364ul, mdp->getNumberOfStates());
    EXPECT_EQ(654ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(573ul, mdp->getNumberOfChoices());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/coin2-2.nm");
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().translateProgram(program);
    
    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>();
    
    EXPECT_EQ(272ul, mdp->getNumberOfStates());
    EXPECT_EQ(492ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(400ul, mdp->getNumberOfChoices());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/csma2-2.nm");
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().translateProgram(program);
    
    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>();
    
    EXPECT_EQ(1038ul, mdp->getNumberOfStates());
    EXPECT_EQ(1282ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(1054ul, mdp->getNumberOfChoices());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/firewire3-0.5.nm");
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().translateProgram(program);
    
    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>();
    
    EXPECT_EQ(4093ul, mdp->getNumberOfStates());
    EXPECT_EQ(5585ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(5519ul, mdp->getNumberOfChoices());

    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/wlan0-2-2.nm");
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().translateProgram(program);
    
    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>();
    
    EXPECT_EQ(37ul, mdp->getNumberOfStates());
    EXPECT_EQ(59ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(59ul, mdp->getNumberOfChoices());
}

TEST(DdPrismModelBuilderTest_Cudd, Mdp) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/two_dice.nm");
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().translateProgram(program);
    
    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    std::shared_ptr<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>> mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>();

    EXPECT_EQ(169ul, mdp->getNumberOfStates());
    EXPECT_EQ(436ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(254ul, mdp->getNumberOfChoices());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/leader3.nm");
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().translateProgram(program);

    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>();
    
    EXPECT_EQ(364ul, mdp->getNumberOfStates());
    EXPECT_EQ(654ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(573ul, mdp->getNumberOfChoices());

    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/coin2-2.nm");
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().translateProgram(program);
    
    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>();
    
    EXPECT_EQ(272ul, mdp->getNumberOfStates());
    EXPECT_EQ(492ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(400ul, mdp->getNumberOfChoices());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/csma2-2.nm");
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().translateProgram(program);
    
    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>();
    
    EXPECT_EQ(1038ul, mdp->getNumberOfStates());
    EXPECT_EQ(1282ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(1054ul, mdp->getNumberOfChoices());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/firewire3-0.5.nm");
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().translateProgram(program);
    
    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>();
    
    EXPECT_EQ(4093ul, mdp->getNumberOfStates());
    EXPECT_EQ(5585ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(5519ul, mdp->getNumberOfChoices());

    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/wlan0-2-2.nm");
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().translateProgram(program);
    
    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>();
    
    EXPECT_EQ(37ul, mdp->getNumberOfStates());
    EXPECT_EQ(59ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(59ul, mdp->getNumberOfChoices());
}

TEST(DdPrismModelBuilderTest_Sylvan, Composition) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/system_composition.nm");
    
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().translateProgram(program);
    
    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    std::shared_ptr<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>> mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>();
    
    EXPECT_EQ(21ul, mdp->getNumberOfStates());
    EXPECT_EQ(61ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(61ul, mdp->getNumberOfChoices());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/system_composition2.nm");
    
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().translateProgram(program);
    
    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>();
    
    EXPECT_EQ(8ul, mdp->getNumberOfStates());
    EXPECT_EQ(21ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(21ul, mdp->getNumberOfChoices());
}

TEST(DdPrismModelBuilderTest_Cudd, Composition) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/system_composition.nm");
    
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().translateProgram(program);
    
    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    std::shared_ptr<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>> mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>();
    
    EXPECT_EQ(21ul, mdp->getNumberOfStates());
    EXPECT_EQ(61ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(61ul, mdp->getNumberOfChoices());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/system_composition2.nm");
    
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().translateProgram(program);
    
    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>();
    
    EXPECT_EQ(8ul, mdp->getNumberOfStates());
    EXPECT_EQ(21ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(21ul, mdp->getNumberOfChoices());
}

