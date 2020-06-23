#include "test/storm_gtest.h"
#include "storm-config.h"
#include "test/storm_gtest.h"

#include "storm-parsers/parser/FormulaParser.h"
#include "storm/logic/Formulas.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm-parsers/parser/AutoParser.h"
#include "storm-parsers/parser/PrismParser.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/api/builder.h"

#include "storm-pars/transformer/SparseParametricDtmcSimplifier.h"

#include "storm-pars/api/storm-pars.h"
#include "storm/api/storm.h"
#include "storm-parsers/api/storm-parsers.h"

//TEST(OrderExtenderTest, Brp_with_bisimulation_on_model) {
//    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/brp16_2.pm";
//    std::string formulaAsString = "P=? [F s=4 & i=N ]";
//    std::string constantsAsString = ""; //e.g. pL=0.9,TOACK=0.5
//
//    // Program and formula
//    storm::prism::Program program = storm::api::parseProgram(programFile);
//    program = storm::utility::prism::preprocess(program, constantsAsString);
//    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
//    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
//    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
//    auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>>(*dtmc);
//    ASSERT_TRUE(simplifier.simplify(*(formulas[0])));
//    model = simplifier.getSimplifiedModel();
//
//    // Apply bisimulation
//    storm::storage::BisimulationType bisimType = storm::storage::BisimulationType::Strong;
//    if (storm::settings::getModule<storm::settings::modules::BisimulationSettings>().isWeakBisimulationSet()) {
//        bisimType = storm::storage::BisimulationType::Weak;
//    }
//
//    dtmc = storm::api::performBisimulationMinimization<storm::RationalFunction>(model, formulas, bisimType)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
//
//    ASSERT_EQ(dtmc->getNumberOfStates(), 99);
//    ASSERT_EQ(dtmc->getNumberOfTransitions(), 195);
//
//    auto vars = storm::models::sparse::getProbabilityParameters(*dtmc);
//    auto region = storm::api::parseRegion<storm::RationalFunction>("0.00001 <= pK <= 0.999999, 0.00001 <= pL <= 0.999999", vars);
//
//    auto extender = storm::analysis::OrderExtender<storm::RationalFunction, double>(dtmc, formulas[0], region);
//    auto monRes = new storm::analysis::MonotonicityResult<typename storm::analysis::OrderExtender<storm::RationalFunction, double>::VariableType>;
//    auto criticalTuple = extender.toOrder(make_shared<storm::analysis::MonotonicityResult<typename storm::analysis::OrderExtender<storm::RationalFunction, double>::VariableType>>(*monRes));
//    EXPECT_EQ(dtmc->getNumberOfStates(), std::get<1>(criticalTuple));
//    EXPECT_EQ(dtmc->getNumberOfStates(), std::get<2>(criticalTuple));
//
//    auto order = std::get<0>(criticalTuple);
//    for (uint_fast64_t i = 0; i < dtmc->getNumberOfStates(); ++i) {
//        EXPECT_TRUE((*order->getAddedStates())[i]);
//    }
//
//    // Check on some nodes
//    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(1,0));
//    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(1,5));
//    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(5,0));
//    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(94,5));
//    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, order->compare(7,13));
//}
//
//TEST(OrderExtenderTest, Brp_without_bisimulation_on_model) {
//    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/brp16_2.pm";
//    std::string formulaAsString = "P=? [F s=4 & i=N ]";
//    std::string constantsAsString = ""; //e.g. pL=0.9,TOACK=0.5
//
//    // Program and formula
//    storm::prism::Program program = storm::api::parseProgram(programFile);
//    program = storm::utility::prism::preprocess(program, constantsAsString);
//    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
//    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
//    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
//    auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>>(*dtmc);
//    ASSERT_TRUE(simplifier.simplify(*(formulas[0])));
//    model = simplifier.getSimplifiedModel();
//    dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
//
//    ASSERT_EQ(dtmc->getNumberOfStates(), 193);
//    ASSERT_EQ(dtmc->getNumberOfTransitions(), 383);
//
//    auto vars = storm::models::sparse::getProbabilityParameters(*dtmc);
//    auto region = storm::api::parseRegion<storm::RationalFunction>("0.00001 <= pK <= 0.999999, 0.00001 <= pL <= 0.999999", vars);
//
//    auto extender = storm::analysis::OrderExtender<storm::RationalFunction, double>(dtmc, formulas[0], region);
//    auto monRes = new storm::analysis::MonotonicityResult<typename storm::analysis::OrderExtender<storm::RationalFunction, double>::VariableType>;
//    auto criticalTuple = extender.toOrder(make_shared<storm::analysis::MonotonicityResult<typename storm::analysis::OrderExtender<storm::RationalFunction, double>::VariableType>>(*monRes));
//    EXPECT_EQ(183ul, std::get<1>(criticalTuple));
//    EXPECT_EQ(186ul, std::get<2>(criticalTuple));
//}
//
//TEST(OrderExtenderTest, Brp_with_bisimulation_on_matrix) {
//    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/brp16_2.pm";
//    std::string formulaAsString = "P=? [F s=4 & i=N ]";
//    std::string constantsAsString = ""; //e.g. pL=0.9,TOACK=0.5
//
//    // Program and formula
//    storm::prism::Program program = storm::api::parseProgram(programFile);
//    program = storm::utility::prism::preprocess(program, constantsAsString);
//    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
//    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
//    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
//    auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>>(*dtmc);
//    ASSERT_TRUE(simplifier.simplify(*(formulas[0])));
//    model = simplifier.getSimplifiedModel();
//
//    // Apply bisimulation
//    storm::storage::BisimulationType bisimType = storm::storage::BisimulationType::Strong;
//    if (storm::settings::getModule<storm::settings::modules::BisimulationSettings>().isWeakBisimulationSet()) {
//        bisimType = storm::storage::BisimulationType::Weak;
//    }
//
//    dtmc = storm::api::performBisimulationMinimization<storm::RationalFunction>(model, formulas, bisimType)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
//
//    ASSERT_EQ(dtmc->getNumberOfStates(), 99);
//    ASSERT_EQ(dtmc->getNumberOfTransitions(), 195);
//
//    auto vars = storm::models::sparse::getProbabilityParameters(*dtmc);
//    auto region = storm::api::parseRegion<storm::RationalFunction>("0.00001 <= pK <= 0.999999, 0.00001 <= pL <= 0.999999", vars);
//    storm::modelchecker::SparsePropositionalModelChecker<storm::models::sparse::Model<storm::RationalFunction>> propositionalChecker(*model);
//    storm::storage::BitVector phiStates;
//    storm::storage::BitVector psiStates;
//    phiStates = storm::storage::BitVector(model->getTransitionMatrix().getRowCount(), true);
//    storm::logic::EventuallyFormula formula = formulas[0]->asProbabilityOperatorFormula().getSubformula().asEventuallyFormula();
//    psiStates = propositionalChecker.check(formula.getSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
//    // Get the maybeStates
//    std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01 = storm::utility::graph::performProb01(model->getBackwardTransitions(), phiStates, psiStates);
//    storm::storage::BitVector topStates = statesWithProbability01.second;
//    storm::storage::BitVector bottomStates = statesWithProbability01.first;
//
//    auto extender = storm::analysis::OrderExtender<storm::RationalFunction, double>(&topStates, &bottomStates, model->getTransitionMatrix());
//    auto order = extender.extendOrder(nullptr, region);
//    EXPECT_EQ(order->getNumberOfAddedStates(), dtmc->getNumberOfStates());
//    EXPECT_TRUE(order->getDoneBuilding());
//
//    // Check on some nodes
//    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(1,0));
//    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(1,5));
//    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(5,0));
//    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(94,5));
//    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, order->compare(7,13));
//}
//
//TEST(OrderExtenderTest, Brp_without_bisimulation_on_matrix) {
//    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/brp16_2.pm";
//    std::string formulaAsString = "P=? [F s=4 & i=N ]";
//    std::string constantsAsString = ""; //e.g. pL=0.9,TOACK=0.5
//
//    // Program and formula
//    storm::prism::Program program = storm::api::parseProgram(programFile);
//    program = storm::utility::prism::preprocess(program, constantsAsString);
//    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
//    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
//    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
//    auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>>(*dtmc);
//    ASSERT_TRUE(simplifier.simplify(*(formulas[0])));
//    model = simplifier.getSimplifiedModel();
//    dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
//
//    ASSERT_EQ(dtmc->getNumberOfStates(), 193);
//    ASSERT_EQ(dtmc->getNumberOfTransitions(), 383);
//
//    auto vars = storm::models::sparse::getProbabilityParameters(*dtmc);
//    auto region = storm::api::parseRegion<storm::RationalFunction>("0.00001 <= pK <= 0.999999, 0.00001 <= pL <= 0.999999", vars);
//    storm::modelchecker::SparsePropositionalModelChecker<storm::models::sparse::Model<storm::RationalFunction>> propositionalChecker(*model);
//    storm::storage::BitVector phiStates;
//    storm::storage::BitVector psiStates;
//    phiStates = storm::storage::BitVector(model->getTransitionMatrix().getRowCount(), true);
//    storm::logic::EventuallyFormula formula = formulas[0]->asProbabilityOperatorFormula().getSubformula().asEventuallyFormula();
//    psiStates = propositionalChecker.check(formula.getSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
//    // Get the maybeStates
//    std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01 = storm::utility::graph::performProb01(model->getBackwardTransitions(), phiStates, psiStates);
//    storm::storage::BitVector topStates = statesWithProbability01.second;
//    storm::storage::BitVector bottomStates = statesWithProbability01.first;
//
//    auto extender = storm::analysis::OrderExtender<storm::RationalFunction, double>(&topStates, &bottomStates, model->getTransitionMatrix());
//    auto order = extender.extendOrder(nullptr, region);
//    EXPECT_FALSE(order->getAddedStates()->full());
//    EXPECT_FALSE(order->getDoneBuilding());
//}
TEST(OrderExtenderTest, simple1_on_model) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/simple1.pm";
    std::string formulaAsString = "P=? [F s=3 ]";
    std::string constantsAsString = "";

    // model
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>>(*dtmc);
    ASSERT_TRUE(simplifier.simplify(*(formulas[0])));
    model = simplifier.getSimplifiedModel();

    // Create the region
    auto modelParameters = storm::models::sparse::getProbabilityParameters(*model);
    auto region=storm::api::parseRegion<storm::RationalFunction>("0.51<=p<=0.9", modelParameters);

    auto extender = storm::analysis::OrderExtender<storm::RationalFunction, double>(dtmc, formulas[0], region);
    auto order = std::get<0>(extender.toOrder());
    EXPECT_EQ(order->getNumberOfAddedStates(), 4);
    EXPECT_FALSE(order->getDoneBuilding());

    // Check on all states
    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, order->compare(3,0));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(3,1));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(3,2));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(3,4));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, order->compare(1,0));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, order->compare(1,2));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(1,4));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, order->compare(0,2));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, order->compare(0,4));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(2,4));
}

