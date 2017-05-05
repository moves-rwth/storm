#include "gtest/gtest.h"
#include "storm-config.h"

#include "storm/modelchecker/multiobjective/multiObjectiveModelChecking.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/modules/MultiObjectiveSettings.h"
#include "storm/settings/SettingsManager.h"
#include "storm/utility/storm.h"


TEST(SparseMaCbMultiObjectiveModelCheckerTest, server) {
    
    std::string programFile = STORM_TEST_RESOURCES_DIR "/ma/server.ma";
    std::string formulasAsString = "multi(T>=5/3 [ F \"error\" ], P>=7/12 [ F \"processB\" ]) "; // true
    formulasAsString += "; multi(T>=16/9 [ F \"error\" ], P>=7/12 [ F \"processB\" ]) "; // false

    storm::prism::Program program = storm::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, "");
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::extractFormulasFromProperties(storm::parsePropertiesForPrismProgram(formulasAsString, program));
    std::shared_ptr<storm::models::sparse::MarkovAutomaton<double>> ma = storm::buildSparseModel<double>(program, formulas)->as<storm::models::sparse::MarkovAutomaton<double>>();
    uint_fast64_t const initState = *ma->getInitialStates().begin();

    std::unique_ptr<storm::modelchecker::CheckResult> result;

    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(*ma, formulas[0]->asMultiObjectiveFormula(), storm::modelchecker::multiobjective::MultiObjectiveMethodSelection::ConstraintBased);
    ASSERT_TRUE(result->isExplicitQualitativeCheckResult());
    EXPECT_TRUE(result->asExplicitQualitativeCheckResult()[initState]);

    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(*ma, formulas[1]->asMultiObjectiveFormula(), storm::modelchecker::multiobjective::MultiObjectiveMethodSelection::ConstraintBased);
    ASSERT_TRUE(result->isExplicitQualitativeCheckResult());
    EXPECT_FALSE(result->asExplicitQualitativeCheckResult()[initState]);
}

TEST(SparseMaCbMultiObjectiveModelCheckerTest, jobscheduler_achievability_3Obj) {

    std::string programFile = STORM_TEST_RESOURCES_DIR "/ma/jobscheduler.ma";
    std::string formulasAsString = "multi(T<=1.31 [ F  \"all_jobs_finished\" ], P>=0.17 [ F<=0.2 \"half_of_jobs_finished\" ], P<=0.31 [ F \"slowest_before_fastest\"  ]) "; //true
    formulasAsString += "; multi(T<=1.29 [ F  \"all_jobs_finished\" ], P>=0.18 [ F<=0.2 \"half_of_jobs_finished\" ], P<=0.29 [ F \"slowest_before_fastest\"  ])"; //false

    storm::prism::Program program = storm::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, "");
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::extractFormulasFromProperties(storm::parsePropertiesForPrismProgram(formulasAsString, program));
    std::shared_ptr<storm::models::sparse::MarkovAutomaton<double>> ma = storm::buildSparseModel<double>(program, formulas)->as<storm::models::sparse::MarkovAutomaton<double>>();
    uint_fast64_t const initState = *ma->getInitialStates().begin();

    std::unique_ptr<storm::modelchecker::CheckResult> result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(*ma, formulas[0]->asMultiObjectiveFormula(), storm::modelchecker::multiobjective::MultiObjectiveMethodSelection::ConstraintBased);
    ASSERT_TRUE(result->isExplicitQualitativeCheckResult());
    EXPECT_TRUE(result->asExplicitQualitativeCheckResult()[initState]);

    std::unique_ptr<storm::modelchecker::CheckResult> result2 = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(*ma, formulas[1]->asMultiObjectiveFormula(), storm::modelchecker::multiobjective::MultiObjectiveMethodSelection::ConstraintBased);
    ASSERT_TRUE(result2->isExplicitQualitativeCheckResult());
    EXPECT_FALSE(result2->asExplicitQualitativeCheckResult()[initState]);
}

