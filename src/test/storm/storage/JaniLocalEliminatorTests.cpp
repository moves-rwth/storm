#include <storm/builder/ExplicitModelBuilder.h>
#include "test/storm_gtest.h"

#include <storm/api/storm.h>
#include "storm-parsers/api/model_descriptions.h"
#include "storm/storage/jani/ModelFeatures.h"
#include "storm/storage/jani/Model.h"
#include "storm/storage/jani/Property.h"
#include "storm/storage/jani/localeliminator/JaniLocalEliminator.h"
#include "storm/storage/jani/localeliminator/AutomaticAction.h"
#include "storm/storage/jani/localeliminator/EliminateAction.h"
#include "storm/storage/jani/localeliminator/EliminateAutomaticallyAction.h"
#include "storm/storage/jani/localeliminator/FinishAction.h"
#include "storm/storage/jani/localeliminator/RebuildWithoutUnreachableAction.h"
#include "storm/storage/jani/localeliminator/UnfoldAction.h"
#include "storm/storage/expressions/Expression.h"
#include "storm/storage/expressions/Variable.h"
#include <storm/modelchecker/results/CheckResult.h>
#include <storm/modelchecker/results/ExplicitQuantitativeCheckResult.h>
#include "storm/environment/Environment.h"
#include <storm/settings/modules/GeneralSettings.h>
#include <storm-parsers/parser/JaniParser.h>
#include <storm-parsers/parser/FormulaParser.h>
#include "storm/storage/jani/JSONExporter.h"


typedef storm::models::sparse::Dtmc<double> Dtmc;
typedef storm::modelchecker::SparseDtmcPrctlModelChecker<Dtmc> DtmcModelChecker;

using storm::jani::JaniLocalEliminator;
using namespace storm::jani::elimination_actions;

// The tests are grouped into these groups
// * Input behaviour
// * Common functionality
// * Unfolding
// * Elimination
// * Rebuilding without unreachable locations
// Input behaviour tests whether the basic public interface of the eliminator works correctly, whereas
// common functionality mainly refers to internal functions that are used in multiple places.

// **************
// Helper functions
// **************

// As many tests rely on building and checking a model (which requires quite a few lines of code), we use this common
// helper function to do the model checking. Apart from this, the tests don't share any helper functions.
void checkModel(storm::jani::Model model, std::vector<storm::jani::Property> properties, std::map<storm::expressions::Variable, storm::expressions::Expression> consts, double expectedValue, int states = -1, int transitions = -1){
    model = model.defineUndefinedConstants(consts);
    properties[0] = properties[0].substitute(consts);

    auto formulae = storm::api::extractFormulasFromProperties(properties);
    storm::builder::BuilderOptions  options(formulae, model);
    options.setBuildAllLabels(true);
    auto explicitModel = storm::api::buildSparseModel<double>(model, options)->template as<Dtmc>();

    if (states != -1){
        EXPECT_EQ(states, explicitModel->getNumberOfStates());
    }
    if (transitions != -1){
        EXPECT_EQ(transitions, explicitModel->getNumberOfTransitions());
    }

    auto trans_matr = explicitModel->getTransitionMatrix();
    for (uint64_t  state = 0; state < explicitModel->getNumberOfStates(); state++){
        std::cout << state << "(";
        for (auto label : explicitModel->getLabelsOfState(state)){
            std::cout << label << ", ";
        }
        std::cout << ") -> ";
        for (auto trans : trans_matr.getRow(state)) {
            std::cout << trans.getValue() << ": " << trans.getColumn() << ", ";
        }
        std::cout << std::endl;

    }

    auto task = storm::modelchecker::CheckTask<>(*(formulae[0]), true);
    storm::Environment env;
    auto checkResult = storm::api::verifyWithSparseEngine<double>(env, explicitModel, task);
    auto quantResult = checkResult->asExplicitQuantitativeCheckResult<double>();

    auto initialStates = explicitModel->getInitialStates();
    EXPECT_EQ(1, initialStates.getNumberOfSetBits());
    for (auto state = initialStates.begin(); state != initialStates.end(); ++state){
        EXPECT_NEAR(expectedValue, quantResult[*state], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    }
}

// *******************************
// Tests for input behaviour:
// *******************************

// This test verifies that an error is produced if the type of property is not supported and that no error is thrown
// when a supported property type is provided.
TEST(JaniLocalEliminator, PropertyTypeTest) {
    // Load a model (the model contains two variables x and y, but doesn't do anything with them).
    auto model = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/localeliminator/do_nothing.jani",
                                            storm::jani::getAllKnownModelFeatures(), boost::none).first;
    storm::parser::FormulaParser formulaParser(model.getExpressionManager().shared_from_this());

    {
        // This should fail because we only support F and R as top-level operators
        std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString(
                "S=? [ true ]");
        auto property = storm::jani::Property("steady_state", formula,
                                              std::set<storm::expressions::Variable>());
        STORM_SILENT_EXPECT_THROW(JaniLocalEliminator(model, property), storm::exceptions::NotSupportedException);
    }
    {
        // This should fail because we only support reachability formulas
        std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString(
                "P=? [G x=1]");
        auto property = storm::jani::Property("generally", formula, std::set<storm::expressions::Variable>());
        STORM_SILENT_EXPECT_THROW(JaniLocalEliminator(model, property), storm::exceptions::NotSupportedException);
    }
    {
        // This should fail because we only support reachability formulas (this test is mainly there because of the
        // similarity between eventually and until formulas)
        std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString(
                "P=? [x=1 U y=1]");
        auto property = storm::jani::Property("until", formula, std::set<storm::expressions::Variable>());
        STORM_SILENT_EXPECT_THROW(JaniLocalEliminator(model, property), storm::exceptions::NotSupportedException);
    }
    {
        // This should fail because we only support reachability formulas with an atomic subexpression.
        std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString(
                "P=? [F (P>=1[G x = 1])]");
        auto property = storm::jani::Property("complex_reachability", formula,
                                              std::set<storm::expressions::Variable>());
        STORM_SILENT_EXPECT_THROW(JaniLocalEliminator(model, property), storm::exceptions::NotSupportedException);
    }
    {
        // Similarly, rewards are also only allowed with atomic reachability expressions.
        std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString(
                "R=? [F (P>=1[G x = 1])]");
        auto property = storm::jani::Property("complex_reward_reachability", formula,
                                              std::set<storm::expressions::Variable>());
        STORM_SILENT_EXPECT_THROW(JaniLocalEliminator(model, property), storm::exceptions::NotSupportedException);
    }
    {
        // This should succeed because reachability probabilities are supported
        std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString(
                "P=? [ F x=1 ]");
        auto property = storm::jani::Property("reachability", formula,
                                              std::set<storm::expressions::Variable>());
        EXPECT_NO_THROW(JaniLocalEliminator(model, property));
    }
    {
        // This should succeed because reachability probabilities are supported
        std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString(
                "R=? [ F x=1 ]");
        auto property = storm::jani::Property("reward_reachability", formula,
                                              std::set<storm::expressions::Variable>());
        EXPECT_NO_THROW(JaniLocalEliminator(model, property));
    }
}

