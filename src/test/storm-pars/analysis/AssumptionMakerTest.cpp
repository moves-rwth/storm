//
// Created by Jip Spel on 20.09.18.
//

// TODO: cleanup includes
#include "test/storm_gtest.h"
#include "storm-config.h"
#include "test/storm_gtest.h"

#include "storm-parsers/parser/FormulaParser.h"
#include "storm/logic/Formulas.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm-parsers/parser/AutoParser.h"
#include "storm-parsers/parser/PrismParser.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/api/builder.h"

#include "storm-pars/analysis/Order.h"
#include "storm-pars/analysis/AssumptionMaker.h"
#include "storm-pars/analysis/AssumptionChecker.h"
#include "storm-pars/analysis/OrderExtender.h"
#include "storm-pars/transformer/SparseParametricDtmcSimplifier.h"

#include "storm-pars/api/storm-pars.h"
#include "storm/api/storm.h"

#include "storm-parsers/api/storm-parsers.h"

TEST(AssumptionMakerTest, Brp_without_bisimulation) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/brp16_2.pm";
    std::string formulaAsString = "P=? [F s=4 & i=N ]";
    std::string constantsAsString = ""; //e.g. pL=0.9,TOACK=0.5

    // Program and formula
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>>(*dtmc);
    ASSERT_TRUE(simplifier.simplify(*(formulas[0])));
    model = simplifier.getSimplifiedModel();
    dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();

    // Create the region
    auto vars = storm::models::sparse::getProbabilityParameters(*dtmc);
    auto region = storm::api::parseRegion<storm::RationalFunction>("0.00001 <= pK <= 0.999999, 0.00001 <= pL <= 0.999999", vars);

    ASSERT_EQ(dtmc->getNumberOfStates(), 193ull);
    ASSERT_EQ(dtmc->getNumberOfTransitions(), 383ull);

    auto *extender = new storm::analysis::OrderExtender<storm::RationalFunction>(dtmc);
    auto criticalTuple = extender->toOrder(formulas);
    ASSERT_EQ(183ul, std::get<1>(criticalTuple));
    ASSERT_EQ(186ul, std::get<2>(criticalTuple));

    auto assumptionChecker = storm::analysis::AssumptionChecker<storm::RationalFunction>(formulas[0], dtmc, region, 3);
    auto assumptionMaker = storm::analysis::AssumptionMaker<storm::RationalFunction>(&assumptionChecker, dtmc->getNumberOfStates(), true);
    auto result = assumptionMaker.createAndCheckAssumption(std::get<1>(criticalTuple), std::get<2>(criticalTuple), std::get<0>(criticalTuple));

    EXPECT_EQ(3ul, result.size());

    bool foundFirst = false;
    bool foundSecond = false;
    bool foundThird = false;
    for (auto itr = result.begin(); itr != result.end(); ++itr) {
        if (!foundFirst && itr->first->getFirstOperand()->asVariableExpression().getVariable().getName() == "183") {
            EXPECT_EQ(storm::analysis::AssumptionStatus::INVALID, itr->second);
            EXPECT_EQ(true, itr->first->getFirstOperand()->isVariable());
            EXPECT_EQ(true, itr->first->getSecondOperand()->isVariable());
            EXPECT_EQ("186", itr->first->getSecondOperand()->asVariableExpression().getVariable().getName());
            EXPECT_EQ(storm::expressions::BinaryRelationExpression::RelationType::Greater, itr->first->getRelationType());
            foundFirst = true;
        } else if (!foundSecond && itr->first->getRelationType() == storm::expressions::BinaryRelationExpression::RelationType::Greater) {
            EXPECT_EQ(storm::analysis::AssumptionStatus::UNKNOWN, itr->second);
            EXPECT_EQ(true, itr->first->getFirstOperand()->isVariable());
            EXPECT_EQ(true, itr->first->getSecondOperand()->isVariable());
            EXPECT_EQ("186", itr->first->getFirstOperand()->asVariableExpression().getVariable().getName());
            EXPECT_EQ("183", itr->first->getSecondOperand()->asVariableExpression().getVariable().getName());
            EXPECT_EQ(storm::expressions::BinaryRelationExpression::RelationType::Greater, itr->first->getRelationType());
            foundSecond = true;
        } else if (!foundThird && itr->first->getRelationType() == storm::expressions::BinaryRelationExpression::RelationType::Equal) {
            EXPECT_EQ(storm::analysis::AssumptionStatus::INVALID, itr->second);
            EXPECT_EQ(true, itr->first->getFirstOperand()->isVariable());
            EXPECT_EQ(true, itr->first->getSecondOperand()->isVariable());
            EXPECT_EQ("186", itr->first->getFirstOperand()->asVariableExpression().getVariable().getName());
            EXPECT_EQ("183", itr->first->getSecondOperand()->asVariableExpression().getVariable().getName());
            EXPECT_EQ(storm::expressions::BinaryRelationExpression::RelationType::Equal, itr->first->getRelationType());
            foundThird = true;
        } else {
            EXPECT_TRUE(false);
        }
    }
    EXPECT_TRUE(foundFirst);
    EXPECT_TRUE(foundSecond);
    EXPECT_TRUE(foundThird);
}

