#include "gtest/gtest.h"
#include "storm-config.h"

#ifdef STORM_HAVE_CARL

#include "storm/adapters/CarlAdapter.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/modules/RegionSettings.h"

#include "utility/storm.h"
#include "storm/models/sparse/Model.h"
#include "modelchecker/region/SparseRegionModelChecker.h"
#include "modelchecker/region/ParameterRegion.h"

TEST(SparseMdpRegionModelCheckerTest, two_dice_Prob) {
    
    std::string programFile = STORM_CPP_BASE_PATH "/examples/pmdp/two_dice/two_dice.nm";
    std::string formulaFile = STORM_CPP_BASE_PATH "/examples/pmdp/two_dice/two_dice.prctl"; //P<=0.17 [F \"doubles\" ]";
    std::string constantsAsString = ""; //e.g. pL=0.9,TOACK=0.5
    
    carl::VariablePool::getInstance().clear();

    storm::prism::Program program = storm::parseProgram(programFile);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::parseFormulasForPrismProgram(formulaFile, program);
    std::shared_ptr<storm::models::sparse::Mdp<storm::RationalFunction>> model = storm::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Mdp<storm::RationalFunction>>();
    auto const& regionSettings = storm::settings::getModule<storm::settings::modules::RegionSettings>();
    storm::modelchecker::region::SparseRegionModelCheckerSettings settings(regionSettings.getSampleMode(), regionSettings.getApproxMode(), regionSettings.getSmtMode());
    auto mdpModelchecker = std::make_shared<storm::modelchecker::region::SparseMdpRegionModelChecker<storm::models::sparse::Mdp<storm::RationalFunction>, double>>(model, settings);
    mdpModelchecker->specifyFormula(formulas[0]);

    
    auto allSatRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.495<=p1<=0.5,0.5<=p2<=0.505");
    auto exBothRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.45<=p1<=0.55,0.45<=p2<=0.55");
    auto allVioRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.6<=p1<=0.7,0.6<=p2<=0.6");

    EXPECT_TRUE(mdpModelchecker->checkFormulaOnSamplingPoint(allSatRegion.getSomePoint()));
    EXPECT_FALSE(mdpModelchecker->checkFormulaOnSamplingPoint(allVioRegion.getSomePoint()));
    
    //Test the methods provided in storm.h
    EXPECT_TRUE(storm::checkSamplingPoint(mdpModelchecker,allSatRegion.getLowerBoundaries()));
    EXPECT_TRUE(storm::checkSamplingPoint(mdpModelchecker,allSatRegion.getUpperBoundaries()));
    EXPECT_FALSE(storm::checkSamplingPoint(mdpModelchecker,exBothRegion.getLowerBoundaries()));
    EXPECT_FALSE(storm::checkSamplingPoint(mdpModelchecker,exBothRegion.getUpperBoundaries()));
    EXPECT_TRUE(storm::checkSamplingPoint(mdpModelchecker,exBothRegion.getVerticesOfRegion(exBothRegion.getVariables())[1]));
    EXPECT_TRUE(storm::checkSamplingPoint(mdpModelchecker,exBothRegion.getVerticesOfRegion(exBothRegion.getVariables())[2]));
    EXPECT_FALSE(storm::checkSamplingPoint(mdpModelchecker,exBothRegion.getUpperBoundaries()));
    EXPECT_FALSE(storm::checkSamplingPoint(mdpModelchecker,allVioRegion.getLowerBoundaries()));
    EXPECT_FALSE(storm::checkSamplingPoint(mdpModelchecker,allVioRegion.getUpperBoundaries()));
    
    EXPECT_TRUE(storm::checkRegionApproximation(mdpModelchecker, allSatRegion.getLowerBoundaries(), allSatRegion.getUpperBoundaries(), true));
    EXPECT_FALSE(storm::checkRegionApproximation(mdpModelchecker, allSatRegion.getLowerBoundaries(), allSatRegion.getUpperBoundaries(), false));
    EXPECT_FALSE(storm::checkRegionApproximation(mdpModelchecker, exBothRegion.getLowerBoundaries(), exBothRegion.getUpperBoundaries(), true));
    EXPECT_FALSE(storm::checkRegionApproximation(mdpModelchecker, exBothRegion.getLowerBoundaries(), exBothRegion.getUpperBoundaries(), false));
    EXPECT_FALSE(storm::checkRegionApproximation(mdpModelchecker, allVioRegion.getLowerBoundaries(), allVioRegion.getUpperBoundaries(), true));
    EXPECT_TRUE(storm::checkRegionApproximation(mdpModelchecker, allVioRegion.getLowerBoundaries(), allVioRegion.getUpperBoundaries(), false));
    
    //Remaining tests..
    EXPECT_NEAR(0.1666665285, mdpModelchecker->getReachabilityValue(allSatRegion.getLowerBoundaries()), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.1666665529, mdpModelchecker->getReachabilityValue(allSatRegion.getUpperBoundaries()),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.1716553235, mdpModelchecker->getReachabilityValue(exBothRegion.getLowerBoundaries()), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.1709666953, mdpModelchecker->getReachabilityValue(exBothRegion.getUpperBoundaries()),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.1826972576, mdpModelchecker->getReachabilityValue(allVioRegion.getLowerBoundaries()), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.1964429282, mdpModelchecker->getReachabilityValue(allVioRegion.getUpperBoundaries()),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    //test approximative method
    settings = storm::modelchecker::region::SparseRegionModelCheckerSettings(storm::settings::modules::RegionSettings::SampleMode::INSTANTIATE, storm::settings::modules::RegionSettings::ApproxMode::TESTFIRST, storm::settings::modules::RegionSettings::SmtMode::OFF);
    mdpModelchecker = std::make_shared<storm::modelchecker::region::SparseMdpRegionModelChecker<storm::models::sparse::Mdp<storm::RationalFunction>, double>>(model, settings);
    mdpModelchecker->specifyFormula(formulas[0]);
    
    ASSERT_TRUE(mdpModelchecker->getSettings().doApprox());
    ASSERT_TRUE(mdpModelchecker->getSettings().doSample());
    ASSERT_FALSE(mdpModelchecker->getSettings().doSmt());

    mdpModelchecker->checkRegion(allSatRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLSAT), allSatRegion.getCheckResult());
    mdpModelchecker->checkRegion(exBothRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::EXISTSBOTH), exBothRegion.getCheckResult());
    mdpModelchecker->checkRegion(allVioRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLVIOLATED), allVioRegion.getCheckResult());

    carl::VariablePool::getInstance().clear();
}

