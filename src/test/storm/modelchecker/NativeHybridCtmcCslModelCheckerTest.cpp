#include "gtest/gtest.h"
#include "storm-config.h"
#include "storm/settings/SettingMemento.h"
#include "storm/parser/PrismParser.h"
#include "storm/parser/FormulaParser.h"
#include "storm/logic/Formulas.h"
#include "storm/builder/DdPrismModelBuilder.h"
#include "storm/storage/dd/DdType.h"
#include "storm/storage/SymbolicModelDescription.h"

#include "storm/solver/NativeLinearEquationSolver.h"
#include "storm/models/symbolic/StandardRewardModel.h"
#include "storm/modelchecker/csl/HybridCtmcCslModelChecker.h"
#include "storm/modelchecker/results/HybridQuantitativeCheckResult.h"
#include "storm/modelchecker/results/SymbolicQualitativeCheckResult.h"
#include "storm/modelchecker/results/SymbolicQuantitativeCheckResult.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/modules/IOSettings.h"

#include "storm/settings/modules/NativeEquationSolverSettings.h"

TEST(NativeHybridCtmcCslModelCheckerTest, Cluster_Cudd) {
    // Set the PRISM compatibility mode temporarily. It is set to its old value once the returned object is destructed.
    std::unique_ptr<storm::settings::SettingMemento> enablePrismCompatibility = storm::settings::mutableIOSettings().overridePrismCompatibilityMode(true);
    
    // Parse the model description.
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/ctmc/cluster2.sm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    storm::parser::FormulaParser formulaParser(program);
    std::shared_ptr<storm::logic::Formula const> formula(nullptr);
    
    // Build the model.
#ifdef WINDOWS
	storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
#else
	typename storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
#endif
    options.buildAllRewardModels = false;
    options.rewardModelsToBuild.insert("num_repairs");
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program, options);
    ASSERT_EQ(storm::models::ModelType::Ctmc, model->getType());
    std::shared_ptr<storm::models::symbolic::Ctmc<storm::dd::DdType::CUDD>> ctmc = model->as<storm::models::symbolic::Ctmc<storm::dd::DdType::CUDD>>();
    
    // Create model checker.
    storm::modelchecker::HybridCtmcCslModelChecker<storm::models::symbolic::Ctmc<storm::dd::DdType::CUDD, double>> modelchecker(*ctmc, std::make_unique<storm::solver::NativeLinearEquationSolverFactory<double>>());
    
    // Start checking properties.
    formula = formulaParser.parseSingleFormulaFromString("P=? [ F<=100 !\"minimum\"]");
    std::unique_ptr<storm::modelchecker::CheckResult> checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD> quantitativeCheckResult1 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    EXPECT_NEAR(5.5461254704419085E-5, quantitativeCheckResult1.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(5.5461254704419085E-5, quantitativeCheckResult1.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ F[100,100] !\"minimum\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>  quantitativeCheckResult2 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    EXPECT_NEAR(2.3397873548343415E-6, quantitativeCheckResult2.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(2.3397873548343415E-6, quantitativeCheckResult2.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ F[100,2000] !\"minimum\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>  quantitativeCheckResult3 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    EXPECT_NEAR(0.001105335651670241, quantitativeCheckResult3.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.001105335651670241, quantitativeCheckResult3.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ \"minimum\" U<=10 \"premium\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>  quantitativeCheckResult4 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    EXPECT_NEAR(1, quantitativeCheckResult4.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(1, quantitativeCheckResult4.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ !\"minimum\" U>=1 \"minimum\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>  quantitativeCheckResult5 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    EXPECT_NEAR(0, quantitativeCheckResult5.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0, quantitativeCheckResult5.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ \"minimum\" U>=1 !\"minimum\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>  quantitativeCheckResult6 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    EXPECT_NEAR(0.9999999033633374, quantitativeCheckResult6.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.9999999033633374, quantitativeCheckResult6.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("R=? [C<=100]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>  quantitativeCheckResult7 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    EXPECT_NEAR(0.8602815057967503, quantitativeCheckResult7.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.8602815057967503, quantitativeCheckResult7.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("LRA=? [\"minimum\"]");
    checkResult = modelchecker.check(*formula);
    
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD> quantitativeCheckResult8 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    EXPECT_NEAR(0.99979112284455396, quantitativeCheckResult8.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.99979112284455396, quantitativeCheckResult8.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
}

TEST(NativeHybridCtmcCslModelCheckerTest, Cluster_Sylvan) {
    // Set the PRISM compatibility mode temporarily. It is set to its old value once the returned object is destructed.
    std::unique_ptr<storm::settings::SettingMemento> enablePrismCompatibility = storm::settings::mutableIOSettings().overridePrismCompatibilityMode(true);
    
    // Parse the model description.
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/ctmc/cluster2.sm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    storm::parser::FormulaParser formulaParser(program);
    std::shared_ptr<storm::logic::Formula const> formula(nullptr);
    
    // Build the model.
#ifdef WINDOWS
    storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>::Options options;
#else
    typename storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>::Options options;
#endif
    options.buildAllRewardModels = false;
    options.rewardModelsToBuild.insert("num_repairs");
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program, options);
    ASSERT_EQ(storm::models::ModelType::Ctmc, model->getType());
    std::shared_ptr<storm::models::symbolic::Ctmc<storm::dd::DdType::Sylvan>> ctmc = model->as<storm::models::symbolic::Ctmc<storm::dd::DdType::Sylvan>>();
    
    // Create model checker.
    storm::modelchecker::HybridCtmcCslModelChecker<storm::models::symbolic::Ctmc<storm::dd::DdType::Sylvan, double>> modelchecker(*ctmc, std::make_unique<storm::solver::NativeLinearEquationSolverFactory<double>>());
    
    // Start checking properties.
    formula = formulaParser.parseSingleFormulaFromString("P=? [ F<=100 !\"minimum\"]");
    std::unique_ptr<storm::modelchecker::CheckResult> checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan> quantitativeCheckResult1 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    EXPECT_NEAR(5.5461254704419085E-5, quantitativeCheckResult1.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(5.5461254704419085E-5, quantitativeCheckResult1.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ F[100,100] !\"minimum\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan>  quantitativeCheckResult2 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    EXPECT_NEAR(2.3397873548343415E-6, quantitativeCheckResult2.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(2.3397873548343415E-6, quantitativeCheckResult2.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ F[100,2000] !\"minimum\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan>  quantitativeCheckResult3 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    EXPECT_NEAR(0.001105335651670241, quantitativeCheckResult3.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.001105335651670241, quantitativeCheckResult3.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ \"minimum\" U<=10 \"premium\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan>  quantitativeCheckResult4 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    EXPECT_NEAR(1, quantitativeCheckResult4.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(1, quantitativeCheckResult4.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ !\"minimum\" U>=1 \"minimum\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan>  quantitativeCheckResult5 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    EXPECT_NEAR(0, quantitativeCheckResult5.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0, quantitativeCheckResult5.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ \"minimum\" U>=1 !\"minimum\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan>  quantitativeCheckResult6 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    EXPECT_NEAR(0.9999999033633374, quantitativeCheckResult6.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.9999999033633374, quantitativeCheckResult6.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("R=? [C<=100]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan>  quantitativeCheckResult7 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    EXPECT_NEAR(0.8602815057967503, quantitativeCheckResult7.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.8602815057967503, quantitativeCheckResult7.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("LRA=? [\"minimum\"]");
    checkResult = modelchecker.check(*formula);
    
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan> quantitativeCheckResult8 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    EXPECT_NEAR(0.99979112284455396, quantitativeCheckResult8.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.99979112284455396, quantitativeCheckResult8.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
}

TEST(NativeHybridCtmcCslModelCheckerTest, Embedded_Cudd) {
    // Set the PRISM compatibility mode temporarily. It is set to its old value once the returned object is destructed.
    std::unique_ptr<storm::settings::SettingMemento> enablePrismCompatibility = storm::settings::mutableIOSettings().overridePrismCompatibilityMode(true);
    
    // Parse the model description.
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/ctmc/embedded2.sm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    storm::parser::FormulaParser formulaParser(program);
    std::shared_ptr<storm::logic::Formula const> formula(nullptr);
    
    // Build the model.
#ifdef WINDOWS
	storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
#else
	typename storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
#endif
    options.buildAllRewardModels = false;
    options.rewardModelsToBuild.insert("up");
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program, options);
    ASSERT_EQ(storm::models::ModelType::Ctmc, model->getType());
    std::shared_ptr<storm::models::symbolic::Ctmc<storm::dd::DdType::CUDD>> ctmc = model->as<storm::models::symbolic::Ctmc<storm::dd::DdType::CUDD>>();
    
    // Create model checker.
    storm::modelchecker::HybridCtmcCslModelChecker<storm::models::symbolic::Ctmc<storm::dd::DdType::CUDD, double>> modelchecker(*ctmc, std::make_unique<storm::solver::NativeLinearEquationSolverFactory<double>>());
    
    // Start checking properties.
    formula = formulaParser.parseSingleFormulaFromString("P=? [ F<=10000 \"down\"]");
    std::unique_ptr<storm::modelchecker::CheckResult> checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD> quantitativeCheckResult1 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    EXPECT_NEAR(0.0019216435246119591, quantitativeCheckResult1.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.0019216435246119591, quantitativeCheckResult1.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ !\"down\" U<=10000 \"fail_actuators\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD> quantitativeCheckResult2 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    EXPECT_NEAR(3.7079151806696567E-6, quantitativeCheckResult2.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(3.7079151806696567E-6, quantitativeCheckResult2.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ !\"down\" U<=10000 \"fail_io\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD> quantitativeCheckResult3 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    EXPECT_NEAR(0.001556839327673734, quantitativeCheckResult3.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.001556839327673734, quantitativeCheckResult3.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ !\"down\" U<=10000 \"fail_sensors\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD> quantitativeCheckResult4 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    EXPECT_NEAR(4.429620626755424E-5, quantitativeCheckResult4.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(4.429620626755424E-5, quantitativeCheckResult4.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("R=? [C<=10000]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD> quantitativeCheckResult5 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    EXPECT_NEAR(2.7745274082080154, quantitativeCheckResult5.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(2.7745274082080154, quantitativeCheckResult5.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("LRA=? [\"fail_sensors\"]");
    checkResult = modelchecker.check(*formula);
    
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD> quantitativeCheckResult6 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    EXPECT_NEAR(0.93458777106146362, quantitativeCheckResult6.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.93458777106146362, quantitativeCheckResult6.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
}

TEST(NativeHybridCtmcCslModelCheckerTest, Embedded_Sylvan) {
    // Set the PRISM compatibility mode temporarily. It is set to its old value once the returned object is destructed.
    std::unique_ptr<storm::settings::SettingMemento> enablePrismCompatibility = storm::settings::mutableIOSettings().overridePrismCompatibilityMode(true);
    
    // Parse the model description.
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/ctmc/embedded2.sm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    storm::parser::FormulaParser formulaParser(program);
    std::shared_ptr<storm::logic::Formula const> formula(nullptr);
    
    // Build the model.
#ifdef WINDOWS
    storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>::Options options;
#else
    typename storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>::Options options;
#endif
    options.buildAllRewardModels = false;
    options.rewardModelsToBuild.insert("up");
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program, options);
    ASSERT_EQ(storm::models::ModelType::Ctmc, model->getType());
    std::shared_ptr<storm::models::symbolic::Ctmc<storm::dd::DdType::Sylvan>> ctmc = model->as<storm::models::symbolic::Ctmc<storm::dd::DdType::Sylvan>>();
    
    // Create model checker.
    storm::modelchecker::HybridCtmcCslModelChecker<storm::models::symbolic::Ctmc<storm::dd::DdType::Sylvan, double>> modelchecker(*ctmc, std::make_unique<storm::solver::NativeLinearEquationSolverFactory<double>>());
    
    // Start checking properties.
    formula = formulaParser.parseSingleFormulaFromString("P=? [ F<=10000 \"down\"]");
    std::unique_ptr<storm::modelchecker::CheckResult> checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan> quantitativeCheckResult1 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    EXPECT_NEAR(0.0019216435246119591, quantitativeCheckResult1.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.0019216435246119591, quantitativeCheckResult1.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ !\"down\" U<=10000 \"fail_actuators\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan> quantitativeCheckResult2 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    EXPECT_NEAR(3.7079151806696567E-6, quantitativeCheckResult2.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(3.7079151806696567E-6, quantitativeCheckResult2.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ !\"down\" U<=10000 \"fail_io\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan> quantitativeCheckResult3 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    EXPECT_NEAR(0.001556839327673734, quantitativeCheckResult3.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.001556839327673734, quantitativeCheckResult3.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ !\"down\" U<=10000 \"fail_sensors\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan> quantitativeCheckResult4 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    EXPECT_NEAR(4.429620626755424E-5, quantitativeCheckResult4.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(4.429620626755424E-5, quantitativeCheckResult4.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("R=? [C<=10000]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan> quantitativeCheckResult5 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    EXPECT_NEAR(2.7745274082080154, quantitativeCheckResult5.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(2.7745274082080154, quantitativeCheckResult5.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("LRA=? [\"fail_sensors\"]");
    checkResult = modelchecker.check(*formula);
    
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan> quantitativeCheckResult6 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    EXPECT_NEAR(0.93458777106146362, quantitativeCheckResult6.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.93458777106146362, quantitativeCheckResult6.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
}

TEST(NativeHybridCtmcCslModelCheckerTest, Polling_Cudd) {
    // Set the PRISM compatibility mode temporarily. It is set to its old value once the returned object is destructed.
    std::unique_ptr<storm::settings::SettingMemento> enablePrismCompatibility = storm::settings::mutableIOSettings().overridePrismCompatibilityMode(true);
    
    // Parse the model description.
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/ctmc/polling2.sm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    storm::parser::FormulaParser formulaParser(program);
    std::shared_ptr<storm::logic::Formula const> formula(nullptr);
    
    // Build the model.
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program);
    ASSERT_EQ(storm::models::ModelType::Ctmc, model->getType());
    std::shared_ptr<storm::models::symbolic::Ctmc<storm::dd::DdType::CUDD>> ctmc = model->as<storm::models::symbolic::Ctmc<storm::dd::DdType::CUDD>>();
    
    // Create model checker.
    storm::modelchecker::HybridCtmcCslModelChecker<storm::models::symbolic::Ctmc<storm::dd::DdType::CUDD, double>> modelchecker(*ctmc, std::make_unique<storm::solver::NativeLinearEquationSolverFactory<double>>());
    
    // Start checking properties.
    formula = formulaParser.parseSingleFormulaFromString("P=?[ F<=10 \"target\"]");
    std::unique_ptr<storm::modelchecker::CheckResult> checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD> quantitativeCheckResult1 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    EXPECT_NEAR(1, quantitativeCheckResult1.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(1, quantitativeCheckResult1.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("LRA=? [\"target\"]");
    checkResult = modelchecker.check(*formula);
    
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD> quantitativeCheckResult2 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    EXPECT_NEAR(0.20079750055570736, quantitativeCheckResult2.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.20079750055570736, quantitativeCheckResult2.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
}

TEST(NativeHybridCtmcCslModelCheckerTest, Polling_Sylvan) {
    // Set the PRISM compatibility mode temporarily. It is set to its old value once the returned object is destructed.
    std::unique_ptr<storm::settings::SettingMemento> enablePrismCompatibility = storm::settings::mutableIOSettings().overridePrismCompatibilityMode(true);
    
    // Parse the model description.
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/ctmc/polling2.sm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    storm::parser::FormulaParser formulaParser(program);
    std::shared_ptr<storm::logic::Formula const> formula(nullptr);
    
    // Build the model.
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program);
    ASSERT_EQ(storm::models::ModelType::Ctmc, model->getType());
    std::shared_ptr<storm::models::symbolic::Ctmc<storm::dd::DdType::Sylvan>> ctmc = model->as<storm::models::symbolic::Ctmc<storm::dd::DdType::Sylvan>>();
    
    // Create model checker.
    storm::modelchecker::HybridCtmcCslModelChecker<storm::models::symbolic::Ctmc<storm::dd::DdType::Sylvan, double>> modelchecker(*ctmc, std::make_unique<storm::solver::NativeLinearEquationSolverFactory<double>>());
    
    // Start checking properties.
    formula = formulaParser.parseSingleFormulaFromString("P=?[ F<=10 \"target\"]");
    std::unique_ptr<storm::modelchecker::CheckResult> checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan> quantitativeCheckResult1 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    EXPECT_NEAR(1, quantitativeCheckResult1.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(1, quantitativeCheckResult1.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("LRA=? [\"target\"]");
    checkResult = modelchecker.check(*formula);
    
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan> quantitativeCheckResult2 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    EXPECT_NEAR(0.20079750055570736, quantitativeCheckResult2.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.20079750055570736, quantitativeCheckResult2.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
}

TEST(NativeHybridCtmcCslModelCheckerTest, Fms) {
    // Set the PRISM compatibility mode temporarily. It is set to its old value once the returned object is destructed.
    std::unique_ptr<storm::settings::SettingMemento> enablePrismCompatibility = storm::settings::mutableIOSettings().overridePrismCompatibilityMode(true);
    
    // No properties to check at this point.
}

TEST(NativeHybridCtmcCslModelCheckerTest, Tandem_Cudd) {
    // Set the PRISM compatibility mode temporarily. It is set to its old value once the returned object is destructed.
    std::unique_ptr<storm::settings::SettingMemento> enablePrismCompatibility = storm::settings::mutableIOSettings().overridePrismCompatibilityMode(true);
    
    // Parse the model description.
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/ctmc/tandem5.sm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    storm::parser::FormulaParser formulaParser(program);
    std::shared_ptr<storm::logic::Formula const> formula(nullptr);
    
    // Build the model with the customers reward structure.
#ifdef WINDOWS
	storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
#else
	typename storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
#endif
    options.buildAllRewardModels = false;
    options.rewardModelsToBuild.insert("customers");
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program, options);
    ASSERT_EQ(storm::models::ModelType::Ctmc, model->getType());
    std::shared_ptr<storm::models::symbolic::Ctmc<storm::dd::DdType::CUDD>> ctmc = model->as<storm::models::symbolic::Ctmc<storm::dd::DdType::CUDD>>();
    
    // Create model checker.
    storm::modelchecker::HybridCtmcCslModelChecker<storm::models::symbolic::Ctmc<storm::dd::DdType::CUDD, double>> modelchecker(*ctmc, std::make_unique<storm::solver::NativeLinearEquationSolverFactory<double>>());
    
    // Start checking properties.
    formula = formulaParser.parseSingleFormulaFromString("P=? [ F<=10 \"network_full\" ]");
    std::unique_ptr<storm::modelchecker::CheckResult> checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD> quantitativeCheckResult1 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    EXPECT_NEAR(0.015446370562428037, quantitativeCheckResult1.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.015446370562428037, quantitativeCheckResult1.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ F<=10 \"first_queue_full\" ]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD> quantitativeCheckResult2 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    EXPECT_NEAR(0.999999837225515, quantitativeCheckResult2.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.999999837225515, quantitativeCheckResult2.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [\"second_queue_full\" U<=1 !\"second_queue_full\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD> quantitativeCheckResult3 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    EXPECT_NEAR(1, quantitativeCheckResult3.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(1, quantitativeCheckResult3.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("R=? [I=10]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD> quantitativeCheckResult4 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    EXPECT_NEAR(5.679243850315877, quantitativeCheckResult4.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(5.679243850315877, quantitativeCheckResult4.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("R=? [C<=10]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD> quantitativeCheckResult5 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    EXPECT_NEAR(55.44792186036232, quantitativeCheckResult5.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(55.44792186036232, quantitativeCheckResult5.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("R=? [F \"first_queue_full\"&\"second_queue_full\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD> quantitativeCheckResult6 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    EXPECT_NEAR(262.78571505691389, quantitativeCheckResult6.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(262.78571505691389, quantitativeCheckResult6.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("LRA=? [\"first_queue_full\"]");
    checkResult = modelchecker.check(*formula);
    
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD> quantitativeCheckResult7 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>();
    
    // FIXME: because of divergence, these results are not correct.
//    EXPECT_NEAR(0.9100373532, quantitativeCheckResult7.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
//    EXPECT_NEAR(0.9100373532, quantitativeCheckResult7.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
}

TEST(NativeHybridCtmcCslModelCheckerTest, Tandem_Sylvan) {
    // Set the PRISM compatibility mode temporarily. It is set to its old value once the returned object is destructed.
    std::unique_ptr<storm::settings::SettingMemento> enablePrismCompatibility = storm::settings::mutableIOSettings().overridePrismCompatibilityMode(true);
    
    // Parse the model description.
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/ctmc/tandem5.sm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    storm::parser::FormulaParser formulaParser(program);
    std::shared_ptr<storm::logic::Formula const> formula(nullptr);
    
    // Build the model with the customers reward structure.
#ifdef WINDOWS
    storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>::Options options;
#else
    typename storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>::Options options;
#endif
    options.buildAllRewardModels = false;
    options.rewardModelsToBuild.insert("customers");
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program, options);
    ASSERT_EQ(storm::models::ModelType::Ctmc, model->getType());
    std::shared_ptr<storm::models::symbolic::Ctmc<storm::dd::DdType::Sylvan>> ctmc = model->as<storm::models::symbolic::Ctmc<storm::dd::DdType::Sylvan>>();
    
    // Create model checker.
    storm::modelchecker::HybridCtmcCslModelChecker<storm::models::symbolic::Ctmc<storm::dd::DdType::Sylvan, double>> modelchecker(*ctmc, std::make_unique<storm::solver::NativeLinearEquationSolverFactory<double>>());
    
    // Start checking properties.
    formula = formulaParser.parseSingleFormulaFromString("P=? [ F<=10 \"network_full\" ]");
    std::unique_ptr<storm::modelchecker::CheckResult> checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan> quantitativeCheckResult1 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    EXPECT_NEAR(0.015446370562428037, quantitativeCheckResult1.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.015446370562428037, quantitativeCheckResult1.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ F<=10 \"first_queue_full\" ]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan> quantitativeCheckResult2 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    EXPECT_NEAR(0.999999837225515, quantitativeCheckResult2.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(0.999999837225515, quantitativeCheckResult2.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [\"second_queue_full\" U<=1 !\"second_queue_full\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan> quantitativeCheckResult3 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    EXPECT_NEAR(1, quantitativeCheckResult3.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(1, quantitativeCheckResult3.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("R=? [I=10]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan> quantitativeCheckResult4 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    EXPECT_NEAR(5.679243850315877, quantitativeCheckResult4.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(5.679243850315877, quantitativeCheckResult4.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("R=? [C<=10]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan> quantitativeCheckResult5 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    EXPECT_NEAR(55.44792186036232, quantitativeCheckResult5.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(55.44792186036232, quantitativeCheckResult5.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("R=? [F \"first_queue_full\"&\"second_queue_full\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan> quantitativeCheckResult6 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();
    EXPECT_NEAR(262.78571505691389, quantitativeCheckResult6.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    EXPECT_NEAR(262.78571505691389, quantitativeCheckResult6.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("LRA=? [\"first_queue_full\"]");
    checkResult = modelchecker.check(*formula);
    
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan> quantitativeCheckResult7 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>();

    // FIXME: because of divergence, these results are not correct.
//    EXPECT_NEAR(0.9100373532, quantitativeCheckResult7.getMin(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
//    EXPECT_NEAR(0.9100373532, quantitativeCheckResult7.getMax(), storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
}
