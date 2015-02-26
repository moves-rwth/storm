#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/storage/dd/CuddDdManager.h"
#include "src/storage/dd/CuddDd.h"
#include "src/storage/dd/CuddOdd.h"
#include "src/storage/dd/DdMetaVariable.h"

TEST(CuddDdManager, Constants) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    storm::dd::Dd<storm::dd::DdType::CUDD> zero;
    ASSERT_NO_THROW(zero = manager->getZero());
    
    EXPECT_EQ(0, zero.getNonZeroCount());
    EXPECT_EQ(1, zero.getLeafCount());
    EXPECT_EQ(1, zero.getNodeCount());
    EXPECT_EQ(0, zero.getMin());
    EXPECT_EQ(0, zero.getMax());
    
    storm::dd::Dd<storm::dd::DdType::CUDD> one;
    ASSERT_NO_THROW(one = manager->getOne());
    
    EXPECT_EQ(1, one.getNonZeroCount());
    EXPECT_EQ(1, one.getLeafCount());
    EXPECT_EQ(1, one.getNodeCount());
    EXPECT_EQ(1, one.getMin());
    EXPECT_EQ(1, one.getMax());

    storm::dd::Dd<storm::dd::DdType::CUDD> two;
    ASSERT_NO_THROW(two = manager->getConstant(2));
    
    EXPECT_EQ(1, two.getNonZeroCount());
    EXPECT_EQ(1, two.getLeafCount());
    EXPECT_EQ(1, two.getNodeCount());
    EXPECT_EQ(2, two.getMin());
    EXPECT_EQ(2, two.getMax());
}

TEST(CuddDdManager, AddGetMetaVariableTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    ASSERT_NO_THROW(manager->addMetaVariable("x", 1, 9));
    EXPECT_EQ(2, manager->getNumberOfMetaVariables());
    
    ASSERT_THROW(manager->addMetaVariable("x", 0, 3), storm::exceptions::InvalidArgumentException);

    ASSERT_NO_THROW(manager->addMetaVariable("y", 0, 3));
    EXPECT_EQ(4, manager->getNumberOfMetaVariables());
    
    EXPECT_TRUE(manager->hasMetaVariable("x'"));
    EXPECT_TRUE(manager->hasMetaVariable("y'"));
    
    std::set<std::string> metaVariableSet = {"x", "x'", "y", "y'"};
    EXPECT_EQ(metaVariableSet, manager->getAllMetaVariableNames());
}

