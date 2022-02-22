#include "storm-config.h"
#include "test/storm_gtest.h"

#include "test/storm_gtest.h"

#include "storm/environment/solver/MinMaxSolverEnvironment.h"
#include "storm/environment/solver/NativeSolverEnvironment.h"
#include "storm/environment/solver/TopologicalSolverEnvironment.h"
#include "storm/solver/MinMaxLinearEquationSolver.h"
#include "storm/solver/SolverSelectionOptions.h"
#include "storm/storage/SparseMatrix.h"

namespace {

class DoubleTopologicalViEnvironment {
   public:
    typedef double ValueType;
    static const bool isExact = false;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().minMax().setMethod(storm::solver::MinMaxMethod::Topological);
        env.solver().topological().setUnderlyingMinMaxMethod(storm::solver::MinMaxMethod::ValueIteration);
        env.solver().minMax().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-8));
        return env;
    }
};


template<typename TestType>
class FixedChoiceTopoligicalMinMaxLinearEquationSolverTest : public ::testing::Test {
   public:
    typedef typename TestType::ValueType ValueType;
    FixedChoiceTopoligicalMinMaxLinearEquationSolverTest() : _environment(TestType::createEnvironment()) {}
    storm::Environment const& env() const {
        return _environment;
    }
    ValueType precision() const {
        return TestType::isExact ? parseNumber("0") : parseNumber("1e-6");
    }
    ValueType parseNumber(std::string const& input) const {
        return storm::utility::convertNumber<ValueType>(input);
    }

   private:
    storm::Environment _environment;
};

typedef ::testing::Types<DoubleTopologicalViEnvironment>
    TestingTypes;

TYPED_TEST_SUITE(FixedChoiceTopoligicalMinMaxLinearEquationSolverTest, TestingTypes, );

TYPED_TEST(FixedChoiceTopoligicalMinMaxLinearEquationSolverTest, SolveEquationsFixedChoice) {
    typedef typename TestFixture::ValueType ValueType;

    storm::storage::SparseMatrixBuilder<ValueType> builder(6, 4, 6, false, true);
    ASSERT_NO_THROW(builder.newRowGroup(0));
    ASSERT_NO_THROW(builder.addNextValue(0, 1, this->parseNumber("0.1")));
    ASSERT_NO_THROW(builder.addNextValue(0, 2, this->parseNumber("0.9")));
    ASSERT_NO_THROW(builder.addNextValue(1, 1, this->parseNumber("0.1")));
    ASSERT_NO_THROW(builder.addNextValue(1, 3, this->parseNumber("0.9")));
    ASSERT_NO_THROW(builder.addNextValue(2, 2, this->parseNumber("0.1")));
    ASSERT_NO_THROW(builder.addNextValue(2, 3, this->parseNumber("0.9")));
    ASSERT_NO_THROW(builder.newRowGroup(3));
    ASSERT_NO_THROW(builder.newRowGroup(4));
    ASSERT_NO_THROW(builder.newRowGroup(5));

    storm::storage::SparseMatrix<ValueType> A;
    ASSERT_NO_THROW(A = builder.build());

    std::vector<ValueType> x(4,1);
    std::vector<ValueType> b = {this->parseNumber("0"), this->parseNumber("0"), this->parseNumber("0"),  this->parseNumber("3"), this->parseNumber("5"), this->parseNumber("7")};

    auto factory = storm::solver::GeneralMinMaxLinearEquationSolverFactory<ValueType>();
    auto solver = factory.create(this->env(), A);
    solver->setHasUniqueSolution(true);
    storm::solver::MinMaxLinearEquationSolverRequirements req = solver->getRequirements(this->env());

    std::vector<uint_fast64_t> scheduler(4, 0);
    solver->setInitialScheduler(std::move(scheduler));
    storm::storage::BitVector schedulerFixedForRowGroup(4, false);
    schedulerFixedForRowGroup.set(0);
    solver->setSchedulerFixedForRowGroup(std::move(schedulerFixedForRowGroup));
    req.clearBounds();
    ASSERT_FALSE(req.hasEnabledRequirement());
    ASSERT_NO_THROW(solver->solveEquations(this->env(), storm::OptimizationDirection::Minimize, x, b));
    EXPECT_NEAR(x[0], 4.8, this->precision());
    EXPECT_NEAR(x[1], 3, this->precision());
    EXPECT_NEAR(x[2], 5, this->precision());
    EXPECT_NEAR(x[3], 7, this->precision());
    ASSERT_NO_THROW(solver->solveEquations(this->env(), storm::OptimizationDirection::Maximize, x, b));
    EXPECT_NEAR(x[0], 4.8, this->precision());
    EXPECT_NEAR(x[1], 3, this->precision());
    EXPECT_NEAR(x[2], 5, this->precision());
    EXPECT_NEAR(x[3], 7, this->precision());

    scheduler = std::vector<uint_fast64_t>(4, 0);
    scheduler[0] = 1;
    solver->setInitialScheduler(std::move(scheduler));
    schedulerFixedForRowGroup = storm::storage::BitVector(4, false);
    schedulerFixedForRowGroup.set(0);
    solver->setSchedulerFixedForRowGroup(std::move(schedulerFixedForRowGroup));
    ASSERT_NO_THROW(solver->solveEquations(this->env(), storm::OptimizationDirection::Minimize, x, b));
    EXPECT_NEAR(x[0], 6.6, this->precision());
    EXPECT_NEAR(x[1], 3, this->precision());
    EXPECT_NEAR(x[2], 5, this->precision());
    EXPECT_NEAR(x[3], 7, this->precision());
    ASSERT_NO_THROW(solver->solveEquations(this->env(), storm::OptimizationDirection::Maximize, x, b));
    EXPECT_NEAR(x[0], 6.6, this->precision());
    EXPECT_NEAR(x[1], 3, this->precision());
    EXPECT_NEAR(x[2], 5, this->precision());
    EXPECT_NEAR(x[3], 7, this->precision());

    scheduler = std::vector<uint_fast64_t>(4, 0);
    scheduler[0] = 2;
    solver->setInitialScheduler(std::move(scheduler));
    schedulerFixedForRowGroup = storm::storage::BitVector(4, false);
    schedulerFixedForRowGroup.set(0);
    solver->setSchedulerFixedForRowGroup(std::move(schedulerFixedForRowGroup));
    ASSERT_NO_THROW(solver->solveEquations(this->env(), storm::OptimizationDirection::Minimize, x, b));
    EXPECT_NEAR(x[0], 6.8, this->precision());
    EXPECT_NEAR(x[1], 3, this->precision());
    EXPECT_NEAR(x[2], 5, this->precision());
    EXPECT_NEAR(x[3], 7, this->precision());
    ASSERT_NO_THROW(solver->solveEquations(this->env(), storm::OptimizationDirection::Maximize, x, b));
    EXPECT_NEAR(x[0], 6.8, this->precision());
    EXPECT_NEAR(x[1], 3, this->precision());
    EXPECT_NEAR(x[2], 5, this->precision());
    EXPECT_NEAR(x[3], 7, this->precision());
}
}  // namespace
