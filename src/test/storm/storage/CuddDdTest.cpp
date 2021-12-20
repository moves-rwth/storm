#include "storm-config.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/settings/SettingsManager.h"
#include "storm/storage/dd/Add.h"
#include "storm/storage/dd/DdManager.h"
#include "storm/storage/dd/DdMetaVariable.h"
#include "storm/storage/dd/Odd.h"
#include "storm/storage/expressions/Expression.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "test/storm_gtest.h"

#include "storm/storage/SparseMatrix.h"

TEST(CuddDd, AddConstants) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    storm::dd::Add<storm::dd::DdType::CUDD, double> zero;
    ASSERT_NO_THROW(zero = manager->template getAddZero<double>());

    EXPECT_EQ(0ul, zero.getNonZeroCount());
    EXPECT_EQ(1ul, zero.getLeafCount());
    EXPECT_EQ(1ul, zero.getNodeCount());
    EXPECT_EQ(0, zero.getMin());
    EXPECT_EQ(0, zero.getMax());

    storm::dd::Add<storm::dd::DdType::CUDD, double> one;
    ASSERT_NO_THROW(one = manager->template getAddOne<double>());

    EXPECT_EQ(0ul, one.getNonZeroCount());
    EXPECT_EQ(1ul, one.getLeafCount());
    EXPECT_EQ(1ul, one.getNodeCount());
    EXPECT_EQ(1, one.getMin());
    EXPECT_EQ(1, one.getMax());

    storm::dd::Add<storm::dd::DdType::CUDD, double> two;
    ASSERT_NO_THROW(two = manager->template getConstant<double>(2));

    EXPECT_EQ(0ul, two.getNonZeroCount());
    EXPECT_EQ(1ul, two.getLeafCount());
    EXPECT_EQ(1ul, two.getNodeCount());
    EXPECT_EQ(2, two.getMin());
    EXPECT_EQ(2, two.getMax());
}

TEST(CuddDd, BddConstants) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    storm::dd::Bdd<storm::dd::DdType::CUDD> zero;
    ASSERT_NO_THROW(zero = manager->getBddZero());

    EXPECT_EQ(0ul, zero.getNonZeroCount());
    EXPECT_EQ(1ul, zero.getLeafCount());
    EXPECT_EQ(1ul, zero.getNodeCount());

    storm::dd::Bdd<storm::dd::DdType::CUDD> one;
    ASSERT_NO_THROW(one = manager->getBddOne());

    EXPECT_EQ(0ul, one.getNonZeroCount());
    EXPECT_EQ(1ul, one.getLeafCount());
    EXPECT_EQ(1ul, one.getNodeCount());
}

