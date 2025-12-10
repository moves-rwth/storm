#include "storm-config.h"
#include "test/storm_gtest.h"

#include "storm-parsers/api/model_descriptions.h"
#include "storm-parsers/api/properties.h"
#include "storm-parsers/parser/DirectEncodingParser.h"
#include "storm/api/builder.h"
#include "storm/api/properties.h"
#include "storm/api/verification.h"
#include "storm/environment/solver/MinMaxSolverEnvironment.h"
#include "storm/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/transformer/AddUncertainty.h"

std::unique_ptr<storm::modelchecker::QualitativeCheckResult> getInitialStateFilter(
    std::shared_ptr<storm::models::sparse::Model<storm::Interval>> const& model) {
    return std::make_unique<storm::modelchecker::ExplicitQualitativeCheckResult>(model->getInitialStates());
}

std::unique_ptr<storm::modelchecker::QualitativeCheckResult> getInitialStateFilter(std::shared_ptr<storm::models::sparse::Model<double>> const& model) {
    return std::make_unique<storm::modelchecker::ExplicitQualitativeCheckResult>(model->getInitialStates());
}

double getQuantitativeResultAtInitialState(std::shared_ptr<storm::models::sparse::Model<storm::Interval>> const& model,
                                           std::unique_ptr<storm::modelchecker::CheckResult>& result) {
    auto filter = getInitialStateFilter(model);
    result->filter(*filter);
    return result->asQuantitativeCheckResult<double>().getMin();
}

double getQuantitativeResultAtInitialState(std::shared_ptr<storm::models::sparse::Model<double>> const& model,
                                           std::unique_ptr<storm::modelchecker::CheckResult>& result) {
    auto filter = getInitialStateFilter(model);
    result->filter(*filter);
    return result->asQuantitativeCheckResult<double>().getMin();
}

void expectThrow(std::string const& path, std::string const& formulaString,
                 std::optional<storm::UncertaintyResolutionMode> uncertaintyResolutionMode = std::nullopt) {
    std::shared_ptr<storm::models::sparse::Model<storm::Interval>> modelPtr = storm::parser::DirectEncodingParser<storm::Interval>::parseModel(path);
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::api::extractFormulasFromProperties(storm::api::parseProperties(formulaString));

    storm::Environment env;
    env.solver().minMax().setMethod(storm::solver::MinMaxMethod::ValueIteration);

    std::shared_ptr<storm::models::sparse::Dtmc<storm::Interval>> dtmc = modelPtr->as<storm::models::sparse::Dtmc<storm::Interval>>();
    ASSERT_EQ(storm::models::ModelType::Dtmc, modelPtr->getType());
    auto task = storm::modelchecker::CheckTask<storm::logic::Formula, double>(*formulas[0]);
    if (uncertaintyResolutionMode.has_value()) {
        task.setUncertaintyResolutionMode(uncertaintyResolutionMode.value());
    }

    auto checker = storm::modelchecker::SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<storm::Interval>>(*dtmc);
    STORM_SILENT_EXPECT_THROW(checker.check(env, task), storm::exceptions::BaseException);
}

void expectThrowPrism(std::string const& path, std::string const& formulaString) {
    storm::prism::Program program = storm::api::parseProgram(path);
    program = storm::utility::prism::preprocess(program, "");

    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::api::extractFormulasFromProperties(storm::api::parseProperties(formulaString));
    std::shared_ptr<storm::models::sparse::Model<storm::Interval>> modelPtr = storm::api::buildSparseModel<storm::Interval>(program, formulas);

    storm::Environment env;
    env.solver().minMax().setMethod(storm::solver::MinMaxMethod::ValueIteration);

    std::shared_ptr<storm::models::sparse::Dtmc<storm::Interval>> dtmc = modelPtr->as<storm::models::sparse::Dtmc<storm::Interval>>();
    ASSERT_EQ(storm::models::ModelType::Dtmc, modelPtr->getType());
    auto task = storm::modelchecker::CheckTask<storm::logic::Formula, double>(*formulas[0]);

    auto checker = storm::modelchecker::SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<storm::Interval>>(*dtmc);
    STORM_SILENT_EXPECT_THROW(checker.check(env, task), storm::exceptions::InvalidArgumentException);
}

