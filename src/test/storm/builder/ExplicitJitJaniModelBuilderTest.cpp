#include "storm-config.h"
#include "storm-parsers/api/model_descriptions.h"
#include "storm-parsers/parser/PrismParser.h"
#include "storm/builder/jit/ExplicitJitJaniModelBuilder.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/settings/SettingMemento.h"
#include "storm/storage/jani/Model.h"
#include "test/storm_gtest.h"

#include "storm/settings/SettingsManager.h"

TEST(ExplicitJitJaniModelBuilderTest, Dtmc) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/die.pm");
    storm::jani::Model janiModel = program.toJani().substituteConstantsFunctions();

    std::shared_ptr<storm::models::sparse::Model<double>> model = storm::builder::jit::ExplicitJitJaniModelBuilder<double>(janiModel).build();
    ;
    EXPECT_EQ(13ul, model->getNumberOfStates());
    EXPECT_EQ(20ul, model->getNumberOfTransitions());

    program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/brp-16-2.pm");
    janiModel = program.toJani().substituteConstantsFunctions();
    model = storm::builder::jit::ExplicitJitJaniModelBuilder<double>(janiModel).build();
    ;
    EXPECT_EQ(677ul, model->getNumberOfStates());
    EXPECT_EQ(867ul, model->getNumberOfTransitions());

    program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/crowds-5-5.pm");
    janiModel = program.toJani().substituteConstantsFunctions();
    model = storm::builder::jit::ExplicitJitJaniModelBuilder<double>(janiModel).build();
    ;
    EXPECT_EQ(8607ul, model->getNumberOfStates());
    EXPECT_EQ(15113ul, model->getNumberOfTransitions());

    program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/leader-3-5.pm");
    janiModel = program.toJani().substituteConstantsFunctions();
    model = storm::builder::jit::ExplicitJitJaniModelBuilder<double>(janiModel).build();
    ;
    EXPECT_EQ(273ul, model->getNumberOfStates());
    EXPECT_EQ(397ul, model->getNumberOfTransitions());

    program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/nand-5-2.pm");
    janiModel = program.toJani().substituteConstantsFunctions();
    model = storm::builder::jit::ExplicitJitJaniModelBuilder<double>(janiModel).build();
    ;
    EXPECT_EQ(1728ul, model->getNumberOfStates());
    EXPECT_EQ(2505ul, model->getNumberOfTransitions());
}

TEST(ExplicitJitJaniModelBuilderTest, Ctmc) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/ctmc/cluster2.sm", true);
    storm::jani::Model janiModel = program.toJani().substituteConstantsFunctions();

    std::shared_ptr<storm::models::sparse::Model<double>> model = storm::builder::jit::ExplicitJitJaniModelBuilder<double>(janiModel).build();
    ;
    EXPECT_EQ(276ul, model->getNumberOfStates());
    EXPECT_EQ(1120ul, model->getNumberOfTransitions());

    program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/ctmc/embedded2.sm", true);
    janiModel = program.toJani().substituteConstantsFunctions();
    model = storm::builder::jit::ExplicitJitJaniModelBuilder<double>(janiModel).build();
    ;
    EXPECT_EQ(3478ul, model->getNumberOfStates());
    EXPECT_EQ(14639ul, model->getNumberOfTransitions());

    program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/ctmc/polling2.sm", true);
    janiModel = program.toJani().substituteConstantsFunctions();
    model = storm::builder::jit::ExplicitJitJaniModelBuilder<double>(janiModel).build();
    ;
    EXPECT_EQ(12ul, model->getNumberOfStates());
    EXPECT_EQ(22ul, model->getNumberOfTransitions());

    program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/ctmc/fms2.sm", true);
    janiModel = program.toJani().substituteConstantsFunctions();
    model = storm::builder::jit::ExplicitJitJaniModelBuilder<double>(janiModel).build();
    ;
    EXPECT_EQ(810ul, model->getNumberOfStates());
    EXPECT_EQ(3699ul, model->getNumberOfTransitions());

    program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/ctmc/tandem5.sm", true);
    janiModel = program.toJani().substituteConstantsFunctions();
    model = storm::builder::jit::ExplicitJitJaniModelBuilder<double>(janiModel).build();
    ;
    EXPECT_EQ(66ul, model->getNumberOfStates());
    EXPECT_EQ(189ul, model->getNumberOfTransitions());
}

