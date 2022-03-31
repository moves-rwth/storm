#include <storm-pars/analysis/OrderExtender.h>
#include <storm-pars/analysis/ReachabilityOrderExtenderMdp.h>
#include "storm-config.h"

#include "storm-pars/api/storm-pars.h"
#include "storm-pars/transformer/SparseParametricDtmcSimplifier.h"

#include "storm-parsers/api/storm-parsers.h"
#include "storm-parsers/parser/AutoParser.h"
#include "storm-parsers/parser/PrismParser.h"

#include "storm/api/builder.h"
#include "storm/api/storm.h"
#include "storm/logic/Formulas.h"
#include "storm/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/models/sparse/StandardRewardModel.h"

#include "test/storm_gtest.h"
#include "storm-pars/transformer/SparseParametricMdpSimplifier.h"

TEST(ReachabilityOrderExtenderMdpTest, SimpleCaseCheck1_max) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pmdp/simpleCaseTest1.nm";
    std::string formulaAsString = "Pmax=? [F s=5]";
    std::string constantsAsString = "";

    // Program and formula
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Mdp<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Mdp<storm::RationalFunction>>();

    ASSERT_EQ(7, model->getNumberOfStates());
    auto vars = storm::models::sparse::getProbabilityParameters(*model);
    auto region = storm::api::parseRegion<storm::RationalFunction>("0.00001 <= p <= 0.999999, 0.00001 <= q <= 0.999999", vars);
    std::vector<storm::storage::ParameterRegion<storm::RationalFunction>> regions = {region};

    auto extender = storm::analysis::ReachabilityOrderExtenderMdp<storm::RationalFunction, double>(model, formulas[0], true);

    auto monRes = new storm::analysis::MonotonicityResult<typename storm::analysis::ReachabilityOrderExtenderMdp<storm::RationalFunction, double>::VariableType>;
    auto criticalTuple = extender.toOrder(region, false, make_shared<storm::analysis::MonotonicityResult<typename storm::analysis::ReachabilityOrderExtenderMdp<storm::RationalFunction, double>::VariableType>>(*monRes));
    EXPECT_EQ(model->getNumberOfStates(), std::get<1>(criticalTuple));
    EXPECT_EQ(model->getNumberOfStates(), std::get<2>(criticalTuple));

    auto order = std::get<0>(criticalTuple);
    for (uint_fast64_t i = 0; i < model->getNumberOfStates(); ++i) {
        EXPECT_TRUE(order->contains(i));
    }

    // Check nodes
    EXPECT_TRUE(order->isTopState(5));
    EXPECT_TRUE(order->isBottomState(6));

    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(5, 1));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(1, 0));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(0, 2));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(2, 3));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(3, 4));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(4, 6));
}

TEST(ReachabilityOrderExtenderMdpTest, SimpleCaseCheck1_min) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pmdp/simpleCaseTest1.nm";
    std::string formulaAsString = "Pmin=? [F s=5]";
    std::string constantsAsString = "";

    // Program and formula
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Mdp<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Mdp<storm::RationalFunction>>();

    ASSERT_EQ(7, model->getNumberOfStates());
    auto vars = storm::models::sparse::getProbabilityParameters(*model);
    auto region = storm::api::parseRegion<storm::RationalFunction>("0.00001 <= p <= 0.999999, 0.00001 <= q <= 0.999999", vars);
    std::vector<storm::storage::ParameterRegion<storm::RationalFunction>> regions = {region};

    auto extender = storm::analysis::ReachabilityOrderExtenderMdp<storm::RationalFunction, double>(model, formulas[0], false);

    auto monRes = new storm::analysis::MonotonicityResult<typename storm::analysis::ReachabilityOrderExtenderMdp<storm::RationalFunction, double>::VariableType>;
    auto criticalTuple = extender.toOrder(region, false, make_shared<storm::analysis::MonotonicityResult<typename storm::analysis::ReachabilityOrderExtenderMdp<storm::RationalFunction, double>::VariableType>>(*monRes));
    EXPECT_EQ(model->getNumberOfStates(), std::get<1>(criticalTuple));
    EXPECT_EQ(model->getNumberOfStates(), std::get<2>(criticalTuple));

    auto order = std::get<0>(criticalTuple);
    for (uint_fast64_t i = 0; i < model->getNumberOfStates(); ++i) {
        EXPECT_TRUE(order->contains(i));
    }

    // Check nodes
    EXPECT_TRUE(order->isTopState(5));
    EXPECT_TRUE(order->isBottomState(6));

    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(5, 1));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(1, 0));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(0, 2));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(2, 3));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(3, 4));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(4, 6));

}

