#include "gtest/gtest.h"
#include "storm-config.h"

#ifdef STORM_HAVE_CARL

#include "storm/adapters/CarlAdapter.h"

#include "utility/storm.h"
#include "storm/models/sparse/Model.h"
#include "storm/modelchecker/parametric/ParameterLifting.h"

TEST(SparseMdpRegionModelCheckerTest, two_dice_Prob) {
    
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pmdp/two_dice.nm";
    std::string formulaFile = STORM_TEST_RESOURCES_DIR "/prctl/two_dice.prctl"; //P<=0.17 [F \"doubles\" ]";
    
    carl::VariablePool::getInstance().clear();

    storm::prism::Program program = storm::parseProgram(programFile);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::extractFormulasFromProperties(storm::parsePropertiesForPrismProgram(formulaFile, program));
    std::shared_ptr<storm::models::sparse::Mdp<storm::RationalFunction>> model = storm::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Mdp<storm::RationalFunction>>();
    
    storm::modelchecker::parametric::ParameterLifting<storm::models::sparse::Dtmc<storm::RationalFunction>, double> parameterLiftingContext(model);
    parameterLiftingContext->specifyFormula(*formulas[0]);
    
    auto allSatRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.495<=p1<=0.5,0.5<=p2<=0.505");
    auto exBothRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.45<=p1<=0.55,0.45<=p2<=0.55");
    auto allVioRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.6<=p1<=0.7,0.6<=p2<=0.6");

    
    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::AllSat, parameterLiftingContext.analyzeRegion(allSatRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::ExistsBoth, parameterLiftingContext.analyzeRegion(exBothRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::AllViolated, parameterLiftingContext.analyzeRegion(allVioRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));
    
    carl::VariablePool::getInstance().clear();
}

TEST(SparseMdpRegionModelCheckerTest, coin_Prob) {
    
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pmdp/coin2_2.pm";
    std::string formulaAsString = "P>0.25 [F \"finished\"&\"all_coins_equal_1\" ]";
    
    carl::VariablePool::getInstance().clear();

    storm::prism::Program program = storm::parseProgram(programFile);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::extractFormulasFromProperties(storm::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Mdp<storm::RationalFunction>> model = storm::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Mdp<storm::RationalFunction>>();
    
    storm::modelchecker::parametric::ParameterLifting<storm::models::sparse::Dtmc<storm::RationalFunction>, double> parameterLiftingContext(model);
    parameterLiftingContext->specifyFormula(*formulas[0]);
    
    //start testing
    auto allSatRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.3<=p1<=0.45,0.2<=p2<=0.54");
    auto exBothRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.4<=p1<=0.65,0.5<=p2<=0.7");
    auto allVioRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.4<=p1<=0.7,0.55<=p2<=0.6");
        
    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::AllSat, parameterLiftingContext.analyzeRegion(allSatRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::ExistsBoth, parameterLiftingContext.analyzeRegion(exBothRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::AllViolated, parameterLiftingContext.analyzeRegion(allVioRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));

    carl::VariablePool::getInstance().clear();
}

#endif
