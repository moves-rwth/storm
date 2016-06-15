#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/parser/PrismParser.h"
#include "src/parser/FormulaParser.h"
#include "src/logic/Formulas.h"
#include "src/permissivesched/PermissiveSchedulers.h"
#include "src/builder/ExplicitModelBuilder.h"

#include "src/models/sparse/StandardRewardModel.h"
#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"


#if defined(STORM_HAVE_MSAT) || defined(STORM_HAVE_Z3)

TEST(SmtPermissiveSchedulerTest, DieSelection) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/die_c1.nm");
    storm::parser::FormulaParser formulaParser(program.getManager().getSharedPointer());
    
//    auto formula02 = formulaParser.parseSingleFormulaFromString("P>=0.10 [ F \"one\"]")->asProbabilityOperatorFormula();
//    ASSERT_TRUE(storm::logic::isLowerBound(formula02.getComparisonType()));
//    auto formula001 = formulaParser.parseSingleFormulaFromString("P>=0.17 [ F \"one\"]")->asProbabilityOperatorFormula();
    
    auto formula02b = formulaParser.parseSingleFormulaFromString("P<=0.16 [ F \"one\"]")->asProbabilityOperatorFormula();
    auto formula001b = formulaParser.parseSingleFormulaFromString("P<=0.05 [ F \"one\"]")->asProbabilityOperatorFormula();
    
    // Customize and perform model-building.
    storm::generator::NextStateGeneratorOptions options(formula02b);
    
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = storm::builder::ExplicitModelBuilder<double>(program, options).build()->as<storm::models::sparse::Mdp<double>>();
    
//    boost::optional<storm::ps::SubMDPPermissiveScheduler<>> perms = storm::ps::computePermissiveSchedulerViaSMT<>(*mdp, formula02);
//    EXPECT_NE(perms, boost::none);
//    boost::optional<storm::ps::SubMDPPermissiveScheduler<>> perms2 = storm::ps::computePermissiveSchedulerViaSMT<>(*mdp, formula001);
//    EXPECT_EQ(perms2, boost::none);
    
    boost::optional<storm::ps::SubMDPPermissiveScheduler<>> perms3 = storm::ps::computePermissiveSchedulerViaSMT<>(*mdp, formula02b);
    EXPECT_NE(perms3, boost::none);
    boost::optional<storm::ps::SubMDPPermissiveScheduler<>> perms4 = storm::ps::computePermissiveSchedulerViaSMT<>(*mdp, formula001b);
    EXPECT_EQ(perms4, boost::none);
    
    storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>> checker0(*mdp, std::unique_ptr<storm::utility::solver::MinMaxLinearEquationSolverFactory<double>>(new storm::utility::solver::MinMaxLinearEquationSolverFactory<double>(storm::solver::EquationSolverTypeSelection::Native)));
    
    std::unique_ptr<storm::modelchecker::CheckResult> result0 = checker0.check(formula02b);
    storm::modelchecker::ExplicitQualitativeCheckResult& qualitativeResult0 = result0->asExplicitQualitativeCheckResult();
    
    ASSERT_FALSE(qualitativeResult0[0]);
    
    auto submdp = perms3->apply();
    storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>> checker1(submdp, std::unique_ptr<storm::utility::solver::MinMaxLinearEquationSolverFactory<double>>(new storm::utility::solver::MinMaxLinearEquationSolverFactory<double>(storm::solver::EquationSolverTypeSelection::Native)));
    
    std::unique_ptr<storm::modelchecker::CheckResult> result1 = checker1.check(formula02b);
    storm::modelchecker::ExplicitQualitativeCheckResult& qualitativeResult1 = result1->asExplicitQualitativeCheckResult();
    
    EXPECT_TRUE(qualitativeResult1[0]);
    
    //
    
}

#endif // STORM_HAVE_MSAT || STORM_HAVE_Z3
