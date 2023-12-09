#include "storm-config.h"
#include "test/storm_gtest.h"

#ifdef STORM_HAVE_CARL

#include "storm/adapters/RationalFunctionAdapter.h"

#include "storm-pars/api/storm-pars.h"
#include "storm/api/storm.h"

#include "storm-parsers/api/storm-parsers.h"

#include "storm-pars/transformer/SparseParametricDtmcSimplifier.h"
#include "storm/environment/solver/MinMaxSolverEnvironment.h"
#include "storm/solver/stateelimination/NondeterministicModelStateEliminator.h"
#include "storm/storage/StronglyConnectedComponentDecomposition.h"
#include "storm/storage/jani/Property.h"

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

class DoubleSVIEnvironment {
   public:
    typedef double ValueType;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().minMax().setMethod(storm::solver::MinMaxMethod::SoundValueIteration);
        env.solver().minMax().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-6));
        return env;
    }
};

class RationalPiEnvironment {
   public:
    typedef storm::RationalNumber ValueType;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().minMax().setMethod(storm::solver::MinMaxMethod::PolicyIteration);
        return env;
    }
};
template<typename TestType>
class SparseDtmcParameterLiftingTest : public ::testing::Test {
   public:
    typedef typename TestType::ValueType ValueType;
    SparseDtmcParameterLiftingTest() : _environment(TestType::createEnvironment()) {}
    storm::Environment const& env() const {
        return _environment;
    }
    virtual void SetUp() {
        carl::VariablePool::getInstance().clear();
    }
    virtual void TearDown() {
        carl::VariablePool::getInstance().clear();
    }

   private:
    storm::Environment _environment;
};

typedef ::testing::Types<DoubleViEnvironment, DoubleSVIEnvironment, RationalPiEnvironment> TestingTypes;

TYPED_TEST_SUITE(SparseDtmcParameterLiftingTest, TestingTypes, );