TEST(ReachabilityOrderExtenderMdpTest, SimpleCaseChecWithPLA_max) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pmdp/simpleCaseTest2.nm";
    std::string formulaAsString = "Pmax=? [F s=5]";
    std::string constantsAsString = "";

    // Program and formula
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Mdp<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Mdp<storm::RationalFunction>>();

    ASSERT_EQ(7, model->getNumberOfStates());
    auto vars = storm::models::sparse::getProbabilityParameters(*model);
    auto region = storm::api::parseRegion<storm::RationalFunction>("0.00001 <= p <= 0.999999, 0.00001 <= q <= 0.999999", vars);
    std::vector<storm::storage::ParameterRegion<storm::RationalFunction>> regions = {region};

    auto extender = storm::analysis::ReachabilityOrderExtenderMdp<storm::RationalFunction, double>(model, formulas[0], true);
    extender.initializeMinMaxValues(region);

    auto monRes = new storm::analysis::MonotonicityResult<typename storm::analysis::ReachabilityOrderExtenderMdp<storm::RationalFunction, double>::VariableType>;
    auto criticalTuple = extender.toOrder(region, false, std::make_shared<storm::analysis::MonotonicityResult<typename storm::analysis::ReachabilityOrderExtenderMdp<storm::RationalFunction, double>::VariableType>>(*monRes));
    EXPECT_EQ(model->getNumberOfStates(), std::get<1>(criticalTuple));
    EXPECT_EQ(model->getNumberOfStates(), std::get<2>(criticalTuple));

    auto order = std::get<0>(criticalTuple);
    for (uint_fast64_t i = 0; i < model->getNumberOfStates(); ++i) {
        EXPECT_TRUE(order->contains(i));
    }

    // Check nodes
    EXPECT_TRUE(order->isTopState(5));
    EXPECT_TRUE(order->isBottomState(6));

    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(5, 1));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(1, 0));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(0, 2));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(2, 3));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(3, 4));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(4, 6));

}
TEST(ReachabilityOrderExtenderMdpTest, SimpleCaseChecWithPLA_min) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pmdp/simpleCaseTest2.nm";
    std::string formulaAsString = "Pmin=? [F s=5]";
    std::string constantsAsString = "";

    // Program and formula
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Mdp<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Mdp<storm::RationalFunction>>();

    ASSERT_EQ(7, model->getNumberOfStates());
    auto vars = storm::models::sparse::getProbabilityParameters(*model);
    auto region = storm::api::parseRegion<storm::RationalFunction>("0.00001 <= p <= 0.999999, 0.00001 <= q <= 0.999999", vars);
    std::vector<storm::storage::ParameterRegion<storm::RationalFunction>> regions = {region};

    auto extender = storm::analysis::ReachabilityOrderExtenderMdp<storm::RationalFunction, double>(model, formulas[0], false);
    extender.initializeMinMaxValues(region);

    auto monRes = new storm::analysis::MonotonicityResult<typename storm::analysis::ReachabilityOrderExtenderMdp<storm::RationalFunction, double>::VariableType>;
    auto criticalTuple = extender.toOrder(region, false, std::make_shared<storm::analysis::MonotonicityResult<typename storm::analysis::ReachabilityOrderExtenderMdp<storm::RationalFunction, double>::VariableType>>(*monRes));
    EXPECT_EQ(model->getNumberOfStates(), std::get<1>(criticalTuple));
    EXPECT_EQ(model->getNumberOfStates(), std::get<2>(criticalTuple));

    auto order = std::get<0>(criticalTuple);
    for (uint_fast64_t i = 0; i < model->getNumberOfStates(); ++i) {
        EXPECT_TRUE(order->contains(i));
    }

    // Check nodes
    EXPECT_TRUE(order->isTopState(5));
    EXPECT_TRUE(order->isBottomState(6));

    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(5, 1));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(1, 0));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(0, 2));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(2, 3));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(3, 4));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(4, 6));

}

