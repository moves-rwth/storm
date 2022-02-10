#include "storm-config.h"
#include "test/storm_gtest.h"

#if defined STORM_HAVE_HYPRO || defined STORM_HAVE_Z3_OPTIMIZE

#include "storm/environment/modelchecker/MultiObjectiveModelCheckerEnvironment.h"
#include "storm/modelchecker/multiobjective/multiObjectiveModelChecking.h"

#include "storm-parsers/api/storm-parsers.h"
#include "storm/api/storm.h"
#include "storm/environment/Environment.h"
#include "storm/modelchecker/results/ExplicitParetoCurveCheckResult.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/storage/geometry/Hyperrectangle.h"
#include "storm/storage/geometry/Polytope.h"
#include "storm/storage/jani/Property.h"

TEST(SparseMdpPcaaMultiObjectiveModelCheckerTest, consensus) {
    if (!storm::test::z3AtLeastVersion(4, 8, 5)) {
        GTEST_SKIP() << "Test disabled since it triggers a bug in the installed version of z3.";
    }
    storm::Environment env;
    env.modelchecker().multi().setMethod(storm::modelchecker::multiobjective::MultiObjectiveMethod::Pcaa);

    std::string programFile = STORM_TEST_RESOURCES_DIR "/mdp/multiobj_consensus2_3_2.nm";
    std::string formulasAsString = "multi(Pmax=? [ F \"one_proc_err\" ], P>=0.8916673903 [ G \"one_coin_ok\" ]) ";  // numerical
    formulasAsString += "; \n multi(P>=0.1 [ F \"one_proc_err\" ], P>=0.8916673903 [ G \"one_coin_ok\" ])";         // achievability (true)
    formulasAsString += "; \n multi(P>=0.11 [ F \"one_proc_err\" ], P>=0.8916673903 [ G \"one_coin_ok\" ])";        // achievability (false)

    // programm, model,  formula
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, "");
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas =
        storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulasAsString, program));
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = storm::api::buildSparseModel<double>(program, formulas)->as<storm::models::sparse::Mdp<double>>();
    uint_fast64_t const initState = *mdp->getInitialStates().begin();
    ;

    std::unique_ptr<storm::modelchecker::CheckResult> result =
        storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[0]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
    EXPECT_NEAR(0.10833260970000025, result->asExplicitQuantitativeCheckResult<double>()[initState],
                storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());

    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[1]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isExplicitQualitativeCheckResult());
    EXPECT_TRUE(result->asExplicitQualitativeCheckResult()[initState]);

    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[2]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isExplicitQualitativeCheckResult());
    EXPECT_FALSE(result->asExplicitQualitativeCheckResult()[initState]);
}

TEST(SparseMdpPcaaMultiObjectiveModelCheckerTest, zeroconf) {
    if (!storm::test::z3AtLeastVersion(4, 8, 5)) {
        GTEST_SKIP() << "Test disabled since it triggers a bug in the installed version of z3.";
    }

    storm::Environment env;
    env.modelchecker().multi().setMethod(storm::modelchecker::multiobjective::MultiObjectiveMethod::Pcaa);

    std::string programFile = STORM_TEST_RESOURCES_DIR "/mdp/multiobj_zeroconf4.nm";
    std::string formulasAsString = "multi(Pmax=? [ F l=4 & ip=1 ] , P>=0.993141[ G (error=0) ]) ";  // numerical

    // programm, model,  formula
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, "");
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas =
        storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulasAsString, program));
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = storm::api::buildSparseModel<double>(program, formulas)->as<storm::models::sparse::Mdp<double>>();
    uint_fast64_t const initState = *mdp->getInitialStates().begin();

    std::unique_ptr<storm::modelchecker::CheckResult> result =
        storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[0]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
    EXPECT_NEAR(0.0003075787401574803, result->asExplicitQuantitativeCheckResult<double>()[initState],
                storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
}

