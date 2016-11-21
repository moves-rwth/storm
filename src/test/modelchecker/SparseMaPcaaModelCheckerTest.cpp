#include "gtest/gtest.h"
#include "storm-config.h"

#ifdef STORM_HAVE_HYPRO

#include "storm/modelchecker/multiobjective/pcaa.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/modelchecker/results/ParetoCurveCheckResult.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/storage/geometry/Polytope.h"
#include "storm/storage/geometry/Hyperrectangle.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/SettingsManager.h"
#include "storm/utility/storm.h"


/* Rationals for MAs not supported at this point
TEST(SparseMaPcaaModelCheckerTest, serverRationalNumbers) {
    
    std::string programFile = STORM_TEST_RESOURCES_DIR "/ma/server.ma";
    std::string formulasAsString = "multi(Tmax=? [ F \"error\" ], Pmax=? [ F \"processB\" ]) "; // pareto
  //  formulasAsString += "; \n multi(..)";
    
    // programm, model,  formula
    storm::prism::Program program = storm::parseProgram(programFile);
    program.checkValidity();
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::parseFormulasForProgram(formulasAsString, program);
    storm::generator::NextStateGeneratorOptions options(formulas);
    std::shared_ptr<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>> ma = storm::builder::ExplicitModelBuilder<storm::RationalNumber>(program, options).build()->as<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>();
    
    storm::modelchecker::SparseMarkovAutomatonCslModelChecker<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>> checker(*ma);
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(storm::modelchecker::CheckTask<storm::logic::Formula>(*formulas[0], true));
    ASSERT_TRUE(result->isParetoCurveCheckResult());
    
    storm::RationalNumber p1 = storm::utility::convertNumber<storm::RationalNumber>(11.0); p1 /= storm::utility::convertNumber<storm::RationalNumber>(6.0);
    storm::RationalNumber p2 = storm::utility::convertNumber<storm::RationalNumber>(1.0); p2 /= storm::utility::convertNumber<storm::RationalNumber>(2.0);
    std::vector<storm::RationalNumber> p = {p1, p2};
    storm::RationalNumber q1 = storm::utility::convertNumber<storm::RationalNumber>(29.0); q1 /= storm::utility::convertNumber<storm::RationalNumber>(18.0);
    storm::RationalNumber q2 = storm::utility::convertNumber<storm::RationalNumber>(2.0); q2 /= storm::utility::convertNumber<storm::RationalNumber>(3.0);
    std::vector<storm::RationalNumber> q = {q1, q2};
    auto expectedAchievableValues = storm::storage::geometry::Polytope<storm::RationalNumber>::createDownwardClosure(std::vector<std::vector<storm::RationalNumber>>({p,q}));
    EXPECT_TRUE(expectedAchievableValues->contains(result->asParetoCurveCheckResult<storm::RationalNumber>().getUnderApproximation()));
    EXPECT_TRUE(result->asParetoCurveCheckResult<storm::RationalNumber>().getUnderApproximation()->contains(expectedAchievableValues));
    
}*/


TEST(SparseMaPcaaModelCheckerTest, server) {
    
    std::string programFile = STORM_TEST_RESOURCES_DIR "/ma/server.ma";
    std::string formulasAsString = "multi(Tmax=? [ F \"error\" ], Pmax=? [ F \"processB\" ]) "; // pareto
    //  formulasAsString += "; \n multi(..)";
    
    // programm, model,  formula
    
    storm::prism::Program program = storm::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, "");
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::parseFormulasForPrismProgram(formulasAsString, program);
    std::shared_ptr<storm::models::sparse::MarkovAutomaton<double>> ma = storm::buildSparseModel<double>(program, formulas, true)->as<storm::models::sparse::MarkovAutomaton<double>>();
    storm::modelchecker::SparseMarkovAutomatonCslModelChecker<storm::models::sparse::MarkovAutomaton<double>> checker(*ma);
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = storm::modelchecker::multiobjective::performPcaa(*ma, formulas[0]->asMultiObjectiveFormula());
    ASSERT_TRUE(result->isParetoCurveCheckResult());
    
    std::vector<double> p = {11.0/6.0, 1.0/2.0};
    std::vector<double> q = {29.0/18.0, 2.0/3.0};
    auto expectedAchievableValues = storm::storage::geometry::Polytope<double>::createDownwardClosure(std::vector<std::vector<double>>({p,q}));
    // due to precision issues, we enlarge one of the polytopes before checking containement
    std::vector<double> lb = {-storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision(), -storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision()};
    std::vector<double> ub = {storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision()};
    auto bloatingBox = storm::storage::geometry::Hyperrectangle<double>(lb,ub).asPolytope();
    
    EXPECT_TRUE(expectedAchievableValues->minkowskiSum(bloatingBox)->contains(result->asParetoCurveCheckResult<double>().getUnderApproximation()));
    EXPECT_TRUE(result->asParetoCurveCheckResult<double>().getUnderApproximation()->minkowskiSum(bloatingBox)->contains(expectedAchievableValues));
    
}



#endif /* STORM_HAVE_HYPRO */
