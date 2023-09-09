#include "storm-config.h"
#include "test/storm_gtest.h"

#include "storm/exceptions/InvalidAccessException.h"
#include "storm/solver/GlpkLpSolver.h"
#include "storm/solver/GurobiLpSolver.h"
#include "storm/solver/LpSolver.h"
#include "storm/solver/OptimizationDirection.h"
#include "storm/storage/expressions/Expression.h"
#include "storm/storage/expressions/Variable.h"
#include "storm/utility/solver.h"
#include "storm/utility/vector.h"
namespace {

class DefaultEnvironment {
   public:
    typedef double ValueType;
    static const bool isExact = false;
    static const storm::solver::LpSolverTypeSelection solverSelection = storm::solver::LpSolverTypeSelection::FROMSETTINGS;
    static const bool IntegerSupport = true;
    static const bool IncrementalSupport = true;
    static const bool strictRelationSupport = true;
};

class GlpkEnvironment {
   public:
    typedef double ValueType;
    static const bool isExact = false;
    static const storm::solver::LpSolverTypeSelection solverSelection = storm::solver::LpSolverTypeSelection::Glpk;
    static const bool IntegerSupport = true;
    static const bool IncrementalSupport = true;
    static const bool strictRelationSupport = true;
};

class GurobiEnvironment {
   public:
    typedef double ValueType;
    static const bool isExact = false;
    static const storm::solver::LpSolverTypeSelection solverSelection = storm::solver::LpSolverTypeSelection::Gurobi;
    static const bool IntegerSupport = true;
    static const bool IncrementalSupport = true;
    static const bool strictRelationSupport = true;
};

class Z3Environment {
   public:
    typedef storm::RationalNumber ValueType;
    static const bool isExact = true;
    static const storm::solver::LpSolverTypeSelection solverSelection = storm::solver::LpSolverTypeSelection::Z3;
    static const bool IntegerSupport = true;
    static const bool IncrementalSupport = true;
    static const bool strictRelationSupport = true;
};

class SoplexEnvironment {
   public:
    typedef double ValueType;
    static const bool isExact = false;
    static const storm::solver::LpSolverTypeSelection solverSelection = storm::solver::LpSolverTypeSelection::Soplex;
    static const bool IntegerSupport = false;
    static const bool IncrementalSupport = false;
    static const bool strictRelationSupport = false;
};

class SoplexExactEnvironment {
   public:
    typedef storm::RationalNumber ValueType;
    static const bool isExact = true;
    static const storm::solver::LpSolverTypeSelection solverSelection = storm::solver::LpSolverTypeSelection::Soplex;
    static const bool IntegerSupport = false;
    static const bool IncrementalSupport = false;
    static const bool strictRelationSupport = false;
};

template<typename TestType>
class LpSolverTest : public ::testing::Test {
   public:
    typedef typename TestType::ValueType ValueType;

    storm::solver::LpSolverTypeSelection solverSelection() const {
        return TestType::solverSelection;
    }

    std::unique_ptr<storm::utility::solver::LpSolverFactory<ValueType>> factory() const {
        return storm::utility::solver::getLpSolverFactory<ValueType>(solverSelection());
    }

    ValueType parseNumber(std::string const& input) const {
        return storm::utility::convertNumber<ValueType>(input);
    }

    ValueType precision() const {
        return TestType::isExact ? parseNumber("0") : parseNumber("1e-15");
    }

    bool supportsInteger() const {
        return TestType::IntegerSupport;
    }

    bool supportsIncremental() const {
        return TestType::IncrementalSupport;
    }

