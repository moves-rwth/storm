#include "storm-config.h"
#include "test/storm_gtest.h"

#include "storm/environment/solver/EigenSolverEnvironment.h"
#include "storm/environment/solver/GmmxxSolverEnvironment.h"
#include "storm/environment/solver/NativeSolverEnvironment.h"
#include "storm/environment/solver/TopologicalSolverEnvironment.h"
#include "storm/solver/LinearEquationSolver.h"

#include "storm/utility/vector.h"
namespace {

class NativeDoublePowerEnvironment {
   public:
    typedef double ValueType;
    static const bool isExact = false;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Native);
        env.solver().native().setMethod(storm::solver::NativeLinearEquationSolverMethod::Power);
        env.solver().native().setPrecision(storm::utility::convertNumber<storm::RationalNumber, std::string>("1e-10"));
        return env;
    }
};

class NativeDoublePowerRegMultEnvironment {
   public:
    typedef double ValueType;
    static const bool isExact = false;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Native);
        env.solver().native().setMethod(storm::solver::NativeLinearEquationSolverMethod::Power);
        env.solver().native().setPrecision(storm::utility::convertNumber<storm::RationalNumber, std::string>("1e-10"));
        env.solver().native().setPowerMethodMultiplicationStyle(storm::solver::MultiplicationStyle::Regular);
        return env;
    }
};

class NativeDoubleSoundValueIterationEnvironment {
   public:
    typedef double ValueType;
    static const bool isExact = false;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setForceSoundness(true);
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Native);
        env.solver().native().setMethod(storm::solver::NativeLinearEquationSolverMethod::SoundValueIteration);
        env.solver().native().setRelativeTerminationCriterion(false);
        env.solver().native().setPrecision(storm::utility::convertNumber<storm::RationalNumber, std::string>("1e-6"));
        return env;
    }
};

class NativeDoubleOptimisticValueIterationEnvironment {
   public:
    typedef double ValueType;
    static const bool isExact = false;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setForceSoundness(true);
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Native);
        env.solver().native().setMethod(storm::solver::NativeLinearEquationSolverMethod::OptimisticValueIteration);
        env.solver().native().setRelativeTerminationCriterion(false);
        env.solver().native().setPrecision(storm::utility::convertNumber<storm::RationalNumber, std::string>("1e-6"));
        return env;
    }
};

class NativeDoubleIntervalIterationEnvironment {
   public:
    typedef double ValueType;
    static const bool isExact = false;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setForceSoundness(true);
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Native);
        env.solver().native().setMethod(storm::solver::NativeLinearEquationSolverMethod::IntervalIteration);
        env.solver().native().setRelativeTerminationCriterion(false);
        env.solver().native().setPrecision(storm::utility::convertNumber<storm::RationalNumber, std::string>("1e-6"));
        return env;
    }
};

class NativeDoubleJacobiEnvironment {
   public:
    typedef double ValueType;
    static const bool isExact = false;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Native);
        env.solver().native().setMethod(storm::solver::NativeLinearEquationSolverMethod::Jacobi);
        env.solver().native().setPrecision(storm::utility::convertNumber<storm::RationalNumber, std::string>("1e-10"));
        return env;
    }
};

class NativeDoubleGaussSeidelEnvironment {
   public:
    typedef double ValueType;
    static const bool isExact = false;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Native);
        env.solver().native().setMethod(storm::solver::NativeLinearEquationSolverMethod::GaussSeidel);
        env.solver().native().setPrecision(storm::utility::convertNumber<storm::RationalNumber, std::string>("1e-10"));
        return env;
    }
};

class NativeDoubleSorEnvironment {
   public:
    typedef double ValueType;
    static const bool isExact = false;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Native);
        env.solver().native().setMethod(storm::solver::NativeLinearEquationSolverMethod::SOR);
        env.solver().native().setPrecision(storm::utility::convertNumber<storm::RationalNumber, std::string>("1e-10"));
        return env;
    }
};

class NativeDoubleWalkerChaeEnvironment {
   public:
    typedef double ValueType;
    static const bool isExact = false;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Native);
        env.solver().native().setMethod(storm::solver::NativeLinearEquationSolverMethod::WalkerChae);
        env.solver().native().setPrecision(storm::utility::convertNumber<storm::RationalNumber, std::string>("1e-8"));
        env.solver().native().setMaximalNumberOfIterations(500000);
        return env;
    }
};

class NativeRationalRationalSearchEnvironment {
   public:
    typedef storm::RationalNumber ValueType;
    static const bool isExact = true;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Native);
        env.solver().native().setMethod(storm::solver::NativeLinearEquationSolverMethod::RationalSearch);
        return env;
    }
};

