#include "gtest/gtest.h"
#include "storm-config.h"

#ifdef STORM_HAVE_HYPRO

#include "src/modelchecker/multiobjective/SparseMdpMultiObjectiveModelChecker.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "src/models/sparse/Mdp.h"
#include "src/settings/modules/GeneralSettings.h"
#include "src/settings/SettingsManager.h"
#include "src/utility/storm.h"



TEST(SparseMdpMultiObjectiveModelCheckerTest, probEqual1Objective) {
    
    std::string programFile = STORM_CPP_TESTS_BASE_PATH "/functional/modelchecker/multiobjective1.nm";
    std::string formulasAsString = "multi(Rmax=? [ F s=2 ], P>=1 [ s=0 U s=1 ]) ";
    formulasAsString += "; \n multi(Rmax=? [ F s=2 ], P>=1 [ F s=1 ]) ";
    
    // programm, model,  formula
    storm::prism::Program program = storm::parseProgram(programFile);
    program.checkValidity();
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::parseFormulasForProgram(formulasAsString, program);
    typename storm::builder::ExplicitPrismModelBuilder<double>::Options options = storm::builder::ExplicitPrismModelBuilder<double>::Options(formulas);
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = storm::builder::ExplicitPrismModelBuilder<double>(program, options).translate()->as<storm::models::sparse::Mdp<double>>();
    uint_fast64_t const initState = *mdp->getInitialStates().begin();
    
    storm::modelchecker::SparseMdpMultiObjectiveModelChecker<storm::models::sparse::Mdp<double>> checker(*mdp);
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[0], true));
    ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
    EXPECT_NEAR(7.647058824, result->asExplicitQuantitativeCheckResult<double>()[initState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[1], true));
    ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
    EXPECT_NEAR(7.647058824, result->asExplicitQuantitativeCheckResult<double>()[initState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
}

TEST(SparseMdpMultiObjectiveModelCheckerTest, probEqual0Objective) {
    
    std::string programFile = STORM_CPP_TESTS_BASE_PATH "/functional/modelchecker/multiobjective1.nm";
    std::string formulasAsString = "multi(Rmax=? [ F s=2 ], P<=0 [ s=0 U s=1 ]) ";
    formulasAsString += "; \n multi(Rmax=? [ F s=2 ], P<=0 [ F s=1 ]) ";
    
    // programm, model,  formula
    storm::prism::Program program = storm::parseProgram(programFile);
    program.checkValidity();
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::parseFormulasForProgram(formulasAsString, program);
    typename storm::builder::ExplicitPrismModelBuilder<double>::Options options = storm::builder::ExplicitPrismModelBuilder<double>::Options(formulas);
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = storm::builder::ExplicitPrismModelBuilder<double>(program, options).translate()->as<storm::models::sparse::Mdp<double>>();
    uint_fast64_t const initState = *mdp->getInitialStates().begin();
    
    storm::modelchecker::SparseMdpMultiObjectiveModelChecker<storm::models::sparse::Mdp<double>> checker(*mdp);
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[0], true));
    ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
    EXPECT_NEAR(0.0, result->asExplicitQuantitativeCheckResult<double>()[initState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[1], true));
    ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
    EXPECT_NEAR(0.0, result->asExplicitQuantitativeCheckResult<double>()[initState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
}

TEST(SparseMdpMultiObjectiveModelCheckerTest, preprocessorResultsTest) {
    
    std::string programFile = STORM_CPP_TESTS_BASE_PATH "/functional/modelchecker/multiobjective2.nm";
    std::string formulasAsString = "multi(Rmin=? [ F s=2 ], P>=1 [ s!=1 U s=2 ]) ";
    formulasAsString += "; \n multi(Rmax=? [ F s=2 ], P>=1 [ s!=1 U s=2 ]) ";
    formulasAsString += "; \n multi(R<=0 [ F s=2 ], P>=1 [ s!=1 U s=2 ]) ";
    formulasAsString += "; \n multi(R>0 [ F s=2 ], P>=1 [ s!=1 U s=2 ]) ";
    formulasAsString += "; \n multi(Rmin=? [ F s=2 ], P>=1 [ F s=1 ], P>=1 [F s=2]) ";
    formulasAsString += "; \n multi(Rmax=? [ F s=2 ], P>=1 [ F s=1 ], P>=1 [F s=2]) ";
    formulasAsString += "; \n multi(R>=300 [ F s=2 ], P>=1 [ F s=1 ], P>=1 [F s=2]) ";
    formulasAsString += "; \n multi(R<10 [ F s=2 ], P>=1 [ F s=1 ], P>=1 [F s=2]) ";
    formulasAsString += "; \n multi(Rmin=? [ C ]) ";
    formulasAsString += "; \n multi(Rmax=? [ C ]) ";
    formulasAsString += "; \n multi(R>1 [ C ]) ";
    formulasAsString += "; \n multi(R<1 [ C ]) ";
    
    // programm, model,  formula
    storm::prism::Program program = storm::parseProgram(programFile);
    program.checkValidity();
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::parseFormulasForProgram(formulasAsString, program);
    typename storm::builder::ExplicitPrismModelBuilder<double>::Options options = storm::builder::ExplicitPrismModelBuilder<double>::Options(formulas);
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = storm::builder::ExplicitPrismModelBuilder<double>(program, options).translate()->as<storm::models::sparse::Mdp<double>>();
    uint_fast64_t const initState = *mdp->getInitialStates().begin();
    
    storm::modelchecker::SparseMdpMultiObjectiveModelChecker<storm::models::sparse::Mdp<double>> checker(*mdp);
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[0], true));
    ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
    EXPECT_NEAR(0.0, result->asExplicitQuantitativeCheckResult<double>()[initState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[1], true));
    ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
    EXPECT_NEAR(0.0, result->asExplicitQuantitativeCheckResult<double>()[initState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[2], true));
    ASSERT_TRUE(result->isExplicitQualitativeCheckResult());
    EXPECT_TRUE(result->asExplicitQualitativeCheckResult()[initState]);
    
    result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[3], true));
    ASSERT_TRUE(result->isExplicitQualitativeCheckResult());
    EXPECT_FALSE(result->asExplicitQualitativeCheckResult()[initState]);
    
    result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[4], true));
    ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
    EXPECT_NEAR(10.0, result->asExplicitQuantitativeCheckResult<double>()[initState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[5], true));
    ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
    EXPECT_EQ(storm::utility::infinity<double>(), result->asExplicitQuantitativeCheckResult<double>()[initState]);
    
    result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[6], true));
    ASSERT_TRUE(result->isExplicitQualitativeCheckResult());
    EXPECT_TRUE(result->asExplicitQualitativeCheckResult()[initState]);
    
    result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[7], true));
    ASSERT_TRUE(result->isExplicitQualitativeCheckResult());
    EXPECT_FALSE(result->asExplicitQualitativeCheckResult()[initState]);
    
    result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[8], true));
    ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
    EXPECT_EQ(storm::utility::infinity<double>(), result->asExplicitQuantitativeCheckResult<double>()[initState]);
    
    result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[9], true));
    ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
    EXPECT_EQ(storm::utility::infinity<double>(), result->asExplicitQuantitativeCheckResult<double>()[initState]);
    
    result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[10], true));
    ASSERT_TRUE(result->isExplicitQualitativeCheckResult());
    EXPECT_TRUE(result->asExplicitQualitativeCheckResult()[initState]);
    
    result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[11], true));
    ASSERT_TRUE(result->isExplicitQualitativeCheckResult());
    EXPECT_FALSE(result->asExplicitQualitativeCheckResult()[initState]);
}

