#include "storm-config.h"
#include "test/storm_gtest.h"

#include "storm/environment/modelchecker/MultiObjectiveModelCheckerEnvironment.h"
#include "storm/modelchecker/multiobjective/multiObjectiveModelChecking.h"

#include "storm-parsers/api/storm-parsers.h"
#include "storm/api/storm.h"
#include "storm/environment/Environment.h"
#include "storm/exceptions/InvalidOperationException.h"
#include "storm/modelchecker/results/ExplicitParetoCurveCheckResult.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/storage/SchedulerClass.h"
#include "storm/storage/jani/Property.h"

#if defined STORM_HAVE_HYPRO || defined STORM_HAVE_Z3_OPTIMIZE

namespace {

storm::Environment getPositionalDeterministicEnvironment() {
    storm::Environment env;
    env.modelchecker().multi().setSchedulerRestriction(storm::storage::SchedulerClass().setPositional().setIsDeterministic());
    return env;
}

storm::Environment getGoalDeterministicEnvironment() {
    storm::Environment env;
    env.modelchecker().multi().setSchedulerRestriction(
        storm::storage::SchedulerClass().setMemoryPattern(storm::storage::SchedulerClass::MemoryPattern::GoalMemory).setIsDeterministic());
    return env;
}

typedef std::vector<storm::RationalNumber> Point;

std::vector<Point> parsePoints(std::vector<std::string> const& input) {
    std::vector<Point> result;
    for (auto const& i : input) {
        Point currPoint;
        std::size_t pos1 = 0;
        std::size_t pos2 = i.find(",");
        while (pos2 != std::string::npos) {
            currPoint.push_back(storm::utility::convertNumber<storm::RationalNumber, std::string>(i.substr(pos1, pos2 - pos1)));
            pos1 = pos2 + 1;
            pos2 = i.find(",", pos1);
        }
        currPoint.push_back(storm::utility::convertNumber<storm::RationalNumber, std::string>(i.substr(pos1)));
        result.push_back(currPoint);
    }
    return result;
}

std::set<Point> setMinus(std::vector<Point> const& lhs, std::vector<Point> const& rhs) {
    std::set<Point> result(lhs.begin(), lhs.end());
    for (auto const& r : rhs) {
        for (auto lIt = result.begin(); lIt != result.end(); ++lIt) {
            if (*lIt == r) {
                result.erase(lIt);
                break;
            }
        }
    }
    return result;
}

std::string toString(std::vector<Point> pointset, bool asDouble) {
    std::stringstream s;
    for (auto const& p : pointset) {
        s << "[";
        bool first = true;
        for (auto const& pi : p) {
            if (first) {
                first = false;
            } else {
                s << ", ";
            }
            if (asDouble) {
                s << storm::utility::convertNumber<double>(pi);
            } else {
                s << pi;
            }
        }
        s << "]\n";
    }
    return s.str();
}

TEST(MultiObjectiveSchedRestModelCheckerTest, steps) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/mdp/multiobj_stairs.nm";
    std::string constantsString = "N=3";
    std::string formulasAsString = "multi(Pmax=? [ F y=1], Pmax=? [ F y=2 ])";

    // programm, model, formula
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsString);
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas =
        storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulasAsString, program));
    std::shared_ptr<storm::models::sparse::Mdp<storm::RationalNumber>> mdp =
        storm::api::buildSparseModel<storm::RationalNumber>(program, formulas)->as<storm::models::sparse::Mdp<storm::RationalNumber>>();
    std::vector<Point> expected, actual;
    std::set<Point> incorrectPoints, missingPoints;
    std::unique_ptr<storm::modelchecker::CheckResult> result;

    storm::Environment env = getPositionalDeterministicEnvironment();

    expected = parsePoints({"0.875,0", "0,0.875", "0.125,0.75", "0.25,0.625", "0.375,0.5", "0.5,0.375", "0.625,0.25", "0.75,0.125"});

    env.modelchecker().multi().setEncodingType(storm::MultiObjectiveModelCheckerEnvironment::EncodingType::Flow);
    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[0]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isParetoCurveCheckResult());
    actual = result->asExplicitParetoCurveCheckResult<storm::RationalNumber>().getPoints();
    missingPoints = setMinus(expected, actual);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the expected solution are missing:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);
    incorrectPoints = setMinus(actual, expected);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the returned solution are not expected:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);

    env.modelchecker().multi().setEncodingType(storm::MultiObjectiveModelCheckerEnvironment::EncodingType::Classic);
    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[0]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isParetoCurveCheckResult());
    actual = result->asExplicitParetoCurveCheckResult<storm::RationalNumber>().getPoints();
    missingPoints = setMinus(expected, actual);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the expected solution are missing:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);
    incorrectPoints = setMinus(actual, expected);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the returned solution are not expected:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);
}

