#include "storm-config.h"
#include "test/storm_gtest.h"

#include "storm/environment/solver/MinMaxSolverEnvironment.h"
#include "storm/environment/solver/NativeSolverEnvironment.h"
#include "storm/environment/solver/TopologicalSolverEnvironment.h"
#include "storm/solver/MinMaxLinearEquationSolver.h"
#include "storm/solver/SolverSelectionOptions.h"
#include "storm/storage/SparseMatrix.h"

namespace {

class DoubleViEnvironment {
   public:
    typedef double ValueType;
    static const bool isExact = false;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().minMax().setMethod(storm::solver::MinMaxMethod::ValueIteration);
        env.solver().minMax().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-8));
        return env;
    }
};

class DoubleViRegMultEnvironment {
   public:
    typedef double ValueType;
    static const bool isExact = false;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().minMax().setMethod(storm::solver::MinMaxMethod::ValueIteration);
        env.solver().minMax().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-8));
        env.solver().minMax().setMultiplicationStyle(storm::solver::MultiplicationStyle::Regular);
        return env;
    }
};

class DoubleSoundViEnvironment {
   public:
    typedef double ValueType;
    static const bool isExact = false;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().minMax().setMethod(storm::solver::MinMaxMethod::SoundValueIteration);
        env.solver().setForceSoundness(true);
        env.solver().minMax().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-6));
        return env;
    }
};

class DoubleIntervalIterationEnvironment {
   public:
    typedef double ValueType;
    static const bool isExact = false;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().minMax().setMethod(storm::solver::MinMaxMethod::IntervalIteration);
        env.solver().setForceSoundness(true);
        env.solver().minMax().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-6));
        return env;
    }
};

class DoubleGuessingViEnvironment {
   public:
    typedef double ValueType;
    static const bool isExact = false;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().minMax().setMethod(storm::solver::MinMaxMethod::GuessingValueIteration);
        env.solver().setForceSoundness(true);
        env.solver().minMax().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-6));
        return env;
    }
};

class DoubleOptimisticViEnvironment {
   public:
    typedef double ValueType;
    static const bool isExact = false;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().minMax().setMethod(storm::solver::MinMaxMethod::OptimisticValueIteration);
        env.solver().setForceSoundness(true);
        env.solver().minMax().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-6));
        return env;
    }
};

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

class DoublePIEnvironment {
   public:
    typedef double ValueType;
    static const bool isExact = false;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().minMax().setMethod(storm::solver::MinMaxMethod::PolicyIteration);
        env.solver().minMax().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-8));
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Native);
        env.solver().native().setMethod(storm::solver::NativeLinearEquationSolverMethod::Jacobi);
        env.solver().setLinearEquationSolverPrecision(env.solver().minMax().getPrecision());
        return env;
    }
};
class RationalPIEnvironment {
   public:
    typedef storm::RationalNumber ValueType;
    static const bool isExact = true;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().minMax().setMethod(storm::solver::MinMaxMethod::PolicyIteration);
        return env;
    }
};
class RationalRationalSearchEnvironment {
   public:
    typedef storm::RationalNumber ValueType;
    static const bool isExact = true;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().minMax().setMethod(storm::solver::MinMaxMethod::RationalSearch);
        return env;
    }
};

