#include "gtest/gtest.h"
#include "storm-config.h"

#ifdef STORM_HAVE_CARL

#include "src/adapters/CarlAdapter.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/modules/GeneralSettings.h"
#include "src/settings/modules/RegionSettings.h"

#include "src/models/sparse/Dtmc.h"
#include "src/parser/PrismParser.h"
#include "src/parser/FormulaParser.h"
#include "src/logic/Formulas.h"
#include "src/models/ModelBase.h"
#include "src/models/sparse/Model.h"
#include "src/models/sparse/Dtmc.h"
#include "builder/ExplicitPrismModelBuilder.h"
#include "modelchecker/region/SparseDtmcRegionModelChecker.h"
#include "modelchecker/region/ParameterRegion.h"

TEST(SparseDtmcRegionModelCheckerTest, Brp_Prob) {
    
    std::string const& programFile = STORM_CPP_BASE_PATH "/examples/pdtmc/brp/brp_16_2.pm";
    std::string const& formulaAsString = "P<=0.85 [F \"target\" ]";
    std::string const& constantsAsString = ""; //e.g. pL=0.9,TOACK=0.5
    
    //Build model, formula, region model checker
    boost::optional<storm::prism::Program> program=storm::parser::PrismParser::parse(programFile).simplify().simplify();
    program->checkValidity();
    storm::parser::FormulaParser formulaParser(program.get().getManager().getSharedPointer());
    std::vector<std::shared_ptr<storm::logic::Formula>> formulas = formulaParser.parseFromString(formulaAsString);
    typename storm::builder::ExplicitPrismModelBuilder<storm::RationalFunction>::Options options=storm::builder::ExplicitPrismModelBuilder<storm::RationalFunction>::Options(*formulas[0]);
    options.addConstantDefinitionsFromString(program.get(), constantsAsString); 
    options.preserveFormula(*formulas[0]);
    std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> model = storm::builder::ExplicitPrismModelBuilder<storm::RationalFunction>().translateProgram(program.get(), options)->as<storm::models::sparse::Model<storm::RationalFunction>>();
    ASSERT_EQ(storm::models::ModelType::Dtmc, model->getType());
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->template as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    storm::modelchecker::region::SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double> modelchecker(*dtmc);
    ASSERT_TRUE(modelchecker.canHandle(*formulas[0]));
    modelchecker.specifyFormula(formulas[0]);
    
    //start testing
    auto allSatRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.7<=pL<=0.9,0.75<=pK<=0.95");
    auto exBothRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.4<=pL<=0.65,0.75<=pK<=0.95");
    auto allVioRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.1<=pL<=0.9,0.2<=pK<=0.5");

    EXPECT_NEAR(0.8369631407, modelchecker.getReachabilityValue(allSatRegion.getLowerBounds()), storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.8369631407, storm::utility::region::convertNumber<double>(modelchecker.evaluateReachabilityFunction(allSatRegion.getLowerBounds())),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.0476784174, modelchecker.getReachabilityValue(allSatRegion.getUpperBounds()),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.0476784174, storm::utility::region::convertNumber<double>(modelchecker.evaluateReachabilityFunction(allSatRegion.getUpperBounds())),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.9987948367, modelchecker.getReachabilityValue(exBothRegion.getLowerBounds()), storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.9987948367, storm::utility::region::convertNumber<double>(modelchecker.evaluateReachabilityFunction(exBothRegion.getLowerBounds())),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.6020480995, modelchecker.getReachabilityValue(exBothRegion.getUpperBounds()),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.6020480995, storm::utility::region::convertNumber<double>(modelchecker.evaluateReachabilityFunction(exBothRegion.getUpperBounds())),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(1.0000000000, modelchecker.getReachabilityValue(allVioRegion.getLowerBounds()), storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(1.0000000000, storm::utility::region::convertNumber<double>(modelchecker.evaluateReachabilityFunction(allVioRegion.getLowerBounds())),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.9456084185, modelchecker.getReachabilityValue(allVioRegion.getUpperBounds()),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.9456084185, storm::utility::region::convertNumber<double>(modelchecker.evaluateReachabilityFunction(allVioRegion.getUpperBounds())),  storm::settings::generalSettings().getPrecision());
    
    //test approximative method
    storm::settings::mutableRegionSettings().modifyModes(storm::settings::modules::RegionSettings::ApproxMode::TESTFIRST, storm::settings::modules::RegionSettings::SampleMode::INSTANTIATE, storm::settings::modules::RegionSettings::SmtMode::OFF);
    ASSERT_TRUE(storm::settings::regionSettings().doApprox());
    ASSERT_TRUE(storm::settings::regionSettings().doSample());
    ASSERT_FALSE(storm::settings::regionSettings().doSmt());
    modelchecker.checkRegion(allSatRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLSAT), allSatRegion.getCheckResult());
    modelchecker.checkRegion(exBothRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::EXISTSBOTH), exBothRegion.getCheckResult());
    modelchecker.checkRegion(allVioRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLVIOLATED), allVioRegion.getCheckResult());

    //test smt method (the regions need to be created again, because the old ones have some information stored in their internal state)
    auto allSatRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.7<=pL<=0.9,0.75<=pK<=0.95");
    auto exBothRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.4<=pL<=0.65,0.75<=pK<=0.95");
    auto allVioRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.1<=pL<=0.9,0.2<=pK<=0.5");
    storm::settings::mutableRegionSettings().modifyModes(storm::settings::modules::RegionSettings::ApproxMode::OFF, storm::settings::modules::RegionSettings::SampleMode::EVALUATE, storm::settings::modules::RegionSettings::SmtMode::FUNCTION);
    ASSERT_FALSE(storm::settings::regionSettings().doApprox());
    ASSERT_TRUE(storm::settings::regionSettings().doSample());
    ASSERT_TRUE(storm::settings::regionSettings().doSmt());
    modelchecker.checkRegion(allSatRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLSAT), allSatRegionSmt.getCheckResult());
    modelchecker.checkRegion(exBothRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::EXISTSBOTH), exBothRegionSmt.getCheckResult());
    modelchecker.checkRegion(allVioRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLVIOLATED), allVioRegionSmt.getCheckResult());
    
    storm::settings::mutableRegionSettings().resetModes();
}