class EliminationRationalEnvironment {
   public:
    typedef storm::RationalNumber ValueType;
    static const bool isExact = true;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Elimination);
        return env;
    }
};

class GmmGmresIluEnvironment {
   public:
    typedef double ValueType;
    static const bool isExact = false;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Gmmxx);
        env.solver().gmmxx().setMethod(storm::solver::GmmxxLinearEquationSolverMethod::Gmres);
        env.solver().gmmxx().setPreconditioner(storm::solver::GmmxxLinearEquationSolverPreconditioner::Ilu);
        env.solver().gmmxx().setPrecision(storm::utility::convertNumber<storm::RationalNumber, std::string>("1e-8"));
        return env;
    }
};

class GmmGmresDiagonalEnvironment {
   public:
    typedef double ValueType;
    static const bool isExact = false;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Gmmxx);
        env.solver().gmmxx().setMethod(storm::solver::GmmxxLinearEquationSolverMethod::Gmres);
        env.solver().gmmxx().setPreconditioner(storm::solver::GmmxxLinearEquationSolverPreconditioner::Diagonal);
        env.solver().gmmxx().setPrecision(storm::utility::convertNumber<storm::RationalNumber, std::string>("1e-8"));
        return env;
    }
};

class GmmGmresNoneEnvironment {
   public:
    typedef double ValueType;
    static const bool isExact = false;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Gmmxx);
        env.solver().gmmxx().setMethod(storm::solver::GmmxxLinearEquationSolverMethod::Gmres);
        env.solver().gmmxx().setPreconditioner(storm::solver::GmmxxLinearEquationSolverPreconditioner::None);
        env.solver().gmmxx().setPrecision(storm::utility::convertNumber<storm::RationalNumber, std::string>("1e-8"));
        return env;
    }
};

class GmmBicgstabIluEnvironment {
   public:
    typedef double ValueType;
    static const bool isExact = false;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Gmmxx);
        env.solver().gmmxx().setMethod(storm::solver::GmmxxLinearEquationSolverMethod::Bicgstab);
        env.solver().gmmxx().setPreconditioner(storm::solver::GmmxxLinearEquationSolverPreconditioner::Ilu);
        env.solver().gmmxx().setPrecision(storm::utility::convertNumber<storm::RationalNumber, std::string>("1e-8"));
        return env;
    }
};

class GmmQmrDiagonalEnvironment {
   public:
    typedef double ValueType;
    static const bool isExact = false;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Gmmxx);
        env.solver().gmmxx().setMethod(storm::solver::GmmxxLinearEquationSolverMethod::Qmr);
        env.solver().gmmxx().setPreconditioner(storm::solver::GmmxxLinearEquationSolverPreconditioner::Diagonal);
        env.solver().gmmxx().setPrecision(storm::utility::convertNumber<storm::RationalNumber, std::string>("1e-8"));
        return env;
    }
};

class EigenDGmresDiagonalEnvironment {
   public:
    typedef double ValueType;
    static const bool isExact = false;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Eigen);
        env.solver().eigen().setMethod(storm::solver::EigenLinearEquationSolverMethod::DGmres);
        env.solver().eigen().setPreconditioner(storm::solver::EigenLinearEquationSolverPreconditioner::Diagonal);
        env.solver().eigen().setPrecision(storm::utility::convertNumber<storm::RationalNumber, std::string>("1e-8"));
        return env;
    }
};

class EigenGmresIluEnvironment {
   public:
    typedef double ValueType;
    static const bool isExact = false;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Eigen);
        env.solver().eigen().setMethod(storm::solver::EigenLinearEquationSolverMethod::Gmres);
        env.solver().eigen().setPreconditioner(storm::solver::EigenLinearEquationSolverPreconditioner::Ilu);
        env.solver().eigen().setPrecision(storm::utility::convertNumber<storm::RationalNumber, std::string>("1e-8"));
        return env;
    }
};

class EigenBicgstabNoneEnvironment {
   public:
    typedef double ValueType;
    static const bool isExact = false;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Eigen);
        env.solver().eigen().setMethod(storm::solver::EigenLinearEquationSolverMethod::Bicgstab);
        env.solver().eigen().setPreconditioner(storm::solver::EigenLinearEquationSolverPreconditioner::None);
        env.solver().eigen().setPrecision(storm::utility::convertNumber<storm::RationalNumber, std::string>("1e-8"));
        return env;
    }
};

class EigenDoubleLUEnvironment {
   public:
    typedef double ValueType;
    static const bool isExact = false;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Eigen);
        env.solver().eigen().setMethod(storm::solver::EigenLinearEquationSolverMethod::SparseLU);
        return env;
    }
};

