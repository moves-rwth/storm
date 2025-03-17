#include "storm-config.h"
#include "test/storm_gtest.h"

#include "storm-conv/api/storm-conv.h"
#include "storm-parsers/api/model_descriptions.h"
#include "storm-parsers/api/properties.h"
#include "storm-parsers/parser/FormulaParser.h"
#include "storm-parsers/parser/PrismParser.h"
#include "storm/api/builder.h"
#include "storm/api/properties.h"
#include "storm/environment/solver/EigenSolverEnvironment.h"
#include "storm/environment/solver/GmmxxSolverEnvironment.h"
#include "storm/environment/solver/NativeSolverEnvironment.h"
#include "storm/environment/solver/TopologicalSolverEnvironment.h"
#include "storm/logic/Formulas.h"
#include "storm/modelchecker/prctl/HybridDtmcPrctlModelChecker.h"
#include "storm/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "storm/modelchecker/prctl/SymbolicDtmcPrctlModelChecker.h"
#include "storm/modelchecker/prctl/helper/SparseDtmcPrctlHelper.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/modelchecker/results/QualitativeCheckResult.h"
#include "storm/modelchecker/results/QuantitativeCheckResult.h"
#include "storm/modelchecker/results/SymbolicQualitativeCheckResult.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/models/symbolic/Dtmc.h"
#include "storm/models/symbolic/StandardRewardModel.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/solver/EigenLinearEquationSolver.h"
#include "storm/storage/expressions/ExpressionManager.h"

namespace {

enum class DtmcEngine { PrismSparse, JaniSparse, Hybrid, PrismDd, JaniDd };

class SparseGmmxxGmresIluEnvironment {
   public:
    static const storm::dd::DdType ddType = storm::dd::DdType::Sylvan;  // unused for sparse models
    static const DtmcEngine engine = DtmcEngine::PrismSparse;
    static const bool isExact = false;
    typedef double ValueType;
    typedef storm::models::sparse::Dtmc<ValueType> ModelType;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Gmmxx);
        env.solver().gmmxx().setMethod(storm::solver::GmmxxLinearEquationSolverMethod::Gmres);
        env.solver().gmmxx().setPreconditioner(storm::solver::GmmxxLinearEquationSolverPreconditioner::Ilu);
        env.solver().gmmxx().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-8));
        return env;
    }
};

class JaniSparseGmmxxGmresIluEnvironment {
   public:
    static const storm::dd::DdType ddType = storm::dd::DdType::Sylvan;  // unused for sparse models
    static const DtmcEngine engine = DtmcEngine::JaniSparse;
    static const bool isExact = false;
    typedef double ValueType;
    typedef storm::models::sparse::Dtmc<ValueType> ModelType;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Gmmxx);
        env.solver().gmmxx().setMethod(storm::solver::GmmxxLinearEquationSolverMethod::Gmres);
        env.solver().gmmxx().setPreconditioner(storm::solver::GmmxxLinearEquationSolverPreconditioner::Ilu);
        env.solver().gmmxx().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-8));
        return env;
    }
};

class SparseGmmxxGmresDiagEnvironment {
   public:
    static const storm::dd::DdType ddType = storm::dd::DdType::Sylvan;  // unused for sparse models
    static const DtmcEngine engine = DtmcEngine::PrismSparse;
    static const bool isExact = false;
    typedef double ValueType;
    typedef storm::models::sparse::Dtmc<ValueType> ModelType;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Gmmxx);
        env.solver().gmmxx().setMethod(storm::solver::GmmxxLinearEquationSolverMethod::Gmres);
        env.solver().gmmxx().setPreconditioner(storm::solver::GmmxxLinearEquationSolverPreconditioner::Diagonal);
        env.solver().gmmxx().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-8));
        return env;
    }
};

class SparseGmmxxBicgstabIluEnvironment {
   public:
    static const storm::dd::DdType ddType = storm::dd::DdType::Sylvan;  // unused for sparse models
    static const DtmcEngine engine = DtmcEngine::PrismSparse;
    static const bool isExact = false;
    typedef double ValueType;
    typedef storm::models::sparse::Dtmc<ValueType> ModelType;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Gmmxx);
        env.solver().gmmxx().setMethod(storm::solver::GmmxxLinearEquationSolverMethod::Bicgstab);
        env.solver().gmmxx().setPreconditioner(storm::solver::GmmxxLinearEquationSolverPreconditioner::Ilu);
        env.solver().gmmxx().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-8));
        return env;
    }
};