TEST(ExplicitJitJaniModelBuilderTest, Mdp) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/two_dice.nm");
    storm::jani::Model janiModel = program.toJani().substituteConstantsFunctions();

    std::shared_ptr<storm::models::sparse::Model<double>> model = storm::builder::jit::ExplicitJitJaniModelBuilder<double>(janiModel).build();
    ;
    EXPECT_EQ(169ul, model->getNumberOfStates());
    EXPECT_EQ(436ul, model->getNumberOfTransitions());

    program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/leader3.nm");
    janiModel = program.toJani().substituteConstantsFunctions();
    model = storm::builder::jit::ExplicitJitJaniModelBuilder<double>(janiModel).build();
    ;
    EXPECT_EQ(364ul, model->getNumberOfStates());
    EXPECT_EQ(654ul, model->getNumberOfTransitions());

    program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/coin2-2.nm");
    janiModel = program.toJani().substituteConstantsFunctions();
    model = storm::builder::jit::ExplicitJitJaniModelBuilder<double>(janiModel).build();
    ;
    EXPECT_EQ(272ul, model->getNumberOfStates());
    EXPECT_EQ(492ul, model->getNumberOfTransitions());

    program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/csma2-2.nm");
    janiModel = program.toJani().substituteConstantsFunctions();
    model = storm::builder::jit::ExplicitJitJaniModelBuilder<double>(janiModel).build();
    ;
    EXPECT_EQ(1038ul, model->getNumberOfStates());
    EXPECT_EQ(1282ul, model->getNumberOfTransitions());

    program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/firewire3-0.5.nm");
    janiModel = program.toJani().substituteConstantsFunctions();
    model = storm::builder::jit::ExplicitJitJaniModelBuilder<double>(janiModel).build();
    ;
    EXPECT_EQ(4093ul, model->getNumberOfStates());
    EXPECT_EQ(5585ul, model->getNumberOfTransitions());

    program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/wlan0-2-2.nm");
    janiModel = program.toJani().substituteConstantsFunctions();
    model = storm::builder::jit::ExplicitJitJaniModelBuilder<double>(janiModel).build();
    ;
    EXPECT_EQ(37ul, model->getNumberOfStates());
    EXPECT_EQ(59ul, model->getNumberOfTransitions());
}

TEST(ExplicitJitJaniModelBuilderTest, Ma) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/ma/simple.ma");
    storm::jani::Model janiModel = program.toJani().substituteConstantsFunctions();

    std::shared_ptr<storm::models::sparse::Model<double>> model = storm::builder::jit::ExplicitJitJaniModelBuilder<double>(janiModel).build();
    ;
    EXPECT_EQ(5ul, model->getNumberOfStates());
    EXPECT_EQ(8ul, model->getNumberOfTransitions());
    ASSERT_TRUE(model->isOfType(storm::models::ModelType::MarkovAutomaton));
    EXPECT_EQ(4ul, model->as<storm::models::sparse::MarkovAutomaton<double>>()->getMarkovianStates().getNumberOfSetBits());

    program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/ma/hybrid_states.ma");
    janiModel = program.toJani().substituteConstantsFunctions();
    model = storm::builder::jit::ExplicitJitJaniModelBuilder<double>(janiModel).build();
    ;
    EXPECT_EQ(5ul, model->getNumberOfStates());
    EXPECT_EQ(13ul, model->getNumberOfTransitions());
    ASSERT_TRUE(model->isOfType(storm::models::ModelType::MarkovAutomaton));
    EXPECT_EQ(5ul, model->as<storm::models::sparse::MarkovAutomaton<double>>()->getMarkovianStates().getNumberOfSetBits());

    program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/ma/stream2.ma");
    janiModel = program.toJani().substituteConstantsFunctions();
    model = storm::builder::jit::ExplicitJitJaniModelBuilder<double>(janiModel).build();
    ;
    EXPECT_EQ(12ul, model->getNumberOfStates());
    EXPECT_EQ(14ul, model->getNumberOfTransitions());
    ASSERT_TRUE(model->isOfType(storm::models::ModelType::MarkovAutomaton));
    EXPECT_EQ(7ul, model->as<storm::models::sparse::MarkovAutomaton<double>>()->getMarkovianStates().getNumberOfSetBits());
}

TEST(ExplicitJitJaniModelBuilderTest, FailComposition) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/system_composition.nm");
    storm::jani::Model janiModel = program.toJani().substituteConstantsFunctions();

    STORM_SILENT_ASSERT_THROW(storm::builder::jit::ExplicitJitJaniModelBuilder<double>(janiModel).build();, storm::exceptions::WrongFormatException);
}