TEST(CuddDd, BddExistAbstractRepresentative) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());

    storm::dd::Bdd<storm::dd::DdType::CUDD> zero;
    ASSERT_NO_THROW(zero = manager->getBddZero());
    storm::dd::Bdd<storm::dd::DdType::CUDD> one;
    ASSERT_NO_THROW(one = manager->getBddOne());

    std::pair<storm::expressions::Variable, storm::expressions::Variable> x;
    std::pair<storm::expressions::Variable, storm::expressions::Variable> y;
    std::pair<storm::expressions::Variable, storm::expressions::Variable> z;
    ASSERT_NO_THROW(x = manager->addMetaVariable("x", 0, 1));
    ASSERT_NO_THROW(y = manager->addMetaVariable("y", 0, 1));
    ASSERT_NO_THROW(z = manager->addMetaVariable("z", 0, 1));

    storm::dd::Bdd<storm::dd::DdType::CUDD> bddX0 = manager->getEncoding(x.first, 0);
    storm::dd::Bdd<storm::dd::DdType::CUDD> bddX1 = manager->getEncoding(x.first, 1);
    storm::dd::Bdd<storm::dd::DdType::CUDD> bddY0 = manager->getEncoding(y.first, 0);
    storm::dd::Bdd<storm::dd::DdType::CUDD> bddY1 = manager->getEncoding(y.first, 1);
    storm::dd::Bdd<storm::dd::DdType::CUDD> bddZ0 = manager->getEncoding(z.first, 0);
    storm::dd::Bdd<storm::dd::DdType::CUDD> bddZ1 = manager->getEncoding(z.first, 1);

    // Abstract from FALSE
    storm::dd::Bdd<storm::dd::DdType::CUDD> representative_false_x = zero.existsAbstractRepresentative({x.first});
    EXPECT_EQ(0ul, representative_false_x.getNonZeroCount());
    EXPECT_EQ(1ul, representative_false_x.getLeafCount());
    EXPECT_EQ(1ul, representative_false_x.getNodeCount());
    EXPECT_TRUE(representative_false_x == zero);

    // Abstract from TRUE
    storm::dd::Bdd<storm::dd::DdType::CUDD> representative_true_x = one.existsAbstractRepresentative({x.first});
    EXPECT_EQ(0ul, representative_true_x.getNonZeroCount());
    EXPECT_EQ(1ul, representative_true_x.getLeafCount());
    EXPECT_EQ(2ul, representative_true_x.getNodeCount());
    EXPECT_TRUE(representative_true_x == bddX0);

    storm::dd::Bdd<storm::dd::DdType::CUDD> representative_true_xyz = one.existsAbstractRepresentative({x.first, y.first, z.first});
    EXPECT_EQ(0ul, representative_true_xyz.getNonZeroCount());
    EXPECT_EQ(1ul, representative_true_xyz.getLeafCount());
    EXPECT_EQ(4ul, representative_true_xyz.getNodeCount());
    EXPECT_TRUE(representative_true_xyz == ((bddX0 && bddY0) && bddZ0));

    storm::dd::Bdd<storm::dd::DdType::CUDD> bddX1Y0Z0 = (bddX1 && bddY0) && bddZ0;
    EXPECT_EQ(1ul, bddX1Y0Z0.getNonZeroCount());
    EXPECT_EQ(1ul, bddX1Y0Z0.getLeafCount());
    EXPECT_EQ(4ul, bddX1Y0Z0.getNodeCount());

    storm::dd::Bdd<storm::dd::DdType::CUDD> representative_x = bddX1Y0Z0.existsAbstractRepresentative({x.first});
    EXPECT_EQ(1ul, representative_x.getNonZeroCount());
    EXPECT_EQ(1ul, representative_x.getLeafCount());
    EXPECT_EQ(4ul, representative_x.getNodeCount());
    EXPECT_TRUE(bddX1Y0Z0 == representative_x);

    storm::dd::Bdd<storm::dd::DdType::CUDD> representative_y = bddX1Y0Z0.existsAbstractRepresentative({y.first});
    EXPECT_EQ(1ul, representative_y.getNonZeroCount());
    EXPECT_EQ(1ul, representative_y.getLeafCount());
    EXPECT_EQ(4ul, representative_y.getNodeCount());
    EXPECT_TRUE(bddX1Y0Z0 == representative_y);

    storm::dd::Bdd<storm::dd::DdType::CUDD> representative_z = bddX1Y0Z0.existsAbstractRepresentative({z.first});
    EXPECT_EQ(1ul, representative_z.getNonZeroCount());
    EXPECT_EQ(1ul, representative_z.getLeafCount());
    EXPECT_EQ(4ul, representative_z.getNodeCount());
    EXPECT_TRUE(bddX1Y0Z0 == representative_z);

    storm::dd::Bdd<storm::dd::DdType::CUDD> representative_xyz = bddX1Y0Z0.existsAbstractRepresentative({x.first, y.first, z.first});
    EXPECT_EQ(1ul, representative_xyz.getNonZeroCount());
    EXPECT_EQ(1ul, representative_xyz.getLeafCount());
    EXPECT_EQ(4ul, representative_xyz.getNodeCount());
    EXPECT_TRUE(bddX1Y0Z0 == representative_xyz);

    storm::dd::Bdd<storm::dd::DdType::CUDD> bddX0Y0Z0 = (bddX0 && bddY0) && bddZ0;
    storm::dd::Bdd<storm::dd::DdType::CUDD> bddX1Y1Z1 = (bddX1 && bddY1) && bddZ1;

    storm::dd::Bdd<storm::dd::DdType::CUDD> bddAllTrueOrAllFalse = bddX0Y0Z0 || bddX1Y1Z1;
    // bddAllTrueOrAllFalse.template toAdd<double>().exportToDot("test_cudd_addAllTrueOrAllFalse.dot");

    representative_x = bddAllTrueOrAllFalse.existsAbstractRepresentative({x.first});
    EXPECT_EQ(2ul, representative_x.getNonZeroCount());
    EXPECT_EQ(1ul, representative_x.getLeafCount());
    EXPECT_EQ(5ul, representative_x.getNodeCount());
    EXPECT_TRUE(bddAllTrueOrAllFalse == representative_x);

    representative_y = bddAllTrueOrAllFalse.existsAbstractRepresentative({y.first});
    EXPECT_EQ(2ul, representative_y.getNonZeroCount());
    EXPECT_EQ(1ul, representative_y.getLeafCount());
    EXPECT_EQ(5ul, representative_y.getNodeCount());
    EXPECT_TRUE(bddAllTrueOrAllFalse == representative_y);

    representative_z = bddAllTrueOrAllFalse.existsAbstractRepresentative({z.first});
    EXPECT_EQ(2ul, representative_z.getNonZeroCount());
    EXPECT_EQ(1ul, representative_z.getLeafCount());
    EXPECT_EQ(5ul, representative_z.getNodeCount());
    EXPECT_TRUE(bddAllTrueOrAllFalse == representative_z);

    representative_xyz = bddAllTrueOrAllFalse.existsAbstractRepresentative({x.first, y.first, z.first});
    EXPECT_EQ(1ul, representative_xyz.getNonZeroCount());
    EXPECT_EQ(1ul, representative_xyz.getLeafCount());
    EXPECT_EQ(4ul, representative_xyz.getNodeCount());
    EXPECT_TRUE(bddX0Y0Z0 == representative_xyz);
}

