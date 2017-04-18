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
#include "modelchecker/region/SparseDtmcRegionModelChecker.h"
#include "modelchecker/region/ParameterRegion.h"

TEST(SparseDtmcRegionModelCheckerTest, Brp_Prob) {
    
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/brp16_2.pm";
    std::string formulaAsString = "P<=0.84 [F s=5 ]";
    std::string constantsAsString = ""; //e.g. pL=0.9,TOACK=0.5

    // Program and formula
    storm::prism::Program program = storm::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::extractFormulasFromProperties(storm::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    auto const& regionSettings = storm::settings::getModule<storm::settings::modules::RegionSettings>();
    storm::modelchecker::region::SparseRegionModelCheckerSettings settings(regionSettings.getSampleMode(), regionSettings.getApproxMode(), regionSettings.getSmtMode());
    auto dtmcModelchecker = std::make_shared<storm::modelchecker::region::SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>>(model, settings);
    dtmcModelchecker->specifyFormula(formulas[0]);

    //start testing
    auto allSatRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.7<=pL<=0.9,0.75<=pK<=0.95");
    auto exBothRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.4<=pL<=0.65,0.75<=pK<=0.95");
    auto allVioRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.1<=pL<=0.73,0.2<=pK<=0.715");

    EXPECT_TRUE(dtmcModelchecker->checkFormulaOnSamplingPoint(allSatRegion.getSomePoint()));
    EXPECT_FALSE(dtmcModelchecker->checkFormulaOnSamplingPoint(allVioRegion.getSomePoint()));
    
    EXPECT_NEAR(0.8369631407, dtmcModelchecker->getReachabilityValue(allSatRegion.getLowerBoundaries()), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.8369631407, storm::utility::convertNumber<double>(dtmcModelchecker->evaluateReachabilityFunction(allSatRegion.getLowerBoundaries())),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.0476784174, dtmcModelchecker->getReachabilityValue(allSatRegion.getUpperBoundaries()),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.0476784174, storm::utility::convertNumber<double>(dtmcModelchecker->evaluateReachabilityFunction(allSatRegion.getUpperBoundaries())),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.9987948367, dtmcModelchecker->getReachabilityValue(exBothRegion.getLowerBoundaries()), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.9987948367, storm::utility::convertNumber<double>(dtmcModelchecker->evaluateReachabilityFunction(exBothRegion.getLowerBoundaries())),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.6020480995, dtmcModelchecker->getReachabilityValue(exBothRegion.getUpperBoundaries()),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.6020480995, storm::utility::convertNumber<double>(dtmcModelchecker->evaluateReachabilityFunction(exBothRegion.getUpperBoundaries())),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(1.0000000000, dtmcModelchecker->getReachabilityValue(allVioRegion.getLowerBoundaries()), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(1.0000000000, storm::utility::convertNumber<double>(dtmcModelchecker->evaluateReachabilityFunction(allVioRegion.getLowerBoundaries())),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.8429289733, dtmcModelchecker->getReachabilityValue(allVioRegion.getUpperBoundaries()),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.8429289733, storm::utility::convertNumber<double>(dtmcModelchecker->evaluateReachabilityFunction(allVioRegion.getUpperBoundaries())),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    //test approximative method
    settings = storm::modelchecker::region::SparseRegionModelCheckerSettings(storm::settings::modules::RegionSettings::SampleMode::INSTANTIATE, storm::settings::modules::RegionSettings::ApproxMode::TESTFIRST,  storm::settings::modules::RegionSettings::SmtMode::OFF);
    dtmcModelchecker = std::make_shared<storm::modelchecker::region::SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>>(model, settings);
    dtmcModelchecker->specifyFormula(formulas[0]);
    ASSERT_TRUE(settings.doApprox());
    ASSERT_TRUE(settings.doSample());
    ASSERT_FALSE(settings.doSmt());
    dtmcModelchecker->checkRegion(allSatRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLSAT), allSatRegion.getCheckResult());
    dtmcModelchecker->checkRegion(exBothRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::EXISTSBOTH), exBothRegion.getCheckResult());
    dtmcModelchecker->checkRegion(allVioRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLVIOLATED), allVioRegion.getCheckResult());

    //test smt method (the regions need to be created again, because the old ones have some information stored in their internal state)
    auto allSatRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.7<=pL<=0.9,0.75<=pK<=0.95");
    auto exBothRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.4<=pL<=0.65,0.75<=pK<=0.95");
    auto allVioRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.1<=pL<=0.9,0.2<=pK<=0.5");

    settings = storm::modelchecker::region::SparseRegionModelCheckerSettings(storm::settings::modules::RegionSettings::SampleMode::EVALUATE, storm::settings::modules::RegionSettings::ApproxMode::OFF,  storm::settings::modules::RegionSettings::SmtMode::FUNCTION);
    dtmcModelchecker = std::make_shared<storm::modelchecker::region::SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>>(model, settings);
    dtmcModelchecker->specifyFormula(formulas[0]);
    ASSERT_FALSE(settings.doApprox());
    ASSERT_TRUE(settings.doSample());
    ASSERT_TRUE(settings.doSmt());
    dtmcModelchecker->checkRegion(allSatRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLSAT), allSatRegionSmt.getCheckResult());
    dtmcModelchecker->checkRegion(exBothRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::EXISTSBOTH), exBothRegionSmt.getCheckResult());
    dtmcModelchecker->checkRegion(allVioRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLVIOLATED), allVioRegionSmt.getCheckResult());

    carl::VariablePool::getInstance().clear();
}

