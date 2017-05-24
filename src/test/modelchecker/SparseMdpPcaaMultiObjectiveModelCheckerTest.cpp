#include "gtest/gtest.h"
#include "storm-config.h"

#if defined STORM_HAVE_HYPRO || defined STORM_HAVE_Z3_OPTIMIZE

#include "storm/modelchecker/multiobjective/multiObjectiveModelChecking.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/SettingsManager.h"
#include "storm/utility/storm.h"


TEST(SparseMdpPcaaMultiObjectiveModelCheckerTest, consensus) {
    
    std::string programFile = STORM_TEST_RESOURCES_DIR "/mdp/multiobj_consensus2_3_2.nm";
    std::string formulasAsString = "multi(Pmax=? [ F \"one_proc_err\" ], P>=0.8916673903 [ G \"one_coin_ok\" ]) "; // numerical
    formulasAsString += "; \n multi(P>=0.1 [ F \"one_proc_err\" ], P>=0.8916673903 [ G \"one_coin_ok\" ])"; // achievability (true)
    formulasAsString += "; \n multi(P>=0.11 [ F \"one_proc_err\" ], P>=0.8916673903 [ G \"one_coin_ok\" ])"; // achievability (false)
    
    // programm, model,  formula
    storm::prism::Program program = storm::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, "");
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::extractFormulasFromProperties(storm::parsePropertiesForPrismProgram(formulasAsString, program));
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = storm::buildSparseModel<double>(program, formulas).getModel()->as<storm::models::sparse::Mdp<double>>();
    uint_fast64_t const initState = *mdp->getInitialStates().begin();;
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(*mdp, formulas[0]->asMultiObjectiveFormula(), storm::modelchecker::multiobjective::MultiObjectiveMethodSelection::Pcaa);
    ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
    EXPECT_NEAR(0.10833260970000025, result->asExplicitQuantitativeCheckResult<double>()[initState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(*mdp, formulas[1]->asMultiObjectiveFormula(), storm::modelchecker::multiobjective::MultiObjectiveMethodSelection::Pcaa);
    ASSERT_TRUE(result->isExplicitQualitativeCheckResult());
    EXPECT_TRUE(result->asExplicitQualitativeCheckResult()[initState]);
    
    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(*mdp, formulas[2]->asMultiObjectiveFormula(), storm::modelchecker::multiobjective::MultiObjectiveMethodSelection::Pcaa);
    ASSERT_TRUE(result->isExplicitQualitativeCheckResult());
    EXPECT_FALSE(result->asExplicitQualitativeCheckResult()[initState]);
    
}

TEST(SparseMdpPcaaMultiObjectiveModelCheckerTest, zeroconf) {
    
    std::string programFile = STORM_TEST_RESOURCES_DIR "/mdp/multiobj_zeroconf4.nm";
    std::string formulasAsString = "multi(Pmax=? [ F l=4 & ip=1 ] , P>=0.993141[ G (error=0) ]) "; // numerical
    
    // programm, model,  formula
    storm::prism::Program program = storm::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, "");
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::extractFormulasFromProperties(storm::parsePropertiesForPrismProgram(formulasAsString, program));
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = storm::buildSparseModel<double>(program, formulas).getModel()->as<storm::models::sparse::Mdp<double>>();
    uint_fast64_t const initState = *mdp->getInitialStates().begin();
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(*mdp, formulas[0]->asMultiObjectiveFormula(), storm::modelchecker::multiobjective::MultiObjectiveMethodSelection::Pcaa);
    ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
    EXPECT_NEAR(0.0003075787401574803, result->asExplicitQuantitativeCheckResult<double>()[initState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
}

TEST(SparseMdpPcaaMultiObjectiveModelCheckerTest, team3with3objectives) {
    
    std::string programFile = STORM_TEST_RESOURCES_DIR "/mdp/multiobj_team3.nm";
    std::string formulasAsString = "multi(Pmax=? [ F \"task1_compl\" ], R{\"w_1_total\"}>=2.210204082 [ C ],  P>=0.5 [ F \"task2_compl\" ])"; // numerical
    
    // programm, model,  formula
    storm::prism::Program program = storm::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, "");
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::extractFormulasFromProperties(storm::parsePropertiesForPrismProgram(formulasAsString, program));
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = storm::buildSparseModel<double>(program, formulas).getModel()->as<storm::models::sparse::Mdp<double>>();
    uint_fast64_t const initState = *mdp->getInitialStates().begin();
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(*mdp, formulas[0]->asMultiObjectiveFormula(), storm::modelchecker::multiobjective::MultiObjectiveMethodSelection::Pcaa);
    ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
    EXPECT_NEAR(0.7448979591841851, result->asExplicitQuantitativeCheckResult<double>()[initState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
}

TEST(SparseMdpPcaaMultiObjectiveModelCheckerTest, scheduler) {
    
    std::string programFile = STORM_TEST_RESOURCES_DIR "/mdp/multiobj_scheduler05.nm";
    std::string formulasAsString = "multi(R{\"time\"}<= 11.778[ F \"tasks_complete\" ], R{\"energy\"}<=1.45 [  F \"tasks_complete\" ]) ";
    
    // programm, model,  formula
    storm::prism::Program program = storm::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, "");
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::extractFormulasFromProperties(storm::parsePropertiesForPrismProgram(formulasAsString, program));
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = storm::buildSparseModel<double>(program, formulas).getModel()->as<storm::models::sparse::Mdp<double>>();
    uint_fast64_t const initState = *mdp->getInitialStates().begin();
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(*mdp, formulas[0]->asMultiObjectiveFormula(), storm::modelchecker::multiobjective::MultiObjectiveMethodSelection::Pcaa);
    EXPECT_TRUE(result->asExplicitQualitativeCheckResult()[initState]);
}

TEST(SparseMdpPcaaMultiObjectiveModelCheckerTest, dpm) {
    
    std::string programFile = STORM_TEST_RESOURCES_DIR "/mdp/multiobj_dpm100.nm";
    std::string formulasAsString = "multi(R{\"power\"}min=? [ C<=100 ], R{\"queue\"}<=70 [ C<=100 ])"; // numerical
    
    // programm, model,  formula
    storm::prism::Program program = storm::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, "");
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::extractFormulasFromProperties(storm::parsePropertiesForPrismProgram(formulasAsString, program));
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = storm::buildSparseModel<double>(program, formulas).getModel()->as<storm::models::sparse::Mdp<double>>();
    uint_fast64_t const initState = *mdp->getInitialStates().begin();
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(*mdp, formulas[0]->asMultiObjectiveFormula(), storm::modelchecker::multiobjective::MultiObjectiveMethodSelection::Pcaa);
    ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
    EXPECT_NEAR(121.6128842, result->asExplicitQuantitativeCheckResult<double>()[initState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
}




#endif /* STORM_HAVE_HYPRO || defined STORM_HAVE_Z3_OPTIMIZE */