TEST(SparseDtmcRegionModelCheckerTest, Brp_Rew) {
    
    std::string const& programFile = STORM_CPP_BASE_PATH "/examples/pdtmc/brp_rewards/brp_16_2.pm";
    std::string const& formulaAsString = "R>2.5 [F \"target\" ]";
    std::string const& constantsAsString = "pL=0.9,TOAck=0.5";
    
    //Build model, formula, region model checker
    boost::optional<storm::prism::Program> program=storm::parser::PrismParser::parse(programFile).simplify().simplify();
    program->checkValidity();
    storm::parser::FormulaParser formulaParser(program.get().getManager().getSharedPointer());
    std::vector<std::shared_ptr<storm::logic::Formula>> formulas = formulaParser.parseFromString(formulaAsString);
    typename storm::builder::ExplicitPrismModelBuilder<storm::RationalFunction>::Options options=storm::builder::ExplicitPrismModelBuilder<storm::RationalFunction>::Options(*formulas[0]);
    options.addConstantDefinitionsFromString(program.get(), constantsAsString); 
    options.preserveFormula(*formulas[0]);
    std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> model = storm::builder::ExplicitPrismModelBuilder<storm::RationalFunction>().translateProgram(program.get(), options)->as<storm::models::sparse::Model<storm::RationalFunction>>();
    ASSERT_EQ(storm::models::ModelType::Dtmc, model->getType());
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->template as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    storm::modelchecker::region::SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double> modelchecker(*dtmc);
    ASSERT_TRUE(modelchecker.canHandle(*formulas[0]));
    modelchecker.specifyFormula(formulas[0]);
    
    //start testing
    auto allSatRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.7<=pK<=0.875,0.75<=TOMsg<=0.95");
    auto exBothRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.6<=pK<=0.9,0.5<=TOMsg<=0.95");
    auto exBothHardRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.5<=pK<=0.75,0.3<=TOMsg<=0.4"); //this region has a local maximum!
    auto allVioRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.1<=pK<=0.3,0.2<=TOMsg<=0.3");

    EXPECT_NEAR(4.367791292, modelchecker.getReachabilityValue(allSatRegion.getLowerBounds()), storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(4.367791292, storm::utility::region::convertNumber<double>(modelchecker.evaluateReachabilityFunction(allSatRegion.getLowerBounds())),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(3.044795147, modelchecker.getReachabilityValue(allSatRegion.getUpperBounds()),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(3.044795147, storm::utility::region::convertNumber<double>(modelchecker.evaluateReachabilityFunction(allSatRegion.getUpperBounds())),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(3.182535759, modelchecker.getReachabilityValue(exBothRegion.getLowerBounds()), storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(3.182535759, storm::utility::region::convertNumber<double>(modelchecker.evaluateReachabilityFunction(exBothRegion.getLowerBounds())),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(2.609602197, modelchecker.getReachabilityValue(exBothRegion.getUpperBounds()),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(2.609602197, storm::utility::region::convertNumber<double>(modelchecker.evaluateReachabilityFunction(exBothRegion.getUpperBounds())),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(1.842551039, modelchecker.getReachabilityValue(exBothHardRegion.getLowerBounds()), storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(1.842551039, storm::utility::region::convertNumber<double>(modelchecker.evaluateReachabilityFunction(exBothHardRegion.getLowerBounds())),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(2.453500364, modelchecker.getReachabilityValue(exBothHardRegion.getUpperBounds()),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(2.453500364, storm::utility::region::convertNumber<double>(modelchecker.evaluateReachabilityFunction(exBothHardRegion.getUpperBounds())),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.6721974438, modelchecker.getReachabilityValue(allVioRegion.getLowerBounds()), storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.6721974438, storm::utility::region::convertNumber<double>(modelchecker.evaluateReachabilityFunction(allVioRegion.getLowerBounds())),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(1.308324558, modelchecker.getReachabilityValue(allVioRegion.getUpperBounds()),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(1.308324558, storm::utility::region::convertNumber<double>(modelchecker.evaluateReachabilityFunction(allVioRegion.getUpperBounds())),  storm::settings::generalSettings().getPrecision());
    
    //test approximative method
    storm::settings::mutableRegionSettings().modifyModes(storm::settings::modules::RegionSettings::ApproxMode::TESTFIRST, storm::settings::modules::RegionSettings::SampleMode::INSTANTIATE, storm::settings::modules::RegionSettings::SmtMode::OFF);
    ASSERT_TRUE(storm::settings::regionSettings().doApprox());
    ASSERT_TRUE(storm::settings::regionSettings().doSample());
    ASSERT_FALSE(storm::settings::regionSettings().doSmt());
    modelchecker.checkRegion(allSatRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLSAT), allSatRegion.getCheckResult());
    modelchecker.checkRegion(exBothRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::EXISTSBOTH), exBothRegion.getCheckResult());
    modelchecker.checkRegion(exBothHardRegion);
    //At this moment, Approximation should not be able to get a result for this region. (However, it is not wrong if it can)
    EXPECT_TRUE(
                (exBothHardRegion.getCheckResult()==(storm::modelchecker::region::RegionCheckResult::EXISTSBOTH)) ||
                (exBothHardRegion.getCheckResult()==(storm::modelchecker::region::RegionCheckResult::EXISTSVIOLATED))
            );
    modelchecker.checkRegion(allVioRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLVIOLATED), allVioRegion.getCheckResult());

    //test smt method (the regions need to be created again, because the old ones have some information stored in their internal state)
    auto allSatRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.7<=pK<=0.9,0.75<=TOMsg<=0.95");
    auto exBothRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.3<=pK<=0.5,0.5<=TOMsg<=0.75");
    auto exBothHardRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.5<=pK<=0.75,0.3<=TOMsg<=0.4"); //this region has a local maximum!
    auto allVioRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.1<=pK<=0.3,0.2<=TOMsg<=0.3");
    storm::settings::mutableRegionSettings().modifyModes(storm::settings::modules::RegionSettings::ApproxMode::OFF, storm::settings::modules::RegionSettings::SampleMode::EVALUATE, storm::settings::modules::RegionSettings::SmtMode::FUNCTION);
    ASSERT_FALSE(storm::settings::regionSettings().doApprox());
    ASSERT_TRUE(storm::settings::regionSettings().doSample());
    ASSERT_TRUE(storm::settings::regionSettings().doSmt());
    modelchecker.checkRegion(allSatRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLSAT), allSatRegionSmt.getCheckResult());
    modelchecker.checkRegion(exBothRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::EXISTSBOTH), exBothRegionSmt.getCheckResult());
    modelchecker.checkRegion(exBothHardRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::EXISTSBOTH), exBothHardRegionSmt.getCheckResult());
    modelchecker.checkRegion(allVioRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLVIOLATED), allVioRegionSmt.getCheckResult());
    
    //test smt + approx
    auto exBothHardRegionSmtApp=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.5<=pK<=0.75,0.3<=TOMsg<=0.4"); //this region has a local maximum!
    storm::settings::mutableRegionSettings().modifyModes(storm::settings::modules::RegionSettings::ApproxMode::TESTFIRST, storm::settings::modules::RegionSettings::SampleMode::EVALUATE, storm::settings::modules::RegionSettings::SmtMode::FUNCTION);
    ASSERT_TRUE(storm::settings::regionSettings().doApprox());
    ASSERT_TRUE(storm::settings::regionSettings().doSample());
    ASSERT_TRUE(storm::settings::regionSettings().doSmt());
    modelchecker.checkRegion(exBothHardRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::EXISTSBOTH), exBothHardRegionSmtApp.getCheckResult());
    
    
    storm::settings::mutableRegionSettings().resetModes();
}

TEST(SparseDtmcRegionModelCheckerTest, Brp_Rew_Infty) {
    
    std::string const& programFile = STORM_CPP_BASE_PATH "/examples/pdtmc/brp_rewards/brp_16_2.pm";
    std::string const& formulaAsString = "R>2.5 [F \"success\" ]";
    std::string const& constantsAsString = "";
    
    //Build model, formula, region model checker
    boost::optional<storm::prism::Program> program=storm::parser::PrismParser::parse(programFile).simplify().simplify();
    program->checkValidity();
    storm::parser::FormulaParser formulaParser(program.get().getManager().getSharedPointer());
    std::vector<std::shared_ptr<storm::logic::Formula>> formulas = formulaParser.parseFromString(formulaAsString);
    typename storm::builder::ExplicitPrismModelBuilder<storm::RationalFunction>::Options options=storm::builder::ExplicitPrismModelBuilder<storm::RationalFunction>::Options(*formulas[0]);
    options.addConstantDefinitionsFromString(program.get(), constantsAsString); 
    options.preserveFormula(*formulas[0]);
    std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> model = storm::builder::ExplicitPrismModelBuilder<storm::RationalFunction>().translateProgram(program.get(), options)->as<storm::models::sparse::Model<storm::RationalFunction>>();
    ASSERT_EQ(storm::models::ModelType::Dtmc, model->getType());
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->template as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    storm::modelchecker::region::SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double> modelchecker(*dtmc);
    ASSERT_TRUE(modelchecker.canHandle(*formulas[0]));
    modelchecker.specifyFormula(formulas[0]);
    
    //start testing
    auto allSatRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("");

    EXPECT_EQ(storm::utility::infinity<double>(), modelchecker.getReachabilityValue(allSatRegion.getLowerBounds()));
    
    //test approximative method
    storm::settings::mutableRegionSettings().modifyModes(storm::settings::modules::RegionSettings::ApproxMode::TESTFIRST, storm::settings::modules::RegionSettings::SampleMode::INSTANTIATE, storm::settings::modules::RegionSettings::SmtMode::OFF);
    ASSERT_TRUE(storm::settings::regionSettings().doApprox());
    ASSERT_TRUE(storm::settings::regionSettings().doSample());
    ASSERT_FALSE(storm::settings::regionSettings().doSmt());
    modelchecker.checkRegion(allSatRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLSAT), allSatRegion.getCheckResult());

    //test smt method (the regions need to be created again, because the old ones have some information stored in their internal state)
    auto allSatRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("");
    storm::settings::mutableRegionSettings().modifyModes(storm::settings::modules::RegionSettings::ApproxMode::OFF, storm::settings::modules::RegionSettings::SampleMode::EVALUATE, storm::settings::modules::RegionSettings::SmtMode::FUNCTION);
    ASSERT_FALSE(storm::settings::regionSettings().doApprox());
    ASSERT_TRUE(storm::settings::regionSettings().doSample());
    ASSERT_TRUE(storm::settings::regionSettings().doSmt());
    modelchecker.checkRegion(allSatRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLSAT), allSatRegionSmt.getCheckResult());
    
    storm::settings::mutableRegionSettings().resetModes();
}

TEST(SparseDtmcRegionModelCheckerTest, Brp_Rew_4Par) {
    
    std::string const& programFile = STORM_CPP_BASE_PATH "/examples/pdtmc/brp_rewards/brp_16_2.pm";
    std::string const& formulaAsString = "R>2.5 [F \"target\" ]";
    std::string const& constantsAsString = ""; //!! this model will have 4 parameters
    
    //Build model, formula, region model checker
    boost::optional<storm::prism::Program> program=storm::parser::PrismParser::parse(programFile).simplify().simplify();
    program->checkValidity();
    storm::parser::FormulaParser formulaParser(program.get().getManager().getSharedPointer());
    std::vector<std::shared_ptr<storm::logic::Formula>> formulas = formulaParser.parseFromString(formulaAsString);
    typename storm::builder::ExplicitPrismModelBuilder<storm::RationalFunction>::Options options=storm::builder::ExplicitPrismModelBuilder<storm::RationalFunction>::Options(*formulas[0]);
    options.addConstantDefinitionsFromString(program.get(), constantsAsString); 
    options.preserveFormula(*formulas[0]);
    std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> model = storm::builder::ExplicitPrismModelBuilder<storm::RationalFunction>().translateProgram(program.get(), options)->as<storm::models::sparse::Model<storm::RationalFunction>>();
    ASSERT_EQ(storm::models::ModelType::Dtmc, model->getType());
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->template as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    storm::modelchecker::region::SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double> modelchecker(*dtmc);
    ASSERT_TRUE(modelchecker.canHandle(*formulas[0]));
    modelchecker.specifyFormula(formulas[0]);
    
    //start testing
    auto allSatRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.7<=pK<=0.9,0.6<=pL<=0.85,0.9<=TOMsg<=0.95,0.85<=TOAck<=0.9");
    auto exBothRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.1<=pK<=0.7,0.2<=pL<=0.8,0.15<=TOMsg<=0.65,0.3<=TOAck<=0.9");
    auto allVioRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.1<=pK<=0.4,0.2<=pL<=0.3,0.15<=TOMsg<=0.3,0.1<=TOAck<=0.2");

    EXPECT_NEAR(4.834779705, modelchecker.getReachabilityValue(allSatRegion.getLowerBounds()), storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(4.834779705, storm::utility::region::convertNumber<double>(modelchecker.evaluateReachabilityFunction(allSatRegion.getLowerBounds())),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(4.674651623, modelchecker.getReachabilityValue(exBothRegion.getUpperBounds()),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(4.674651623, storm::utility::region::convertNumber<double>(modelchecker.evaluateReachabilityFunction(exBothRegion.getUpperBounds())),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.4467496536, modelchecker.getReachabilityValue(allVioRegion.getLowerBounds()), storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.4467496536, storm::utility::region::convertNumber<double>(modelchecker.evaluateReachabilityFunction(allVioRegion.getLowerBounds())),  storm::settings::generalSettings().getPrecision());
    
    //test approximative method
    storm::settings::mutableRegionSettings().modifyModes(storm::settings::modules::RegionSettings::ApproxMode::TESTFIRST, storm::settings::modules::RegionSettings::SampleMode::INSTANTIATE, storm::settings::modules::RegionSettings::SmtMode::OFF);
    ASSERT_TRUE(storm::settings::regionSettings().doApprox());
    ASSERT_TRUE(storm::settings::regionSettings().doSample());
    ASSERT_FALSE(storm::settings::regionSettings().doSmt());
    modelchecker.checkRegion(allSatRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLSAT), allSatRegion.getCheckResult());
    modelchecker.checkRegion(exBothRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::EXISTSBOTH), exBothRegion.getCheckResult());
    modelchecker.checkRegion(allVioRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLVIOLATED), allVioRegion.getCheckResult());

    //test smt method (the regions need to be created again, because the old ones have some information stored in their internal state)
    auto allSatRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.7<=pK<=0.9,0.6<=pL<=0.85,0.9<=TOMsg<=0.95,0.85<=TOAck<=0.9");
    auto exBothRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.1<=pK<=0.7,0.2<=pL<=0.8,0.15<=TOMsg<=0.65,0.3<=TOAck<=0.9");
    auto allVioRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.1<=pK<=0.4,0.2<=pL<=0.3,0.15<=TOMsg<=0.3,0.1<=TOAck<=0.2");
    storm::settings::mutableRegionSettings().modifyModes(storm::settings::modules::RegionSettings::ApproxMode::OFF, storm::settings::modules::RegionSettings::SampleMode::EVALUATE, storm::settings::modules::RegionSettings::SmtMode::FUNCTION);
    ASSERT_FALSE(storm::settings::regionSettings().doApprox());
    ASSERT_TRUE(storm::settings::regionSettings().doSample());
    ASSERT_TRUE(storm::settings::regionSettings().doSmt());
    modelchecker.checkRegion(allSatRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLSAT), allSatRegionSmt.getCheckResult());
    modelchecker.checkRegion(exBothRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::EXISTSBOTH), exBothRegionSmt.getCheckResult());
    modelchecker.checkRegion(allVioRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLVIOLATED), allVioRegionSmt.getCheckResult());
    
    storm::settings::mutableRegionSettings().resetModes();
}

