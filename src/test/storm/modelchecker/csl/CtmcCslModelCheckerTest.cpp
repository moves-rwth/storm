#include "test/storm_gtest.h"
#include "test/storm_gtest.h"
#include "storm-config.h"

#include "storm/api/builder.h"
#include "storm-parsers/api/model_descriptions.h"
#include "storm/api/properties.h"
#include "storm-conv/api/storm-conv.h"
#include "storm-parsers/api/properties.h"
#include "storm-parsers/parser/FormulaParser.h"
#include "storm/logic/Formulas.h"
#include "storm/solver/EigenLinearEquationSolver.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/models/symbolic/StandardRewardModel.h"
#include "storm/models/sparse/Ctmc.h"
#include "storm/models/symbolic/Ctmc.h"
#include "storm/modelchecker/csl/SparseCtmcCslModelChecker.h"
#include "storm/modelchecker/csl/HybridCtmcCslModelChecker.h"
#include "storm/modelchecker/csl/helper/SparseCtmcCslHelper.h"
#include "storm/modelchecker/results/QuantitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/modelchecker/results/SymbolicQualitativeCheckResult.h"
#include "storm/modelchecker/results/QualitativeCheckResult.h"
#include "storm-parsers/parser/PrismParser.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/environment/solver/NativeSolverEnvironment.h"
#include "storm/environment/solver/GmmxxSolverEnvironment.h"
#include "storm/environment/solver/EigenSolverEnvironment.h"

namespace {
    
    enum class CtmcEngine {PrismSparse, JaniSparse, JitSparse, PrismHybrid, JaniHybrid};
    