TEST(ReachabilityOrderExtenderMdpTest, SmtInvolvedChecking_max) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pmdp/test1.nm";
    std::string formulaAsString = "Pmax=? [F s=5]";
    std::string constantsAsString = "";

    // Program and formula
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Mdp<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Mdp<storm::RationalFunction>>();

    ASSERT_EQ(6, model->getNumberOfStates());
    auto vars = storm::models::sparse::getProbabilityParameters(*model);
    auto region = storm::api::parseRegion<storm::RationalFunction>("0.00001 <= p <= 0.999999, 0.00001 <= q <= 0.999999", vars);
    std::vector<storm::storage::ParameterRegion<storm::RationalFunction>> regions = {region};

    auto extender = storm::analysis::ReachabilityOrderExtenderMdp<storm::RationalFunction, double>(model, formulas[0], true);

    auto monRes = new storm::analysis::MonotonicityResult<typename storm::analysis::ReachabilityOrderExtenderMdp<storm::RationalFunction, double>::VariableType>;
    auto criticalTuple = extender.toOrder(region, false, std::make_shared<storm::analysis::MonotonicityResult<typename storm::analysis::ReachabilityOrderExtenderMdp<storm::RationalFunction, double>::VariableType>>(*monRes));
    EXPECT_EQ(model->getNumberOfStates(), std::get<1>(criticalTuple));
    EXPECT_EQ(model->getNumberOfStates(), std::get<2>(criticalTuple));

    auto order = std::get<0>(criticalTuple);
    for (uint_fast64_t i = 0; i < model->getNumberOfStates(); ++i) {
        EXPECT_TRUE(order->contains(i));
    }
    std::cout << model->getTransitionMatrix() << std::endl;

    // Check nodes, 3 and 4 are swapped due to preprocessing
    EXPECT_TRUE(order->isTopState(5));
    EXPECT_TRUE(order->isBottomState(3));
    EXPECT_EQ(0, order->getActionAtState(0));
    EXPECT_EQ(0, order->getActionAtState(1));
    EXPECT_FALSE(order->isActionSetAtState(2));

    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, order->compare(0, 1));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, order->compare(0, 2));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(0, 3));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, order->compare(0, 4));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, order->compare(0, 5));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, order->compare(1, 2));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(1, 3));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(1, 4));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, order->compare(1, 5));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(2, 3));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, order->compare(2, 4));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, order->compare(2, 5));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, order->compare(3, 4));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, order->compare(3, 5));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(4, 5));

}

TEST(ReachabilityOrderExtenderMdpTest, SmtInvolvedChecking_min) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pmdp/test1.nm"; // TODO Write this
    std::string formulaAsString = "Pmin=? [F s=5]";
    std::string constantsAsString = "";

    // Program and formula
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Mdp<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Mdp<storm::RationalFunction>>();

    ASSERT_EQ(6, model->getNumberOfStates());
    auto vars = storm::models::sparse::getProbabilityParameters(*model);
    auto region = storm::api::parseRegion<storm::RationalFunction>("0.00001 <= p <= 0.999999, 0.00001 <= q <= 0.999999", vars);
    std::vector<storm::storage::ParameterRegion<storm::RationalFunction>> regions = {region};

    auto extender = storm::analysis::ReachabilityOrderExtenderMdp<storm::RationalFunction, double>(model, formulas[0], false);

    auto monRes = new storm::analysis::MonotonicityResult<typename storm::analysis::ReachabilityOrderExtenderMdp<storm::RationalFunction, double>::VariableType>;
    auto criticalTuple = extender.toOrder(region, false, std::make_shared<storm::analysis::MonotonicityResult<typename storm::analysis::ReachabilityOrderExtenderMdp<storm::RationalFunction, double>::VariableType>>(*monRes));
    EXPECT_EQ(model->getNumberOfStates(), std::get<1>(criticalTuple));
    EXPECT_EQ(model->getNumberOfStates(), std::get<2>(criticalTuple));

    auto order = std::get<0>(criticalTuple);
    for (uint_fast64_t i = 0; i < model->getNumberOfStates(); ++i) {
        EXPECT_TRUE(order->contains(i));
    }

    // Check nodes
    EXPECT_TRUE(order->isTopState(4));
    EXPECT_TRUE(order->isBottomState(5));

    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(4, 1));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(1, 3));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(1, 0));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(0, 2));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(3, 5));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(2, 5));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, order->compare(3, 0));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, order->compare(3, 2));
}