// This test verifies that an error is given if no properties are provided.
TEST(JaniLocalEliminator, NoPropertiesTest) {
    auto model = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/localeliminator/do_nothing.jani",
                                            storm::jani::getAllKnownModelFeatures(), boost::none).first;
    std::vector<storm::jani::Property> empty_properties;
    STORM_SILENT_ASSERT_THROW(JaniLocalEliminator(model, empty_properties), storm::exceptions::InvalidArgumentException);
}

// This test verifies that the model is flattened if it has more than one automaton and that the user is informed
// of this.
TEST(JaniLocalEliminator, FlatteningTest) {
    auto modelAndProps = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/localeliminator/two_modules.jani",
                                                    storm::jani::getAllKnownModelFeatures(), boost::none);
    auto eliminator = JaniLocalEliminator(modelAndProps.first, modelAndProps.second);
    eliminator.eliminate();
    auto result = eliminator.getResult();
    EXPECT_EQ(1, result.getNumberOfAutomata());
}

// *******************************
// Tests for common functionality:
// *******************************

// This test verifies that missing guard completion works correctly. It should not alter the behaviour of the model
// and ensure that every location has an outgoing edge for every possible case.
TEST(JaniLocalEliminator, MissingGuardCompletion) {
    // In this model, s_1 has a missing guard (the case x=0 is not covered). Thus, if we eliminate s_1 without
    // taking additional precautions, this missing guard will propagate to s_0, which causes an incorrect result.
    // If we activate the addMissingGuards option of the eliminator, this will be avoided.

    auto modelAndProps = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/localeliminator/missing_guard.jani",
                                                    storm::jani::getAllKnownModelFeatures(), boost::none);
    auto eliminator = JaniLocalEliminator(modelAndProps.first, modelAndProps.second, true);
    eliminator.scheduler.addAction(std::make_unique<UnfoldAction>("main", "s"));
    eliminator.scheduler.addAction(std::make_unique<EliminateAction>("main", "l_s_1"));
    eliminator.eliminate();
    auto result = eliminator.getResult();
    EXPECT_EQ(1, result.getNumberOfAutomata());
    checkModel(result, modelAndProps.second, std::map<storm::expressions::Variable, storm::expressions::Expression>(), 0.5);
}

// This test verifies that locations with loops are correctly identified.
TEST(JaniLocalEliminator, LoopDetection) {
    auto model = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/localeliminator/loops.jani",
                                            storm::jani::getAllKnownModelFeatures(), boost::none).first;

    storm::parser::FormulaParser formulaParser(model.getExpressionManager().shared_from_this());
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString(
            "P=? [F x=1]");
    auto property = storm::jani::Property("prop", formula, std::set<storm::expressions::Variable>());

    // TODO: To access the internals, I replicate some of the internal behaviour of the JaniLocalEliminator. This should probably be done in a neater way by exposing the relevant bits publicly.

    auto session = JaniLocalEliminator::Session(model, property);
    UnfoldAction unfoldAction("main", "s");
    unfoldAction.doAction(session);

    EXPECT_FALSE(session.hasLoops("main", "l_s_0"));
    EXPECT_TRUE(session.hasLoops("main", "l_s_1"));
    EXPECT_TRUE(session.hasLoops("main", "l_s_2"));
    EXPECT_FALSE(session.hasLoops("main", "l_s_3"));
    EXPECT_TRUE(session.hasLoops("main", "l_s_4"));
    EXPECT_TRUE(session.hasLoops("main", "l_s_5"));
    EXPECT_FALSE(session.hasLoops("main", "l_s_6"));
}