class SparseEigenDGmresEnvironment {
   public:
    static const storm::dd::DdType ddType = storm::dd::DdType::Sylvan;  // unused for sparse models
    static const DtmcEngine engine = DtmcEngine::PrismSparse;
    static const bool isExact = false;
    typedef double ValueType;
    typedef storm::models::sparse::Dtmc<ValueType> ModelType;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Eigen);
        env.solver().eigen().setMethod(storm::solver::EigenLinearEquationSolverMethod::DGmres);
        env.solver().eigen().setPreconditioner(storm::solver::EigenLinearEquationSolverPreconditioner::Ilu);
        env.solver().eigen().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-8));
        return env;
    }
};

class SparseEigenDoubleLUEnvironment {
   public:
    static const storm::dd::DdType ddType = storm::dd::DdType::Sylvan;  // unused for sparse models
    static const DtmcEngine engine = DtmcEngine::PrismSparse;
    static const bool isExact = false;
    typedef double ValueType;
    typedef storm::models::sparse::Dtmc<ValueType> ModelType;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Eigen);
        env.solver().eigen().setMethod(storm::solver::EigenLinearEquationSolverMethod::SparseLU);
        return env;
    }
};

class SparseEigenRationalLUEnvironment {
   public:
    static const storm::dd::DdType ddType = storm::dd::DdType::Sylvan;  // unused for sparse models
    static const DtmcEngine engine = DtmcEngine::PrismSparse;
    static const bool isExact = true;
    typedef storm::RationalNumber ValueType;
    typedef storm::models::sparse::Dtmc<ValueType> ModelType;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Eigen);
        env.solver().eigen().setMethod(storm::solver::EigenLinearEquationSolverMethod::SparseLU);
        return env;
    }
};

class SparseRationalEliminationEnvironment {
   public:
    static const storm::dd::DdType ddType = storm::dd::DdType::Sylvan;  // unused for sparse models
    static const DtmcEngine engine = DtmcEngine::PrismSparse;
    static const bool isExact = true;
    typedef storm::RationalNumber ValueType;
    typedef storm::models::sparse::Dtmc<ValueType> ModelType;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Elimination);
        return env;
    }
};

class SparseNativeJacobiEnvironment {
   public:
    static const storm::dd::DdType ddType = storm::dd::DdType::Sylvan;  // unused for sparse models
    static const DtmcEngine engine = DtmcEngine::PrismSparse;
    static const bool isExact = false;
    typedef double ValueType;
    typedef storm::models::sparse::Dtmc<ValueType> ModelType;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Native);
        env.solver().native().setMethod(storm::solver::NativeLinearEquationSolverMethod::Jacobi);
        env.solver().native().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-8));
        return env;
    }
};

class SparseNativeWalkerChaeEnvironment {
   public:
    static const storm::dd::DdType ddType = storm::dd::DdType::Sylvan;  // unused for sparse models
    static const DtmcEngine engine = DtmcEngine::PrismSparse;
    static const bool isExact = false;
    typedef double ValueType;
    typedef storm::models::sparse::Dtmc<ValueType> ModelType;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Native);
        env.solver().native().setMethod(storm::solver::NativeLinearEquationSolverMethod::WalkerChae);
        env.solver().native().setMaximalNumberOfIterations(1000000);
        env.solver().native().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-7));
        return env;
    }
};

class SparseNativeSorEnvironment {
   public:
    static const storm::dd::DdType ddType = storm::dd::DdType::Sylvan;  // unused for sparse models
    static const DtmcEngine engine = DtmcEngine::PrismSparse;
    static const bool isExact = false;
    typedef double ValueType;
    typedef storm::models::sparse::Dtmc<ValueType> ModelType;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Native);
        env.solver().native().setMethod(storm::solver::NativeLinearEquationSolverMethod::SOR);
        env.solver().native().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-8));
        return env;
    }
};

class SparseNativePowerEnvironment {
   public:
    static const storm::dd::DdType ddType = storm::dd::DdType::Sylvan;  // unused for sparse models
    static const DtmcEngine engine = DtmcEngine::PrismSparse;
    static const bool isExact = false;
    typedef double ValueType;
    typedef storm::models::sparse::Dtmc<ValueType> ModelType;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Native);
        env.solver().native().setMethod(storm::solver::NativeLinearEquationSolverMethod::Power);
        env.solver().native().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-8));
        return env;
    }
};