TEST(CuddDd, AddMinExistAbstractRepresentative) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());

    storm::dd::Bdd<storm::dd::DdType::CUDD> bddZero;
    ASSERT_NO_THROW(bddZero = manager->getBddZero());
    storm::dd::Bdd<storm::dd::DdType::CUDD> bddOne;
    ASSERT_NO_THROW(bddOne = manager->getBddOne());

    storm::dd::Add<storm::dd::DdType::CUDD, double> addZero;
    ASSERT_NO_THROW(addZero = manager->template getAddZero<double>());
    storm::dd::Add<storm::dd::DdType::CUDD, double> addOne;
    ASSERT_NO_THROW(addOne = manager->template getAddOne<double>());

    std::pair<storm::expressions::Variable, storm::expressions::Variable> x;
    std::pair<storm::expressions::Variable, storm::expressions::Variable> y;
    std::pair<storm::expressions::Variable, storm::expressions::Variable> z;
    ASSERT_NO_THROW(x = manager->addMetaVariable("x", 0, 1));
    ASSERT_NO_THROW(y = manager->addMetaVariable("y", 0, 1));
    ASSERT_NO_THROW(z = manager->addMetaVariable("z", 0, 1));

    storm::dd::Bdd<storm::dd::DdType::CUDD> bddX0 = manager->getEncoding(x.first, 0);
    storm::dd::Bdd<storm::dd::DdType::CUDD> bddX1 = manager->getEncoding(x.first, 1);
    storm::dd::Bdd<storm::dd::DdType::CUDD> bddY0 = manager->getEncoding(y.first, 0);
    storm::dd::Bdd<storm::dd::DdType::CUDD> bddY1 = manager->getEncoding(y.first, 1);
    storm::dd::Bdd<storm::dd::DdType::CUDD> bddZ0 = manager->getEncoding(z.first, 0);
    storm::dd::Bdd<storm::dd::DdType::CUDD> bddZ1 = manager->getEncoding(z.first, 1);

    storm::dd::Add<storm::dd::DdType::CUDD, double> complexAdd =
        ((bddX1 && (bddY1 && bddZ1)).template toAdd<double>() * manager->template getConstant<double>(0.4)) +
        ((bddX1 && (bddY1 && bddZ0)).template toAdd<double>() * manager->template getConstant<double>(0.7)) +
        ((bddX1 && (bddY0 && bddZ1)).template toAdd<double>() * manager->template getConstant<double>(0.3)) +
        ((bddX1 && (bddY0 && bddZ0)).template toAdd<double>() * manager->template getConstant<double>(0.3)) +
        ((bddX0 && (bddY1 && bddZ1)).template toAdd<double>() * manager->template getConstant<double>(0.9)) +
        ((bddX0 && (bddY1 && bddZ0)).template toAdd<double>() * manager->template getConstant<double>(0.5)) +
        ((bddX0 && (bddY0 && bddZ1)).template toAdd<double>() * manager->template getConstant<double>(1.0)) +
        ((bddX0 && (bddY0 && bddZ0)).template toAdd<double>() * manager->template getConstant<double>(0.0));

    // Abstract from FALSE
    storm::dd::Bdd<storm::dd::DdType::CUDD> representative_false_x = addZero.minAbstractRepresentative({x.first});
    EXPECT_EQ(0ul, representative_false_x.getNonZeroCount());
    EXPECT_EQ(1ul, representative_false_x.getLeafCount());
    EXPECT_EQ(2ul, representative_false_x.getNodeCount());
    EXPECT_TRUE(representative_false_x == bddX0);

    // Abstract from TRUE
    storm::dd::Bdd<storm::dd::DdType::CUDD> representative_true_x = addOne.minAbstractRepresentative({x.first});
    EXPECT_EQ(0ul, representative_true_x.getNonZeroCount());
    EXPECT_EQ(1ul, representative_true_x.getLeafCount());
    EXPECT_EQ(2ul, representative_true_x.getNodeCount());
    EXPECT_TRUE(representative_true_x == bddX0);

    storm::dd::Bdd<storm::dd::DdType::CUDD> representative_true_xyz = addOne.minAbstractRepresentative({x.first, y.first, z.first});
    EXPECT_EQ(0ul, representative_true_xyz.getNonZeroCount());
    EXPECT_EQ(1ul, representative_true_xyz.getLeafCount());
    EXPECT_EQ(4ul, representative_true_xyz.getNodeCount());
    EXPECT_TRUE(representative_true_xyz == ((bddX0 && bddY0) && bddZ0));

    // Abstract x
    storm::dd::Bdd<storm::dd::DdType::CUDD> representative_complex_x = complexAdd.minAbstractRepresentative({x.first});
    storm::dd::Bdd<storm::dd::DdType::CUDD> comparison_complex_x =
        (((bddX0 && (bddY0 && bddZ0))) || ((bddX1 && (bddY0 && bddZ1))) || ((bddX0 && (bddY1 && bddZ0))) || ((bddX1 && (bddY1 && bddZ1))));
    EXPECT_EQ(4ul, representative_complex_x.getNonZeroCount());
    EXPECT_EQ(1ul, representative_complex_x.getLeafCount());
    EXPECT_EQ(3ul, representative_complex_x.getNodeCount());
    EXPECT_TRUE(representative_complex_x == comparison_complex_x);

    // Abstract y
    storm::dd::Bdd<storm::dd::DdType::CUDD> representative_complex_y = complexAdd.minAbstractRepresentative({y.first});
    storm::dd::Bdd<storm::dd::DdType::CUDD> comparison_complex_y =
        (((bddX0 && (bddY0 && bddZ0))) || ((bddX0 && (bddY1 && bddZ1))) || ((bddX1 && (bddY0 && bddZ0))) || ((bddX1 && (bddY0 && bddZ1))));
    EXPECT_EQ(4ul, representative_complex_y.getNonZeroCount());
    EXPECT_EQ(1ul, representative_complex_y.getLeafCount());
    EXPECT_EQ(5ul, representative_complex_y.getNodeCount());
    EXPECT_TRUE(representative_complex_y == comparison_complex_y);

    // Abstract z
    storm::dd::Bdd<storm::dd::DdType::CUDD> representative_complex_z = complexAdd.minAbstractRepresentative({z.first});
    storm::dd::Bdd<storm::dd::DdType::CUDD> comparison_complex_z =
        (((bddX0 && (bddY0 && bddZ0))) || ((bddX0 && (bddY1 && bddZ0))) || ((bddX1 && (bddY0 && bddZ0))) || ((bddX1 && (bddY1 && bddZ1))));
    EXPECT_EQ(4ul, representative_complex_z.getNonZeroCount());
    EXPECT_EQ(1ul, representative_complex_z.getLeafCount());
    EXPECT_EQ(4ul, representative_complex_z.getNodeCount());
    EXPECT_TRUE(representative_complex_z == comparison_complex_z);

    // Abstract x, y, z
    storm::dd::Bdd<storm::dd::DdType::CUDD> representative_complex_xyz = complexAdd.minAbstractRepresentative({x.first, y.first, z.first});
    storm::dd::Bdd<storm::dd::DdType::CUDD> comparison_complex_xyz = (bddX0 && (bddY0 && bddZ0));
    EXPECT_EQ(1ul, representative_complex_xyz.getNonZeroCount());
    EXPECT_EQ(1ul, representative_complex_xyz.getLeafCount());
    EXPECT_EQ(4ul, representative_complex_xyz.getNodeCount());
    EXPECT_TRUE(representative_complex_xyz == comparison_complex_xyz);
}