TEST(AssumptionMakerTest, Brp_without_bisimulation_no_validation) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/brp16_2.pm";
    std::string formulaAsString = "P=? [F s=4 & i=N ]";
    std::string constantsAsString = ""; //e.g. pL=0.9,TOACK=0.5

    // Program and formula
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>>(*dtmc);
    ASSERT_TRUE(simplifier.simplify(*(formulas[0])));
    model = simplifier.getSimplifiedModel();
    dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();

    // Create the region
    auto vars = storm::models::sparse::getProbabilityParameters(*dtmc);
    auto region = storm::api::parseRegion<storm::RationalFunction>("0.00001 <= pK <= 0.999999, 0.00001 <= pL <= 0.999999", vars);

    ASSERT_EQ(dtmc->getNumberOfStates(), 193ull);
    ASSERT_EQ(dtmc->getNumberOfTransitions(), 383ull);

    auto *extender = new storm::analysis::OrderExtender<storm::RationalFunction>(dtmc);
    auto criticalTuple = extender->toOrder(formulas);
    ASSERT_EQ(183ul, std::get<1>(criticalTuple));
    ASSERT_EQ(186ul, std::get<2>(criticalTuple));

    auto assumptionChecker = storm::analysis::AssumptionChecker<storm::RationalFunction>(formulas[0], dtmc, region, 3);
    // This one does not validate the assumptions!
    auto assumptionMaker = storm::analysis::AssumptionMaker<storm::RationalFunction>(&assumptionChecker, dtmc->getNumberOfStates(), false);
    auto result = assumptionMaker.createAndCheckAssumption(std::get<1>(criticalTuple), std::get<2>(criticalTuple), std::get<0>(criticalTuple));

    EXPECT_EQ(3ul, result.size());

    bool foundFirst = false;
    bool foundSecond = false;
    bool foundThird = false;
    for (auto itr = result.begin(); itr != result.end(); ++itr) {
        if (!foundFirst && itr->first->getFirstOperand()->asVariableExpression().getVariable().getName() == "183") {
            EXPECT_EQ(storm::analysis::AssumptionStatus::UNKNOWN, itr->second);
            EXPECT_EQ(true, itr->first->getFirstOperand()->isVariable());
            EXPECT_EQ(true, itr->first->getSecondOperand()->isVariable());
            EXPECT_EQ("186", itr->first->getSecondOperand()->asVariableExpression().getVariable().getName());
            EXPECT_EQ(storm::expressions::BinaryRelationExpression::RelationType::Greater, itr->first->getRelationType());
            foundFirst = true;
        } else if (!foundSecond && itr->first->getRelationType() == storm::expressions::BinaryRelationExpression::RelationType::Greater) {
            EXPECT_EQ(storm::analysis::AssumptionStatus::UNKNOWN, itr->second);
            EXPECT_EQ(true, itr->first->getFirstOperand()->isVariable());
            EXPECT_EQ(true, itr->first->getSecondOperand()->isVariable());
            EXPECT_EQ("186", itr->first->getFirstOperand()->asVariableExpression().getVariable().getName());
            EXPECT_EQ("183", itr->first->getSecondOperand()->asVariableExpression().getVariable().getName());
            EXPECT_EQ(storm::expressions::BinaryRelationExpression::RelationType::Greater, itr->first->getRelationType());
            foundSecond = true;
        } else if (!foundThird && itr->first->getRelationType() == storm::expressions::BinaryRelationExpression::RelationType::Equal) {
            EXPECT_EQ(storm::analysis::AssumptionStatus::UNKNOWN, itr->second);
            EXPECT_EQ(true, itr->first->getFirstOperand()->isVariable());
            EXPECT_EQ(true, itr->first->getSecondOperand()->isVariable());
            EXPECT_EQ("186", itr->first->getFirstOperand()->asVariableExpression().getVariable().getName());
            EXPECT_EQ("183", itr->first->getSecondOperand()->asVariableExpression().getVariable().getName());
            EXPECT_EQ(storm::expressions::BinaryRelationExpression::RelationType::Equal, itr->first->getRelationType());
            foundThird = true;
        } else {
            EXPECT_TRUE(false);
        }
    }
    EXPECT_TRUE(foundFirst);
    EXPECT_TRUE(foundSecond);
    EXPECT_TRUE(foundThird);
}