TEST(MultiObjectiveSchedRestModelCheckerTest, mecs) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/mdp/multiobj_mecs.nm";
    std::string constantsString = "";
    std::string formulasAsString = "multi(Pmin=? [ F x=3], Pmax=? [ F x=4 ]);";
    formulasAsString += "\nmulti(R{\"test\"}min=? [C], Pmin=? [F \"t1\"]);";
    formulasAsString += "\nmulti(R{\"test\"}min=? [C], Pmin=? [F \"t2\"]);";
    formulasAsString += "\nmulti(R{\"test\"}min=? [C], Pmin=? [F \"t2\"], Pmax=? [F \"t1\"]);";
    // programm, model, formula
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsString);
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas =
        storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulasAsString, program));
    std::shared_ptr<storm::models::sparse::Mdp<storm::RationalNumber>> mdp =
        storm::api::buildSparseModel<storm::RationalNumber>(program, formulas)->as<storm::models::sparse::Mdp<storm::RationalNumber>>();
    std::vector<Point> expected, actual;
    std::set<Point> incorrectPoints, missingPoints;
    std::unique_ptr<storm::modelchecker::CheckResult> result;

    storm::Environment env = getPositionalDeterministicEnvironment();

    uint64_t formulaIndex = 0;
    expected = parsePoints({"0.5,0.5", "1,1"});

    env.modelchecker().multi().setEncodingType(storm::MultiObjectiveModelCheckerEnvironment::EncodingType::Flow);
    STORM_SILENT_EXPECT_THROW(
        storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[formulaIndex]->asMultiObjectiveFormula()),
        storm::exceptions::InvalidOperationException);

    env.modelchecker().multi().setEncodingType(storm::MultiObjectiveModelCheckerEnvironment::EncodingType::Classic);
    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[formulaIndex]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isParetoCurveCheckResult());
    actual = result->asExplicitParetoCurveCheckResult<storm::RationalNumber>().getPoints();
    missingPoints = setMinus(expected, actual);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the expected solution are missing:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);
    incorrectPoints = setMinus(actual, expected);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the returned solution are not expected:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);

    ++formulaIndex;
    expected = parsePoints({"0,0"});

    env.modelchecker().multi().setEncodingType(storm::MultiObjectiveModelCheckerEnvironment::EncodingType::Flow);

    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[formulaIndex]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isParetoCurveCheckResult());
    actual = result->asExplicitParetoCurveCheckResult<storm::RationalNumber>().getPoints();
    missingPoints = setMinus(expected, actual);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the expected solution are missing:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);
    incorrectPoints = setMinus(actual, expected);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the returned solution are not expected:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);

    env.modelchecker().multi().setEncodingType(storm::MultiObjectiveModelCheckerEnvironment::EncodingType::Classic);
    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[formulaIndex]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isParetoCurveCheckResult());
    actual = result->asExplicitParetoCurveCheckResult<storm::RationalNumber>().getPoints();
    missingPoints = setMinus(expected, actual);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the expected solution are missing:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);
    incorrectPoints = setMinus(actual, expected);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the returned solution are not expected:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);

    ++formulaIndex;
    expected = parsePoints({"0,1"});

    env.modelchecker().multi().setEncodingType(storm::MultiObjectiveModelCheckerEnvironment::EncodingType::Flow);
    STORM_SILENT_EXPECT_THROW(
        storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[formulaIndex]->asMultiObjectiveFormula()),
        storm::exceptions::InvalidOperationException);

    env.modelchecker().multi().setEncodingType(storm::MultiObjectiveModelCheckerEnvironment::EncodingType::Classic);
    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[formulaIndex]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isParetoCurveCheckResult());
    actual = result->asExplicitParetoCurveCheckResult<storm::RationalNumber>().getPoints();
    missingPoints = setMinus(expected, actual);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the expected solution are missing:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);
    incorrectPoints = setMinus(actual, expected);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the returned solution are not expected:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);

    ++formulaIndex;
    expected = parsePoints({"0,1,0", "2,1,1"});

    env.modelchecker().multi().setEncodingType(storm::MultiObjectiveModelCheckerEnvironment::EncodingType::Flow);
    STORM_SILENT_EXPECT_THROW(
        storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[formulaIndex]->asMultiObjectiveFormula()),
        storm::exceptions::InvalidOperationException);

    env.modelchecker().multi().setEncodingType(storm::MultiObjectiveModelCheckerEnvironment::EncodingType::Classic);
    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[formulaIndex]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isParetoCurveCheckResult());
    actual = result->asExplicitParetoCurveCheckResult<storm::RationalNumber>().getPoints();
    missingPoints = setMinus(expected, actual);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the expected solution are missing:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);
    incorrectPoints = setMinus(actual, expected);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the returned solution are not expected:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);
}

