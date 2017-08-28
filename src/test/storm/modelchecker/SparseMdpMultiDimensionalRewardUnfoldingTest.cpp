#include "gtest/gtest.h"
#include "storm-config.h"

#include "storm/modelchecker/multiobjective/multiObjectiveModelChecking.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitParetoCurveCheckResult.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/SettingsManager.h"
#include "storm/utility/constants.h"
#include "storm/api/storm.h"


TEST(SparseMdpMultiDimensionalRewardUnfoldingTest, one_dim_walk_small) {
    
    std::string programFile = STORM_TEST_RESOURCES_DIR "/mdp/one_dim_walk.nm";
    std::string constantsDef = "N=2";
    std::string formulasAsString = "multi(Pmax=? [ F{\"r\"}<=1 x=N ]) ";
    formulasAsString += "; \n multi(Pmax=? [ multi( F{\"r\"}<=1 x=N, F{\"l\"}<=2 x=0 )])";
    formulasAsString += "; \n multi(Pmin=? [ multi( F{\"r\"}<=1 x=N, F{\"l\"}<=2 x=0 )])";
    formulasAsString += "; \n multi(Pmax=? [ F{\"r\"}<=1 x=N], Pmax=? [ F{\"l\"}<=2 x=0])";
    formulasAsString += "; \n multi(Pmin=? [ F{\"r\"}<=1 x=N], Pmax=? [ F{\"l\"}<=2 x=0])";
    formulasAsString += "; \n multi(Pmin=? [ F{\"r\"}<=1 x=N], Pmin=? [ F{\"l\"}<=2 x=0])";

    // programm, model,  formula
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsDef);
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulasAsString, program));
    std::shared_ptr<storm::models::sparse::Mdp<storm::RationalNumber>> mdp = storm::api::buildSparseModel<storm::RationalNumber>(program, formulas)->as<storm::models::sparse::Mdp<storm::RationalNumber>>();
    uint_fast64_t const initState = *mdp->getInitialStates().begin();;
    
    std::unique_ptr<storm::modelchecker::CheckResult> result;
    
    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(*mdp, formulas[0]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
    EXPECT_EQ(storm::utility::convertNumber<storm::RationalNumber>(0.5), result->asExplicitQuantitativeCheckResult<storm::RationalNumber>()[initState]);

    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(*mdp, formulas[1]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
    EXPECT_EQ(storm::utility::convertNumber<storm::RationalNumber>(0.125), result->asExplicitQuantitativeCheckResult<storm::RationalNumber>()[initState]);

    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(*mdp, formulas[2]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
    EXPECT_EQ(storm::utility::convertNumber<storm::RationalNumber>(0.0), result->asExplicitQuantitativeCheckResult<storm::RationalNumber>()[initState]);

    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(*mdp, formulas[3]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isExplicitParetoCurveCheckResult());
    std::vector<storm::RationalNumber> p1 = {storm::utility::convertNumber<storm::RationalNumber>(0.5), storm::utility::convertNumber<storm::RationalNumber>(0.5)};
    std::vector<storm::RationalNumber> q1 = {storm::utility::convertNumber<storm::RationalNumber>(0.125), storm::utility::convertNumber<storm::RationalNumber>(0.75)};
    auto expectedAchievableValues = storm::storage::geometry::Polytope<storm::RationalNumber>::createDownwardClosure(std::vector<std::vector<storm::RationalNumber>>({p1, q1}));
    
    EXPECT_TRUE(expectedAchievableValues->contains(result->asExplicitParetoCurveCheckResult<storm::RationalNumber>().getUnderApproximation()));
    EXPECT_TRUE(result->asExplicitParetoCurveCheckResult<storm::RationalNumber>().getOverApproximation()->contains(expectedAchievableValues));
    
    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(*mdp, formulas[4]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isExplicitParetoCurveCheckResult());
    std::vector<storm::RationalNumber> p2 = {storm::utility::convertNumber<storm::RationalNumber>(0.0), storm::utility::convertNumber<storm::RationalNumber>(0.75)};
    expectedAchievableValues = storm::storage::geometry::Polytope<storm::RationalNumber>::createDownwardClosure(std::vector<std::vector<storm::RationalNumber>>({p2}));
    std::vector<std::vector<storm::RationalNumber>> transformationMatrix(2, std::vector<storm::RationalNumber>(2, storm::utility::zero<storm::RationalNumber>()));
    transformationMatrix[0][0] = -storm::utility::one<storm::RationalNumber>();
    transformationMatrix[1][1] = storm::utility::one<storm::RationalNumber>();
    expectedAchievableValues = expectedAchievableValues->affineTransformation(transformationMatrix, std::vector<storm::RationalNumber>(2, storm::utility::zero<storm::RationalNumber>()));
    EXPECT_TRUE(expectedAchievableValues->contains(result->asExplicitParetoCurveCheckResult<storm::RationalNumber>().getUnderApproximation()));
    EXPECT_TRUE(result->asExplicitParetoCurveCheckResult<storm::RationalNumber>().getOverApproximation()->contains(expectedAchievableValues));
    
    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(*mdp, formulas[5]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isExplicitParetoCurveCheckResult());
    std::vector<storm::RationalNumber> p3 = {storm::utility::convertNumber<storm::RationalNumber>(0.0), storm::utility::convertNumber<storm::RationalNumber>(-0.75)};
    std::vector<storm::RationalNumber> q3 = {storm::utility::convertNumber<storm::RationalNumber>(-0.5), storm::utility::convertNumber<storm::RationalNumber>(0.0)};
    expectedAchievableValues = storm::storage::geometry::Polytope<storm::RationalNumber>::createDownwardClosure(std::vector<std::vector<storm::RationalNumber>>({p3 ,q3}));
    transformationMatrix[1][1] = -storm::utility::one<storm::RationalNumber>();
    expectedAchievableValues = expectedAchievableValues->affineTransformation(transformationMatrix, std::vector<storm::RationalNumber>(2, storm::utility::zero<storm::RationalNumber>()));
    EXPECT_TRUE(expectedAchievableValues->contains(result->asExplicitParetoCurveCheckResult<storm::RationalNumber>().getUnderApproximation()));
    EXPECT_TRUE(result->asExplicitParetoCurveCheckResult<storm::RationalNumber>().getOverApproximation()->contains(expectedAchievableValues));

}

TEST(SparseMdpMultiDimensionalRewardUnfoldingTest, one_dim_walk_large) {
    
    std::string programFile = STORM_TEST_RESOURCES_DIR "/mdp/one_dim_walk.nm";
    std::string constantsDef = "N=10";
    std::string formulasAsString = "multi(Pmax=? [ F{\"r\"}<=5 x=N ]) ";
    formulasAsString += "; \n multi(Pmax=? [ multi( F{\"r\"}<=5 x=N, F{\"l\"}<=10 x=0 )])";
    formulasAsString += "; \n multi(P>=1/512 [ F{\"r\"}<=5 x=N], Pmax=? [ F{\"l\"}<=10 x=0])";

    // programm, model,  formula
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsDef);
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulasAsString, program));
    std::shared_ptr<storm::models::sparse::Mdp<storm::RationalNumber>> mdp = storm::api::buildSparseModel<storm::RationalNumber>(program, formulas)->as<storm::models::sparse::Mdp<storm::RationalNumber>>();
    uint_fast64_t const initState = *mdp->getInitialStates().begin();;
    
    std::unique_ptr<storm::modelchecker::CheckResult> result;
    
    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(*mdp, formulas[0]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
    storm::RationalNumber expectedResult = storm::utility::pow(storm::utility::convertNumber<storm::RationalNumber>(0.5), 5);
    EXPECT_EQ(expectedResult, result->asExplicitQuantitativeCheckResult<storm::RationalNumber>()[initState]);

    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(*mdp, formulas[1]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
    expectedResult = storm::utility::pow(storm::utility::convertNumber<storm::RationalNumber>(0.5), 15);
    EXPECT_EQ(expectedResult, result->asExplicitQuantitativeCheckResult<storm::RationalNumber>()[initState]);

    result = storm::modelchecker::multiobjective::performMultiObjectiveModelChecking(*mdp, formulas[2]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isExplicitQuantitativeCheckResult());
    expectedResult = storm::utility::convertNumber<storm::RationalNumber, std::string>("2539/4096");
    EXPECT_EQ(expectedResult, result->asExplicitQuantitativeCheckResult<storm::RationalNumber>()[initState]);
    
}


