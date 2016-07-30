#include "gtest/gtest.h"
#include "storm-config.h"

#ifdef STORM_HAVE_HYPRO

#include "src/modelchecker/multiobjective/SparseMaMultiObjectiveModelChecker.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "src/modelchecker/results/ParetoCurveCheckResult.h"
#include "src/models/sparse/MarkovAutomaton.h"
#include "src/storage/geometry/Polytope.h"
#include "src/settings/modules/GeneralSettings.h"
#include "src/settings/SettingsManager.h"
#include "src/utility/storm.h"



TEST(SparseMdpMultiObjectiveModelCheckerTest, server) {
    
    std::string programFile = STORM_CPP_BASE_PATH "/examples/multiobjective/ma/server/server.ma";
    std::string formulasAsString = "multi(Tmax=? [ F \"error\" ], Pmax=? [ F \"processB\" ]) "; // pareto
  //  formulasAsString += "; \n multi(..)";
    
    // programm, model,  formula
    storm::prism::Program program = storm::parseProgram(programFile);
    program.checkValidity();
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::parseFormulasForProgram(formulasAsString, program);
    storm::generator::NextStateGeneratorOptions options(formulas);
    std::shared_ptr<storm::models::sparse::MarkovAutomaton<double>> ma = storm::builder::ExplicitModelBuilder<double>(program, options).build()->as<storm::models::sparse::MarkovAutomaton<double>>();
    
    storm::modelchecker::SparseMaMultiObjectiveModelChecker<storm::models::sparse::MarkovAutomaton<double>> checker(*ma);
    
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
    
}



#endif /* STORM_HAVE_HYPRO */
