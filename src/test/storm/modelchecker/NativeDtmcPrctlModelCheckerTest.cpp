#include "gtest/gtest.h"
#include "storm-config.h"

#include "storm/parser/FormulaParser.h"
#include "storm/settings/SettingMemento.h"
#include "storm/logic/Formulas.h"
#include "storm/solver/NativeLinearEquationSolver.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/settings/SettingsManager.h"
#include "storm/solver/NativeLinearEquationSolver.h"
#include "storm/settings/modules/GeneralSettings.h"

#include "storm/settings/modules/NativeEquationSolverSettings.h"
#include "storm/settings/SettingMemento.h"
#include "storm/parser/AutoParser.h"

TEST(NativeDtmcPrctlModelCheckerTest, Die) {
    std::shared_ptr<storm::models::sparse::Model<double>> abstractModel = storm::parser::AutoParser<>::parseModel(STORM_TEST_RESOURCES_DIR "/tra/die.tra", STORM_TEST_RESOURCES_DIR "/lab/die.lab", "", STORM_TEST_RESOURCES_DIR "/rew/die.coin_flips.trans.rew");
    
    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser formulaParser;
    
    ASSERT_EQ(abstractModel->getType(), storm::models::ModelType::Dtmc);
    
    std::shared_ptr<storm::models::sparse::Dtmc<double>> dtmc = abstractModel->as<storm::models::sparse::Dtmc<double>>();
    
    ASSERT_EQ(dtmc->getNumberOfStates(), 13ull);
    ASSERT_EQ(dtmc->getNumberOfTransitions(), 20ull);
    
    storm::modelchecker::SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<double>> checker(*dtmc, std::make_unique<storm::solver::NativeLinearEquationSolverFactory<double>>());
    
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("P=? [F \"one\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult1 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(1.0/6.0, quantitativeResult1[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [F \"two\"]");
    
    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult2 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(1.0/6.0, quantitativeResult2[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [F \"three\"]");
    
    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult3 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(1.0/6.0, quantitativeResult3[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("R=? [F \"done\"]");
    
    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult4 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(3.6666650772094727, quantitativeResult4[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
}

TEST(NativeDtmcPrctlModelCheckerTest, Crowds) {
    std::shared_ptr<storm::models::sparse::Model<double>> abstractModel = storm::parser::AutoParser<>::parseModel(STORM_TEST_RESOURCES_DIR "/tra/crowds5_5.tra", STORM_TEST_RESOURCES_DIR "/lab/crowds5_5.lab", "", "");
    
    ASSERT_EQ(abstractModel->getType(), storm::models::ModelType::Dtmc);
    
    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser formulaParser;
    
    std::shared_ptr<storm::models::sparse::Dtmc<double>> dtmc = abstractModel->as<storm::models::sparse::Dtmc<double>>();
    
    ASSERT_EQ(8607ull, dtmc->getNumberOfStates());
    ASSERT_EQ(15113ull, dtmc->getNumberOfTransitions());
    
    storm::modelchecker::SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<double>> checker(*dtmc, std::make_unique<storm::solver::NativeLinearEquationSolverFactory<double>>());
    
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("P=? [F \"observe0Greater1\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult1 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.33288205191646525, quantitativeResult1[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [F \"observeIGreater1\"]");
    
    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult2 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.15222066094730619, quantitativeResult2[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [F \"observeOnlyTrueSender\"]");
    
    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult3 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.32153900158185761, quantitativeResult3[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
}

TEST(NativeDtmcPrctlModelCheckerTest, SynchronousLeader) {
    std::shared_ptr<storm::models::sparse::Model<double>> abstractModel = storm::parser::AutoParser<>::parseModel(STORM_TEST_RESOURCES_DIR "/tra/leader4_8.tra", STORM_TEST_RESOURCES_DIR "/lab/leader4_8.lab", "", STORM_TEST_RESOURCES_DIR "/rew/leader4_8.pick.trans.rew");
    
    ASSERT_EQ(abstractModel->getType(), storm::models::ModelType::Dtmc);
    
    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser formulaParser;
    
    std::shared_ptr<storm::models::sparse::Dtmc<double>> dtmc = abstractModel->as<storm::models::sparse::Dtmc<double>>();
    
    ASSERT_EQ(12400ull, dtmc->getNumberOfStates());
    ASSERT_EQ(16495ull, dtmc->getNumberOfTransitions());
    
    storm::modelchecker::SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<double>> checker(*dtmc, std::make_unique<storm::solver::NativeLinearEquationSolverFactory<double>>());
    
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("P=? [F \"elected\"]");
    
    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult1 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(1.0, quantitativeResult1[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("P=? [F<=20 \"elected\"]");
    
    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult2 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(0.9999965911265462636, quantitativeResult2[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    formula = formulaParser.parseSingleFormulaFromString("R=? [F \"elected\"]");
    
    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult3 = result->asExplicitQuantitativeCheckResult<double>();
    
    EXPECT_NEAR(1.0448979589010925, quantitativeResult3[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
}

TEST(NativeDtmcPrctlModelCheckerTest, LRASingleBscc) {
	storm::storage::SparseMatrixBuilder<double> matrixBuilder;
	std::shared_ptr<storm::models::sparse::Dtmc<double>> dtmc;

    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser formulaParser;
    
	{
		matrixBuilder = storm::storage::SparseMatrixBuilder<double>(2, 2, 2);
		matrixBuilder.addNextValue(0, 1, 1.);
		matrixBuilder.addNextValue(1, 0, 1.);
		storm::storage::SparseMatrix<double> transitionMatrix = matrixBuilder.build();

		storm::models::sparse::StateLabeling ap(2);
		ap.addLabel("a");
		ap.addLabelToState("a", 1);

		dtmc.reset(new storm::models::sparse::Dtmc<double>(transitionMatrix, ap));

        auto factory = std::make_unique<storm::solver::NativeLinearEquationSolverFactory<double>>();
        factory->getSettings().setSolutionMethod(storm::solver::NativeLinearEquationSolverSettings<double>::SolutionMethod::SOR);
        factory->getSettings().setOmega(0.9);
        storm::modelchecker::SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<double>> checker(*dtmc, std::move(factory));

        std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("LRA=? [\"a\"]");
        
        std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
            storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult1 = result->asExplicitQuantitativeCheckResult<double>();

		EXPECT_NEAR(.5, quantitativeResult1[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
		EXPECT_NEAR(.5, quantitativeResult1[1], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
	}
	{
		matrixBuilder = storm::storage::SparseMatrixBuilder<double>(2, 2, 4);
		matrixBuilder.addNextValue(0, 0, .5);
		matrixBuilder.addNextValue(0, 1, .5);
		matrixBuilder.addNextValue(1, 0, .5);
		matrixBuilder.addNextValue(1, 1, .5);
		storm::storage::SparseMatrix<double> transitionMatrix = matrixBuilder.build();

		storm::models::sparse::StateLabeling ap(2);
		ap.addLabel("a");
		ap.addLabelToState("a", 1);

		dtmc.reset(new storm::models::sparse::Dtmc<double>(transitionMatrix, ap));

        auto factory = std::make_unique<storm::solver::NativeLinearEquationSolverFactory<double>>();
        factory->getSettings().setSolutionMethod(storm::solver::NativeLinearEquationSolverSettings<double>::SolutionMethod::SOR);
        factory->getSettings().setOmega(0.9);
        storm::modelchecker::SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<double>> checker(*dtmc, std::move(factory));

        std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("LRA=? [\"a\"]");
        
        std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
        storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult1 = result->asExplicitQuantitativeCheckResult<double>();

		EXPECT_NEAR(.5, quantitativeResult1[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
		EXPECT_NEAR(.5, quantitativeResult1[1], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
	}

	{
		matrixBuilder = storm::storage::SparseMatrixBuilder<double>(3, 3, 3);
		matrixBuilder.addNextValue(0, 1, 1);
		matrixBuilder.addNextValue(1, 2, 1);
		matrixBuilder.addNextValue(2, 0, 1);
		storm::storage::SparseMatrix<double> transitionMatrix = matrixBuilder.build();

		storm::models::sparse::StateLabeling ap(3);
		ap.addLabel("a");
		ap.addLabelToState("a", 2);

		dtmc.reset(new storm::models::sparse::Dtmc<double>(transitionMatrix, ap));

        auto factory = std::make_unique<storm::solver::NativeLinearEquationSolverFactory<double>>();
        factory->getSettings().setSolutionMethod(storm::solver::NativeLinearEquationSolverSettings<double>::SolutionMethod::WalkerChae);
        storm::modelchecker::SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<double>> checker(*dtmc, std::move(factory));

        std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("LRA=? [\"a\"]");
        
        std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
        storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult1 = result->asExplicitQuantitativeCheckResult<double>();

		EXPECT_NEAR(1. / 3., quantitativeResult1[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
		EXPECT_NEAR(1. / 3., quantitativeResult1[1], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
		EXPECT_NEAR(1. / 3., quantitativeResult1[2], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
	}
}

TEST(NativeDtmcPrctlModelCheckerTest, LRA) {
	storm::storage::SparseMatrixBuilder<double> matrixBuilder;
    std::shared_ptr<storm::models::sparse::Dtmc<double>> dtmc;

    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser formulaParser;
    
	{
		matrixBuilder = storm::storage::SparseMatrixBuilder<double>(15, 15, 20, true);
		matrixBuilder.addNextValue(0, 1, 1);
		matrixBuilder.addNextValue(1, 4, 0.7);
		matrixBuilder.addNextValue(1, 6, 0.3);
		matrixBuilder.addNextValue(2, 0, 1);

		matrixBuilder.addNextValue(3, 5, 0.8);
		matrixBuilder.addNextValue(3, 9, 0.2);
		matrixBuilder.addNextValue(4, 3, 1);
		matrixBuilder.addNextValue(5, 3, 1);

		matrixBuilder.addNextValue(6, 7, 1);
		matrixBuilder.addNextValue(7, 8, 1);
		matrixBuilder.addNextValue(8, 6, 1);

		matrixBuilder.addNextValue(9, 10, 1);
		matrixBuilder.addNextValue(10, 9, 1);
		matrixBuilder.addNextValue(11, 9, 1);

		matrixBuilder.addNextValue(12, 5, 0.4);
		matrixBuilder.addNextValue(12, 8, 0.3);
		matrixBuilder.addNextValue(12, 11, 0.3);

		matrixBuilder.addNextValue(13, 7, 0.7);
		matrixBuilder.addNextValue(13, 12, 0.3);

		matrixBuilder.addNextValue(14, 12, 1);

		storm::storage::SparseMatrix<double> transitionMatrix = matrixBuilder.build();

		storm::models::sparse::StateLabeling ap(15);
		ap.addLabel("a");
		ap.addLabelToState("a", 1);
		ap.addLabelToState("a", 4);
		ap.addLabelToState("a", 5);
		ap.addLabelToState("a", 7);
		ap.addLabelToState("a", 11);
		ap.addLabelToState("a", 13);
		ap.addLabelToState("a", 14);

		dtmc.reset(new storm::models::sparse::Dtmc<double>(transitionMatrix, ap));

        auto factory = std::make_unique<storm::solver::NativeLinearEquationSolverFactory<double>>();
        factory->getSettings().setSolutionMethod(storm::solver::NativeLinearEquationSolverSettings<double>::SolutionMethod::WalkerChae);
        factory->getSettings().setPrecision(1e-7);
        factory->getSettings().setMaximalNumberOfIterations(50000);
        storm::modelchecker::SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<double>> checker(*dtmc, std::move(factory));

        std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("LRA=? [\"a\"]");
        
        std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
        storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult1 = result->asExplicitQuantitativeCheckResult<double>();

		EXPECT_NEAR(0.3 / 3., quantitativeResult1[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
		EXPECT_NEAR(0.0, quantitativeResult1[3], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
		EXPECT_NEAR(1. / 3., quantitativeResult1[6], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
		EXPECT_NEAR(0.0, quantitativeResult1[9], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
		EXPECT_NEAR(0.3/3., quantitativeResult1[12], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
		EXPECT_NEAR(.79 / 3., quantitativeResult1[13], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
		EXPECT_NEAR(0.3 / 3., quantitativeResult1[14], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
	}
}