TEST(SparseDtmcRegionModelCheckerTest, Crowds_Prob) {
    
    std::string const& programFile = STORM_CPP_BASE_PATH "/examples/pdtmc/crowds/crowds_3-5.pm";
    std::string const& formulaAsString = "P<0.5 [F \"observe0Greater1\" ]";
    std::string const& constantsAsString = ""; //e.g. pL=0.9,TOACK=0.5
    
    //Build model, formula, region model checker
    boost::optional<storm::prism::Program> program=storm::parser::PrismParser::parse(programFile).simplify().simplify();
    program->checkValidity();
    storm::parser::FormulaParser formulaParser(program.get().getManager().getSharedPointer());
    std::vector<std::shared_ptr<storm::logic::Formula>> formulas = formulaParser.parseFromString(formulaAsString);
    typename storm::builder::ExplicitPrismModelBuilder<storm::RationalFunction>::Options options=storm::builder::ExplicitPrismModelBuilder<storm::RationalFunction>::Options(*formulas[0]);
    options.addConstantDefinitionsFromString(program.get(), constantsAsString); 
    options.preserveFormula(*formulas[0]);
    std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> model = storm::builder::ExplicitPrismModelBuilder<storm::RationalFunction>().translateProgram(program.get(), options)->as<storm::models::sparse::Model<storm::RationalFunction>>();
    ASSERT_EQ(storm::models::ModelType::Dtmc, model->getType());
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->template as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    storm::modelchecker::region::SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double> modelchecker(*dtmc);
    ASSERT_TRUE(modelchecker.canHandle(*formulas[0]));
    modelchecker.specifyFormula(formulas[0]);
    
    //start testing
    auto allSatRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.1<=PF<=0.75,0.15<=badC<=0.2");
    auto exBothRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.75<=PF<=0.8,0.2<=badC<=0.3");
    auto allVioRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.8<=PF<=0.95,0.2<=badC<=0.2");
    auto allVioHardRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.8<=PF<=0.95,0.2<=badC<=0.9");

    EXPECT_NEAR(0.1734086422, modelchecker.getReachabilityValue(allSatRegion.getLowerBounds()), storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.1734086422, storm::utility::region::convertNumber<double>(modelchecker.evaluateReachabilityFunction(allSatRegion.getLowerBounds())),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.47178, modelchecker.getReachabilityValue(allSatRegion.getUpperBounds()),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.47178, storm::utility::region::convertNumber<double>(modelchecker.evaluateReachabilityFunction(allSatRegion.getUpperBounds())),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.7085157883, modelchecker.getReachabilityValue(exBothRegion.getUpperBounds()),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.7085157883, storm::utility::region::convertNumber<double>(modelchecker.evaluateReachabilityFunction(exBothRegion.getUpperBounds())),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.5095205203, modelchecker.getReachabilityValue(allVioRegion.getLowerBounds()), storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.5095205203, storm::utility::region::convertNumber<double>(modelchecker.evaluateReachabilityFunction(allVioRegion.getLowerBounds())),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.6819701472, modelchecker.getReachabilityValue(allVioRegion.getUpperBounds()), storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.6819701472, storm::utility::region::convertNumber<double>(modelchecker.evaluateReachabilityFunction(allVioRegion.getUpperBounds())),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.999895897, modelchecker.getReachabilityValue(allVioHardRegion.getUpperBounds()),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.999895897, storm::utility::region::convertNumber<double>(modelchecker.evaluateReachabilityFunction(allVioHardRegion.getUpperBounds())),  storm::settings::generalSettings().getPrecision());
    
    //test approximative method
    storm::settings::mutableRegionSettings().modifyModes(storm::settings::modules::RegionSettings::ApproxMode::TESTFIRST, storm::settings::modules::RegionSettings::SampleMode::INSTANTIATE, storm::settings::modules::RegionSettings::SmtMode::OFF);
    ASSERT_TRUE(storm::settings::regionSettings().doApprox());
    ASSERT_TRUE(storm::settings::regionSettings().doSample());
    ASSERT_FALSE(storm::settings::regionSettings().doSmt());
    modelchecker.checkRegion(allSatRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLSAT), allSatRegion.getCheckResult());
    modelchecker.checkRegion(exBothRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::EXISTSBOTH), exBothRegion.getCheckResult());
    modelchecker.checkRegion(allVioRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLVIOLATED), allVioRegion.getCheckResult());
    modelchecker.checkRegion(allVioHardRegion);
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
    storm::settings::mutableRegionSettings().modifyModes(storm::settings::modules::RegionSettings::ApproxMode::OFF, storm::settings::modules::RegionSettings::SampleMode::EVALUATE, storm::settings::modules::RegionSettings::SmtMode::FUNCTION);
    ASSERT_FALSE(storm::settings::regionSettings().doApprox());
    ASSERT_TRUE(storm::settings::regionSettings().doSample());
    ASSERT_TRUE(storm::settings::regionSettings().doSmt());
    modelchecker.checkRegion(allSatRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLSAT), allSatRegionSmt.getCheckResult());
    modelchecker.checkRegion(exBothRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::EXISTSBOTH), exBothRegionSmt.getCheckResult());
    modelchecker.checkRegion(allVioRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLVIOLATED), allVioRegionSmt.getCheckResult());
    modelchecker.checkRegion(allVioHardRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLVIOLATED), allVioHardRegionSmt.getCheckResult());
    
    //test smt + approx
    auto allVioHardRegionSmtApp=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.8<=PF<=0.95,0.2<=badC<=0.9");
    storm::settings::mutableRegionSettings().modifyModes(storm::settings::modules::RegionSettings::ApproxMode::TESTFIRST, storm::settings::modules::RegionSettings::SampleMode::EVALUATE, storm::settings::modules::RegionSettings::SmtMode::FUNCTION);
    ASSERT_TRUE(storm::settings::regionSettings().doApprox());
    ASSERT_TRUE(storm::settings::regionSettings().doSample());
    ASSERT_TRUE(storm::settings::regionSettings().doSmt());
    modelchecker.checkRegion(allVioHardRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLVIOLATED), allVioHardRegionSmtApp.getCheckResult());
    
    storm::settings::mutableRegionSettings().resetModes();
}