    class SparseGmmxxGmresIluEnvironment {
    public:
        static const storm::dd::DdType ddType = storm::dd::DdType::Sylvan; // unused for sparse models
        static const CtmcEngine engine = CtmcEngine::PrismSparse;
        static const bool isExact = false;
        typedef double ValueType;
        typedef storm::models::sparse::Ctmc<ValueType> ModelType;
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
        static const storm::dd::DdType ddType = storm::dd::DdType::Sylvan; // unused for sparse models
        static const CtmcEngine engine = CtmcEngine::JaniSparse;
        static const bool isExact = false;
        typedef double ValueType;
        typedef storm::models::sparse::Ctmc<ValueType> ModelType;
        static storm::Environment createEnvironment() {
            storm::Environment env;
            env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Gmmxx);
            env.solver().gmmxx().setMethod(storm::solver::GmmxxLinearEquationSolverMethod::Gmres);
            env.solver().gmmxx().setPreconditioner(storm::solver::GmmxxLinearEquationSolverPreconditioner::Ilu);
            env.solver().gmmxx().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-8));
            return env;
        }
    };
    
    class JitSparseGmmxxGmresIluEnvironment {
    public:
        static const storm::dd::DdType ddType = storm::dd::DdType::Sylvan; // unused for sparse models
        static const CtmcEngine engine = CtmcEngine::JitSparse;
        static const bool isExact = false;
        typedef double ValueType;
        typedef storm::models::sparse::Ctmc<ValueType> ModelType;
        static storm::Environment createEnvironment() {
            storm::Environment env;
            env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Gmmxx);
            env.solver().gmmxx().setMethod(storm::solver::GmmxxLinearEquationSolverMethod::Gmres);
            env.solver().gmmxx().setPreconditioner(storm::solver::GmmxxLinearEquationSolverPreconditioner::Ilu);
            env.solver().gmmxx().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-8));
            return env;
        }
    };
    
    class SparseEigenDGmresEnvironment {
    public:
        static const storm::dd::DdType ddType = storm::dd::DdType::Sylvan; // unused for sparse models
        static const CtmcEngine engine = CtmcEngine::PrismSparse;
        static const bool isExact = false;
        typedef double ValueType;
        typedef storm::models::sparse::Ctmc<ValueType> ModelType;
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
        static const storm::dd::DdType ddType = storm::dd::DdType::Sylvan; // unused for sparse models
        static const CtmcEngine engine = CtmcEngine::PrismSparse;
        static const bool isExact = false;
        typedef double ValueType;
        typedef storm::models::sparse::Ctmc<ValueType> ModelType;
        static storm::Environment createEnvironment() {
            storm::Environment env;
            env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Eigen);
            env.solver().eigen().setMethod(storm::solver::EigenLinearEquationSolverMethod::SparseLU);
            return env;
        }
    };
    
    class SparseNativeSorEnvironment {
    public:
        static const storm::dd::DdType ddType = storm::dd::DdType::Sylvan; // unused for sparse models
        static const CtmcEngine engine = CtmcEngine::PrismSparse;
        static const bool isExact = false;
        typedef double ValueType;
        typedef storm::models::sparse::Ctmc<ValueType> ModelType;
        static storm::Environment createEnvironment() {
            storm::Environment env;
            env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Native);
            env.solver().native().setMethod(storm::solver::NativeLinearEquationSolverMethod::SOR);
            env.solver().native().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-9));
            env.solver().native().setRelativeTerminationCriterion(false);
            env.solver().native().setMaximalNumberOfIterations(5000000);
            return env;
        }
    };

    class HybridCuddGmmxxGmresEnvironment {
    public:
        static const storm::dd::DdType ddType = storm::dd::DdType::CUDD;
        static const CtmcEngine engine = CtmcEngine::PrismHybrid;
        static const bool isExact = false;
        typedef double ValueType;
        typedef storm::models::symbolic::Ctmc<ddType, ValueType> ModelType;
        static storm::Environment createEnvironment() {
            storm::Environment env;
            env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Gmmxx);
            env.solver().gmmxx().setMethod(storm::solver::GmmxxLinearEquationSolverMethod::Gmres);
            env.solver().gmmxx().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-8));
            return env;
        }
    };
    
    class JaniHybridCuddGmmxxGmresEnvironment {
    public:
        static const storm::dd::DdType ddType = storm::dd::DdType::CUDD;
        static const CtmcEngine engine = CtmcEngine::JaniHybrid;
        static const bool isExact = false;
        typedef double ValueType;
        typedef storm::models::symbolic::Ctmc<ddType, ValueType> ModelType;
        static storm::Environment createEnvironment() {
            storm::Environment env;
            env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Gmmxx);
            env.solver().gmmxx().setMethod(storm::solver::GmmxxLinearEquationSolverMethod::Gmres);
            env.solver().gmmxx().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-8));
            return env;
        }
    };
    
    class HybridSylvanGmmxxGmresEnvironment {
    public:
        static const storm::dd::DdType ddType = storm::dd::DdType::Sylvan;
        static const CtmcEngine engine = CtmcEngine::PrismHybrid;
        static const bool isExact = false;
        typedef double ValueType;
        typedef storm::models::symbolic::Ctmc<ddType, ValueType> ModelType;
        static storm::Environment createEnvironment() {
            storm::Environment env;
            env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Gmmxx);
            env.solver().gmmxx().setMethod(storm::solver::GmmxxLinearEquationSolverMethod::Gmres);
            env.solver().gmmxx().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-8));
            return env;
        }
    };
    
    template<typename TestType>
    class CtmcCslModelCheckerTest : public ::testing::Test {
    public:
        typedef typename TestType::ValueType ValueType;
        typedef typename storm::models::sparse::Ctmc<ValueType> SparseModelType;
        typedef typename storm::models::symbolic::Ctmc<TestType::ddType, ValueType> SymbolicModelType;
        
        CtmcCslModelCheckerTest() : _environment(TestType::createEnvironment()) {}
        storm::Environment const& env() const { return _environment; }
        ValueType parseNumber(std::string const& input) const { return storm::utility::convertNumber<ValueType>(input);}
        ValueType precision() const { return TestType::isExact ? parseNumber("0") : parseNumber("1e-6");}
        bool isSparseModel() const { return std::is_same<typename TestType::ModelType, SparseModelType>::value; }
        bool isSymbolicModel() const { return std::is_same<typename TestType::ModelType, SymbolicModelType>::value; }
        CtmcEngine getEngine() const { return TestType::engine; }
        
        template <typename MT = typename TestType::ModelType>
        typename std::enable_if<std::is_same<MT, SparseModelType>::value, std::pair<std::shared_ptr<MT>, std::vector<std::shared_ptr<storm::logic::Formula const>>>>::type
        buildModelFormulas(std::string const& pathToPrismFile, std::string const& formulasAsString, std::string const& constantDefinitionString = "") const {
            std::pair<std::shared_ptr<MT>, std::vector<std::shared_ptr<storm::logic::Formula const>>> result;
            storm::prism::Program program = storm::api::parseProgram(pathToPrismFile, true);
            program = storm::utility::prism::preprocess(program, constantDefinitionString);
            if (TestType::engine == CtmcEngine::PrismSparse) {
                result.second = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulasAsString, program));
                result.first = storm::api::buildSparseModel<ValueType>(program, result.second)->template as<MT>();
            } else if (TestType::engine == CtmcEngine::JaniSparse || TestType::engine == CtmcEngine::JitSparse) {
                auto janiData = storm::api::convertPrismToJani(program, storm::api::parsePropertiesForPrismProgram(formulasAsString, program));
                janiData.first.substituteFunctions();
                result.second = storm::api::extractFormulasFromProperties(janiData.second);
                result.first = storm::api::buildSparseModel<ValueType>(janiData.first, result.second, TestType::engine == CtmcEngine::JitSparse)->template as<MT>();
            }
            return result;
        }
        
        template <typename MT = typename TestType::ModelType>
        typename std::enable_if<std::is_same<MT, SymbolicModelType>::value, std::pair<std::shared_ptr<MT>, std::vector<std::shared_ptr<storm::logic::Formula const>>>>::type
        buildModelFormulas(std::string const& pathToPrismFile, std::string const& formulasAsString, std::string const& constantDefinitionString = "") const {
            std::pair<std::shared_ptr<MT>, std::vector<std::shared_ptr<storm::logic::Formula const>>> result;
            storm::prism::Program program = storm::api::parseProgram(pathToPrismFile, true);
            program = storm::utility::prism::preprocess(program, constantDefinitionString);
            if (TestType::engine == CtmcEngine::PrismHybrid) {
                result.second = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulasAsString, program));
                result.first = storm::api::buildSymbolicModel<TestType::ddType, ValueType>(program, result.second)->template as<MT>();
            } else if (TestType::engine == CtmcEngine::JaniHybrid) {
                auto janiData = storm::api::convertPrismToJani(program, storm::api::parsePropertiesForPrismProgram(formulasAsString, program));
                janiData.first.substituteFunctions();
                result.second = storm::api::extractFormulasFromProperties(janiData.second);
                result.first = storm::api::buildSymbolicModel<TestType::ddType, ValueType>(janiData.first, result.second)->template as<MT>();
            }
            return result;
        }
        
        std::vector<storm::modelchecker::CheckTask<storm::logic::Formula, ValueType>> getTasks(std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas) const {
            std::vector<storm::modelchecker::CheckTask<storm::logic::Formula, ValueType>> result;
            for (auto const& f : formulas) {
                result.emplace_back(*f);
            }
            return result;
        }
        
        template <typename MT = typename TestType::ModelType>
        typename std::enable_if<std::is_same<MT, SparseModelType>::value, std::shared_ptr<storm::modelchecker::AbstractModelChecker<MT>>>::type
        createModelChecker(std::shared_ptr<MT> const& model) const {
            if (TestType::engine == CtmcEngine::PrismSparse || TestType::engine == CtmcEngine::JaniSparse || TestType::engine == CtmcEngine::JitSparse) {
                return std::make_shared<storm::modelchecker::SparseCtmcCslModelChecker<SparseModelType>>(*model);
            }
        }
        
        template <typename MT = typename TestType::ModelType>
        typename std::enable_if<std::is_same<MT, SymbolicModelType>::value, std::shared_ptr<storm::modelchecker::AbstractModelChecker<MT>>>::type
        createModelChecker(std::shared_ptr<MT> const& model) const {
            if (TestType::engine == CtmcEngine::PrismHybrid || TestType::engine == CtmcEngine::JaniHybrid) {
                return std::make_shared<storm::modelchecker::HybridCtmcCslModelChecker<SymbolicModelType>>(*model);
            }
        }
        
        bool getQualitativeResultAtInitialState(std::shared_ptr<storm::models::Model<ValueType>> const& model, std::unique_ptr<storm::modelchecker::CheckResult>& result) {
            auto filter = getInitialStateFilter(model);
            result->filter(*filter);
            return result->asQualitativeCheckResult().forallTrue();
        }
        
        ValueType getQuantitativeResultAtInitialState(std::shared_ptr<storm::models::Model<ValueType>> const& model, std::unique_ptr<storm::modelchecker::CheckResult>& result) {
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
                return std::make_unique<storm::modelchecker::SymbolicQualitativeCheckResult<TestType::ddType>>(model->template as<SymbolicModelType>()->getReachableStates(), model->template as<SymbolicModelType>()->getInitialStates());
            }
        }
    };
  
    typedef ::testing::Types<
            SparseGmmxxGmresIluEnvironment,
            JaniSparseGmmxxGmresIluEnvironment,
            JitSparseGmmxxGmresIluEnvironment,
            SparseEigenDGmresEnvironment,
            SparseEigenDoubleLUEnvironment,
            SparseNativeSorEnvironment,
            HybridCuddGmmxxGmresEnvironment,
            JaniHybridCuddGmmxxGmresEnvironment,
            HybridSylvanGmmxxGmresEnvironment
        > TestingTypes;
    
    TYPED_TEST_SUITE(CtmcCslModelCheckerTest, TestingTypes,);

    TYPED_TEST(CtmcCslModelCheckerTest, Cluster) {
        std::string formulasString = "P=? [ F<=100 !\"minimum\"]";
        formulasString += "; P=? [ F[100,100] !\"minimum\"]";
        formulasString += "; P=? [ F[100,2000] !\"minimum\"]";
        formulasString += "; P=? [ \"minimum\" U<=10 \"premium\"]";
        formulasString += "; P=? [ !\"minimum\" U>=1 \"minimum\"]";
        formulasString += "; P=? [ \"minimum\" U>=1 !\"minimum\"]";
        formulasString += "; R=? [C<=100]";
        
        auto modelFormulas = this->buildModelFormulas(STORM_TEST_RESOURCES_DIR "/ctmc/cluster2.sm", formulasString);
        auto model = std::move(modelFormulas.first);
        auto tasks = this->getTasks(modelFormulas.second);
        EXPECT_EQ(276ul, model->getNumberOfStates());
        EXPECT_EQ(1120ul, model->getNumberOfTransitions());
        ASSERT_EQ(model->getType(), storm::models::ModelType::Ctmc);
        auto checker = this->createModelChecker(model);
        std::unique_ptr<storm::modelchecker::CheckResult> result;
        
        result = checker->check(this->env(), tasks[0]);
        EXPECT_NEAR(this->parseNumber("5.5461254704419085E-5"), this->getQuantitativeResultAtInitialState(model, result), this->precision());
        
        result = checker->check(this->env(), tasks[1]);
        EXPECT_NEAR(this->parseNumber("2.3397873548343415E-6"), this->getQuantitativeResultAtInitialState(model, result), this->precision());
        
        result = checker->check(this->env(), tasks[2]);
        EXPECT_NEAR(this->parseNumber("0.001105335651670241"), this->getQuantitativeResultAtInitialState(model, result), this->precision());
        
        result = checker->check(this->env(), tasks[3]);
        EXPECT_NEAR(this->parseNumber("1"), this->getQuantitativeResultAtInitialState(model, result), this->precision());
        
        result = checker->check(this->env(), tasks[4]);
        EXPECT_NEAR(this->parseNumber("0"), this->getQuantitativeResultAtInitialState(model, result), this->precision());
        
        result = checker->check(this->env(), tasks[5]);
        EXPECT_NEAR(this->parseNumber("0.9999999033633374"), this->getQuantitativeResultAtInitialState(model, result), this->precision());
        
        result = checker->check(this->env(), tasks[6]);
        EXPECT_NEAR(this->parseNumber("0.8602815057967503"), this->getQuantitativeResultAtInitialState(model, result), this->precision());
    
    }
    
    TYPED_TEST(CtmcCslModelCheckerTest, Embedded) {
        std::string formulasString = "P=? [ F<=10000 \"down\"]";
        formulasString += "; P=? [ !\"down\" U<=10000 \"fail_actuators\"]";
        formulasString += "; P=? [ !\"down\" U<=10000 \"fail_io\"]";
        formulasString += "; P=? [ !\"down\" U<=10000 \"fail_sensors\"]";
        formulasString += "; R{\"up\"}=? [C<=10000]";
        
        auto modelFormulas = this->buildModelFormulas(STORM_TEST_RESOURCES_DIR "/ctmc/embedded2.sm", formulasString);
        auto model = std::move(modelFormulas.first);
        auto tasks = this->getTasks(modelFormulas.second);
        EXPECT_EQ(3478ul, model->getNumberOfStates());
        EXPECT_EQ(14639ul, model->getNumberOfTransitions());
        ASSERT_EQ(model->getType(), storm::models::ModelType::Ctmc);
        auto checker = this->createModelChecker(model);
        std::unique_ptr<storm::modelchecker::CheckResult> result;
        
        result = checker->check(this->env(), tasks[0]);
        EXPECT_NEAR(this->parseNumber("0.0019216435246119591"), this->getQuantitativeResultAtInitialState(model, result), this->precision());
        
        result = checker->check(this->env(), tasks[1]);
        EXPECT_NEAR(this->parseNumber("3.7079151806696567E-6"), this->getQuantitativeResultAtInitialState(model, result), this->precision());
        
        result = checker->check(this->env(), tasks[2]);
        EXPECT_NEAR(this->parseNumber("0.001556839327673734"), this->getQuantitativeResultAtInitialState(model, result), this->precision());
        
        result = checker->check(this->env(), tasks[3]);
        EXPECT_NEAR(this->parseNumber("4.429620626755424E-5"), this->getQuantitativeResultAtInitialState(model, result), this->precision());
        
        result = checker->check(this->env(), tasks[4]);
        EXPECT_NEAR(this->parseNumber("2.7745274082080154"), this->getQuantitativeResultAtInitialState(model, result), this->precision());
        
    }
    
    TYPED_TEST(CtmcCslModelCheckerTest, Tandem) {
        std::string formulasString = "P=? [ F<=10 \"network_full\" ]";
        formulasString += "; P=? [ F<=10 \"first_queue_full\" ]";
        formulasString += "; P=? [\"second_queue_full\" U<=1 !\"second_queue_full\"]";
        formulasString += "; R=? [I=10]";
        formulasString += "; R=? [C<=10]";
        formulasString += "; R=? [F \"first_queue_full\"&\"second_queue_full\"]";
        
        auto modelFormulas = this->buildModelFormulas(STORM_TEST_RESOURCES_DIR "/ctmc/tandem5.sm", formulasString);
        auto model = std::move(modelFormulas.first);
        auto tasks = this->getTasks(modelFormulas.second);
        EXPECT_EQ(66ul, model->getNumberOfStates());
        EXPECT_EQ(189ul, model->getNumberOfTransitions());
        ASSERT_EQ(model->getType(), storm::models::ModelType::Ctmc);
        auto checker = this->createModelChecker(model);
        std::unique_ptr<storm::modelchecker::CheckResult> result;
        
        result = checker->check(this->env(), tasks[0]);
        EXPECT_NEAR(this->parseNumber("0.015446370562428037"), this->getQuantitativeResultAtInitialState(model, result), this->precision());
        
        result = checker->check(this->env(), tasks[1]);
        EXPECT_NEAR(this->parseNumber("0.999999837225515"), this->getQuantitativeResultAtInitialState(model, result), this->precision());
        
        result = checker->check(this->env(), tasks[2]);
        EXPECT_NEAR(this->parseNumber("1"), this->getQuantitativeResultAtInitialState(model, result), this->precision());
        
        result = checker->check(this->env(), tasks[3]);
        EXPECT_NEAR(this->parseNumber("5.679243850315877"), this->getQuantitativeResultAtInitialState(model, result), this->precision());
        
        result = checker->check(this->env(), tasks[4]);
        EXPECT_NEAR(this->parseNumber("55.44792186036232"), this->getQuantitativeResultAtInitialState(model, result), this->precision());
        
        result = checker->check(this->env(), tasks[5]);
        EXPECT_NEAR(this->parseNumber("262.85103824276308"), this->getQuantitativeResultAtInitialState(model, result), this->precision());
    }
    
    TYPED_TEST(CtmcCslModelCheckerTest, simple2) {
        std::string formulasString = "R{\"rew1\"}=? [ C ]";
        formulasString += "; R{\"rew2\"}=? [ C ]";
        
        auto modelFormulas = this->buildModelFormulas(STORM_TEST_RESOURCES_DIR "/ctmc/simple2.sm", formulasString);
        auto model = std::move(modelFormulas.first);
        auto tasks = this->getTasks(modelFormulas.second);
        EXPECT_EQ(5ul, model->getNumberOfStates());
        EXPECT_EQ(8ul, model->getNumberOfTransitions());
        ASSERT_EQ(model->getType(), storm::models::ModelType::Ctmc);
        auto checker = this->createModelChecker(model);
        std::unique_ptr<storm::modelchecker::CheckResult> result;
        
        // Total reward formulas are currently not supported for non-sparse models.
        if (this->isSparseModel()) {
            result = checker->check(this->env(), tasks[0]);
            EXPECT_NEAR(this->parseNumber("23/8"), this->getQuantitativeResultAtInitialState(model, result), this->precision());
            
            result = checker->check(this->env(), tasks[1]);
            EXPECT_TRUE(storm::utility::isInfinity(this->getQuantitativeResultAtInitialState(model, result)));
        }
        
    }

    TEST(CtmcCslModelCheckerTest, TransientProbabilities) {
        // Example from lecture
        storm::storage::SparseMatrixBuilder<double> matrixBuilder;
        matrixBuilder.addNextValue(0, 1, 3.0);
        matrixBuilder.addNextValue(1, 0, 2.0);
        storm::storage::SparseMatrix<double> matrix = matrixBuilder.build();

        std::vector<double> exitRates = {3, 2};
        storm::storage::BitVector initialStates(2);
        initialStates.set(0);
        storm::storage::BitVector phiStates(2);
        storm::storage::BitVector psiStates(2);
        storm::Environment env;
        std::vector<double> result = storm::modelchecker::helper::SparseCtmcCslHelper::computeAllTransientProbabilities(env, matrix, initialStates, phiStates, psiStates, exitRates, 1);

        EXPECT_NEAR(0.404043, result[0], 1e-6);
        EXPECT_NEAR(0.595957, result[1], 1e-6);
    }
}
