#include "test/storm_gtest.h"
#include "storm-config.h"

#if defined STORM_HAVE_HYPRO || defined STORM_HAVE_Z3_OPTIMIZE

#include "storm/modelchecker/multiobjective/multiObjectiveModelChecking.h"
#include "storm/environment/modelchecker/MultiObjectiveModelCheckerEnvironment.h"

#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitParetoCurveCheckResult.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/storage/geometry/Polytope.h"
#include "storm/storage/geometry/Hyperrectangle.h"
#include "storm/storage/jani/Property.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/modules/MultiObjectiveSettings.h"
#include "storm/settings/SettingsManager.h"
#include "storm/api/storm.h"
#include "storm-parsers/api/storm-parsers.h"

#include "storm/environment/Environment.h"

TEST(SparseMaPcaaMultiObjectiveModelCheckerTest, serverRationalNumbers) {
    storm::Environment env;
        env.modelchecker().multi().setMethod(storm::modelchecker::multiobjective::MultiObjectiveMethod::Pcaa);


    std::string programFile = STORM_TEST_RESOURCES_DIR "/ma/server.ma";
    std::string formulasAsString = "multi(Tmax=? [ F \"error\" ], Pmax=? [ F \"processB\" ]) "; // pareto
    
    // programm, model,  formula
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program.checkValidity();
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulasAsString, program));
    storm::generator::NextStateGeneratorOptions options(formulas);
    std::shared_ptr<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>> ma = storm::builder::ExplicitModelBuilder<storm::RationalNumber>(program, options).build()->as<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>();
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *ma, formulas[0]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isExplicitParetoCurveCheckResult());
    
    storm::RationalNumber p1 = storm::utility::convertNumber<storm::RationalNumber>(11.0); p1 /= storm::utility::convertNumber<storm::RationalNumber>(6.0);
    storm::RationalNumber p2 = storm::utility::convertNumber<storm::RationalNumber>(1.0); p2 /= storm::utility::convertNumber<storm::RationalNumber>(2.0);
    std::vector<storm::RationalNumber> p = {p1, p2};
    storm::RationalNumber q1 = storm::utility::convertNumber<storm::RationalNumber>(29.0); q1 /= storm::utility::convertNumber<storm::RationalNumber>(18.0);
    storm::RationalNumber q2 = storm::utility::convertNumber<storm::RationalNumber>(2.0); q2 /= storm::utility::convertNumber<storm::RationalNumber>(3.0);
    std::vector<storm::RationalNumber> q = {q1, q2};
    auto expectedAchievableValues = storm::storage::geometry::Polytope<storm::RationalNumber>::createDownwardClosure(std::vector<std::vector<storm::RationalNumber>>({p,q}));
    EXPECT_TRUE(expectedAchievableValues->contains(result->asExplicitParetoCurveCheckResult<storm::RationalNumber>().getUnderApproximation()));
    EXPECT_TRUE(result->asExplicitParetoCurveCheckResult<storm::RationalNumber>().getOverApproximation()->contains(expectedAchievableValues));
    
}


TEST(SparseMaPcaaMultiObjectiveModelCheckerTest, server) {
    
    if (!storm::test::z3AtLeastVersion(4,8,5)) {
        GTEST_SKIP() << "Test disabled since it triggers a bug in the installed version of z3.";
    }
    
    storm::Environment env;
    env.modelchecker().multi().setMethod(storm::modelchecker::multiobjective::MultiObjectiveMethod::Pcaa);

    std::string programFile = STORM_TEST_RESOURCES_DIR "/ma/server.ma";
    std::string formulasAsString = "multi(Tmax=? [ F \"error\" ], Pmax=? [ F \"processB\" ]) "; // pareto

    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, "");
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulasAsString, program));
    std::shared_ptr<storm::models::sparse::MarkovAutomaton<double>> ma = storm::api::buildSparseModel<double>(program, formulas)->as<storm::models::sparse::MarkovAutomaton<double>>();

    std::unique_ptr<storm::modelchecker::CheckResult> result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *ma, formulas[0]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isExplicitParetoCurveCheckResult());
    
    // we do our checks with rationals to avoid numerical issues when doing polytope computations...
    std::vector<double> p = {11.0/6.0, 1.0/2.0};
    std::vector<double> q = {29.0/18.0, 2.0/3.0};
    auto expectedAchievableValues = storm::storage::geometry::Polytope<storm::RationalNumber>::createDownwardClosure(
            std::vector<std::vector<storm::RationalNumber>>({storm::utility::vector::convertNumericVector<storm::RationalNumber>(p),
                                                             storm::utility::vector::convertNumericVector<storm::RationalNumber>(q)}));
    // due to precision issues, we enlarge one of the polytopes before checking containement
    storm::RationalNumber eps = storm::utility::convertNumber<storm::RationalNumber>(storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    std::vector<storm::RationalNumber> lb(2,-eps), ub(2,eps);
    auto bloatingBox = storm::storage::geometry::Hyperrectangle<storm::RationalNumber>(lb,ub).asPolytope();

    EXPECT_TRUE(expectedAchievableValues->minkowskiSum(bloatingBox)->contains(result->asExplicitParetoCurveCheckResult<double>().getUnderApproximation()->convertNumberRepresentation<storm::RationalNumber>()));
    EXPECT_TRUE(result->asExplicitParetoCurveCheckResult<double>().getOverApproximation()->convertNumberRepresentation<storm::RationalNumber>()->minkowskiSum(bloatingBox)->contains(expectedAchievableValues));

}