TEST(SparseMdpMultiObjectiveModelCheckerTest, consensus) {
    
    std::string programFile = STORM_CPP_BASE_PATH "/examples/multiobjective/mdp/consensus/consensus2_3_2.nm";
    std::string formulasAsString = "multi(Pmax=? [ F \"one_proc_err\" ], P>=0.8916673903 [ G \"one_coin_ok\" ]) "; // numerical
    formulasAsString += "; \n multi(P>=0.1 [ F \"one_proc_err\" ], P>=0.8916673903 [ G \"one_coin_ok\" ])"; // achievability (true)
    formulasAsString += "; \n multi(P>=0.11 [ F \"one_proc_err\" ], P>=0.8916673903 [ G \"one_coin_ok\" ])"; // achievability (false)
    
    // programm, model,  formula
    storm::prism::Program program = storm::parseProgram(programFile);
    program.checkValidity();
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::parseFormulasForProgram(formulasAsString, program);
    typename storm::builder::ExplicitPrismModelBuilder<double>::Options options = storm::builder::ExplicitPrismModelBuilder<double>::Options(formulas);
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = storm::builder::ExplicitPrismModelBuilder<double>(program, options).translate()->as<storm::models::sparse::Mdp<double>>();
    uint_fast64_t const initState = *mdp->getInitialStates().begin();
    
    storm::modelchecker::SparseMdpMultiObjectiveModelChecker<storm::models::sparse::Mdp<double>> checker(*mdp);
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[0], true));
    ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
    EXPECT_NEAR(0.10833260970000025, result->asExplicitQuantitativeCheckResult<double>()[initState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[1], true));
    ASSERT_TRUE(result->isExplicitQualitativeCheckResult());
    EXPECT_TRUE(result->asExplicitQualitativeCheckResult()[initState]);
    
    result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[2], true));
    ASSERT_TRUE(result->isExplicitQualitativeCheckResult());
    EXPECT_FALSE(result->asExplicitQualitativeCheckResult()[initState]);
    
}

