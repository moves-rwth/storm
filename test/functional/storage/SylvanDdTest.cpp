#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/adapters/CarlAdapter.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/storage/dd/DdManager.h"
#include "src/storage/dd/Add.h"
#include "src/storage/dd/Odd.h"
#include "src/storage/dd/DdMetaVariable.h"
#include "src/settings/SettingsManager.h"

#include "src/storage/SparseMatrix.h"

TEST(SylvanDd, Constants) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::Sylvan>> manager(new storm::dd::DdManager<storm::dd::DdType::Sylvan>());
    storm::dd::Add<storm::dd::DdType::Sylvan, double> zero;
    ASSERT_NO_THROW(zero = manager->template getAddZero<double>());
    
    EXPECT_EQ(0ul, zero.getNonZeroCount());
    EXPECT_EQ(1ul, zero.getLeafCount());
    EXPECT_EQ(1ul, zero.getNodeCount());
    EXPECT_EQ(0, zero.getMin());
    EXPECT_EQ(0, zero.getMax());
    
    storm::dd::Add<storm::dd::DdType::Sylvan, double> one;
    ASSERT_NO_THROW(one = manager->template getAddOne<double>());
    
    EXPECT_EQ(0ul, one.getNonZeroCount());
    EXPECT_EQ(1ul, one.getLeafCount());
    EXPECT_EQ(1ul, one.getNodeCount());
    EXPECT_EQ(1, one.getMin());
    EXPECT_EQ(1, one.getMax());
    
    storm::dd::Add<storm::dd::DdType::Sylvan, double> two;
    ASSERT_NO_THROW(two = manager->template getConstant<double>(2));
    
    EXPECT_EQ(0ul, two.getNonZeroCount());
    EXPECT_EQ(1ul, two.getLeafCount());
    EXPECT_EQ(1ul, two.getNodeCount());
    EXPECT_EQ(2, two.getMin());
    EXPECT_EQ(2, two.getMax());
}

TEST(SylvanDd, AddGetMetaVariableTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::Sylvan>> manager(new storm::dd::DdManager<storm::dd::DdType::Sylvan>());
    ASSERT_NO_THROW(manager->addMetaVariable("x", 1, 9));
    EXPECT_EQ(2ul, manager->getNumberOfMetaVariables());
    
    ASSERT_THROW(manager->addMetaVariable("x", 0, 3), storm::exceptions::InvalidArgumentException);
    
    ASSERT_NO_THROW(manager->addMetaVariable("y", 0, 3));
    EXPECT_EQ(4ul, manager->getNumberOfMetaVariables());
    
    EXPECT_TRUE(manager->hasMetaVariable("x'"));
    EXPECT_TRUE(manager->hasMetaVariable("y'"));
    
    std::set<std::string> metaVariableSet = {"x", "x'", "y", "y'"};
    EXPECT_EQ(metaVariableSet, manager->getAllMetaVariableNames());
}

