#include "gtest/gtest.h"
#include "storm-config.h"

#ifdef STORM_HAVE_CARL

#include "storm/adapters/CarlAdapter.h"

#include "storm/utility/storm.h"

TEST(SparseMdpParameterLiftingTest, two_dice_Prob) {
    
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pmdp/two_dice.nm";
    std::string formulaFile = "P<=0.17 [ F \"doubles\" ]";
    
    carl::VariablePool::getInstance().clear();

    storm::prism::Program program = storm::parseProgram(programFile);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::extractFormulasFromProperties(storm::parsePropertiesForPrismProgram(formulaFile, program));
    std::shared_ptr<storm::models::sparse::Mdp<storm::RationalFunction>> model = storm::buildSparseModel<storm::RationalFunction>(program, formulas).getModel()->as<storm::models::sparse::Mdp<storm::RationalFunction>>();
    
    auto modelParameters = storm::models::sparse::getProbabilityParameters(*model);
    auto rewParameters = storm::models::sparse::getRewardParameters(*model);
    modelParameters.insert(rewParameters.begin(), rewParameters.end());
    
    storm::modelchecker::parametric::SparseMdpRegionChecker<storm::models::sparse::Mdp<storm::RationalFunction>, double, storm::RationalNumber> regionChecker(*model);
    regionChecker.specifyFormula(storm::modelchecker::CheckTask<storm::logic::Formula, storm::RationalFunction>(*formulas[0], true));
    
    auto allSatRegion = storm::storage::ParameterRegion<storm::RationalFunction>::parseRegion("0.495<=p1<=0.5,0.5<=p2<=0.505", modelParameters);
    auto exBothRegion = storm::storage::ParameterRegion<storm::RationalFunction>::parseRegion("0.45<=p1<=0.55,0.45<=p2<=0.55", modelParameters);
    auto allVioRegion = storm::storage::ParameterRegion<storm::RationalFunction>::parseRegion("0.6<=p1<=0.7,0.6<=p2<=0.6", modelParameters);

    
    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::AllSat, regionChecker.analyzeRegion(allSatRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::ExistsBoth, regionChecker.analyzeRegion(exBothRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::AllViolated, regionChecker.analyzeRegion(allVioRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));
    
    carl::VariablePool::getInstance().clear();
}

TEST(SparseMdpParameterLiftingTest, two_dice_Prob_bounded) {
    
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pmdp/two_dice.nm";
    std::string formulaFile = "P<=0.17 [ F<100 \"doubles\" ]";
    
    carl::VariablePool::getInstance().clear();

    storm::prism::Program program = storm::parseProgram(programFile);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::extractFormulasFromProperties(storm::parsePropertiesForPrismProgram(formulaFile, program));
    std::shared_ptr<storm::models::sparse::Mdp<storm::RationalFunction>> model = storm::buildSparseModel<storm::RationalFunction>(program, formulas).getModel()->as<storm::models::sparse::Mdp<storm::RationalFunction>>();
    
    auto modelParameters = storm::models::sparse::getProbabilityParameters(*model);
    auto rewParameters = storm::models::sparse::getRewardParameters(*model);
    modelParameters.insert(rewParameters.begin(), rewParameters.end());
    
    storm::modelchecker::parametric::SparseMdpRegionChecker<storm::models::sparse::Mdp<storm::RationalFunction>, double, storm::RationalNumber> regionChecker(*model);
    regionChecker.specifyFormula(storm::modelchecker::CheckTask<storm::logic::Formula, storm::RationalFunction>(*formulas[0], true));
    
    auto allSatRegion = storm::storage::ParameterRegion<storm::RationalFunction>::parseRegion("0.495<=p1<=0.5,0.5<=p2<=0.505", modelParameters);
    auto exBothRegion = storm::storage::ParameterRegion<storm::RationalFunction>::parseRegion("0.45<=p1<=0.55,0.45<=p2<=0.55", modelParameters);
    auto allVioRegion = storm::storage::ParameterRegion<storm::RationalFunction>::parseRegion("0.6<=p1<=0.7,0.6<=p2<=0.6", modelParameters);

    
    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::AllSat, regionChecker.analyzeRegion(allSatRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::ExistsBoth, regionChecker.analyzeRegion(exBothRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::AllViolated, regionChecker.analyzeRegion(allVioRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));
    
    carl::VariablePool::getInstance().clear();
}

TEST(SparseMdpParameterLiftingTest, two_dice_Prob_exactValidation) {
    
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pmdp/two_dice.nm";
    std::string formulaFile = "P<=0.17 [ F \"doubles\" ]";
    
    carl::VariablePool::getInstance().clear();

    storm::prism::Program program = storm::parseProgram(programFile);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::extractFormulasFromProperties(storm::parsePropertiesForPrismProgram(formulaFile, program));
    std::shared_ptr<storm::models::sparse::Mdp<storm::RationalFunction>> model = storm::buildSparseModel<storm::RationalFunction>(program, formulas).getModel()->as<storm::models::sparse::Mdp<storm::RationalFunction>>();
    
    auto modelParameters = storm::models::sparse::getProbabilityParameters(*model);
    auto rewParameters = storm::models::sparse::getRewardParameters(*model);
    modelParameters.insert(rewParameters.begin(), rewParameters.end());
    
    storm::modelchecker::parametric::SparseMdpRegionChecker<storm::models::sparse::Mdp<storm::RationalFunction>, double, storm::RationalNumber> regionChecker(*model);
    regionChecker.specifyFormula(storm::modelchecker::CheckTask<storm::logic::Formula, storm::RationalFunction>(*formulas[0], true));
    
    auto allSatRegion = storm::storage::ParameterRegion<storm::RationalFunction>::parseRegion("0.495<=p1<=0.5,0.5<=p2<=0.505", modelParameters);
    auto exBothRegion = storm::storage::ParameterRegion<storm::RationalFunction>::parseRegion("0.45<=p1<=0.55,0.45<=p2<=0.55", modelParameters);
    auto allVioRegion = storm::storage::ParameterRegion<storm::RationalFunction>::parseRegion("0.6<=p1<=0.7,0.6<=p2<=0.6", modelParameters);

    
    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::AllSat, regionChecker.analyzeRegion(allSatRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::ExistsBoth, regionChecker.analyzeRegion(exBothRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::AllViolated, regionChecker.analyzeRegion(allVioRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));
    
    carl::VariablePool::getInstance().clear();
}

TEST(SparseMdpParameterLiftingTest, two_dice_Prob_bounded_exactValidation) {
    
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pmdp/two_dice.nm";
    std::string formulaFile = "P<=0.17 [ F<100 \"doubles\" ]";
    
    carl::VariablePool::getInstance().clear();

    storm::prism::Program program = storm::parseProgram(programFile);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::extractFormulasFromProperties(storm::parsePropertiesForPrismProgram(formulaFile, program));
    std::shared_ptr<storm::models::sparse::Mdp<storm::RationalFunction>> model = storm::buildSparseModel<storm::RationalFunction>(program, formulas).getModel()->as<storm::models::sparse::Mdp<storm::RationalFunction>>();
    
    auto modelParameters = storm::models::sparse::getProbabilityParameters(*model);
    auto rewParameters = storm::models::sparse::getRewardParameters(*model);
    modelParameters.insert(rewParameters.begin(), rewParameters.end());
    
    storm::modelchecker::parametric::SparseMdpRegionChecker<storm::models::sparse::Mdp<storm::RationalFunction>, double, storm::RationalNumber> regionChecker(*model);
    regionChecker.specifyFormula(storm::modelchecker::CheckTask<storm::logic::Formula, storm::RationalFunction>(*formulas[0], true));
    
    auto allSatRegion = storm::storage::ParameterRegion<storm::RationalFunction>::parseRegion("0.495<=p1<=0.5,0.5<=p2<=0.505", modelParameters);
    auto exBothRegion = storm::storage::ParameterRegion<storm::RationalFunction>::parseRegion("0.45<=p1<=0.55,0.45<=p2<=0.55", modelParameters);
    auto allVioRegion = storm::storage::ParameterRegion<storm::RationalFunction>::parseRegion("0.6<=p1<=0.7,0.6<=p2<=0.6", modelParameters);

    
    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::AllSat, regionChecker.analyzeRegion(allSatRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::ExistsBoth, regionChecker.analyzeRegion(exBothRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::AllViolated, regionChecker.analyzeRegion(allVioRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));
    
    carl::VariablePool::getInstance().clear();
}

TEST(SparseMdpParameterLiftingTest, coin_Prob) {
    
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pmdp/coin2_2.nm";
    std::string formulaAsString = "P>0.25 [F \"finished\"&\"all_coins_equal_1\" ]";
    
    storm::prism::Program program = storm::parseProgram(programFile);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::extractFormulasFromProperties(storm::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Mdp<storm::RationalFunction>> model = storm::buildSparseModel<storm::RationalFunction>(program, formulas).getModel()->as<storm::models::sparse::Mdp<storm::RationalFunction>>();
    
    auto modelParameters = storm::models::sparse::getProbabilityParameters(*model);
    auto rewParameters = storm::models::sparse::getRewardParameters(*model);
    modelParameters.insert(rewParameters.begin(), rewParameters.end());
    
    storm::modelchecker::parametric::SparseMdpRegionChecker<storm::models::sparse::Mdp<storm::RationalFunction>, double, storm::RationalNumber> regionChecker(*model);
    regionChecker.specifyFormula(storm::modelchecker::CheckTask<storm::logic::Formula, storm::RationalFunction>(*formulas[0], true));
    
    //start testing
    auto allSatRegion = storm::storage::ParameterRegion<storm::RationalFunction>::parseRegion("0.3<=p1<=0.45,0.2<=p2<=0.54", modelParameters);
    auto exBothRegion = storm::storage::ParameterRegion<storm::RationalFunction>::parseRegion("0.4<=p1<=0.65,0.5<=p2<=0.7", modelParameters);
    auto allVioRegion = storm::storage::ParameterRegion<storm::RationalFunction>::parseRegion("0.4<=p1<=0.7,0.55<=p2<=0.6", modelParameters);
        
    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::AllSat, regionChecker.analyzeRegion(allSatRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::ExistsBoth, regionChecker.analyzeRegion(exBothRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::AllViolated, regionChecker.analyzeRegion(allVioRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));

    carl::VariablePool::getInstance().clear();
}

TEST(SparseMdpParameterLiftingTest, brp_Prop) {
    
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pmdp/brp16_2.nm";
    std::string formulaAsString = "P<=0.84 [ F (s=5 & T) ]";
    std::string constantsAsString = "TOMsg=0.0,TOAck=0.0";

    storm::prism::Program program = storm::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::extractFormulasFromProperties(storm::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Mdp<storm::RationalFunction>> model = storm::buildSparseModel<storm::RationalFunction>(program, formulas).getModel()->as<storm::models::sparse::Mdp<storm::RationalFunction>>();
    
    auto modelParameters = storm::models::sparse::getProbabilityParameters(*model);
    auto rewParameters = storm::models::sparse::getRewardParameters(*model);
    modelParameters.insert(rewParameters.begin(), rewParameters.end());
    
    storm::modelchecker::parametric::SparseMdpRegionChecker<storm::models::sparse::Mdp<storm::RationalFunction>, double, storm::RationalNumber> regionChecker(*model);
    regionChecker.specifyFormula(storm::modelchecker::CheckTask<storm::logic::Formula, storm::RationalFunction>(*formulas[0], true));
    
    //start testing
    auto allSatRegion=storm::storage::ParameterRegion<storm::RationalFunction>::parseRegion("0.7<=pL<=0.9,0.75<=pK<=0.95", modelParameters);
    auto exBothRegion=storm::storage::ParameterRegion<storm::RationalFunction>::parseRegion("0.4<=pL<=0.65,0.75<=pK<=0.95", modelParameters);
    auto allVioRegion=storm::storage::ParameterRegion<storm::RationalFunction>::parseRegion("0.1<=pL<=0.73,0.2<=pK<=0.715", modelParameters);
        
    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::AllSat, regionChecker.analyzeRegion(allSatRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::ExistsBoth, regionChecker.analyzeRegion(exBothRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::AllViolated, regionChecker.analyzeRegion(allVioRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));

    carl::VariablePool::getInstance().clear();
}

TEST(SparseMdpParameterLiftingTest, brp_Rew) {
    
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pmdp/brp16_2.nm";
    std::string formulaAsString = "R>2.5 [F ((s=5) | (s=0&srep=3)) ]";
    std::string constantsAsString = "pL=0.9,TOAck=0.5";


    storm::prism::Program program = storm::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::extractFormulasFromProperties(storm::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Mdp<storm::RationalFunction>> model = storm::buildSparseModel<storm::RationalFunction>(program, formulas).getModel()->as<storm::models::sparse::Mdp<storm::RationalFunction>>();
    
    auto modelParameters = storm::models::sparse::getProbabilityParameters(*model);
    auto rewParameters = storm::models::sparse::getRewardParameters(*model);
    modelParameters.insert(rewParameters.begin(), rewParameters.end());
    
    storm::modelchecker::parametric::SparseMdpRegionChecker<storm::models::sparse::Mdp<storm::RationalFunction>, double, storm::RationalNumber> regionChecker(*model);
    regionChecker.specifyFormula(storm::modelchecker::CheckTask<storm::logic::Formula, storm::RationalFunction>(*formulas[0], true));
    
   //start testing
    auto allSatRegion=storm::storage::ParameterRegion<storm::RationalFunction>::parseRegion("0.7<=pK<=0.875,0.75<=TOMsg<=0.95", modelParameters);
    auto exBothRegion=storm::storage::ParameterRegion<storm::RationalFunction>::parseRegion("0.6<=pK<=0.9,0.5<=TOMsg<=0.95", modelParameters);
    auto allVioRegion=storm::storage::ParameterRegion<storm::RationalFunction>::parseRegion("0.1<=pK<=0.3,0.2<=TOMsg<=0.3", modelParameters);

    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::AllSat, regionChecker.analyzeRegion(allSatRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::ExistsBoth, regionChecker.analyzeRegion(exBothRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::AllViolated, regionChecker.analyzeRegion(allVioRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));

    carl::VariablePool::getInstance().clear();
}

TEST(SparseMdpParameterLiftingTest, brp_Rew_bounded) {
    
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pmdp/brp16_2.nm";
    std::string formulaAsString = "R>2.5 [ C<=300 ]";
    std::string constantsAsString = "pL=0.9,TOAck=0.5";

    storm::prism::Program program = storm::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::extractFormulasFromProperties(storm::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Mdp<storm::RationalFunction>> model = storm::buildSparseModel<storm::RationalFunction>(program, formulas).getModel()->as<storm::models::sparse::Mdp<storm::RationalFunction>>();
    
    auto modelParameters = storm::models::sparse::getProbabilityParameters(*model);
    auto rewParameters = storm::models::sparse::getRewardParameters(*model);
    modelParameters.insert(rewParameters.begin(), rewParameters.end());
    
    storm::modelchecker::parametric::SparseMdpRegionChecker<storm::models::sparse::Mdp<storm::RationalFunction>, double, storm::RationalNumber> regionChecker(*model);
    regionChecker.specifyFormula(storm::modelchecker::CheckTask<storm::logic::Formula, storm::RationalFunction>(*formulas[0], true));
    
   //start testing
    auto allSatRegion=storm::storage::ParameterRegion<storm::RationalFunction>::parseRegion("0.7<=pK<=0.875,0.75<=TOMsg<=0.95", modelParameters);
    auto exBothRegion=storm::storage::ParameterRegion<storm::RationalFunction>::parseRegion("0.6<=pK<=0.9,0.5<=TOMsg<=0.95", modelParameters);
    auto allVioRegion=storm::storage::ParameterRegion<storm::RationalFunction>::parseRegion("0.1<=pK<=0.3,0.2<=TOMsg<=0.3", modelParameters);

    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::AllSat, regionChecker.analyzeRegion(allSatRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::ExistsBoth, regionChecker.analyzeRegion(exBothRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::AllViolated, regionChecker.analyzeRegion(allVioRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));

    carl::VariablePool::getInstance().clear();
}


TEST(SparseMdpParameterLiftingTest, Brp_Rew_Infty) {
    
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pmdp/brp16_2.nm";
    std::string formulaAsString = "R>2.5 [F (s=0&srep=3) ]";
    std::string constantsAsString = "";
    carl::VariablePool::getInstance().clear();
    storm::prism::Program program = storm::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::extractFormulasFromProperties(storm::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Mdp<storm::RationalFunction>> model = storm::buildSparseModel<storm::RationalFunction>(program, formulas).getModel()->as<storm::models::sparse::Mdp<storm::RationalFunction>>();
    
    auto modelParameters = storm::models::sparse::getProbabilityParameters(*model);
    auto rewParameters = storm::models::sparse::getRewardParameters(*model);
    modelParameters.insert(rewParameters.begin(), rewParameters.end());
    
    storm::modelchecker::parametric::SparseMdpRegionChecker<storm::models::sparse::Mdp<storm::RationalFunction>, double, storm::RationalNumber> regionChecker(*model);
    regionChecker.specifyFormula(storm::modelchecker::CheckTask<storm::logic::Formula, storm::RationalFunction>(*formulas[0], true));

    //start testing
    auto allSatRegion=storm::storage::ParameterRegion<storm::RationalFunction>::parseRegion("0.7<=pK<=0.9,0.6<=pL<=0.85,0.9<=TOMsg<=0.95,0.85<=TOAck<=0.9", modelParameters);
    
    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::AllSat, regionChecker.analyzeRegion(allSatRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));

    carl::VariablePool::getInstance().clear();
}

TEST(SparseMdpParameterLiftingTest, Brp_Rew_4Par) {
    
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pmdp/brp16_2.nm";
    std::string formulaAsString = "R>2.5 [F ((s=5) | (s=0&srep=3)) ]";
    std::string constantsAsString = ""; //!! this model will have 4 parameters
    carl::VariablePool::getInstance().clear();
    storm::prism::Program program = storm::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::extractFormulasFromProperties(storm::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Mdp<storm::RationalFunction>> model = storm::buildSparseModel<storm::RationalFunction>(program, formulas).getModel()->as<storm::models::sparse::Mdp<storm::RationalFunction>>();
    
    auto modelParameters = storm::models::sparse::getProbabilityParameters(*model);
    auto rewParameters = storm::models::sparse::getRewardParameters(*model);
    modelParameters.insert(rewParameters.begin(), rewParameters.end());
    
    storm::modelchecker::parametric::SparseMdpRegionChecker<storm::models::sparse::Mdp<storm::RationalFunction>, double, storm::RationalNumber> regionChecker(*model);
    regionChecker.specifyFormula(storm::modelchecker::CheckTask<storm::logic::Formula, storm::RationalFunction>(*formulas[0], true));
    
    //start testing
    auto allSatRegion=storm::storage::ParameterRegion<storm::RationalFunction>::parseRegion("0.7<=pK<=0.9,0.6<=pL<=0.85,0.9<=TOMsg<=0.95,0.85<=TOAck<=0.9", modelParameters);
    auto exBothRegion=storm::storage::ParameterRegion<storm::RationalFunction>::parseRegion("0.1<=pK<=0.7,0.2<=pL<=0.8,0.15<=TOMsg<=0.65,0.3<=TOAck<=0.9", modelParameters);
    auto allVioRegion=storm::storage::ParameterRegion<storm::RationalFunction>::parseRegion("0.1<=pK<=0.4,0.2<=pL<=0.3,0.15<=TOMsg<=0.3,0.1<=TOAck<=0.2", modelParameters);

    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::AllSat, regionChecker.analyzeRegion(allSatRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::ExistsBoth, regionChecker.analyzeRegion(exBothRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));
    EXPECT_EQ(storm::modelchecker::parametric::RegionCheckResult::AllViolated, regionChecker.analyzeRegion(allVioRegion, storm::modelchecker::parametric::RegionCheckResult::Unknown, true));
    
    carl::VariablePool::getInstance().clear();
}





#endif