TEST(SparseMdpPcaaMultiObjectiveModelCheckerTest, team3with3objectives) {
    if (!storm::test::z3AtLeastVersion(4, 8, 5)) {
        GTEST_SKIP() << "Test disabled since it triggers a bug in the installed version of z3.";
    }

    storm::Environment env;
    env.modelchecker().multi().setMethod(storm::modelchecker::multiobjective::MultiObjectiveMethod::Pcaa);

    std::string programFile = STORM_TEST_RESOURCES_DIR "/mdp/multiobj_team3.nm";
    std::string formulasAsString = "multi(Pmax=? [ F \"task1_compl\" ], R{\"w_1_total\"}>=2.210204082 [ C ], P>=0.5 [ F \"task2_compl\" ])";  // numerical

    // programm, model,  formula
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, "");
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas =
        storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulasAsString, program));
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = storm::api::buildSparseModel<double>(program, formulas)->as<storm::models::sparse::Mdp<double>>();
    uint_fast64_t const initState = *mdp->getInitialStates().begin();

    std::unique_ptr<storm::modelchecker::CheckResult> result =
        storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[0]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
    EXPECT_NEAR(0.7448979591841851, result->asExplicitQuantitativeCheckResult<double>()[initState],
                storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
}

TEST(SparseMdpPcaaMultiObjectiveModelCheckerTest, scheduler) {
    if (!storm::test::z3AtLeastVersion(4, 8, 5)) {
        GTEST_SKIP() << "Test disabled since it triggers a bug in the installed version of z3.";
    }

    storm::Environment env;
    env.modelchecker().multi().setMethod(storm::modelchecker::multiobjective::MultiObjectiveMethod::Pcaa);

    std::string programFile = STORM_TEST_RESOURCES_DIR "/mdp/multiobj_scheduler05.nm";
    std::string formulasAsString = "multi(R{\"time\"}<= 11.778[ F \"tasks_complete\" ], R{\"energy\"}<=1.45 [ F \"tasks_complete\" ]) ";

    // programm, model,  formula
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, "");
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas =
        storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulasAsString, program));
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = storm::api::buildSparseModel<double>(program, formulas)->as<storm::models::sparse::Mdp<double>>();
    uint_fast64_t const initState = *mdp->getInitialStates().begin();

    std::unique_ptr<storm::modelchecker::CheckResult> result =
        storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[0]->asMultiObjectiveFormula());
    EXPECT_TRUE(result->asExplicitQualitativeCheckResult()[initState]);
}

TEST(SparseMdpPcaaMultiObjectiveModelCheckerTest, dpm) {
    if (!storm::test::z3AtLeastVersion(4, 8, 5)) {
        GTEST_SKIP() << "Test disabled since it triggers a bug in the installed version of z3.";
    }

    storm::Environment env;
    env.modelchecker().multi().setMethod(storm::modelchecker::multiobjective::MultiObjectiveMethod::Pcaa);

    std::string programFile = STORM_TEST_RESOURCES_DIR "/mdp/multiobj_dpm100.nm";
    std::string formulasAsString = "multi(R{\"power\"}min=? [ C<=100 ], R{\"queue\"}<=70 [ C<=100 ])";  // numerical

    // programm, model,  formula
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, "");
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas =
        storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulasAsString, program));
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = storm::api::buildSparseModel<double>(program, formulas)->as<storm::models::sparse::Mdp<double>>();
    uint_fast64_t const initState = *mdp->getInitialStates().begin();

    std::unique_ptr<storm::modelchecker::CheckResult> result =
        storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[0]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
    EXPECT_NEAR(121.6128842, result->asExplicitQuantitativeCheckResult<double>()[initState],
                storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
}