class SparseNativeSoundValueIterationEnvironment {
   public:
    static const storm::dd::DdType ddType = storm::dd::DdType::Sylvan;  // unused for sparse models
    static const DtmcEngine engine = DtmcEngine::PrismSparse;
    static const bool isExact = false;
    typedef double ValueType;
    typedef storm::models::sparse::Dtmc<ValueType> ModelType;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setForceSoundness(true);
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Native);
        env.solver().native().setMethod(storm::solver::NativeLinearEquationSolverMethod::SoundValueIteration);
        env.solver().native().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-6));
        return env;
    }
};

class SparseNativeOptimisticValueIterationEnvironment {
   public:
    static const storm::dd::DdType ddType = storm::dd::DdType::Sylvan;  // unused for sparse models
    static const DtmcEngine engine = DtmcEngine::PrismSparse;
    static const bool isExact = false;
    typedef double ValueType;
    typedef storm::models::sparse::Dtmc<ValueType> ModelType;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setForceSoundness(true);
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Native);
        env.solver().native().setMethod(storm::solver::NativeLinearEquationSolverMethod::OptimisticValueIteration);
        env.solver().native().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-6));
        return env;
    }
};

class SparseNativeIntervalIterationEnvironment {
   public:
    static const storm::dd::DdType ddType = storm::dd::DdType::Sylvan;  // unused for sparse models
    static const DtmcEngine engine = DtmcEngine::PrismSparse;
    static const bool isExact = false;
    typedef double ValueType;
    typedef storm::models::sparse::Dtmc<ValueType> ModelType;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setForceSoundness(true);
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Native);
        env.solver().native().setMethod(storm::solver::NativeLinearEquationSolverMethod::IntervalIteration);
        env.solver().native().setRelativeTerminationCriterion(false);
        env.solver().native().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-6));
        return env;
    }
};

class HybridSylvanGmmxxGmresEnvironment {
   public:
    static const storm::dd::DdType ddType = storm::dd::DdType::Sylvan;
    static const DtmcEngine engine = DtmcEngine::Hybrid;
    static const bool isExact = false;
    typedef double ValueType;
    typedef storm::models::symbolic::Dtmc<ddType, ValueType> ModelType;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Gmmxx);
        env.solver().gmmxx().setMethod(storm::solver::GmmxxLinearEquationSolverMethod::Gmres);
        env.solver().gmmxx().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-8));
        return env;
    }
};

class DdSylvanNativePowerEnvironment {
   public:
    static const storm::dd::DdType ddType = storm::dd::DdType::Sylvan;
    static const DtmcEngine engine = DtmcEngine::PrismDd;
    static const bool isExact = false;
    typedef double ValueType;
    typedef storm::models::symbolic::Dtmc<ddType, ValueType> ModelType;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Native);
        env.solver().native().setMethod(storm::solver::NativeLinearEquationSolverMethod::Power);
        env.solver().native().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-8));
        return env;
    }
};

template<typename TestType>
class DiscountingDtmcPrctlModelCheckerTest : public ::testing::Test {
   public:
    typedef typename TestType::ValueType ValueType;
    typedef typename storm::models::sparse::Dtmc<ValueType> SparseModelType;
    typedef typename storm::models::symbolic::Dtmc<TestType::ddType, ValueType> SymbolicModelType;

    DiscountingDtmcPrctlModelCheckerTest() : _environment(TestType::createEnvironment()) {}

    void SetUp() override {
#ifndef STORM_HAVE_Z3
        GTEST_SKIP() << "Z3 not available.";
#endif
    }

    storm::Environment const& env() const {
        return _environment;
    }
    ValueType parseNumber(std::string const& input) const {
        return storm::utility::convertNumber<ValueType>(input);
    }
    ValueType precision() const {
        return TestType::isExact ? parseNumber("0") : parseNumber("1e-6");
    }
    bool isSparseModel() const {
        return std::is_same<typename TestType::ModelType, SparseModelType>::value;
    }
    bool isSymbolicModel() const {
        return std::is_same<typename TestType::ModelType, SymbolicModelType>::value;
    }

