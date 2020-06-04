#include "test/storm_gtest.h"
#include "storm-config.h"

#include "storm-pomdp/modelchecker/BeliefExplorationPomdpModelChecker.h"
#include "storm-pomdp/transformer/MakePOMDPCanonic.h"
#include "storm/api/storm.h"
#include "storm-parsers/api/storm-parsers.h"

#include "storm/environment/solver/MinMaxSolverEnvironment.h"


namespace {
    enum class PreprocessingType { None };
    class CoarseDoubleVIEnvironment {
    public:
        typedef double ValueType;
        static storm::Environment createEnvironment() {
            storm::Environment env;
            env.solver().minMax().setMethod(storm::solver::MinMaxMethod::ValueIteration);
            env.solver().minMax().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-6));
            return env;
        }
        static void adaptOptions(storm::pomdp::modelchecker::BeliefExplorationPomdpModelCheckerOptions<ValueType>& options) {}  // TODO
        static PreprocessingType const preprocessingType = PreprocessingType::None;
    };
    
    class DefaultDoubleVIEnvironment {
    public:
        typedef double ValueType;
        static storm::Environment createEnvironment() {
            storm::Environment env;
            env.solver().minMax().setMethod(storm::solver::MinMaxMethod::ValueIteration);
            env.solver().minMax().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-6));
            return env;
        }
        static void adaptOptions(storm::pomdp::modelchecker::BeliefExplorationPomdpModelCheckerOptions<ValueType>&) { /* intentionally left empty */ }
        static PreprocessingType const preprocessingType = PreprocessingType::None;
    };
    
    class PreprocessedDefaultDoubleVIEnvironment {
    public:
        typedef double ValueType;
        static storm::Environment createEnvironment() {
            storm::Environment env;
            env.solver().minMax().setMethod(storm::solver::MinMaxMethod::ValueIteration);
            env.solver().minMax().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-6));
            return env;
        }
        static void adaptOptions(storm::pomdp::modelchecker::BeliefExplorationPomdpModelCheckerOptions<ValueType>& options) { /* intentionally left empty */ }
        static PreprocessingType const preprocessingType = PreprocessingType::None; // TODO
    };

    class FineDoubleVIEnvironment {
    public:
        typedef double ValueType;
        static storm::Environment createEnvironment() {
            storm::Environment env;
            env.solver().minMax().setMethod(storm::solver::MinMaxMethod::ValueIteration);
            env.solver().minMax().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-6));
            return env;
        }
        static void adaptOptions(storm::pomdp::modelchecker::BeliefExplorationPomdpModelCheckerOptions<ValueType>& options) {}  // TODO
        static PreprocessingType const preprocessingType = PreprocessingType::None;
    };

    class RefineDoubleVIEnvironment {
    public:
        typedef double ValueType;
        static storm::Environment createEnvironment() {
            storm::Environment env;
            env.solver().minMax().setMethod(storm::solver::MinMaxMethod::ValueIteration);
            env.solver().minMax().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-6));
            return env;
        }
        static void adaptOptions(storm::pomdp::modelchecker::BeliefExplorationPomdpModelCheckerOptions<ValueType>& options) {}  // TODO
        static PreprocessingType const preprocessingType = PreprocessingType::None;
    };
    
    class DefaultDoubleOVIEnvironment {
    public:
        typedef double ValueType;
        static storm::Environment createEnvironment() {
            storm::Environment env;
            env.solver().minMax().setMethod(storm::solver::MinMaxMethod::SoundValueIteration);
            env.solver().minMax().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-6));
            env.solver().setForceSoundness(true);
            return env;
        }
        static void adaptOptions(storm::pomdp::modelchecker::BeliefExplorationPomdpModelCheckerOptions<ValueType>& options) { /* intentionally left empty */ }
        static PreprocessingType const preprocessingType = PreprocessingType::None;
    };
    
    class DefaultRationalPIEnvironment {
    public:
        typedef storm::RationalNumber ValueType;
        static storm::Environment createEnvironment() {
            storm::Environment env;
            env.solver().minMax().setMethod(storm::solver::MinMaxMethod::PolicyIteration);
            env.solver().setForceExact(true);
            return env;
        }
        static void adaptOptions(storm::pomdp::modelchecker::BeliefExplorationPomdpModelCheckerOptions<ValueType>& options) { /* intentionally left empty */ }
        static PreprocessingType const preprocessingType = PreprocessingType::None;
    };
    
    class PreprocessedDefaultRationalPIEnvironment {
    public:
        typedef storm::RationalNumber ValueType;
        static storm::Environment createEnvironment() {
            storm::Environment env;
            env.solver().minMax().setMethod(storm::solver::MinMaxMethod::PolicyIteration);
            env.solver().setForceExact(true);
            return env;
        }
        static void adaptOptions(storm::pomdp::modelchecker::BeliefExplorationPomdpModelCheckerOptions<ValueType>& options) { /* intentionally left empty */ }
        static PreprocessingType const preprocessingType = PreprocessingType::None; // TODO
    };
    
    template<typename TestType>
    class BeliefExplorationTest : public ::testing::Test {
    public:
        typedef typename TestType::ValueType ValueType;
        BeliefExplorationTest() : _environment(TestType::createEnvironment()) {}
        storm::Environment const& env() const { return _environment; }
        storm::pomdp::modelchecker::BeliefExplorationPomdpModelCheckerOptions<ValueType> options() const {
            storm::pomdp::modelchecker::BeliefExplorationPomdpModelCheckerOptions<ValueType> opt(true, true); // Always compute both bounds (lower and upper)
            TestType::adaptOptions(opt);
            return opt;
        }
        ValueType parseNumber(std::string const& str) {
            return storm::utility::convertNumber<ValueType>(str);
        }
        struct Input {
            std::shared_ptr<storm::models::sparse::Pomdp<ValueType>> model;
            std::shared_ptr<storm::logic::Formula const> formula;
        };
        Input buildPrism(std::string const& programFile, std::string const& formulaAsString, std::string const& constantsAsString = "") const {
            // Parse and build input
            storm::prism::Program program = storm::api::parseProgram(programFile);
            program = storm::utility::prism::preprocess(program, constantsAsString);
            Input input;
            input.formula = storm::api::parsePropertiesForPrismProgram(formulaAsString, program).front().getRawFormula();
            input.model = storm::api::buildSparseModel<ValueType>(program, {input.formula})->template as<storm::models::sparse::Pomdp<ValueType>>();
            // Preprocess
            storm::transformer::MakePOMDPCanonic<ValueType> makeCanonic(*input.model);
            input.model = makeCanonic.transform();
            EXPECT_TRUE(input.model->isCanonic());
            switch (TestType::preprocessingType) {
                case PreprocessingType::None:
                    // nothing to do
                    break;
                default:
                    ADD_FAILURE() << "Unhandled preprocessing type.";
            }
            EXPECT_TRUE(input.model->isCanonic());
            return input;
        }
        
    private:
        storm::Environment _environment;
    };
  
    typedef ::testing::Types<
            CoarseDoubleVIEnvironment,
            DefaultDoubleVIEnvironment,
            PreprocessedDefaultDoubleVIEnvironment,
            FineDoubleVIEnvironment,
            RefineDoubleVIEnvironment,
            DefaultDoubleOVIEnvironment,
            DefaultRationalPIEnvironment,
            PreprocessedDefaultRationalPIEnvironment
    > TestingTypes;
    
    TYPED_TEST_SUITE(BeliefExplorationTest, TestingTypes,);
    
    TYPED_TEST(BeliefExplorationTest, simple_max) {
        typedef typename TestFixture::ValueType ValueType;

        auto data = this->buildPrism(STORM_TEST_RESOURCES_DIR "/pomdp/simple.prism", "Pmax=? [F \"goal\" ]", "slippery=0");
        storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>> checker(data.model, this->options());
        auto result = checker.check(*data.formula);
        
        EXPECT_LE(result.lowerBound, this->parseNumber("7/10"));
        EXPECT_GE(result.upperBound, this->parseNumber("7/10"));
        
    }
}
