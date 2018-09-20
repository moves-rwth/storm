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

TEST(MonotonicityCheckerTest, Monotone) {
    auto checker = storm::analysis::MonotonicityChecker<storm::RationalFunction>();
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

TEST(MonotonicityCheckerTest, NotMonotone) {
    auto checker = storm::analysis::MonotonicityChecker<storm::RationalFunction>();
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