TEST(SparseMdpMultiObjectiveModelCheckerTest, zeroconf) {
    
    std::string programFile = STORM_CPP_BASE_PATH "/examples/multiobjective/mdp/zeroconf/zeroconf4.nm";
    std::string formulasAsString = "multi(Pmax=? [ F l=4 & ip=1 ] , P>=0.993141[ G (error=0) ]) "; // numerical
    formulasAsString += "; \n multi(P>=0.0003 [ F l=4 & ip=1 ] , P>=0.993141[ G (error=0) ])"; // achievability (true)
    formulasAsString += "; \n multi(P>=0.00031 [ F l=4 & ip=1 ] , P>=0.993141[ G (error=0) ])"; // achievability (false)
    
    // programm, model,  formula
    storm::prism::Program program = storm::parseProgram(programFile);
    program.checkValidity();
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::parseFormulasForProgram(formulasAsString, program);
    typename storm::builder::ExplicitPrismModelBuilder<double>::Options options = storm::builder::ExplicitPrismModelBuilder<double>::Options(formulas);
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = storm::builder::ExplicitPrismModelBuilder<double>(program, options).translate()->as<storm::models::sparse::Mdp<double>>();
    uint_fast64_t const initState = *mdp->getInitialStates().begin();
    
    storm::modelchecker::SparseMdpMultiObjectiveModelChecker<storm::models::sparse::Mdp<double>> checker(*mdp);
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[0], true));
    ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
    EXPECT_NEAR(0.0003075787401574803, result->asExplicitQuantitativeCheckResult<double>()[initState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[1], true));
    ASSERT_TRUE(result->isExplicitQualitativeCheckResult());
    EXPECT_TRUE(result->asExplicitQualitativeCheckResult()[initState]);
    
    result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[2], true));
    ASSERT_TRUE(result->isExplicitQualitativeCheckResult());
    EXPECT_FALSE(result->asExplicitQualitativeCheckResult()[initState]);
    
}

TEST(SparseMdpMultiObjectiveModelCheckerTest, zeroconfTb) {
    
    std::string programFile = STORM_CPP_BASE_PATH "/examples/multiobjective/mdp/zeroconf-tb/zeroconf-tb2_14.nm";
    std::string formulasAsString = " multi(Pmax=? [ F time_error=1 ] , P>=0.81[ G (error=0) ])"; // numerical
    formulasAsString += "; \n  multi(P>=0.00000008 [ F time_error=1 ] , P>=0.81[ G (error=0) ])"; // achievability (true)
    formulasAsString += "; \n  multi(P>=0.000000081 [ F time_error=1 ] , P>=0.81[ G (error=0) ])"; // achievability (false)
    
    // programm, model,  formula
    storm::prism::Program program = storm::parseProgram(programFile);
    program.checkValidity();
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::parseFormulasForProgram(formulasAsString, program);
    typename storm::builder::ExplicitPrismModelBuilder<double>::Options options = storm::builder::ExplicitPrismModelBuilder<double>::Options(formulas);
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = storm::builder::ExplicitPrismModelBuilder<double>(program, options).translate()->as<storm::models::sparse::Mdp<double>>();
    uint_fast64_t const initState = *mdp->getInitialStates().begin();
    
    storm::modelchecker::SparseMdpMultiObjectiveModelChecker<storm::models::sparse::Mdp<double>> checker(*mdp);
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[0], true));
    ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
    EXPECT_NEAR(8.059348391417451e-8, result->asExplicitQuantitativeCheckResult<double>()[initState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[1], true));
    ASSERT_TRUE(result->isExplicitQualitativeCheckResult());
    EXPECT_TRUE(result->asExplicitQualitativeCheckResult()[initState]);
    
    result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[2], true));
    ASSERT_TRUE(result->isExplicitQualitativeCheckResult());
    EXPECT_FALSE(result->asExplicitQualitativeCheckResult()[initState]);
    
}

