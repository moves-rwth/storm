//
// Created by Jip Spel on 20.09.18.
//

// TODO: cleanup includes
#include "gtest/gtest.h"
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

#include "storm-pars/analysis/Lattice.h"
#include "storm-pars/analysis/AssumptionMaker.h"
#include "storm-pars/analysis/AssumptionChecker.h"
#include "storm-pars/analysis/LatticeExtender.h"
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

    ASSERT_EQ(dtmc->getNumberOfStates(), 193ull);
    ASSERT_EQ(dtmc->getNumberOfTransitions(), 383ull);

    auto *extender = new storm::analysis::LatticeExtender<storm::RationalFunction>(dtmc);
    auto criticalTuple = extender->toLattice(formulas);
    ASSERT_EQ(183, std::get<1>(criticalTuple));
    ASSERT_EQ(186, std::get<2>(criticalTuple));

    auto assumptionChecker = storm::analysis::AssumptionChecker<storm::RationalFunction>(formulas[0], dtmc, 3);
    auto assumptionMaker = storm::analysis::AssumptionMaker<storm::RationalFunction>(&assumptionChecker, dtmc->getNumberOfStates(), true);
    auto result = assumptionMaker.createAndCheckAssumption(std::get<1>(criticalTuple), std::get<2>(criticalTuple), std::get<0>(criticalTuple));

    auto itr = result.begin();

    auto var1 = itr->first->getManager().getVariable("183");
    auto var2 = itr->first->getManager().getVariable("186");

    EXPECT_EQ(3, result.size());

    EXPECT_EQ(false, itr->second);
    EXPECT_EQ(true, itr->first->getFirstOperand()->isVariable());
    EXPECT_EQ(true, itr->first->getSecondOperand()->isVariable());
    EXPECT_EQ(var1, itr->first->getFirstOperand()->asVariableExpression().getVariable());
    EXPECT_EQ(var2, itr->first->getSecondOperand()->asVariableExpression().getVariable());
    EXPECT_EQ(storm::expressions::BinaryRelationExpression::RelationType::Greater, itr->first->getRelationType());

    ++itr;
    EXPECT_EQ(false, itr->second);
    EXPECT_EQ(true, itr->first->getFirstOperand()->isVariable());
    EXPECT_EQ(true, itr->first->getSecondOperand()->isVariable());
    EXPECT_EQ(var2, itr->first->getFirstOperand()->asVariableExpression().getVariable());
    EXPECT_EQ(var1, itr->first->getSecondOperand()->asVariableExpression().getVariable());
    EXPECT_EQ(storm::expressions::BinaryRelationExpression::RelationType::Greater, itr->first->getRelationType());

    ++itr;
    EXPECT_EQ(false, itr->second);
    EXPECT_EQ(true, itr->first->getFirstOperand()->isVariable());
    EXPECT_EQ(true, itr->first->getSecondOperand()->isVariable());
    EXPECT_EQ(var2, itr->first->getFirstOperand()->asVariableExpression().getVariable());
    EXPECT_EQ(var1, itr->first->getSecondOperand()->asVariableExpression().getVariable());
    EXPECT_EQ(storm::expressions::BinaryRelationExpression::RelationType::Equal, itr->first->getRelationType());
    // TODO: createEqualsAssumption checken
}
