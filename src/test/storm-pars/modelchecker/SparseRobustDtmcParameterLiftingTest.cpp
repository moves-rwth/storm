#include "storm-config.h"
#include "storm-pars/modelchecker/region/RegionCheckEngine.h"
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
class IsGraphPreserving {
   public:
    typedef double ValueType;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().minMax().setMethod(storm::solver::MinMaxMethod::ValueIteration);
        env.solver().minMax().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-8));
        return env;
    }
    static bool graphPreserving() {
        return true;
    }
};

class AssumeGraphPreserving {
   public:
    typedef double ValueType;
    static storm::Environment createEnvironment() {
        storm::Environment env;
        env.solver().minMax().setMethod(storm::solver::MinMaxMethod::ValueIteration);
        env.solver().minMax().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-8));
        return env;
    }
    static bool graphPreserving() {
        return false;
    }
};

template<typename TestType>
class SparseRobustDtmcParameterLiftingTest : public ::testing::Test {
   public:
    typedef typename TestType::ValueType ValueType;
    SparseRobustDtmcParameterLiftingTest() : _environment(TestType::createEnvironment()), _graphPreserving(TestType::graphPreserving()) {}
    storm::Environment const& env() const {
        return _environment;
    }
    bool const& graphPreserving() const {
        return _graphPreserving;
    }
    virtual void SetUp() {
        carl::VariablePool::getInstance().clear();
#ifndef STORM_HAVE_Z3
        GTEST_SKIP() << "Z3 not available.";
#endif
    }
    virtual void TearDown() {
        carl::VariablePool::getInstance().clear();
    }

   private:
    storm::Environment _environment;
    bool _graphPreserving;
};

typedef ::testing::Types<IsGraphPreserving, AssumeGraphPreserving> TestingTypes;

TYPED_TEST_SUITE(SparseRobustDtmcParameterLiftingTest, TestingTypes, );