TEST(SparseDtmcRegionModelCheckerTest, Brp_Rew) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/brp_rewards16_2.pm";
    std::string formulaAsString = "R>2.5 [F ((s=5) | (s=0&srep=3)) ]";
    std::string constantsAsString = "pL=0.9,TOAck=0.5";
    carl::VariablePool::getInstance().clear();
    
    storm::prism::Program program = storm::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::extractFormulasFromProperties(storm::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    auto const& regionSettings = storm::settings::getModule<storm::settings::modules::RegionSettings>();
    storm::modelchecker::region::SparseRegionModelCheckerSettings settings(regionSettings.getSampleMode(), regionSettings.getApproxMode(), regionSettings.getSmtMode());
    auto dtmcModelchecker = std::make_shared<storm::modelchecker::region::SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>>(model, settings);
    dtmcModelchecker->specifyFormula(formulas[0]);
    //start testing
    auto allSatRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.7<=pK<=0.875,0.75<=TOMsg<=0.95");
    auto exBothRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.6<=pK<=0.9,0.5<=TOMsg<=0.95");
    auto exBothHardRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.5<=pK<=0.75,0.3<=TOMsg<=0.4"); //this region has a local maximum!
    auto allVioRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.1<=pK<=0.3,0.2<=TOMsg<=0.3");

    EXPECT_TRUE(dtmcModelchecker->checkFormulaOnSamplingPoint(allSatRegion.getSomePoint()));
    EXPECT_FALSE(dtmcModelchecker->checkFormulaOnSamplingPoint(allVioRegion.getSomePoint()));
    
    EXPECT_NEAR(4.367791292, dtmcModelchecker->getReachabilityValue(allSatRegion.getLowerBoundaries()), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(4.367791292, storm::utility::convertNumber<double>(dtmcModelchecker->evaluateReachabilityFunction(allSatRegion.getLowerBoundaries())),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(3.044795147, dtmcModelchecker->getReachabilityValue(allSatRegion.getUpperBoundaries()),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(3.044795147, storm::utility::convertNumber<double>(dtmcModelchecker->evaluateReachabilityFunction(allSatRegion.getUpperBoundaries())),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(3.182535759, dtmcModelchecker->getReachabilityValue(exBothRegion.getLowerBoundaries()), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(3.182535759, storm::utility::convertNumber<double>(dtmcModelchecker->evaluateReachabilityFunction(exBothRegion.getLowerBoundaries())),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(2.609602197, dtmcModelchecker->getReachabilityValue(exBothRegion.getUpperBoundaries()),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(2.609602197, storm::utility::convertNumber<double>(dtmcModelchecker->evaluateReachabilityFunction(exBothRegion.getUpperBoundaries())),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(1.842551039, dtmcModelchecker->getReachabilityValue(exBothHardRegion.getLowerBoundaries()), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(1.842551039, storm::utility::convertNumber<double>(dtmcModelchecker->evaluateReachabilityFunction(exBothHardRegion.getLowerBoundaries())),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(2.453500364, dtmcModelchecker->getReachabilityValue(exBothHardRegion.getUpperBoundaries()),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(2.453500364, storm::utility::convertNumber<double>(dtmcModelchecker->evaluateReachabilityFunction(exBothHardRegion.getUpperBoundaries())),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.6721974438, dtmcModelchecker->getReachabilityValue(allVioRegion.getLowerBoundaries()), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.6721974438, storm::utility::convertNumber<double>(dtmcModelchecker->evaluateReachabilityFunction(allVioRegion.getLowerBoundaries())),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(1.308324558, dtmcModelchecker->getReachabilityValue(allVioRegion.getUpperBoundaries()),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(1.308324558, storm::utility::convertNumber<double>(dtmcModelchecker->evaluateReachabilityFunction(allVioRegion.getUpperBoundaries())),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    //test approximative method
    settings = storm::modelchecker::region::SparseRegionModelCheckerSettings(storm::settings::modules::RegionSettings::SampleMode::INSTANTIATE, storm::settings::modules::RegionSettings::ApproxMode::TESTFIRST,  storm::settings::modules::RegionSettings::SmtMode::OFF);
    dtmcModelchecker = std::make_shared<storm::modelchecker::region::SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>>(model, settings);
    dtmcModelchecker->specifyFormula(formulas[0]);
    ASSERT_TRUE(dtmcModelchecker->getSettings().doApprox());
    ASSERT_TRUE(dtmcModelchecker->getSettings().doSample());
    ASSERT_FALSE(dtmcModelchecker->getSettings().doSmt());
    dtmcModelchecker->checkRegion(allSatRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLSAT), allSatRegion.getCheckResult());
    dtmcModelchecker->checkRegion(exBothRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::EXISTSBOTH), exBothRegion.getCheckResult());
    dtmcModelchecker->checkRegion(exBothHardRegion);
    dtmcModelchecker->checkRegion(allVioRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLVIOLATED), allVioRegion.getCheckResult());

    //test smt method (the regions need to be created again, because the old ones have some information stored in their internal state)
    auto allSatRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.7<=pK<=0.9,0.75<=TOMsg<=0.95");
    auto exBothRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.3<=pK<=0.5,0.5<=TOMsg<=0.75");
    auto exBothHardRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.5<=pK<=0.75,0.3<=TOMsg<=0.4"); //this region has a local maximum!
    auto allVioRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.1<=pK<=0.3,0.2<=TOMsg<=0.3");
    settings = storm::modelchecker::region::SparseRegionModelCheckerSettings(storm::settings::modules::RegionSettings::SampleMode::EVALUATE, storm::settings::modules::RegionSettings::ApproxMode::OFF,  storm::settings::modules::RegionSettings::SmtMode::FUNCTION);
    dtmcModelchecker = std::make_shared<storm::modelchecker::region::SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>>(model, settings);
    dtmcModelchecker->specifyFormula(formulas[0]);
    ASSERT_FALSE(dtmcModelchecker->getSettings().doApprox());
    ASSERT_TRUE(dtmcModelchecker->getSettings().doSample());
    ASSERT_TRUE(dtmcModelchecker->getSettings().doSmt());
    dtmcModelchecker->checkRegion(allSatRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLSAT), allSatRegionSmt.getCheckResult());
    dtmcModelchecker->checkRegion(exBothRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::EXISTSBOTH), exBothRegionSmt.getCheckResult());
    dtmcModelchecker->checkRegion(exBothHardRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::EXISTSBOTH), exBothHardRegionSmt.getCheckResult());
    dtmcModelchecker->checkRegion(allVioRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLVIOLATED), allVioRegionSmt.getCheckResult());
    
    //test smt + approx
    auto exBothHardRegionSmtApp=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.5<=pK<=0.75,0.3<=TOMsg<=0.4"); //this region has a local maximum!
    settings = storm::modelchecker::region::SparseRegionModelCheckerSettings(storm::settings::modules::RegionSettings::SampleMode::EVALUATE, storm::settings::modules::RegionSettings::ApproxMode::TESTFIRST, storm::settings::modules::RegionSettings::SmtMode::FUNCTION);
    dtmcModelchecker = std::make_shared<storm::modelchecker::region::SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>>(model, settings);
    dtmcModelchecker->specifyFormula(formulas[0]);
    ASSERT_TRUE(dtmcModelchecker->getSettings().doApprox());
    ASSERT_TRUE(dtmcModelchecker->getSettings().doSample());
    ASSERT_TRUE(dtmcModelchecker->getSettings().doSmt());
    dtmcModelchecker->checkRegion(exBothHardRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::EXISTSBOTH), exBothHardRegionSmtApp.getCheckResult());

    carl::VariablePool::getInstance().clear();
}

TEST(SparseDtmcRegionModelCheckerTest, Brp_Rew_Infty) {
    
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/brp_rewards16_2.pm";
    std::string formulaAsString = "R>2.5 [F (s=0&srep=3) ]";
    std::string constantsAsString = "";
    carl::VariablePool::getInstance().clear();
    storm::prism::Program program = storm::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::extractFormulasFromProperties(storm::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    auto const& regionSettings = storm::settings::getModule<storm::settings::modules::RegionSettings>();
    storm::modelchecker::region::SparseRegionModelCheckerSettings settings(regionSettings.getSampleMode(), regionSettings.getApproxMode(), regionSettings.getSmtMode());
    auto dtmcModelchecker = std::make_shared<storm::modelchecker::region::SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>>(model, settings);
    dtmcModelchecker->specifyFormula(formulas[0]);
    //start testing
    auto allSatRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("");
    
    EXPECT_TRUE(dtmcModelchecker->checkFormulaOnSamplingPoint(allSatRegion.getSomePoint()));
    EXPECT_EQ(storm::utility::infinity<double>(), dtmcModelchecker->getReachabilityValue(allSatRegion.getLowerBoundaries()));
    
    //test approximative method
    settings = storm::modelchecker::region::SparseRegionModelCheckerSettings(storm::settings::modules::RegionSettings::SampleMode::INSTANTIATE, storm::settings::modules::RegionSettings::ApproxMode::TESTFIRST,  storm::settings::modules::RegionSettings::SmtMode::OFF);
    dtmcModelchecker = std::make_shared<storm::modelchecker::region::SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>>(model, settings);
    dtmcModelchecker->specifyFormula(formulas[0]);
    ASSERT_TRUE(dtmcModelchecker->getSettings().doApprox());
    ASSERT_TRUE(dtmcModelchecker->getSettings().doSample());
    ASSERT_FALSE(dtmcModelchecker->getSettings().doSmt());
    dtmcModelchecker->checkRegion(allSatRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLSAT), allSatRegion.getCheckResult());

    //test smt method (the regions need to be created again, because the old ones have some information stored in their internal state)
    auto allSatRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("");
    settings = storm::modelchecker::region::SparseRegionModelCheckerSettings(storm::settings::modules::RegionSettings::SampleMode::EVALUATE, storm::settings::modules::RegionSettings::ApproxMode::OFF,  storm::settings::modules::RegionSettings::SmtMode::FUNCTION);
    dtmcModelchecker = std::make_shared<storm::modelchecker::region::SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>>(model, settings);
    dtmcModelchecker->specifyFormula(formulas[0]);
    ASSERT_FALSE(dtmcModelchecker->getSettings().doApprox());
    ASSERT_TRUE(dtmcModelchecker->getSettings().doSample());
    ASSERT_TRUE(dtmcModelchecker->getSettings().doSmt());
    dtmcModelchecker->checkRegion(allSatRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLSAT), allSatRegionSmt.getCheckResult());

    carl::VariablePool::getInstance().clear();
}

TEST(SparseDtmcRegionModelCheckerTest, DISABLED_Brp_Rew_4Par) {
    
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/brp_rewards16_2.pm";
    std::string formulaAsString = "R>2.5 [F ((s=5) | (s=0&srep=3)) ]";
    std::string constantsAsString = ""; //!! this model will have 4 parameters
    carl::VariablePool::getInstance().clear();
    storm::prism::Program program = storm::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::extractFormulasFromProperties(storm::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    auto const& regionSettings = storm::settings::getModule<storm::settings::modules::RegionSettings>();
    storm::modelchecker::region::SparseRegionModelCheckerSettings settings(regionSettings.getSampleMode(), regionSettings.getApproxMode(), regionSettings.getSmtMode());
    auto dtmcModelchecker = std::make_shared<storm::modelchecker::region::SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>>(model, settings);
    dtmcModelchecker->specifyFormula(formulas[0]);
    //start testing
    auto allSatRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.7<=pK<=0.9,0.6<=pL<=0.85,0.9<=TOMsg<=0.95,0.85<=TOAck<=0.9");
    auto exBothRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.1<=pK<=0.7,0.2<=pL<=0.8,0.15<=TOMsg<=0.65,0.3<=TOAck<=0.9");
    auto allVioRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.1<=pK<=0.4,0.2<=pL<=0.3,0.15<=TOMsg<=0.3,0.1<=TOAck<=0.2");

    EXPECT_TRUE(dtmcModelchecker->checkFormulaOnSamplingPoint(allSatRegion.getSomePoint()));
    EXPECT_FALSE(dtmcModelchecker->checkFormulaOnSamplingPoint(allVioRegion.getSomePoint()));
    
    EXPECT_NEAR(4.834779705, dtmcModelchecker->getReachabilityValue(allSatRegion.getLowerBoundaries()), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(4.834779705, storm::utility::convertNumber<double>(dtmcModelchecker->evaluateReachabilityFunction(allSatRegion.getLowerBoundaries())),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(4.674651623, dtmcModelchecker->getReachabilityValue(exBothRegion.getUpperBoundaries()),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(4.674651623, storm::utility::convertNumber<double>(dtmcModelchecker->evaluateReachabilityFunction(exBothRegion.getUpperBoundaries())),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.4467496536, dtmcModelchecker->getReachabilityValue(allVioRegion.getLowerBoundaries()), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.4467496536, storm::utility::convertNumber<double>(dtmcModelchecker->evaluateReachabilityFunction(allVioRegion.getLowerBoundaries())),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    //test approximative method
    settings = storm::modelchecker::region::SparseRegionModelCheckerSettings(storm::settings::modules::RegionSettings::SampleMode::INSTANTIATE, storm::settings::modules::RegionSettings::ApproxMode::TESTFIRST,  storm::settings::modules::RegionSettings::SmtMode::OFF);
    dtmcModelchecker = std::make_shared<storm::modelchecker::region::SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>>(model, settings);
    dtmcModelchecker->specifyFormula(formulas[0]);
    ASSERT_TRUE(dtmcModelchecker->getSettings().doApprox());
    ASSERT_TRUE(dtmcModelchecker->getSettings().doSample());
    ASSERT_FALSE(dtmcModelchecker->getSettings().doSmt());
    dtmcModelchecker->checkRegion(allSatRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLSAT), allSatRegion.getCheckResult());
    dtmcModelchecker->checkRegion(exBothRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::EXISTSBOTH), exBothRegion.getCheckResult());
    dtmcModelchecker->checkRegion(allVioRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLVIOLATED), allVioRegion.getCheckResult());

    //test smt method (the regions need to be created again, because the old ones have some information stored in their internal state)
    auto allSatRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.7<=pK<=0.9,0.6<=pL<=0.85,0.9<=TOMsg<=0.95,0.85<=TOAck<=0.9");
    auto exBothRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.1<=pK<=0.7,0.2<=pL<=0.8,0.15<=TOMsg<=0.65,0.3<=TOAck<=0.9");
    auto allVioRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.1<=pK<=0.4,0.2<=pL<=0.3,0.15<=TOMsg<=0.3,0.1<=TOAck<=0.2");
    settings = storm::modelchecker::region::SparseRegionModelCheckerSettings(storm::settings::modules::RegionSettings::SampleMode::EVALUATE, storm::settings::modules::RegionSettings::ApproxMode::OFF,  storm::settings::modules::RegionSettings::SmtMode::FUNCTION);
    dtmcModelchecker = std::make_shared<storm::modelchecker::region::SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>>(model, settings);
    dtmcModelchecker->specifyFormula(formulas[0]);
    ASSERT_FALSE(dtmcModelchecker->getSettings().doApprox());
    ASSERT_TRUE(dtmcModelchecker->getSettings().doSample());
    ASSERT_TRUE(dtmcModelchecker->getSettings().doSmt());
    dtmcModelchecker->checkRegion(allSatRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLSAT), allSatRegionSmt.getCheckResult());
    dtmcModelchecker->checkRegion(exBothRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::EXISTSBOTH), exBothRegionSmt.getCheckResult());
    dtmcModelchecker->checkRegion(allVioRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLVIOLATED), allVioRegionSmt.getCheckResult());

    carl::VariablePool::getInstance().clear();
}

TEST(SparseDtmcRegionModelCheckerTest, Crowds_Prob) {
    
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/crowds3_5.pm";
    std::string formulaAsString = "P<0.5 [F \"observe0Greater1\" ]";
    std::string constantsAsString = ""; //e.g. pL=0.9,TOACK=0.5
    carl::VariablePool::getInstance().clear();

    storm::prism::Program program = storm::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::extractFormulasFromProperties(storm::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    auto const& regionSettings = storm::settings::getModule<storm::settings::modules::RegionSettings>();
    storm::modelchecker::region::SparseRegionModelCheckerSettings settings(regionSettings.getSampleMode(), regionSettings.getApproxMode(), regionSettings.getSmtMode());
    auto dtmcModelchecker = std::make_shared<storm::modelchecker::region::SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>>(model, settings);
    dtmcModelchecker->specifyFormula(formulas[0]);
    //start testing
    auto allSatRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.1<=PF<=0.75,0.15<=badC<=0.2");
    auto exBothRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.75<=PF<=0.8,0.2<=badC<=0.3");
    auto allVioRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.8<=PF<=0.95,0.2<=badC<=0.2");
    auto allVioHardRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.8<=PF<=0.95,0.2<=badC<=0.9");

    EXPECT_TRUE(dtmcModelchecker->checkFormulaOnSamplingPoint(allSatRegion.getSomePoint()));
    EXPECT_FALSE(dtmcModelchecker->checkFormulaOnSamplingPoint(allVioHardRegion.getSomePoint()));
    
    EXPECT_NEAR(0.1734086422, dtmcModelchecker->getReachabilityValue(allSatRegion.getLowerBoundaries()), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.1734086422, storm::utility::convertNumber<double>(dtmcModelchecker->evaluateReachabilityFunction(allSatRegion.getLowerBoundaries())),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.47178, dtmcModelchecker->getReachabilityValue(allSatRegion.getUpperBoundaries()),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.47178, storm::utility::convertNumber<double>(dtmcModelchecker->evaluateReachabilityFunction(allSatRegion.getUpperBoundaries())),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.7085157883, dtmcModelchecker->getReachabilityValue(exBothRegion.getUpperBoundaries()),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.7085157883, storm::utility::convertNumber<double>(dtmcModelchecker->evaluateReachabilityFunction(exBothRegion.getUpperBoundaries())),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.5095205203, dtmcModelchecker->getReachabilityValue(allVioRegion.getLowerBoundaries()), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.5095205203, storm::utility::convertNumber<double>(dtmcModelchecker->evaluateReachabilityFunction(allVioRegion.getLowerBoundaries())),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.6819701472, dtmcModelchecker->getReachabilityValue(allVioRegion.getUpperBoundaries()), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.6819701472, storm::utility::convertNumber<double>(dtmcModelchecker->evaluateReachabilityFunction(allVioRegion.getUpperBoundaries())),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.999895897, dtmcModelchecker->getReachabilityValue(allVioHardRegion.getUpperBoundaries()),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.999895897, storm::utility::convertNumber<double>(dtmcModelchecker->evaluateReachabilityFunction(allVioHardRegion.getUpperBoundaries())),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    //test approximative method
    settings = storm::modelchecker::region::SparseRegionModelCheckerSettings(storm::settings::modules::RegionSettings::SampleMode::INSTANTIATE, storm::settings::modules::RegionSettings::ApproxMode::TESTFIRST,  storm::settings::modules::RegionSettings::SmtMode::OFF);
    dtmcModelchecker = std::make_shared<storm::modelchecker::region::SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>>(model, settings);
    dtmcModelchecker->specifyFormula(formulas[0]);
    ASSERT_TRUE(dtmcModelchecker->getSettings().doApprox());
    ASSERT_TRUE(dtmcModelchecker->getSettings().doSample());
    ASSERT_FALSE(dtmcModelchecker->getSettings().doSmt());
    dtmcModelchecker->checkRegion(allSatRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLSAT), allSatRegion.getCheckResult());
    dtmcModelchecker->checkRegion(exBothRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::EXISTSBOTH), exBothRegion.getCheckResult());
    dtmcModelchecker->checkRegion(allVioRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLVIOLATED), allVioRegion.getCheckResult());
    dtmcModelchecker->checkRegion(allVioHardRegion);
        //At this moment, Approximation should not be able to get a result for this region. (However, it is not wrong if it can)
    EXPECT_TRUE(
                (allVioHardRegion.getCheckResult()==(storm::modelchecker::region::RegionCheckResult::ALLVIOLATED)) ||
                (allVioHardRegion.getCheckResult()==(storm::modelchecker::region::RegionCheckResult::EXISTSVIOLATED))
            );

    //test smt method (the regions need to be created again, because the old ones have some information stored in their internal state)
    auto allSatRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.1<=PF<=0.75,0.15<=badC<=0.2");
    auto exBothRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.75<=PF<=0.8,0.2<=badC<=0.3");
    auto allVioRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.8<=PF<=0.95,0.2<=badC<=0.2");
    auto allVioHardRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.8<=PF<=0.95,0.2<=badC<=0.9");
    settings = storm::modelchecker::region::SparseRegionModelCheckerSettings(storm::settings::modules::RegionSettings::SampleMode::EVALUATE, storm::settings::modules::RegionSettings::ApproxMode::OFF,  storm::settings::modules::RegionSettings::SmtMode::FUNCTION);
    dtmcModelchecker = std::make_shared<storm::modelchecker::region::SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>>(model, settings);
    dtmcModelchecker->specifyFormula(formulas[0]);
    ASSERT_FALSE(dtmcModelchecker->getSettings().doApprox());
    ASSERT_TRUE(dtmcModelchecker->getSettings().doSample());
    ASSERT_TRUE(dtmcModelchecker->getSettings().doSmt());
    dtmcModelchecker->checkRegion(allSatRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLSAT), allSatRegionSmt.getCheckResult());
    dtmcModelchecker->checkRegion(exBothRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::EXISTSBOTH), exBothRegionSmt.getCheckResult());
    dtmcModelchecker->checkRegion(allVioRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLVIOLATED), allVioRegionSmt.getCheckResult());
    dtmcModelchecker->checkRegion(allVioHardRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLVIOLATED), allVioHardRegionSmt.getCheckResult());
    
    //test smt + approx
    auto allVioHardRegionSmtApp=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.8<=PF<=0.95,0.2<=badC<=0.9");
    settings = storm::modelchecker::region::SparseRegionModelCheckerSettings(storm::settings::modules::RegionSettings::SampleMode::EVALUATE, storm::settings::modules::RegionSettings::ApproxMode::TESTFIRST, storm::settings::modules::RegionSettings::SmtMode::FUNCTION);
    dtmcModelchecker = std::make_shared<storm::modelchecker::region::SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>>(model, settings);
    dtmcModelchecker->specifyFormula(formulas[0]);
    ASSERT_TRUE(dtmcModelchecker->getSettings().doApprox());
    ASSERT_TRUE(dtmcModelchecker->getSettings().doSample());
    ASSERT_TRUE(dtmcModelchecker->getSettings().doSmt());
    dtmcModelchecker->checkRegion(allVioHardRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLVIOLATED), allVioHardRegionSmtApp.getCheckResult());

    carl::VariablePool::getInstance().clear();
}