// This test verifies that locations that can potentially satisfy the property are correctly identified.
TEST(JaniLocalEliminator, IsPartOfPropComputation) {
    auto model = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/localeliminator/three_variables.jani",
                                            storm::jani::getAllKnownModelFeatures(), boost::none).first;

    storm::parser::FormulaParser formulaParser(model.getExpressionManager().shared_from_this());
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString(
            "P=? [F (x=1 & (y>3 | z<2))]");
    auto property = storm::jani::Property("prop", formula, std::set<storm::expressions::Variable>());

    // TODO: To access the internals, I replicate some of the internal behaviour of the JaniLocalEliminator. This should probably be done in a neater way by exposing the relevant bits publicly.

    auto session = JaniLocalEliminator::Session(model, property);
    EXPECT_TRUE(session.computeIsPartOfProp("main", "l"));

    UnfoldAction unfoldActionX("main", "x");
    unfoldActionX.doAction(session);

    EXPECT_FALSE(session.computeIsPartOfProp("main", "l_x_0"));
    EXPECT_TRUE(session.computeIsPartOfProp("main", "l_x_1"));
    EXPECT_FALSE(session.computeIsPartOfProp("main", "l_x_2"));

    UnfoldAction unfoldActionY("main", "y");
    unfoldActionY.doAction(session);

    EXPECT_TRUE(session.computeIsPartOfProp("main", "l_x_1_y_4"));
    EXPECT_TRUE(session.computeIsPartOfProp("main", "l_x_1_y_3"));
    EXPECT_FALSE(session.computeIsPartOfProp("main", "l_x_2_y_4"));

    UnfoldAction unfoldActionZ("main", "z");
    unfoldActionZ.doAction(session);

    EXPECT_FALSE(session.computeIsPartOfProp("main", "l_x_0_y_2_z_3"));
    EXPECT_FALSE(session.computeIsPartOfProp("main", "l_x_0_y_2_z_1"));
    EXPECT_FALSE(session.computeIsPartOfProp("main", "l_x_0_y_5_z_3"));
    EXPECT_FALSE(session.computeIsPartOfProp("main", "l_x_0_y_5_z_1"));
    EXPECT_FALSE(session.computeIsPartOfProp("main", "l_x_1_y_2_z_3"));
    EXPECT_TRUE(session.computeIsPartOfProp("main", "l_x_1_y_2_z_1"));
    EXPECT_TRUE(session.computeIsPartOfProp("main", "l_x_1_y_5_z_3"));
    EXPECT_TRUE(session.computeIsPartOfProp("main", "l_x_1_y_5_z_1"));
    EXPECT_TRUE(session.computeIsPartOfProp("main", "l_x_1_y_4_z_1"));
    EXPECT_TRUE(session.computeIsPartOfProp("main", "l_x_1_y_5_z_0"));
}

// This test verifies that the initial location is correctly identified.
TEST(JaniLocalEliminator, IsInitialDetection) {
    auto model = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/localeliminator/initial_locations.jani",
                                            storm::jani::getAllKnownModelFeatures(), boost::none).first;

    storm::parser::FormulaParser formulaParser(model.getExpressionManager().shared_from_this());
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString(
            "P=? [F x=1]");
    auto property = storm::jani::Property("prop", formula, std::set<storm::expressions::Variable>());

    auto session = JaniLocalEliminator::Session(model, property);

    EXPECT_TRUE(session.isPossiblyInitial("main", "l"));

    UnfoldAction unfoldActionX("main", "x");
    unfoldActionX.doAction(session);

    EXPECT_FALSE(session.isPossiblyInitial("main", "l_x_0"));
    EXPECT_FALSE(session.isPossiblyInitial("main", "l_x_1"));
    EXPECT_TRUE(session.isPossiblyInitial("main", "l_x_2"));

    UnfoldAction unfoldActionY("main", "y");
    unfoldActionY.doAction(session);

    EXPECT_TRUE(session.isPossiblyInitial("main", "l_x_2_y_-1"));
    EXPECT_FALSE(session.isPossiblyInitial("main", "l_x_2_y_0"));
    EXPECT_FALSE(session.isPossiblyInitial("main", "l_x_0_y_0"));
    EXPECT_FALSE(session.isPossiblyInitial("main", "l_x_0_y_-1"));

    UnfoldAction unfoldActionZ("main", "z");
    unfoldActionZ.doAction(session);

    EXPECT_TRUE(session.isPossiblyInitial("main", "l_x_2_y_-1_z_0"));
    EXPECT_FALSE(session.isPossiblyInitial("main", "l_x_2_y_-1_z_1"));
    EXPECT_FALSE(session.isPossiblyInitial("main", "l_x_2_y_0_z_0"));
    EXPECT_FALSE(session.isPossiblyInitial("main", "l_x_1_y_-1_z_0"));

    UnfoldAction unfoldActionA("main", "a");
    unfoldActionA.doAction(session);

    EXPECT_TRUE(session.isPossiblyInitial("main", "l_x_2_y_-1_z_0_a_true"));
    EXPECT_FALSE(session.isPossiblyInitial("main", "l_x_2_y_-1_z_0_a_false"));
    EXPECT_FALSE(session.isPossiblyInitial("main", "l_x_0_y_-1_z_0_a_true"));

    UnfoldAction unfoldActionB("main", "b");
    unfoldActionB.doAction(session);

    EXPECT_TRUE(session.isPossiblyInitial("main", "l_x_2_y_-1_z_0_a_true_b_false"));
    EXPECT_FALSE(session.isPossiblyInitial("main", "l_x_2_y_-1_z_0_a_true_b_true"));
    EXPECT_FALSE(session.isPossiblyInitial("main", "l_x_2_y_0_z_0_a_true_b_true"));

    UnfoldAction unfoldActionC("main", "c");
    unfoldActionC.doAction(session);

    EXPECT_TRUE(session.isPossiblyInitial("main", "l_x_2_y_-1_z_0_a_true_b_false_c_false"));
    EXPECT_FALSE(session.isPossiblyInitial("main", "l_x_2_y_-1_z_0_a_true_b_false_c_true"));
}