TEST(AssumptionMakerTest, Simple1) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/simple1.pm";
    std::string formulaAsString = "P=? [F s=3]";
    std::string constantsAsString = ""; //e.g. pL=0.9,TOACK=0.5

    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>>(*dtmc);
    ASSERT_TRUE(simplifier.simplify(*(formulas[0])));
    model = simplifier.getSimplifiedModel();
    dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();

    // Create the region
    auto vars = storm::models::sparse::getProbabilityParameters(*dtmc);
    auto region = storm::api::parseRegion<storm::RationalFunction>("0.00001 <= p <= 0.999999", vars);

    ASSERT_EQ(dtmc->getNumberOfStates(), 5ul);
    ASSERT_EQ(dtmc->getNumberOfTransitions(), 8ul);

    storm::storage::BitVector above(5);
    above.set(3);
    storm::storage::BitVector below(5);
    below.set(4);
    storm::storage::BitVector initialMiddle(5);
    std::vector<uint_fast64_t> statesSorted = storm::utility::graph::getTopologicalSort(model->getTransitionMatrix());

    auto order = new storm::analysis::Order(&above, &below, &initialMiddle, 5, &statesSorted);

    auto assumptionChecker = storm::analysis::AssumptionChecker<storm::RationalFunction>(formulas[0], dtmc, region, 3);
    auto assumptionMaker = storm::analysis::AssumptionMaker<storm::RationalFunction>(&assumptionChecker, dtmc->getNumberOfStates(), true);
    auto result = assumptionMaker.createAndCheckAssumption(1, 2, order);

    EXPECT_EQ(3ul, result.size());

    bool foundFirst = false;
    bool foundSecond = false;
    bool foundThird = false;
    for (auto itr = result.begin(); itr != result.end(); ++itr) {
        if (!foundFirst && itr->first->getFirstOperand()->asVariableExpression().getVariable().getName() == "1") {
            EXPECT_EQ(storm::analysis::AssumptionStatus::INVALID, itr->second);
            EXPECT_EQ(true, itr->first->getFirstOperand()->isVariable());
            EXPECT_EQ(true, itr->first->getSecondOperand()->isVariable());
            EXPECT_EQ("2", itr->first->getSecondOperand()->asVariableExpression().getVariable().getName());
            EXPECT_EQ(storm::expressions::BinaryRelationExpression::RelationType::Greater, itr->first->getRelationType());
            foundFirst = true;
        } else if (!foundSecond && itr->first->getRelationType() == storm::expressions::BinaryRelationExpression::RelationType::Greater) {
            EXPECT_EQ(storm::analysis::AssumptionStatus::INVALID, itr->second);
            EXPECT_EQ(true, itr->first->getFirstOperand()->isVariable());
            EXPECT_EQ(true, itr->first->getSecondOperand()->isVariable());
            EXPECT_EQ("2", itr->first->getFirstOperand()->asVariableExpression().getVariable().getName());
            EXPECT_EQ("1", itr->first->getSecondOperand()->asVariableExpression().getVariable().getName());
            EXPECT_EQ(storm::expressions::BinaryRelationExpression::RelationType::Greater, itr->first->getRelationType());
            foundSecond = true;
        } else if (!foundThird && itr->first->getRelationType() == storm::expressions::BinaryRelationExpression::RelationType::Equal) {
            EXPECT_EQ(storm::analysis::AssumptionStatus::INVALID, itr->second);
            EXPECT_EQ(true, itr->first->getFirstOperand()->isVariable());
            EXPECT_EQ(true, itr->first->getSecondOperand()->isVariable());
            EXPECT_EQ("2", itr->first->getFirstOperand()->asVariableExpression().getVariable().getName());
            EXPECT_EQ("1", itr->first->getSecondOperand()->asVariableExpression().getVariable().getName());
            EXPECT_EQ(storm::expressions::BinaryRelationExpression::RelationType::Equal, itr->first->getRelationType());
            foundThird = true;
        } else {
            EXPECT_TRUE(false);
        }
    }
    EXPECT_TRUE(foundFirst);
    EXPECT_TRUE(foundSecond);
    EXPECT_TRUE(foundThird);
}