TEST(SylvanDd, EncodingTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::Sylvan>> manager(new storm::dd::DdManager<storm::dd::DdType::Sylvan>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x = manager->addMetaVariable("x", 1, 9);
    
    storm::dd::Bdd<storm::dd::DdType::Sylvan> encoding;
    ASSERT_THROW(encoding = manager->getEncoding(x.first, 0), storm::exceptions::InvalidArgumentException);
    ASSERT_THROW(encoding = manager->getEncoding(x.first, 10), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(encoding = manager->getEncoding(x.first, 4));
    EXPECT_EQ(1ul, encoding.getNonZeroCount());
    
    // As a BDD, this DD has one only leaf, because there does not exist a 0-leaf, and (consequently) one node less
    // than the MTBDD.
    EXPECT_EQ(5ul, encoding.getNodeCount());
    EXPECT_EQ(1ul, encoding.getLeafCount());
    
    storm::dd::Add<storm::dd::DdType::Sylvan, double> add;
    ASSERT_NO_THROW(add = encoding.template toAdd<double>());

    // As an MTBDD, the 0-leaf is there, so the count is actually 2 and the node count is 6.
    EXPECT_EQ(6ul, add.getNodeCount());
    EXPECT_EQ(2ul, add.getLeafCount());
}

#ifdef STORM_HAVE_CARL
TEST(SylvanDd, RationalFunctionConstants) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::Sylvan>> manager(new storm::dd::DdManager<storm::dd::DdType::Sylvan>());
    storm::dd::Add<storm::dd::DdType::Sylvan, storm::RationalFunction> zero;
    ASSERT_NO_THROW(zero = manager->template getAddZero<storm::RationalFunction>());
    
    EXPECT_EQ(0ul, zero.getNonZeroCount());
    EXPECT_EQ(1ul, zero.getLeafCount());
    EXPECT_EQ(1ul, zero.getNodeCount());
    
    storm::dd::Add<storm::dd::DdType::Sylvan, storm::RationalFunction> one;
    ASSERT_NO_THROW(one = manager->template getAddOne<storm::RationalFunction>());
    
    EXPECT_EQ(0ul, one.getNonZeroCount());
    EXPECT_EQ(1ul, one.getLeafCount());
    EXPECT_EQ(1ul, one.getNodeCount());
    
    storm::dd::Add<storm::dd::DdType::Sylvan, storm::RationalFunction> two;
	storm::RationalFunction constantTwo(2);
	
    ASSERT_NO_THROW(two = manager->template getConstant<storm::RationalFunction>(constantTwo));
    
    EXPECT_EQ(0ul, two.getNonZeroCount());
    EXPECT_EQ(1ul, two.getLeafCount());
    EXPECT_EQ(1ul, two.getNodeCount());
}

TEST(SylvanDd, RationalFunctionEncodingTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::Sylvan>> manager(new storm::dd::DdManager<storm::dd::DdType::Sylvan>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x = manager->addMetaVariable("x", 1, 9);
    
    storm::dd::Bdd<storm::dd::DdType::Sylvan> encoding;
    ASSERT_THROW(encoding = manager->getEncoding(x.first, 0), storm::exceptions::InvalidArgumentException);
    ASSERT_THROW(encoding = manager->getEncoding(x.first, 10), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(encoding = manager->getEncoding(x.first, 4));
    EXPECT_EQ(1ul, encoding.getNonZeroCount());
    
    // As a BDD, this DD has one only leaf, because there does not exist a 0-leaf, and (consequently) one node less
    // than the MTBDD.
    EXPECT_EQ(5ul, encoding.getNodeCount());
    EXPECT_EQ(1ul, encoding.getLeafCount());
    
    storm::dd::Add<storm::dd::DdType::Sylvan, storm::RationalFunction> add;
    ASSERT_NO_THROW(add = encoding.template toAdd<storm::RationalFunction>());

    // As an MTBDD, the 0-leaf is there, so the count is actually 2 and the node count is 6.
    EXPECT_EQ(6ul, add.getNodeCount());
    EXPECT_EQ(2ul, add.getLeafCount());
}

TEST(SylvanDd, RationalFunctionIdentityTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::Sylvan>> manager(new storm::dd::DdManager<storm::dd::DdType::Sylvan>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x = manager->addMetaVariable("x", 1, 9);
    
    storm::dd::Add<storm::dd::DdType::Sylvan, storm::RationalFunction> identity;
    ASSERT_NO_THROW(identity = manager->getIdentity<storm::RationalFunction>(x.first));

    EXPECT_EQ(9ul, identity.getNonZeroCount());
    EXPECT_EQ(10ul, identity.getLeafCount());
    EXPECT_EQ(21ul, identity.getNodeCount());
}

#endif

TEST(SylvanDd, RangeTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::Sylvan>> manager(new storm::dd::DdManager<storm::dd::DdType::Sylvan>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x;
    ASSERT_NO_THROW(x = manager->addMetaVariable("x", 1, 9));
    
    storm::dd::Bdd<storm::dd::DdType::Sylvan> range;
    ASSERT_NO_THROW(range = manager->getRange(x.first));
    
    EXPECT_EQ(9ul, range.getNonZeroCount());
    EXPECT_EQ(1ul, range.getLeafCount());
    EXPECT_EQ(5ul, range.getNodeCount());
}

TEST(SylvanDd, IdentityTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::Sylvan>> manager(new storm::dd::DdManager<storm::dd::DdType::Sylvan>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x = manager->addMetaVariable("x", 1, 9);
    
    storm::dd::Add<storm::dd::DdType::Sylvan, double> identity;
    ASSERT_NO_THROW(identity = manager->getIdentity<double>(x.first));

    EXPECT_EQ(9ul, identity.getNonZeroCount());
    EXPECT_EQ(10ul, identity.getLeafCount());
    EXPECT_EQ(21ul, identity.getNodeCount());
}

TEST(SylvanDd, OperatorTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::Sylvan>> manager(new storm::dd::DdManager<storm::dd::DdType::Sylvan>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x = manager->addMetaVariable("x", 1, 9);
    EXPECT_TRUE(manager->template getAddZero<double>() == manager->template getAddZero<double>());
    EXPECT_FALSE(manager->template getAddZero<double>() == manager->template getAddOne<double>());
    
    EXPECT_FALSE(manager->template getAddZero<double>() != manager->template getAddZero<double>());
    EXPECT_TRUE(manager->template getAddZero<double>() != manager->template getAddOne<double>());
    
    storm::dd::Add<storm::dd::DdType::Sylvan, double> dd1 = manager->template getAddOne<double>();
    storm::dd::Add<storm::dd::DdType::Sylvan, double> dd2 = manager->template getAddOne<double>();
    storm::dd::Add<storm::dd::DdType::Sylvan, double> dd3 = dd1 + dd2;
    storm::dd::Bdd<storm::dd::DdType::Sylvan> bdd;
    EXPECT_TRUE(dd3 == manager->template getConstant<double>(2));
    
    dd3 += manager->template getAddZero<double>();
    EXPECT_TRUE(dd3 == manager->template getConstant<double>(2));
    
    dd3 = dd1 * manager->template getConstant<double>(3);
    EXPECT_TRUE(dd3 == manager->template getConstant<double>(3));
    
    dd3 *= manager->template getConstant<double>(2);
    EXPECT_TRUE(dd3 == manager->template getConstant<double>(6));
    
    dd3 = dd1 - dd2;
    EXPECT_TRUE(dd3.isZero());
    
    dd3 -= manager->template getConstant<double>(-2);
    EXPECT_TRUE(dd3 == manager->template getConstant<double>(2));
    
    dd3 /= manager->template getConstant<double>(2);
    EXPECT_TRUE(dd3.isOne());
    
    bdd = !dd3.toBdd();
    EXPECT_TRUE(bdd.isZero());
    
    bdd = !bdd;
    EXPECT_TRUE(bdd.isOne());
    
    bdd = dd1.toBdd() || dd2.toBdd();
    EXPECT_TRUE(bdd.isOne());
    
    dd1 = manager->template getIdentity<double>(x.first);
    dd2 = manager->template getConstant<double>(5);
    
    bdd = dd1.equals(dd2);
    EXPECT_EQ(1ul, bdd.getNonZeroCount());
    
    storm::dd::Bdd<storm::dd::DdType::Sylvan> bdd2 = dd1.notEquals(dd2);
    EXPECT_TRUE(bdd2 == !bdd);
    
    bdd = dd1.less(dd2);
    EXPECT_EQ(11ul, bdd.getNonZeroCount());
    
    bdd = dd1.lessOrEqual(dd2);
    EXPECT_EQ(12ul, bdd.getNonZeroCount());
    
    bdd = dd1.greater(dd2);
    EXPECT_EQ(4ul, bdd.getNonZeroCount());
    
    bdd = dd1.greaterOrEqual(dd2);
    EXPECT_EQ(5ul, bdd.getNonZeroCount());
    
    dd3 = manager->getEncoding(x.first, 2).ite(dd2, dd1);
    bdd = dd3.less(dd2);
    EXPECT_EQ(10ul, bdd.getNonZeroCount());

    storm::dd::Add<storm::dd::DdType::Sylvan, double> dd4 = dd3.minimum(dd1);
    dd4 *= manager->getEncoding(x.first, 2).template toAdd<double>();
    dd4 = dd4.sumAbstract({x.first});
    EXPECT_EQ(2, dd4.getValue());

    dd4 = dd3.maximum(dd1);
    dd4 *= manager->getEncoding(x.first, 2).template toAdd<double>();
    dd4 = dd4.sumAbstract({x.first});
    EXPECT_EQ(5, dd4.getValue());
    
    dd1 = manager->template getConstant<double>(0.01);
    dd2 = manager->template getConstant<double>(0.01 + 1e-6);
    EXPECT_TRUE(dd1.equalModuloPrecision(dd2, 1e-6, false));
    EXPECT_FALSE(dd1.equalModuloPrecision(dd2, 1e-6));
}

TEST(SylvanDd, AbstractionTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::Sylvan>> manager(new storm::dd::DdManager<storm::dd::DdType::Sylvan>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x = manager->addMetaVariable("x", 1, 9);
    storm::dd::Add<storm::dd::DdType::Sylvan, double> dd1;
    storm::dd::Add<storm::dd::DdType::Sylvan, double> dd2;
    storm::dd::Add<storm::dd::DdType::Sylvan, double> dd3;
    storm::dd::Bdd<storm::dd::DdType::Sylvan> bdd;
    
    dd1 = manager->template getIdentity<double>(x.first);
    dd2 = manager->template getConstant<double>(5);
    bdd = dd1.equals(dd2);
    EXPECT_EQ(1ul, bdd.getNonZeroCount());
    ASSERT_THROW(bdd = bdd.existsAbstract({x.second}), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(bdd = bdd.existsAbstract({x.first}));
    EXPECT_EQ(0ul, bdd.getNonZeroCount());
    EXPECT_EQ(1, bdd.template toAdd<double>().getMax());
    
    dd3 = dd1.equals(dd2).template toAdd<double>();
    dd3 *= manager->template getConstant<double>(3);
    EXPECT_EQ(1ul, dd3.getNonZeroCount());
    ASSERT_THROW(bdd = dd3.toBdd().existsAbstract({x.second}), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(bdd = dd3.toBdd().existsAbstract({x.first}));
    EXPECT_TRUE(bdd.isOne());
    
    dd3 = dd1.equals(dd2).template toAdd<double>();
    dd3 *= manager->template getConstant<double>(3);
    ASSERT_THROW(dd3 = dd3.sumAbstract({x.second}), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(dd3 = dd3.sumAbstract({x.first}));
    EXPECT_EQ(0ul, dd3.getNonZeroCount());
    EXPECT_EQ(3, dd3.getMax());
    
    dd3 = dd1.equals(dd2).template toAdd<double>();
    dd3 *= manager->template getConstant<double>(3);
    ASSERT_THROW(dd3 = dd3.minAbstract({x.second}), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(dd3 = dd3.minAbstract({x.first}));
    EXPECT_EQ(0ul, dd3.getNonZeroCount());
    EXPECT_EQ(0, dd3.getMax());
    
    dd3 = dd1.equals(dd2).template toAdd<double>();
    dd3 *= manager->template getConstant<double>(3);
    ASSERT_THROW(dd3 = dd3.maxAbstract({x.second}), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(dd3 = dd3.maxAbstract({x.first}));
    EXPECT_EQ(0ul, dd3.getNonZeroCount());
    EXPECT_EQ(3, dd3.getMax());
}

TEST(SylvanDd, SwapTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::Sylvan>> manager(new storm::dd::DdManager<storm::dd::DdType::Sylvan>());
    
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x = manager->addMetaVariable("x", 1, 9);
    std::pair<storm::expressions::Variable, storm::expressions::Variable> z = manager->addMetaVariable("z", 2, 8);
    storm::dd::Add<storm::dd::DdType::Sylvan, double> dd1;
    
    dd1 = manager->template getIdentity<double>(x.first);
    ASSERT_THROW(dd1 = dd1.swapVariables({std::make_pair(x.first, z.first)}), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(dd1 = dd1.swapVariables({std::make_pair(x.first, x.second)}));
    EXPECT_TRUE(dd1 == manager->template getIdentity<double>(x.second));
}

TEST(SylvanDd, MultiplyMatrixTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::Sylvan>> manager(new storm::dd::DdManager<storm::dd::DdType::Sylvan>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x = manager->addMetaVariable("x", 1, 9);
    
    storm::dd::Add<storm::dd::DdType::Sylvan, double> dd1 = manager->template getIdentity<double>(x.first).equals(manager->template getIdentity<double>(x.second)).template toAdd<double>();
    storm::dd::Add<storm::dd::DdType::Sylvan, double> dd2 = manager->getRange(x.second).template toAdd<double>();
    storm::dd::Add<storm::dd::DdType::Sylvan, double> dd3;
    dd1 *= manager->template getConstant<double>(2);
    
    ASSERT_NO_THROW(dd3 = dd1.multiplyMatrix(dd2, {x.second}));
    ASSERT_NO_THROW(dd3 = dd3.swapVariables({std::make_pair(x.first, x.second)}));
    EXPECT_TRUE(dd3 == dd2 * manager->template getConstant<double>(2));
}

TEST(SylvanDd, GetSetValueTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::Sylvan>> manager(new storm::dd::DdManager<storm::dd::DdType::Sylvan>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x = manager->addMetaVariable("x", 1, 9);
    
    storm::dd::Add<storm::dd::DdType::Sylvan, double> dd1 = manager->template getAddOne<double>();
    ASSERT_NO_THROW(dd1.setValue(x.first, 4, 2));
    EXPECT_EQ(2ul, dd1.getLeafCount());
    
    std::map<storm::expressions::Variable, int_fast64_t> metaVariableToValueMap;
    metaVariableToValueMap.emplace(x.first, 1);
    EXPECT_EQ(1, dd1.getValue(metaVariableToValueMap));
    
    metaVariableToValueMap.clear();
    metaVariableToValueMap.emplace(x.first, 4);
    EXPECT_EQ(2, dd1.getValue(metaVariableToValueMap));
}

TEST(SylvanDd, AddIteratorTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::Sylvan>> manager(new storm::dd::DdManager<storm::dd::DdType::Sylvan>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x = manager->addMetaVariable("x", 1, 9);
    std::pair<storm::expressions::Variable, storm::expressions::Variable> y = manager->addMetaVariable("y", 0, 3);
    
    storm::dd::Add<storm::dd::DdType::Sylvan, double> dd;
    ASSERT_NO_THROW(dd = manager->getRange(x.first).template toAdd<double>());
    
    storm::dd::AddIterator<storm::dd::DdType::Sylvan, double> it, ite;
    ASSERT_NO_THROW(it = dd.begin());
    ASSERT_NO_THROW(ite = dd.end());
    std::pair<storm::expressions::SimpleValuation, double> valuationValuePair;
    uint_fast64_t numberOfValuations = 0;
    dd.exportToDot("dd.dot");
    while (it != ite) {
        ASSERT_NO_THROW(valuationValuePair = *it);
        ASSERT_NO_THROW(++it);
        ++numberOfValuations;
    }
    EXPECT_EQ(9ul, numberOfValuations);
    
    dd = manager->getRange(x.first).template toAdd<double>();
    dd = dd.notZero().ite(manager->template getAddOne<double>(), manager->template getAddOne<double>());
    ASSERT_NO_THROW(it = dd.begin());
    ASSERT_NO_THROW(ite = dd.end());
    numberOfValuations = 0;
    while (it != ite) {
        ASSERT_NO_THROW(valuationValuePair = *it);
        ASSERT_NO_THROW(++it);
        ++numberOfValuations;
    }
    EXPECT_EQ(16ul, numberOfValuations);
    
    ASSERT_NO_THROW(it = dd.begin(false));
    ASSERT_NO_THROW(ite = dd.end());
    numberOfValuations = 0;
    while (it != ite) {
        ASSERT_NO_THROW(valuationValuePair = *it);
        ASSERT_NO_THROW(++it);
        ++numberOfValuations;
    }
    EXPECT_EQ(1ul, numberOfValuations);
}

TEST(SylvanDd, AddOddTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::Sylvan>> manager(new storm::dd::DdManager<storm::dd::DdType::Sylvan>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> a = manager->addMetaVariable("a");
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x = manager->addMetaVariable("x", 1, 9);
    
    storm::dd::Add<storm::dd::DdType::Sylvan, double> dd = manager->template getIdentity<double>(x.first);
    storm::dd::Odd odd;
    ASSERT_NO_THROW(odd = dd.createOdd());
    EXPECT_EQ(9ul, odd.getTotalOffset());
    EXPECT_EQ(12ul, odd.getNodeCount());
    
    std::vector<double> ddAsVector;
    ASSERT_NO_THROW(ddAsVector = dd.toVector());
    EXPECT_EQ(9ul, ddAsVector.size());
    for (uint_fast64_t i = 0; i < ddAsVector.size(); ++i) {
        EXPECT_TRUE(i+1 == ddAsVector[i]);
    }
    
    // Create a non-trivial matrix.
    dd = manager->template getIdentity<double>(x.first).equals(manager->template getIdentity<double>(x.second)).template toAdd<double>() * manager->getRange(x.first).template toAdd<double>();
    dd += manager->getEncoding(x.first, 1).template toAdd<double>() * manager->getRange(x.second).template toAdd<double>() + manager->getEncoding(x.second, 1).template toAdd<double>() * manager->getRange(x.first).template toAdd<double>();
    
    // Create the ODDs.
    storm::dd::Odd rowOdd;
    ASSERT_NO_THROW(rowOdd = manager->getRange(x.first).template toAdd<double>().createOdd());
    storm::dd::Odd columnOdd;
    ASSERT_NO_THROW(columnOdd = manager->getRange(x.second).template toAdd<double>().createOdd());
    
    // Try to translate the matrix.
    storm::storage::SparseMatrix<double> matrix;
    ASSERT_NO_THROW(matrix = dd.toMatrix({x.first}, {x.second}, rowOdd, columnOdd));
    
    EXPECT_EQ(9ul, matrix.getRowCount());
    EXPECT_EQ(9ul, matrix.getColumnCount());
    EXPECT_EQ(25ul, matrix.getNonzeroEntryCount());
    
    dd = manager->getRange(x.first).template toAdd<double>() * manager->getRange(x.second).template toAdd<double>() * manager->getEncoding(a.first, 0).ite(dd, dd + manager->template getConstant<double>(1));
    ASSERT_NO_THROW(matrix = dd.toMatrix({a.first}, rowOdd, columnOdd));
    EXPECT_EQ(18ul, matrix.getRowCount());
    EXPECT_EQ(9ul, matrix.getRowGroupCount());
    EXPECT_EQ(9ul, matrix.getColumnCount());
    EXPECT_EQ(106ul, matrix.getNonzeroEntryCount());
}

TEST(SylvanDd, BddOddTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::Sylvan>> manager(new storm::dd::DdManager<storm::dd::DdType::Sylvan>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> a = manager->addMetaVariable("a");
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x = manager->addMetaVariable("x", 1, 9);
    
    storm::dd::Add<storm::dd::DdType::Sylvan, double> dd = manager->template getIdentity<double>(x.first);
    storm::dd::Bdd<storm::dd::DdType::Sylvan> bdd = dd.notZero();
    storm::dd::Odd odd;
    ASSERT_NO_THROW(odd = bdd.createOdd());
    EXPECT_EQ(9ul, odd.getTotalOffset());
    EXPECT_EQ(5ul, odd.getNodeCount());

    std::vector<double> ddAsVector;
    ASSERT_NO_THROW(ddAsVector = dd.toVector());
    EXPECT_EQ(9ul, ddAsVector.size());
    for (uint_fast64_t i = 0; i < ddAsVector.size(); ++i) {
        EXPECT_EQ(i+1, ddAsVector[i]);
    }
    
    storm::dd::Add<storm::dd::DdType::Sylvan, double> vectorAdd = storm::dd::Add<storm::dd::DdType::Sylvan, double>::fromVector(*manager, ddAsVector, odd, {x.first});
    
    // Create a non-trivial matrix.
    dd = manager->template getIdentity<double>(x.first).equals(manager->template getIdentity<double>(x.second)).template toAdd<double>() * manager->getRange(x.first).template toAdd<double>();
    dd += manager->getEncoding(x.first, 1).template toAdd<double>() * manager->getRange(x.second).template toAdd<double>() + manager->getEncoding(x.second, 1).template toAdd<double>() * manager->getRange(x.first).template toAdd<double>();
    
    // Create the ODDs.
    storm::dd::Odd rowOdd;
    ASSERT_NO_THROW(rowOdd = manager->getRange(x.first).createOdd());
    storm::dd::Odd columnOdd;
    ASSERT_NO_THROW(columnOdd = manager->getRange(x.second).createOdd());
    
    // Try to translate the matrix.
    storm::storage::SparseMatrix<double> matrix;
    ASSERT_NO_THROW(matrix = dd.toMatrix({x.first}, {x.second}, rowOdd, columnOdd));
    
    EXPECT_EQ(9ul, matrix.getRowCount());
    EXPECT_EQ(9ul, matrix.getColumnCount());
    EXPECT_EQ(25ul, matrix.getNonzeroEntryCount());
    
    dd = manager->getRange(x.first).template toAdd<double>() * manager->getRange(x.second).template toAdd<double>() * manager->getEncoding(a.first, 0).ite(dd, dd + manager->template getConstant<double>(1));
    ASSERT_NO_THROW(matrix = dd.toMatrix({a.first}, rowOdd, columnOdd));
    EXPECT_EQ(18ul, matrix.getRowCount());
    EXPECT_EQ(9ul, matrix.getRowGroupCount());
    EXPECT_EQ(9ul, matrix.getColumnCount());
    EXPECT_EQ(106ul, matrix.getNonzeroEntryCount());
}
