#include "storm-config.h"
#include "test/storm_gtest.h"

#include "storm-parsers/api/properties.h"
#include "storm-parsers/parser/DirectEncodingParser.h"
#include "storm/api/properties.h"
#include "storm/api/verification.h"
#include "storm/environment/solver/MinMaxSolverEnvironment.h"
#include "storm/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/models/sparse/Mdp.h"

#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"

std::unique_ptr<storm::modelchecker::QualitativeCheckResult> getInitialStateFilter(
    std::shared_ptr<storm::models::sparse::Model<storm::Interval>> const& model) {
    return std::make_unique<storm::modelchecker::ExplicitQualitativeCheckResult>(model->getInitialStates());
}

double getQuantitativeResultAtInitialState(std::shared_ptr<storm::models::sparse::Model<storm::Interval>> const& model,
                                           std::unique_ptr<storm::modelchecker::CheckResult>& result) {
    auto filter = getInitialStateFilter(model);
    result->filter(*filter);
    return result->asQuantitativeCheckResult<double>().getMin();
}

TEST(RobustMDPModelCheckingTest, Tiny01maxmin) {
    std::shared_ptr<storm::models::sparse::Model<storm::Interval>> modelPtr =
        storm::parser::DirectEncodingParser<storm::Interval>::parseModel(STORM_TEST_RESOURCES_DIR "/imdp/tiny-01.drn");
    std::string formulasMaxString = "Pmax=? [ F \"target\"];Pmin=? [ F \"target\"]";
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::api::extractFormulasFromProperties(storm::api::parseProperties(formulasMaxString));
    storm::Environment env;
    env.solver().minMax().setMethod(storm::solver::MinMaxMethod::ValueIteration);

    std::shared_ptr<storm::models::sparse::Mdp<storm::Interval>> mdp = modelPtr->as<storm::models::sparse::Mdp<storm::Interval>>();
    ASSERT_EQ(storm::models::ModelType::Mdp, modelPtr->getType());
    auto taskMax = storm::modelchecker::CheckTask<storm::logic::Formula, double>(*formulas[0]);
    auto checker = storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<storm::Interval>>(*mdp);
    auto resultMax = checker.check(env, taskMax);
    EXPECT_NEAR(0.4, getQuantitativeResultAtInitialState(mdp, resultMax), 0.0001);
    taskMax.setRobustUncertainty(false);
    auto resultMaxNonRobust = checker.check(env, taskMax);
    EXPECT_NEAR(0.5, getQuantitativeResultAtInitialState(mdp, resultMaxNonRobust), 0.0001);

    auto taskMin = storm::modelchecker::CheckTask<storm::logic::Formula, double>(*formulas[1]);
    auto resultMin = checker.check(env, taskMin);
    EXPECT_NEAR(0.5, getQuantitativeResultAtInitialState(mdp, resultMin), 0.0001);
    taskMin.setRobustUncertainty(false);
    auto resultMinNonRobust = checker.check(env, taskMin);
    EXPECT_NEAR(0.4, getQuantitativeResultAtInitialState(mdp, resultMinNonRobust), 0.0001);

}


TEST(RobustMDPModelCheckingTest, Tiny03maxmin) {
    std::shared_ptr<storm::models::sparse::Model<storm::Interval>> modelPtr =
        storm::parser::DirectEncodingParser<storm::Interval>::parseModel(STORM_TEST_RESOURCES_DIR "/imdp/tiny-03.drn");
    std::string formulasMaxString = "Pmax=? [ F \"target\"];Pmin=? [ F \"target\"]";
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::api::extractFormulasFromProperties(storm::api::parseProperties(formulasMaxString));
    storm::Environment env;
    env.solver().minMax().setMethod(storm::solver::MinMaxMethod::ValueIteration);

    std::shared_ptr<storm::models::sparse::Mdp<storm::Interval>> mdp = modelPtr->as<storm::models::sparse::Mdp<storm::Interval>>();
    auto checker = storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<storm::Interval>>(*mdp);

    ASSERT_EQ(storm::models::ModelType::Mdp, modelPtr->getType());
    auto taskMax = storm::modelchecker::CheckTask<storm::logic::Formula, double>(*formulas[0]);
    taskMax.setProduceSchedulers();
    auto resultMax = checker.check(env, taskMax);
    EXPECT_NEAR(0.4, getQuantitativeResultAtInitialState(mdp, resultMax), 0.0001);
    taskMax.setRobustUncertainty(false);
    auto resultMaxNonRobust = checker.check(env, taskMax);
    EXPECT_NEAR(0.5, getQuantitativeResultAtInitialState(mdp, resultMaxNonRobust), 0.0001);

    auto taskMin = storm::modelchecker::CheckTask<storm::logic::Formula, double>(*formulas[1]);
    taskMin.setProduceSchedulers();
    auto resultMin = checker.check(env, taskMin);
    EXPECT_NEAR(0.5, getQuantitativeResultAtInitialState(mdp, resultMin), 0.0001);
    taskMin.setRobustUncertainty(false);
    auto resultMinNonRobust = checker.check(env, taskMin);
    EXPECT_NEAR(0.4, getQuantitativeResultAtInitialState(mdp, resultMinNonRobust), 0.0001);
}