TEST(SparseMdpMultiObjectiveModelCheckerTest, team3with2objectives) {
    
    std::string programFile = STORM_CPP_BASE_PATH "/examples/multiobjective/mdp/team/team2obj_3.nm";
    std::string formulasAsString = " multi(Pmax=? [ F task1_completed ], R{\"w_1_total\"}>=2.210204082 [ C ])"; // numerical
    formulasAsString += "; \n  multi(P>=0.871 [ F task1_completed ], R{\"w_1_total\"}>=2.210204082 [ C ])"; // achievability (true)
    formulasAsString += "; \n  multi(P>=0.872 [ F task1_completed ], R{\"w_1_total\"}>=2.210204082 [ C ])"; // achievability (false)
    
    // programm, model,  formula
    storm::prism::Program program = storm::parseProgram(programFile);
    program.checkValidity();
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::parseFormulasForProgram(formulasAsString, program);
    typename storm::builder::ExplicitPrismModelBuilder<double>::Options options = storm::builder::ExplicitPrismModelBuilder<double>::Options(formulas);
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = storm::builder::ExplicitPrismModelBuilder<double>(program, options).translate()->as<storm::models::sparse::Mdp<double>>();
    uint_fast64_t const initState = *mdp->getInitialStates().begin();
    
    storm::modelchecker::SparseMdpMultiObjectiveModelChecker<storm::models::sparse::Mdp<double>> checker(*mdp);
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[0], true));
    ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
    EXPECT_NEAR(0.8714285710612256, result->asExplicitQuantitativeCheckResult<double>()[initState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[1], true));
    ASSERT_TRUE(result->isExplicitQualitativeCheckResult());
    EXPECT_TRUE(result->asExplicitQualitativeCheckResult()[initState]);
    
    result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[2], true));
    ASSERT_TRUE(result->isExplicitQualitativeCheckResult());
    EXPECT_FALSE(result->asExplicitQualitativeCheckResult()[initState]);
    
}

TEST(SparseMdpMultiObjectiveModelCheckerTest, team3with3objectives) {
    
    std::string programFile = STORM_CPP_BASE_PATH "/examples/multiobjective/mdp/team/team3obj_3.nm";
    std::string formulasAsString = "multi(Pmax=? [ F task1_completed ], R{\"w_1_total\"}>=2.210204082 [ C ],  P>=0.5 [ F task2_completed ])"; // numerical
    formulasAsString += "; \n  multi(P>=0.744 [ F task1_completed ], R{\"w_1_total\"}>=2.210204082 [ C ],  P>=0.5 [ F task2_completed ])"; // achievability (true)
    formulasAsString += "; \n  multi(P>=0.745 [ F task1_completed ], R{\"w_1_total\"}>=2.210204082 [ C ],  P>=0.5 [ F task2_completed ])"; // achievability (false)
    
    // programm, model,  formula
    storm::prism::Program program = storm::parseProgram(programFile);
    program.checkValidity();
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::parseFormulasForProgram(formulasAsString, program);
    typename storm::builder::ExplicitPrismModelBuilder<double>::Options options = storm::builder::ExplicitPrismModelBuilder<double>::Options(formulas);
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = storm::builder::ExplicitPrismModelBuilder<double>(program, options).translate()->as<storm::models::sparse::Mdp<double>>();
    uint_fast64_t const initState = *mdp->getInitialStates().begin();
    
    storm::modelchecker::SparseMdpMultiObjectiveModelChecker<storm::models::sparse::Mdp<double>> checker(*mdp);
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[0], true));
    ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
    EXPECT_NEAR(0.7448979591841851, result->asExplicitQuantitativeCheckResult<double>()[initState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[1], true));
    ASSERT_TRUE(result->isExplicitQualitativeCheckResult());
    EXPECT_TRUE(result->asExplicitQualitativeCheckResult()[initState]);
    
    result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[2], true));
    ASSERT_TRUE(result->isExplicitQualitativeCheckResult());
    EXPECT_FALSE(result->asExplicitQualitativeCheckResult()[initState]);
    
}