TEST(SparseMaPcaaMultiObjectiveModelCheckerTest, jobscheduler_pareto_3Obj) {
    if (!storm::test::z3AtLeastVersion(4,8,5)) {
        GTEST_SKIP() << "Test disabled since it triggers a bug in the installed version of z3.";
    }

    storm::Environment env;
    env.modelchecker().multi().setMethod(storm::modelchecker::multiobjective::MultiObjectiveMethod::Pcaa);

    std::string programFile = STORM_TEST_RESOURCES_DIR "/ma/jobscheduler.ma";
    std::string formulasAsString = "multi(Tmin=? [ F \"all_jobs_finished\" ], Pmax=? [ F<=0.2 \"half_of_jobs_finished\" ], Pmin=? [ F \"slowest_before_fastest\" ]) ";

    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, "");
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulasAsString, program));
    std::shared_ptr<storm::models::sparse::MarkovAutomaton<double>> ma = storm::api::buildSparseModel<double>(program, formulas)->as<storm::models::sparse::MarkovAutomaton<double>>();

    std::unique_ptr<storm::modelchecker::CheckResult> result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *ma, formulas[0]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isExplicitParetoCurveCheckResult());

    std::vector<double> j12 = {1.266666667,0.1617571721,0.5};
    std::vector<double> j13 = {1.283333333,0.1707737575,0.25};
    std::vector<double> j23 = {1.333333333,0.1978235137,0.1};

    // we do our checks with rationals to avoid numerical issues when doing polytope computations...
    auto expectedAchievableValues = storm::storage::geometry::Polytope<storm::RationalNumber>::create(
            std::vector<std::vector<storm::RationalNumber>>({storm::utility::vector::convertNumericVector<storm::RationalNumber>(j12),
                                                             storm::utility::vector::convertNumericVector<storm::RationalNumber>(j13),
                                                             storm::utility::vector::convertNumericVector<storm::RationalNumber>(j23)}));
    // due to precision issues, we enlarge one of the polytopes before checking containement
    storm::RationalNumber eps = storm::utility::convertNumber<storm::RationalNumber>(storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    std::vector<storm::RationalNumber> lb(3,-eps), ub(3,eps);
    auto bloatingBox = storm::storage::geometry::Hyperrectangle<storm::RationalNumber>(lb,ub).asPolytope();

    EXPECT_TRUE(result->asExplicitParetoCurveCheckResult<double>().getOverApproximation()->convertNumberRepresentation<storm::RationalNumber>()->minkowskiSum(bloatingBox)->contains(expectedAchievableValues));
}

TEST(SparseMaPcaaMultiObjectiveModelCheckerTest, jobscheduler_achievability_3Obj) {
    storm::Environment env;
    env.modelchecker().multi().setMethod(storm::modelchecker::multiobjective::MultiObjectiveMethod::Pcaa);

    std::string programFile = STORM_TEST_RESOURCES_DIR "/ma/jobscheduler.ma";
    std::string formulasAsString = "multi(T<=1.31 [ F \"all_jobs_finished\" ], P>=0.17 [ F<=0.2 \"half_of_jobs_finished\" ], P<=0.31 [ F \"slowest_before_fastest\" ]) "; //true
    formulasAsString += "; multi(T<=1.29 [ F \"all_jobs_finished\" ], P>=0.18 [ F<=0.2 \"half_of_jobs_finished\" ], P<=0.29 [ F \"slowest_before_fastest\" ])"; //false

    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, "");
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulasAsString, program));
    std::shared_ptr<storm::models::sparse::MarkovAutomaton<double>> ma = storm::api::buildSparseModel<double>(program, formulas)->as<storm::models::sparse::MarkovAutomaton<double>>();
    uint_fast64_t const initState = *ma->getInitialStates().begin();

    std::unique_ptr<storm::modelchecker::CheckResult> result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *ma, formulas[0]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isExplicitQualitativeCheckResult());
    EXPECT_TRUE(result->asExplicitQualitativeCheckResult()[initState]);

    std::unique_ptr<storm::modelchecker::CheckResult> result2 = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *ma, formulas[1]->asMultiObjectiveFormula());
    ASSERT_TRUE(result2->isExplicitQualitativeCheckResult());
    EXPECT_FALSE(result2->asExplicitQualitativeCheckResult()[initState]);
}