TEST(SparseDtmcRegionModelCheckerTest, Crowds_Prob_1Par) {
    
    std::string const& programFile = STORM_CPP_BASE_PATH "/examples/pdtmc/crowds/crowds_3-5.pm";
    std::string const& formulaAsString = "P>0.75 [F \"observe0Greater1\" ]";
    std::string const& constantsAsString = "badC=0.3"; //e.g. pL=0.9,TOACK=0.5
    
    //Build model, formula, region model checker
    boost::optional<storm::prism::Program> program=storm::parser::PrismParser::parse(programFile).simplify().simplify();
    program->checkValidity();
    storm::parser::FormulaParser formulaParser(program.get().getManager().getSharedPointer());
    std::vector<std::shared_ptr<storm::logic::Formula>> formulas = formulaParser.parseFromString(formulaAsString);
    typename storm::builder::ExplicitPrismModelBuilder<storm::RationalFunction>::Options options=storm::builder::ExplicitPrismModelBuilder<storm::RationalFunction>::Options(*formulas[0]);
    options.addConstantDefinitionsFromString(program.get(), constantsAsString); 
    options.preserveFormula(*formulas[0]);
    std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> model = storm::builder::ExplicitPrismModelBuilder<storm::RationalFunction>().translateProgram(program.get(), options)->as<storm::models::sparse::Model<storm::RationalFunction>>();
    ASSERT_EQ(storm::models::ModelType::Dtmc, model->getType());
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->template as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    storm::modelchecker::region::SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double> modelchecker(*dtmc);
    ASSERT_TRUE(modelchecker.canHandle(*formulas[0]));
    modelchecker.specifyFormula(formulas[0]);
    
    //start testing
    auto allSatRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.9<=PF<=0.99");
    auto exBothRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.8<=PF<=0.9");
    auto allVioRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.01<=PF<=0.8");

    EXPECT_NEAR(0.8430128158, modelchecker.getReachabilityValue(allSatRegion.getUpperBounds()),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.8430128158, storm::utility::region::convertNumber<double>(modelchecker.evaluateReachabilityFunction(allSatRegion.getUpperBounds())),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.7731321947, modelchecker.getReachabilityValue(exBothRegion.getUpperBounds()),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.7731321947, storm::utility::region::convertNumber<double>(modelchecker.evaluateReachabilityFunction(exBothRegion.getUpperBounds())),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.4732302663, modelchecker.getReachabilityValue(allVioRegion.getLowerBounds()), storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.4732302663, storm::utility::region::convertNumber<double>(modelchecker.evaluateReachabilityFunction(allVioRegion.getLowerBounds())),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.7085157883, modelchecker.getReachabilityValue(allVioRegion.getUpperBounds()), storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.7085157883, storm::utility::region::convertNumber<double>(modelchecker.evaluateReachabilityFunction(allVioRegion.getUpperBounds())),  storm::settings::generalSettings().getPrecision());
    
    //test approximative method
    storm::settings::mutableRegionSettings().modifyModes(storm::settings::modules::RegionSettings::ApproxMode::TESTFIRST, storm::settings::modules::RegionSettings::SampleMode::INSTANTIATE, storm::settings::modules::RegionSettings::SmtMode::OFF);
    ASSERT_TRUE(storm::settings::regionSettings().doApprox());
    ASSERT_TRUE(storm::settings::regionSettings().doSample());
    ASSERT_FALSE(storm::settings::regionSettings().doSmt());
    modelchecker.checkRegion(allSatRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLSAT), allSatRegion.getCheckResult());
    modelchecker.checkRegion(exBothRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::EXISTSBOTH), exBothRegion.getCheckResult());
    modelchecker.checkRegion(allVioRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLVIOLATED), allVioRegion.getCheckResult());

    //test smt method (the regions need to be created again, because the old ones have some information stored in their internal state)
    auto allSatRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.9<=PF<=0.99");
    auto exBothRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.8<=PF<=0.9");
    auto allVioRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("0.01<=PF<=0.8");
    storm::settings::mutableRegionSettings().modifyModes(storm::settings::modules::RegionSettings::ApproxMode::OFF, storm::settings::modules::RegionSettings::SampleMode::EVALUATE, storm::settings::modules::RegionSettings::SmtMode::FUNCTION);
    ASSERT_FALSE(storm::settings::regionSettings().doApprox());
    ASSERT_TRUE(storm::settings::regionSettings().doSample());
    ASSERT_TRUE(storm::settings::regionSettings().doSmt());
    modelchecker.checkRegion(allSatRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLSAT), allSatRegionSmt.getCheckResult());
    modelchecker.checkRegion(exBothRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::EXISTSBOTH), exBothRegionSmt.getCheckResult());
    modelchecker.checkRegion(allVioRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLVIOLATED), allVioRegionSmt.getCheckResult());
    
    storm::settings::mutableRegionSettings().resetModes();
}