class EigenRationalLUEnvironment {
   public:
    typedef storm::RationalNumber ValueType;
    static const bool isExact = true;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Eigen);
        env.solver().eigen().setMethod(storm::solver::EigenLinearEquationSolverMethod::SparseLU);
        return env;
    }
};

class TopologicalEigenRationalLUEnvironment {
   public:
    typedef storm::RationalNumber ValueType;
    static const bool isExact = true;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Topological);
        env.solver().topological().setUnderlyingEquationSolverType(storm::solver::EquationSolverType::Eigen);
        env.solver().eigen().setMethod(storm::solver::EigenLinearEquationSolverMethod::SparseLU);
        return env;
    }
};

template<typename TestType>
class LinearEquationSolverTest : public ::testing::Test {
   public:
    typedef typename TestType::ValueType ValueType;
    LinearEquationSolverTest() : _environment(TestType::createEnvironment()) {}
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

typedef ::testing::Types<NativeDoublePowerEnvironment, NativeDoublePowerRegMultEnvironment, NativeDoubleSoundValueIterationEnvironment,
                         NativeDoubleOptimisticValueIterationEnvironment, NativeDoubleIntervalIterationEnvironment, NativeDoubleJacobiEnvironment,
                         NativeDoubleGaussSeidelEnvironment, NativeDoubleSorEnvironment, NativeDoubleWalkerChaeEnvironment,
                         NativeRationalRationalSearchEnvironment, EliminationRationalEnvironment, GmmGmresIluEnvironment, GmmGmresDiagonalEnvironment,
                         GmmGmresNoneEnvironment, GmmBicgstabIluEnvironment, GmmQmrDiagonalEnvironment, EigenDGmresDiagonalEnvironment,
                         EigenGmresIluEnvironment, EigenBicgstabNoneEnvironment, EigenDoubleLUEnvironment, EigenRationalLUEnvironment,
                         TopologicalEigenRationalLUEnvironment>
    TestingTypes;

TYPED_TEST_SUITE(LinearEquationSolverTest, TestingTypes, );

TYPED_TEST(LinearEquationSolverTest, solveEquationSystem) {
    typedef typename TestFixture::ValueType ValueType;
    ASSERT_NO_THROW(storm::storage::SparseMatrixBuilder<ValueType> builder);
    storm::storage::SparseMatrixBuilder<ValueType> builder;
    ASSERT_NO_THROW(builder.addNextValue(0, 0, this->parseNumber("1/5")));
    ASSERT_NO_THROW(builder.addNextValue(0, 1, this->parseNumber("2/5")));
    ASSERT_NO_THROW(builder.addNextValue(0, 2, this->parseNumber("2/5")));
    ASSERT_NO_THROW(builder.addNextValue(1, 0, this->parseNumber("1/50")));
    ASSERT_NO_THROW(builder.addNextValue(1, 1, this->parseNumber("48/50")));
    ASSERT_NO_THROW(builder.addNextValue(1, 2, this->parseNumber("1/50")));
    ASSERT_NO_THROW(builder.addNextValue(2, 0, this->parseNumber("4/10")));
    ASSERT_NO_THROW(builder.addNextValue(2, 1, this->parseNumber("3/10")));
    ASSERT_NO_THROW(builder.addNextValue(2, 2, this->parseNumber("0")));

    storm::storage::SparseMatrix<ValueType> A;
    ASSERT_NO_THROW(A = builder.build());

    std::vector<ValueType> x(3);
    std::vector<ValueType> b = {this->parseNumber("3"), this->parseNumber("-0.01"), this->parseNumber("12")};

    auto factory = storm::solver::GeneralLinearEquationSolverFactory<ValueType>();
    if (factory.getEquationProblemFormat(this->env()) == storm::solver::LinearEquationSolverProblemFormat::EquationSystem) {
        A.convertToEquationSystem();
    }

    auto requirements = factory.getRequirements(this->env());
    requirements.clearUpperBounds();
    requirements.clearLowerBounds();
    ASSERT_FALSE(requirements.hasEnabledRequirement());
    auto solver = factory.create(this->env(), A);
    solver->setBounds(this->parseNumber("-100"), this->parseNumber("100"));
    ASSERT_NO_THROW(solver->solveEquations(this->env(), x, b));
    EXPECT_NEAR(x[0], this->parseNumber("481/9"), this->precision());
    EXPECT_NEAR(x[1], this->parseNumber("457/9"), this->precision());
    EXPECT_NEAR(x[2], this->parseNumber("875/18"), this->precision());
}
}  // namespace