// This test verifies that the eliminable locations are correctly identified.
TEST(JaniLocalEliminator, IsEliminableDetection) {
    auto model = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/localeliminator/uneliminable_locations.jani",
                                            storm::jani::getAllKnownModelFeatures(), boost::none).first;

    storm::parser::FormulaParser formulaParser(model.getExpressionManager().shared_from_this());
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString(
            "P=? [F s=4]");
    auto property = storm::jani::Property("prop", formula, std::set<storm::expressions::Variable>());

    auto session = JaniLocalEliminator::Session(model, property);

    EXPECT_FALSE(session.isEliminable("main", "l"));

    UnfoldAction unfoldActionX("main", "s");
    unfoldActionX.doAction(session);

    EXPECT_FALSE(session.isEliminable("main", "l_s_0"));
    EXPECT_FALSE(session.isEliminable("main", "l_s_1"));
    EXPECT_TRUE(session.isEliminable("main", "l_s_2"));
    EXPECT_TRUE(session.isEliminable("main", "l_s_3"));
    EXPECT_FALSE(session.isEliminable("main", "l_s_4"));
    EXPECT_FALSE(session.isEliminable("main", "l_s_5"));
}

// This test verifies that the set of variables that make up the property is correctly identified.
TEST(JaniLocalEliminator, IsVariablePartOfProperty) {
    auto model = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/localeliminator/do_nothing.jani",
                                            storm::jani::getAllKnownModelFeatures(), boost::none).first;
    {
        storm::parser::FormulaParser formulaParser(model.getExpressionManager().shared_from_this());
        std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString(
                "P=? [F x=1]");
        auto property = storm::jani::Property("prop", formula, std::set<storm::expressions::Variable>());
        auto session = JaniLocalEliminator::Session(model, property);
        EXPECT_TRUE(session.isVariablePartOfProperty("x"));
        EXPECT_FALSE(session.isVariablePartOfProperty("y"));
    }{
        storm::parser::FormulaParser formulaParser(model.getExpressionManager().shared_from_this());
        std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString(
                "P=? [F x=1 & y=1]");
        auto property = storm::jani::Property("prop", formula, std::set<storm::expressions::Variable>());
        auto session = JaniLocalEliminator::Session(model, property);
        EXPECT_TRUE(session.isVariablePartOfProperty("x"));
        EXPECT_TRUE(session.isVariablePartOfProperty("y"));
    }
}

// *******************
// Tests for unfolding
// *******************

// This test verifies that unfolding a bounded integer works correctly
TEST(JaniLocalEliminator, UnfoldingBoundedInteger) {

    auto model = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/localeliminator/simple_bounded_integer_unfolding.jani",
                                                    storm::jani::getAllKnownModelFeatures(), boost::none).first;
    storm::parser::FormulaParser formulaParser(model.getExpressionManager().shared_from_this());
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString(
            "P=? [F x=1]");
    auto property = storm::jani::Property("prop", formula, std::set<storm::expressions::Variable>());

    auto eliminator = JaniLocalEliminator(model, property, false);
    eliminator.scheduler.addAction(std::make_unique<UnfoldAction>("main", "x"));
    eliminator.eliminate();
    auto result = eliminator.getResult();
    EXPECT_EQ(5, result.getAutomaton(0).getNumberOfLocations());
    checkModel(result, {property}, std::map<storm::expressions::Variable, storm::expressions::Expression>(), 0.25);
}

// This test verifies that unfolding a boolean works correctly
TEST(JaniLocalEliminator, UnfoldingBoolean) {

    auto model = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/localeliminator/simple_bool_unfolding.jani",
                                            storm::jani::getAllKnownModelFeatures(), boost::none).first;
    storm::parser::FormulaParser formulaParser(model.getExpressionManager().shared_from_this());
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString(
            "P=? [F (!x)]");
    auto property = storm::jani::Property("prop", formula, std::set<storm::expressions::Variable>());

    auto eliminator = JaniLocalEliminator(model, property, false);
    eliminator.scheduler.addAction(std::make_unique<UnfoldAction>("main", "x"));
    eliminator.eliminate();
    auto result = eliminator.getResult();
    EXPECT_EQ(2, result.getAutomaton(0).getNumberOfLocations());
    checkModel(result, {property}, std::map<storm::expressions::Variable, storm::expressions::Expression>(), 0.875);
}

