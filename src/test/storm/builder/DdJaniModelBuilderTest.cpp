#include "storm-config.h"
#include "test/storm_gtest.h"

#include "storm/models/symbolic/Ctmc.h"
#include "storm/models/symbolic/Dtmc.h"
#include "storm/models/symbolic/Mdp.h"

#include "storm/storage/SymbolicModelDescription.h"
#include "storm/storage/dd/Add.h"
#include "storm/storage/dd/Bdd.h"
#include "storm/storage/jani/Compositions.h"

#include "storm-parsers/parser/JaniParser.h"
#include "storm-parsers/parser/PrismParser.h"
#include "storm/builder/DdJaniModelBuilder.h"
#include "storm/models/symbolic/StandardRewardModel.h"

#include "storm-parsers/api/model_descriptions.h"
#include "storm/exceptions/WrongFormatException.h"
#include "storm/settings/SettingMemento.h"
#include "storm/settings/SettingsManager.h"

TEST(DdJaniModelBuilderTest_Sylvan, Dtmc) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/die.pm");
    storm::jani::Model janiModel = modelDescription.toJani(true).preprocess().asJaniModel();

    storm::builder::DdJaniModelBuilder<storm::dd::DdType::Sylvan, double> builder;
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = builder.build(janiModel);
    EXPECT_EQ(13ul, model->getNumberOfStates());
    EXPECT_EQ(20ul, model->getNumberOfTransitions());

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/brp-16-2.pm");
    janiModel = modelDescription.toJani(true).preprocess().asJaniModel();
    model = builder.build(janiModel);
    EXPECT_EQ(677ul, model->getNumberOfStates());
    EXPECT_EQ(867ul, model->getNumberOfTransitions());

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/crowds-5-5.pm");
    janiModel = modelDescription.toJani(true).preprocess().asJaniModel();
    model = builder.build(janiModel);
    EXPECT_EQ(8607ul, model->getNumberOfStates());
    EXPECT_EQ(15113ul, model->getNumberOfTransitions());

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/leader-3-5.pm");
    janiModel = modelDescription.toJani(true).preprocess().asJaniModel();
    model = builder.build(janiModel);
    EXPECT_EQ(273ul, model->getNumberOfStates());
    EXPECT_EQ(397ul, model->getNumberOfTransitions());

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/nand-5-2.pm");
    janiModel = modelDescription.toJani(true).preprocess().asJaniModel();

    model = builder.build(janiModel);
    EXPECT_EQ(1728ul, model->getNumberOfStates());
    EXPECT_EQ(2505ul, model->getNumberOfTransitions());
}

TEST(DdJaniModelBuilderTest_Cudd, Dtmc) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/die.pm");
    storm::jani::Model janiModel = modelDescription.toJani(true).preprocess().asJaniModel();

    storm::builder::DdJaniModelBuilder<storm::dd::DdType::CUDD, double> builder;
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = builder.build(janiModel);
    EXPECT_EQ(13ul, model->getNumberOfStates());
    EXPECT_EQ(20ul, model->getNumberOfTransitions());

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/brp-16-2.pm");
    janiModel = modelDescription.toJani(true).preprocess().asJaniModel();
    model = builder.build(janiModel);
    EXPECT_EQ(677ul, model->getNumberOfStates());
    EXPECT_EQ(867ul, model->getNumberOfTransitions());

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/crowds-5-5.pm");
    janiModel = modelDescription.toJani(true).preprocess().asJaniModel();
    model = builder.build(janiModel);
    EXPECT_EQ(8607ul, model->getNumberOfStates());
    EXPECT_EQ(15113ul, model->getNumberOfTransitions());

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/leader-3-5.pm");
    janiModel = modelDescription.toJani(true).preprocess().asJaniModel();
    model = builder.build(janiModel);
    EXPECT_EQ(273ul, model->getNumberOfStates());
    EXPECT_EQ(397ul, model->getNumberOfTransitions());

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/nand-5-2.pm");
    janiModel = modelDescription.toJani(true).preprocess().asJaniModel();
    model = builder.build(janiModel);
    EXPECT_EQ(1728ul, model->getNumberOfStates());
    EXPECT_EQ(2505ul, model->getNumberOfTransitions());
}

