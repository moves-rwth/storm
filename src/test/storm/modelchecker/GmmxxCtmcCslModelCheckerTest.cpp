#include "gtest/gtest.h"
#include "storm-config.h"
#include "storm/settings/SettingMemento.h"
#include "storm/parser/PrismParser.h"
#include "storm/parser/FormulaParser.h"
#include "storm/logic/Formulas.h"
#include "storm/builder/ExplicitModelBuilder.h"

#include "storm/solver/GmmxxLinearEquationSolver.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/modelchecker/csl/SparseCtmcCslModelChecker.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GeneralSettings.h"

#include "storm/settings/modules/NativeEquationSolverSettings.h"
#include "storm/settings/modules/GmmxxEquationSolverSettings.h"

#include "storm/storage/expressions/ExpressionManager.h"

TEST(GmmxxCtmcCslModelCheckerTest, Cluster) {
    // Parse the model description.
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/ctmc/cluster2.sm", true);
    storm::parser::FormulaParser formulaParser(program.getManager().getSharedPointer());
    std::shared_ptr<storm::logic::Formula const> formula(nullptr);
    
    // Build the model.
    storm::generator::NextStateGeneratorOptions options;
    std::shared_ptr<storm::models::sparse::Model<double>> model = storm::builder::ExplicitModelBuilder<double>(program, storm::generator::NextStateGeneratorOptions(false, true). addRewardModel("num_repairs")).build();
    ASSERT_EQ(storm::models::ModelType::Ctmc, model->getType());
    std::shared_ptr<storm::models::sparse::Ctmc<double>> ctmc = model->as<storm::models::sparse::Ctmc<double>>();
    uint_fast64_t initialState = *ctmc->getInitialStates().begin();
    
    // Create model checker.
    storm::modelchecker::SparseCtmcCslModelChecker<storm::models::sparse::Ctmc<double>> modelchecker(*ctmc, std::make_unique<storm::solver::GmmxxLinearEquationSolverFactory<double>>());
    
    // Start checking properties.
    formula = formulaParser.parseSingleFormulaFromString("P=? [ F<=100 !\"minimum\"]");
    std::unique_ptr<storm::modelchecker::CheckResult> checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult1 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(5.5461254704419085E-5, quantitativeCheckResult1[initialState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ F[100,100] !\"minimum\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult2 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(2.3397873548343415E-6, quantitativeCheckResult2[initialState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ F[100,2000] !\"minimum\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult3 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(0.001105335651670241, quantitativeCheckResult3[initialState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ \"minimum\" U<=10 \"premium\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult4 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(1, quantitativeCheckResult4[initialState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ !\"minimum\" U>=1 \"minimum\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult5 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(0, quantitativeCheckResult5[initialState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ \"minimum\" U>=1 !\"minimum\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult6 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(0.9999999033633374, quantitativeCheckResult6[initialState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("R=? [C<=100]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult7 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(0.8602815057967503, quantitativeCheckResult7[initialState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("LRA=? [\"minimum\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult8 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(0.99999766034263426, quantitativeCheckResult8[initialState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
}

TEST(GmmxxCtmcCslModelCheckerTest, Embedded) {
    // Parse the model description.
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/ctmc/embedded2.sm", true);
    storm::parser::FormulaParser formulaParser(program.getManager().getSharedPointer());
    std::shared_ptr<storm::logic::Formula const> formula(nullptr);
    
    // Build the model.
    std::shared_ptr<storm::models::sparse::Model<double>> model = storm::builder::ExplicitModelBuilder<double>(program, storm::generator::NextStateGeneratorOptions(false, true). addRewardModel("up")).build();
    ASSERT_EQ(storm::models::ModelType::Ctmc, model->getType());
    std::shared_ptr<storm::models::sparse::Ctmc<double>> ctmc = model->as<storm::models::sparse::Ctmc<double>>();
    uint_fast64_t initialState = *ctmc->getInitialStates().begin();
    
    // Create model checker.
    storm::modelchecker::SparseCtmcCslModelChecker<storm::models::sparse::Ctmc<double>> modelchecker(*ctmc, std::make_unique<storm::solver::GmmxxLinearEquationSolverFactory<double>>());
    
    // Start checking properties.
    formula = formulaParser.parseSingleFormulaFromString("P=? [ F<=10000 \"down\"]");
    std::unique_ptr<storm::modelchecker::CheckResult> checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult1 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(0.0019216435246119591, quantitativeCheckResult1[initialState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ !\"down\" U<=10000 \"fail_actuators\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult2 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(3.7079151806696567E-6, quantitativeCheckResult2[initialState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ !\"down\" U<=10000 \"fail_io\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult3 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(0.001556839327673734, quantitativeCheckResult3[initialState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ !\"down\" U<=10000 \"fail_sensors\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult4 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(4.429620626755424E-5, quantitativeCheckResult4[initialState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("R=? [C<=10000]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult5 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(2.7745274082080154, quantitativeCheckResult5[initialState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("LRA=? [\"fail_sensors\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult6 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(0.93458866427696596, quantitativeCheckResult6[initialState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
}

TEST(GmmxxCtmcCslModelCheckerTest, Polling) {
    // Parse the model description.
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/ctmc/polling2.sm", true);
    storm::parser::FormulaParser formulaParser(program.getManager().getSharedPointer());
    std::shared_ptr<storm::logic::Formula const> formula(nullptr);
    
    // Build the model.
    std::shared_ptr<storm::models::sparse::Model<double>> model = storm::builder::ExplicitModelBuilder<double>(program, storm::generator::NextStateGeneratorOptions(false, true)).build();
    ASSERT_EQ(storm::models::ModelType::Ctmc, model->getType());
    std::shared_ptr<storm::models::sparse::Ctmc<double>> ctmc = model->as<storm::models::sparse::Ctmc<double>>();
    uint_fast64_t initialState = *ctmc->getInitialStates().begin();
    
    // Create model checker.
    storm::modelchecker::SparseCtmcCslModelChecker<storm::models::sparse::Ctmc<double>> modelchecker(*ctmc, std::make_unique<storm::solver::GmmxxLinearEquationSolverFactory<double>>());
    
    // Start checking properties.
    formula = formulaParser.parseSingleFormulaFromString("P=?[ F<=10 \"target\"]");
    std::unique_ptr<storm::modelchecker::CheckResult> checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult1 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(1, quantitativeCheckResult1[initialState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("LRA=?[\"target\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult2 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(0.20079750055570736, quantitativeCheckResult2[initialState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
}

TEST(GmmxxCtmcCslModelCheckerTest, Fms) {
    // No properties to check at this point.
}

TEST(GmmxxCtmcCslModelCheckerTest, Tandem) {

    // Parse the model description.
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/ctmc/tandem5.sm", true);
    storm::parser::FormulaParser formulaParser(program.getManager().getSharedPointer());
    std::shared_ptr<storm::logic::Formula const> formula(nullptr);
    
    // Build the model.
    std::shared_ptr<storm::models::sparse::Model<double>> model = storm::builder::ExplicitModelBuilder<double>(program, storm::generator::NextStateGeneratorOptions(false, true). addRewardModel("customers")).build();
    ASSERT_EQ(storm::models::ModelType::Ctmc, model->getType());
    std::shared_ptr<storm::models::sparse::Ctmc<double>> ctmc = model->as<storm::models::sparse::Ctmc<double>>();
    uint_fast64_t initialState = *ctmc->getInitialStates().begin();
    
    // Create model checker.
    storm::modelchecker::SparseCtmcCslModelChecker<storm::models::sparse::Ctmc<double>> modelchecker(*ctmc, std::make_unique<storm::solver::GmmxxLinearEquationSolverFactory<double>>());
    
    // Start checking properties.
    formula = formulaParser.parseSingleFormulaFromString("P=? [ F<=10 \"network_full\" ]");
    std::unique_ptr<storm::modelchecker::CheckResult> checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult1 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(0.015446370562428037, quantitativeCheckResult1[initialState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [ F<=10 \"first_queue_full\" ]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult2 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(0.999999837225515, quantitativeCheckResult2[initialState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [\"second_queue_full\" U<=1 !\"second_queue_full\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult3 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(1, quantitativeCheckResult3[initialState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("R=? [I=10]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult4 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(5.679243850315877, quantitativeCheckResult4[initialState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("R=? [C<=10]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult5 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(55.44792186036232, quantitativeCheckResult5[initialState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("R=? [F \"first_queue_full\"&\"second_queue_full\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult6 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(262.85103661561755, quantitativeCheckResult6[initialState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("LRA=? [\"first_queue_full\"]");
    checkResult = modelchecker.check(*formula);
    
    ASSERT_TRUE(checkResult->isExplicitQuantitativeCheckResult());
    storm::modelchecker::ExplicitQuantitativeCheckResult<double> quantitativeCheckResult7 = checkResult->asExplicitQuantitativeCheckResult<double>();
    EXPECT_NEAR(0.9100373532, quantitativeCheckResult7[initialState], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
}
