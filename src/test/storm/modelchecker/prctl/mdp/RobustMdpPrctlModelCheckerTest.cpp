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
    std::string formulasString = "Pmax=? [ F \"target\"]";
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::api::extractFormulasFromProperties(storm::api::parseProperties(formulasString));
    storm::Environment env;
    env.solver().minMax().setMethod(storm::solver::MinMaxMethod::ValueIteration);

    std::shared_ptr<storm::models::sparse::Mdp<storm::Interval>> mdp = modelPtr->as<storm::models::sparse::Mdp<storm::Interval>>();
    ASSERT_EQ(storm::models::ModelType::Mdp, modelPtr->getType());
    auto task = storm::modelchecker::CheckTask<storm::logic::Formula, double>(*formulas[0]);
    auto checker = storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<storm::Interval>>(*mdp);
    auto result = checker.check(env, task);
    EXPECT_NEAR(0.4, getQuantitativeResultAtInitialState(mdp, result), 0.0001);
}

TEST(RobustMDPModelCheckingTest, Tiny01minmax) {
    std::shared_ptr<storm::models::sparse::Model<storm::Interval>> modelPtr =
        storm::parser::DirectEncodingParser<storm::Interval>::parseModel(STORM_TEST_RESOURCES_DIR "/imdp/tiny-01.drn");
    std::string formulasString = "Pmin=? [ F \"target\"]";
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::api::extractFormulasFromProperties(storm::api::parseProperties(formulasString));
    storm::Environment env;
    env.solver().minMax().setMethod(storm::solver::MinMaxMethod::ValueIteration);

    std::shared_ptr<storm::models::sparse::Mdp<storm::Interval>> mdp = modelPtr->as<storm::models::sparse::Mdp<storm::Interval>>();
    ASSERT_EQ(storm::models::ModelType::Mdp, modelPtr->getType());
    auto task = storm::modelchecker::CheckTask<storm::logic::Formula, double>(*formulas[0]);
    auto checker = storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<storm::Interval>>(*mdp);
    auto result = checker.check(env, task);
    EXPECT_NEAR(0.5, getQuantitativeResultAtInitialState(mdp, result), 0.0001);
}