TEST(SparseDtmcRegionModelCheckerTest, Crowds_Prob_1Par) {
    
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/crowds3_5.pm";
    std::string formulaAsString = "P>0.75 [F \"observe0Greater1\" ]";
    std::string constantsAsString = "badC=0.3"; //e.g. pL=0.9,TOACK=0.5
    carl::VariablePool::getInstance().clear();

    
    storm::prism::Program program = storm::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::extractFormulasFromProperties(storm::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    auto const& regionSettings = storm::settings::getModule<storm::settings::modules::RegionSettings>();
    storm::modelchecker::region::SparseRegionModelCheckerSettings settings(regionSettings.getSampleMode(), regionSettings.getApproxMode(), regionSettings.getSmtMode());
    auto dtmcModelchecker = std::make_shared<storm::modelchecker::region::SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>>(model, settings);
    dtmcModelchecker->specifyFormula(formulas[0]);
    //start testing
    auto allSatRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.9<=PF<=0.99");
    auto exBothRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.8<=PF<=0.9");
    auto allVioRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.01<=PF<=0.8");

    EXPECT_TRUE(dtmcModelchecker->checkFormulaOnSamplingPoint(allSatRegion.getSomePoint()));
    EXPECT_FALSE(dtmcModelchecker->checkFormulaOnSamplingPoint(allVioRegion.getSomePoint()));
    
    EXPECT_NEAR(0.8430128158, dtmcModelchecker->getReachabilityValue(allSatRegion.getUpperBoundaries()),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.8430128158, storm::utility::convertNumber<double>(dtmcModelchecker->evaluateReachabilityFunction(allSatRegion.getUpperBoundaries())),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.7731321947, dtmcModelchecker->getReachabilityValue(exBothRegion.getUpperBoundaries()),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.7731321947, storm::utility::convertNumber<double>(dtmcModelchecker->evaluateReachabilityFunction(exBothRegion.getUpperBoundaries())),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.4732302663, dtmcModelchecker->getReachabilityValue(allVioRegion.getLowerBoundaries()), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.4732302663, storm::utility::convertNumber<double>(dtmcModelchecker->evaluateReachabilityFunction(allVioRegion.getLowerBoundaries())),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.7085157883, dtmcModelchecker->getReachabilityValue(allVioRegion.getUpperBoundaries()), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.7085157883, storm::utility::convertNumber<double>(dtmcModelchecker->evaluateReachabilityFunction(allVioRegion.getUpperBoundaries())),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    //test approximative method
    settings = storm::modelchecker::region::SparseRegionModelCheckerSettings(storm::settings::modules::RegionSettings::SampleMode::INSTANTIATE, storm::settings::modules::RegionSettings::ApproxMode::TESTFIRST,  storm::settings::modules::RegionSettings::SmtMode::OFF);
    dtmcModelchecker = std::make_shared<storm::modelchecker::region::SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>>(model, settings);
    dtmcModelchecker->specifyFormula(formulas[0]);
    ASSERT_TRUE(dtmcModelchecker->getSettings().doApprox());
    ASSERT_TRUE(dtmcModelchecker->getSettings().doSample());
    ASSERT_FALSE(dtmcModelchecker->getSettings().doSmt());
    dtmcModelchecker->checkRegion(allSatRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLSAT), allSatRegion.getCheckResult());
    dtmcModelchecker->checkRegion(exBothRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::EXISTSBOTH), exBothRegion.getCheckResult());
    dtmcModelchecker->checkRegion(allVioRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLVIOLATED), allVioRegion.getCheckResult());

    //test smt method (the regions need to be created again, because the old ones have some information stored in their internal state)
    auto allSatRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.9<=PF<=0.99");
    auto exBothRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.8<=PF<=0.9");
    auto allVioRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.01<=PF<=0.8");
    settings = storm::modelchecker::region::SparseRegionModelCheckerSettings(storm::settings::modules::RegionSettings::SampleMode::EVALUATE, storm::settings::modules::RegionSettings::ApproxMode::OFF,  storm::settings::modules::RegionSettings::SmtMode::FUNCTION);
    dtmcModelchecker = std::make_shared<storm::modelchecker::region::SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>>(model, settings);
    dtmcModelchecker->specifyFormula(formulas[0]);
    ASSERT_FALSE(dtmcModelchecker->getSettings().doApprox());
    ASSERT_TRUE(dtmcModelchecker->getSettings().doSample());
    ASSERT_TRUE(dtmcModelchecker->getSettings().doSmt());
    dtmcModelchecker->checkRegion(allSatRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLSAT), allSatRegionSmt.getCheckResult());
    dtmcModelchecker->checkRegion(exBothRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::EXISTSBOTH), exBothRegionSmt.getCheckResult());
    dtmcModelchecker->checkRegion(allVioRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLVIOLATED), allVioRegionSmt.getCheckResult());

    carl::VariablePool::getInstance().clear();
}

