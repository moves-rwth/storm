#include "gtest/gtest.h"
#include "storm-config.h"

#ifdef STORM_HAVE_CARL

#include "src/adapters/CarlAdapter.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/modules/GeneralSettings.h"
#include "src/settings/modules/RegionSettings.h"

#include "utility/storm.h"
#include "src/models/sparse/Model.h"
#include "modelchecker/region/SparseRegionModelChecker.h"
#include "modelchecker/region/ParameterRegion.h"

TEST(SparseMdpRegionModelCheckerTest, two_dice_Prob) {
    
    std::string programFile = STORM_CPP_BASE_PATH "/examples/pmdp/two_dice/two_dice.nm";
    std::string formulaFile = STORM_CPP_BASE_PATH "/examples/pmdp/two_dice/two_dice.prctl"; //P<=0.17 [F \"doubles\" ]";
    std::string constantsAsString = ""; //e.g. pL=0.9,TOACK=0.5
    
    std::shared_ptr<storm::modelchecker::region::AbstractSparseRegionModelChecker<storm::RationalFunction, double>> modelchecker;
    ASSERT_TRUE(storm::initializeRegionModelChecker(modelchecker, programFile, formulaFile, constantsAsString));
    
    auto allSatRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.495<=p1<=0.5,0.5<=p2<=0.505");
    auto exBothRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.45<=p1<=0.55,0.45<=p2<=0.55");
    auto allVioRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.6<=p1<=0.7,0.6<=p2<=0.6");

    EXPECT_TRUE(modelchecker->checkFormulaOnSamplingPoint(allSatRegion.getSomePoint()));
    EXPECT_FALSE(modelchecker->checkFormulaOnSamplingPoint(allVioRegion.getSomePoint()));
    
    //Test the methods provided in storm.h
    EXPECT_TRUE(storm::checkSamplingPoint(modelchecker,allSatRegion.getLowerBoundaries()));
    EXPECT_TRUE(storm::checkSamplingPoint(modelchecker,allSatRegion.getUpperBoundaries()));
    EXPECT_FALSE(storm::checkSamplingPoint(modelchecker,exBothRegion.getLowerBoundaries()));
    EXPECT_FALSE(storm::checkSamplingPoint(modelchecker,exBothRegion.getUpperBoundaries()));
    EXPECT_TRUE(storm::checkSamplingPoint(modelchecker,exBothRegion.getVerticesOfRegion(exBothRegion.getVariables())[1]));
    EXPECT_TRUE(storm::checkSamplingPoint(modelchecker,exBothRegion.getVerticesOfRegion(exBothRegion.getVariables())[2]));
    EXPECT_FALSE(storm::checkSamplingPoint(modelchecker,exBothRegion.getUpperBoundaries()));
    EXPECT_FALSE(storm::checkSamplingPoint(modelchecker,allVioRegion.getLowerBoundaries()));
    EXPECT_FALSE(storm::checkSamplingPoint(modelchecker,allVioRegion.getUpperBoundaries()));
    
    EXPECT_TRUE(storm::checkRegionApproximation(modelchecker, allSatRegion.getLowerBoundaries(), allSatRegion.getUpperBoundaries(), true));
    EXPECT_FALSE(storm::checkRegionApproximation(modelchecker, allSatRegion.getLowerBoundaries(), allSatRegion.getUpperBoundaries(), false));
    EXPECT_FALSE(storm::checkRegionApproximation(modelchecker, exBothRegion.getLowerBoundaries(), exBothRegion.getUpperBoundaries(), true));
    EXPECT_FALSE(storm::checkRegionApproximation(modelchecker, exBothRegion.getLowerBoundaries(), exBothRegion.getUpperBoundaries(), false));
    EXPECT_FALSE(storm::checkRegionApproximation(modelchecker, allVioRegion.getLowerBoundaries(), allVioRegion.getUpperBoundaries(), true));
    EXPECT_TRUE(storm::checkRegionApproximation(modelchecker, allVioRegion.getLowerBoundaries(), allVioRegion.getUpperBoundaries(), false));
    
    //Remaining tests..
    EXPECT_NEAR(0.1666665285, modelchecker->getReachabilityValue(allSatRegion.getLowerBoundaries()), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.1666665529, modelchecker->getReachabilityValue(allSatRegion.getUpperBoundaries()),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.1716553235, modelchecker->getReachabilityValue(exBothRegion.getLowerBoundaries()), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.1709666953, modelchecker->getReachabilityValue(exBothRegion.getUpperBoundaries()),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.1826972576, modelchecker->getReachabilityValue(allVioRegion.getLowerBoundaries()), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.1964429282, modelchecker->getReachabilityValue(allVioRegion.getUpperBoundaries()),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    //test approximative method
    storm::settings::mutableRegionSettings().modifyModes(storm::settings::modules::RegionSettings::ApproxMode::TESTFIRST, storm::settings::modules::RegionSettings::SampleMode::INSTANTIATE, storm::settings::modules::RegionSettings::SmtMode::OFF);
    ASSERT_TRUE(storm::settings::regionSettings().doApprox());
    ASSERT_TRUE(storm::settings::regionSettings().doSample());
    ASSERT_FALSE(storm::settings::regionSettings().doSmt());
    
    modelchecker->checkRegion(allSatRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLSAT), allSatRegion.getCheckResult());
    modelchecker->checkRegion(exBothRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::EXISTSBOTH), exBothRegion.getCheckResult());
    modelchecker->checkRegion(allVioRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLVIOLATED), allVioRegion.getCheckResult());
    
    storm::settings::mutableRegionSettings().resetModes();
    carl::VariablePool::getInstance().clear();
}