    template<typename MT = typename TestType::ModelType>
    typename std::enable_if<std::is_same<MT, SparseModelType>::value,
                            std::pair<std::shared_ptr<MT>, std::vector<std::shared_ptr<storm::logic::Formula const>>>>::type
    buildModelFormulas(std::string const& pathToPrismFile, std::string const& formulasAsString, std::string const& constantDefinitionString = "") const {
        std::pair<std::shared_ptr<MT>, std::vector<std::shared_ptr<storm::logic::Formula const>>> result;
        storm::prism::Program program = storm::api::parseProgram(pathToPrismFile);
        program = storm::utility::prism::preprocess(program, constantDefinitionString);
        if (TestType::engine == DtmcEngine::PrismSparse) {
            result.second = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulasAsString, program));
            result.first = storm::api::buildSparseModel<ValueType>(program, result.second)->template as<MT>();
        } else if (TestType::engine == DtmcEngine::JaniSparse) {
            auto janiData = storm::api::convertPrismToJani(program, storm::api::parsePropertiesForPrismProgram(formulasAsString, program));
            result.second = storm::api::extractFormulasFromProperties(janiData.second);
            result.first = storm::api::buildSparseModel<ValueType>(janiData.first, result.second)->template as<MT>();
        }

        return result;
    }

    template<typename MT = typename TestType::ModelType>
    typename std::enable_if<std::is_same<MT, SymbolicModelType>::value,
                            std::pair<std::shared_ptr<MT>, std::vector<std::shared_ptr<storm::logic::Formula const>>>>::type
    buildModelFormulas(std::string const& pathToPrismFile, std::string const& formulasAsString, std::string const& constantDefinitionString = "") const {
        std::pair<std::shared_ptr<MT>, std::vector<std::shared_ptr<storm::logic::Formula const>>> result;
        storm::prism::Program program = storm::api::parseProgram(pathToPrismFile);
        program = storm::utility::prism::preprocess(program, constantDefinitionString);
        if (TestType::engine == DtmcEngine::Hybrid || TestType::engine == DtmcEngine::PrismDd) {
            result.second = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulasAsString, program));
            result.first = storm::api::buildSymbolicModel<TestType::ddType, ValueType>(program, result.second)->template as<MT>();
        } else if (TestType::engine == DtmcEngine::JaniDd) {
            auto janiData = storm::api::convertPrismToJani(program, storm::api::parsePropertiesForPrismProgram(formulasAsString, program));
            janiData.first.substituteFunctions();
            result.second = storm::api::extractFormulasFromProperties(janiData.second);
            result.first = storm::api::buildSymbolicModel<TestType::ddType, ValueType>(janiData.first, result.second)->template as<MT>();
        }
        return result;
    }

    std::vector<storm::modelchecker::CheckTask<storm::logic::Formula, ValueType>> getTasks(
        std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas) const {
        std::vector<storm::modelchecker::CheckTask<storm::logic::Formula, ValueType>> result;
        for (auto const& f : formulas) {
            result.emplace_back(*f);
        }
        return result;
    }

    template<typename MT = typename TestType::ModelType>
    typename std::enable_if<std::is_same<MT, SparseModelType>::value, std::shared_ptr<storm::modelchecker::AbstractModelChecker<MT>>>::type createModelChecker(
        std::shared_ptr<MT> const& model) const {
        if (TestType::engine == DtmcEngine::PrismSparse || TestType::engine == DtmcEngine::JaniSparse) {
            return std::make_shared<storm::modelchecker::SparseDtmcPrctlModelChecker<SparseModelType>>(*model);
        }
    }

    template<typename MT = typename TestType::ModelType>
    typename std::enable_if<std::is_same<MT, SymbolicModelType>::value, std::shared_ptr<storm::modelchecker::AbstractModelChecker<MT>>>::type
    createModelChecker(std::shared_ptr<MT> const& model) const {
        if (TestType::engine == DtmcEngine::Hybrid) {
            return std::make_shared<storm::modelchecker::HybridDtmcPrctlModelChecker<SymbolicModelType>>(*model);
        } else if (TestType::engine == DtmcEngine::PrismDd || TestType::engine == DtmcEngine::JaniDd) {
            return std::make_shared<storm::modelchecker::SymbolicDtmcPrctlModelChecker<SymbolicModelType>>(*model);
        }
    }

    bool getQualitativeResultAtInitialState(std::shared_ptr<storm::models::Model<ValueType>> const& model,
                                            std::unique_ptr<storm::modelchecker::CheckResult>& result) {
        auto filter = getInitialStateFilter(model);
        result->filter(*filter);
        return result->asQualitativeCheckResult().forallTrue();
    }

    ValueType getQuantitativeResultAtInitialState(std::shared_ptr<storm::models::Model<ValueType>> const& model,
                                                  std::unique_ptr<storm::modelchecker::CheckResult>& result) {
        auto filter = getInitialStateFilter(model);
        result->filter(*filter);
        return result->asQuantitativeCheckResult<ValueType>().getMin();
    }

   private:
    storm::Environment _environment;

    std::unique_ptr<storm::modelchecker::QualitativeCheckResult> getInitialStateFilter(std::shared_ptr<storm::models::Model<ValueType>> const& model) const {
        if (isSparseModel()) {
            return std::make_unique<storm::modelchecker::ExplicitQualitativeCheckResult>(model->template as<SparseModelType>()->getInitialStates());
        } else {
            return std::make_unique<storm::modelchecker::SymbolicQualitativeCheckResult<TestType::ddType>>(
                model->template as<SymbolicModelType>()->getReachableStates(), model->template as<SymbolicModelType>()->getInitialStates());
        }
    }
};