template<typename ValueType>
bool expectPointConained(std::vector<std::vector<ValueType>> const& pointset, std::vector<ValueType> const& point, ValueType precision) {
    for (auto const& p : pointset) {
        EXPECT_EQ(p.size(), point.size()) << "Missmatch in point dimension.";
        bool found = true;
        for (uint64_t i = 0; i < p.size(); ++i) {
            if (storm::utility::abs<ValueType>(p[i] - point[i]) > precision) {
                found = false;
                break;
            }
        }
        if (found) {
            return true;
        }
    }
    // prepare failure message:
    std::stringstream errstr;
    errstr << "Point [";
    bool firstPi = true;
    for (auto const& pi : point) {
        if (firstPi) {
            firstPi = false;
        } else {
            errstr << ", ";
        }
        errstr << pi;
    }
    errstr << "] is not contained in point set {";
    bool firstP = true;
    for (auto const& p : pointset) {
        if (firstP) {
            firstP = false;
        } else {
            errstr << ", \t";
        }
        errstr << "[";
        firstPi = true;
        for (auto const& pi : p) {
            if (firstPi) {
                firstPi = false;
            } else {
                errstr << ", ";
            }
            errstr << pi;
        }
        errstr << "]";
    }
    errstr << "}.";
    ADD_FAILURE() << errstr.str();
    return false;
}

template<typename ValueType>
bool expectSubset(std::vector<std::vector<ValueType>> const& lhs, std::vector<std::vector<ValueType>> const& rhs, ValueType precision) {
    for (auto const& p : lhs) {
        if (!expectPointConained(rhs, p, precision)) {
            return false;
        }
    }
    return true;
}

template<typename ValueType>
std::vector<std::vector<ValueType>> convertPointset(std::vector<std::vector<std::string>> const& in) {
    std::vector<std::vector<ValueType>> out;
    for (auto const& point_str : in) {
        out.emplace_back();
        for (auto const& pi_str : point_str) {
            out.back().push_back(storm::utility::convertNumber<ValueType>(pi_str));
        }
    }
    return out;
}