// This test verifies that trying to unfold a variable that has already been unfolded produces an error
TEST(JaniLocalEliminator, UnfoldingTwice) {
    auto model = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/localeliminator/simple_bounded_integer_unfolding.jani",
    storm::jani::getAllKnownModelFeatures(), boost::none).first;
    storm::parser::FormulaParser formulaParser(model.getExpressionManager().shared_from_this());
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString(
            "P=? [F x=1]");
    auto property = storm::jani::Property("prop", formula, std::set<storm::expressions::Variable>());

    auto eliminator = JaniLocalEliminator(model, property, false);
    eliminator.scheduler.addAction(std::make_unique<UnfoldAction>("main", "x"));
    eliminator.scheduler.addAction(std::make_unique<UnfoldAction>("main", "x"));
    STORM_SILENT_EXPECT_THROW(eliminator.eliminate(), storm::exceptions::InvalidOperationException);
}

// This test verifies that the sink isn't duplicated during unfolding and that the correct location is marked as sink
TEST(JaniLocalEliminator, UnfoldingWithSink) {
    auto modelAndProps = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/localeliminator/missing_guard.jani",
                                                    storm::jani::getAllKnownModelFeatures(), boost::none);
    auto eliminator = JaniLocalEliminator(modelAndProps.first, modelAndProps.second, true);
    eliminator.scheduler.addAction(std::make_unique<UnfoldAction>("main", "s"));
    eliminator.eliminate();
    auto result = eliminator.getResult();
    EXPECT_EQ(2, result.getAutomaton(0).getNumberOfLocations());
}

// This test verifies that whether a location can potentially satisfy the property is correctly propagated to the new
// locations during unfolding.
TEST(JaniLocalEliminator, IsPartOfPropPropagationUnfolding) {
    // TODO
}

// *********************
// Tests for elimination
// *********************

// This test verifies that combining the two guards during elimination works correctly.
TEST(JaniLocalEliminator, EliminationNewGuardTest) {

    auto modelAndProps = storm::api::parseJaniModel("/home/johannes/Documents/hiwi/out-of-control-benchmarking/files/leader_sync.5-4-custom-prop.jani",
                                                    storm::jani::getAllKnownModelFeatures(), boost::none);
    auto eliminator = JaniLocalEliminator(modelAndProps.first, modelAndProps.second, true);
    // eliminator.scheduler.addAction(std::make_unique<UnfoldAction>("main", "s"));
    eliminator.scheduler.addAction(std::make_unique<AutomaticAction>());
    eliminator.eliminate();
    // TODO
}

// This test verifies that combining the two probabilities during elimination works correctly.
TEST(JaniLocalEliminator, EliminationNewProbabilityTest) {
    // TODO
}

// This test verifies that generating a new assignment block from two existing ones works correctly.
TEST(JaniLocalEliminator, EliminationNewUpdatesTest) {
    // TODO
}

// This test tests the elimination process for a very simple model
TEST(JaniLocalEliminator, SimpleEliminationTest) {
    // TODO
}

// This test tests the elimination process if multiple edges are incoming from the location to be eliminated
TEST(JaniLocalEliminator, EliminationMultipleIncomingTest) {
    // TODO
}

// This test tests the elimination process if multiple edges are ougoing from the location to be eliminated
TEST(JaniLocalEliminator, EliminationMultipleOutgoingTest) {
    // TODO
}

// This test verifies the behaviour if multiple destinations of a single edge point to the location to be eliminated.
TEST(JaniLocalEliminator, EliminationMultiplicityTest) {
    // TODO
}

// This test tests the elimination process for a fairly complex case with multiple incoming and outgoing edges
TEST(JaniLocalEliminator, EliminationComplexTest) {
    // TODO
}

// This test tests whether guards evaluating to false are skipped during elimination
TEST(JaniLocalEliminator, EliminationFalseGuards) {
    // TODO
}

// This test verifies that whether a location can potentially satisfy the property is correctly propagated during
// elimination.
TEST(JaniLocalEliminator, IsPartOfPropPropagationElimination) {
    // TODO
}

// **************************************************
// Tests for rebuilding without unreachable locations
// **************************************************

// This test verifies that whether a location can potentially satisfy the property is correctly propagated to the
// remaining locations when rebuilding the model without unreachable locations.
TEST(JaniLocalEliminator, IsPartOfPropPropagationRebuildWithoutUnreachable) {
    // TODO
}



