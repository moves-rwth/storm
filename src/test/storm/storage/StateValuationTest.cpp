#include "storm-config.h"
#include "test/storm_gtest.h"

#include "storm-parsers/parser/PrismParser.h"
#include "storm/builder/ExplicitModelBuilder.h"
#include "storm/exceptions/WrongFormatException.h"
#include "storm/generator/PrismNextStateGenerator.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/sparse/StateValuationTransformer.h"

class StateValuationTest : public ::testing::Test {
   protected:
    void SetUp() override {
#ifndef STORM_HAVE_Z3
        GTEST_SKIP() << "Z3 not available.";
#endif
    }
};

TEST_F(StateValuationTest, StateValuationConstruction) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/die.pm");
    storm::generator::NextStateGeneratorOptions generatorOptions;
    generatorOptions.setBuildStateValuations();
    auto builder = storm::builder::ExplicitModelBuilder<double>(program, generatorOptions);
    std::shared_ptr<storm::models::sparse::Model<double>> model = builder.build();
    ASSERT_TRUE(model->hasStateValuations());
    auto const& sv = model->getStateValuations();
    ASSERT_EQ(sv.getNumberOfStates(), model->getNumberOfStates());
    auto val = sv.at(0).begin();
    ASSERT_EQ(val.getName(), "s");
    val.operator++();
    ASSERT_EQ(val.getName(), "d");
    val.operator++();
    ASSERT_TRUE(val == sv.at(0).end());
}

TEST_F(StateValuationTest, StateValuationTransformation) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/die.pm");
    storm::generator::NextStateGeneratorOptions generatorOptions;
    generatorOptions.setBuildStateValuations();
    auto builder = storm::builder::ExplicitModelBuilder<double>(program, generatorOptions);
    std::shared_ptr<storm::models::sparse::Model<double>> model = builder.build();
    ASSERT_TRUE(model->hasStateValuations());
    auto const& sv = model->getStateValuations();
    storm::storage::sparse::StateValuationTransform transformer(sv);
    auto svar = program.getManager().getVariable("s");
    transformer.addBooleanExpression(program.getManager().declareBooleanVariable("sGT3"), svar.getExpression() > program.getManager().integer(3));
    auto newsv = transformer.buildNewStateValuations(true);
    auto val = newsv.at(0).begin();
    ASSERT_EQ(val.getName(), "sGT3");
    val.operator++();
    ASSERT_EQ(val.getName(), "s");
    val.operator++();
    ASSERT_EQ(val.getName(), "d");
    val.operator++();
    ASSERT_TRUE(val == newsv.at(0).end());
    for (uint64_t state = 0; state < newsv.getNumberOfStates(); ++state) {
        ASSERT_EQ(sv.getIntegerValue(state, svar), newsv.getIntegerValue(state, svar));
        ASSERT_EQ(newsv.getBooleanValue(state, program.getManager().getVariable("sGT3")), newsv.getIntegerValue(state, svar) > 3);
    }
}