void checkExplicitModelForQuantitativeResult(std::string const& path, std::string const& formulaString, double min, double max) {
    std::shared_ptr<storm::models::sparse::Model<storm::Interval>> modelPtr = storm::parser::DirectEncodingParser<storm::Interval>::parseModel(path);
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::api::extractFormulasFromProperties(storm::api::parseProperties(formulaString));
    storm::Environment env;
    env.solver().minMax().setMethod(storm::solver::MinMaxMethod::ValueIteration);

    std::shared_ptr<storm::models::sparse::Dtmc<storm::Interval>> dtmc = modelPtr->as<storm::models::sparse::Dtmc<storm::Interval>>();
    ASSERT_EQ(storm::models::ModelType::Dtmc, modelPtr->getType());
    auto taskMax = storm::modelchecker::CheckTask<storm::logic::Formula, double>(*formulas[0]);
    taskMax.setUncertaintyResolutionMode(storm::UncertaintyResolutionMode::Maximize);

    auto checker = storm::modelchecker::SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<storm::Interval>>(*dtmc);
    auto resultMax = checker.check(env, taskMax);
    EXPECT_NEAR(max, getQuantitativeResultAtInitialState(dtmc, resultMax), 0.0001);

    auto taskMin = storm::modelchecker::CheckTask<storm::logic::Formula, double>(*formulas[1]);
    taskMin.setUncertaintyResolutionMode(storm::UncertaintyResolutionMode::Minimize);

    auto resultMin = checker.check(env, taskMin);
    EXPECT_NEAR(min, getQuantitativeResultAtInitialState(dtmc, resultMin), 0.0001);
}

void checkPrismModelForQuantitativeResult(std::string const& path, std::string const& formulaString, double min, double max) {
    storm::prism::Program program = storm::api::parseProgram(path);
    program = storm::utility::prism::preprocess(program, "");

    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::api::extractFormulasFromProperties(storm::api::parseProperties(formulaString));
    std::shared_ptr<storm::models::sparse::Model<storm::Interval>> modelPtr = storm::api::buildSparseModel<storm::Interval>(program, formulas);

    storm::Environment env;
    env.solver().minMax().setMethod(storm::solver::MinMaxMethod::ValueIteration);

    std::shared_ptr<storm::models::sparse::Dtmc<storm::Interval>> dtmc = modelPtr->as<storm::models::sparse::Dtmc<storm::Interval>>();
    ASSERT_EQ(storm::models::ModelType::Dtmc, modelPtr->getType());
    auto taskMax = storm::modelchecker::CheckTask<storm::logic::Formula, double>(*formulas[0]);
    taskMax.setUncertaintyResolutionMode(storm::UncertaintyResolutionMode::Maximize);

    auto checker = storm::modelchecker::SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<storm::Interval>>(*dtmc);
    auto resultMax = checker.check(env, taskMax);
    EXPECT_NEAR(max, getQuantitativeResultAtInitialState(dtmc, resultMax), 0.0001);

    auto taskMin = storm::modelchecker::CheckTask<storm::logic::Formula, double>(*formulas[1]);
    taskMin.setUncertaintyResolutionMode(storm::UncertaintyResolutionMode::Minimize);

    auto resultMin = checker.check(env, taskMin);
    EXPECT_NEAR(min, getQuantitativeResultAtInitialState(dtmc, resultMin), 0.0001);
}

void checkModelForQualitativeResult(std::string const& path, std::string const& formulaString, std::vector<storm::storage::BitVector> expectedResultVector) {
    std::shared_ptr<storm::models::sparse::Model<storm::Interval>> modelPtr = storm::parser::DirectEncodingParser<storm::Interval>::parseModel(path);
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::api::extractFormulasFromProperties(storm::api::parseProperties(formulaString));
    storm::Environment env;
    env.solver().minMax().setMethod(storm::solver::MinMaxMethod::ValueIteration);

    std::shared_ptr<storm::models::sparse::Dtmc<storm::Interval>> dtmc = modelPtr->as<storm::models::sparse::Dtmc<storm::Interval>>();
    ASSERT_EQ(storm::models::ModelType::Dtmc, modelPtr->getType());
    auto task1 = storm::modelchecker::CheckTask<storm::logic::Formula, double>(*formulas[0]);

    auto checker = storm::modelchecker::SparsePropositionalModelChecker<storm::models::sparse::Dtmc<storm::Interval>>(*dtmc);
    auto result = checker.check(env, task1);

    ASSERT_TRUE(result->isExplicitQualitativeCheckResult());

    for (size_t i = 0; i < expectedResultVector[0].size(); i++) {
        EXPECT_EQ(expectedResultVector[0].get(i), result->asExplicitQualitativeCheckResult()[i]);
    }

    auto task2 = storm::modelchecker::CheckTask<storm::logic::Formula, double>(*formulas[1]);

    result = checker.check(env, task2);

    ASSERT_TRUE(result->isExplicitQualitativeCheckResult());

    for (size_t i = 0; i < expectedResultVector[1].size(); i++) {
        EXPECT_EQ(expectedResultVector[1].get(i), result->asExplicitQualitativeCheckResult()[i]);
    }
}