TEST(DdJaniModelBuilderTest_Sylvan, Ctmc) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/ctmc/cluster2.sm", true);
    storm::jani::Model janiModel = modelDescription.toJani(true).preprocess().asJaniModel();
    storm::builder::DdJaniModelBuilder<storm::dd::DdType::Sylvan, double> builder;
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = builder.build(janiModel);
    EXPECT_EQ(276ul, model->getNumberOfStates());
    EXPECT_EQ(1120ul, model->getNumberOfTransitions());

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/ctmc/embedded2.sm", true);
    janiModel = modelDescription.toJani(true).preprocess().asJaniModel();
    model = builder.build(janiModel);
    EXPECT_EQ(3478ul, model->getNumberOfStates());
    EXPECT_EQ(14639ul, model->getNumberOfTransitions());

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/ctmc/polling2.sm", true);
    janiModel = modelDescription.toJani(true).preprocess().asJaniModel();
    model = builder.build(janiModel);
    EXPECT_EQ(12ul, model->getNumberOfStates());
    EXPECT_EQ(22ul, model->getNumberOfTransitions());

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/ctmc/fms2.sm", true);
    janiModel = modelDescription.toJani(true).preprocess().asJaniModel();
    model = builder.build(janiModel);
    EXPECT_EQ(810ul, model->getNumberOfStates());
    EXPECT_EQ(3699ul, model->getNumberOfTransitions());

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/ctmc/tandem5.sm", true);
    janiModel = modelDescription.toJani(true).preprocess().asJaniModel();
    model = builder.build(janiModel);
    EXPECT_EQ(66ul, model->getNumberOfStates());
    EXPECT_EQ(189ul, model->getNumberOfTransitions());
}

TEST(DdJaniModelBuilderTest_Cudd, Ctmc) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/ctmc/cluster2.sm", true);
    storm::jani::Model janiModel = modelDescription.toJani(true).preprocess().asJaniModel();
    storm::builder::DdJaniModelBuilder<storm::dd::DdType::CUDD, double> builder;
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = builder.build(janiModel);
    EXPECT_EQ(276ul, model->getNumberOfStates());
    EXPECT_EQ(1120ul, model->getNumberOfTransitions());

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/ctmc/embedded2.sm", true);
    janiModel = modelDescription.toJani(true).preprocess().asJaniModel();
    model = builder.build(janiModel);
    EXPECT_EQ(3478ul, model->getNumberOfStates());
    EXPECT_EQ(14639ul, model->getNumberOfTransitions());

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/ctmc/polling2.sm", true);
    janiModel = modelDescription.toJani(true).preprocess().asJaniModel();
    model = builder.build(janiModel);
    EXPECT_EQ(12ul, model->getNumberOfStates());
    EXPECT_EQ(22ul, model->getNumberOfTransitions());

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/ctmc/fms2.sm", true);
    janiModel = modelDescription.toJani(true).preprocess().asJaniModel();
    model = builder.build(janiModel);
    EXPECT_EQ(810ul, model->getNumberOfStates());
    EXPECT_EQ(3699ul, model->getNumberOfTransitions());

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/ctmc/tandem5.sm", true);
    janiModel = modelDescription.toJani(true).preprocess().asJaniModel();
    model = builder.build(janiModel);
    EXPECT_EQ(66ul, model->getNumberOfStates());
    EXPECT_EQ(189ul, model->getNumberOfTransitions());
}

