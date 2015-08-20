#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/settings/SettingMemento.h"
#include "src/parser/PrismParser.h"
#include "src/parser/FormulaParser.h"
#include "src/logic/Formulas.h"
#include "src/builder/DdPrismModelBuilder.h"
#include "src/storage/dd/DdType.h"

#include "src/utility/solver.h"
#include "src/modelchecker/csl/HybridCtmcCslModelChecker.h"
#include "src/modelchecker/results/HybridQuantitativeCheckResult.h"
#include "src/modelchecker/results/SymbolicQualitativeCheckResult.h"
#include "src/modelchecker/results/SymbolicQuantitativeCheckResult.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/modules/GeneralSettings.h"

TEST(NativeHybridCtmcCslModelCheckerTest, Cluster) {
    // Set the PRISM compatibility mode temporarily. It is set to its old value once the returned object is destructed.
    std::unique_ptr<storm::settings::SettingMemento> enablePrismCompatibility = storm::settings::mutableGeneralSettings().overridePrismCompatibilityMode(true);
    
    // Parse the model description.
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/cluster2.sm");
    storm::parser::FormulaParser formulaParser(program.getManager().getSharedPointer());
    std::shared_ptr<storm::logic::Formula> formula(nullptr);
    
    // Build the model.
#ifdef WINDOWS
	storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
#else
	typename storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
#endif
    options.rewardModelsToBuild.insert("num_repairs");
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::translateProgram(program, options);
    ASSERT_EQ(storm::models::ModelType::Ctmc, model->getType());
    std::shared_ptr<storm::models::symbolic::Ctmc<storm::dd::DdType::CUDD>> ctmc = model->as<storm::models::symbolic::Ctmc<storm::dd::DdType::CUDD>>();
    
    // Create model checker.
    storm::modelchecker::HybridCtmcCslModelChecker<storm::dd::DdType::CUDD, double> modelchecker(*ctmc, std::unique_ptr<storm::utility::solver::LinearEquationSolverFactory<double>>(new storm::utility::solver::NativeLinearEquationSolverFactory<double>()));
    
    // Start checking properties.
    formula = formulaParser.parseFromString("P=? [ F<=100 !\"minimum\"]");
    std::unique_ptr<storm::modelchecker::CheckResult> checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD> quantitativeCheckResult1 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    EXPECT_NEAR(5.5461254704419085E-5, quantitativeCheckResult1.getMin(), storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(5.5461254704419085E-5, quantitativeCheckResult1.getMax(), storm::settings::generalSettings().getPrecision());
    
    formula = formulaParser.parseFromString("P=? [ F[100,100] !\"minimum\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>  quantitativeCheckResult2 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    EXPECT_NEAR(2.3397873548343415E-6, quantitativeCheckResult2.getMin(), storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(2.3397873548343415E-6, quantitativeCheckResult2.getMax(), storm::settings::generalSettings().getPrecision());
    
    formula = formulaParser.parseFromString("P=? [ F[100,2000] !\"minimum\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>  quantitativeCheckResult3 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    EXPECT_NEAR(0.001105335651670241, quantitativeCheckResult3.getMin(), storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.001105335651670241, quantitativeCheckResult3.getMax(), storm::settings::generalSettings().getPrecision());
    
    formula = formulaParser.parseFromString("P=? [ \"minimum\" U<=10 \"premium\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>  quantitativeCheckResult4 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    EXPECT_NEAR(1, quantitativeCheckResult4.getMin(), storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(1, quantitativeCheckResult4.getMax(), storm::settings::generalSettings().getPrecision());
    
    formula = formulaParser.parseFromString("P=? [ !\"minimum\" U[1,inf] \"minimum\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>  quantitativeCheckResult5 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    EXPECT_NEAR(0, quantitativeCheckResult5.getMin(), storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0, quantitativeCheckResult5.getMax(), storm::settings::generalSettings().getPrecision());
    
    formula = formulaParser.parseFromString("P=? [ \"minimum\" U[1,inf] !\"minimum\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>  quantitativeCheckResult6 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    EXPECT_NEAR(0.9999999033633374, quantitativeCheckResult6.getMin(), storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.9999999033633374, quantitativeCheckResult6.getMax(), storm::settings::generalSettings().getPrecision());
    
    formula = formulaParser.parseFromString("R=? [C<=100]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>  quantitativeCheckResult7 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    EXPECT_NEAR(0.8602815057967503, quantitativeCheckResult7.getMin(), storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.8602815057967503, quantitativeCheckResult7.getMax(), storm::settings::generalSettings().getPrecision());
}

TEST(NativeHybridCtmcCslModelCheckerTest, Embedded) {
    // Set the PRISM compatibility mode temporarily. It is set to its old value once the returned object is destructed.
    std::unique_ptr<storm::settings::SettingMemento> enablePrismCompatibility = storm::settings::mutableGeneralSettings().overridePrismCompatibilityMode(true);
    
    // Parse the model description.
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/embedded2.sm");
    storm::parser::FormulaParser formulaParser(program.getManager().getSharedPointer());
    std::shared_ptr<storm::logic::Formula> formula(nullptr);
    
    // Build the model.
#ifdef WINDOWS
	storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
#else
	typename storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
#endif
    options.rewardModelsToBuild.insert("up");
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::translateProgram(program, options);
    ASSERT_EQ(storm::models::ModelType::Ctmc, model->getType());
    std::shared_ptr<storm::models::symbolic::Ctmc<storm::dd::DdType::CUDD>> ctmc = model->as<storm::models::symbolic::Ctmc<storm::dd::DdType::CUDD>>();
    
    // Create model checker.
    storm::modelchecker::HybridCtmcCslModelChecker<storm::dd::DdType::CUDD, double> modelchecker(*ctmc, std::unique_ptr<storm::utility::solver::LinearEquationSolverFactory<double>>(new storm::utility::solver::NativeLinearEquationSolverFactory<double>()));
    
    // Start checking properties.
    formula = formulaParser.parseFromString("P=? [ F<=10000 \"down\"]");
    std::unique_ptr<storm::modelchecker::CheckResult> checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD> quantitativeCheckResult1 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    EXPECT_NEAR(0.0019216435246119591, quantitativeCheckResult1.getMin(), storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.0019216435246119591, quantitativeCheckResult1.getMax(), storm::settings::generalSettings().getPrecision());
    
    formula = formulaParser.parseFromString("P=? [ !\"down\" U<=10000 \"fail_actuators\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD> quantitativeCheckResult2 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    EXPECT_NEAR(3.7079151806696567E-6, quantitativeCheckResult2.getMin(), storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(3.7079151806696567E-6, quantitativeCheckResult2.getMax(), storm::settings::generalSettings().getPrecision());
    
    formula = formulaParser.parseFromString("P=? [ !\"down\" U<=10000 \"fail_io\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD> quantitativeCheckResult3 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    EXPECT_NEAR(0.001556839327673734, quantitativeCheckResult3.getMin(), storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.001556839327673734, quantitativeCheckResult3.getMax(), storm::settings::generalSettings().getPrecision());
    
    formula = formulaParser.parseFromString("P=? [ !\"down\" U<=10000 \"fail_sensors\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD> quantitativeCheckResult4 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    EXPECT_NEAR(4.429620626755424E-5, quantitativeCheckResult4.getMin(), storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(4.429620626755424E-5, quantitativeCheckResult4.getMax(), storm::settings::generalSettings().getPrecision());
    
    formula = formulaParser.parseFromString("R=? [C<=10000]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD> quantitativeCheckResult5 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    EXPECT_NEAR(2.7745274082080154, quantitativeCheckResult5.getMin(), storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(2.7745274082080154, quantitativeCheckResult5.getMax(), storm::settings::generalSettings().getPrecision());
}

TEST(NativeHybridCtmcCslModelCheckerTest, Polling) {
    // Set the PRISM compatibility mode temporarily. It is set to its old value once the returned object is destructed.
    std::unique_ptr<storm::settings::SettingMemento> enablePrismCompatibility = storm::settings::mutableGeneralSettings().overridePrismCompatibilityMode(true);
    
    // Parse the model description.
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/polling2.sm");
    storm::parser::FormulaParser formulaParser(program.getManager().getSharedPointer());
    std::shared_ptr<storm::logic::Formula> formula(nullptr);
    
    // Build the model.
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::translateProgram(program);
    ASSERT_EQ(storm::models::ModelType::Ctmc, model->getType());
    std::shared_ptr<storm::models::symbolic::Ctmc<storm::dd::DdType::CUDD>> ctmc = model->as<storm::models::symbolic::Ctmc<storm::dd::DdType::CUDD>>();
    
    // Create model checker.
    storm::modelchecker::HybridCtmcCslModelChecker<storm::dd::DdType::CUDD, double> modelchecker(*ctmc, std::unique_ptr<storm::utility::solver::LinearEquationSolverFactory<double>>(new storm::utility::solver::NativeLinearEquationSolverFactory<double>()));
    
    // Start checking properties.
    formula = formulaParser.parseFromString("P=?[ F<=10 \"target\"]");
    std::unique_ptr<storm::modelchecker::CheckResult> checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD> quantitativeCheckResult1 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    EXPECT_NEAR(1, quantitativeCheckResult1.getMin(), storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(1, quantitativeCheckResult1.getMax(), storm::settings::generalSettings().getPrecision());
}

TEST(NativeHybridCtmcCslModelCheckerTest, Fms) {
    // Set the PRISM compatibility mode temporarily. It is set to its old value once the returned object is destructed.
    std::unique_ptr<storm::settings::SettingMemento> enablePrismCompatibility = storm::settings::mutableGeneralSettings().overridePrismCompatibilityMode(true);
    
    // No properties to check at this point.
}

TEST(NativeHybridCtmcCslModelCheckerTest, Tandem) {
    // Set the PRISM compatibility mode temporarily. It is set to its old value once the returned object is destructed.
    std::unique_ptr<storm::settings::SettingMemento> enablePrismCompatibility = storm::settings::mutableGeneralSettings().overridePrismCompatibilityMode(true);
    
    // Parse the model description.
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/tandem5.sm");
    storm::parser::FormulaParser formulaParser(program.getManager().getSharedPointer());
    std::shared_ptr<storm::logic::Formula> formula(nullptr);
    
    // Build the model with the customers reward structure.
#ifdef WINDOWS
	storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
#else
	typename storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
#endif
    options.rewardModelsToBuild.insert("customers");
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::translateProgram(program, options);
    ASSERT_EQ(storm::models::ModelType::Ctmc, model->getType());
    std::shared_ptr<storm::models::symbolic::Ctmc<storm::dd::DdType::CUDD>> ctmc = model->as<storm::models::symbolic::Ctmc<storm::dd::DdType::CUDD>>();
    
    // Create model checker.
    storm::modelchecker::HybridCtmcCslModelChecker<storm::dd::DdType::CUDD, double> modelchecker(*ctmc, std::unique_ptr<storm::utility::solver::LinearEquationSolverFactory<double>>(new storm::utility::solver::NativeLinearEquationSolverFactory<double>()));
    
    // Start checking properties.
    formula = formulaParser.parseFromString("P=? [ F<=10 \"network_full\" ]");
    std::unique_ptr<storm::modelchecker::CheckResult> checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD> quantitativeCheckResult1 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    EXPECT_NEAR(0.015446370562428037, quantitativeCheckResult1.getMin(), storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.015446370562428037, quantitativeCheckResult1.getMax(), storm::settings::generalSettings().getPrecision());
    
    formula = formulaParser.parseFromString("P=? [ F<=10 \"first_queue_full\" ]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD> quantitativeCheckResult2 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    EXPECT_NEAR(0.999999837225515, quantitativeCheckResult2.getMin(), storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(0.999999837225515, quantitativeCheckResult2.getMax(), storm::settings::generalSettings().getPrecision());
    
    formula = formulaParser.parseFromString("P=? [\"second_queue_full\" U<=1 !\"second_queue_full\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD> quantitativeCheckResult3 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    EXPECT_NEAR(1, quantitativeCheckResult3.getMin(), storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(1, quantitativeCheckResult3.getMax(), storm::settings::generalSettings().getPrecision());
    
    formula = formulaParser.parseFromString("R=? [I=10]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD> quantitativeCheckResult4 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    EXPECT_NEAR(5.679243850315877, quantitativeCheckResult4.getMin(), storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(5.679243850315877, quantitativeCheckResult4.getMax(), storm::settings::generalSettings().getPrecision());
    
    formula = formulaParser.parseFromString("R=? [C<=10]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD> quantitativeCheckResult5 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    EXPECT_NEAR(55.44792186036232, quantitativeCheckResult5.getMin(), storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(55.44792186036232, quantitativeCheckResult5.getMax(), storm::settings::generalSettings().getPrecision());
    
    formula = formulaParser.parseFromString("R=? [F \"first_queue_full\"&\"second_queue_full\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isHybridQuantitativeCheckResult());
    checkResult->filter(storm::modelchecker::SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>(ctmc->getReachableStates(), ctmc->getInitialStates()));
    storm::modelchecker::HybridQuantitativeCheckResult<storm::dd::DdType::CUDD> quantitativeCheckResult6 = checkResult->asHybridQuantitativeCheckResult<storm::dd::DdType::CUDD>();
    EXPECT_NEAR(262.78571505691389, quantitativeCheckResult6.getMin(), storm::settings::generalSettings().getPrecision());
    EXPECT_NEAR(262.78571505691389, quantitativeCheckResult6.getMax(), storm::settings::generalSettings().getPrecision());
}