TEST(SparseMdpRegionModelCheckerTest, coin_Prob) {
    
    std::string programFile = STORM_CPP_BASE_PATH "/examples/pmdp/coin2/coin2_2.pm";
    std::string formulaAsString = "P>0.25 [F \"finished\"&\"all_coins_equal_1\" ]";
    std::string constantsAsString = ""; //e.g. pL=0.9,TOACK=0.5
    
    carl::VariablePool::getInstance().clear();

    storm::prism::Program program = storm::parseProgram(programFile);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::parseFormulasForPrismProgram(formulaAsString, program);
    std::shared_ptr<storm::models::sparse::Mdp<storm::RationalFunction>> model = storm::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Mdp<storm::RationalFunction>>();
    auto const& regionSettings = storm::settings::getModule<storm::settings::modules::RegionSettings>();
    storm::modelchecker::region::SparseRegionModelCheckerSettings settings(regionSettings.getSampleMode(), regionSettings.getApproxMode(), regionSettings.getSmtMode());
    auto mdpModelchecker = std::make_shared<storm::modelchecker::region::SparseMdpRegionModelChecker<storm::models::sparse::Mdp<storm::RationalFunction>, double>>(model, settings);
    mdpModelchecker->specifyFormula(formulas[0]);
    
    //start testing
    auto allSatRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.3<=p1<=0.45,0.2<=p2<=0.54");
    auto exBothRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.4<=p1<=0.65,0.5<=p2<=0.7");
    auto allVioRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.4<=p1<=0.7,0.55<=p2<=0.6");
    
    EXPECT_TRUE(mdpModelchecker->checkFormulaOnSamplingPoint(allSatRegion.getSomePoint()));
    EXPECT_FALSE(mdpModelchecker->checkFormulaOnSamplingPoint(allVioRegion.getSomePoint()));

    EXPECT_NEAR(0.95128124239, mdpModelchecker->getReachabilityValue(allSatRegion.getLowerBoundaries()), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.26787251126, mdpModelchecker->getReachabilityValue(allSatRegion.getUpperBoundaries()),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.41880006098, mdpModelchecker->getReachabilityValue(exBothRegion.getLowerBoundaries()), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.01535089684, mdpModelchecker->getReachabilityValue(exBothRegion.getUpperBoundaries()),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.24952791523, mdpModelchecker->getReachabilityValue(allVioRegion.getLowerBoundaries()), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.01711494956, mdpModelchecker->getReachabilityValue(allVioRegion.getUpperBoundaries()),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
   
    //test approximative method
    settings = storm::modelchecker::region::SparseRegionModelCheckerSettings(storm::settings::modules::RegionSettings::SampleMode::INSTANTIATE, storm::settings::modules::RegionSettings::ApproxMode::TESTFIRST, storm::settings::modules::RegionSettings::SmtMode::OFF);
    mdpModelchecker = std::make_shared<storm::modelchecker::region::SparseMdpRegionModelChecker<storm::models::sparse::Mdp<storm::RationalFunction>, double>>(model, settings);
    mdpModelchecker->specifyFormula(formulas[0]);
    ASSERT_TRUE(mdpModelchecker->getSettings().doApprox());
    ASSERT_TRUE(mdpModelchecker->getSettings().doSample());
    ASSERT_FALSE(mdpModelchecker->getSettings().doSmt());
    mdpModelchecker->checkRegion(allSatRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLSAT), allSatRegion.getCheckResult());
    mdpModelchecker->checkRegion(exBothRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::EXISTSBOTH), exBothRegion.getCheckResult());
    mdpModelchecker->checkRegion(allVioRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLVIOLATED), allVioRegion.getCheckResult());

    carl::VariablePool::getInstance().clear();
}

#endif
