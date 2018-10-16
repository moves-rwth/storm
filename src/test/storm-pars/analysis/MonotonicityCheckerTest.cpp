//
// Created by Jip Spel on 20.09.18.
//

#include "gtest/gtest.h"
#include "storm-config.h"
#include "test/storm_gtest.h"
#include "storm-pars/analysis/MonotonicityChecker.h"
#include "storm/storage/expressions/BinaryRelationExpression.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/adapters/RationalFunctionAdapter.h"

#include "storm-parsers/parser/FormulaParser.h"
#include "storm/logic/Formulas.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm-parsers/parser/AutoParser.h"
#include "storm-parsers/parser/PrismParser.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/api/builder.h"

#include "storm-pars/transformer/SparseParametricDtmcSimplifier.h"

#include "storm-pars/api/storm-pars.h"
#include "storm/api/storm.h"

#include "storm-parsers/api/storm-parsers.h"

TEST(MonotonicityCheckerTest, Monotone_no_model) {
    std::shared_ptr<storm::models::ModelBase> model;
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas;
    auto checker = storm::analysis::MonotonicityChecker<storm::RationalFunction>(model, formulas, false);
    // Build lattice
    auto numberOfStates = 4;
    auto above = storm::storage::BitVector(numberOfStates);
    above.set(1);
    auto below = storm::storage::BitVector(numberOfStates);
    below.set(0);
    auto lattice = storm::analysis::Lattice(above, below, numberOfStates);
    lattice.add(2);
    lattice.add(3);
    // Build map
    std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>> assumptions;
    std::map<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> map;
    map.insert(std::pair<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>>(&lattice, assumptions));

    // Build matrix
    auto builder = storm::storage::SparseMatrixBuilder<storm::RationalFunction>(numberOfStates, numberOfStates, 4);
    std::shared_ptr<storm::RawPolynomialCache> cache = std::make_shared<storm::RawPolynomialCache>();
    carl::StringParser parser;
    parser.setVariables({"p", "q"});
    auto func = storm::RationalFunction(storm::Polynomial(parser.template parseMultivariatePolynomial<storm::RationalFunctionCoefficient>("p"), cache));
    auto funcMin = storm::RationalFunction(storm::RationalFunction(1)-func);
    builder.addNextValue(2, 1, func);
    builder.addNextValue(2, 0, funcMin);
    func = storm::RationalFunction(storm::Polynomial(parser.template parseMultivariatePolynomial<storm::RationalFunctionCoefficient>("q"), cache));
    funcMin = storm::RationalFunction(storm::RationalFunction(1)-func);
    builder.addNextValue(3, 1, funcMin);
    builder.addNextValue(3, 0, func);
    storm::storage::SparseMatrix<storm::RationalFunction> matrix = builder.build();

    std::map<storm::analysis::Lattice*, std::map<carl::Variable, std::pair<bool, bool>>> result = checker.checkMonotonicity(map, matrix);
    ASSERT_EQ(1, result.size());
    ASSERT_EQ(2, result.begin()->second.size());
    auto entry1 = result.begin()->second.begin();
    auto entry2 = ++ (result.begin()->second.begin());
    ASSERT_EQ("p", entry1->first.name());
    EXPECT_TRUE(entry1->second.first);
    EXPECT_FALSE(entry1->second.second);
    EXPECT_FALSE(entry2->second.first);
    EXPECT_TRUE(entry2->second.second);
}

TEST(MonotonicityCheckerTest, Not_monotone_no_model) {
    std::shared_ptr<storm::models::ModelBase> model;
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas;
    auto checker = storm::analysis::MonotonicityChecker<storm::RationalFunction>(model, formulas, false);
    // Build lattice
    auto numberOfStates = 4;
    auto above = storm::storage::BitVector(numberOfStates);
    above.set(1);
    auto below = storm::storage::BitVector(numberOfStates);
    below.set(0);
    auto lattice = storm::analysis::Lattice(above, below, numberOfStates);
    lattice.add(2);
    lattice.add(3);
    // Build map
    std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>> assumptions;
    std::map<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> map;
    map.insert(std::pair<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>>(&lattice, assumptions));

    // Build matrix
    auto builder = storm::storage::SparseMatrixBuilder<storm::RationalFunction>(numberOfStates, numberOfStates, 4);
    std::shared_ptr<storm::RawPolynomialCache> cache = std::make_shared<storm::RawPolynomialCache>();
    carl::StringParser parser;
    parser.setVariables({"p", "q"});
    auto func = storm::RationalFunction(storm::Polynomial(parser.template parseMultivariatePolynomial<storm::RationalFunctionCoefficient>("p"), cache));
    auto funcMin = storm::RationalFunction(storm::RationalFunction(1)-func);
    builder.addNextValue(2, 1, func);
    builder.addNextValue(2, 0, funcMin);
    builder.addNextValue(3, 1, funcMin);
    builder.addNextValue(3, 0, func);
    auto matrix = builder.build();

    auto result = checker.checkMonotonicity(map, matrix);
    ASSERT_EQ(1, result.size());
    ASSERT_EQ(1, result.begin()->second.size());
    auto    entry1 = result.begin()->second.begin();
    ASSERT_EQ("p", entry1->first.name());
    EXPECT_FALSE(entry1->second.first);
    EXPECT_FALSE(entry1->second.second);
}

TEST(MonotonicityCheckerTest, Brp_with_bisimulation) {
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

    // Apply bisimulation
    storm::storage::BisimulationType bisimType = storm::storage::BisimulationType::Strong;
    if (storm::settings::getModule<storm::settings::modules::BisimulationSettings>().isWeakBisimulationSet()) {
        bisimType = storm::storage::BisimulationType::Weak;
    }

    dtmc = storm::api::performBisimulationMinimization<storm::RationalFunction>(model, formulas, bisimType)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();

    ASSERT_EQ(dtmc->getNumberOfStates(), 99ull);
    ASSERT_EQ(dtmc->getNumberOfTransitions(), 195ull);

    storm::analysis::MonotonicityChecker<storm::RationalFunction> monotonicityChecker = storm::analysis::MonotonicityChecker<storm::RationalFunction>(dtmc, formulas, true);
    auto result = monotonicityChecker.checkMonotonicity();
    EXPECT_EQ(1, result.size());
    EXPECT_EQ(2, result.begin()->second.size());
    auto monotone = result.begin()->second.begin();
    EXPECT_EQ(monotone->second.first, true);
    EXPECT_EQ(monotone->second.second, false);
}