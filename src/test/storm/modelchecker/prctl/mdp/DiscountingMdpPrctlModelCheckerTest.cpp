#include "test/storm_gtest.h"

#include "storm-config.h"

#include "storm-conv/api/storm-conv.h"
#include "storm-parsers/api/model_descriptions.h"
#include "storm-parsers/api/properties.h"
#include "storm/api/builder.h"
#include "storm/api/properties.h"

#include "storm/exceptions/NotSupportedException.h"

#include "storm/logic/Formulas.h"
#include "storm/modelchecker/prctl/HybridMdpPrctlModelChecker.h"
#include "storm/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "storm/modelchecker/prctl/SymbolicMdpPrctlModelChecker.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/modelchecker/results/QualitativeCheckResult.h"
#include "storm/modelchecker/results/QuantitativeCheckResult.h"
#include "storm/modelchecker/results/SymbolicQualitativeCheckResult.h"
#include "storm/settings/SettingsManager.h"
#include "storm/solver/StandardMinMaxLinearEquationSolver.h"

#include "storm/environment/solver/MinMaxSolverEnvironment.h"
#include "storm/environment/solver/MultiplierEnvironment.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/modules/NativeEquationSolverSettings.h"

namespace {

enum class MdpEngine { PrismSparse, JaniSparse, Hybrid, PrismDd, JaniDd };

class SparseDoubleValueIterationGmmxxGaussSeidelMultEnvironment {
   public:
    static const storm::dd::DdType ddType = storm::dd::DdType::Sylvan;  // Unused for sparse models
    static const MdpEngine engine = MdpEngine::PrismSparse;
    static const bool isExact = false;
    typedef double ValueType;
    typedef storm::models::sparse::Mdp<ValueType> ModelType;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().minMax().setMethod(storm::solver::MinMaxMethod::ValueIteration);
        env.solver().minMax().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-10));
        env.solver().minMax().setMultiplicationStyle(storm::solver::MultiplicationStyle::GaussSeidel);
        env.solver().multiplier().setType(storm::solver::MultiplierType::ViOperator);
        return env;
    }
};

class SparseDoubleValueIterationGmmxxRegularMultEnvironment {
   public:
    static const storm::dd::DdType ddType = storm::dd::DdType::Sylvan;  // Unused for sparse models
    static const MdpEngine engine = MdpEngine::PrismSparse;
    static const bool isExact = false;
    typedef double ValueType;
    typedef storm::models::sparse::Mdp<ValueType> ModelType;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().minMax().setMethod(storm::solver::MinMaxMethod::ValueIteration);
        env.solver().minMax().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-10));
        env.solver().minMax().setMultiplicationStyle(storm::solver::MultiplicationStyle::Regular);
        env.solver().multiplier().setType(storm::solver::MultiplierType::ViOperator);
        return env;
    }
};

class SparseDoubleValueIterationNativeGaussSeidelMultEnvironment {
   public:
    static const storm::dd::DdType ddType = storm::dd::DdType::Sylvan;  // Unused for sparse models
    static const MdpEngine engine = MdpEngine::PrismSparse;
    static const bool isExact = false;
    typedef double ValueType;
    typedef storm::models::sparse::Mdp<ValueType> ModelType;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().minMax().setMethod(storm::solver::MinMaxMethod::ValueIteration);
        env.solver().minMax().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-10));
        env.solver().minMax().setMultiplicationStyle(storm::solver::MultiplicationStyle::GaussSeidel);
        env.solver().multiplier().setType(storm::solver::MultiplierType::Native);
        return env;
    }
};

class SparseDoubleValueIterationNativeRegularMultEnvironment {
   public:
    static const storm::dd::DdType ddType = storm::dd::DdType::Sylvan;  // Unused for sparse models
    static const MdpEngine engine = MdpEngine::PrismSparse;
    static const bool isExact = false;
    typedef double ValueType;
    typedef storm::models::sparse::Mdp<ValueType> ModelType;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().minMax().setMethod(storm::solver::MinMaxMethod::ValueIteration);
        env.solver().minMax().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-10));
        env.solver().minMax().setMultiplicationStyle(storm::solver::MultiplicationStyle::Regular);
        env.solver().multiplier().setType(storm::solver::MultiplierType::Native);
        return env;
    }
};

class JaniSparseDoubleValueIterationEnvironment {
   public:
    static const storm::dd::DdType ddType = storm::dd::DdType::Sylvan;  // Unused for sparse models
    static const MdpEngine engine = MdpEngine::JaniSparse;
    static const bool isExact = false;
    typedef double ValueType;
    typedef storm::models::sparse::Mdp<ValueType> ModelType;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().minMax().setMethod(storm::solver::MinMaxMethod::ValueIteration);
        env.solver().minMax().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-10));
        return env;
    }
};

class SparseDoubleIntervalIterationEnvironment {
   public:
    static const storm::dd::DdType ddType = storm::dd::DdType::Sylvan;  // Unused for sparse models
    static const MdpEngine engine = MdpEngine::PrismSparse;
    static const bool isExact = false;
    typedef double ValueType;
    typedef storm::models::sparse::Mdp<ValueType> ModelType;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setForceSoundness(true);
        env.solver().minMax().setMethod(storm::solver::MinMaxMethod::IntervalIteration);
        env.solver().minMax().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-6));
        env.solver().minMax().setRelativeTerminationCriterion(false);
        return env;
    }
};