TYPED_TEST(SparseDtmcParameterLiftingTest, Brp_Prob) {
    typedef typename TestFixture::ValueType ValueType;

    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/brp16_2.pm";
    std::string formulaAsString = "P<=0.84 [F s=5 ]";
    std::string constantsAsString = "";  // e.g. pL=0.9,TOACK=0.5

    // Program and formula
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas =
        storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model =
        storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();

    auto modelParameters = storm::models::sparse::getProbabilityParameters(*model);
    auto rewParameters = storm::models::sparse::getRewardParameters(*model);
    modelParameters.insert(rewParameters.begin(), rewParameters.end());

    auto regionChecker = storm::api::initializeParameterLiftingRegionModelChecker<storm::RationalFunction, ValueType>(
        this->env(), model, storm::api::createTask<storm::RationalFunction>(formulas[0], true));

    // start testing
    auto allSatRegion = storm::api::parseRegion<storm::RationalFunction>("0.7<=pL<=0.9,0.75<=pK<=0.95", modelParameters);
    auto exBothRegion = storm::api::parseRegion<storm::RationalFunction>("0.4<=pL<=0.65,0.75<=pK<=0.95", modelParameters);
    auto allVioRegion = storm::api::parseRegion<storm::RationalFunction>("0.1<=pL<=0.73,0.2<=pK<=0.715", modelParameters);

    EXPECT_EQ(storm::modelchecker::RegionResult::AllSat,
              regionChecker->analyzeRegion(this->env(), allSatRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                           storm::modelchecker::RegionResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::ExistsBoth,
              regionChecker->analyzeRegion(this->env(), exBothRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                           storm::modelchecker::RegionResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::AllViolated,
              regionChecker->analyzeRegion(this->env(), allVioRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                           storm::modelchecker::RegionResult::Unknown, true));
}

TYPED_TEST(SparseDtmcParameterLiftingTest, Brp_Prob_no_simplification) {
    typedef typename TestFixture::ValueType ValueType;

    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/brp16_2.pm";
    std::string formulaAsString = "P<=0.84 [F s=5 ]";
    std::string constantsAsString = "";  // e.g. pL=0.9,TOACK=0.5

    // Program and formula
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas =
        storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model =
        storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();

    auto modelParameters = storm::models::sparse::getProbabilityParameters(*model);
    auto rewParameters = storm::models::sparse::getRewardParameters(*model);
    modelParameters.insert(rewParameters.begin(), rewParameters.end());

    auto regionChecker = storm::api::initializeParameterLiftingRegionModelChecker<storm::RationalFunction, ValueType>(
        this->env(), model, storm::api::createTask<storm::RationalFunction>(formulas[0], true), false, false);

    // start testing
    auto allSatRegion = storm::api::parseRegion<storm::RationalFunction>("0.7<=pL<=0.9,0.75<=pK<=0.95", modelParameters);
    auto exBothRegion = storm::api::parseRegion<storm::RationalFunction>("0.4<=pL<=0.65,0.75<=pK<=0.95", modelParameters);
    auto allVioRegion = storm::api::parseRegion<storm::RationalFunction>("0.1<=pL<=0.73,0.2<=pK<=0.715", modelParameters);

    EXPECT_EQ(storm::modelchecker::RegionResult::AllSat,
              regionChecker->analyzeRegion(this->env(), allSatRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                           storm::modelchecker::RegionResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::ExistsBoth,
              regionChecker->analyzeRegion(this->env(), exBothRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                           storm::modelchecker::RegionResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::AllViolated,
              regionChecker->analyzeRegion(this->env(), allVioRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                           storm::modelchecker::RegionResult::Unknown, true));
}

TYPED_TEST(SparseDtmcParameterLiftingTest, Brp_Rew) {
    typedef typename TestFixture::ValueType ValueType;
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/brp_rewards16_2.pm";
    std::string formulaAsString = "R>2.5 [F ((s=5) | (s=0&srep=3)) ]";
    std::string constantsAsString = "pL=0.9,TOAck=0.5";

    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas =
        storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model =
        storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();

    auto modelParameters = storm::models::sparse::getProbabilityParameters(*model);
    auto rewParameters = storm::models::sparse::getRewardParameters(*model);
    modelParameters.insert(rewParameters.begin(), rewParameters.end());

    auto regionChecker = storm::api::initializeParameterLiftingRegionModelChecker<storm::RationalFunction, ValueType>(
        this->env(), model, storm::api::createTask<storm::RationalFunction>(formulas[0], true));

    // start testing
    auto allSatRegion = storm::api::parseRegion<storm::RationalFunction>("0.7<=pK<=0.875,0.75<=TOMsg<=0.95", modelParameters);
    auto exBothRegion = storm::api::parseRegion<storm::RationalFunction>("0.6<=pK<=0.9,0.5<=TOMsg<=0.95", modelParameters);
    auto allVioRegion = storm::api::parseRegion<storm::RationalFunction>("0.1<=pK<=0.3,0.2<=TOMsg<=0.3", modelParameters);

    EXPECT_EQ(storm::modelchecker::RegionResult::AllSat,
              regionChecker->analyzeRegion(this->env(), allSatRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                           storm::modelchecker::RegionResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::ExistsBoth,
              regionChecker->analyzeRegion(this->env(), exBothRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                           storm::modelchecker::RegionResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::AllViolated,
              regionChecker->analyzeRegion(this->env(), allVioRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                           storm::modelchecker::RegionResult::Unknown, true));
}

TYPED_TEST(SparseDtmcParameterLiftingTest, Brp_Rew_Bounded) {
    typedef typename TestFixture::ValueType ValueType;
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/brp_rewards16_2.pm";
    std::string formulaAsString = "R>2.5 [ C<=300]";
    std::string constantsAsString = "pL=0.9,TOAck=0.5";

    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas =
        storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model =
        storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();

    auto modelParameters = storm::models::sparse::getProbabilityParameters(*model);
    auto rewParameters = storm::models::sparse::getRewardParameters(*model);
    modelParameters.insert(rewParameters.begin(), rewParameters.end());

    auto regionChecker = storm::api::initializeParameterLiftingRegionModelChecker<storm::RationalFunction, ValueType>(
        this->env(), model, storm::api::createTask<storm::RationalFunction>(formulas[0], true));

    // start testing
    auto allSatRegion = storm::api::parseRegion<storm::RationalFunction>("0.7<=pK<=0.875,0.75<=TOMsg<=0.95", modelParameters);
    auto exBothRegion = storm::api::parseRegion<storm::RationalFunction>("0.6<=pK<=0.9,0.5<=TOMsg<=0.95", modelParameters);
    auto allVioRegion = storm::api::parseRegion<storm::RationalFunction>("0.1<=pK<=0.3,0.2<=TOMsg<=0.3", modelParameters);

    EXPECT_EQ(storm::modelchecker::RegionResult::AllSat,
              regionChecker->analyzeRegion(this->env(), allSatRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                           storm::modelchecker::RegionResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::ExistsBoth,
              regionChecker->analyzeRegion(this->env(), exBothRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                           storm::modelchecker::RegionResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::AllViolated,
              regionChecker->analyzeRegion(this->env(), allVioRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                           storm::modelchecker::RegionResult::Unknown, true));
}

TYPED_TEST(SparseDtmcParameterLiftingTest, Brp_Prob_exactValidation) {
    typedef typename TestFixture::ValueType ValueType;
    if (!std::is_same<ValueType, storm::RationalNumber>::value) {
        std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/brp16_2.pm";
        std::string formulaAsString = "P<=0.84 [F s=5 ]";
        std::string constantsAsString = "";  // e.g. pL=0.9,TOACK=0.5

        // Program and formula
        storm::prism::Program program = storm::api::parseProgram(programFile);
        program = storm::utility::prism::preprocess(program, constantsAsString);
        std::vector<std::shared_ptr<const storm::logic::Formula>> formulas =
            storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
        std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model =
            storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();

        auto regionChecker = storm::api::initializeValidatingRegionModelChecker<storm::RationalFunction, ValueType, storm::RationalNumber>(
            this->env(), model, storm::api::createTask<storm::RationalFunction>(formulas[0], true));

        auto modelParameters = storm::models::sparse::getProbabilityParameters(*model);
        auto rewParameters = storm::models::sparse::getRewardParameters(*model);
        modelParameters.insert(rewParameters.begin(), rewParameters.end());

        // start testing
        auto allSatRegion = storm::api::parseRegion<storm::RationalFunction>("0.7<=pL<=0.9,0.75<=pK<=0.95", modelParameters);
        auto exBothRegion = storm::api::parseRegion<storm::RationalFunction>("0.4<=pL<=0.65,0.75<=pK<=0.95", modelParameters);
        auto allVioRegion = storm::api::parseRegion<storm::RationalFunction>("0.1<=pL<=0.73,0.2<=pK<=0.715", modelParameters);

        EXPECT_EQ(storm::modelchecker::RegionResult::AllSat,
                  regionChecker->analyzeRegion(this->env(), allSatRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                               storm::modelchecker::RegionResult::Unknown, true));
        EXPECT_EQ(storm::modelchecker::RegionResult::ExistsBoth,
                  regionChecker->analyzeRegion(this->env(), exBothRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                               storm::modelchecker::RegionResult::Unknown, true));
        EXPECT_EQ(storm::modelchecker::RegionResult::AllViolated,
                  regionChecker->analyzeRegion(this->env(), allVioRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                               storm::modelchecker::RegionResult::Unknown, true));
    }
}

TYPED_TEST(SparseDtmcParameterLiftingTest, Brp_Rew_exactValidation) {
    typedef typename TestFixture::ValueType ValueType;
    if (!std::is_same<ValueType, storm::RationalNumber>::value) {
        std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/brp_rewards16_2.pm";
        std::string formulaAsString = "R>2.5 [F ((s=5) | (s=0&srep=3)) ]";
        std::string constantsAsString = "pL=0.9,TOAck=0.5";

        storm::prism::Program program = storm::api::parseProgram(programFile);
        program = storm::utility::prism::preprocess(program, constantsAsString);
        std::vector<std::shared_ptr<const storm::logic::Formula>> formulas =
            storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
        std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model =
            storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();

        auto modelParameters = storm::models::sparse::getProbabilityParameters(*model);
        auto rewParameters = storm::models::sparse::getRewardParameters(*model);
        modelParameters.insert(rewParameters.begin(), rewParameters.end());

        auto regionChecker = storm::api::initializeValidatingRegionModelChecker<storm::RationalFunction, ValueType, storm::RationalNumber>(
            this->env(), model, storm::api::createTask<storm::RationalFunction>(formulas[0], true));

        // start testing
        auto allSatRegion = storm::api::parseRegion<storm::RationalFunction>("0.7<=pK<=0.875,0.75<=TOMsg<=0.95", modelParameters);
        auto exBothRegion = storm::api::parseRegion<storm::RationalFunction>("0.6<=pK<=0.9,0.5<=TOMsg<=0.95", modelParameters);
        auto allVioRegion = storm::api::parseRegion<storm::RationalFunction>("0.1<=pK<=0.3,0.2<=TOMsg<=0.3", modelParameters);

        EXPECT_EQ(storm::modelchecker::RegionResult::AllSat,
                  regionChecker->analyzeRegion(this->env(), allSatRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                               storm::modelchecker::RegionResult::Unknown, true));
        EXPECT_EQ(storm::modelchecker::RegionResult::ExistsBoth,
                  regionChecker->analyzeRegion(this->env(), exBothRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                               storm::modelchecker::RegionResult::Unknown, true));
        EXPECT_EQ(storm::modelchecker::RegionResult::AllViolated,
                  regionChecker->analyzeRegion(this->env(), allVioRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                               storm::modelchecker::RegionResult::Unknown, true));
    }
}

TYPED_TEST(SparseDtmcParameterLiftingTest, Brp_Rew_Bounded_exactValidation) {
    typedef typename TestFixture::ValueType ValueType;

    if (!std::is_same<ValueType, storm::RationalNumber>::value) {
        std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/brp_rewards16_2.pm";
        std::string formulaAsString = "R>2.5 [ C<=300]";
        std::string constantsAsString = "pL=0.9,TOAck=0.5";

        storm::prism::Program program = storm::api::parseProgram(programFile);
        program = storm::utility::prism::preprocess(program, constantsAsString);
        std::vector<std::shared_ptr<const storm::logic::Formula>> formulas =
            storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
        std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model =
            storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();

        auto modelParameters = storm::models::sparse::getProbabilityParameters(*model);
        auto rewParameters = storm::models::sparse::getRewardParameters(*model);
        modelParameters.insert(rewParameters.begin(), rewParameters.end());

        auto regionChecker = storm::api::initializeValidatingRegionModelChecker<storm::RationalFunction, ValueType, storm::RationalNumber>(
            this->env(), model, storm::api::createTask<storm::RationalFunction>(formulas[0], true));

        // start testing
        auto allSatRegion = storm::api::parseRegion<storm::RationalFunction>("0.7<=pK<=0.875,0.75<=TOMsg<=0.95", modelParameters);
        auto exBothRegion = storm::api::parseRegion<storm::RationalFunction>("0.6<=pK<=0.9,0.5<=TOMsg<=0.95", modelParameters);
        auto allVioRegion = storm::api::parseRegion<storm::RationalFunction>("0.1<=pK<=0.3,0.2<=TOMsg<=0.3", modelParameters);

        EXPECT_EQ(storm::modelchecker::RegionResult::AllSat,
                  regionChecker->analyzeRegion(this->env(), allSatRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                               storm::modelchecker::RegionResult::Unknown, true));
        EXPECT_EQ(storm::modelchecker::RegionResult::ExistsBoth,
                  regionChecker->analyzeRegion(this->env(), exBothRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                               storm::modelchecker::RegionResult::Unknown, true));
        EXPECT_EQ(storm::modelchecker::RegionResult::AllViolated,
                  regionChecker->analyzeRegion(this->env(), allVioRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                               storm::modelchecker::RegionResult::Unknown, true));
    }
}

TYPED_TEST(SparseDtmcParameterLiftingTest, Brp_Rew_Infty) {
    typedef typename TestFixture::ValueType ValueType;

    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/brp_rewards16_2.pm";
    std::string formulaAsString = "R>2.5 [F (s=0&srep=3) ]";
    std::string constantsAsString = "";
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas =
        storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model =
        storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();

    auto modelParameters = storm::models::sparse::getProbabilityParameters(*model);
    auto rewParameters = storm::models::sparse::getRewardParameters(*model);
    modelParameters.insert(rewParameters.begin(), rewParameters.end());

    auto regionChecker = storm::api::initializeParameterLiftingRegionModelChecker<storm::RationalFunction, ValueType>(
        this->env(), model, storm::api::createTask<storm::RationalFunction>(formulas[0], true));

    // start testing
    auto allSatRegion = storm::api::parseRegion<storm::RationalFunction>("0.7<=pK<=0.9,0.6<=pL<=0.85,0.9<=TOMsg<=0.95,0.85<=TOAck<=0.9", modelParameters);

    EXPECT_EQ(storm::modelchecker::RegionResult::AllSat,
              regionChecker->analyzeRegion(this->env(), allSatRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                           storm::modelchecker::RegionResult::Unknown, true));
}

TYPED_TEST(SparseDtmcParameterLiftingTest, Brp_Rew_4Par) {
    typedef typename TestFixture::ValueType ValueType;

    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/brp_rewards16_2.pm";
    std::string formulaAsString = "R>2.5 [F ((s=5) | (s=0&srep=3)) ]";
    std::string constantsAsString = "";  //!! this model will have 4 parameters
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas =
        storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model =
        storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();

    auto modelParameters = storm::models::sparse::getProbabilityParameters(*model);
    auto rewParameters = storm::models::sparse::getRewardParameters(*model);
    modelParameters.insert(rewParameters.begin(), rewParameters.end());

    auto regionChecker = storm::api::initializeParameterLiftingRegionModelChecker<storm::RationalFunction, ValueType>(
        this->env(), model, storm::api::createTask<storm::RationalFunction>(formulas[0], true));

    // start testing
    auto allSatRegion = storm::api::parseRegion<storm::RationalFunction>("0.7<=pK<=0.9,0.6<=pL<=0.85,0.9<=TOMsg<=0.95,0.85<=TOAck<=0.9", modelParameters);
    auto exBothRegion = storm::api::parseRegion<storm::RationalFunction>("0.1<=pK<=0.7,0.2<=pL<=0.8,0.15<=TOMsg<=0.65,0.3<=TOAck<=0.9", modelParameters);
    auto allVioRegion = storm::api::parseRegion<storm::RationalFunction>("0.1<=pK<=0.4,0.2<=pL<=0.3,0.15<=TOMsg<=0.3,0.1<=TOAck<=0.2", modelParameters);

    EXPECT_EQ(storm::modelchecker::RegionResult::AllSat,
              regionChecker->analyzeRegion(this->env(), allSatRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                           storm::modelchecker::RegionResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::ExistsBoth,
              regionChecker->analyzeRegion(this->env(), exBothRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                           storm::modelchecker::RegionResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::AllViolated,
              regionChecker->analyzeRegion(this->env(), allVioRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                           storm::modelchecker::RegionResult::Unknown, true));
}

TYPED_TEST(SparseDtmcParameterLiftingTest, Crowds_Prob) {
    typedef typename TestFixture::ValueType ValueType;

    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/crowds3_5.pm";
    std::string formulaAsString = "P<0.5 [F \"observe0Greater1\" ]";
    std::string constantsAsString = "";  // e.g. pL=0.9,TOACK=0.5

    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas =
        storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model =
        storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();

    auto modelParameters = storm::models::sparse::getProbabilityParameters(*model);
    auto rewParameters = storm::models::sparse::getRewardParameters(*model);
    modelParameters.insert(rewParameters.begin(), rewParameters.end());

    auto regionChecker = storm::api::initializeParameterLiftingRegionModelChecker<storm::RationalFunction, ValueType>(
        this->env(), model, storm::api::createTask<storm::RationalFunction>(formulas[0], true));

    // start testing
    auto allSatRegion = storm::api::parseRegion<storm::RationalFunction>("0.1<=PF<=0.75,0.15<=badC<=0.2", modelParameters);
    auto exBothRegion = storm::api::parseRegion<storm::RationalFunction>("0.75<=PF<=0.8,0.2<=badC<=0.3", modelParameters);
    auto allVioRegion = storm::api::parseRegion<storm::RationalFunction>("0.8<=PF<=0.95,0.2<=badC<=0.2", modelParameters);
    auto allVioHardRegion = storm::api::parseRegion<storm::RationalFunction>("0.8<=PF<=0.95,0.2<=badC<=0.9", modelParameters);

    EXPECT_EQ(storm::modelchecker::RegionResult::AllSat,
              regionChecker->analyzeRegion(this->env(), allSatRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                           storm::modelchecker::RegionResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::ExistsBoth,
              regionChecker->analyzeRegion(this->env(), exBothRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                           storm::modelchecker::RegionResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::AllViolated,
              regionChecker->analyzeRegion(this->env(), allVioRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                           storm::modelchecker::RegionResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::CenterViolated,
              regionChecker->analyzeRegion(this->env(), allVioHardRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                           storm::modelchecker::RegionResult::Unknown, true));
}

TYPED_TEST(SparseDtmcParameterLiftingTest, Crowds_Prob_stepBounded) {
    typedef typename TestFixture::ValueType ValueType;

    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/crowds3_5.pm";
    std::string formulaAsString = "P<0.5 [F<=300 \"observe0Greater1\" ]";
    std::string constantsAsString = "";  // e.g. pL=0.9,TOACK=0.5

    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas =
        storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model =
        storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();

    auto modelParameters = storm::models::sparse::getProbabilityParameters(*model);
    auto rewParameters = storm::models::sparse::getRewardParameters(*model);
    modelParameters.insert(rewParameters.begin(), rewParameters.end());

    auto regionChecker = storm::api::initializeParameterLiftingRegionModelChecker<storm::RationalFunction, ValueType>(
        this->env(), model, storm::api::createTask<storm::RationalFunction>(formulas[0], true));

    // start testing
    auto allSatRegion = storm::api::parseRegion<storm::RationalFunction>("0.1<=PF<=0.75,0.15<=badC<=0.2", modelParameters);
    auto exBothRegion = storm::api::parseRegion<storm::RationalFunction>("0.75<=PF<=0.8,0.2<=badC<=0.3", modelParameters);
    auto allVioRegion = storm::api::parseRegion<storm::RationalFunction>("0.8<=PF<=0.95,0.2<=badC<=0.2", modelParameters);
    auto allVioHardRegion = storm::api::parseRegion<storm::RationalFunction>("0.8<=PF<=0.95,0.2<=badC<=0.9", modelParameters);

    EXPECT_EQ(storm::modelchecker::RegionResult::AllSat,
              regionChecker->analyzeRegion(this->env(), allSatRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                           storm::modelchecker::RegionResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::ExistsBoth,
              regionChecker->analyzeRegion(this->env(), exBothRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                           storm::modelchecker::RegionResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::AllViolated,
              regionChecker->analyzeRegion(this->env(), allVioRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                           storm::modelchecker::RegionResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::CenterViolated,
              regionChecker->analyzeRegion(this->env(), allVioHardRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                           storm::modelchecker::RegionResult::Unknown, true));
}

TYPED_TEST(SparseDtmcParameterLiftingTest, Crowds_Prob_1Par) {
    typedef typename TestFixture::ValueType ValueType;

    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/crowds3_5.pm";
    std::string formulaAsString = "P>0.75 [F \"observe0Greater1\" ]";
    std::string constantsAsString = "badC=0.3";  // e.g. pL=0.9,TOACK=0.5

    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas =
        storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model =
        storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();

    auto modelParameters = storm::models::sparse::getProbabilityParameters(*model);
    auto rewParameters = storm::models::sparse::getRewardParameters(*model);
    modelParameters.insert(rewParameters.begin(), rewParameters.end());

    auto regionChecker = storm::api::initializeParameterLiftingRegionModelChecker<storm::RationalFunction, ValueType>(
        this->env(), model, storm::api::createTask<storm::RationalFunction>(formulas[0], true));

    // start testing
    auto allSatRegion = storm::api::parseRegion<storm::RationalFunction>("0.9<=PF<=0.99", modelParameters);
    auto exBothRegion = storm::api::parseRegion<storm::RationalFunction>("0.8<=PF<=0.9", modelParameters);
    auto allVioRegion = storm::api::parseRegion<storm::RationalFunction>("0.01<=PF<=0.8", modelParameters);

    EXPECT_EQ(storm::modelchecker::RegionResult::AllSat,
              regionChecker->analyzeRegion(this->env(), allSatRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                           storm::modelchecker::RegionResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::ExistsBoth,
              regionChecker->analyzeRegion(this->env(), exBothRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                           storm::modelchecker::RegionResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::AllViolated,
              regionChecker->analyzeRegion(this->env(), allVioRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                           storm::modelchecker::RegionResult::Unknown, true));
}

TYPED_TEST(SparseDtmcParameterLiftingTest, Crowds_Prob_Const) {
    typedef typename TestFixture::ValueType ValueType;

    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/crowds3_5.pm";
    std::string formulaAsString = "P>0.6 [F \"observe0Greater1\" ]";
    std::string constantsAsString = "PF=0.9,badC=0.2";

    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas =
        storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model =
        storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();

    auto modelParameters = storm::models::sparse::getProbabilityParameters(*model);
    auto rewParameters = storm::models::sparse::getRewardParameters(*model);
    modelParameters.insert(rewParameters.begin(), rewParameters.end());

    auto regionChecker = storm::api::initializeParameterLiftingRegionModelChecker<storm::RationalFunction, ValueType>(
        this->env(), model, storm::api::createTask<storm::RationalFunction>(formulas[0], true));

    // start testing
    auto allSatRegion = storm::api::parseRegion<storm::RationalFunction>("", modelParameters);

    EXPECT_EQ(storm::modelchecker::RegionResult::AllSat,
              regionChecker->analyzeRegion(this->env(), allSatRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                           storm::modelchecker::RegionResult::Unknown, true));
}

TYPED_TEST(SparseDtmcParameterLiftingTest, ZeroConf) {
    typedef typename TestFixture::ValueType ValueType;

    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/zeroconf4.pm";
    std::string formulaAsString = "P>0.5 [F s=5 ]";
    std::string constantsAsString = " n = 4";  // e.g. pL=0.9,TOACK=0.5

    // Program and formula
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas =
        storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model =
        storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();

    auto modelParameters = storm::models::sparse::getProbabilityParameters(*model);
    auto rewParameters = storm::models::sparse::getRewardParameters(*model);
    modelParameters.insert(rewParameters.begin(), rewParameters.end());

    auto regionChecker = storm::api::initializeParameterLiftingRegionModelChecker<storm::RationalFunction, ValueType>(
        this->env(), model, storm::api::createTask<storm::RationalFunction>(formulas[0], true));

    // start testing
    auto allSatRegion = storm::api::parseRegion<storm::RationalFunction>("0.8<=pL<=0.95,0.8<=pK<=0.95", modelParameters);
    auto exBothRegion = storm::api::parseRegion<storm::RationalFunction>("0.6<=pL<=0.9,0.6<=pK<=0.9", modelParameters);
    auto allVioRegion = storm::api::parseRegion<storm::RationalFunction>("0.1<=pL<=0.7,0.1<=pK<=0.7", modelParameters);

    EXPECT_EQ(storm::modelchecker::RegionResult::AllSat,
              regionChecker->analyzeRegion(this->env(), allSatRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                           storm::modelchecker::RegionResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::ExistsBoth,
              regionChecker->analyzeRegion(this->env(), exBothRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                           storm::modelchecker::RegionResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::AllViolated,
              regionChecker->analyzeRegion(this->env(), allVioRegion, storm::modelchecker::RegionResultHypothesis::Unknown,
                                           storm::modelchecker::RegionResult::Unknown, true));
}
}  // namespace
#endif