TEST(SparseDtmcRegionModelCheckerTest, Crowds_Prob_Const) {
    
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/crowds3_5.pm";
    std::string formulaAsString = "P>0.6 [F \"observe0Greater1\" ]";
    std::string constantsAsString = "PF=0.9,badC=0.2";
    carl::VariablePool::getInstance().clear();
    
    storm::prism::Program program = storm::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::extractFormulasFromProperties(storm::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    auto const& regionSettings = storm::settings::getModule<storm::settings::modules::RegionSettings>();
    storm::modelchecker::region::SparseRegionModelCheckerSettings settings(regionSettings.getSampleMode(), regionSettings.getApproxMode(), regionSettings.getSmtMode());
    auto dtmcModelchecker = std::make_shared<storm::modelchecker::region::SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>>(model, settings);
    dtmcModelchecker->specifyFormula(formulas[0]);
    //start testing
    auto allSatRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("");
    
    EXPECT_TRUE(dtmcModelchecker->checkFormulaOnSamplingPoint(allSatRegion.getSomePoint()));
    
    EXPECT_NEAR(0.6119660237, dtmcModelchecker->getReachabilityValue(allSatRegion.getUpperBoundaries()),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.6119660237, storm::utility::convertNumber<double>(dtmcModelchecker->evaluateReachabilityFunction(allSatRegion.getUpperBoundaries())),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.6119660237, dtmcModelchecker->getReachabilityValue(allSatRegion.getLowerBoundaries()),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.6119660237, storm::utility::convertNumber<double>(dtmcModelchecker->evaluateReachabilityFunction(allSatRegion.getLowerBoundaries())),  storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    //test approximative method
    settings = storm::modelchecker::region::SparseRegionModelCheckerSettings(storm::settings::modules::RegionSettings::SampleMode::INSTANTIATE, storm::settings::modules::RegionSettings::ApproxMode::TESTFIRST,  storm::settings::modules::RegionSettings::SmtMode::OFF);
    dtmcModelchecker = std::make_shared<storm::modelchecker::region::SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>>(model, settings);
    dtmcModelchecker->specifyFormula(formulas[0]);
    ASSERT_TRUE(dtmcModelchecker->getSettings().doApprox());
    ASSERT_TRUE(dtmcModelchecker->getSettings().doSample());
    ASSERT_FALSE(dtmcModelchecker->getSettings().doSmt());
    dtmcModelchecker->checkRegion(allSatRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLSAT), allSatRegion.getCheckResult());

    //test smt method (the regions need to be created again, because the old ones have some information stored in their internal state)
    auto allSatRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("");
    settings = storm::modelchecker::region::SparseRegionModelCheckerSettings(storm::settings::modules::RegionSettings::SampleMode::EVALUATE, storm::settings::modules::RegionSettings::ApproxMode::OFF,  storm::settings::modules::RegionSettings::SmtMode::FUNCTION);
    dtmcModelchecker = std::make_shared<storm::modelchecker::region::SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>>(model, settings);
    dtmcModelchecker->specifyFormula(formulas[0]);
    ASSERT_FALSE(dtmcModelchecker->getSettings().doApprox());
    ASSERT_TRUE(dtmcModelchecker->getSettings().doSample());
    ASSERT_TRUE(dtmcModelchecker->getSettings().doSmt());
    dtmcModelchecker->checkRegion(allSatRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLSAT), allSatRegionSmt.getCheckResult());

    carl::VariablePool::getInstance().clear();
}

#endif