class SparseDoubleSoundValueIterationEnvironment {
   public:
    static const storm::dd::DdType ddType = storm::dd::DdType::Sylvan;  // Unused for sparse models
    static const MdpEngine engine = MdpEngine::PrismSparse;
    static const bool isExact = false;
    typedef double ValueType;
    typedef storm::models::sparse::Mdp<ValueType> ModelType;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setForceSoundness(true);
        env.solver().minMax().setMethod(storm::solver::MinMaxMethod::SoundValueIteration);
        env.solver().minMax().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-6));
        env.solver().minMax().setRelativeTerminationCriterion(false);
        return env;
    }
};

class SparseDoubleOptimisticValueIterationEnvironment {
   public:
    static const storm::dd::DdType ddType = storm::dd::DdType::Sylvan;  // Unused for sparse models
    static const MdpEngine engine = MdpEngine::PrismSparse;
    static const bool isExact = false;
    typedef double ValueType;
    typedef storm::models::sparse::Mdp<ValueType> ModelType;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setForceSoundness(true);
        env.solver().minMax().setMethod(storm::solver::MinMaxMethod::OptimisticValueIteration);
        env.solver().minMax().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-6));
        env.solver().minMax().setRelativeTerminationCriterion(false);
        return env;
    }
};

class SparseRationalPolicyIterationEnvironment {
   public:
    static const storm::dd::DdType ddType = storm::dd::DdType::Sylvan;  // Unused for sparse models
    static const MdpEngine engine = MdpEngine::PrismSparse;
    static const bool isExact = true;
    typedef storm::RationalNumber ValueType;
    typedef storm::models::sparse::Mdp<ValueType> ModelType;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().setForceExact(true);
        env.solver().minMax().setMethod(storm::solver::MinMaxMethod::PolicyIteration);
        return env;
    }
};

class HybridCuddDoubleValueIterationEnvironment {
   public:
    static const storm::dd::DdType ddType = storm::dd::DdType::CUDD;
    static const MdpEngine engine = MdpEngine::Hybrid;
    static const bool isExact = false;
    typedef double ValueType;
    typedef storm::models::symbolic::Mdp<ddType, ValueType> ModelType;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().minMax().setMethod(storm::solver::MinMaxMethod::ValueIteration);
        env.solver().minMax().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-10));
        return env;
    }
};

template<typename TestType>
class DiscountingMdpPrctlModelCheckerTest : public ::testing::Test {
   public:
    typedef typename TestType::ValueType ValueType;
    typedef typename storm::models::sparse::Mdp<ValueType> SparseModelType;
    typedef typename storm::models::symbolic::Mdp<TestType::ddType, ValueType> SymbolicModelType;