TEST(CuddDd, AddMaxExistAbstractRepresentative) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());

    storm::dd::Bdd<storm::dd::DdType::CUDD> bddZero;
    ASSERT_NO_THROW(bddZero = manager->getBddZero());
    storm::dd::Bdd<storm::dd::DdType::CUDD> bddOne;
    ASSERT_NO_THROW(bddOne = manager->getBddOne());

    storm::dd::Add<storm::dd::DdType::CUDD, double> addZero;
    ASSERT_NO_THROW(addZero = manager->template getAddZero<double>());
    storm::dd::Add<storm::dd::DdType::CUDD, double> addOne;
    ASSERT_NO_THROW(addOne = manager->template getAddOne<double>());

    std::pair<storm::expressions::Variable, storm::expressions::Variable> x;
    std::pair<storm::expressions::Variable, storm::expressions::Variable> y;
    std::pair<storm::expressions::Variable, storm::expressions::Variable> z;
    ASSERT_NO_THROW(x = manager->addMetaVariable("x", 0, 1));
    ASSERT_NO_THROW(y = manager->addMetaVariable("y", 0, 1));
    ASSERT_NO_THROW(z = manager->addMetaVariable("z", 0, 1));

    storm::dd::Bdd<storm::dd::DdType::CUDD> bddX0 = manager->getEncoding(x.first, 0);
    storm::dd::Bdd<storm::dd::DdType::CUDD> bddX1 = manager->getEncoding(x.first, 1);
    storm::dd::Bdd<storm::dd::DdType::CUDD> bddY0 = manager->getEncoding(y.first, 0);
    storm::dd::Bdd<storm::dd::DdType::CUDD> bddY1 = manager->getEncoding(y.first, 1);
    storm::dd::Bdd<storm::dd::DdType::CUDD> bddZ0 = manager->getEncoding(z.first, 0);
    storm::dd::Bdd<storm::dd::DdType::CUDD> bddZ1 = manager->getEncoding(z.first, 1);

    storm::dd::Add<storm::dd::DdType::CUDD, double> complexAdd =
        ((bddX1 && (bddY1 && bddZ1)).template toAdd<double>() * manager->template getConstant<double>(0.4)) +
        ((bddX1 && (bddY1 && bddZ0)).template toAdd<double>() * manager->template getConstant<double>(0.7)) +
        ((bddX1 && (bddY0 && bddZ1)).template toAdd<double>() * manager->template getConstant<double>(0.3)) +
        ((bddX1 && (bddY0 && bddZ0)).template toAdd<double>() * manager->template getConstant<double>(0.3)) +
        ((bddX0 && (bddY1 && bddZ1)).template toAdd<double>() * manager->template getConstant<double>(0.9)) +
        ((bddX0 && (bddY1 && bddZ0)).template toAdd<double>() * manager->template getConstant<double>(0.5)) +
        ((bddX0 && (bddY0 && bddZ1)).template toAdd<double>() * manager->template getConstant<double>(1.0)) +
        ((bddX0 && (bddY0 && bddZ0)).template toAdd<double>() * manager->template getConstant<double>(0.0));

    // Abstract from FALSE
    storm::dd::Bdd<storm::dd::DdType::CUDD> representative_false_x = addZero.maxAbstractRepresentative({x.first});
    EXPECT_EQ(0ul, representative_false_x.getNonZeroCount());
    EXPECT_EQ(1ul, representative_false_x.getLeafCount());
    EXPECT_EQ(2ul, representative_false_x.getNodeCount());
    EXPECT_TRUE(representative_false_x == bddX0);

    // Abstract from TRUE
    storm::dd::Bdd<storm::dd::DdType::CUDD> representative_true_x = addOne.maxAbstractRepresentative({x.first});
    EXPECT_EQ(0ul, representative_true_x.getNonZeroCount());
    EXPECT_EQ(1ul, representative_true_x.getLeafCount());
    EXPECT_EQ(2ul, representative_true_x.getNodeCount());
    EXPECT_TRUE(representative_true_x == bddX0);

    storm::dd::Bdd<storm::dd::DdType::CUDD> representative_true_xyz = addOne.maxAbstractRepresentative({x.first, y.first, z.first});
    EXPECT_EQ(0ul, representative_true_xyz.getNonZeroCount());
    EXPECT_EQ(1ul, representative_true_xyz.getLeafCount());
    EXPECT_EQ(4ul, representative_true_xyz.getNodeCount());
    EXPECT_TRUE(representative_true_xyz == ((bddX0 && bddY0) && bddZ0));

    // Abstract x
    storm::dd::Bdd<storm::dd::DdType::CUDD> representative_complex_x = complexAdd.maxAbstractRepresentative({x.first});
    storm::dd::Bdd<storm::dd::DdType::CUDD> comparison_complex_x =
        (((bddX1 && (bddY0 && bddZ0))) || ((bddX0 && (bddY0 && bddZ1))) || ((bddX1 && (bddY1 && bddZ0))) || ((bddX0 && (bddY1 && bddZ1))));
    EXPECT_EQ(4ul, representative_complex_x.getNonZeroCount());
    EXPECT_EQ(1ul, representative_complex_x.getLeafCount());
    EXPECT_EQ(3ul, representative_complex_x.getNodeCount());
    EXPECT_TRUE(representative_complex_x == comparison_complex_x);

    // Abstract y
    storm::dd::Bdd<storm::dd::DdType::CUDD> representative_complex_y = complexAdd.maxAbstractRepresentative({y.first});
    storm::dd::Bdd<storm::dd::DdType::CUDD> comparison_complex_y =
        (((bddX0 && (bddY1 && bddZ0))) || ((bddX0 && (bddY0 && bddZ1))) || ((bddX1 && (bddY1 && bddZ0))) || ((bddX1 && (bddY1 && bddZ1))));
    EXPECT_EQ(4ul, representative_complex_y.getNonZeroCount());
    EXPECT_EQ(1ul, representative_complex_y.getLeafCount());
    EXPECT_EQ(5ul, representative_complex_y.getNodeCount());
    EXPECT_TRUE(representative_complex_y == comparison_complex_y);

    // Abstract z
    storm::dd::Bdd<storm::dd::DdType::CUDD> representative_complex_z = complexAdd.maxAbstractRepresentative({z.first});
    storm::dd::Bdd<storm::dd::DdType::CUDD> comparison_complex_z =
        (((bddX0 && (bddY0 && bddZ1))) || ((bddX0 && (bddY1 && bddZ1))) || ((bddX1 && (bddY0 && bddZ0))) || ((bddX1 && (bddY1 && bddZ0))));
    EXPECT_EQ(4ul, representative_complex_z.getNonZeroCount());
    EXPECT_EQ(1ul, representative_complex_z.getLeafCount());
    EXPECT_EQ(3ul, representative_complex_z.getNodeCount());
    EXPECT_TRUE(representative_complex_z == comparison_complex_z);

    // Abstract x, y, z
    storm::dd::Bdd<storm::dd::DdType::CUDD> representative_complex_xyz = complexAdd.maxAbstractRepresentative({x.first, y.first, z.first});
    storm::dd::Bdd<storm::dd::DdType::CUDD> comparison_complex_xyz = (bddX0 && (bddY0 && bddZ1));
    EXPECT_EQ(1ul, representative_complex_xyz.getNonZeroCount());
    EXPECT_EQ(1ul, representative_complex_xyz.getLeafCount());
    EXPECT_EQ(4ul, representative_complex_xyz.getNodeCount());
    EXPECT_TRUE(representative_complex_xyz == comparison_complex_xyz);
}