TEST(MultiObjectiveSchedRestModelCheckerTest, compromise) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/mdp/multiobj_compromise.nm";
    std::string constantsString = "";
    std::string formulasAsString = "multi(Pmax=? [ F x=1], Pmax=? [ F x=2 ]);";
    formulasAsString += "\nmulti(R{\"test\"}min=? [F x=1], Pmax=? [F x=1 ]);";
    formulasAsString += "\nmulti(Pmax=? [F x=1], Pmax=? [F x=2 ], Pmax=? [F x=3 ]);";
    formulasAsString += "\nmulti(R{\"test\"}min=? [C], Pmin=? [F x=2 ], Pmin=? [F x=3]);";
    // programm, model, formula
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsString);
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas =
        storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulasAsString, program));
    std::shared_ptr<storm::models::sparse::Mdp<storm::RationalNumber>> mdp =
        storm::api::buildSparseModel<storm::RationalNumber>(program, formulas)->as<storm::models::sparse::Mdp<storm::RationalNumber>>();
    std::vector<Point> expected, actual;
    std::set<Point> incorrectPoints, missingPoints;
    std::unique_ptr<storm::modelchecker::CheckResult> result;

    storm::Environment env = getPositionalDeterministicEnvironment();

    uint64_t formulaIndex = 0;
    expected = parsePoints({"1,0", "0,1", "0.3,3/7"});

    env.modelchecker().multi().setEncodingType(storm::MultiObjectiveModelCheckerEnvironment::EncodingType::Flow);
    STORM_SILENT_EXPECT_THROW(
        storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[formulaIndex]->asMultiObjectiveFormula()),
        storm::exceptions::InvalidOperationException);

    env.modelchecker().multi().setEncodingType(storm::MultiObjectiveModelCheckerEnvironment::EncodingType::Classic);
    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[formulaIndex]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isParetoCurveCheckResult());
    actual = result->asExplicitParetoCurveCheckResult<storm::RationalNumber>().getPoints();
    missingPoints = setMinus(expected, actual);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the expected solution are missing:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);
    incorrectPoints = setMinus(actual, expected);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the returned solution are not expected:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);

    ++formulaIndex;
    expected = parsePoints({"0,0.3", "2,1"});

    env.modelchecker().multi().setEncodingType(storm::MultiObjectiveModelCheckerEnvironment::EncodingType::Flow);
    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[formulaIndex]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isParetoCurveCheckResult());
    actual = result->asExplicitParetoCurveCheckResult<storm::RationalNumber>().getPoints();
    missingPoints = setMinus(expected, actual);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the expected solution are missing:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);
    incorrectPoints = setMinus(actual, expected);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the returned solution are not expected:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);

    env.modelchecker().multi().setEncodingType(storm::MultiObjectiveModelCheckerEnvironment::EncodingType::Classic);
    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[formulaIndex]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isParetoCurveCheckResult());
    actual = result->asExplicitParetoCurveCheckResult<storm::RationalNumber>().getPoints();
    missingPoints = setMinus(expected, actual);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the expected solution are missing:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);
    incorrectPoints = setMinus(actual, expected);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the returned solution are not expected:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);

    ++formulaIndex;
    expected = parsePoints({"1,0,0", "0,1,0", "0.3,3/7,4/7"});

    env.modelchecker().multi().setEncodingType(storm::MultiObjectiveModelCheckerEnvironment::EncodingType::Flow);
    STORM_SILENT_EXPECT_THROW(
        storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[formulaIndex]->asMultiObjectiveFormula()),
        storm::exceptions::InvalidOperationException);

    env.modelchecker().multi().setEncodingType(storm::MultiObjectiveModelCheckerEnvironment::EncodingType::Classic);
    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[formulaIndex]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isParetoCurveCheckResult());
    actual = result->asExplicitParetoCurveCheckResult<storm::RationalNumber>().getPoints();
    missingPoints = setMinus(expected, actual);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the expected solution are missing:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);
    incorrectPoints = setMinus(actual, expected);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the returned solution are not expected:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);

    ++formulaIndex;
    expected = parsePoints({"0,1,0", "0,3/7,4/7"});

    env.modelchecker().multi().setEncodingType(storm::MultiObjectiveModelCheckerEnvironment::EncodingType::Flow);
    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[formulaIndex]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isParetoCurveCheckResult());
    actual = result->asExplicitParetoCurveCheckResult<storm::RationalNumber>().getPoints();
    missingPoints = setMinus(expected, actual);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the expected solution are missing:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);
    incorrectPoints = setMinus(actual, expected);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the returned solution are not expected:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);

    env.modelchecker().multi().setEncodingType(storm::MultiObjectiveModelCheckerEnvironment::EncodingType::Classic);
    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[formulaIndex]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isParetoCurveCheckResult());
    actual = result->asExplicitParetoCurveCheckResult<storm::RationalNumber>().getPoints();
    missingPoints = setMinus(expected, actual);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the expected solution are missing:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);
    incorrectPoints = setMinus(actual, expected);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the returned solution are not expected:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);

    env = getGoalDeterministicEnvironment();

    formulaIndex = 0;
    expected = parsePoints({"1,1"});

    env.modelchecker().multi().setEncodingType(storm::MultiObjectiveModelCheckerEnvironment::EncodingType::Flow);
    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[formulaIndex]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isParetoCurveCheckResult());
    actual = result->asExplicitParetoCurveCheckResult<storm::RationalNumber>().getPoints();
    missingPoints = setMinus(expected, actual);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the expected solution are missing:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);
    incorrectPoints = setMinus(actual, expected);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the returned solution are not expected:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);

    env.modelchecker().multi().setEncodingType(storm::MultiObjectiveModelCheckerEnvironment::EncodingType::Classic);
    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[formulaIndex]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isParetoCurveCheckResult());
    actual = result->asExplicitParetoCurveCheckResult<storm::RationalNumber>().getPoints();
    missingPoints = setMinus(expected, actual);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the expected solution are missing:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);
    incorrectPoints = setMinus(actual, expected);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the returned solution are not expected:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);

    ++formulaIndex;
    expected = parsePoints({"0,0.3", "2,1"});

    env.modelchecker().multi().setEncodingType(storm::MultiObjectiveModelCheckerEnvironment::EncodingType::Flow);
    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[formulaIndex]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isParetoCurveCheckResult());
    actual = result->asExplicitParetoCurveCheckResult<storm::RationalNumber>().getPoints();
    missingPoints = setMinus(expected, actual);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the expected solution are missing:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);
    incorrectPoints = setMinus(actual, expected);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the returned solution are not expected:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);

    env.modelchecker().multi().setEncodingType(storm::MultiObjectiveModelCheckerEnvironment::EncodingType::Classic);
    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[formulaIndex]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isParetoCurveCheckResult());
    actual = result->asExplicitParetoCurveCheckResult<storm::RationalNumber>().getPoints();
    missingPoints = setMinus(expected, actual);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the expected solution are missing:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);
    incorrectPoints = setMinus(actual, expected);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the returned solution are not expected:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);

    ++formulaIndex;
    expected = parsePoints({"1,1,0", "1,3/7,4/7"});

    env.modelchecker().multi().setEncodingType(storm::MultiObjectiveModelCheckerEnvironment::EncodingType::Flow);
    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[formulaIndex]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isParetoCurveCheckResult());
    actual = result->asExplicitParetoCurveCheckResult<storm::RationalNumber>().getPoints();
    missingPoints = setMinus(expected, actual);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the expected solution are missing:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);
    incorrectPoints = setMinus(actual, expected);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the returned solution are not expected:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);

    env.modelchecker().multi().setEncodingType(storm::MultiObjectiveModelCheckerEnvironment::EncodingType::Classic);
    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[formulaIndex]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isParetoCurveCheckResult());
    actual = result->asExplicitParetoCurveCheckResult<storm::RationalNumber>().getPoints();
    missingPoints = setMinus(expected, actual);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the expected solution are missing:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);
    incorrectPoints = setMinus(actual, expected);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the returned solution are not expected:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);

    ++formulaIndex;
    expected = parsePoints({"0,1,0", "0,3/7,4/7"});

    env.modelchecker().multi().setEncodingType(storm::MultiObjectiveModelCheckerEnvironment::EncodingType::Flow);
    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[formulaIndex]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isParetoCurveCheckResult());
    actual = result->asExplicitParetoCurveCheckResult<storm::RationalNumber>().getPoints();
    missingPoints = setMinus(expected, actual);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the expected solution are missing:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);
    incorrectPoints = setMinus(actual, expected);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the returned solution are not expected:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);

    env.modelchecker().multi().setEncodingType(storm::MultiObjectiveModelCheckerEnvironment::EncodingType::Classic);
    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[formulaIndex]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isParetoCurveCheckResult());
    actual = result->asExplicitParetoCurveCheckResult<storm::RationalNumber>().getPoints();
    missingPoints = setMinus(expected, actual);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the expected solution are missing:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);
    incorrectPoints = setMinus(actual, expected);
    EXPECT_TRUE(incorrectPoints.empty()) << "Some points of the returned solution are not expected:\n"
                                         << "Expected:\n"
                                         << toString(expected, true) << "Actual:\n"
                                         << toString(actual, true);
}
}  // namespace

#endif /* defined STORM_HAVE_HYPRO || defined STORM_HAVE_Z3_OPTIMIZE */