    DiscountingMdpPrctlModelCheckerTest() : _environment(TestType::createEnvironment()) {}

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
        if (TestType::engine == MdpEngine::PrismSparse) {
            result.second = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulasAsString, program));
            result.first = storm::api::buildSparseModel<ValueType>(program, result.second)->template as<MT>();
        } else if (TestType::engine == MdpEngine::JaniSparse) {
            auto janiData = storm::api::convertPrismToJani(program, storm::api::parsePropertiesForPrismProgram(formulasAsString, program));
            janiData.first.substituteFunctions();
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
        if (TestType::engine == MdpEngine::Hybrid || TestType::engine == MdpEngine::PrismDd) {
            result.second = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulasAsString, program));
            result.first = storm::api::buildSymbolicModel<TestType::ddType, ValueType>(program, result.second)->template as<MT>();
        } else if (TestType::engine == MdpEngine::JaniDd) {
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
        if (TestType::engine == MdpEngine::PrismSparse || TestType::engine == MdpEngine::JaniSparse) {
            return std::make_shared<storm::modelchecker::SparseMdpPrctlModelChecker<SparseModelType>>(*model);
        }
    }

    template<typename MT = typename TestType::ModelType>
    typename std::enable_if<std::is_same<MT, SymbolicModelType>::value, std::shared_ptr<storm::modelchecker::AbstractModelChecker<MT>>>::type
    createModelChecker(std::shared_ptr<MT> const& model) const {
        if (TestType::engine == MdpEngine::Hybrid) {
            return std::make_shared<storm::modelchecker::HybridMdpPrctlModelChecker<SymbolicModelType>>(*model);
        } else if (TestType::engine == MdpEngine::PrismDd || TestType::engine == MdpEngine::JaniDd) {
            return std::make_shared<storm::modelchecker::SymbolicMdpPrctlModelChecker<SymbolicModelType>>(*model);
        }
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

typedef ::testing::Types<SparseDoubleValueIterationGmmxxGaussSeidelMultEnvironment, SparseDoubleValueIterationGmmxxRegularMultEnvironment,
                         SparseDoubleValueIterationNativeGaussSeidelMultEnvironment, SparseDoubleValueIterationNativeRegularMultEnvironment,
                         JaniSparseDoubleValueIterationEnvironment, SparseDoubleIntervalIterationEnvironment, SparseDoubleSoundValueIterationEnvironment,
                         SparseDoubleOptimisticValueIterationEnvironment, SparseRationalPolicyIterationEnvironment, HybridCuddDoubleValueIterationEnvironment>
    TestingTypes;

TYPED_TEST_SUITE(DiscountingMdpPrctlModelCheckerTest, TestingTypes, );

TYPED_TEST(DiscountingMdpPrctlModelCheckerTest, SmallDiscount) {
    std::string formulasString = "Rmax=? [ C ]";
    formulasString += "; Rmax=? [ Cdiscount=9/10 ]";
    formulasString += "; Rmax=? [ Cdiscount=15/16 ]";
    auto modelFormulas = this->buildModelFormulas(STORM_TEST_RESOURCES_DIR "/mdp/small_discount.nm", formulasString);
    auto model = std::move(modelFormulas.first);
    auto tasks = this->getTasks(modelFormulas.second);
    EXPECT_EQ(4ul, model->getNumberOfStates());
    EXPECT_EQ(6ul, model->getNumberOfTransitions());
    ASSERT_EQ(model->getType(), storm::models::ModelType::Mdp);
    auto checker = this->createModelChecker(model);
    std::unique_ptr<storm::modelchecker::CheckResult> result;

    // This example considers a discounted expected total reward formula, which is not supported in all engines
    if (TypeParam::engine == MdpEngine::PrismSparse || TypeParam::engine == MdpEngine::JaniSparse) {
        result = checker->check(this->env(), tasks[0]);
        EXPECT_NEAR(this->parseNumber("5"), this->getQuantitativeResultAtInitialState(model, result), this->precision());
        if (TypeParam::isExact) {
            // Exact solving of discounted expected total reward formulas is not supported yet
            STORM_SILENT_EXPECT_THROW(result = checker->check(this->env(), tasks[1]), storm::exceptions::NotSupportedException);
            STORM_SILENT_EXPECT_THROW(result = checker->check(this->env(), tasks[2]), storm::exceptions::NotSupportedException);
        } else {
            result = checker->check(this->env(), tasks[1]);
            EXPECT_NEAR(this->parseNumber("18/5"), this->getQuantitativeResultAtInitialState(model, result), this->precision());
            result = checker->check(this->env(), tasks[2]);
            EXPECT_NEAR(this->parseNumber("15/4"), this->getQuantitativeResultAtInitialState(model, result), this->precision());
        }
    } else {
        EXPECT_FALSE(checker->canHandle(tasks[0]));
        EXPECT_FALSE(checker->canHandle(tasks[1]));
        EXPECT_FALSE(checker->canHandle(tasks[2]));
    }
}

TYPED_TEST(DiscountingMdpPrctlModelCheckerTest, OneStateDiscounting) {
    std::string formulasString = "Rmax=? [ Cdiscount=9/10 ]";
    formulasString += "; Rmax=? [ C<=5discount=9/10 ]";
    formulasString += "; Rmax=? [ C<=10discount=9/10 ]";
    auto modelFormulas = this->buildModelFormulas(STORM_TEST_RESOURCES_DIR "/mdp/one_state.nm", formulasString);
    auto model = std::move(modelFormulas.first);
    auto tasks = this->getTasks(modelFormulas.second);
    EXPECT_EQ(1ul, model->getNumberOfStates());
    EXPECT_EQ(2ul, model->getNumberOfTransitions());
    ASSERT_EQ(model->getType(), storm::models::ModelType::Mdp);
    auto checker = this->createModelChecker(model);
    std::unique_ptr<storm::modelchecker::CheckResult> result;

    // This example considers discounted reward formulae, which are currently only supported for sparse models
    if ((TypeParam::engine == MdpEngine::PrismSparse || TypeParam::engine == MdpEngine::JaniSparse)) {
        if (TypeParam::isExact) {
            // Exact solving of discounted expected total reward formulas is not supported yet
            STORM_SILENT_EXPECT_THROW(result = checker->check(this->env(), tasks[0]), storm::exceptions::NotSupportedException);
        } else {
            result = checker->check(this->env(), tasks[0]);
            EXPECT_NEAR(this->parseNumber("1000"), this->getQuantitativeResultAtInitialState(model, result), this->precision());
        }
        // Discounted cumulative reward formulae can be solved exactly
        result = checker->check(this->env(), tasks[1]);
        EXPECT_NEAR(this->parseNumber("40951/100"), this->getQuantitativeResultAtInitialState(model, result), this->precision());
        result = checker->check(this->env(), tasks[2]);
        EXPECT_NEAR(this->parseNumber("6513215599/10000000"), this->getQuantitativeResultAtInitialState(model, result), this->precision());
    } else {
        EXPECT_FALSE(checker->canHandle(tasks[0]));
        EXPECT_FALSE(checker->canHandle(tasks[1]));
        EXPECT_FALSE(checker->canHandle(tasks[2]));
    }
}

}  // namespace
