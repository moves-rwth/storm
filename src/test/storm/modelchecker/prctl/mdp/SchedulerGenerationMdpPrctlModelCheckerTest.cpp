#include "test/storm_gtest.h"
#include "storm-config.h"

#include "storm/api/builder.h"
#include "storm-conv/api/storm-conv.h"
#include "storm-parsers/api/model_descriptions.h"
#include "storm/api/properties.h"
#include "storm-parsers/api/properties.h"


#include "storm/logic/Formulas.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GeneralSettings.h"

#include "storm/environment/solver/MinMaxSolverEnvironment.h"
#include "storm/environment/solver/LongRunAverageSolverEnvironment.h"

namespace {
    class DoubleViEnvironment {
    public:
        typedef double ValueType;
        static storm::Environment createEnvironment() {
            storm::Environment env;
            env.solver().minMax().setMethod(storm::solver::MinMaxMethod::ValueIteration);
            env.solver().minMax().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-8));
            env.solver().lra().setNondetLraMethod(storm::solver::LraMethod::ValueIteration);
            return env;
        }
    };
    class DoubleSoundViEnvironment {
    public:
        typedef double ValueType;
        static storm::Environment createEnvironment() {
            storm::Environment env;
            env.solver().minMax().setMethod(storm::solver::MinMaxMethod::ValueIteration);
            env.solver().setForceSoundness(true);
            env.solver().minMax().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-6));
            env.solver().lra().setNondetLraMethod(storm::solver::LraMethod::ValueIteration);
            return env;
        }
    };
    class DoublePIEnvironment {
    public:
        typedef double ValueType;
        static storm::Environment createEnvironment() {
            storm::Environment env;
            env.solver().minMax().setMethod(storm::solver::MinMaxMethod::PolicyIteration);
            env.solver().minMax().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-8));
            env.solver().lra().setNondetLraMethod(storm::solver::LraMethod::ValueIteration);
            return env;
        }
    };
    class RationalPIEnvironment {
    public:
        typedef storm::RationalNumber ValueType;
        static storm::Environment createEnvironment() {
            storm::Environment env;
            env.solver().minMax().setMethod(storm::solver::MinMaxMethod::PolicyIteration);
            return env;
        }
    };
 //   class RationalRationalSearchEnvironment {
 //   public:
 //       typedef storm::RationalNumber ValueType;
 //       static storm::Environment createEnvironment() {
 //           storm::Environment env;
 //           env.solver().minMax().setMethod(storm::solver::MinMaxMethod::RationalSearch);
 //           return env;
 //       }
 //   };
    
    template<typename TestType>
    class SchedulerGenerationMdpPrctlModelCheckerTest : public ::testing::Test {
    public:
        typedef typename TestType::ValueType ValueType;
        SchedulerGenerationMdpPrctlModelCheckerTest() : _environment(TestType::createEnvironment()) {}
        storm::Environment const& env() const { return _environment; }
        
        std::pair<std::shared_ptr<storm::models::sparse::Mdp<ValueType>>, std::vector<std::shared_ptr<storm::logic::Formula const>>> buildModelFormulas(std::string const& pathToPrismFile, std::string const& formulasAsString, std::string const& constantDefinitionString = "") const {
            std::pair<std::shared_ptr<storm::models::sparse::Mdp<ValueType>>, std::vector<std::shared_ptr<storm::logic::Formula const>>> result;
            storm::prism::Program program = storm::api::parseProgram(pathToPrismFile);
            program = storm::utility::prism::preprocess(program, constantDefinitionString);
            result.second = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulasAsString, program));
            result.first = storm::api::buildSparseModel<ValueType>(program, result.second)->template as<storm::models::sparse::Mdp<ValueType>>();
            return result;
        }
        
        std::vector<storm::modelchecker::CheckTask<storm::logic::Formula, ValueType>> getTasks(std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas) const {
            std::vector<storm::modelchecker::CheckTask<storm::logic::Formula, ValueType>> result;
            for (auto const& f : formulas) {
                result.emplace_back(*f);
                result.back().setProduceSchedulers(true);
            }
            return result;
        }
        
        ValueType parseNumber(std::string const& input) const { return storm::utility::convertNumber<ValueType>(input);}
        
    private:
        storm::Environment _environment;
    };
  
    typedef ::testing::Types<
            DoubleViEnvironment,
            DoubleSoundViEnvironment,
            DoublePIEnvironment,
            RationalPIEnvironment
            //RationalRationalSearchEnvironment
    > TestingTypes;
    
   TYPED_TEST_SUITE(SchedulerGenerationMdpPrctlModelCheckerTest, TestingTypes,);

    
    TYPED_TEST(SchedulerGenerationMdpPrctlModelCheckerTest, reachability) {
        typedef typename TestFixture::ValueType ValueType;

        std::string formulasString = "Pmin=? [F \"target\"]; Pmax=? [F \"target\"];";
        auto modelFormulas = this->buildModelFormulas(STORM_TEST_RESOURCES_DIR "/mdp/scheduler_generation.nm", formulasString);
        auto mdp = std::move(modelFormulas.first);
        auto tasks = this->getTasks(modelFormulas.second);
        EXPECT_EQ(4ull, mdp->getNumberOfStates());
        EXPECT_EQ(11ull, mdp->getNumberOfTransitions());
        ASSERT_EQ(mdp->getType(), storm::models::ModelType::Mdp);
        EXPECT_EQ(7ull, mdp->getNumberOfChoices());
    
        storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<ValueType>> checker(*mdp);
        
        auto result = checker.check(this->env(), tasks[0]);
        ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
        ASSERT_TRUE(result->template asExplicitQuantitativeCheckResult<ValueType>().hasScheduler());
        storm::storage::Scheduler<ValueType> const& scheduler = result->template asExplicitQuantitativeCheckResult<ValueType>().getScheduler();
        EXPECT_EQ(0ull, scheduler.getChoice(0).getDeterministicChoice());
        EXPECT_EQ(1ull, scheduler.getChoice(1).getDeterministicChoice());
        EXPECT_EQ(0ull, scheduler.getChoice(2).getDeterministicChoice());
        EXPECT_EQ(0ull, scheduler.getChoice(3).getDeterministicChoice());
        
        result = checker.check(tasks[1]);
        ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
        ASSERT_TRUE(result->template asExplicitQuantitativeCheckResult<ValueType>().hasScheduler());
        storm::storage::Scheduler<ValueType> const& scheduler2 = result->template asExplicitQuantitativeCheckResult<ValueType>().getScheduler();
        EXPECT_EQ(1ull, scheduler2.getChoice(0).getDeterministicChoice());
        EXPECT_EQ(2ull, scheduler2.getChoice(1).getDeterministicChoice());
        EXPECT_EQ(0ull, scheduler2.getChoice(2).getDeterministicChoice());
        EXPECT_EQ(0ull, scheduler2.getChoice(3).getDeterministicChoice());
    }
    
    TYPED_TEST(SchedulerGenerationMdpPrctlModelCheckerTest, lra) {
        typedef typename TestFixture::ValueType ValueType;

        std::string formulasString = "R{\"grants\"}max=? [ MP ]; R{\"grants\"}min=? [ MP ];";
        
        auto modelFormulas = this->buildModelFormulas(STORM_TEST_RESOURCES_DIR "/mdp/cs_nfail3.nm", formulasString);
        auto mdp = std::move(modelFormulas.first);
        auto tasks = this->getTasks(modelFormulas.second);
        EXPECT_EQ(184ul, mdp->getNumberOfStates());
        EXPECT_EQ(541ul, mdp->getNumberOfTransitions());
        EXPECT_EQ(439ul, mdp->getNumberOfChoices());
        ASSERT_EQ(mdp->getType(), storm::models::ModelType::Mdp);
        storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<ValueType>> checker(*mdp);
        
        if (!std::is_same<ValueType, double>::value) {
            GTEST_SKIP() << "Lra scheduler extraction not supported for LP based method";
        }
        {
            auto result = checker.check(this->env(), tasks[0]);
            ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
            EXPECT_NEAR(this->parseNumber("333/1000"), result->template asExplicitQuantitativeCheckResult<ValueType>()[*mdp->getInitialStates().begin()], this->env().solver().lra().getPrecision());
            ASSERT_TRUE(result->template asExplicitQuantitativeCheckResult<ValueType>().hasScheduler());
            storm::storage::Scheduler<ValueType> const& scheduler = result->template asExplicitQuantitativeCheckResult<ValueType>().getScheduler();
            EXPECT_TRUE(scheduler.isDeterministicScheduler());
            EXPECT_TRUE(scheduler.isMemorylessScheduler());
            EXPECT_TRUE(!scheduler.isPartialScheduler());
            auto inducedModel = mdp->applyScheduler(scheduler);
            ASSERT_EQ(inducedModel->getType(), storm::models::ModelType::Mdp);
            auto const& inducedMdp = inducedModel->template as<storm::models::sparse::Mdp<ValueType>>();
            EXPECT_EQ(inducedMdp->getNumberOfChoices(), inducedMdp->getNumberOfStates());
            storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<ValueType>> inducedChecker(*inducedMdp);
            auto inducedResult = inducedChecker.check(this->env(), tasks[0]);
            ASSERT_TRUE(inducedResult->isExplicitQuantitativeCheckResult());
            EXPECT_NEAR(this->parseNumber("333/1000"), inducedResult->template asExplicitQuantitativeCheckResult<ValueType>()[*mdp->getInitialStates().begin()], this->env().solver().lra().getPrecision());
        }
        {
            auto result = checker.check(this->env(), tasks[1]);
            ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
            EXPECT_NEAR(this->parseNumber("0"), result->template asExplicitQuantitativeCheckResult<ValueType>()[*mdp->getInitialStates().begin()], this->env().solver().lra().getPrecision());
            ASSERT_TRUE(result->template asExplicitQuantitativeCheckResult<ValueType>().hasScheduler());
            storm::storage::Scheduler<ValueType> const& scheduler = result->template asExplicitQuantitativeCheckResult<ValueType>().getScheduler();
            EXPECT_TRUE(scheduler.isDeterministicScheduler());
            EXPECT_TRUE(scheduler.isMemorylessScheduler());
            EXPECT_TRUE(!scheduler.isPartialScheduler());
            auto inducedModel = mdp->applyScheduler(scheduler);
            ASSERT_EQ(inducedModel->getType(), storm::models::ModelType::Mdp);
            auto const& inducedMdp = inducedModel->template as<storm::models::sparse::Mdp<ValueType>>();
            EXPECT_EQ(inducedMdp->getNumberOfChoices(), inducedMdp->getNumberOfStates());
            storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<ValueType>> inducedChecker(*inducedMdp);
            auto inducedResult = inducedChecker.check(this->env(), tasks[1]);
            ASSERT_TRUE(inducedResult->isExplicitQuantitativeCheckResult());
            EXPECT_NEAR(this->parseNumber("0"), inducedResult->template asExplicitQuantitativeCheckResult<ValueType>()[*mdp->getInitialStates().begin()], this->env().solver().lra().getPrecision());
        }
    }
    
}