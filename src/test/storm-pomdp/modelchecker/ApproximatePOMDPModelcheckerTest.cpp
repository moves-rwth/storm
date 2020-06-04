#include "test/storm_gtest.h"
#include "storm-config.h"

#include "storm-pomdp/modelchecker/ApproximatePOMDPModelchecker.h"
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
        static void adaptOptions(storm::pomdp::modelchecker::ApproximatePOMDPModelCheckerOptions<ValueType>& options) {}  // TODO
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
        static void adaptOptions(storm::pomdp::modelchecker::ApproximatePOMDPModelCheckerOptions<ValueType>&) { /* intentionally left empty */ }
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
        static void adaptOptions(storm::pomdp::modelchecker::ApproximatePOMDPModelCheckerOptions<ValueType>& options) { /* intentionally left empty */ }
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
        static void adaptOptions(storm::pomdp::modelchecker::ApproximatePOMDPModelCheckerOptions<ValueType>& options) {}  // TODO
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
        static void adaptOptions(storm::pomdp::modelchecker::ApproximatePOMDPModelCheckerOptions<ValueType>& options) {}  // TODO
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
        static void adaptOptions(storm::pomdp::modelchecker::ApproximatePOMDPModelCheckerOptions<ValueType>& options) { /* intentionally left empty */ }
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
        static void adaptOptions(storm::pomdp::modelchecker::ApproximatePOMDPModelCheckerOptions<ValueType>& options) { /* intentionally left empty */ }
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
        static void adaptOptions(storm::pomdp::modelchecker::ApproximatePOMDPModelCheckerOptions<ValueType>& options) { /* intentionally left empty */ }
        static PreprocessingType const preprocessingType = PreprocessingType::None; // TODO
    };
    
    template<typename TestType>
    class BeliefExplorationTest : public ::testing::Test {
    public:
        typedef typename TestType::ValueType ValueType;
        BeliefExplorationTest() : _environment(TestType::createEnvironment()) {}
        storm::Environment const& env() const { return _environment; }
        storm::pomdp::modelchecker::ApproximatePOMDPModelCheckerOptions<ValueType> options() const {
            storm::pomdp::modelchecker::ApproximatePOMDPModelCheckerOptions<ValueType> opt(true, true); // Always compute both bounds (lower and upper)
            TestType::adaptOptions(opt);
            return opt;
        }
        void preprocess(std::shared_ptr<storm::models::sparse::Pomdp<ValueType>>& model, std::shared_ptr<const storm::logic::Formula>& formula) {
            switch(TestType::preprocessingType) {
                case PreprocessingType::None:
                    // nothing to do
                    break;
                default:
                    FAIL() << "Unhandled preprocessing type.";
            }
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
    
    TYPED_TEST(BeliefExplorationTest, simple) {
        typedef typename TestFixture::ValueType ValueType;

        std::string programFile = STORM_TEST_RESOURCES_DIR "/pomdp/simple.prism";
        std::string formulaAsString = "P=? [F \"goal\" ]";
        std::string constantsAsString = ""; //e.g. pL=0.9,TOACK=0.5
    
        // Program and formula
        storm::prism::Program program = storm::api::parseProgram(programFile);
        program = storm::utility::prism::preprocess(program, constantsAsString);
        std::shared_ptr<const storm::logic::Formula> formula = storm::api::parsePropertiesForPrismProgram(formulaAsString, program).front().getRawFormula();
        std::shared_ptr<storm::models::sparse::Pomdp<ValueType>> model = storm::api::buildSparseModel<ValueType>(program, {formula})->template as<storm::models::sparse::Pomdp<ValueType>>();
        this->preprocess(model, formula);
        
        // Invoke model checking
        storm::pomdp::modelchecker::ApproximatePOMDPModelchecker<storm::models::sparse::Pomdp<ValueType>> checker(*model, this->options());
        auto result = checker.check(*formula);
        std::cout << "[" << result.lowerBound << "," << result.upperBound << std::endl;
        
    }
}