typedef ::testing::Types<SparseGmmxxGmresIluEnvironment, JaniSparseGmmxxGmresIluEnvironment, SparseGmmxxGmresDiagEnvironment, SparseGmmxxBicgstabIluEnvironment,
                         SparseEigenDGmresEnvironment, SparseEigenDoubleLUEnvironment, SparseEigenRationalLUEnvironment, SparseRationalEliminationEnvironment,
                         SparseNativeJacobiEnvironment, SparseNativeWalkerChaeEnvironment, SparseNativeSorEnvironment, SparseNativePowerEnvironment,
                         SparseNativeSoundValueIterationEnvironment, SparseNativeOptimisticValueIterationEnvironment, SparseNativeIntervalIterationEnvironment,
                         HybridSylvanGmmxxGmresEnvironment, DdSylvanNativePowerEnvironment>
    TestingTypes;

TYPED_TEST_SUITE(DiscountingDtmcPrctlModelCheckerTest, TestingTypes, );

TYPED_TEST(DiscountingDtmcPrctlModelCheckerTest, SmallDiscount) {
    std::string formulasString = "R=? [ C ]";
    formulasString += "; R=? [ Cdiscount=9/10 ]";
    formulasString += "; R=? [ Cdiscount=15/16 ]";
    formulasString += "; R=? [ C<5discount=9/10 ]";
    formulasString += "; R=? [ C<5discount=15/16 ]";
    auto modelFormulas = this->buildModelFormulas(STORM_TEST_RESOURCES_DIR "/dtmc/small_discount.nm", formulasString);
    auto model = std::move(modelFormulas.first);
    auto tasks = this->getTasks(modelFormulas.second);
    EXPECT_EQ(3ul, model->getNumberOfStates());
    EXPECT_EQ(4ul, model->getNumberOfTransitions());
    ASSERT_EQ(model->getType(), storm::models::ModelType::Dtmc);
    auto checker = this->createModelChecker(model);
    std::unique_ptr<storm::modelchecker::CheckResult> result;

    // This example considers a discounted expected total reward formula, which is currently only supported in a value iteration setting for sparse models

    if (TypeParam::engine == DtmcEngine::PrismSparse || TypeParam::engine == DtmcEngine::JaniSparse) {
        result = checker->check(this->env(), tasks[0]);
        EXPECT_NEAR(this->parseNumber("5"), this->getQuantitativeResultAtInitialState(model, result), this->precision());

        if (TypeParam::isExact) {
            // Exact solving of discounted expected total reward formulas is not supported yet
            STORM_SILENT_EXPECT_THROW(result = checker->check(this->env(), tasks[1]), storm::exceptions::NotSupportedException);
            STORM_SILENT_EXPECT_THROW(result = checker->check(this->env(), tasks[2]), storm::exceptions::NotSupportedException);
        } else {
            result = checker->check(this->env(), tasks[1]);
            EXPECT_NEAR(this->parseNumber("45/14"), this->getQuantitativeResultAtInitialState(model, result), this->precision());
            result = checker->check(this->env(), tasks[2]);
            EXPECT_NEAR(this->parseNumber("15/4"), this->getQuantitativeResultAtInitialState(model, result), this->precision());
        }
        result = checker->check(this->env(), tasks[3]);
        EXPECT_NEAR(this->parseNumber("12591/6250"), this->getQuantitativeResultAtInitialState(model, result), this->precision());
        result = checker->check(this->env(), tasks[4]);
        EXPECT_NEAR(this->parseNumber("555/256"), this->getQuantitativeResultAtInitialState(model, result), this->precision());
    } else {
        EXPECT_FALSE(checker->canHandle(tasks[0]));
        EXPECT_FALSE(checker->canHandle(tasks[1]));
        EXPECT_FALSE(checker->canHandle(tasks[2]));
        EXPECT_FALSE(checker->canHandle(tasks[3]));
        EXPECT_FALSE(checker->canHandle(tasks[4]));
    }
}

}  // namespace