template<typename TestType>
class MinMaxLinearEquationSolverTest : public ::testing::Test {
   public:
    typedef typename TestType::ValueType ValueType;
    MinMaxLinearEquationSolverTest() : _environment(TestType::createEnvironment()) {}
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

typedef ::testing::Types<DoubleViEnvironment, DoubleViRegMultEnvironment, DoubleSoundViEnvironment, DoubleIntervalIterationEnvironment,
                         DoubleOptimisticViEnvironment, DoubleGuessingViEnvironment, DoubleTopologicalViEnvironment, DoublePIEnvironment, RationalPIEnvironment,
                         RationalRationalSearchEnvironment>
    TestingTypes;

TYPED_TEST_SUITE(MinMaxLinearEquationSolverTest, TestingTypes, );

TYPED_TEST(MinMaxLinearEquationSolverTest, SolveEquations) {
    typedef typename TestFixture::ValueType ValueType;

    storm::storage::SparseMatrixBuilder<ValueType> builder(0, 0, 0, false, true);
    ASSERT_NO_THROW(builder.newRowGroup(0));
    ASSERT_NO_THROW(builder.addNextValue(0, 0, this->parseNumber("0.9")));

    storm::storage::SparseMatrix<ValueType> A;
    ASSERT_NO_THROW(A = builder.build(2));

    std::vector<ValueType> x(1);
    std::vector<ValueType> b = {this->parseNumber("0.099"), this->parseNumber("0.5")};

    auto factory = storm::solver::GeneralMinMaxLinearEquationSolverFactory<ValueType>();
    auto solver = factory.create(this->env(), A);
    solver->setHasUniqueSolution(true);
    solver->setHasNoEndComponents(true);
    solver->setBounds(this->parseNumber("0"), this->parseNumber("2"));
    storm::solver::MinMaxLinearEquationSolverRequirements req = solver->getRequirements(this->env());
    req.clearBounds();
    ASSERT_FALSE(req.hasEnabledRequirement());
    ASSERT_NO_THROW(solver->solveEquations(this->env(), storm::OptimizationDirection::Minimize, x, b));
    EXPECT_NEAR(x[0], this->parseNumber("0.5"), this->precision());

    ASSERT_NO_THROW(solver->solveEquations(this->env(), storm::OptimizationDirection::Maximize, x, b));
    EXPECT_NEAR(x[0], this->parseNumber("0.99"), this->precision());
}
TEST(IntervalIterationJustPreciseEnough, MinMax) {
    storm::storage::SparseMatrixBuilder<double> builder(0, 0, 0, false, true);
    builder.newRowGroup(0);
    builder.addNextValue(0, 0, 0.9);
    builder.addNextValue(1, 0, 0.9);
    auto matrix = builder.build();
    std::vector<double> b = {0.099, 0.05};  // Fixed points 0.99 and 0.5.
    double const precision = 1e-3;

    for (bool relative : {false, true}) {
        for (auto dir : {storm::OptimizationDirection::Minimize, storm::OptimizationDirection::Maximize}) {
            SCOPED_TRACE(::testing::Message() << "relative=" << relative << ", maximize=" << storm::solver::maximize(dir));
            auto env = DoubleIntervalIterationEnvironment::createEnvironment();
            env.solver().minMax().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(precision));
            env.solver().minMax().setRelativeTerminationCriterion(relative);
            auto solver = storm::solver::GeneralMinMaxLinearEquationSolverFactory<double>().create(env, matrix);
            solver->setHasUniqueSolution(true);
            solver->setHasNoEndComponents(true);
            solver->setBounds(0.0, 1.0);
            std::vector<double> x(1);
            ASSERT_TRUE(solver->solveEquations(env, dir, x, b));
            ASSERT_TRUE(solver->hasSolutionLowerBounds());
            ASSERT_TRUE(solver->hasSolutionUpperBounds());
            double const lower = solver->getSolutionLowerBounds()[0];
            double const upper = solver->getSolutionUpperBounds()[0];
            double const exact = storm::solver::maximize(dir) ? 0.99 : 0.5;
            double const tolerance = precision * (relative ? lower : 1.0);
            EXPECT_LE(lower, exact);
            EXPECT_GE(upper, exact);
            EXPECT_DOUBLE_EQ(x[0], (lower + upper) / 2.0);
            EXPECT_NEAR(x[0], exact, precision * (relative ? exact : 1.0));
            EXPECT_LE(upper - lower, 2.0 * tolerance);
            // With this slow contraction, stopping at half this width takes unnecessary iterations.
            EXPECT_GT(upper - lower, tolerance);
        }
    }
}
}  // namespace