TYPED_TEST(SparseRobustDtmcParameterLiftingTest, Brp_Prob) {
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

    auto regionChecker = storm::api::initializeRegionModelChecker<storm::RationalFunction>(
        this->env(), model, storm::api::createTask<storm::RationalFunction>(formulas[0], true), storm::modelchecker::RegionCheckEngine::RobustParameterLifting,
        true, this->graphPreserving());

    // start testing
    auto allSatRegion = storm::api::parseRegion<storm::RationalFunction>("0.7<=pL<=0.9,0.75<=pK<=0.95", modelParameters);
    auto exBothRegion = storm::api::parseRegion<storm::RationalFunction>("0.4<=pL<=0.65,0.75<=pK<=0.95", modelParameters);
    auto allVioRegion = storm::api::parseRegion<storm::RationalFunction>("0.1<=pL<=0.73,0.2<=pK<=0.715", modelParameters);

    EXPECT_EQ(storm::modelchecker::RegionResult::AllSat,
              regionChecker->analyzeRegion(this->env(), allSatRegion, storm::modelchecker::RegionResultHypothesis::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::ExistsBoth,
              regionChecker->analyzeRegion(this->env(), exBothRegion, storm::modelchecker::RegionResultHypothesis::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::AllViolated,
              regionChecker->analyzeRegion(this->env(), allVioRegion, storm::modelchecker::RegionResultHypothesis::Unknown, true));
}

TYPED_TEST(SparseRobustDtmcParameterLiftingTest, Brp_Prob_no_simplification) {
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

    auto regionChecker = storm::api::initializeRegionModelChecker<storm::RationalFunction>(
        this->env(), model, storm::api::createTask<storm::RationalFunction>(formulas[0], true), storm::modelchecker::RegionCheckEngine::RobustParameterLifting,
        true, this->graphPreserving());

    // start testing
    auto allSatRegion = storm::api::parseRegion<storm::RationalFunction>("0.7<=pL<=0.9,0.75<=pK<=0.95", modelParameters);
    auto exBothRegion = storm::api::parseRegion<storm::RationalFunction>("0.4<=pL<=0.65,0.75<=pK<=0.95", modelParameters);
    auto allVioRegion = storm::api::parseRegion<storm::RationalFunction>("0.1<=pL<=0.73,0.2<=pK<=0.715", modelParameters);

    EXPECT_EQ(storm::modelchecker::RegionResult::AllSat,
              regionChecker->analyzeRegion(this->env(), allSatRegion, storm::modelchecker::RegionResultHypothesis::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::ExistsBoth,
              regionChecker->analyzeRegion(this->env(), exBothRegion, storm::modelchecker::RegionResultHypothesis::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::AllViolated,
              regionChecker->analyzeRegion(this->env(), allVioRegion, storm::modelchecker::RegionResultHypothesis::Unknown, true));
}

TYPED_TEST(SparseRobustDtmcParameterLiftingTest, Brp_Rew) {
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

    auto regionChecker = storm::api::initializeRegionModelChecker<storm::RationalFunction>(
        this->env(), model, storm::api::createTask<storm::RationalFunction>(formulas[0], true), storm::modelchecker::RegionCheckEngine::RobustParameterLifting,
        true, this->graphPreserving());

    // start testing
    auto allSatRegion = storm::api::parseRegion<storm::RationalFunction>("0.7<=pK<=0.875,0.75<=TOMsg<=0.95", modelParameters);
    auto exBothRegion = storm::api::parseRegion<storm::RationalFunction>("0.6<=pK<=0.9,0.5<=TOMsg<=0.95", modelParameters);
    auto allVioRegion = storm::api::parseRegion<storm::RationalFunction>("0.1<=pK<=0.3,0.2<=TOMsg<=0.3", modelParameters);

    EXPECT_EQ(storm::modelchecker::RegionResult::AllSat,
              regionChecker->analyzeRegion(this->env(), allSatRegion, storm::modelchecker::RegionResultHypothesis::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::ExistsBoth,
              regionChecker->analyzeRegion(this->env(), exBothRegion, storm::modelchecker::RegionResultHypothesis::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::AllViolated,
              regionChecker->analyzeRegion(this->env(), allVioRegion, storm::modelchecker::RegionResultHypothesis::Unknown, true));
}

TYPED_TEST(SparseRobustDtmcParameterLiftingTest, Brp_Rew_4Par) {
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

    auto regionChecker = storm::api::initializeRegionModelChecker<storm::RationalFunction>(
        this->env(), model, storm::api::createTask<storm::RationalFunction>(formulas[0], true), storm::modelchecker::RegionCheckEngine::RobustParameterLifting,
        true, this->graphPreserving());

    // start testing
    auto allSatRegion = storm::api::parseRegion<storm::RationalFunction>("0.7<=pK<=0.9,0.6<=pL<=0.85,0.9<=TOMsg<=0.95,0.85<=TOAck<=0.9", modelParameters);
    auto exBothRegion = storm::api::parseRegion<storm::RationalFunction>("0.1<=pK<=0.7,0.2<=pL<=0.8,0.15<=TOMsg<=0.65,0.3<=TOAck<=0.9", modelParameters);
    auto allVioRegion = storm::api::parseRegion<storm::RationalFunction>("0.1<=pK<=0.4,0.2<=pL<=0.3,0.15<=TOMsg<=0.3,0.1<=TOAck<=0.2", modelParameters);

    EXPECT_EQ(storm::modelchecker::RegionResult::AllSat,
              regionChecker->analyzeRegion(this->env(), allSatRegion, storm::modelchecker::RegionResultHypothesis::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::ExistsBoth,
              regionChecker->analyzeRegion(this->env(), exBothRegion, storm::modelchecker::RegionResultHypothesis::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::AllViolated,
              regionChecker->analyzeRegion(this->env(), allVioRegion, storm::modelchecker::RegionResultHypothesis::Unknown, true));
}

TYPED_TEST(SparseRobustDtmcParameterLiftingTest, Crowds_Prob) {
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

    auto regionChecker = storm::api::initializeRegionModelChecker<storm::RationalFunction>(
        this->env(), model, storm::api::createTask<storm::RationalFunction>(formulas[0], true), storm::modelchecker::RegionCheckEngine::RobustParameterLifting,
        true, this->graphPreserving());

    // start testing
    auto allSatRegion = storm::api::parseRegion<storm::RationalFunction>("0.1<=PF<=0.75,0.15<=badC<=0.2", modelParameters);
    auto exBothRegion = storm::api::parseRegion<storm::RationalFunction>("0.75<=PF<=0.8,0.2<=badC<=0.3", modelParameters);
    auto allVioRegion = storm::api::parseRegion<storm::RationalFunction>("0.8<=PF<=0.95,0.2<=badC<=0.2", modelParameters);
    auto allVioHardRegion = storm::api::parseRegion<storm::RationalFunction>("0.8<=PF<=0.95,0.2<=badC<=0.9", modelParameters);

    EXPECT_EQ(storm::modelchecker::RegionResult::AllSat,
              regionChecker->analyzeRegion(this->env(), allSatRegion, storm::modelchecker::RegionResultHypothesis::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::ExistsBoth,
              regionChecker->analyzeRegion(this->env(), exBothRegion, storm::modelchecker::RegionResultHypothesis::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::AllViolated,
              regionChecker->analyzeRegion(this->env(), allVioRegion, storm::modelchecker::RegionResultHypothesis::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::CenterViolated,
              regionChecker->analyzeRegion(this->env(), allVioHardRegion, storm::modelchecker::RegionResultHypothesis::Unknown, true));
}

TYPED_TEST(SparseRobustDtmcParameterLiftingTest, Crowds_Prob_1Par) {
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

    auto regionChecker = storm::api::initializeRegionModelChecker<storm::RationalFunction>(
        this->env(), model, storm::api::createTask<storm::RationalFunction>(formulas[0], true), storm::modelchecker::RegionCheckEngine::RobustParameterLifting,
        true, this->graphPreserving());

    // start testing
    auto allSatRegion = storm::api::parseRegion<storm::RationalFunction>("0.9<=PF<=0.99", modelParameters);
    auto exBothRegion = storm::api::parseRegion<storm::RationalFunction>("0.8<=PF<=0.9", modelParameters);
    auto allVioRegion = storm::api::parseRegion<storm::RationalFunction>("0.01<=PF<=0.8", modelParameters);

    EXPECT_EQ(storm::modelchecker::RegionResult::AllSat,
              regionChecker->analyzeRegion(this->env(), allSatRegion, storm::modelchecker::RegionResultHypothesis::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::ExistsBoth,
              regionChecker->analyzeRegion(this->env(), exBothRegion, storm::modelchecker::RegionResultHypothesis::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::AllViolated,
              regionChecker->analyzeRegion(this->env(), allVioRegion, storm::modelchecker::RegionResultHypothesis::Unknown, true));
}

TYPED_TEST(SparseRobustDtmcParameterLiftingTest, Crowds_Prob_Const) {
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

    auto regionChecker = storm::api::initializeRegionModelChecker<storm::RationalFunction>(
        this->env(), model, storm::api::createTask<storm::RationalFunction>(formulas[0], true), storm::modelchecker::RegionCheckEngine::RobustParameterLifting,
        true, this->graphPreserving());

    // start testing
    auto allSatRegion = storm::api::parseRegion<storm::RationalFunction>("", modelParameters);

    EXPECT_EQ(storm::modelchecker::RegionResult::AllSat,
              regionChecker->analyzeRegion(this->env(), allSatRegion, storm::modelchecker::RegionResultHypothesis::Unknown, true));
}

TYPED_TEST(SparseRobustDtmcParameterLiftingTest, ZeroConf) {
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

    auto regionChecker = storm::api::initializeRegionModelChecker<storm::RationalFunction>(
        this->env(), model, storm::api::createTask<storm::RationalFunction>(formulas[0], true), storm::modelchecker::RegionCheckEngine::RobustParameterLifting,
        true, this->graphPreserving());

    // start testing
    auto allSatRegion = storm::api::parseRegion<storm::RationalFunction>("0.8<=pL<=0.95,0.8<=pK<=0.95", modelParameters);
    auto exBothRegion = storm::api::parseRegion<storm::RationalFunction>("0.6<=pL<=0.9,0.6<=pK<=0.9", modelParameters);
    auto allVioRegion = storm::api::parseRegion<storm::RationalFunction>("0.1<=pL<=0.7,0.1<=pK<=0.7", modelParameters);

    EXPECT_EQ(storm::modelchecker::RegionResult::AllSat,
              regionChecker->analyzeRegion(this->env(), allSatRegion, storm::modelchecker::RegionResultHypothesis::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::ExistsBoth,
              regionChecker->analyzeRegion(this->env(), exBothRegion, storm::modelchecker::RegionResultHypothesis::Unknown, true));
    EXPECT_EQ(storm::modelchecker::RegionResult::AllViolated,
              regionChecker->analyzeRegion(this->env(), allVioRegion, storm::modelchecker::RegionResultHypothesis::Unknown, true));
}
}  // namespace
#endif