    bool supportsStrictRelation() const {
        return TestType::strictRelationSupport;
    }
};

typedef ::testing::Types<DefaultEnvironment
#ifdef STORM_HAVE_GLPK
                         ,
                         GlpkEnvironment
#endif
#ifdef STORM_HAVE_GUROBI
                         ,
                         GurobiEnvironment
#endif
#ifdef STORM_HAVE_Z3_OPTIMIZE
                         ,
                         Z3Environment
#endif
#ifdef STORM_HAVE_SOPLEX
                         ,
                         SoplexEnvironment, SoplexExactEnvironment
#endif
                         >
    TestingTypes;

TYPED_TEST_SUITE(LpSolverTest, TestingTypes, );

TYPED_TEST(LpSolverTest, LPOptimizeMax) {
    typedef typename TestFixture::ValueType ValueType;
    auto solver = this->factory()->create("");
    solver->setOptimizationDirection(storm::OptimizationDirection::Maximize);
    storm::expressions::Variable x;
    storm::expressions::Variable y;
    storm::expressions::Variable z;
    ASSERT_NO_THROW(x = solver->addBoundedContinuousVariable("x", 0, 1, -1));
    ASSERT_NO_THROW(y = solver->addLowerBoundedContinuousVariable("y", 0, 2));
    ASSERT_NO_THROW(z = solver->addLowerBoundedContinuousVariable("z", 0, 1));
    ASSERT_NO_THROW(solver->update());

    ASSERT_NO_THROW(solver->addConstraint("", x + y + z <= solver->getConstant(12)));
    ASSERT_NO_THROW(solver->addConstraint("", solver->getConstant(this->parseNumber("1/2")) * y + z - x == solver->getConstant(5)));
    ASSERT_NO_THROW(solver->addConstraint("", y - x <= solver->getConstant(this->parseNumber("11/2"))));
    ASSERT_NO_THROW(solver->update());

    ASSERT_NO_THROW(solver->optimize());
    ASSERT_TRUE(solver->isOptimal());
    ASSERT_FALSE(solver->isUnbounded());
    ASSERT_FALSE(solver->isInfeasible());
    EXPECT_NEAR(this->parseNumber("1"), solver->getContinuousValue(x), this->precision());
    EXPECT_NEAR(this->parseNumber("13/2"), solver->getContinuousValue(y), this->precision());
    EXPECT_NEAR(this->parseNumber("11/4"), solver->getContinuousValue(z), this->precision());
    EXPECT_NEAR(this->parseNumber("59/4"), solver->getObjectiveValue(), this->precision());
}

TYPED_TEST(LpSolverTest, LPOptimizeMaxRaw) {
    typedef typename TestFixture::ValueType ValueType;
    auto solver = this->factory()->createRaw("");
    solver->setOptimizationDirection(storm::OptimizationDirection::Maximize);
    ASSERT_EQ(0u, solver->addBoundedContinuousVariable("x", 0, 1, -1));
    ASSERT_EQ(1u, solver->addLowerBoundedContinuousVariable("y", 0, 2));
    ASSERT_EQ(2u, solver->addLowerBoundedContinuousVariable("z", 0, 1));
    ASSERT_NO_THROW(solver->update());

    // x + y + z <= 12
    storm::solver::RawLpConstraint<ValueType> constraint1(storm::expressions::RelationType::LessOrEqual, this->parseNumber("12"), 3);
    constraint1.addToLhs(0, this->parseNumber("1"));
    constraint1.addToLhs(1, this->parseNumber("1"));
    constraint1.addToLhs(2, this->parseNumber("1"));
    ASSERT_NO_THROW(solver->addConstraint("", constraint1));
    // -x + 1/2 * y + z == 5
    storm::solver::RawLpConstraint<ValueType> constraint2(storm::expressions::RelationType::Equal, this->parseNumber("5"), 3);
    constraint2.addToLhs(0, -this->parseNumber("1"));
    constraint2.addToLhs(1, this->parseNumber("1/2"));
    constraint2.addToLhs(2, this->parseNumber("1"));
    ASSERT_NO_THROW(solver->addConstraint("", constraint2));
    // -x + y <= 11/2
    storm::solver::RawLpConstraint<ValueType> constraint3(storm::expressions::RelationType::LessOrEqual, this->parseNumber("11/2"), 2);
    constraint3.addToLhs(0, -this->parseNumber("1"));
    constraint3.addToLhs(1, this->parseNumber("1"));
    ASSERT_NO_THROW(solver->addConstraint("", constraint3));
    ASSERT_NO_THROW(solver->update());

    ASSERT_NO_THROW(solver->optimize());
    ASSERT_TRUE(solver->isOptimal());
    ASSERT_FALSE(solver->isUnbounded());
    ASSERT_FALSE(solver->isInfeasible());
    EXPECT_NEAR(this->parseNumber("1"), solver->getContinuousValue(0), this->precision());
    EXPECT_NEAR(this->parseNumber("13/2"), solver->getContinuousValue(1), this->precision());
    EXPECT_NEAR(this->parseNumber("11/4"), solver->getContinuousValue(2), this->precision());
    EXPECT_NEAR(this->parseNumber("59/4"), solver->getObjectiveValue(), this->precision());
}

TYPED_TEST(LpSolverTest, LPOptimizeMin) {
    typedef typename TestFixture::ValueType ValueType;
    auto solver = this->factory()->create("");
    solver->setOptimizationDirection(storm::OptimizationDirection::Minimize);
    storm::expressions::Variable x;
    storm::expressions::Variable y;
    storm::expressions::Variable z;
    ASSERT_NO_THROW(x = solver->addBoundedContinuousVariable("x", 0, 1, -1));
    ASSERT_NO_THROW(y = solver->addLowerBoundedContinuousVariable("y", 0, 2));
    ASSERT_NO_THROW(z = solver->addBoundedContinuousVariable("z", 1, this->parseNumber("57/10"), -1));
    ASSERT_NO_THROW(solver->update());

    ASSERT_NO_THROW(solver->addConstraint("", x + y + z <= solver->getConstant(12)));
    ASSERT_NO_THROW(solver->addConstraint("", solver->getConstant(this->parseNumber("1/2")) * y + z - x <= solver->getConstant(5)));
    ASSERT_NO_THROW(solver->addConstraint("", y - x <= solver->getConstant(this->parseNumber("11/2"))));
    ASSERT_NO_THROW(solver->update());

    ASSERT_NO_THROW(solver->optimize());
    ASSERT_TRUE(solver->isOptimal());
    ASSERT_FALSE(solver->isUnbounded());
    ASSERT_FALSE(solver->isInfeasible());

    EXPECT_NEAR(this->parseNumber("1"), solver->getContinuousValue(x), this->precision());
    EXPECT_NEAR(this->parseNumber("0"), solver->getContinuousValue(y), this->precision());
    EXPECT_NEAR(this->parseNumber("57/10"), solver->getContinuousValue(z), this->precision());
    EXPECT_NEAR(this->parseNumber("-67/10"), solver->getObjectiveValue(), this->precision());
}

TYPED_TEST(LpSolverTest, LPOptimizeMinRaw) {
    typedef typename TestFixture::ValueType ValueType;
    auto solver = this->factory()->createRaw("");
    solver->setOptimizationDirection(storm::OptimizationDirection::Minimize);

    ASSERT_EQ(0u, solver->addBoundedContinuousVariable("x", 0, 1, -1));
    ASSERT_EQ(1u, solver->addLowerBoundedContinuousVariable("y", 0, 2));
    ASSERT_EQ(2u, solver->addBoundedContinuousVariable("z", 1, this->parseNumber("57/10"), -1));
    ASSERT_NO_THROW(solver->update());

    // x + y + z <= 12
    storm::solver::RawLpConstraint<ValueType> constraint1(storm::expressions::RelationType::LessOrEqual, this->parseNumber("12"), 3);
    constraint1.addToLhs(0, this->parseNumber("1"));
    constraint1.addToLhs(1, this->parseNumber("1"));
    constraint1.addToLhs(2, this->parseNumber("1"));
    ASSERT_NO_THROW(solver->addConstraint("", constraint1));
    // -x + 1/2 * y + z <= 5
    storm::solver::RawLpConstraint<ValueType> constraint2(storm::expressions::RelationType::LessOrEqual, this->parseNumber("5"), 3);
    constraint2.addToLhs(0, -this->parseNumber("1"));
    constraint2.addToLhs(1, this->parseNumber("1/2"));
    constraint2.addToLhs(2, this->parseNumber("1"));
    ASSERT_NO_THROW(solver->addConstraint("", constraint2));
    // -x + y <= 11/2
    storm::solver::RawLpConstraint<ValueType> constraint3(storm::expressions::RelationType::LessOrEqual, this->parseNumber("11/2"), 2);
    constraint3.addToLhs(0, -this->parseNumber("1"));
    constraint3.addToLhs(1, this->parseNumber("1"));
    ASSERT_NO_THROW(solver->addConstraint("", constraint3));
    ASSERT_NO_THROW(solver->update());

    ASSERT_NO_THROW(solver->optimize());
    ASSERT_TRUE(solver->isOptimal());
    ASSERT_FALSE(solver->isUnbounded());
    ASSERT_FALSE(solver->isInfeasible());

    EXPECT_NEAR(this->parseNumber("1"), solver->getContinuousValue(0), this->precision());
    EXPECT_NEAR(this->parseNumber("0"), solver->getContinuousValue(1), this->precision());
    EXPECT_NEAR(this->parseNumber("57/10"), solver->getContinuousValue(2), this->precision());
    EXPECT_NEAR(this->parseNumber("-67/10"), solver->getObjectiveValue(), this->precision());
}

TYPED_TEST(LpSolverTest, MILPOptimizeMax) {
    if (!this->supportsInteger()) {
        GTEST_SKIP();
    }
    typedef typename TestFixture::ValueType ValueType;
    auto solver = this->factory()->create("");
    solver->setOptimizationDirection(storm::OptimizationDirection::Maximize);
    storm::expressions::Variable x;
    storm::expressions::Variable y;
    storm::expressions::Variable z;
    ASSERT_NO_THROW(x = solver->addBinaryVariable("x", -1));
    ASSERT_NO_THROW(y = solver->addLowerBoundedIntegerVariable("y", 0, 2));
    ASSERT_NO_THROW(z = solver->addLowerBoundedContinuousVariable("z", 0, 1));
    ASSERT_NO_THROW(solver->update());

    ASSERT_NO_THROW(solver->addConstraint("", x + y + z <= solver->getConstant(12)));
    ASSERT_NO_THROW(solver->addConstraint("", solver->getConstant(this->parseNumber("1/2")) * y + z - x == solver->getConstant(5)));
    ASSERT_NO_THROW(solver->addConstraint("", y - x <= solver->getConstant(this->parseNumber("11/2"))));
    ASSERT_NO_THROW(solver->update());
    ASSERT_NO_THROW(solver->optimize());
    ASSERT_TRUE(solver->isOptimal());
    ASSERT_FALSE(solver->isUnbounded());
    ASSERT_FALSE(solver->isInfeasible());

    EXPECT_TRUE(solver->getBinaryValue(x));
    EXPECT_EQ(6, solver->getIntegerValue(y));
    EXPECT_NEAR(this->parseNumber("3"), solver->getContinuousValue(z), this->precision());
    EXPECT_NEAR(this->parseNumber("14"), solver->getObjectiveValue(), this->precision());
}

TYPED_TEST(LpSolverTest, MILPOptimizeMaxRaw) {
    if (!this->supportsInteger()) {
        GTEST_SKIP();
    }
    typedef typename TestFixture::ValueType ValueType;
    auto solver = this->factory()->createRaw("");
    solver->setOptimizationDirection(storm::OptimizationDirection::Maximize);
    storm::expressions::Variable x;
    storm::expressions::Variable y;
    storm::expressions::Variable z;
    ASSERT_EQ(0u, solver->addBinaryVariable("x", -1));
    ASSERT_EQ(1u, solver->addLowerBoundedIntegerVariable("y", 0, 2));
    ASSERT_EQ(2u, solver->addLowerBoundedContinuousVariable("z", 0, 1));
    ASSERT_NO_THROW(solver->update());

    // x + y + z <= 12
    storm::solver::RawLpConstraint<ValueType> constraint1(storm::expressions::RelationType::LessOrEqual, this->parseNumber("12"), 3);
    constraint1.addToLhs(0, this->parseNumber("1"));
    constraint1.addToLhs(1, this->parseNumber("1"));
    constraint1.addToLhs(2, this->parseNumber("1"));
    ASSERT_NO_THROW(solver->addConstraint("", constraint1));
    // -x + 1/2 * y + z == 5
    storm::solver::RawLpConstraint<ValueType> constraint2(storm::expressions::RelationType::Equal, this->parseNumber("5"), 3);
    constraint2.addToLhs(0, -this->parseNumber("1"));
    constraint2.addToLhs(1, this->parseNumber("1/2"));
    constraint2.addToLhs(2, this->parseNumber("1"));
    ASSERT_NO_THROW(solver->addConstraint("", constraint2));
    // -x + y <= 11/2
    storm::solver::RawLpConstraint<ValueType> constraint3(storm::expressions::RelationType::LessOrEqual, this->parseNumber("11/2"), 2);
    constraint3.addToLhs(0, -this->parseNumber("1"));
    constraint3.addToLhs(1, this->parseNumber("1"));
    ASSERT_NO_THROW(solver->addConstraint("", constraint3));
    ASSERT_NO_THROW(solver->update());

    ASSERT_NO_THROW(solver->optimize());
    ASSERT_TRUE(solver->isOptimal());
    ASSERT_FALSE(solver->isUnbounded());
    ASSERT_FALSE(solver->isInfeasible());
    EXPECT_TRUE(solver->getBinaryValue(0));
    EXPECT_EQ(6, solver->getIntegerValue(1));
    EXPECT_NEAR(this->parseNumber("3"), solver->getContinuousValue(2), this->precision());
    EXPECT_NEAR(this->parseNumber("14"), solver->getObjectiveValue(), this->precision());
}

TYPED_TEST(LpSolverTest, MILPOptimizeMin) {
    if (!this->supportsInteger()) {
        GTEST_SKIP();
    }
    typedef typename TestFixture::ValueType ValueType;
    auto solver = this->factory()->create("");
    solver->setOptimizationDirection(storm::OptimizationDirection::Minimize);
    storm::expressions::Variable x;
    storm::expressions::Variable y;
    storm::expressions::Variable z;
    ASSERT_NO_THROW(x = solver->addBinaryVariable("x", -1));
    ASSERT_NO_THROW(y = solver->addLowerBoundedIntegerVariable("y", 0, 2));
    ASSERT_NO_THROW(z = solver->addBoundedContinuousVariable("z", 0, 5, -1));
    ASSERT_NO_THROW(solver->update());

    ASSERT_NO_THROW(solver->addConstraint("", x + y + z <= solver->getConstant(12)));
    ASSERT_NO_THROW(solver->addConstraint("", solver->getConstant(this->parseNumber("1/2")) * y + z - x <= solver->getConstant(5)));
    ASSERT_NO_THROW(solver->addConstraint("", y - x <= solver->getConstant(this->parseNumber("11/2"))));
    ASSERT_NO_THROW(solver->update());

#ifdef STORM_HAVE_Z3_OPTIMIZE
    if (this->solverSelection() == storm::solver::LpSolverTypeSelection::Z3 && storm::test::z3AtLeastVersion(4, 8, 8)) {
        // TODO: z3 v4.8.8 is known to be broken here. Check if this is fixed in future versions >4.8.8
        GTEST_SKIP() << "Test disabled since it triggers a bug in the installed version of z3.";
    }
#endif

    ASSERT_NO_THROW(solver->optimize());
    ASSERT_TRUE(solver->isOptimal());
    ASSERT_FALSE(solver->isUnbounded());
    ASSERT_FALSE(solver->isInfeasible());

    EXPECT_TRUE(solver->getBinaryValue(x));
    EXPECT_EQ(0, solver->getIntegerValue(y));
    EXPECT_NEAR(this->parseNumber("5"), solver->getContinuousValue(z), this->precision());
    EXPECT_NEAR(this->parseNumber("-6"), solver->getObjectiveValue(), this->precision());
}

TYPED_TEST(LpSolverTest, LPInfeasible) {
    typedef typename TestFixture::ValueType ValueType;
    auto solver = this->factory()->create("");
    solver->setOptimizationDirection(storm::OptimizationDirection::Maximize);
    storm::expressions::Variable x;
    storm::expressions::Variable y;
    storm::expressions::Variable z;
    ASSERT_NO_THROW(x = solver->addBoundedContinuousVariable("x", 0, 1, -1));
    ASSERT_NO_THROW(y = solver->addLowerBoundedContinuousVariable("y", 0, 2));
    ASSERT_NO_THROW(z = solver->addLowerBoundedContinuousVariable("z", 0, 1));
    ASSERT_NO_THROW(solver->update());

    ASSERT_NO_THROW(solver->addConstraint("", x + y + z <= solver->getConstant(12)));
    ASSERT_NO_THROW(solver->addConstraint("", solver->getConstant(this->parseNumber("1/2")) * y + z - x == solver->getConstant(5)));
    ASSERT_NO_THROW(solver->addConstraint("", y - x <= solver->getConstant(this->parseNumber("11/2"))));
    if (this->supportsStrictRelation()) {
        ASSERT_NO_THROW(solver->addConstraint("", y > solver->getConstant((this->parseNumber("7")))));
    } else {
        ASSERT_NO_THROW(solver->addConstraint("", y >= solver->getConstant(this->parseNumber("7") + this->precision())));
    }
    ASSERT_NO_THROW(solver->update());

    ASSERT_NO_THROW(solver->optimize());
    ASSERT_FALSE(solver->isOptimal());
    ASSERT_FALSE(solver->isUnbounded());
    ASSERT_TRUE(solver->isInfeasible());
    STORM_SILENT_ASSERT_THROW(solver->getContinuousValue(x), storm::exceptions::InvalidAccessException);
    STORM_SILENT_ASSERT_THROW(solver->getContinuousValue(y), storm::exceptions::InvalidAccessException);
    STORM_SILENT_ASSERT_THROW(solver->getContinuousValue(z), storm::exceptions::InvalidAccessException);
    STORM_SILENT_ASSERT_THROW(solver->getObjectiveValue(), storm::exceptions::InvalidAccessException);
}

TYPED_TEST(LpSolverTest, MILPInfeasible) {
    if (!this->supportsInteger()) {
        GTEST_SKIP();
    }
    typedef typename TestFixture::ValueType ValueType;
    auto solver = this->factory()->create("");
    solver->setOptimizationDirection(storm::OptimizationDirection::Maximize);
    storm::expressions::Variable x;
    storm::expressions::Variable y;
    storm::expressions::Variable z;
    ASSERT_NO_THROW(x = solver->addBinaryVariable("x", -1));
    ASSERT_NO_THROW(y = solver->addLowerBoundedContinuousVariable("y", 0, 2));
    ASSERT_NO_THROW(z = solver->addLowerBoundedContinuousVariable("z", 0, 1));
    ASSERT_NO_THROW(solver->update());

    ASSERT_NO_THROW(solver->addConstraint("", x + y + z <= solver->getConstant(12)));
    ASSERT_NO_THROW(solver->addConstraint("", solver->getConstant(this->parseNumber("1/2")) * y + z - x == solver->getConstant(5)));
    ASSERT_NO_THROW(solver->addConstraint("", y - x <= solver->getConstant(this->parseNumber("11/2"))));
    if (this->supportsStrictRelation()) {
        ASSERT_NO_THROW(solver->addConstraint("", y > solver->getConstant((this->parseNumber("7")))));
    } else {
        ASSERT_NO_THROW(solver->addConstraint("", y >= solver->getConstant(this->parseNumber("7") + this->precision())));
    }
    ASSERT_NO_THROW(solver->update());

    ASSERT_NO_THROW(solver->optimize());
    ASSERT_FALSE(solver->isOptimal());
    ASSERT_FALSE(solver->isUnbounded());
    ASSERT_TRUE(solver->isInfeasible());
    STORM_SILENT_ASSERT_THROW(solver->getBinaryValue(x), storm::exceptions::InvalidAccessException);
    STORM_SILENT_ASSERT_THROW(solver->getIntegerValue(y), storm::exceptions::InvalidAccessException);
    STORM_SILENT_ASSERT_THROW(solver->getContinuousValue(z), storm::exceptions::InvalidAccessException);
    STORM_SILENT_ASSERT_THROW(solver->getObjectiveValue(), storm::exceptions::InvalidAccessException);
}

TYPED_TEST(LpSolverTest, LPUnbounded) {
    typedef typename TestFixture::ValueType ValueType;
    auto solver = this->factory()->create("");
    solver->setOptimizationDirection(storm::OptimizationDirection::Maximize);
    storm::expressions::Variable x;
    storm::expressions::Variable y;
    storm::expressions::Variable z;
    ASSERT_NO_THROW(x = solver->addBoundedContinuousVariable("x", 0, 1, -1));
    ASSERT_NO_THROW(y = solver->addLowerBoundedContinuousVariable("y", 0, 2));
    ASSERT_NO_THROW(z = solver->addLowerBoundedContinuousVariable("z", 0, 1));
    ASSERT_NO_THROW(solver->update());

    ASSERT_NO_THROW(solver->addConstraint("", x + y - z <= solver->getConstant(12)));
    ASSERT_NO_THROW(solver->addConstraint("", y - x <= solver->getConstant(this->parseNumber("11/2"))));
    ASSERT_NO_THROW(solver->update());

    ASSERT_NO_THROW(solver->optimize());
    ASSERT_FALSE(solver->isOptimal());
    ASSERT_TRUE(solver->isUnbounded());
    ASSERT_FALSE(solver->isInfeasible());
    STORM_SILENT_ASSERT_THROW(solver->getContinuousValue(x), storm::exceptions::InvalidAccessException);
    STORM_SILENT_ASSERT_THROW(solver->getContinuousValue(y), storm::exceptions::InvalidAccessException);
    STORM_SILENT_ASSERT_THROW(solver->getContinuousValue(z), storm::exceptions::InvalidAccessException);
    STORM_SILENT_ASSERT_THROW(solver->getObjectiveValue(), storm::exceptions::InvalidAccessException);
}

TYPED_TEST(LpSolverTest, MILPUnbounded) {
    if (!this->supportsInteger()) {
        GTEST_SKIP();
    }
    typedef typename TestFixture::ValueType ValueType;
    auto solver = this->factory()->create("");
    solver->setOptimizationDirection(storm::OptimizationDirection::Maximize);
    storm::expressions::Variable x;
    storm::expressions::Variable y;
    storm::expressions::Variable z;
    ASSERT_NO_THROW(x = solver->addBinaryVariable("x", -1));
    ASSERT_NO_THROW(y = solver->addLowerBoundedContinuousVariable("y", 0, 2));
    ASSERT_NO_THROW(z = solver->addLowerBoundedContinuousVariable("z", 0, 1));
    ASSERT_NO_THROW(solver->update());

    ASSERT_NO_THROW(solver->addConstraint("", x + y - z <= solver->getConstant(12)));
    ASSERT_NO_THROW(solver->addConstraint("", y - x <= solver->getConstant(this->parseNumber("11/2"))));
    ASSERT_NO_THROW(solver->update());

    ASSERT_NO_THROW(solver->optimize());
    ASSERT_FALSE(solver->isOptimal());
    ASSERT_TRUE(solver->isUnbounded());
    ASSERT_FALSE(solver->isInfeasible());
    STORM_SILENT_ASSERT_THROW(solver->getBinaryValue(x), storm::exceptions::InvalidAccessException);
    STORM_SILENT_ASSERT_THROW(solver->getIntegerValue(y), storm::exceptions::InvalidAccessException);
    STORM_SILENT_ASSERT_THROW(solver->getContinuousValue(z), storm::exceptions::InvalidAccessException);
    STORM_SILENT_ASSERT_THROW(solver->getObjectiveValue(), storm::exceptions::InvalidAccessException);
}

TYPED_TEST(LpSolverTest, Incremental) {
    if (!this->supportsIncremental()) {
        GTEST_SKIP();
    }
    typedef typename TestFixture::ValueType ValueType;
    auto solver = this->factory()->create("");
    solver->setOptimizationDirection(storm::OptimizationDirection::Maximize);
    storm::expressions::Variable x, y, z;
    ASSERT_NO_THROW(x = solver->addUnboundedContinuousVariable("x", 1));

    solver->push();
    ASSERT_NO_THROW(solver->addConstraint("", x <= solver->getConstant(12)));
    ASSERT_NO_THROW(solver->optimize());
    // max x s.t. x<=12
    ASSERT_TRUE(solver->isOptimal());
    EXPECT_NEAR(this->parseNumber("12"), solver->getContinuousValue(x), this->precision());

    solver->push();
    ASSERT_NO_THROW(y = solver->addUnboundedContinuousVariable("y"));
    ASSERT_NO_THROW(solver->addConstraint("", y <= solver->getConstant(6)));
    ASSERT_NO_THROW(solver->addConstraint("", x <= y));
    // max x s.t. x<=12 and y <= 6 and x <= y
    ASSERT_NO_THROW(solver->optimize());
    ASSERT_TRUE(solver->isOptimal());
    EXPECT_NEAR(this->parseNumber("6"), solver->getContinuousValue(x), this->precision());
    EXPECT_NEAR(this->parseNumber("6"), solver->getContinuousValue(y), this->precision());
    solver->pop();
    ASSERT_NO_THROW(solver->optimize());
    // max x s.t. x<=12
    ASSERT_TRUE(solver->isOptimal());
    EXPECT_NEAR(this->parseNumber("12"), solver->getContinuousValue(x), this->precision());

#ifdef STORM_HAVE_Z3_OPTIMIZE
    if (this->solverSelection() == storm::solver::LpSolverTypeSelection::Z3 && !storm::test::z3AtLeastVersion(4, 8, 5)) {
        GTEST_SKIP() << "Test disabled since it triggers a bug in the installed version of z3.";
    }
#endif

    solver->push();
    ASSERT_NO_THROW(y = solver->addUnboundedContinuousVariable("y", 10));
    ASSERT_NO_THROW(solver->addConstraint("", y <= solver->getConstant(20)));
    ASSERT_NO_THROW(solver->addConstraint("", y <= -x));
    // max x+10y s.t. x<=12 and y<=20 and y<=-x
    ASSERT_NO_THROW(solver->optimize());
    ASSERT_TRUE(solver->isOptimal());
    EXPECT_NEAR(this->parseNumber("-20"), solver->getContinuousValue(x), this->precision());
    EXPECT_NEAR(this->parseNumber("20"), solver->getContinuousValue(y), this->precision());

    solver->pop();
    ASSERT_NO_THROW(solver->optimize());
    // max x s.t. x<=12
    ASSERT_TRUE(solver->isOptimal());
    EXPECT_NEAR(this->parseNumber("12"), solver->getContinuousValue(x), this->precision());

    solver->push();
    ASSERT_NO_THROW(z = solver->addUnboundedIntegerVariable("z"));
    ASSERT_NO_THROW(solver->addConstraint("", z <= solver->getConstant(6)));
    ASSERT_NO_THROW(solver->addConstraint("", x <= z));
    ASSERT_NO_THROW(solver->optimize());
    // max x s.t. x<=12 and z <= 6 and x <= z
    ASSERT_TRUE(solver->isOptimal());
    EXPECT_NEAR(this->parseNumber("6"), solver->getContinuousValue(x), this->precision());
    EXPECT_EQ(6, solver->getIntegerValue(z));

    solver->pop();
    ASSERT_NO_THROW(solver->optimize());
    // max x s.t. x<=12
    ASSERT_TRUE(solver->isOptimal());
    EXPECT_NEAR(this->parseNumber("12"), solver->getContinuousValue(x), this->precision());

    solver->pop();
    // max x s.t. true
    ASSERT_NO_THROW(solver->optimize());
    ASSERT_FALSE(solver->isOptimal());
    ASSERT_TRUE(solver->isUnbounded());
    ASSERT_FALSE(solver->isInfeasible());
}

}  // namespace