TEST(DdJaniModelBuilderTest_Sylvan, Mdp) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/two_dice.nm");
    storm::jani::Model janiModel = modelDescription.toJani(true).preprocess().asJaniModel();
    storm::builder::DdJaniModelBuilder<storm::dd::DdType::Sylvan, double> builder;
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = builder.build(janiModel);

    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    std::shared_ptr<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>> mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>();

    EXPECT_EQ(169ul, mdp->getNumberOfStates());
    EXPECT_EQ(436ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(254ul, mdp->getNumberOfChoices());

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/leader3.nm");
    janiModel = modelDescription.toJani(true).preprocess().asJaniModel();
    model = builder.build(janiModel);

    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>();

    EXPECT_EQ(364ul, mdp->getNumberOfStates());
    EXPECT_EQ(654ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(573ul, mdp->getNumberOfChoices());

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/coin2-2.nm");
    janiModel = modelDescription.toJani(true).preprocess().asJaniModel();
    model = builder.build(janiModel);

    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>();

    EXPECT_EQ(272ul, mdp->getNumberOfStates());
    EXPECT_EQ(492ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(400ul, mdp->getNumberOfChoices());

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/csma2-2.nm");
    janiModel = modelDescription.toJani(true).preprocess().asJaniModel();
    model = builder.build(janiModel);

    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>();

    EXPECT_EQ(1038ul, mdp->getNumberOfStates());
    EXPECT_EQ(1282ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(1054ul, mdp->getNumberOfChoices());

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/firewire3-0.5.nm");
    janiModel = modelDescription.toJani(true).preprocess().asJaniModel();
    model = builder.build(janiModel);

    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>();

    EXPECT_EQ(4093ul, mdp->getNumberOfStates());
    EXPECT_EQ(5585ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(5519ul, mdp->getNumberOfChoices());

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/wlan0-2-2.nm");
    janiModel = modelDescription.toJani(true).preprocess().asJaniModel();
    model = builder.build(janiModel);

    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>();

    EXPECT_EQ(37ul, mdp->getNumberOfStates());
    EXPECT_EQ(59ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(59ul, mdp->getNumberOfChoices());
}

TEST(DdJaniModelBuilderTest_Cudd, Mdp) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/two_dice.nm");
    storm::jani::Model janiModel = modelDescription.toJani(true).preprocess().asJaniModel();
    storm::builder::DdJaniModelBuilder<storm::dd::DdType::CUDD, double> builder;
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = builder.build(janiModel);

    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    std::shared_ptr<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>> mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>();

    EXPECT_EQ(169ul, mdp->getNumberOfStates());
    EXPECT_EQ(436ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(254ul, mdp->getNumberOfChoices());

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/leader3.nm");
    janiModel = modelDescription.toJani(true).preprocess().asJaniModel();
    model = builder.build(janiModel);

    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>();

    EXPECT_EQ(364ul, mdp->getNumberOfStates());
    EXPECT_EQ(654ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(573ul, mdp->getNumberOfChoices());

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/coin2-2.nm");
    janiModel = modelDescription.toJani(true).preprocess().asJaniModel();
    model = builder.build(janiModel);

    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>();

    EXPECT_EQ(272ul, mdp->getNumberOfStates());
    EXPECT_EQ(492ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(400ul, mdp->getNumberOfChoices());

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/csma2-2.nm");
    janiModel = modelDescription.toJani(true).preprocess().asJaniModel();
    model = builder.build(janiModel);

    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>();

    EXPECT_EQ(1038ul, mdp->getNumberOfStates());
    EXPECT_EQ(1282ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(1054ul, mdp->getNumberOfChoices());

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/firewire3-0.5.nm");
    janiModel = modelDescription.toJani(true).preprocess().asJaniModel();
    model = builder.build(janiModel);

    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>();

    EXPECT_EQ(4093ul, mdp->getNumberOfStates());
    EXPECT_EQ(5585ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(5519ul, mdp->getNumberOfChoices());

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/wlan0-2-2.nm");
    janiModel = modelDescription.toJani(true).preprocess().asJaniModel();
    model = builder.build(janiModel);

    EXPECT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    mdp = model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>();

    EXPECT_EQ(37ul, mdp->getNumberOfStates());
    EXPECT_EQ(59ul, mdp->getNumberOfTransitions());
    EXPECT_EQ(59ul, mdp->getNumberOfChoices());
}

TEST(DdJaniModelBuilderTest_Cudd, SynchronizationVectors) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/SmallPrismTest.nm");
    storm::jani::Model janiModel = modelDescription.toJani(true).preprocess().asJaniModel();

    storm::builder::DdJaniModelBuilder<storm::dd::DdType::CUDD, double> builder;

    // Start by checking the original composition.
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = builder.build(janiModel);
    EXPECT_EQ(7ul, model->getNumberOfStates());
    EXPECT_EQ(10ul, model->getNumberOfTransitions());

    // Now we tweak it's system composition to check whether synchronization vectors work.
    std::vector<std::shared_ptr<storm::jani::Composition>> automataCompositions;
    automataCompositions.push_back(std::make_shared<storm::jani::AutomatonComposition>("one"));
    automataCompositions.push_back(std::make_shared<storm::jani::AutomatonComposition>("two"));
    automataCompositions.push_back(std::make_shared<storm::jani::AutomatonComposition>("three"));

    // First, make all actions non-synchronizing.
    std::vector<storm::jani::SynchronizationVector> synchronizationVectors;

    std::vector<std::string> inputVector;
    inputVector.push_back("a");
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    synchronizationVectors.emplace_back(inputVector);
    inputVector.clear();
    inputVector.push_back("c");
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    synchronizationVectors.emplace_back(inputVector);
    inputVector.clear();
    inputVector.push_back("d");
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    synchronizationVectors.emplace_back(inputVector);
    inputVector.clear();
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    inputVector.push_back("b");
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    synchronizationVectors.emplace_back(inputVector);
    inputVector.clear();
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    inputVector.push_back("c");
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    synchronizationVectors.emplace_back(inputVector);
    inputVector.clear();
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    inputVector.push_back("c");
    synchronizationVectors.emplace_back(inputVector);
    inputVector.clear();

    std::shared_ptr<storm::jani::Composition> newComposition = std::make_shared<storm::jani::ParallelComposition>(automataCompositions, synchronizationVectors);
    janiModel.setSystemComposition(newComposition);
    model = builder.build(janiModel);
    EXPECT_EQ(24ul, model->getNumberOfStates());
    EXPECT_EQ(48ul, model->getNumberOfTransitions());

    // Then, make only a, b and c synchronize.
    synchronizationVectors.clear();
    inputVector.clear();
    inputVector.push_back("a");
    inputVector.push_back("b");
    inputVector.push_back("c");
    synchronizationVectors.emplace_back(inputVector, "d");
    inputVector.clear();
    inputVector.push_back("c");
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    synchronizationVectors.emplace_back(inputVector);
    inputVector.clear();
    inputVector.push_back("d");
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    synchronizationVectors.emplace_back(inputVector);
    inputVector.clear();
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    inputVector.push_back("c");
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    synchronizationVectors.emplace_back(inputVector);

    newComposition = std::make_shared<storm::jani::ParallelComposition>(automataCompositions, synchronizationVectors);
    janiModel.setSystemComposition(newComposition);
    model = builder.build(janiModel);
    EXPECT_EQ(7ul, model->getNumberOfStates());
    EXPECT_EQ(10ul, model->getNumberOfTransitions());

    synchronizationVectors.clear();
    inputVector.clear();
    inputVector.push_back("a");
    inputVector.push_back("b");
    inputVector.push_back("c");
    synchronizationVectors.emplace_back(inputVector, "d");
    inputVector.clear();
    inputVector.push_back("c");
    inputVector.push_back("c");
    inputVector.push_back("a");
    synchronizationVectors.emplace_back(inputVector, "d");
    inputVector.clear();
    inputVector.push_back("d");
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    synchronizationVectors.emplace_back(inputVector);
    newComposition = std::make_shared<storm::jani::ParallelComposition>(automataCompositions, synchronizationVectors);
    janiModel.setSystemComposition(newComposition);
    model = builder.build(janiModel);
    EXPECT_EQ(3ul, model->getNumberOfStates());
    EXPECT_EQ(3ul, model->getNumberOfTransitions());

    synchronizationVectors.clear();
    inputVector.clear();
    inputVector.push_back("a");
    inputVector.push_back("b");
    inputVector.push_back("c");
    synchronizationVectors.emplace_back(inputVector, "d");
    inputVector.clear();
    inputVector.push_back("c");
    inputVector.push_back("c");
    inputVector.push_back("a");
    synchronizationVectors.emplace_back(inputVector, "d");
    inputVector.clear();
    inputVector.push_back("d");
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    synchronizationVectors.emplace_back(inputVector);
    inputVector.clear();
    inputVector.push_back("d");
    inputVector.push_back("c");
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    synchronizationVectors.emplace_back(inputVector, "b");
    newComposition = std::make_shared<storm::jani::ParallelComposition>(automataCompositions, synchronizationVectors);
    janiModel.setSystemComposition(newComposition);
    model = builder.build(janiModel);
    EXPECT_EQ(4ul, model->getNumberOfStates());
    EXPECT_EQ(5ul, model->getNumberOfTransitions());
}

TEST(DdJaniModelBuilderTest_Sylvan, SynchronizationVectors) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/SmallPrismTest.nm");
    storm::jani::Model janiModel = modelDescription.toJani(true).preprocess().asJaniModel();

    storm::builder::DdJaniModelBuilder<storm::dd::DdType::Sylvan, double> builder;

    // Start by checking the original composition.
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = builder.build(janiModel);
    EXPECT_EQ(7ul, model->getNumberOfStates());
    EXPECT_EQ(10ul, model->getNumberOfTransitions());

    // Now we tweak it's system composition to check whether synchronization vectors work.
    std::vector<std::shared_ptr<storm::jani::Composition>> automataCompositions;
    automataCompositions.push_back(std::make_shared<storm::jani::AutomatonComposition>("one"));
    automataCompositions.push_back(std::make_shared<storm::jani::AutomatonComposition>("two"));
    automataCompositions.push_back(std::make_shared<storm::jani::AutomatonComposition>("three"));

    // First, make all actions non-synchronizing.
    std::vector<storm::jani::SynchronizationVector> synchronizationVectors;

    std::vector<std::string> inputVector;
    inputVector.push_back("a");
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    synchronizationVectors.emplace_back(inputVector);
    inputVector.clear();
    inputVector.push_back("c");
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    synchronizationVectors.emplace_back(inputVector);
    inputVector.clear();
    inputVector.push_back("d");
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    synchronizationVectors.emplace_back(inputVector);
    inputVector.clear();
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    inputVector.push_back("b");
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    synchronizationVectors.emplace_back(inputVector);
    inputVector.clear();
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    inputVector.push_back("c");
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    synchronizationVectors.emplace_back(inputVector);
    inputVector.clear();
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    inputVector.push_back("c");
    synchronizationVectors.emplace_back(inputVector);
    inputVector.clear();

    std::shared_ptr<storm::jani::Composition> newComposition = std::make_shared<storm::jani::ParallelComposition>(automataCompositions, synchronizationVectors);
    janiModel.setSystemComposition(newComposition);
    model = builder.build(janiModel);
    EXPECT_EQ(24ul, model->getNumberOfStates());
    EXPECT_EQ(48ul, model->getNumberOfTransitions());

    // Then, make only a, b and c synchronize.
    synchronizationVectors.clear();
    inputVector.clear();
    inputVector.push_back("a");
    inputVector.push_back("b");
    inputVector.push_back("c");
    synchronizationVectors.emplace_back(inputVector, "d");
    inputVector.clear();
    inputVector.push_back("c");
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    synchronizationVectors.emplace_back(inputVector);
    inputVector.clear();
    inputVector.push_back("d");
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    synchronizationVectors.emplace_back(inputVector);
    inputVector.clear();
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    inputVector.push_back("c");
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    synchronizationVectors.emplace_back(inputVector);

    newComposition = std::make_shared<storm::jani::ParallelComposition>(automataCompositions, synchronizationVectors);
    janiModel.setSystemComposition(newComposition);
    model = builder.build(janiModel);
    EXPECT_EQ(7ul, model->getNumberOfStates());
    EXPECT_EQ(10ul, model->getNumberOfTransitions());

    synchronizationVectors.clear();
    inputVector.clear();
    inputVector.push_back("a");
    inputVector.push_back("b");
    inputVector.push_back("c");
    synchronizationVectors.emplace_back(inputVector, "d");
    inputVector.clear();
    inputVector.push_back("c");
    inputVector.push_back("c");
    inputVector.push_back("a");
    synchronizationVectors.emplace_back(inputVector, "d");
    inputVector.clear();
    inputVector.push_back("d");
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    synchronizationVectors.emplace_back(inputVector);
    newComposition = std::make_shared<storm::jani::ParallelComposition>(automataCompositions, synchronizationVectors);
    janiModel.setSystemComposition(newComposition);
    model = builder.build(janiModel);
    EXPECT_EQ(3ul, model->getNumberOfStates());
    EXPECT_EQ(3ul, model->getNumberOfTransitions());

    synchronizationVectors.clear();
    inputVector.clear();
    inputVector.push_back("a");
    inputVector.push_back("b");
    inputVector.push_back("c");
    synchronizationVectors.emplace_back(inputVector, "d");
    inputVector.clear();
    inputVector.push_back("c");
    inputVector.push_back("c");
    inputVector.push_back("a");
    synchronizationVectors.emplace_back(inputVector, "d");
    inputVector.clear();
    inputVector.push_back("d");
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    synchronizationVectors.emplace_back(inputVector);
    inputVector.clear();
    inputVector.push_back("d");
    inputVector.push_back("c");
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    synchronizationVectors.emplace_back(inputVector, "b");
    newComposition = std::make_shared<storm::jani::ParallelComposition>(automataCompositions, synchronizationVectors);
    janiModel.setSystemComposition(newComposition);
    model = builder.build(janiModel);
    EXPECT_EQ(4ul, model->getNumberOfStates());
    EXPECT_EQ(5ul, model->getNumberOfTransitions());
}

TEST(DdJaniModelBuilderTest_Sylvan, Composition) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/system_composition.nm");
    storm::jani::Model janiModel = modelDescription.toJani(true).preprocess().asJaniModel();

    storm::builder::DdJaniModelBuilder<storm::dd::DdType::Sylvan, double> builder;
    STORM_SILENT_EXPECT_THROW(std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = builder.build(janiModel),
                              storm::exceptions::WrongFormatException);

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/system_composition2.nm");
    janiModel = modelDescription.toJani(true).preprocess().asJaniModel();
    STORM_SILENT_EXPECT_THROW(std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = builder.build(janiModel),
                              storm::exceptions::WrongFormatException);
}

TEST(DdJaniModelBuilderTest_Cudd, Composition) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/system_composition.nm");
    storm::jani::Model janiModel = modelDescription.toJani(true).preprocess().asJaniModel();

    storm::builder::DdJaniModelBuilder<storm::dd::DdType::CUDD, double> builder;
    STORM_SILENT_EXPECT_THROW(std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = builder.build(janiModel),
                              storm::exceptions::WrongFormatException);

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/system_composition2.nm");
    janiModel = modelDescription.toJani(true).preprocess().asJaniModel();
    STORM_SILENT_EXPECT_THROW(std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = builder.build(janiModel),
                              storm::exceptions::WrongFormatException);
}

TEST(DdJaniModelBuilderTest_Cudd, InputEnabling) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/SmallPrismTest2.nm");
    storm::jani::Model janiModel = modelDescription.toJani(true).preprocess().asJaniModel();

    storm::builder::DdJaniModelBuilder<storm::dd::DdType::CUDD, double> builder;

    // Make some automaton compositions input-enabled.
    std::vector<std::shared_ptr<storm::jani::Composition>> automataCompositions;
    automataCompositions.push_back(std::make_shared<storm::jani::AutomatonComposition>("one"));
    automataCompositions.push_back(std::make_shared<storm::jani::AutomatonComposition>("two"));
    automataCompositions.push_back(std::make_shared<storm::jani::AutomatonComposition>("three", std::set<std::string>{"a"}));

    // Create the synchronization vectors.
    std::vector<storm::jani::SynchronizationVector> synchronizationVectors;
    std::vector<std::string> inputVector;
    inputVector.push_back("a");
    inputVector.push_back("b");
    inputVector.push_back("c");
    synchronizationVectors.emplace_back(inputVector, "d");
    inputVector.clear();
    inputVector.push_back("c");
    inputVector.push_back("c");
    inputVector.push_back("a");
    synchronizationVectors.emplace_back(inputVector, "d");
    inputVector.clear();
    inputVector.push_back("d");
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    synchronizationVectors.emplace_back(inputVector);

    std::shared_ptr<storm::jani::Composition> newComposition = std::make_shared<storm::jani::ParallelComposition>(automataCompositions, synchronizationVectors);
    janiModel.setSystemComposition(newComposition);
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = builder.build(janiModel);
    EXPECT_EQ(4ul, model->getNumberOfStates());
    EXPECT_EQ(5ul, model->getNumberOfTransitions());
}

TEST(DdJaniModelBuilderTest_Sylvan, InputEnabling) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/SmallPrismTest2.nm");
    storm::jani::Model janiModel = modelDescription.toJani(true).preprocess().asJaniModel();

    storm::builder::DdJaniModelBuilder<storm::dd::DdType::Sylvan, double> builder;

    // Make some automaton compositions input-enabled.
    std::vector<std::shared_ptr<storm::jani::Composition>> automataCompositions;
    automataCompositions.push_back(std::make_shared<storm::jani::AutomatonComposition>("one"));
    automataCompositions.push_back(std::make_shared<storm::jani::AutomatonComposition>("two"));
    automataCompositions.push_back(std::make_shared<storm::jani::AutomatonComposition>("three", std::set<std::string>{"a"}));

    // Create the synchronization vectors.
    std::vector<storm::jani::SynchronizationVector> synchronizationVectors;
    std::vector<std::string> inputVector;
    inputVector.push_back("a");
    inputVector.push_back("b");
    inputVector.push_back("c");
    synchronizationVectors.emplace_back(inputVector, "d");
    inputVector.clear();
    inputVector.push_back("c");
    inputVector.push_back("c");
    inputVector.push_back("a");
    synchronizationVectors.emplace_back(inputVector, "d");
    inputVector.clear();
    inputVector.push_back("d");
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    inputVector.push_back(storm::jani::SynchronizationVector::NO_ACTION_INPUT);
    synchronizationVectors.emplace_back(inputVector);

    std::shared_ptr<storm::jani::Composition> newComposition = std::make_shared<storm::jani::ParallelComposition>(automataCompositions, synchronizationVectors);
    janiModel.setSystemComposition(newComposition);
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = builder.build(janiModel);
    EXPECT_EQ(4ul, model->getNumberOfStates());
    EXPECT_EQ(5ul, model->getNumberOfTransitions());
}
