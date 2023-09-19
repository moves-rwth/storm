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

void checkModel(std::string const& path, std::string const& formulaString, double maxmin, double maxmax, double minmax, double minmin, bool produceScheduler) {
    std::shared_ptr<storm::models::sparse::Model<storm::Interval>> modelPtr =
        storm::parser::DirectEncodingParser<storm::Interval>::parseModel(path);
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::api::extractFormulasFromProperties(storm::api::parseProperties(formulaString));
    storm::Environment env;
    env.solver().minMax().setMethod(storm::solver::MinMaxMethod::ValueIteration);

    std::shared_ptr<storm::models::sparse::Mdp<storm::Interval>> mdp = modelPtr->as<storm::models::sparse::Mdp<storm::Interval>>();
    ASSERT_EQ(storm::models::ModelType::Mdp, modelPtr->getType());
    auto taskMax = storm::modelchecker::CheckTask<storm::logic::Formula, double>(*formulas[0]);
    taskMax.setProduceSchedulers(produceScheduler);

    auto checker = storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<storm::Interval>>(*mdp);
    auto resultMax = checker.check(env, taskMax);
    EXPECT_NEAR(maxmin, getQuantitativeResultAtInitialState(mdp, resultMax), 0.0001);
    taskMax.setRobustUncertainty(false);
    auto resultMaxNonRobust = checker.check(env, taskMax);
    EXPECT_NEAR(maxmax, getQuantitativeResultAtInitialState(mdp, resultMaxNonRobust), 0.0001);

    auto taskMin = storm::modelchecker::CheckTask<storm::logic::Formula, double>(*formulas[1]);
    taskMin.setProduceSchedulers(produceScheduler);

    auto resultMin = checker.check(env, taskMin);
    EXPECT_NEAR(minmax, getQuantitativeResultAtInitialState(mdp, resultMin), 0.0001);
    taskMin.setRobustUncertainty(false);
    auto resultMinNonRobust = checker.check(env, taskMin);
    EXPECT_NEAR(minmin, getQuantitativeResultAtInitialState(mdp, resultMinNonRobust), 0.0001);
}

TEST(RobustMDPModelCheckingTest, Tiny01maxmin) {
    checkModel(STORM_TEST_RESOURCES_DIR "/imdp/tiny-01.drn", "Pmax=? [ F \"target\"];Pmin=? [ F \"target\"]", 0.4, 0.5, 0.5, 0.4, false);
}


TEST(RobustMDPModelCheckingTest, Tiny03maxmin) {
    checkModel(STORM_TEST_RESOURCES_DIR "/imdp/tiny-03.drn", "Pmax=? [ F \"target\"];Pmin=? [ F \"target\"]", 0.4, 0.5, 0.5, 0.4, true);
}

TEST(RobustMDPModelCheckingTest, Tiny04maxmin) {
    checkModel(STORM_TEST_RESOURCES_DIR "/imdp/tiny-04.drn", "Pmax=? [ F \"target\"];Pmin=? [ F \"target\"]", 1, 1, 0, 0, false);
}