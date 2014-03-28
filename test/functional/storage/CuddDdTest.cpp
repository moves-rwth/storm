#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/storage/dd/CuddDdManager.h"
#include "src/storage/dd/CuddDd.h"
#include "src/storage/dd/DdMetaVariable.h"

TEST(CuddDdManager, Constants) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::CUDD>> manager(new storm::dd::DdManager<storm::dd::CUDD>());
    
    storm::dd::Dd<storm::dd::CUDD> zero;
    ASSERT_NO_THROW(zero = manager->getZero());
    
    EXPECT_EQ(0, zero.getNonZeroCount());
    EXPECT_EQ(1, zero.getLeafCount());
    EXPECT_EQ(1, zero.getNodeCount());
    EXPECT_EQ(0, zero.getMin());
    EXPECT_EQ(0, zero.getMax());
    
    storm::dd::Dd<storm::dd::CUDD> one;
    ASSERT_NO_THROW(one = manager->getOne());
    
    EXPECT_EQ(1, one.getNonZeroCount());
    EXPECT_EQ(1, one.getLeafCount());
    EXPECT_EQ(1, one.getNodeCount());
    EXPECT_EQ(1, one.getMin());
    EXPECT_EQ(1, one.getMax());

    storm::dd::Dd<storm::dd::CUDD> two;
    ASSERT_NO_THROW(two = manager->getConstant(2));
    
    EXPECT_EQ(1, two.getNonZeroCount());
    EXPECT_EQ(1, two.getLeafCount());
    EXPECT_EQ(1, two.getNodeCount());
    EXPECT_EQ(2, two.getMin());
    EXPECT_EQ(2, two.getMax());
}

TEST(CuddDdManager, AddMetaVariableTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::CUDD>> manager(new storm::dd::DdManager<storm::dd::CUDD>());
    
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
    
    ASSERT_THROW(storm::dd::DdMetaVariable<storm::dd::CUDD> const& metaVariableX = manager->getMetaVariable("x'"), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(storm::dd::DdMetaVariable<storm::dd::CUDD> const& metaVariableX = manager->getMetaVariable("x"));
}

TEST(CuddDdManager, EncodingTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::CUDD>> manager(new storm::dd::DdManager<storm::dd::CUDD>());
    
    ASSERT_NO_THROW(manager->addMetaVariable("x", 1, 9));
    
    storm::dd::Dd<storm::dd::CUDD> encoding;
    ASSERT_THROW(encoding = manager->getEncoding("x", 0), storm::exceptions::InvalidArgumentException);
    ASSERT_THROW(encoding = manager->getEncoding("x", 10), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(encoding = manager->getEncoding("x", 4));
    encoding.exportToDot("out.dot");
    EXPECT_EQ(1, encoding.getNonZeroCount());
    EXPECT_EQ(6, encoding.getNodeCount());
    EXPECT_EQ(2, encoding.getLeafCount());
}

TEST(CuddDdMetaVariable, AccessorTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::CUDD>> manager(new storm::dd::DdManager<storm::dd::CUDD>());
    
    ASSERT_NO_THROW(manager->addMetaVariable("x", 1, 9));
    EXPECT_EQ(1, manager->getNumberOfMetaVariables());
    ASSERT_NO_THROW(storm::dd::DdMetaVariable<storm::dd::CUDD> const& metaVariableX = manager->getMetaVariable("x"));
    storm::dd::DdMetaVariable<storm::dd::CUDD> const& metaVariableX = manager->getMetaVariable("x");
    
    EXPECT_EQ(1, metaVariableX.getLow());
    EXPECT_EQ(9, metaVariableX.getHigh());
    EXPECT_EQ("x", metaVariableX.getName());
    EXPECT_EQ(manager, metaVariableX.getDdManager());
    EXPECT_EQ(4, metaVariableX.getNumberOfDdVariables());
}

//TEST(CuddDd, OperatorTest) {
//    std::shared_ptr<storm::dd::DdManager<storm::dd::CUDD>> manager(new storm::dd::DdManager<storm::dd::CUDD>());
//    
//    ASSERT_NO_THROW(manager->addMetaVariable("x", 1, 9));
//    EXPECT_EQ(manager->getZero(), manager->getZero());
//    EXPECT_NE(manager->getZero(), manager->getOne());
//    
//    storm::dd::Dd<storm::dd::CUDD> add;
//    
//}