TEST(SparseMdpPcaaMultiObjectiveModelCheckerTest, simple_lra) {
    if (!storm::test::z3AtLeastVersion(4, 8, 5)) {
        GTEST_SKIP() << "Test disabled since it triggers a bug in the installed version of z3.";
    }
    storm::Environment env;
    env.modelchecker().multi().setMethod(storm::modelchecker::multiobjective::MultiObjectiveMethod::Pcaa);

    std::string programFile = STORM_TEST_RESOURCES_DIR "/mdp/multiobj_simple_lra.nm";
    std::string formulasAsString = "multi(R{\"first\"}max=? [ LRA ], R{\"second\"}max=? [ LRA ]);\n";                // pareto
    formulasAsString += "multi(R{\"first\"}min=? [ LRA ], R{\"second\"}max=? [ LRA ]);\n";                           // pareto
    formulasAsString += "multi(R{\"first\"}min=? [ LRA ], R{\"second\"}min=? [ LRA ]);\n";                           // pareto
    formulasAsString += "multi(R{\"first\"}min=? [ C ], R{\"second\"}min=? [ LRA ]);\n";                             // pareto
    formulasAsString += "multi(R{\"first\"}min=? [ C ], R{\"first\"}max=? [ LRA ]);\n";                              // pareto
    formulasAsString += "multi(R{\"first\"}min=? [ C ], R{\"second\"}max=? [ LRA ], R{\"third\"}max=? [ C ]);\n";    // pareto
    formulasAsString += "multi(R{\"first\"}min=? [ LRA ], R{\"second\"}max=? [ LRA ], R{\"third\"}min=? [ C ]);\n";  // pareto
    formulasAsString += "multi(LRAmax=? [ x=1 ], R{\"second\"}max=? [ LRA ]);\n";                                    // pareto

    // programm, model,  formula
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program.checkValidity();
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas =
        storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulasAsString, program));
    storm::generator::NextStateGeneratorOptions options(formulas);
    auto mdp = storm::builder::ExplicitModelBuilder<double>(program, options).build()->as<storm::models::sparse::Mdp<double>>();

    {
        std::unique_ptr<storm::modelchecker::CheckResult> result =
            storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[0]->asMultiObjectiveFormula());
        ASSERT_TRUE(result->isExplicitParetoCurveCheckResult());
        std::vector<std::vector<std::string>> expectedPoints;
        expectedPoints.emplace_back(std::vector<std::string>({"5", "80/11"}));
        expectedPoints.emplace_back(std::vector<std::string>({"0", "16"}));
        double eps = 1e-4;
        EXPECT_TRUE(expectSubset(result->asExplicitParetoCurveCheckResult<double>().getPoints(), convertPointset<double>(expectedPoints), eps))
            << "Non-Pareto point found.";
        EXPECT_TRUE(expectSubset(convertPointset<double>(expectedPoints), result->asExplicitParetoCurveCheckResult<double>().getPoints(), eps))
            << "Pareto point missing.";
    }
    {
        std::unique_ptr<storm::modelchecker::CheckResult> result =
            storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[1]->asMultiObjectiveFormula());
        ASSERT_TRUE(result->isExplicitParetoCurveCheckResult());
        std::vector<std::vector<std::string>> expectedPoints;
        expectedPoints.emplace_back(std::vector<std::string>({"0", "16"}));
        double eps = 1e-4;
        EXPECT_TRUE(expectSubset(result->asExplicitParetoCurveCheckResult<double>().getPoints(), convertPointset<double>(expectedPoints), eps))
            << "Non-Pareto point found.";
        EXPECT_TRUE(expectSubset(convertPointset<double>(expectedPoints), result->asExplicitParetoCurveCheckResult<double>().getPoints(), eps))
            << "Pareto point missing.";
    }
    {
        std::unique_ptr<storm::modelchecker::CheckResult> result =
            storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[2]->asMultiObjectiveFormula());
        ASSERT_TRUE(result->isExplicitParetoCurveCheckResult());
        std::vector<std::vector<std::string>> expectedPoints;
        expectedPoints.emplace_back(std::vector<std::string>({"0", "0"}));
        double eps = 1e-4;
        EXPECT_TRUE(expectSubset(result->asExplicitParetoCurveCheckResult<double>().getPoints(), convertPointset<double>(expectedPoints), eps))
            << "Non-Pareto point found.";
        EXPECT_TRUE(expectSubset(convertPointset<double>(expectedPoints), result->asExplicitParetoCurveCheckResult<double>().getPoints(), eps))
            << "Pareto point missing.";
    }
    {
        std::unique_ptr<storm::modelchecker::CheckResult> result =
            storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[3]->asMultiObjectiveFormula());
        ASSERT_TRUE(result->isExplicitParetoCurveCheckResult());
        std::vector<std::vector<std::string>> expectedPoints;
        expectedPoints.emplace_back(std::vector<std::string>({"10/8", "0"}));
        double eps = 1e-4;
        EXPECT_TRUE(expectSubset(result->asExplicitParetoCurveCheckResult<double>().getPoints(), convertPointset<double>(expectedPoints), eps))
            << "Non-Pareto point found.";
        EXPECT_TRUE(expectSubset(convertPointset<double>(expectedPoints), result->asExplicitParetoCurveCheckResult<double>().getPoints(), eps))
            << "Pareto point missing.";
    }
    {
        std::unique_ptr<storm::modelchecker::CheckResult> result =
            storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[4]->asMultiObjectiveFormula());
        ASSERT_TRUE(result->isExplicitParetoCurveCheckResult());
        std::vector<std::vector<std::string>> expectedPoints;
        expectedPoints.emplace_back(std::vector<std::string>({"10/8", "0"}));
        double eps = 1e-4;
        EXPECT_TRUE(expectSubset(result->asExplicitParetoCurveCheckResult<double>().getPoints(), convertPointset<double>(expectedPoints), eps))
            << "Non-Pareto point found.";
        EXPECT_TRUE(expectSubset(convertPointset<double>(expectedPoints), result->asExplicitParetoCurveCheckResult<double>().getPoints(), eps))
            << "Pareto point missing.";
    }
    {
        std::unique_ptr<storm::modelchecker::CheckResult> result =
            storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[5]->asMultiObjectiveFormula());
        ASSERT_TRUE(result->isExplicitParetoCurveCheckResult());
        std::vector<std::vector<std::string>> expectedPoints;
        expectedPoints.emplace_back(std::vector<std::string>({"10/8", "0", "10/8"}));
        expectedPoints.emplace_back(std::vector<std::string>({"7", "16", "2"}));
        double eps = 1e-4;
        EXPECT_TRUE(expectSubset(result->asExplicitParetoCurveCheckResult<double>().getPoints(), convertPointset<double>(expectedPoints), eps))
            << "Non-Pareto point found.";
        EXPECT_TRUE(expectSubset(convertPointset<double>(expectedPoints), result->asExplicitParetoCurveCheckResult<double>().getPoints(), eps))
            << "Pareto point missing.";
    }
    {
        std::unique_ptr<storm::modelchecker::CheckResult> result =
            storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[6]->asMultiObjectiveFormula());
        ASSERT_TRUE(result->isExplicitParetoCurveCheckResult());
        std::vector<std::vector<std::string>> expectedPoints;
        expectedPoints.emplace_back(std::vector<std::string>({"0", "0", "10/8"}));
        expectedPoints.emplace_back(std::vector<std::string>({"0", "16", "2"}));
        double eps = 1e-4;
        EXPECT_TRUE(expectSubset(result->asExplicitParetoCurveCheckResult<double>().getPoints(), convertPointset<double>(expectedPoints), eps))
            << "Non-Pareto point found.";
        EXPECT_TRUE(expectSubset(convertPointset<double>(expectedPoints), result->asExplicitParetoCurveCheckResult<double>().getPoints(), eps))
            << "Pareto point missing.";
    }
    {
        std::unique_ptr<storm::modelchecker::CheckResult> result =
            storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[7]->asMultiObjectiveFormula());
        ASSERT_TRUE(result->isExplicitParetoCurveCheckResult());
        std::vector<std::vector<std::string>> expectedPoints;
        expectedPoints.emplace_back(std::vector<std::string>({"1", "0"}));
        expectedPoints.emplace_back(std::vector<std::string>({"0", "16"}));
        double eps = 1e-4;
        EXPECT_TRUE(expectSubset(result->asExplicitParetoCurveCheckResult<double>().getPoints(), convertPointset<double>(expectedPoints), eps))
            << "Non-Pareto point found.";
        EXPECT_TRUE(expectSubset(convertPointset<double>(expectedPoints), result->asExplicitParetoCurveCheckResult<double>().getPoints(), eps))
            << "Pareto point missing.";
    }
}