TEST(OrderExtenderTest, simple1_on_matrix) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/simple1.pm";
    std::string formulaAsString = "P=? [F s=3 ]";
    std::string constantsAsString = "";

    // model
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>>(*dtmc);
    ASSERT_TRUE(simplifier.simplify(*(formulas[0])));
    model = simplifier.getSimplifiedModel();

    // Create the region
    auto modelParameters = storm::models::sparse::getProbabilityParameters(*model);
    auto region=storm::api::parseRegion<storm::RationalFunction>("0.51<=p<=0.9", modelParameters);

    // For order extender
    storm::modelchecker::SparsePropositionalModelChecker<storm::models::sparse::Model<storm::RationalFunction>> propositionalChecker(*model);
    storm::storage::BitVector phiStates;
    storm::storage::BitVector psiStates;
    phiStates = storm::storage::BitVector(model->getTransitionMatrix().getRowCount(), true);
    storm::logic::EventuallyFormula formula = formulas[0]->asProbabilityOperatorFormula().getSubformula().asEventuallyFormula();
    psiStates = propositionalChecker.check(formula.getSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
    // Get the maybeStates
    std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01 = storm::utility::graph::performProb01(model->getBackwardTransitions(), phiStates, psiStates);
    storm::storage::BitVector topStates = statesWithProbability01.second;
    storm::storage::BitVector bottomStates = statesWithProbability01.first;

    // OrderExtender
    auto extender = storm::analysis::OrderExtender<storm::RationalFunction, double>(&topStates, &bottomStates, model->getTransitionMatrix());
    auto order = extender.extendOrder(nullptr, region);
    EXPECT_EQ(order->getNumberOfAddedStates(), dtmc->getNumberOfStates());
    EXPECT_TRUE(order->getDoneBuilding());

    // Check on all states
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(3,0));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(3,1));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(3,2));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(3,4));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(1,0));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(1,2));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(1,4));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(0,2));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(0,4));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order->compare(2,4));
}