TEST(CuddDd, AddGetMetaVariableTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    ASSERT_NO_THROW(manager->addMetaVariable("x", 1, 9));
    EXPECT_EQ(2ul, manager->getNumberOfMetaVariables());

    STORM_SILENT_ASSERT_THROW(manager->addMetaVariable("x", 0, 3), storm::exceptions::InvalidArgumentException);

    ASSERT_NO_THROW(manager->addMetaVariable("y", 0, 3));
    EXPECT_EQ(4ul, manager->getNumberOfMetaVariables());

    EXPECT_TRUE(manager->hasMetaVariable("x'"));
    EXPECT_TRUE(manager->hasMetaVariable("y'"));

    std::set<std::string> metaVariableSet = {"x", "x'", "y", "y'"};
    EXPECT_EQ(metaVariableSet, manager->getAllMetaVariableNames());
}

TEST(CuddDd, EncodingTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x = manager->addMetaVariable("x", 1, 9);

    storm::dd::Bdd<storm::dd::DdType::CUDD> encoding;
    STORM_SILENT_ASSERT_THROW(encoding = manager->getEncoding(x.first, 0), storm::exceptions::InvalidArgumentException);
    STORM_SILENT_ASSERT_THROW(encoding = manager->getEncoding(x.first, 10), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(encoding = manager->getEncoding(x.first, 4));
    EXPECT_EQ(1ul, encoding.getNonZeroCount());

    // As a BDD, this DD has one only leaf, because there does not exist a 0-leaf, and (consequently) one node less
    // than the MTBDD.
    EXPECT_EQ(5ul, encoding.getNodeCount());
    EXPECT_EQ(1ul, encoding.getLeafCount());

    // As an MTBDD, the 0-leaf is there, so the count is actually 2 and the node count is 6.
    EXPECT_EQ(6ul, encoding.template toAdd<double>().getNodeCount());
    EXPECT_EQ(2ul, encoding.template toAdd<double>().getLeafCount());
}

TEST(CuddDd, RangeTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x;
    ASSERT_NO_THROW(x = manager->addMetaVariable("x", 1, 9));

    storm::dd::Bdd<storm::dd::DdType::CUDD> range;
    ASSERT_NO_THROW(range = manager->getRange(x.first));

    EXPECT_EQ(9ul, range.getNonZeroCount());
    EXPECT_EQ(1ul, range.getLeafCount());
    EXPECT_EQ(5ul, range.getNodeCount());
}

TEST(CuddDd, IdentityTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x = manager->addMetaVariable("x", 1, 9);

    storm::dd::Add<storm::dd::DdType::CUDD, double> identity;
    ASSERT_NO_THROW(identity = manager->getIdentity<double>(x.first));

    EXPECT_EQ(9ul, identity.getNonZeroCount());
    EXPECT_EQ(10ul, identity.getLeafCount());
    EXPECT_EQ(21ul, identity.getNodeCount());
}

TEST(CuddDd, OperatorTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x = manager->addMetaVariable("x", 1, 9);
    EXPECT_TRUE(manager->template getAddZero<double>() == manager->template getAddZero<double>());
    EXPECT_FALSE(manager->template getAddZero<double>() == manager->template getAddOne<double>());

    EXPECT_FALSE(manager->template getAddZero<double>() != manager->template getAddZero<double>());
    EXPECT_TRUE(manager->template getAddZero<double>() != manager->template getAddOne<double>());

    storm::dd::Add<storm::dd::DdType::CUDD, double> dd1 = manager->template getAddOne<double>();
    storm::dd::Add<storm::dd::DdType::CUDD, double> dd2 = manager->template getAddOne<double>();
    storm::dd::Add<storm::dd::DdType::CUDD, double> dd3 = dd1 + dd2;
    storm::dd::Bdd<storm::dd::DdType::CUDD> bdd;
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

    dd3 = dd1.equals(dd2).template toAdd<double>();
    EXPECT_EQ(1ul, dd3.getNonZeroCount());

    storm::dd::Add<storm::dd::DdType::CUDD, double> dd4 = dd1.notEquals(dd2).template toAdd<double>();
    EXPECT_TRUE(dd4.toBdd() == !dd3.toBdd());

    dd3 = dd1.less(dd2).template toAdd<double>();
    EXPECT_EQ(11ul, dd3.getNonZeroCount());

    dd3 = dd1.lessOrEqual(dd2).template toAdd<double>();
    EXPECT_EQ(12ul, dd3.getNonZeroCount());

    dd3 = dd1.greater(dd2).template toAdd<double>();
    EXPECT_EQ(4ul, dd3.getNonZeroCount());

    dd3 = dd1.greaterOrEqual(dd2).template toAdd<double>();
    EXPECT_EQ(5ul, dd3.getNonZeroCount());

    dd3 = manager->getEncoding(x.first, 2).ite(dd2, dd1);
    dd4 = dd3.less(dd2).template toAdd<double>();
    EXPECT_EQ(10ul, dd4.getNonZeroCount());

    dd4 = dd3.minimum(dd1);
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

TEST(CuddDd, AbstractionTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x = manager->addMetaVariable("x", 1, 9);
    storm::dd::Add<storm::dd::DdType::CUDD, double> dd1;
    storm::dd::Add<storm::dd::DdType::CUDD, double> dd2;
    storm::dd::Add<storm::dd::DdType::CUDD, double> dd3;

    dd1 = manager->template getIdentity<double>(x.first);
    dd2 = manager->template getConstant<double>(5);
    dd3 = dd1.equals(dd2).template toAdd<double>();
    storm::dd::Bdd<storm::dd::DdType::CUDD> dd3Bdd = dd3.toBdd();
    EXPECT_EQ(1ul, dd3Bdd.getNonZeroCount());
    STORM_SILENT_ASSERT_THROW(dd3Bdd = dd3Bdd.existsAbstract({x.second}), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(dd3Bdd = dd3Bdd.existsAbstract({x.first}));
    EXPECT_EQ(0ul, dd3Bdd.getNonZeroCount());
    EXPECT_EQ(1, dd3Bdd.template toAdd<double>().getMax());

    dd3 = dd1.equals(dd2).template toAdd<double>();
    dd3 *= manager->template getConstant<double>(3);
    EXPECT_EQ(1ul, dd3.getNonZeroCount());
    STORM_SILENT_ASSERT_THROW(dd3Bdd = dd3.toBdd().existsAbstract({x.second}), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(dd3Bdd = dd3.toBdd().existsAbstract({x.first}));
    EXPECT_TRUE(dd3Bdd.isOne());

    dd3 = dd1.equals(dd2).template toAdd<double>();
    dd3 *= manager->template getConstant<double>(3);
    STORM_SILENT_ASSERT_THROW(dd3 = dd3.sumAbstract({x.second}), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(dd3 = dd3.sumAbstract({x.first}));
    EXPECT_EQ(0ul, dd3.getNonZeroCount());
    EXPECT_EQ(3, dd3.getMax());

    dd3 = dd1.equals(dd2).template toAdd<double>();
    dd3 *= manager->template getConstant<double>(3);
    STORM_SILENT_ASSERT_THROW(dd3 = dd3.minAbstract({x.second}), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(dd3 = dd3.minAbstract({x.first}));
    EXPECT_EQ(0ul, dd3.getNonZeroCount());
    EXPECT_EQ(0, dd3.getMax());

    dd3 = dd1.equals(dd2).template toAdd<double>();
    dd3 *= manager->template getConstant<double>(3);
    STORM_SILENT_ASSERT_THROW(dd3 = dd3.maxAbstract({x.second}), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(dd3 = dd3.maxAbstract({x.first}));
    EXPECT_EQ(0ul, dd3.getNonZeroCount());
    EXPECT_EQ(3, dd3.getMax());
}

TEST(CuddDd, SwapTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());

    std::pair<storm::expressions::Variable, storm::expressions::Variable> x = manager->addMetaVariable("x", 1, 9);
    std::pair<storm::expressions::Variable, storm::expressions::Variable> z = manager->addMetaVariable("z", 2, 8);
    storm::dd::Add<storm::dd::DdType::CUDD, double> dd1;
    storm::dd::Add<storm::dd::DdType::CUDD, double> dd2;

    dd1 = manager->template getIdentity<double>(x.first);
    STORM_SILENT_ASSERT_THROW(dd1 = dd1.swapVariables({std::make_pair(x.first, z.first)}), storm::exceptions::InvalidArgumentException);
    ASSERT_NO_THROW(dd1 = dd1.swapVariables({std::make_pair(x.first, x.second)}));
    EXPECT_TRUE(dd1 == manager->template getIdentity<double>(x.second));
}

TEST(CuddDd, MultiplyMatrixTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x = manager->addMetaVariable("x", 1, 9);

    storm::dd::Add<storm::dd::DdType::CUDD, double> dd1 =
        manager->template getIdentity<double>(x.first).equals(manager->template getIdentity<double>(x.second)).template toAdd<double>();
    storm::dd::Add<storm::dd::DdType::CUDD, double> dd2 = manager->getRange(x.second).template toAdd<double>();
    storm::dd::Add<storm::dd::DdType::CUDD, double> dd3;
    dd1 *= manager->template getConstant<double>(2);

    ASSERT_NO_THROW(dd3 = dd1.multiplyMatrix(dd2, {x.second}));
    ASSERT_NO_THROW(dd3 = dd3.swapVariables({std::make_pair(x.first, x.second)}));
    EXPECT_TRUE(dd3 == dd2 * manager->template getConstant<double>(2));
}

TEST(CuddDd, MultiplyMatrixTest2) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x = manager->addMetaVariable("x", 0, 2);
    std::pair<storm::expressions::Variable, storm::expressions::Variable> b = manager->addMetaVariable("b", 0, 2);

    storm::dd::Add<storm::dd::DdType::CUDD, double> p = manager->getAddZero<double>();
    p += (manager->getEncoding(x.first, 2, true) && manager->getEncoding(b.first, 0, true)).template toAdd<double>();
    p += (manager->getEncoding(x.first, 0, true) && manager->getEncoding(b.first, 2, true)).template toAdd<double>();

    storm::dd::Add<storm::dd::DdType::CUDD, double> q = manager->getAddZero<double>();
    q += (manager->getEncoding(x.first, 0, true) && manager->getEncoding(x.second, 0, true)).template toAdd<double>() *
         manager->template getConstant<double>(0.3);
    q += (manager->getEncoding(x.first, 1, true) && manager->getEncoding(x.second, 0, true)).template toAdd<double>() *
         manager->template getConstant<double>(0.3);
    q += (manager->getEncoding(x.first, 0, true) && manager->getEncoding(x.second, 2, true)).template toAdd<double>() *
         manager->template getConstant<double>(0.7);
    q += (manager->getEncoding(x.first, 1, true) && manager->getEncoding(x.second, 2, true)).template toAdd<double>() *
         manager->template getConstant<double>(0.7);
    q +=
        (manager->getEncoding(x.first, 2, true) && manager->getEncoding(x.second, 0, true)).template toAdd<double>() * manager->template getConstant<double>(1);

    storm::dd::Add<storm::dd::DdType::CUDD, double> r = q.multiplyMatrix(p, {x.first});

    ASSERT_EQ(12ull, r.getNodeCount());
    ASSERT_EQ(4ull, r.getLeafCount());
    ASSERT_EQ(3ull, r.getNonZeroCount());
}

TEST(CuddDd, GetSetValueTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x = manager->addMetaVariable("x", 1, 9);

    storm::dd::Add<storm::dd::DdType::CUDD, double> dd1 = manager->template getAddOne<double>();
    ASSERT_NO_THROW(dd1.setValue(x.first, 4, 2));
    EXPECT_EQ(2ul, dd1.getLeafCount());

    std::map<storm::expressions::Variable, int_fast64_t> metaVariableToValueMap;
    metaVariableToValueMap.emplace(x.first, 1);
    EXPECT_EQ(1, dd1.getValue(metaVariableToValueMap));

    metaVariableToValueMap.clear();
    metaVariableToValueMap.emplace(x.first, 4);
    EXPECT_EQ(2, dd1.getValue(metaVariableToValueMap));
}

TEST(CuddDd, AddIteratorTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x = manager->addMetaVariable("x", 1, 9);
    std::pair<storm::expressions::Variable, storm::expressions::Variable> y = manager->addMetaVariable("y", 0, 3);

    storm::dd::Add<storm::dd::DdType::CUDD, double> dd;
    ASSERT_NO_THROW(dd = manager->getRange(x.first).template toAdd<double>());

    storm::dd::AddIterator<storm::dd::DdType::CUDD, double> it, ite;
    ASSERT_NO_THROW(it = dd.begin());
    ASSERT_NO_THROW(ite = dd.end());
    std::pair<storm::expressions::SimpleValuation, double> valuationValuePair;
    uint_fast64_t numberOfValuations = 0;
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

TEST(CuddDd, AddOddTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> a = manager->addMetaVariable("a");
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x = manager->addMetaVariable("x", 1, 9);

    storm::dd::Add<storm::dd::DdType::CUDD, double> dd = manager->template getIdentity<double>(x.first);
    storm::dd::Odd odd;
    ASSERT_NO_THROW(odd = dd.createOdd());
    EXPECT_EQ(9ul, odd.getTotalOffset());
    EXPECT_EQ(12ul, odd.getNodeCount());

    std::vector<double> ddAsVector;
    ASSERT_NO_THROW(ddAsVector = dd.toVector());
    EXPECT_EQ(9ul, ddAsVector.size());
    for (uint_fast64_t i = 0; i < ddAsVector.size(); ++i) {
        EXPECT_TRUE(i + 1 == ddAsVector[i]);
    }

    // Create a non-trivial matrix.
    dd = manager->template getIdentity<double>(x.first).equals(manager->template getIdentity<double>(x.second)).template toAdd<double>() *
         manager->getRange(x.first).template toAdd<double>();
    dd += manager->getEncoding(x.first, 1).template toAdd<double>() * manager->getRange(x.second).template toAdd<double>() +
          manager->getEncoding(x.second, 1).template toAdd<double>() * manager->getRange(x.first).template toAdd<double>();

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

    dd = manager->getRange(x.first).template toAdd<double>() * manager->getRange(x.second).template toAdd<double>() *
         manager->getEncoding(a.first, 0).ite(dd, dd + manager->template getConstant<double>(1));
    ASSERT_NO_THROW(matrix = dd.toMatrix({a.first}, rowOdd, columnOdd));
    EXPECT_EQ(18ul, matrix.getRowCount());
    EXPECT_EQ(9ul, matrix.getRowGroupCount());
    EXPECT_EQ(9ul, matrix.getColumnCount());
    EXPECT_EQ(106ul, matrix.getNonzeroEntryCount());
}

TEST(CuddDd, BddOddTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> a = manager->addMetaVariable("a");
    std::pair<storm::expressions::Variable, storm::expressions::Variable> x = manager->addMetaVariable("x", 1, 9);

    storm::dd::Add<storm::dd::DdType::CUDD, double> dd = manager->template getIdentity<double>(x.first);
    storm::dd::Bdd<storm::dd::DdType::CUDD> bdd = dd.notZero();
    storm::dd::Odd odd;
    ASSERT_NO_THROW(odd = bdd.createOdd());
    EXPECT_EQ(9ul, odd.getTotalOffset());
    EXPECT_EQ(5ul, odd.getNodeCount());

    std::vector<double> ddAsVector;
    ASSERT_NO_THROW(ddAsVector = dd.toVector());
    EXPECT_EQ(9ul, ddAsVector.size());
    for (uint_fast64_t i = 0; i < ddAsVector.size(); ++i) {
        EXPECT_TRUE(i + 1 == ddAsVector[i]);
    }

    storm::dd::Add<storm::dd::DdType::CUDD, double> vectorAdd =
        storm::dd::Add<storm::dd::DdType::CUDD, double>::fromVector(*manager, ddAsVector, odd, {x.first});

    // Create a non-trivial matrix.
    dd = manager->template getIdentity<double>(x.first).equals(manager->template getIdentity<double>(x.second)).template toAdd<double>() *
         manager->getRange(x.first).template toAdd<double>();
    dd += manager->getEncoding(x.first, 1).template toAdd<double>() * manager->getRange(x.second).template toAdd<double>() +
          manager->getEncoding(x.second, 1).template toAdd<double>() * manager->getRange(x.first).template toAdd<double>();

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

    dd = manager->getRange(x.first).template toAdd<double>() * manager->getRange(x.second).template toAdd<double>() *
         manager->getEncoding(a.first, 0).ite(dd, dd + manager->template getConstant<double>(1));
    ASSERT_NO_THROW(matrix = dd.toMatrix({a.first}, rowOdd, columnOdd));
    EXPECT_EQ(18ul, matrix.getRowCount());
    EXPECT_EQ(9ul, matrix.getRowGroupCount());
    EXPECT_EQ(9ul, matrix.getColumnCount());
    EXPECT_EQ(106ul, matrix.getNonzeroEntryCount());
}

TEST(CuddDd, BddToExpressionTest) {
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> ddManager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> a = ddManager->addMetaVariable("a");
    std::pair<storm::expressions::Variable, storm::expressions::Variable> b = ddManager->addMetaVariable("b");

    storm::dd::Bdd<storm::dd::DdType::CUDD> bdd = ddManager->getBddOne();
    bdd &= ddManager->getEncoding(a.first, 1);
    bdd |= ddManager->getEncoding(b.first, 0);

    std::shared_ptr<storm::expressions::ExpressionManager> manager = std::make_shared<storm::expressions::ExpressionManager>();
    storm::expressions::Variable c = manager->declareBooleanVariable("c");
    storm::expressions::Variable d = manager->declareBooleanVariable("d");

    auto result = bdd.toExpression(*manager);
}
