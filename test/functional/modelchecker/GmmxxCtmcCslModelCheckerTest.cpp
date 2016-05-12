#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/settings/SettingMemento.h"
#include "src/parser/PrismParser.h"
#include "src/parser/FormulaParser.h"
#include "src/logic/Formulas.h"
#include "src/builder/ExplicitPrismModelBuilder.h"

#include "src/solver/GmmxxLinearEquationSolver.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/modelchecker/csl/SparseCtmcCslModelChecker.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/modules/GeneralSettings.h"

#include "src/settings/modules/NativeEquationSolverSettings.h"
#include "src/settings/modules/GmmxxEquationSolverSettings.h"

TEST(GmmxxCtmcCslModelCheckerTest, Cluster) {
    // Set the PRISM compatibility mode temporarily. It is set to its old value once the returned object is destructed.
    std::unique_ptr<storm::settings::SettingMemento> enablePrismCompatibility = storm::settings::mutableGeneralSettings().overridePrismCompatibilityMode(true);
    
    // Parse the model description.
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/cluster2.sm");
    storm::parser::FormulaParser formulaParser(program.getManager().getSharedPointer());
    std::shared_ptr<const storm::logic::Formula> formula(nullptr);
    
    // Build the model.
#ifdef WINDOWS
    storm::builder::ExplicitPrismModelBuilder<double>::Options options;
#else
	typename storm::builder::ExplicitPrismModelBuilder<double>::Options options;
#endif
    options.buildAllRewardModels = false;
    options.rewardModelsToBuild.insert("num_repairs");
    std::shared_ptr<storm::models::sparse::Model<double>> model = storm::builder::ExplicitPrismModelBuilder<double>(program, options).translate();
    ASSERT_EQ(storm::models::ModelType::Ctmc, model->getType());
    std::shared_ptr<storm::models::sparse::Ctmc<double>> ctmc = model->as<storm::models::sparse::Ctmc<double>>();
    uint_fast64_t initialState = *ctmc->getInitialStates().begin();
    
    // Create model checker.
    storm::modelchecker::SparseCtmcCslModelChecker<storm::models::sparse::Ctmc<double>> modelchecker(*ctmc, std::unique_ptr<storm::utility::solver::LinearEquationSolverFactory<double>>(new storm::utility::solver::GmmxxLinearEquationSolverFactory<double>()));
    
    // Start checking properties.
    formula = formulaParser.parseSingleFormulaFromString("P=? [ F<=100 !\"minimum\"]");
    std::unique_ptr<storm::modelchecker::CheckResult> checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult1 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(5.5461254704419085E-5, quantitativeCheckResult1[initialState], storm::settings::generalSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ F[100,100] !\"minimum\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult2 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(2.3397873548343415E-6, quantitativeCheckResult2[initialState], storm::settings::generalSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ F[100,2000] !\"minimum\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult3 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(0.001105335651670241, quantitativeCheckResult3[initialState], storm::settings::generalSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ \"minimum\" U<=10 \"premium\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult4 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(1, quantitativeCheckResult4[initialState], storm::settings::generalSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ !\"minimum\" U[1,inf] \"minimum\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult5 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(0, quantitativeCheckResult5[initialState], storm::settings::generalSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ \"minimum\" U[1,inf] !\"minimum\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult6 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(0.9999999033633374, quantitativeCheckResult6[initialState], storm::settings::generalSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("R=? [C<=100]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult7 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(0.8602815057967503, quantitativeCheckResult7[initialState], storm::settings::generalSettings().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("LRA=? [\"minimum\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult8 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(0.99999766034263426, quantitativeCheckResult8[initialState], storm::settings::generalSettings().getPrecision());
}

TEST(GmmxxCtmcCslModelCheckerTest, Embedded) {
    // Set the PRISM compatibility mode temporarily. It is set to its old value once the returned object is destructed.
    std::unique_ptr<storm::settings::SettingMemento> enablePrismCompatibility = storm::settings::mutableGeneralSettings().overridePrismCompatibilityMode(true);
    
    // Parse the model description.
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/embedded2.sm");
    storm::parser::FormulaParser formulaParser(program.getManager().getSharedPointer());
    std::shared_ptr<const storm::logic::Formula> formula(nullptr);
    
    // Build the model.
#ifdef WINDOWS
	storm::builder::ExplicitPrismModelBuilder<double>::Options options;
#else
	typename storm::builder::ExplicitPrismModelBuilder<double>::Options options;
#endif
    options.buildAllRewardModels = false;
    options.rewardModelsToBuild.insert("up");
    std::shared_ptr<storm::models::sparse::Model<double>> model = storm::builder::ExplicitPrismModelBuilder<double>(program, options).translate();
    ASSERT_EQ(storm::models::ModelType::Ctmc, model->getType());
    std::shared_ptr<storm::models::sparse::Ctmc<double>> ctmc = model->as<storm::models::sparse::Ctmc<double>>();
    uint_fast64_t initialState = *ctmc->getInitialStates().begin();
    
    // Create model checker.
    storm::modelchecker::SparseCtmcCslModelChecker<storm::models::sparse::Ctmc<double>> modelchecker(*ctmc, std::unique_ptr<storm::utility::solver::LinearEquationSolverFactory<double>>(new storm::utility::solver::GmmxxLinearEquationSolverFactory<double>()));
    
    // Start checking properties.
    formula = formulaParser.parseSingleFormulaFromString("P=? [ F<=10000 \"down\"]");
    std::unique_ptr<storm::modelchecker::CheckResult> checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult1 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(0.0019216435246119591, quantitativeCheckResult1[initialState], storm::settings::generalSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ !\"down\" U<=10000 \"fail_actuators\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult2 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(3.7079151806696567E-6, quantitativeCheckResult2[initialState], storm::settings::generalSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ !\"down\" U<=10000 \"fail_io\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult3 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(0.001556839327673734, quantitativeCheckResult3[initialState], storm::settings::generalSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ !\"down\" U<=10000 \"fail_sensors\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult4 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(4.429620626755424E-5, quantitativeCheckResult4[initialState], storm::settings::generalSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("R=? [C<=10000]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult5 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(2.7745274082080154, quantitativeCheckResult5[initialState], storm::settings::generalSettings().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("LRA=? [\"fail_sensors\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult6 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(0.93458866427696596, quantitativeCheckResult6[initialState], storm::settings::generalSettings().getPrecision());
}

TEST(GmmxxCtmcCslModelCheckerTest, Polling) {
    // Set the PRISM compatibility mode temporarily. It is set to its old value once the returned object is destructed.
    std::unique_ptr<storm::settings::SettingMemento> enablePrismCompatibility = storm::settings::mutableGeneralSettings().overridePrismCompatibilityMode(true);
    
    // Parse the model description.
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/polling2.sm");
    storm::parser::FormulaParser formulaParser(program.getManager().getSharedPointer());
    std::shared_ptr<const storm::logic::Formula> formula(nullptr);
    
    // Build the model.
    std::shared_ptr<storm::models::sparse::Model<double>> model = storm::builder::ExplicitPrismModelBuilder<double>(program).translate();
    ASSERT_EQ(storm::models::ModelType::Ctmc, model->getType());
    std::shared_ptr<storm::models::sparse::Ctmc<double>> ctmc = model->as<storm::models::sparse::Ctmc<double>>();
    uint_fast64_t initialState = *ctmc->getInitialStates().begin();
    
    // Create model checker.
    storm::modelchecker::SparseCtmcCslModelChecker<storm::models::sparse::Ctmc<double>> modelchecker(*ctmc, std::unique_ptr<storm::utility::solver::LinearEquationSolverFactory<double>>(new storm::utility::solver::GmmxxLinearEquationSolverFactory<double>()));
    
    // Start checking properties.
    formula = formulaParser.parseSingleFormulaFromString("P=?[ F<=10 \"target\"]");
    std::unique_ptr<storm::modelchecker::CheckResult> checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult1 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(1, quantitativeCheckResult1[initialState], storm::settings::generalSettings().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("LRA=?[\"target\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult2 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(0.20079750055570736, quantitativeCheckResult2[initialState], storm::settings::generalSettings().getPrecision());
}

TEST(GmmxxCtmcCslModelCheckerTest, Fms) {
    // Set the PRISM compatibility mode temporarily. It is set to its old value once the returned object is destructed.
    std::unique_ptr<storm::settings::SettingMemento> enablePrismCompatibility = storm::settings::mutableGeneralSettings().overridePrismCompatibilityMode(true);
    
    // No properties to check at this point.
}

TEST(GmmxxCtmcCslModelCheckerTest, Tandem) {
    // Set the PRISM compatibility mode temporarily. It is set to its old value once the returned object is destructed.
    std::unique_ptr<storm::settings::SettingMemento> enablePrismCompatibility = storm::settings::mutableGeneralSettings().overridePrismCompatibilityMode(true);
    
    // Parse the model description.
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/tandem5.sm");
    storm::parser::FormulaParser formulaParser(program.getManager().getSharedPointer());
    std::shared_ptr<const storm::logic::Formula> formula(nullptr);
    
    // Build the model.
#ifdef WINDOWS
	storm::builder::ExplicitPrismModelBuilder<double>::Options options;
#else
	typename storm::builder::ExplicitPrismModelBuilder<double>::Options options;
#endif
    options.rewardModelsToBuild.insert("customers");
    std::shared_ptr<storm::models::sparse::Model<double>> model = storm::builder::ExplicitPrismModelBuilder<double>(program, options).translate();
    ASSERT_EQ(storm::models::ModelType::Ctmc, model->getType());
    std::shared_ptr<storm::models::sparse::Ctmc<double>> ctmc = model->as<storm::models::sparse::Ctmc<double>>();
    uint_fast64_t initialState = *ctmc->getInitialStates().begin();
    
    // Create model checker.
    storm::modelchecker::SparseCtmcCslModelChecker<storm::models::sparse::Ctmc<double>> modelchecker(*ctmc, std::unique_ptr<storm::utility::solver::LinearEquationSolverFactory<double>>(new storm::utility::solver::GmmxxLinearEquationSolverFactory<double>()));
    
    // Start checking properties.
    formula = formulaParser.parseSingleFormulaFromString("P=? [ F<=10 \"network_full\" ]");
    std::unique_ptr<storm::modelchecker::CheckResult> checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult1 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(0.015446370562428037, quantitativeCheckResult1[initialState], storm::settings::generalSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ F<=10 \"first_queue_full\" ]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult2 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(0.999999837225515, quantitativeCheckResult2[initialState], storm::settings::generalSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [\"second_queue_full\" U<=1 !\"second_queue_full\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult3 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(1, quantitativeCheckResult3[initialState], storm::settings::generalSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("R=? [I=10]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult4 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(5.679243850315877, quantitativeCheckResult4[initialState], storm::settings::generalSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("R=? [C<=10]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult5 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(55.44792186036232, quantitativeCheckResult5[initialState], storm::settings::generalSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("R=? [F \"first_queue_full\"&\"second_queue_full\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult6 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(262.85103661561755, quantitativeCheckResult6[initialState], storm::settings::generalSettings().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("LRA=? [\"first_queue_full\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult7 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(0.9100373532, quantitativeCheckResult7[initialState], storm::settings::generalSettings().getPrecision());
}
