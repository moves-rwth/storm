#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/storage/dd/CuddDdManager.h"
#include "src/storage/dd/CuddDd.h"
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
    EXPECT_EQ(1, manager->getNumberOfMetaVariables());
    
    std::vector<std::string> names = {"x", "x'"};
    ASSERT_THROW(manager->addMetaVariablesInterleaved(names, 0, 3), storm::exceptions::InvalidArgumentException);

    names = {"y", "y"};
    ASSERT_THROW(manager->addMetaVariablesInterleaved(names, 0, 3), storm::exceptions::InvalidArgumentException);
    
    names = {"y", "y'"};
    ASSERT_NO_THROW(manager->addMetaVariablesInterleaved(names, 0, 3));
    EXPECT_EQ(3, manager->getNumberOfMetaVariables());
    
    EXPECT_FALSE(manager->hasMetaVariable("x'"));
    EXPECT_TRUE(manager->hasMetaVariable("y'"));
    
    std::set<std::string> metaVariableSet = {"x", "y", "y'"};
    EXPECT_EQ(metaVariableSet, manager->getAllMetaVariableNames());
    
    ASSERT_THROW(storm::dd::DdMetaVariable<storm::dd::DdType::CUDD> const& metaVariableX = manager->getMetaVariable("x'"), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(storm::dd::DdMetaVariable<storm::dd::DdType::CUDD> const& metaVariableX = manager->getMetaVariable("x"));
}