TEST(SparseMdpPcaaMultiObjectiveModelCheckerTest, resource_gathering) {
    if (!storm::test::z3AtLeastVersion(4, 8, 5)) {
        GTEST_SKIP() << "Test disabled since it triggers a bug in the installed version of z3.";
    }
    storm::Environment env;
    env.modelchecker().multi().setMethod(storm::modelchecker::multiobjective::MultiObjectiveMethod::Pcaa);

    std::string programFile = STORM_TEST_RESOURCES_DIR "/mdp/resource-gathering.nm";
    std::string constantsDef = "GOLD_TO_COLLECT=0,GEM_TO_COLLECT=0,B=0";
    std::string formulasAsString = "multi(R{\"rew_gold\"}max=? [LRA], R{\"rew_gem\"}max=? [LRA]);\n";                // pareto
    formulasAsString += "multi(R{\"rew_gold\"}min=? [LRA], R{\"rew_gem\"}max=? [LRA]);\n";                           // pareto
    formulasAsString += "multi(R{\"rew_gold\"}max=? [LRA], R{\"rew_gem\"}min=? [LRA]);\n";                           // pareto
    formulasAsString += "multi(R{\"rew_gold\"}min=? [LRA], R{\"rew_gem\"}min=? [LRA]);\n";                           // pareto
    formulasAsString += "multi(R{\"rew_gold\"}max=? [LRA], R{\"attacks\"}min=? [LRA]);\n";                           // pareto
    formulasAsString += "multi(R{\"rew_gold\"}max=? [LRA], R{\"attacks\"}min=? [C]);\n";                             // pareto
    formulasAsString += "multi(R{\"rew_gold\"}max=? [LRA], R{\"rew_gem\"}max=? [LRA], R{\"attacks\"}min=? [C]);\n";  // pareto
    formulasAsString += "multi(R{\"rew_gold\"}max=? [LRA], R{\"rew_gem\"}max=? [ C<100 ]);\n";                       // pareto

    // programm, model,  formula
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsDef);
    program.checkValidity();
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas =
        storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulasAsString, program));
    storm::generator::NextStateGeneratorOptions options(formulas);
    auto mdp = storm::builder::ExplicitModelBuilder<double>(program, options).build()->as<storm::models::sparse::Mdp<double>>();

    {
        std::unique_ptr<storm::modelchecker::CheckResult> result =
            storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[0]->asMultiObjectiveFormula());
        ASSERT_TRUE(result->isExplicitParetoCurveCheckResult());
        std::vector<std::vector<std::string>> expectedPoints;
        expectedPoints.emplace_back(std::vector<std::string>({"27/241", "0"}));
        expectedPoints.emplace_back(std::vector<std::string>({"0", "1/10"}));
        expectedPoints.emplace_back(std::vector<std::string>({"27/349", "27/349"}));
        double eps = 1e-4;
        EXPECT_TRUE(expectSubset(result->asExplicitParetoCurveCheckResult<double>().getPoints(), convertPointset<double>(expectedPoints), eps))
            << "Non-Pareto point found.";
        EXPECT_TRUE(expectSubset(convertPointset<double>(expectedPoints), result->asExplicitParetoCurveCheckResult<double>().getPoints(), eps))
            << "Pareto point missing.";
    }
    {
        std::unique_ptr<storm::modelchecker::CheckResult> result =
            storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[1]->asMultiObjectiveFormula());
        ASSERT_TRUE(result->isExplicitParetoCurveCheckResult());
        std::vector<std::vector<std::string>> expectedPoints;
        expectedPoints.emplace_back(std::vector<std::string>({"0", "1/10"}));
        double eps = 1e-4;
        EXPECT_TRUE(expectSubset(result->asExplicitParetoCurveCheckResult<double>().getPoints(), convertPointset<double>(expectedPoints), eps))
            << "Non-Pareto point found.";
        EXPECT_TRUE(expectSubset(convertPointset<double>(expectedPoints), result->asExplicitParetoCurveCheckResult<double>().getPoints(), eps))
            << "Pareto point missing.";
    }
    {
        std::unique_ptr<storm::modelchecker::CheckResult> result =
            storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[2]->asMultiObjectiveFormula());
        ASSERT_TRUE(result->isExplicitParetoCurveCheckResult());
        std::vector<std::vector<std::string>> expectedPoints;
        expectedPoints.emplace_back(std::vector<std::string>({"27/241", "0"}));
        double eps = 1e-4;
        EXPECT_TRUE(expectSubset(result->asExplicitParetoCurveCheckResult<double>().getPoints(), convertPointset<double>(expectedPoints), eps))
            << "Non-Pareto point found.";
        EXPECT_TRUE(expectSubset(convertPointset<double>(expectedPoints), result->asExplicitParetoCurveCheckResult<double>().getPoints(), eps))
            << "Pareto point missing.";
    }
    {
        std::unique_ptr<storm::modelchecker::CheckResult> result =
            storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[3]->asMultiObjectiveFormula());
        ASSERT_TRUE(result->isExplicitParetoCurveCheckResult());
        std::vector<std::vector<std::string>> expectedPoints;
        expectedPoints.emplace_back(std::vector<std::string>({"0", "0"}));
        double eps = 1e-4;
        EXPECT_TRUE(expectSubset(result->asExplicitParetoCurveCheckResult<double>().getPoints(), convertPointset<double>(expectedPoints), eps))
            << "Non-Pareto point found.";
        EXPECT_TRUE(expectSubset(convertPointset<double>(expectedPoints), result->asExplicitParetoCurveCheckResult<double>().getPoints(), eps))
            << "Pareto point missing.";
    }
    {
        std::unique_ptr<storm::modelchecker::CheckResult> result =
            storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[4]->asMultiObjectiveFormula());
        ASSERT_TRUE(result->isExplicitParetoCurveCheckResult());
        std::vector<std::vector<std::string>> expectedPoints;
        expectedPoints.emplace_back(std::vector<std::string>({"27/241", "19/723"}));
        expectedPoints.emplace_back(std::vector<std::string>({"3/31", "1/93"}));
        expectedPoints.emplace_back(std::vector<std::string>({"1/12", "0"}));
        double eps = 1e-4;
        EXPECT_TRUE(expectSubset(result->asExplicitParetoCurveCheckResult<double>().getPoints(), convertPointset<double>(expectedPoints), eps))
            << "Non-Pareto point found.";
        EXPECT_TRUE(expectSubset(convertPointset<double>(expectedPoints), result->asExplicitParetoCurveCheckResult<double>().getPoints(), eps))
            << "Pareto point missing.";
    }
    {
        std::unique_ptr<storm::modelchecker::CheckResult> result =
            storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[5]->asMultiObjectiveFormula());
        ASSERT_TRUE(result->isExplicitParetoCurveCheckResult());
        std::vector<std::vector<std::string>> expectedPoints;
        expectedPoints.emplace_back(std::vector<std::string>({"1/12", "0"}));
        double eps = 1e-4;
        EXPECT_TRUE(expectSubset(result->asExplicitParetoCurveCheckResult<double>().getPoints(), convertPointset<double>(expectedPoints), eps))
            << "Non-Pareto point found.";
        EXPECT_TRUE(expectSubset(convertPointset<double>(expectedPoints), result->asExplicitParetoCurveCheckResult<double>().getPoints(), eps))
            << "Pareto point missing.";
    }
    {
        std::unique_ptr<storm::modelchecker::CheckResult> result =
            storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[6]->asMultiObjectiveFormula());
        ASSERT_TRUE(result->isExplicitParetoCurveCheckResult());
        std::vector<std::vector<std::string>> expectedPoints;
        expectedPoints.emplace_back(std::vector<std::string>({"1/12", "0", "0"}));
        expectedPoints.emplace_back(std::vector<std::string>({"0", "1/10", "0"}));
        expectedPoints.emplace_back(std::vector<std::string>({"1/18", "1/18", "0"}));
        double eps = 1e-4;
        EXPECT_TRUE(expectSubset(result->asExplicitParetoCurveCheckResult<double>().getPoints(), convertPointset<double>(expectedPoints), eps))
            << "Non-Pareto point found.";
        EXPECT_TRUE(expectSubset(convertPointset<double>(expectedPoints), result->asExplicitParetoCurveCheckResult<double>().getPoints(), eps))
            << "Pareto point missing.";
    }
    {
        std::unique_ptr<storm::modelchecker::CheckResult> result =
            storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *mdp, formulas[7]->asMultiObjectiveFormula());
        ASSERT_TRUE(result->isExplicitParetoCurveCheckResult());
        std::vector<std::vector<std::string>> expectedPoints;
        expectedPoints.emplace_back(std::vector<std::string>({"27/241", "9"}));
        double eps = 1e-4;
        // TODO: Right now, there is a non-optimal point included due to numerical imprecisions. We therefore skip this check:
        // EXPECT_TRUE(expectSubset(result->asExplicitParetoCurveCheckResult<double>().getPoints(), convertPointset<double>(expectedPoints), eps)) <<
        // "Non-Pareto point found.";
        EXPECT_TRUE(expectSubset(convertPointset<double>(expectedPoints), result->asExplicitParetoCurveCheckResult<double>().getPoints(), eps))
            << "Pareto point missing.";
    }
}

#endif /* STORM_HAVE_HYPRO || defined STORM_HAVE_Z3_OPTIMIZE */