TEST(AssumptionMakerTest, Simple2) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/simple2.pm";
    std::string formulaAsString = "P=? [F s=3]";
    std::string constantsAsString = ""; //e.g. pL=0.9,TOACK=0.5

    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>>(*dtmc);
    ASSERT_TRUE(simplifier.simplify(*(formulas[0])));
    model = simplifier.getSimplifiedModel();
    dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    // Create the region
    auto vars = storm::models::sparse::getProbabilityParameters(*dtmc);
    auto region = storm::api::parseRegion<storm::RationalFunction>("0.00001 <= p <= 0.999999", vars);

    ASSERT_EQ(dtmc->getNumberOfStates(), 5ul);
    ASSERT_EQ(dtmc->getNumberOfTransitions(), 8ul);

    storm::storage::BitVector above(5);
    above.set(3);
    storm::storage::BitVector below(5);
    below.set(4);
    storm::storage::BitVector initialMiddle(5);

    std::vector<uint_fast64_t> statesSorted = storm::utility::graph::getTopologicalSort(model->getTransitionMatrix());

    auto order = new storm::analysis::Order(&above, &below, &initialMiddle, 5, &statesSorted);

    auto assumptionChecker = storm::analysis::AssumptionChecker<storm::RationalFunction>(formulas[0], dtmc, region, 3);
    auto assumptionMaker = storm::analysis::AssumptionMaker<storm::RationalFunction>(&assumptionChecker, dtmc->getNumberOfStates(), true);
    auto result = assumptionMaker.createAndCheckAssumption(1, 2, order);

    EXPECT_EQ(3ul, result.size());

    bool foundFirst = false;
    bool foundSecond = false;
    bool foundThird = false;
    for (auto itr = result.begin(); itr != result.end(); ++itr) {
        if (!foundFirst && itr->first->getFirstOperand()->asVariableExpression().getVariable().getName() == "1") {
            EXPECT_EQ(storm::analysis::AssumptionStatus::VALID, itr->second);
            EXPECT_EQ(true, itr->first->getFirstOperand()->isVariable());
            EXPECT_EQ(true, itr->first->getSecondOperand()->isVariable());
            EXPECT_EQ("2", itr->first->getSecondOperand()->asVariableExpression().getVariable().getName());
            EXPECT_EQ(storm::expressions::BinaryRelationExpression::RelationType::Greater, itr->first->getRelationType());
            foundFirst = true;
        } else if (!foundSecond && itr->first->getRelationType() == storm::expressions::BinaryRelationExpression::RelationType::Greater) {
            EXPECT_EQ(storm::analysis::AssumptionStatus::INVALID, itr->second);
            EXPECT_EQ(true, itr->first->getFirstOperand()->isVariable());
            EXPECT_EQ(true, itr->first->getSecondOperand()->isVariable());
            EXPECT_EQ("2", itr->first->getFirstOperand()->asVariableExpression().getVariable().getName());
            EXPECT_EQ("1", itr->first->getSecondOperand()->asVariableExpression().getVariable().getName());
            EXPECT_EQ(storm::expressions::BinaryRelationExpression::RelationType::Greater, itr->first->getRelationType());
            foundSecond = true;
        } else if (!foundThird && itr->first->getRelationType() == storm::expressions::BinaryRelationExpression::RelationType::Equal) {
            EXPECT_EQ(storm::analysis::AssumptionStatus::INVALID, itr->second);
            EXPECT_EQ(true, itr->first->getFirstOperand()->isVariable());
            EXPECT_EQ(true, itr->first->getSecondOperand()->isVariable());
            EXPECT_EQ("2", itr->first->getFirstOperand()->asVariableExpression().getVariable().getName());
            EXPECT_EQ("1", itr->first->getSecondOperand()->asVariableExpression().getVariable().getName());
            EXPECT_EQ(storm::expressions::BinaryRelationExpression::RelationType::Equal, itr->first->getRelationType());
            foundThird = true;
        } else {
            EXPECT_TRUE(false);
        }
    }
    EXPECT_TRUE(foundFirst);
    EXPECT_TRUE(foundSecond);
    EXPECT_TRUE(foundThird);
}