TEST(RobustDtmcModelCheckerTest, Tiny01ReachMaxMinProbs) {
    // Maximal Reachability probabilities using explicit format.
    checkExplicitModelForQuantitativeResult(STORM_TEST_RESOURCES_DIR "/idtmc/tiny-01.drn", "P=? [ F \"target\"];P=? [ F \"target\"]", 0.3, 0.5);
}

TEST(RobustDtmcModelCheckerTest, Tiny01MaxReachProbNoUncertaintyResolutionMode) {
    // Nature requires a resolution mode, expect thrown.
    expectThrow(STORM_TEST_RESOURCES_DIR "/idtmc/tiny-01.drn", "P=? [ F \"target\"];",
                std::make_optional<storm::UncertaintyResolutionMode>(storm::UncertaintyResolutionMode::Unset));
}

TEST(RobustDtmcModelCheckerTest, Tiny01MaxReachProbNoOptimizationDirectionButRobust) {
    // Nature requires a resolution mode, expect thrown.
    expectThrow(STORM_TEST_RESOURCES_DIR "/idtmc/tiny-01.drn", "P=? [ F \"target\"];",
                std::make_optional<storm::UncertaintyResolutionMode>(storm::UncertaintyResolutionMode::Robust));
}

TEST(RobustDtmcModelCheckerTest, Tiny02GloballyMaxMinProbs) {
    // Globally not yet supported, expect throw.
    expectThrow(STORM_TEST_RESOURCES_DIR "/idtmc/tiny-02.drn", "P=? [ G \"target\"];P=? [ G \"target\"]");
}

TEST(RobustDtmcModelCheckerTest, DieIntervalsMaxMin) {
#ifndef STORM_HAVE_Z3
    GTEST_SKIP() << "Z3 not available.";
#endif
    // Maxima reachability probabilities using PRISM format.
    checkPrismModelForQuantitativeResult(STORM_TEST_RESOURCES_DIR "/idtmc/die-intervals.pm", "P=? [ F \"one\"];P=? [ F \"one\"]", 9.0 / 189.0, 72.0 / 189.0);
}

TEST(RobustDtmcModelCheckerTest, BrpIntervalsMaxMin) {
#ifndef STORM_HAVE_Z3
    GTEST_SKIP() << "Z3 not available.";
#endif
    // Maxima reachability probabilities using PRISM format.
    checkPrismModelForQuantitativeResult(STORM_TEST_RESOURCES_DIR "/idtmc/brp-32-2-intervals.pm", "P=? [ F \"error\" ];P=? [ F \"error\" ]",
                                         2.559615918664207e-10, 0.0008464876763422187);
}

TEST(RobustDtmcModelCheckerTest, DieIntervalsMaxMinRewards) {
#ifndef STORM_HAVE_Z3
    GTEST_SKIP() << "Z3 not available.";
#endif
    // Maxima reachability rewards using PRISM format.
    checkPrismModelForQuantitativeResult(STORM_TEST_RESOURCES_DIR "/idtmc/die-intervals.pm", "R=? [ F \"done\"];R=? [ F \"done\"]", 3.25, 4.6);
}

TEST(RobustDtmcModelCheckerTest, Tiny03MaxMinRewards) {
    // Maxima reachability rewards using explicit format.
    checkExplicitModelForQuantitativeResult(STORM_TEST_RESOURCES_DIR "/idtmc/tiny-03.drn", "R=? [ F \"target\"];R=? [ F \"target\"]", 6.5, 8.5);
}

TEST(RobustDtmcModelCheckerTest, Tiny03RewardsNoUncertaintyResolutionMode) {
    // Nature requires a resolution mode, expect thrown.
    expectThrow(STORM_TEST_RESOURCES_DIR "/idtmc/tiny-03.drn", "R=? [ F \"target\"]", storm::UncertaintyResolutionMode::Unset);
}

TEST(RobustDtmcModelCheckerTest, Tiny04MaxMinRewards) {
    // Maxima reachability rewards using explicit format - infinite reward case.
    checkExplicitModelForQuantitativeResult(STORM_TEST_RESOURCES_DIR "/idtmc/tiny-04.drn", "R=? [ F \"target\"];R=? [ F \"target\"]",
                                            std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());
}

TEST(RobustDtmcModelCheckerTest, TinyO2Propositional) {
    // Propositional formula using explicit format.
    std::vector<storm::storage::BitVector> expectedResults;
    auto result1 = storm::storage::BitVector(3);
    result1.set(0);
    result1.set(2);
    expectedResults.push_back(result1);
    result1.complement();
    expectedResults.push_back(result1);

    checkModelForQualitativeResult(STORM_TEST_RESOURCES_DIR "/idtmc/tiny-02.drn", "\"target\";!\"target\"", expectedResults);
}