TEST(CuddDdManager, EncodingTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x = manager->addMetaVariable("x", 1, 9);
    
    storm::dd::Dd<storm::dd::DdType::CUDD> encoding;
    ASSERT_THROW(encoding = manager->getEncoding(x.first, 0), storm::exceptions::InvalidArgumentException);
    ASSERT_THROW(encoding = manager->getEncoding(x.first, 10), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(encoding = manager->getEncoding(x.first, 4));
    EXPECT_EQ(1, encoding.getNonZeroCount());

    // As a BDD, this DD has one only leaf, because there does not exist a 0-leaf, and (consequently) one node less
    // than the MTBDD.
    EXPECT_EQ(5, encoding.getNodeCount());
    EXPECT_EQ(1, encoding.getLeafCount());
    
    // As an MTBDD, the 0-leaf is there, so the count is actually 2 and the node count is 6.
    EXPECT_EQ(6, encoding.toMtbdd().getNodeCount());
    EXPECT_EQ(2, encoding.toMtbdd().getLeafCount());
}

TEST(CuddDdManager, RangeTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x;
    ASSERT_NO_THROW(x = manager->addMetaVariable("x", 1, 9));
    
    storm::dd::Dd<storm::dd::DdType::CUDD> range;
    ASSERT_NO_THROW(range = manager->getRange(x.first));
    
    EXPECT_EQ(9, range.getNonZeroCount());
    EXPECT_EQ(1, range.getLeafCount());
    EXPECT_EQ(5, range.getNodeCount());
}

TEST(CuddDdManager, IdentityTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x = manager->addMetaVariable("x", 1, 9);
    
    storm::dd::Dd<storm::dd::DdType::CUDD> range;
    ASSERT_NO_THROW(range = manager->getIdentity(x.first));
    
    EXPECT_EQ(9, range.getNonZeroCount());
    EXPECT_EQ(10, range.getLeafCount());
    EXPECT_EQ(21, range.getNodeCount());
}

TEST(CuddDd, OperatorTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x = manager->addMetaVariable("x", 1, 9);
    EXPECT_TRUE(manager->getZero() == manager->getZero());
    EXPECT_FALSE(manager->getZero() == manager->getOne());

    EXPECT_FALSE(manager->getZero() != manager->getZero());
    EXPECT_TRUE(manager->getZero() != manager->getOne());
    
    storm::dd::Dd<storm::dd::DdType::CUDD> dd1 = manager->getOne(true);
    storm::dd::Dd<storm::dd::DdType::CUDD> dd2 = manager->getOne(true);
    storm::dd::Dd<storm::dd::DdType::CUDD> dd3 = dd1 + dd2;
    EXPECT_TRUE(dd3 == manager->getConstant(2));
    
    dd3 += manager->getZero(true);
    EXPECT_TRUE(dd3 == manager->getConstant(2));
    
    dd3 = dd1 * manager->getConstant(3);
    EXPECT_TRUE(dd3 == manager->getConstant(3));

    dd3 *= manager->getConstant(2);
    EXPECT_TRUE(dd3 == manager->getConstant(6));
    
    dd3 = dd1 - dd2;
    EXPECT_TRUE(dd3.isZero());
    
    dd3 -= manager->getConstant(-2);
    EXPECT_TRUE(dd3 == manager->getConstant(2));
    
    dd3 /= manager->getConstant(2);
    EXPECT_TRUE(dd3.isOne());
    
    dd3 = dd3.toBdd().complement();
    EXPECT_TRUE(dd3.isZero());
    
    dd1 = !dd3;
    EXPECT_TRUE(dd1.isOne());

    dd3 = dd1 || dd2.toBdd();
    EXPECT_TRUE(dd3.isOne());
    
    dd1 = manager->getIdentity(x.first);
    dd2 = manager->getConstant(5);

    dd3 = dd1.equals(dd2);
    EXPECT_EQ(1, dd3.getNonZeroCount());
    
    storm::dd::Dd<storm::dd::DdType::CUDD> dd4 = dd1.notEquals(dd2);
    EXPECT_TRUE(dd4.toBdd() == !dd3.toBdd());
    
    dd3 = dd1.less(dd2);
    EXPECT_EQ(11, dd3.getNonZeroCount());
    
    dd3 = dd1.lessOrEqual(dd2);
    EXPECT_EQ(12, dd3.getNonZeroCount());
    
    dd3 = dd1.greater(dd2);
    EXPECT_EQ(4, dd3.getNonZeroCount());

    dd3 = dd1.greaterOrEqual(dd2);
    EXPECT_EQ(5, dd3.getNonZeroCount());
    
    dd3 = (manager->getEncoding(x.first, 2)).ite(dd2, dd1);
    dd4 = dd3.less(dd2);
    EXPECT_EQ(10, dd4.getNonZeroCount());
    
    dd4 = dd3.minimum(dd1);
    dd4 *= manager->getEncoding(x.first, 2, true);
    dd4 = dd4.sumAbstract({x.first});
    EXPECT_EQ(2, dd4.getValue());

    dd4 = dd3.maximum(dd1);
    dd4 *= manager->getEncoding(x.first, 2, true);
    dd4 = dd4.sumAbstract({x.first});
    EXPECT_EQ(5, dd4.getValue());

    dd1 = manager->getConstant(0.01);
    dd2 = manager->getConstant(0.01 + 1e-6);
    EXPECT_TRUE(dd1.equalModuloPrecision(dd2, 1e-6, false));
    EXPECT_FALSE(dd1.equalModuloPrecision(dd2, 1e-6));
}

TEST(CuddDd, AbstractionTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x = manager->addMetaVariable("x", 1, 9);
    storm::dd::Dd<storm::dd::DdType::CUDD> dd1;
    storm::dd::Dd<storm::dd::DdType::CUDD> dd2;
    storm::dd::Dd<storm::dd::DdType::CUDD> dd3;
    
    dd1 = manager->getIdentity(x.first);
    dd2 = manager->getConstant(5);
    dd3 = dd1.equals(dd2);
    EXPECT_EQ(1, dd3.getNonZeroCount());
    ASSERT_THROW(dd3 = dd3.existsAbstract({x.second}), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(dd3 = dd3.existsAbstract({x.first}));
    EXPECT_EQ(1, dd3.getNonZeroCount());
    EXPECT_EQ(1, dd3.getMax());

    dd3 = dd1.equals(dd2);
    dd3 *= manager->getConstant(3);
    EXPECT_EQ(1, dd3.getNonZeroCount());
    ASSERT_THROW(dd3 = dd3.existsAbstract({x.second}), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(dd3 = dd3.existsAbstract({x.first}));
    EXPECT_TRUE(dd3.isOne());

    dd3 = dd1.equals(dd2);
    dd3 *= manager->getConstant(3);
    ASSERT_THROW(dd3 = dd3.sumAbstract({x.second}), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(dd3 = dd3.sumAbstract({x.first}));
    EXPECT_EQ(1, dd3.getNonZeroCount());
    EXPECT_EQ(3, dd3.getMax());

    dd3 = dd1.equals(dd2);
    dd3 *= manager->getConstant(3);
    ASSERT_THROW(dd3 = dd3.minAbstract({x.second}), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(dd3 = dd3.minAbstract({x.first}));
    EXPECT_EQ(0, dd3.getNonZeroCount());
    EXPECT_EQ(0, dd3.getMax());

    dd3 = dd1.equals(dd2);
    dd3 *= manager->getConstant(3);
    ASSERT_THROW(dd3 = dd3.maxAbstract({x.second}), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(dd3 = dd3.maxAbstract({x.first}));
    EXPECT_EQ(1, dd3.getNonZeroCount());
    EXPECT_EQ(3, dd3.getMax());
}

TEST(CuddDd, SwapTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x = manager->addMetaVariable("x", 1, 9);
    std::pair<storm::expressions::Variable, storm::expressions::Variable> z = manager->addMetaVariable("z", 2, 8);
    storm::dd::Dd<storm::dd::DdType::CUDD> dd1;
    storm::dd::Dd<storm::dd::DdType::CUDD> dd2;
    
    dd1 = manager->getIdentity(x.first);
    ASSERT_THROW(dd1 = dd1.swapVariables({std::make_pair(x.first, z.first)}), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(dd1 = dd1.swapVariables({std::make_pair(x.first, x.second)}));
    EXPECT_TRUE(dd1 == manager->getIdentity(x.second));
}

TEST(CuddDd, MultiplyMatrixTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x = manager->addMetaVariable("x", 1, 9);
    
    storm::dd::Dd<storm::dd::DdType::CUDD> dd1 = manager->getIdentity(x.first).equals(manager->getIdentity(x.second));
    storm::dd::Dd<storm::dd::DdType::CUDD> dd2 = manager->getRange(x.second);
    storm::dd::Dd<storm::dd::DdType::CUDD> dd3;
    dd1 *= manager->getConstant(2);
    
    ASSERT_NO_THROW(dd3 = dd1.multiplyMatrix(dd2, {x.second}));
    ASSERT_NO_THROW(dd3 = dd3.swapVariables({std::make_pair(x.first, x.second)}));
    EXPECT_TRUE(dd3 == dd2 * manager->getConstant(2));
}

TEST(CuddDd, GetSetValueTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x = manager->addMetaVariable("x", 1, 9);
    
    storm::dd::Dd<storm::dd::DdType::CUDD> dd1 = manager->getOne(true);
    ASSERT_NO_THROW(dd1.setValue(x.first, 4, 2));
    EXPECT_EQ(2, dd1.getLeafCount());
    
    std::map<storm::expressions::Variable, int_fast64_t> metaVariableToValueMap;
    metaVariableToValueMap.emplace(x.first, 1);
    EXPECT_EQ(1, dd1.getValue(metaVariableToValueMap));
    
    metaVariableToValueMap.clear();
    metaVariableToValueMap.emplace(x.first, 4);
    EXPECT_EQ(2, dd1.getValue(metaVariableToValueMap));
}

TEST(CuddDd, ForwardIteratorTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x = manager->addMetaVariable("x", 1, 9);
    std::pair<storm::expressions::Variable, storm::expressions::Variable> y = manager->addMetaVariable("y", 0, 3);
    
    storm::dd::Dd<storm::dd::DdType::CUDD> dd;
    ASSERT_NO_THROW(dd = manager->getRange(x.first));
    
    storm::dd::DdForwardIterator<storm::dd::DdType::CUDD> it, ite;
    ASSERT_NO_THROW(it = dd.begin());
    ASSERT_NO_THROW(ite = dd.end());
    std::pair<storm::expressions::SimpleValuation, double> valuationValuePair;
    uint_fast64_t numberOfValuations = 0;
    while (it != ite) {
        ASSERT_NO_THROW(valuationValuePair = *it);
        ASSERT_NO_THROW(++it);
        ++numberOfValuations;
    }
    EXPECT_EQ(9, numberOfValuations);

    dd = manager->getRange(x.first);
    dd = dd.ite(manager->getOne(), manager->getOne());
    ASSERT_NO_THROW(it = dd.begin());
    ASSERT_NO_THROW(ite = dd.end());
    numberOfValuations = 0;
    while (it != ite) {
        ASSERT_NO_THROW(valuationValuePair = *it);
        ASSERT_NO_THROW(++it);
        ++numberOfValuations;
    }
    EXPECT_EQ(16, numberOfValuations);
    
    ASSERT_NO_THROW(it = dd.begin(false));
    ASSERT_NO_THROW(ite = dd.end());
    numberOfValuations = 0;
    while (it != ite) {
        ASSERT_NO_THROW(valuationValuePair = *it);
        ASSERT_NO_THROW(++it);
        ++numberOfValuations;
    }
    EXPECT_EQ(1, numberOfValuations);
}

// FIXME: make toExpression work again and then fix this test.
//TEST(CuddDd, ToExpressionTest) {
//    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
//    std::pair<storm::expressions::Variable, storm::expressions::Variable> x = manager->addMetaVariable("x", 1, 9);
//
//    storm::dd::Dd<storm::dd::DdType::CUDD> dd;
//    ASSERT_NO_THROW(dd = manager->getIdentity(x.first));
//    
//    storm::expressions::Expression ddAsExpression;
//    ASSERT_NO_THROW(ddAsExpression = dd.toExpression());
//
//    storm::expressions::SimpleValuation valuation;
//    for (std::size_t bit = 0; bit < manager->getMetaVariable(x.first).getNumberOfDdVariables(); ++bit) {
//        valuation.addBooleanIdentifier("x." + std::to_string(bit));
//    }
//    
//    storm::dd::DdMetaVariable<storm::dd::DdType::CUDD> const& metaVariable = manager->getMetaVariable("x");
//
//    for (auto valuationValuePair : dd) {
//        for (std::size_t i = 0; i < metaVariable.getNumberOfDdVariables(); ++i) {
//            // Check if the i-th bit is set or not and modify the valuation accordingly.
//            if (((valuationValuePair.first.getIntegerValue("x") - metaVariable.getLow()) & (1ull << (metaVariable.getNumberOfDdVariables() - i - 1))) != 0) {
//                valuation.setBooleanValue("x." + std::to_string(i), true);
//            } else {
//                valuation.setBooleanValue("x." + std::to_string(i), false);
//            }
//        }
//        
//        // At this point, the constructed valuation should make the expression obtained from the DD evaluate to the very
//        // same value as the current value obtained from the DD.
//        EXPECT_EQ(valuationValuePair.second, ddAsExpression.evaluateAsDouble(&valuation));
//    }
//    
//    storm::expressions::Expression mintermExpression = dd.getMintermExpression();
//    
//    // Check whether all minterms are covered.
//    for (auto valuationValuePair : dd) {
//        for (std::size_t i = 0; i < metaVariable.getNumberOfDdVariables(); ++i) {
//            // Check if the i-th bit is set or not and modify the valuation accordingly.
//            if (((valuationValuePair.first.getIntegerValue("x") - metaVariable.getLow()) & (1ull << (metaVariable.getNumberOfDdVariables() - i - 1))) != 0) {
//                valuation.setBooleanValue("x." + std::to_string(i), true);
//            } else {
//                valuation.setBooleanValue("x." + std::to_string(i), false);
//            }
//        }
//        
//        // At this point, the constructed valuation should make the expression obtained from the DD evaluate to the very
//        // same value as the current value obtained from the DD.
//        EXPECT_TRUE(mintermExpression.evaluateAsBool(&valuation));
//    }
//    
//    // Now check no additional minterms are covered.
//    dd = !dd;
//    for (auto valuationValuePair : dd) {
//        for (std::size_t i = 0; i < metaVariable.getNumberOfDdVariables(); ++i) {
//            // Check if the i-th bit is set or not and modify the valuation accordingly.
//            if (((valuationValuePair.first.getIntegerValue("x") - metaVariable.getLow()) & (1ull << (metaVariable.getNumberOfDdVariables() - i - 1))) != 0) {
//                valuation.setBooleanValue("x." + std::to_string(i), true);
//            } else {
//                valuation.setBooleanValue("x." + std::to_string(i), false);
//            }
//        }
//        
//        // At this point, the constructed valuation should make the expression obtained from the DD evaluate to the very
//        // same value as the current value obtained from the DD.
//        EXPECT_FALSE(mintermExpression.evaluateAsBool(&valuation));
//    }
//}

TEST(CuddDd, OddTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> a = manager->addMetaVariable("a");
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x = manager->addMetaVariable("x", 1, 9);
    
    storm::dd::Dd<storm::dd::DdType::CUDD> dd = manager->getIdentity(x.first);
    storm::dd::Odd<storm::dd::DdType::CUDD> odd;
    ASSERT_NO_THROW(odd = storm::dd::Odd<storm::dd::DdType::CUDD>(dd));
    EXPECT_EQ(9, odd.getTotalOffset());
    EXPECT_EQ(12, odd.getNodeCount());

    std::vector<double> ddAsVector;
    ASSERT_NO_THROW(ddAsVector = dd.toVector<double>());
    EXPECT_EQ(9, ddAsVector.size());
    for (uint_fast64_t i = 0; i < ddAsVector.size(); ++i) {
        EXPECT_TRUE(i+1 == ddAsVector[i]);
    }
    
    // Create a non-trivial matrix.
    dd = manager->getIdentity(x.first).equals(manager->getIdentity(x.second)) * manager->getRange(x.first);    
    dd += manager->getEncoding(x.first, 1) * manager->getRange(x.second) + manager->getEncoding(x.second, 1) * manager->getRange(x.first);
    
    // Create the ODDs.
    storm::dd::Odd<storm::dd::DdType::CUDD> rowOdd;
    ASSERT_NO_THROW(rowOdd = storm::dd::Odd<storm::dd::DdType::CUDD>(manager->getRange(x.first)));
    storm::dd::Odd<storm::dd::DdType::CUDD> columnOdd;
    ASSERT_NO_THROW(columnOdd = storm::dd::Odd<storm::dd::DdType::CUDD>(manager->getRange(x.second)));
    
    // Try to translate the matrix.
    storm::storage::SparseMatrix<double> matrix;
    ASSERT_NO_THROW(matrix = dd.toMatrix({x.first}, {x.second}, rowOdd, columnOdd));
    
    EXPECT_EQ(9, matrix.getRowCount());
    EXPECT_EQ(9, matrix.getColumnCount());
    EXPECT_EQ(25, matrix.getNonzeroEntryCount());
    
    dd = manager->getRange(x.first) * manager->getRange(x.second) * manager->getEncoding(a.first, 0).ite(dd, dd + manager->getConstant(1));
    ASSERT_NO_THROW(matrix = dd.toMatrix({x.first}, {x.second}, {a.first}, rowOdd, columnOdd));
    EXPECT_EQ(18, matrix.getRowCount());
    EXPECT_EQ(9, matrix.getRowGroupCount());
    EXPECT_EQ(9, matrix.getColumnCount());
    EXPECT_EQ(106, matrix.getNonzeroEntryCount());
}