TEST(CuddDdManager, EncodingTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    manager->addMetaVariable("x", 1, 9);
    
    storm::dd::Dd<storm::dd::DdType::CUDD> encoding;
    ASSERT_THROW(encoding = manager->getEncoding("x", 0), storm::exceptions::InvalidArgumentException);
    ASSERT_THROW(encoding = manager->getEncoding("x", 10), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(encoding = manager->getEncoding("x", 4));
    EXPECT_EQ(1, encoding.getNonZeroCount());
    EXPECT_EQ(6, encoding.getNodeCount());
    EXPECT_EQ(2, encoding.getLeafCount());
}

TEST(CuddDdManager, RangeTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    ASSERT_NO_THROW(manager->addMetaVariable("x", 1, 9));
    
    storm::dd::Dd<storm::dd::DdType::CUDD> range;
    ASSERT_THROW(range = manager->getRange("y"), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(range = manager->getRange("x"));
    
    EXPECT_EQ(9, range.getNonZeroCount());
    EXPECT_EQ(2, range.getLeafCount());
    EXPECT_EQ(6, range.getNodeCount());
}

TEST(CuddDdManager, IdentityTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    manager->addMetaVariable("x", 1, 9);
    
    storm::dd::Dd<storm::dd::DdType::CUDD> range;
    ASSERT_THROW(range = manager->getIdentity("y"), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(range = manager->getIdentity("x"));
    
    EXPECT_EQ(9, range.getNonZeroCount());
    EXPECT_EQ(10, range.getLeafCount());
    EXPECT_EQ(21, range.getNodeCount());
}

TEST(CuddDdMetaVariable, AccessorTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    manager->addMetaVariable("x", 1, 9);
    EXPECT_EQ(1, manager->getNumberOfMetaVariables());
    ASSERT_NO_THROW(storm::dd::DdMetaVariable<storm::dd::DdType::CUDD> const& metaVariableX = manager->getMetaVariable("x"));
    storm::dd::DdMetaVariable<storm::dd::DdType::CUDD> const& metaVariableX = manager->getMetaVariable("x");
    
    EXPECT_EQ(1, metaVariableX.getLow());
    EXPECT_EQ(9, metaVariableX.getHigh());
    EXPECT_EQ("x", metaVariableX.getName());
    EXPECT_EQ(manager, metaVariableX.getDdManager());
    EXPECT_EQ(4, metaVariableX.getNumberOfDdVariables());
}

TEST(CuddDd, OperatorTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    manager->addMetaVariable("x", 1, 9);
    EXPECT_TRUE(manager->getZero() == manager->getZero());
    EXPECT_FALSE(manager->getZero() == manager->getOne());

    EXPECT_FALSE(manager->getZero() != manager->getZero());
    EXPECT_TRUE(manager->getZero() != manager->getOne());
    
    storm::dd::Dd<storm::dd::DdType::CUDD> dd1 = manager->getOne();
    storm::dd::Dd<storm::dd::DdType::CUDD> dd2 = manager->getOne();
    storm::dd::Dd<storm::dd::DdType::CUDD> dd3 = dd1 + dd2;
    EXPECT_TRUE(dd3 == manager->getConstant(2));
    
    dd3 += manager->getZero();
    EXPECT_TRUE(dd3 == manager->getConstant(2));
    
    dd3 = dd1 && manager->getConstant(3);
    EXPECT_TRUE(dd1 == manager->getOne());

    dd3 = dd1 * manager->getConstant(3);
    EXPECT_TRUE(dd3 == manager->getConstant(3));

    dd3 *= manager->getConstant(2);
    EXPECT_TRUE(dd3 == manager->getConstant(6));
    
    dd3 = dd1 - dd2;
    EXPECT_TRUE(dd3 == manager->getZero());
    
    dd3 -= manager->getConstant(-2);
    EXPECT_TRUE(dd3 == manager->getConstant(2));
    
    dd3 /= manager->getConstant(2);
    EXPECT_TRUE(dd3 == manager->getOne());
    
    dd3.complement();
    EXPECT_TRUE(dd3 == manager->getZero());
    
    dd1 = !dd3;
    EXPECT_TRUE(dd1 == manager->getOne());

    dd3 = dd1 || dd2;
    EXPECT_TRUE(dd3 == manager->getOne());
    
    dd1 = manager->getIdentity("x");
    dd2 = manager->getConstant(5);

    dd3 = dd1.equals(dd2);
    EXPECT_EQ(1, dd3.getNonZeroCount());
    
    storm::dd::Dd<storm::dd::DdType::CUDD> dd4 = dd1.notEquals(dd2);
    EXPECT_TRUE(dd4 == !dd3);
    
    dd3 = dd1.less(dd2);
    EXPECT_EQ(11, dd3.getNonZeroCount());
    
    dd3 = dd1.lessOrEqual(dd2);
    EXPECT_EQ(12, dd3.getNonZeroCount());
    
    dd3 = dd1.greater(dd2);
    EXPECT_EQ(4, dd3.getNonZeroCount());

    dd3 = dd1.greaterOrEqual(dd2);
    EXPECT_EQ(5, dd3.getNonZeroCount());
    
    dd1 = manager->getConstant(0.01);
    dd2 = manager->getConstant(0.01 + 1e-6);
    EXPECT_TRUE(dd1.equalModuloPrecision(dd2, 1e-6, false));
    EXPECT_FALSE(dd1.equalModuloPrecision(dd2, 1e-6));
}

TEST(CuddDd, AbstractionTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    manager->addMetaVariablesInterleaved({"x", "x'"}, 1, 9);
    storm::dd::Dd<storm::dd::DdType::CUDD> dd1;
    storm::dd::Dd<storm::dd::DdType::CUDD> dd2;
    storm::dd::Dd<storm::dd::DdType::CUDD> dd3;
    
    dd1 = manager->getIdentity("x");
    dd2 = manager->getConstant(5);
    dd3 = dd1.equals(dd2);
    EXPECT_EQ(1, dd3.getNonZeroCount());
    ASSERT_THROW(dd3.existsAbstract({"x'"}), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(dd3.existsAbstract({"x"}));
    EXPECT_EQ(1, dd3.getNonZeroCount());
    EXPECT_EQ(1, dd3.getMax());

    dd3 = dd1.equals(dd2);
    dd3 *= manager->getConstant(3);
    EXPECT_EQ(1, dd3.getNonZeroCount());
    ASSERT_THROW(dd3.existsAbstract({"x'"}), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(dd3.existsAbstract({"x"}));
    EXPECT_TRUE(dd3 == manager->getZero());

    dd3 = dd1.equals(dd2);
    dd3 *= manager->getConstant(3);
    ASSERT_THROW(dd3.sumAbstract({"x'"}), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(dd3.sumAbstract({"x"}));
    EXPECT_EQ(1, dd3.getNonZeroCount());
    EXPECT_EQ(3, dd3.getMax());

    dd3 = dd1.equals(dd2);
    dd3 *= manager->getConstant(3);
    ASSERT_THROW(dd3.minAbstract({"x'"}), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(dd3.minAbstract({"x"}));
    EXPECT_EQ(0, dd3.getNonZeroCount());
    EXPECT_EQ(0, dd3.getMax());

    dd3 = dd1.equals(dd2);
    dd3 *= manager->getConstant(3);
    ASSERT_THROW(dd3.maxAbstract({"x'"}), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(dd3.maxAbstract({"x"}));
    EXPECT_EQ(1, dd3.getNonZeroCount());
    EXPECT_EQ(3, dd3.getMax());
}

TEST(CuddDd, SwapTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    
    manager->addMetaVariablesInterleaved({"x", "x'"}, 1, 9);
    manager->addMetaVariable("z", 2, 8);
    storm::dd::Dd<storm::dd::DdType::CUDD> dd1;
    storm::dd::Dd<storm::dd::DdType::CUDD> dd2;
    
    dd1 = manager->getIdentity("x");
    ASSERT_THROW(dd1.swapVariables({std::make_pair("x", "z")}), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(dd1.swapVariables({std::make_pair("x", "x'")}));
    EXPECT_TRUE(dd1 == manager->getIdentity("x'"));
}

TEST(CuddDd, MultiplyMatrixTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    manager->addMetaVariablesInterleaved({"x", "x'"}, 1, 9);
    
    storm::dd::Dd<storm::dd::DdType::CUDD> dd1 = manager->getIdentity("x").equals(manager->getIdentity("x'"));
    storm::dd::Dd<storm::dd::DdType::CUDD> dd2 = manager->getRange("x'");
    storm::dd::Dd<storm::dd::DdType::CUDD> dd3;
    dd1 *= manager->getConstant(2);
    
    ASSERT_NO_THROW(dd3 = dd1.multiplyMatrix(dd2, {"x'"}));
    ASSERT_NO_THROW(dd3.swapVariables({std::make_pair("x", "x'")}));
    EXPECT_TRUE(dd3 == dd2 * manager->getConstant(2));
}

TEST(CuddDd, GetSetValueTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    manager->addMetaVariable("x", 1, 9);
    
    storm::dd::Dd<storm::dd::DdType::CUDD> dd1 = manager->getOne();
    ASSERT_NO_THROW(dd1.setValue("x", 4, 2));
    EXPECT_EQ(2, dd1.getLeafCount());
    
    std::map<std::string, int_fast64_t> metaVariableToValueMap;
    metaVariableToValueMap.emplace("x", 1);
    EXPECT_EQ(1, dd1.getValue(metaVariableToValueMap));
    
    metaVariableToValueMap.clear();
    metaVariableToValueMap.emplace("x", 4);
    EXPECT_EQ(2, dd1.getValue(metaVariableToValueMap));
}