void checkModel(storm::jani::Model model, std::vector<storm::jani::Property> properties, std::map<storm::expressions::Variable, storm::expressions::Expression> consts, int states, int transitions, double expectedValue){
    model = model.defineUndefinedConstants(consts);
    properties[0] = properties[0].substitute(consts);

    auto formulae = storm::api::extractFormulasFromProperties(properties);
    storm::builder::BuilderOptions  options(formulae, model);
    options.setBuildAllLabels(true);
    auto explicitModel = storm::api::buildSparseModel<double>(model, options)->template as<Dtmc>();

    auto trans_matr = explicitModel->getTransitionMatrix();
    for (uint64_t  state = 0; state < explicitModel->getNumberOfStates(); state++){
        std::cout << state << "(";
        for (auto label : explicitModel->getLabelsOfState(state)){
            std::cout << label << ", ";
        }
        std::cout << ") -> ";
        for (auto trans : trans_matr.getRow(state)) {
            std::cout << trans.getValue() << ": " << trans.getColumn() << ", ";
        }
        std::cout << std::endl;

    }

    EXPECT_EQ(states, explicitModel->getNumberOfStates());
    EXPECT_EQ(transitions, explicitModel->getNumberOfTransitions());

    auto task = storm::modelchecker::CheckTask<>(*(formulae[0]), true);
    storm::Environment env;
    auto checkResult = storm::api::verifyWithSparseEngine<double>(env, explicitModel, task);
    auto quantResult = checkResult->asExplicitQuantitativeCheckResult<double>();

    auto initialStates = explicitModel->getInitialStates();
    for (auto state = initialStates.begin(); state != initialStates.end(); ++state){
        EXPECT_NEAR(expectedValue, quantResult[*state], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    }
}

//TEST(JaniLocalEliminator, NandNoActionTest) {
//    auto janiModelProperties = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/dtmc/nand.v1.jani", storm::jani::getAllKnownModelFeatures(), boost::none);
//    auto model = janiModelProperties.first;
//    auto props = janiModelProperties.second;
//
//    JaniLocalEliminator eliminator = JaniLocalEliminator(model, props);
//    eliminator.scheduler.addAction(std::make_unique<FinishAction>());
//
//    eliminator.eliminate();
//    model = eliminator.getResult();
//    model.checkValid();
//    model.finalize();
//
//    std::map<storm::expressions::Variable, storm::expressions::Expression> consts = {
//            {model.getConstant("N").getExpressionVariable(), model.getExpressionManager().integer(20)},
//            {model.getConstant("K").getExpressionVariable(), model.getExpressionManager().integer(1)}
//    };
//
//    checkModel(model, props, consts, 78332, 121512, 0.28641904638485044);
//}
//
//TEST(JaniLocalEliminator, NandUnfoldOnlyTest) {
//    auto janiModelProperties = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/dtmc/nand.v1.jani", storm::jani::getAllKnownModelFeatures(), boost::none);
//    auto model = janiModelProperties.first;
//    auto props = janiModelProperties.second;
//
//    JaniLocalEliminator eliminator = JaniLocalEliminator(model, props);
//    eliminator.scheduler.addAction(std::make_unique<UnfoldAction>("multiplex", "s"));
//    eliminator.scheduler.addAction(std::make_unique<FinishAction>());
//
//    eliminator.eliminate();
//    model = eliminator.getResult();
//    model.checkValid();
//    model.finalize();
//
//    std::map<storm::expressions::Variable, storm::expressions::Expression> consts = {
//            {model.getConstant("N").getExpressionVariable(), model.getExpressionManager().integer(20)},
//            {model.getConstant("K").getExpressionVariable(), model.getExpressionManager().integer(1)}
//    };
//
//    checkModel(model, props, consts, 78332, 121512, 0.28641904638485044);
//}
//
//TEST(JaniLocalEliminator, NandSingleElimination) {
//    auto janiModelProperties = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/dtmc/nand.v1.jani", storm::jani::getAllKnownModelFeatures(), boost::none);
//    auto model = janiModelProperties.first;
//    auto props = janiModelProperties.second;
//
//    JaniLocalEliminator eliminator = JaniLocalEliminator(model, props);
//
//    eliminator.scheduler.addAction(std::make_unique<UnfoldAction>("multiplex", "s"));
//    eliminator.scheduler.addAction(std::make_unique<EliminateAction>("multiplex", "l_s_2"));
//    eliminator.scheduler.addAction(std::make_unique<EliminateAction>("multiplex", "l_s_3"));
//    eliminator.scheduler.addAction(std::make_unique<EliminateAction>("multiplex", "l_s_1"));
//    eliminator.scheduler.addAction(std::make_unique<FinishAction>());
//
//    eliminator.eliminate();
//    model = eliminator.getResult();
//    model.checkValid();
//    model.finalize();
//
//    std::map<storm::expressions::Variable, storm::expressions::Expression> consts = {
//            {model.getConstant("N").getExpressionVariable(), model.getExpressionManager().integer(20)},
//            {model.getConstant("K").getExpressionVariable(), model.getExpressionManager().integer(1)}
//    };
//
//    checkModel(model, props, consts, 16182, 58102, 0.28641904638485044);
//}
//
//TEST(JaniLocalEliminator, NandAutoElimination) {
//    auto janiModelProperties = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/dtmc/nand.v1.jani", storm::jani::getAllKnownModelFeatures(), boost::none);
//    auto model = janiModelProperties.first;
//    auto props = janiModelProperties.second;
//
//    JaniLocalEliminator eliminator = JaniLocalEliminator(model, props);
//
//    eliminator.scheduler.addAction(std::make_unique<UnfoldAction>("multiplex", "s"));
//    eliminator.scheduler.addAction(std::make_unique<EliminateAutomaticallyAction>("multiplex", EliminateAutomaticallyAction::EliminationOrder::NewTransitionCount, 5000));
//    eliminator.scheduler.addAction(std::make_unique<FinishAction>());
//
//    eliminator.eliminate();
//    model = eliminator.getResult();
//    model.checkValid();
//    model.finalize();
//
//    std::map<storm::expressions::Variable, storm::expressions::Expression> consts = {
//            {model.getConstant("N").getExpressionVariable(), model.getExpressionManager().integer(20)},
//            {model.getConstant("K").getExpressionVariable(), model.getExpressionManager().integer(1)}
//    };
//
//    checkModel(model, props, consts, 16182, 58102, 0.28641904638485044);
//}
//
//TEST(JaniLocalEliminator, NandAutomatic) {
//    auto janiModelProperties = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/dtmc/nand.v1.jani", storm::jani::getAllKnownModelFeatures(), boost::none);
//    auto model = janiModelProperties.first;
//    auto props = janiModelProperties.second;
//
//    JaniLocalEliminator eliminator = JaniLocalEliminator(model, props);
//
//    eliminator.scheduler.addAction(std::make_unique<AutomaticAction>());
//    eliminator.scheduler.addAction(std::make_unique<FinishAction>());
//
//    eliminator.eliminate();
//    model = eliminator.getResult();
//    model.checkValid();
//    model.finalize();
//
//    std::map<storm::expressions::Variable, storm::expressions::Expression> consts = {
//            {model.getConstant("N").getExpressionVariable(), model.getExpressionManager().integer(20)},
//            {model.getConstant("K").getExpressionVariable(), model.getExpressionManager().integer(1)}
//    };
//
//    checkModel(model, props, consts, 16182, 58102, 0.28641904638485044);
//}

//TEST(JaniLocalEliminator, BoolUnfoldTest2) {
//    auto janiModelProperties = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/dtmc/bool_unfold.jani", storm::jani::getAllKnownModelFeatures(), boost::none);
//    auto model = janiModelProperties.first;
//    auto props = janiModelProperties.second;
//
//    JaniLocalEliminator eliminator = JaniLocalEliminator(model, props);
//
//    eliminator.scheduler.addAction(std::make_unique<UnfoldAction>("main", "on"));
////    eliminator.scheduler.addAction(std::make_unique<EliminateAction>("main", "l_x_2"));
////    eliminator.scheduler.addAction(std::make_unique<RebuildWithoutUnreachableAction>());
//    eliminator.scheduler.addAction(std::make_unique<FinishAction>());
//
//    eliminator.eliminate();
//    model = eliminator.getResult();
//    model.checkValid();
//    model.finalize();
//    std::map<storm::expressions::Variable, storm::expressions::Expression> consts = { };
//
//    checkModel(model, props, consts, 21, 32, 0.9990234375);
//}
//
//
//TEST(JaniLocalEliminator, CouponTest) {
//    // auto janiModelProperties = storm::api::parseJaniModel("/home/johannes/Documents/hiwi/out-of-control-benchmarking/files/synchronisation/train_signal_jani.jani", storm::jani::getAllKnownModelFeatures(), boost::none);
//    // auto janiModelProperties = storm::api::parseJaniModel("/home/johannes/Documents/hiwi/out-of-control-benchmarking/files/brp.v1.jani", storm::jani::getAllKnownModelFeatures(), boost::none);
//    auto janiModelProperties = storm::api::parseJaniModel("/home/johannes/Documents/hiwi/out-of-control-benchmarking/files/rewards_test.jani", storm::jani::getAllKnownModelFeatures(), boost::none);
//    auto model = janiModelProperties.first;
//    auto props = janiModelProperties.second;
//
//    // model = model.flattenComposition();
//
//    JaniLocalEliminator eliminator = JaniLocalEliminator(model, props);
//
//    eliminator.scheduler.addAction(std::make_unique<AutomaticAction>());
//    eliminator.scheduler.addAction(std::make_unique<FinishAction>());
//
//    eliminator.eliminate();
//
//    for (auto logLine : eliminator.getLog()){
//        std::cout << logLine << std::endl;
//    }
//
//    model = eliminator.getResult();
//    model.checkValid();
//    model.finalize();
//
//    storm::jani::JsonExporter::toFile(model, props, "/home/johannes/Documents/hiwi/out-of-control-benchmarking/files/brp_fault.jani", true, false);
//
//
//std::map<storm::expressions::Variable, storm::expressions::Expression> consts = {
//           // {model.getConstant("N").getExpressionVariable(), model.getExpressionManager().integer(2)},
//           // {model.getConstant("MAX").getExpressionVariable(), model.getExpressionManager().integer(2)}
//    };
//
//    checkModel(model, props, consts, 21, 32, 1);
//}
//
//TEST(JaniLocalEliminator, MultiplicityTest) {
//    auto janiModelProperties = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/dtmc/multiplicity.jani", storm::jani::getAllKnownModelFeatures(), boost::none);
//    auto model = janiModelProperties.first;
//    auto props = janiModelProperties.second;
//
//    JaniLocalEliminator eliminator = JaniLocalEliminator(model, props);
//
//    eliminator.scheduler.addAction(std::make_unique<UnfoldAction>("main", "x"));
//    eliminator.scheduler.addAction(std::make_unique<EliminateAction>("main", "l_x_2"));
//    eliminator.scheduler.addAction(std::make_unique<RebuildWithoutUnreachableAction>());
//    eliminator.scheduler.addAction(std::make_unique<FinishAction>());
//
//    eliminator.eliminate();
//    model = eliminator.getResult();
//    model.checkValid();
//    model.finalize();
//
//    EXPECT_EQ(model.getAutomaton("main").getEdges().size(), 0);
//
//    std::map<storm::expressions::Variable, storm::expressions::Expression> consts = {};
//
//    checkModel(model, props, consts, 31, 55, 0.04);
//}
//
//TEST(JaniLocalEliminator, BrpTest) {
//    auto janiModelProperties = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/dtmc/brp.v1.jani", storm::jani::getAllKnownModelFeatures(), boost::none);
//    storm::jani::Model model = janiModelProperties.first;
//    model = model.flattenComposition();
//    auto props = janiModelProperties.second;
//
//    JaniLocalEliminator eliminator = JaniLocalEliminator(model, props);
//
//    eliminator.scheduler.addAction(std::make_unique<AutomaticAction>());
//    eliminator.scheduler.addAction(std::make_unique<FinishAction>());
//
//    eliminator.eliminate();
//    model = eliminator.getResult();
//    model.checkValid();
//    model.finalize();
//
//    std::map<storm::expressions::Variable, storm::expressions::Expression> consts = {
//            {model.getConstant("N").getExpressionVariable(), model.getExpressionManager().integer(32)},
//            {model.getConstant("MAX").getExpressionVariable(), model.getExpressionManager().integer(3)}
//    };
//
//    checkModel(model, props, consts, 1766, 2307, 0.000025235372864445436);
//}
//
//TEST(JaniLocalEliminator, BluetoothTest) {
//    // Doesn't work, even without our unfolding or performing any model checking
//    auto janiModelProperties = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/dtmc/bluetooth.v1.jani", storm::jani::getAllKnownModelFeatures(), boost::none);
//    storm::jani::Model model = janiModelProperties.first;
//    model = model.flattenComposition();
//    auto props = janiModelProperties.second;
//
//    // JaniLocalEliminator eliminator = JaniLocalEliminator(model, props);
//
////    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::UnfoldAction>("bluetooth_flattened", "?"));
////
////    eliminator.eliminate();
////    model = eliminator.getResult();
////    model.checkValid();
////    model.finalize();
//
////    std::map<storm::expressions::Variable, storm::expressions::Expression> consts = {
////            {model.getConstant("N").getExpressionVariable(), model.getExpressionManager().integer(32)},
////            {model.getConstant("MAX").getExpressionVariable(), model.getExpressionManager().integer(3)}
////    };
//
//    // checkModel(model, props, consts, 1766, 2307, 0.000025235372864445436);
//}
//
//TEST(JaniLocalEliminator, CouponTest2) {
//    auto janiModelProperties = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/dtmc/coupon.9-4.v1.jani", storm::jani::getAllKnownModelFeatures(), boost::none);
//    auto model = janiModelProperties.first;
//    auto props = janiModelProperties.second;
//
//    JaniLocalEliminator eliminator = JaniLocalEliminator(model, props);
//
//    // eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::UnfoldAction>("main", "draw0"));
//    // eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::EliminateAutomaticallyAction>("main", JaniLocalEliminator::EliminateAutomaticallyAction::EliminationOrder::NewTransitionCount));
//    eliminator.scheduler.addAction(std::make_unique<FinishAction>());
//
//    eliminator.eliminate();
//    model = eliminator.getResult();
//    model.checkValid();
//    model.finalize();
//
//    std::map<storm::expressions::Variable, storm::expressions::Expression> consts = {
//            {model.getConstant("B").getExpressionVariable(), model.getExpressionManager().integer(5)},
//    };
//
//    checkModel(model, props, consts, 16182, 58102, 0.28641904638485044);
//}
//
//
//TEST(JaniLocalEliminator, PerformanceTest){
//    double timeEliminationTotal = 0;
//    double timeCheckingTotal = 0;
//
//    int runs = 10;
//
//    for (int i = 0; i < runs; i++) {
//        auto janiModelProperties = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/dtmc/nand.v1.jani", storm::jani::getAllKnownModelFeatures(), boost::none);
//        auto model = janiModelProperties.first;
//        auto props = janiModelProperties.second;
//
//        auto startElimination = clock();
//
//        JaniLocalEliminator eliminator = JaniLocalEliminator(model, props);
////
////        eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::UnfoldAction>("multiplex", "s"));
////        eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::EliminateAction>("multiplex", "l_s_2"));
////        eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::EliminateAction>("multiplex", "l_s_3"));
////        eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::EliminateAction>("multiplex", "l_s_1"));
//        eliminator.scheduler.addAction(std::make_unique<FinishAction>());
//
//        eliminator.eliminate();
//        model = eliminator.getResult();
//        model.checkValid();
//        model.finalize();
//
//        auto endElimination = clock();
//
//        std::map<storm::expressions::Variable, storm::expressions::Expression> consts = {
//                {model.getConstant("N").getExpressionVariable(), model.getExpressionManager().integer(20)},
//                {model.getConstant("K").getExpressionVariable(), model.getExpressionManager().integer(1)}
//        };
//
//        checkModel(model, props, consts, 16182, 58102, 0.28641904638485044);
//
//        auto endChecking = clock();
//
//        timeEliminationTotal += (double)(endElimination - startElimination) / CLOCKS_PER_SEC;
//        timeCheckingTotal += (double)(endChecking - endElimination) / CLOCKS_PER_SEC;
//    }
//
//    EXPECT_EQ(timeEliminationTotal / runs, 0);
//    EXPECT_EQ(timeCheckingTotal / runs, 0);
//}