TEST(SparseMdpMultiObjectiveModelCheckerTest, scheduler) {
    
    std::string programFile = STORM_CPP_BASE_PATH "/examples/multiobjective/mdp/scheduler/scheduler05.nm";
    std::string formulasAsString = "multi(R{\"time\"}min=?[ F \"tasks_complete\" ], R{\"energy\"}<=1.45 [  F \"tasks_complete\" ]) "; // numerical
    formulasAsString += "; \n  multi(R{\"time\"}<= 11.778[ F \"tasks_complete\" ], R{\"energy\"}<=1.45 [  F \"tasks_complete\" ]) "; // achievability (true)
    formulasAsString += "; \n  multi(R{\"time\"}<= 11.777 [ F \"tasks_complete\" ], R{\"energy\"}<=1.45 [  F \"tasks_complete\" ]) "; // achievability (false)
    
    // programm, model,  formula
    storm::prism::Program program = storm::parseProgram(programFile);
    program.checkValidity();
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::parseFormulasForProgram(formulasAsString, program);
    typename storm::builder::ExplicitPrismModelBuilder<double>::Options options = storm::builder::ExplicitPrismModelBuilder<double>::Options(formulas);
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = storm::builder::ExplicitPrismModelBuilder<double>(program, options).translate()->as<storm::models::sparse::Mdp<double>>();
    uint_fast64_t const initState = *mdp->getInitialStates().begin();
    
    storm::modelchecker::SparseMdpMultiObjectiveModelChecker<storm::models::sparse::Mdp<double>> checker(*mdp);
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[0], true));
    ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
    EXPECT_NEAR(11.77777778, result->asExplicitQuantitativeCheckResult<double>()[initState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[1], true));
    ASSERT_TRUE(result->isExplicitQualitativeCheckResult());
    EXPECT_TRUE(result->asExplicitQualitativeCheckResult()[initState]);
    
    result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[2], true));
    ASSERT_TRUE(result->isExplicitQualitativeCheckResult());
    EXPECT_FALSE(result->asExplicitQualitativeCheckResult()[initState]);
    
}

TEST(SparseMdpMultiObjectiveModelCheckerTest, dpm) {
    
    std::string programFile = STORM_CPP_BASE_PATH "/examples/multiobjective/mdp/dpm/dpm100.nm";
    std::string formulasAsString = "multi(R{\"power\"}min=? [ C<=100 ], R{\"queue\"}<=70 [ C<=100 ])"; // numerical
    formulasAsString += "; \n  multi(R{\"power\"}<=121.613 [ C<=100 ], R{\"queue\"}<=70 [ C<=100 ])"; // achievability (true)
    formulasAsString += "; \n  multi(R{\"power\"}<=121.612 [ C<=100 ], R{\"queue\"}<=70 [ C<=100 ])"; // achievability (false)
    
    // programm, model,  formula
    storm::prism::Program program = storm::parseProgram(programFile);
    program.checkValidity();
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::parseFormulasForProgram(formulasAsString, program);
    typename storm::builder::ExplicitPrismModelBuilder<double>::Options options = storm::builder::ExplicitPrismModelBuilder<double>::Options(formulas);
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = storm::builder::ExplicitPrismModelBuilder<double>(program, options).translate()->as<storm::models::sparse::Mdp<double>>();
    uint_fast64_t const initState = *mdp->getInitialStates().begin();
    
    storm::modelchecker::SparseMdpMultiObjectiveModelChecker<storm::models::sparse::Mdp<double>> checker(*mdp);
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[0], true));
    ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
    EXPECT_NEAR(121.61288420945114, result->asExplicitQuantitativeCheckResult<double>()[initState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[1], true));
    ASSERT_TRUE(result->isExplicitQualitativeCheckResult());
    EXPECT_TRUE(result->asExplicitQualitativeCheckResult()[initState]);
    
    result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[2], true));
    ASSERT_TRUE(result->isExplicitQualitativeCheckResult());
    EXPECT_FALSE(result->asExplicitQualitativeCheckResult()[initState]);
    
}




#endif /* STORM_HAVE_HYPRO */