TEST(SparseMaPcaaMultiObjectiveModelCheckerTest, jobscheduler_quantitative_3Obj) {
    if (!storm::test::z3AtLeastVersion(4,8,5)) {
        GTEST_SKIP() << "Test disabled since it triggers a bug in the installed version of z3.";
    }
    storm::Environment env;
    env.modelchecker().multi().setMethod(storm::modelchecker::multiobjective::MultiObjectiveMethod::Pcaa);

    std::string programFile = STORM_TEST_RESOURCES_DIR "/ma/jobscheduler.ma";
    std::string formulasAsString = "multi(Tmin=? [ F \"all_jobs_finished\" ], P>=0.1797900683 [ F<=0.2 \"half_of_jobs_finished\" ], P<=0.3 [ F \"slowest_before_fastest\" ]) "; //quantitative
    formulasAsString += "; multi(T<=1.26 [ F \"all_jobs_finished\" ], P>=0.2 [ F<=0.2 \"half_of_jobs_finished\" ], Pmin=? [ F \"slowest_before_fastest\" ])"; //false

    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, "");
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulasAsString, program));
    std::shared_ptr<storm::models::sparse::MarkovAutomaton<double>> ma = storm::api::buildSparseModel<double>(program, formulas)->as<storm::models::sparse::MarkovAutomaton<double>>();
    uint_fast64_t const initState = *ma->getInitialStates().begin();


    std::unique_ptr<storm::modelchecker::CheckResult> result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *ma, formulas[0]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
    EXPECT_NEAR(1.3, result->asExplicitQuantitativeCheckResult<double>()[initState], storm::settings::getModule<storm::settings::modules::MultiObjectiveSettings>().getPrecision());

    std::unique_ptr<storm::modelchecker::CheckResult> result2 = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *ma, formulas[1]->asMultiObjectiveFormula());
    ASSERT_TRUE(result2->isExplicitQualitativeCheckResult());
    EXPECT_FALSE(result2->asExplicitQualitativeCheckResult()[initState]);
}

TEST(SparseMaPcaaMultiObjectiveModelCheckerTest, jobscheduler_pareto_2Obj) {
    storm::Environment env;
    env.modelchecker().multi().setMethod(storm::modelchecker::multiobjective::MultiObjectiveMethod::Pcaa);

    std::string programFile = STORM_TEST_RESOURCES_DIR "/ma/jobscheduler.ma";
    std::string formulasAsString = "multi( Pmax=? [ F<=0.1 \"one_job_finished\"], Pmin=? [F<=0.2 \"all_jobs_finished\"]) ";

    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, "");
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulasAsString, program));
    std::shared_ptr<storm::models::sparse::MarkovAutomaton<double>> ma = storm::api::buildSparseModel<double>(program, formulas)->as<storm::models::sparse::MarkovAutomaton<double>>();

    std::unique_ptr<storm::modelchecker::CheckResult> result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(env, *ma, formulas[0]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isExplicitParetoCurveCheckResult());

    std::vector<double> j12 = {0.2591835573, 0.01990529674};
    std::vector<double> j13 = {0.329682099, 0.01970194998};
    std::vector<double> j23 = {0.3934717664, 0.01948095743};

    // we do our checks with rationals to avoid numerical issues when doing polytope computations...
    auto expectedAchievableValues = storm::storage::geometry::Polytope<storm::RationalNumber>::create(
            std::vector<std::vector<storm::RationalNumber>>({storm::utility::vector::convertNumericVector<storm::RationalNumber>(j12),
                                                             storm::utility::vector::convertNumericVector<storm::RationalNumber>(j13),
                                                             storm::utility::vector::convertNumericVector<storm::RationalNumber>(j23)}));
    // due to precision issues, we enlarge one of the polytopes before checking containement
    storm::RationalNumber eps = storm::utility::convertNumber<storm::RationalNumber>(storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    std::vector<storm::RationalNumber> lb(2,-eps), ub(2,eps);
    auto bloatingBox = storm::storage::geometry::Hyperrectangle<storm::RationalNumber>(lb,ub).asPolytope();

    EXPECT_TRUE(result->asExplicitParetoCurveCheckResult<double>().getOverApproximation()->convertNumberRepresentation<storm::RationalNumber>()->minkowskiSum(bloatingBox)->contains(expectedAchievableValues));

}


#endif /*  defined STORM_HAVE_HYPRO || defined STORM_HAVE_Z3_OPTIMIZE */