TEST(SparseMdpRegionModelCheckerTest, coin_Prob) {
    
    std::string programFile = STORM_CPP_BASE_PATH "/examples/pmdp/coin2/coin2_2.pm";
    std::string formulaAsString = "P>0.25 [F \"finished\"&\"all_coins_equal_1\" ]";
    std::string constantsAsString = ""; //e.g. pL=0.9,TOACK=0.5
    
    std::shared_ptr<storm::modelchecker::region::AbstractSparseRegionModelChecker<storm::RationalFunction, double>> modelchecker;
    ASSERT_TRUE(storm::initializeRegionModelChecker(modelchecker, programFile, formulaAsString, constantsAsString));
    
    //start testing
    auto allSatRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.3<=p1<=0.45,0.2<=p2<=0.54");
    auto exBothRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.4<=p1<=0.65,0.5<=p2<=0.7");
    auto allVioRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.4<=p1<=0.7,0.55<=p2<=0.6");
    
    EXPECT_TRUE(modelchecker->checkFormulaOnSamplingPoint(allSatRegion.getSomePoint()));
    EXPECT_FALSE(modelchecker->checkFormulaOnSamplingPoint(allVioRegion.getSomePoint()));

    EXPECT_NEAR(0.95128124239, modelchecker->getReachabilityValue(allSatRegion.getLowerBoundaries()), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.26787251126, modelchecker->getReachabilityValue(allSatRegion.getUpperBoundaries()),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.41880006098, modelchecker->getReachabilityValue(exBothRegion.getLowerBoundaries()), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.01535089684, modelchecker->getReachabilityValue(exBothRegion.getUpperBoundaries()),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.24952791523, modelchecker->getReachabilityValue(allVioRegion.getLowerBoundaries()), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.01711494956, modelchecker->getReachabilityValue(allVioRegion.getUpperBoundaries()),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
   
    //test approximative method
    storm::settings::mutableRegionSettings().modifyModes(storm::settings::modules::RegionSettings::ApproxMode::TESTFIRST, storm::settings::modules::RegionSettings::SampleMode::INSTANTIATE, storm::settings::modules::RegionSettings::SmtMode::OFF);
    ASSERT_TRUE(storm::settings::regionSettings().doApprox());
    ASSERT_TRUE(storm::settings::regionSettings().doSample());
    ASSERT_FALSE(storm::settings::regionSettings().doSmt());
    modelchecker->checkRegion(allSatRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLSAT), allSatRegion.getCheckResult());
    modelchecker->checkRegion(exBothRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::EXISTSBOTH), exBothRegion.getCheckResult());
    modelchecker->checkRegion(allVioRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLVIOLATED), allVioRegion.getCheckResult());

    storm::settings::mutableRegionSettings().resetModes();
    carl::VariablePool::getInstance().clear();
}

#endif