TEST(SparseDtmcRegionModelCheckerTest, Crowds_Prob_Const) {
    
    std::string const& programFile = STORM_CPP_BASE_PATH "/examples/pdtmc/crowds/crowds_3-5.pm";
    std::string const& formulaAsString = "P>0.6 [F \"observe0Greater1\" ]";
    std::string const& constantsAsString = "PF=0.9,badC=0.2";
    
    //Build model, formula, region model checker
    boost::optional<storm::prism::Program> program=storm::parser::PrismParser::parse(programFile).simplify().simplify();
    program->checkValidity();
    storm::parser::FormulaParser formulaParser(program.get().getManager().getSharedPointer());
    std::vector<std::shared_ptr<storm::logic::Formula>> formulas = formulaParser.parseFromString(formulaAsString);
    typename storm::builder::ExplicitPrismModelBuilder<storm::RationalFunction>::Options options=storm::builder::ExplicitPrismModelBuilder<storm::RationalFunction>::Options(*formulas[0]);
    options.addConstantDefinitionsFromString(program.get(), constantsAsString); 
    options.preserveFormula(*formulas[0]);
    std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> model = storm::builder::ExplicitPrismModelBuilder<storm::RationalFunction>().translateProgram(program.get(), options)->as<storm::models::sparse::Model<storm::RationalFunction>>();
    ASSERT_EQ(storm::models::ModelType::Dtmc, model->getType());
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->template as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    storm::modelchecker::region::SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double> modelchecker(*dtmc);
    ASSERT_TRUE(modelchecker.canHandle(*formulas[0]));
    modelchecker.specifyFormula(formulas[0]);
    //start testing
    auto allSatRegion=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("");
    
    EXPECT_NEAR(0.6119660237, modelchecker.getReachabilityValue(allSatRegion.getUpperBounds()),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.6119660237, storm::utility::region::convertNumber<double>(modelchecker.evaluateReachabilityFunction(allSatRegion.getUpperBounds())),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.6119660237, modelchecker.getReachabilityValue(allSatRegion.getLowerBounds()),  storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.6119660237, storm::utility::region::convertNumber<double>(modelchecker.evaluateReachabilityFunction(allSatRegion.getLowerBounds())),  storm::settings::generalSettings().getPrecision());
    
    //test approximative method
    storm::settings::mutableRegionSettings().modifyModes(storm::settings::modules::RegionSettings::ApproxMode::TESTFIRST, storm::settings::modules::RegionSettings::SampleMode::INSTANTIATE, storm::settings::modules::RegionSettings::SmtMode::OFF);
    ASSERT_TRUE(storm::settings::regionSettings().doApprox());
    ASSERT_TRUE(storm::settings::regionSettings().doSample());
    ASSERT_FALSE(storm::settings::regionSettings().doSmt());
    modelchecker.checkRegion(allSatRegion);
    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLSAT), allSatRegion.getCheckResult());

    //test smt method (the regions need to be created again, because the old ones have some information stored in their internal state)
    auto allSatRegionSmt=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::parseRegion("");
    storm::settings::mutableRegionSettings().modifyModes(storm::settings::modules::RegionSettings::ApproxMode::OFF, storm::settings::modules::RegionSettings::SampleMode::EVALUATE, storm::settings::modules::RegionSettings::SmtMode::FUNCTION);
    ASSERT_FALSE(storm::settings::regionSettings().doApprox());
    ASSERT_TRUE(storm::settings::regionSettings().doSample());
    ASSERT_TRUE(storm::settings::regionSettings().doSmt());
    modelchecker.checkRegion(allSatRegionSmt);
//smt    EXPECT_EQ((storm::modelchecker::region::RegionCheckResult::ALLSAT), allSatRegionSmt.getCheckResult());

    storm::settings::mutableRegionSettings().resetModes();
}

#endif