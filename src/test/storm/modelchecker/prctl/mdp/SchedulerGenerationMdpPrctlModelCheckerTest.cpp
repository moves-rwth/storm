#include "test/storm_gtest.h"
#include "storm-config.h"

#include "storm-parsers/parser/FormulaParser.h"
#include "storm/logic/Formulas.h"
#include "storm/solver/StandardMinMaxLinearEquationSolver.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GeneralSettings.h"

#include "storm/environment/solver/MinMaxSolverEnvironment.h"

#include "storm-parsers/parser/AutoParser.h"
#include "storm-parsers/parser/PrismParser.h"
#include "storm/builder/ExplicitModelBuilder.h"

namespace {
    class DoubleViEnvironment {
    public:
        typedef double ValueType;
        static storm::Environment createEnvironment() {
            storm::Environment env;
            env.solver().minMax().setMethod(storm::solver::MinMaxMethod::ValueIteration);
            env.solver().minMax().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-8));
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

        storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/scheduler_generation.nm");
    
        // A parser that we use for conveniently constructing the formulas.
        storm::parser::FormulaParser formulaParser;
        
        storm::generator::NextStateGeneratorOptions options;
        options.setBuildAllLabels();
        std::shared_ptr<storm::models::sparse::Model<ValueType>> model = storm::builder::ExplicitModelBuilder<ValueType>(program, options).build();
        EXPECT_EQ(4ull, model->getNumberOfStates());
        EXPECT_EQ(11ull, model->getNumberOfTransitions());
        
        ASSERT_EQ(model->getType(), storm::models::ModelType::Mdp);
    
        std::shared_ptr<storm::models::sparse::Mdp<ValueType>> mdp = model->template as<storm::models::sparse::Mdp<ValueType>>();
    
        EXPECT_EQ(7ull, mdp->getNumberOfChoices());
    
        storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<ValueType>> checker(*mdp);
        
        std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F \"target\"]");
        
        storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> checkTask(*formula);
        checkTask.setProduceSchedulers(true);
        
        std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(this->env(), checkTask);
        
        ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
        ASSERT_TRUE(result->asExplicitQuantitativeCheckResult<ValueType>().hasScheduler());
        storm::storage::Scheduler<ValueType> const& scheduler = result->asExplicitQuantitativeCheckResult<ValueType>().getScheduler();
        EXPECT_EQ(0ull, scheduler.getChoice(0).getDeterministicChoice());
        EXPECT_EQ(1ull, scheduler.getChoice(1).getDeterministicChoice());
        EXPECT_EQ(0ull, scheduler.getChoice(2).getDeterministicChoice());
        EXPECT_EQ(0ull, scheduler.getChoice(3).getDeterministicChoice());
        
        formula = formulaParser.parseSingleFormulaFromString("Pmax=? [F \"target\"]");
        
        checkTask = storm::modelchecker::CheckTask<storm::logic::Formula, ValueType>(*formula);
        checkTask.setProduceSchedulers(true);
        result = checker.check(checkTask);
    
        ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
        ASSERT_TRUE(result->asExplicitQuantitativeCheckResult<ValueType>().hasScheduler());
        storm::storage::Scheduler<ValueType> const& scheduler2 = result->asExplicitQuantitativeCheckResult<ValueType>().getScheduler();
        EXPECT_EQ(1ull, scheduler2.getChoice(0).getDeterministicChoice());
        EXPECT_EQ(2ull, scheduler2.getChoice(1).getDeterministicChoice());
        EXPECT_EQ(0ull, scheduler2.getChoice(2).getDeterministicChoice());
        EXPECT_EQ(0ull, scheduler2.getChoice(3).getDeterministicChoice());
    }
}