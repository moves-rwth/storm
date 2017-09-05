#include "gtest/gtest.h"
#include "storm-config.h"

#include "storm/parser/FormulaParser.h"
#include "storm/logic/Formulas.h"
#include "storm/solver/StandardMinMaxLinearEquationSolver.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/settings/SettingsManager.h"

#include "storm/settings/modules/GeneralSettings.h"

#include "storm/settings/modules/NativeEquationSolverSettings.h"
#include "storm/parser/AutoParser.h"

TEST(SparseMdpPrctlModelCheckerTest, Dice) {
    std::shared_ptr<storm::models::sparse::Model<double>> abstractModel = storm::parser::AutoParser<>::parseModel(STORM_TEST_RESOURCES_DIR "/tra/two_dice.tra", STORM_TEST_RESOURCES_DIR "/lab/two_dice.lab", "", STORM_TEST_RESOURCES_DIR "/rew/two_dice.flip.trans.rew");

    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser formulaParser;

    ASSERT_EQ(abstractModel->getType(), storm::models::ModelType::Mdp);

    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = abstractModel->as<storm::models::sparse::Mdp<double>>();

    ASSERT_EQ(mdp->getNumberOfStates(), 169ull);
    ASSERT_EQ(mdp->getNumberOfTransitions(), 436ull);

    storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>> checker(*mdp, std::make_unique<storm::solver::NativeMinMaxLinearEquationSolverFactory<double>>());

    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F \"two\"]");

    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult1 = result->asExplicitQuantitativeCheckResult<double>();

    EXPECT_NEAR(0.0277777612209320068, quantitativeResult1[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("Pmax=? [F \"two\"]");

    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult2 = result->asExplicitQuantitativeCheckResult<double>();

    EXPECT_NEAR(0.0277777612209320068, quantitativeResult2[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F \"three\"]");

    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult3 = result->asExplicitQuantitativeCheckResult<double>();

    EXPECT_NEAR(0.0555555224418640136, quantitativeResult3[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("Pmax=? [F \"three\"]");

    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult4 = result->asExplicitQuantitativeCheckResult<double>();

    EXPECT_NEAR(0.0555555224418640136, quantitativeResult4[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F \"four\"]");

    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult5 = result->asExplicitQuantitativeCheckResult<double>();

    EXPECT_NEAR(0.083333283662796020508, quantitativeResult5[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("Pmax=? [F \"four\"]");

    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult6 = result->asExplicitQuantitativeCheckResult<double>();

    EXPECT_NEAR(0.083333283662796020508, quantitativeResult6[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("Rmin=? [F \"done\"]");

    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult7 = result->asExplicitQuantitativeCheckResult<double>();

    EXPECT_NEAR(7.3333317041397095, quantitativeResult7[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("Rmax=? [F \"done\"]");

    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult8 = result->asExplicitQuantitativeCheckResult<double>();

    EXPECT_NEAR(7.333329499, quantitativeResult8[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

    abstractModel = storm::parser::AutoParser<>::parseModel(STORM_TEST_RESOURCES_DIR "/tra/two_dice.tra", STORM_TEST_RESOURCES_DIR "/lab/two_dice.lab", STORM_TEST_RESOURCES_DIR "/rew/two_dice.flip.state.rew", "");

    ASSERT_EQ(abstractModel->getType(), storm::models::ModelType::Mdp);

    std::shared_ptr<storm::models::sparse::Mdp<double>> stateRewardMdp = abstractModel->as<storm::models::sparse::Mdp<double>>();

    storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>> stateRewardModelChecker(*stateRewardMdp, std::make_unique<storm::solver::NativeMinMaxLinearEquationSolverFactory<double>>());

    formula = formulaParser.parseSingleFormulaFromString("Rmin=? [F \"done\"]");

    result = stateRewardModelChecker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult9 = result->asExplicitQuantitativeCheckResult<double>();

    EXPECT_NEAR(7.3333317041397095, quantitativeResult9[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("Rmax=? [F \"done\"]");

    result = stateRewardModelChecker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult10 = result->asExplicitQuantitativeCheckResult<double>();

    EXPECT_NEAR(7.333329499, quantitativeResult10[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

    abstractModel = storm::parser::AutoParser<>::parseModel(STORM_TEST_RESOURCES_DIR "/tra/two_dice.tra", STORM_TEST_RESOURCES_DIR "/lab/two_dice.lab", STORM_TEST_RESOURCES_DIR "/rew/two_dice.flip.state.rew", STORM_TEST_RESOURCES_DIR "/rew/two_dice.flip.trans.rew");

    ASSERT_EQ(abstractModel->getType(), storm::models::ModelType::Mdp);

    std::shared_ptr<storm::models::sparse::Mdp<double>> stateAndTransitionRewardMdp = abstractModel->as<storm::models::sparse::Mdp<double>>();

    storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>> stateAndTransitionRewardModelChecker(*stateAndTransitionRewardMdp, std::make_unique<storm::solver::NativeMinMaxLinearEquationSolverFactory<double>>());

    formula = formulaParser.parseSingleFormulaFromString("Rmin=? [F \"done\"]");

    result = stateAndTransitionRewardModelChecker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult11 = result->asExplicitQuantitativeCheckResult<double>();

    EXPECT_NEAR(14.666663408279419, quantitativeResult11[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("Rmax=? [F \"done\"]");

    result = stateAndTransitionRewardModelChecker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult12 = result->asExplicitQuantitativeCheckResult<double>();

    EXPECT_NEAR(14.666658998, quantitativeResult12[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
}

TEST(SparseMdpPrctlModelCheckerTest, AsynchronousLeader) {
    std::shared_ptr<storm::models::sparse::Model<double>> abstractModel = storm::parser::AutoParser<>::parseModel(STORM_TEST_RESOURCES_DIR "/tra/leader4.tra", STORM_TEST_RESOURCES_DIR "/lab/leader4.lab", "", STORM_TEST_RESOURCES_DIR "/rew/leader4.trans.rew");

    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser formulaParser;

    ASSERT_EQ(storm::models::ModelType::Mdp, abstractModel->getType());

    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = abstractModel->as<storm::models::sparse::Mdp<double>>();

    ASSERT_EQ(3172ull, mdp->getNumberOfStates());
    ASSERT_EQ(7144ull, mdp->getNumberOfTransitions());

    storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>> checker(*mdp, std::make_unique<storm::solver::NativeMinMaxLinearEquationSolverFactory<double>>());

    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F \"elected\"]");

    std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult1 = result->asExplicitQuantitativeCheckResult<double>();

    EXPECT_NEAR(1, quantitativeResult1[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("Pmax=? [F \"elected\"]");

    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult2 = result->asExplicitQuantitativeCheckResult<double>();

    EXPECT_NEAR(1, quantitativeResult2[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F<=25 \"elected\"]");

    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult3 = result->asExplicitQuantitativeCheckResult<double>();

    EXPECT_NEAR(0.0625, quantitativeResult3[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("Pmax=? [F<=25 \"elected\"]");

    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult4 = result->asExplicitQuantitativeCheckResult<double>();

    EXPECT_NEAR(0.0625, quantitativeResult4[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("Rmin=? [F \"elected\"]");

    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult5 = result->asExplicitQuantitativeCheckResult<double>();

    EXPECT_NEAR(4.2856907116062786, quantitativeResult5[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("Rmax=? [F \"elected\"]");

    result = checker.check(*formula);
    storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult6 = result->asExplicitQuantitativeCheckResult<double>();

    EXPECT_NEAR(4.285689611, quantitativeResult6[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
}

TEST(SparseMdpPrctlModelCheckerTest, LRA_SingleMec) {
    storm::storage::SparseMatrixBuilder<double> matrixBuilder;
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp;

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

        mdp.reset(new storm::models::sparse::Mdp<double>(transitionMatrix, ap));

        storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>> checker(*mdp, std::make_unique<storm::solver::NativeMinMaxLinearEquationSolverFactory<double>>());

        std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("LRAmax=? [\"a\"]");

        std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
        storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult1 = result->asExplicitQuantitativeCheckResult<double>();

        EXPECT_NEAR(.5, quantitativeResult1[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(.5, quantitativeResult1[1], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

        formula = formulaParser.parseSingleFormulaFromString("LRAmin=? [\"a\"]");

        result = checker.check(*formula);
        storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult2 = result->asExplicitQuantitativeCheckResult<double>();

        EXPECT_NEAR(.5, quantitativeResult2[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(.5, quantitativeResult2[1], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
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

        mdp.reset(new storm::models::sparse::Mdp<double>(transitionMatrix, ap));

        storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>> checker(*mdp, std::make_unique<storm::solver::NativeMinMaxLinearEquationSolverFactory<double>>());

        std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("LRAmax=? [\"a\"]");

        std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
        storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult1 = result->asExplicitQuantitativeCheckResult<double>();

        EXPECT_NEAR(.5, quantitativeResult1[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(.5, quantitativeResult1[1], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

        formula = formulaParser.parseSingleFormulaFromString("LRAmin=? [\"a\"]");

        result = checker.check(*formula);
        storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult2 = result->asExplicitQuantitativeCheckResult<double>();

        EXPECT_NEAR(.5, quantitativeResult2[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(.5, quantitativeResult2[1], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    }

    {
        matrixBuilder = storm::storage::SparseMatrixBuilder<double>(4, 3, 4, true, true, 3);
        matrixBuilder.newRowGroup(0);
        matrixBuilder.addNextValue(0, 1, 1);
        matrixBuilder.newRowGroup(1);
        matrixBuilder.addNextValue(1, 0, 1);
        matrixBuilder.addNextValue(2, 2, 1);
        matrixBuilder.newRowGroup(3);
        matrixBuilder.addNextValue(3, 0, 1);
        storm::storage::SparseMatrix<double> transitionMatrix = matrixBuilder.build();

        storm::models::sparse::StateLabeling ap(3);
        ap.addLabel("a");
        ap.addLabelToState("a", 2);
        ap.addLabel("b");
        ap.addLabelToState("b", 0);
        ap.addLabel("c");
        ap.addLabelToState("c", 0);
        ap.addLabelToState("c", 2);

        mdp.reset(new storm::models::sparse::Mdp<double>(transitionMatrix, ap));

        storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>> checker(*mdp, std::make_unique<storm::solver::NativeMinMaxLinearEquationSolverFactory<double>>());

        std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("LRAmax=? [\"a\"]");

        std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
        storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult1 = result->asExplicitQuantitativeCheckResult<double>();

        EXPECT_NEAR(1. / 3., quantitativeResult1[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(1. / 3., quantitativeResult1[1], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(1. / 3., quantitativeResult1[2], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

        formula = formulaParser.parseSingleFormulaFromString("LRAmin=? [\"a\"]");

        result = checker.check(*formula);
        storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult2 = result->asExplicitQuantitativeCheckResult<double>();

        EXPECT_NEAR(0.0, quantitativeResult2[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(0.0, quantitativeResult2[1], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(0.0, quantitativeResult2[2], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

        formula = formulaParser.parseSingleFormulaFromString("LRAmax=? [\"b\"]");

        result = checker.check(*formula);
        storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult3 = result->asExplicitQuantitativeCheckResult<double>();

        EXPECT_NEAR(0.5, quantitativeResult3[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(0.5, quantitativeResult3[1], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(0.5, quantitativeResult3[2], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

        formula = formulaParser.parseSingleFormulaFromString("LRAmin=? [\"b\"]");

        result = checker.check(*formula);
        storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult4 = result->asExplicitQuantitativeCheckResult<double>();

        EXPECT_NEAR(1. / 3., quantitativeResult4[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(1. / 3., quantitativeResult4[1], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(1. / 3., quantitativeResult4[2], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

        formula = formulaParser.parseSingleFormulaFromString("LRAmax=? [\"c\"]");

        result = checker.check(*formula);
        storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult5 = result->asExplicitQuantitativeCheckResult<double>();

        EXPECT_NEAR(2. / 3., quantitativeResult5[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(2. / 3., quantitativeResult5[1], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(2. / 3., quantitativeResult5[2], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

        formula = formulaParser.parseSingleFormulaFromString("LRAmin=? [\"c\"]");

        result = checker.check(*formula);
        storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult6 = result->asExplicitQuantitativeCheckResult<double>();

        EXPECT_NEAR(0.5, quantitativeResult6[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(0.5, quantitativeResult6[1], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(0.5, quantitativeResult6[2], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    }
}

TEST(SparseMdpPrctlModelCheckerTest, LRA) {
    storm::storage::SparseMatrixBuilder<double> matrixBuilder;
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp;

    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser formulaParser;

    {
        matrixBuilder = storm::storage::SparseMatrixBuilder<double>(4, 3, 4, true, true, 3);
        matrixBuilder.newRowGroup(0);
        matrixBuilder.addNextValue(0, 1, 1);
        matrixBuilder.newRowGroup(1);
        matrixBuilder.addNextValue(1, 1, 1);
        matrixBuilder.addNextValue(2, 2, 1);
        matrixBuilder.newRowGroup(3);
        matrixBuilder.addNextValue(3, 2, 1);
        storm::storage::SparseMatrix<double> transitionMatrix = matrixBuilder.build();

        storm::models::sparse::StateLabeling ap(3);
        ap.addLabel("a");
        ap.addLabelToState("a", 0);
        ap.addLabel("b");
        ap.addLabelToState("b", 1);
        ap.addLabel("c");
        ap.addLabelToState("c", 2);

        mdp.reset(new storm::models::sparse::Mdp<double>(transitionMatrix, ap));

        storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>> checker(*mdp, std::make_unique<storm::solver::NativeMinMaxLinearEquationSolverFactory<double>>());

        std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("LRAmax=? [\"a\"]");

        std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
        storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult1 = result->asExplicitQuantitativeCheckResult<double>();

        EXPECT_NEAR(0.0, quantitativeResult1[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(0.0, quantitativeResult1[1], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(0.0, quantitativeResult1[2], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

        formula = formulaParser.parseSingleFormulaFromString("LRAmin=? [\"a\"]");

        result = checker.check(*formula);
        storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult2 = result->asExplicitQuantitativeCheckResult<double>();

        EXPECT_NEAR(0.0, quantitativeResult2[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(0.0, quantitativeResult2[1], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(0.0, quantitativeResult2[2], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

        formula = formulaParser.parseSingleFormulaFromString("LRAmax=? [\"b\"]");

        result = checker.check(*formula);
        storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult3 = result->asExplicitQuantitativeCheckResult<double>();

        EXPECT_NEAR(1.0, quantitativeResult3[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(1.0, quantitativeResult3[1], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(0.0, quantitativeResult3[2], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

        formula = formulaParser.parseSingleFormulaFromString("LRAmin=? [\"b\"]");

        result = checker.check(*formula);
        storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult4 = result->asExplicitQuantitativeCheckResult<double>();

        EXPECT_NEAR(0.0, quantitativeResult4[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(0.0, quantitativeResult4[1], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(0.0, quantitativeResult4[2], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

        formula = formulaParser.parseSingleFormulaFromString("LRAmax=? [\"c\"]");

        result = checker.check(*formula);
        storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult5 = result->asExplicitQuantitativeCheckResult<double>();

        EXPECT_NEAR(1.0, quantitativeResult5[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(1.0, quantitativeResult5[1], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(1.0, quantitativeResult5[2], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

        formula = formulaParser.parseSingleFormulaFromString("LRAmin=? [\"c\"]");

        result = checker.check(*formula);
        storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult6 = result->asExplicitQuantitativeCheckResult<double>();

        EXPECT_NEAR(0.0, quantitativeResult6[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(0.0, quantitativeResult6[1], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(1.0, quantitativeResult6[2], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    }
    {
        matrixBuilder = storm::storage::SparseMatrixBuilder<double>(22, 15, 28, true, true, 15);
        matrixBuilder.newRowGroup(0);
        matrixBuilder.addNextValue(0, 1, 1);
        matrixBuilder.newRowGroup(1);
        matrixBuilder.addNextValue(1, 0, 1);
        matrixBuilder.addNextValue(2, 2, 1);
        matrixBuilder.addNextValue(3, 4, 0.7);
        matrixBuilder.addNextValue(3, 6, 0.3);
        matrixBuilder.newRowGroup(4);
        matrixBuilder.addNextValue(4, 0, 1);

        matrixBuilder.newRowGroup(5);
        matrixBuilder.addNextValue(5, 4, 1);
        matrixBuilder.addNextValue(6, 5, 0.8);
        matrixBuilder.addNextValue(6, 9, 0.2);
        matrixBuilder.newRowGroup(7);
        matrixBuilder.addNextValue(7, 3, 1);
        matrixBuilder.addNextValue(8, 5, 1);
        matrixBuilder.newRowGroup(9);
        matrixBuilder.addNextValue(9, 3, 1);

        matrixBuilder.newRowGroup(10);
        matrixBuilder.addNextValue(10, 7, 1);
        matrixBuilder.newRowGroup(11);
        matrixBuilder.addNextValue(11, 6, 1);
        matrixBuilder.addNextValue(12, 8, 1);
        matrixBuilder.newRowGroup(13);
        matrixBuilder.addNextValue(13, 6, 1);

        matrixBuilder.newRowGroup(14);
        matrixBuilder.addNextValue(14, 10, 1);
        matrixBuilder.newRowGroup(15);
        matrixBuilder.addNextValue(15, 9, 1);
        matrixBuilder.addNextValue(16, 11, 1);
        matrixBuilder.newRowGroup(17);
        matrixBuilder.addNextValue(17, 9, 1);

        matrixBuilder.newRowGroup(18);
        matrixBuilder.addNextValue(18, 5, 0.4);
        matrixBuilder.addNextValue(18, 8, 0.3);
        matrixBuilder.addNextValue(18, 11, 0.3);

        matrixBuilder.newRowGroup(19);
        matrixBuilder.addNextValue(19, 7, 0.7);
        matrixBuilder.addNextValue(19, 12, 0.3);

        matrixBuilder.newRowGroup(20);
        matrixBuilder.addNextValue(20, 12, 0.1);
        matrixBuilder.addNextValue(20, 13, 0.9);
        matrixBuilder.addNextValue(21, 12, 1);

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

        mdp.reset(new storm::models::sparse::Mdp<double>(transitionMatrix, ap));

        storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>> checker(*mdp, std::make_unique<storm::solver::NativeMinMaxLinearEquationSolverFactory<double>>());

        std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("LRAmax=? [\"a\"]");

        std::unique_ptr<storm::modelchecker::CheckResult> result = checker.check(*formula);
        storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult1 = result->asExplicitQuantitativeCheckResult<double>();

        EXPECT_NEAR(37. / 60., quantitativeResult1[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(2. / 3., quantitativeResult1[3], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(0.5, quantitativeResult1[6], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(1. / 3., quantitativeResult1[9], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(31. / 60., quantitativeResult1[12], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(101. / 200., quantitativeResult1[13], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(31. / 60., quantitativeResult1[14], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

        formula = formulaParser.parseSingleFormulaFromString("LRAmin=? [\"a\"]");

        result = checker.check(*formula);
        storm::modelchecker::ExplicitQuantitativeCheckResult<double>& quantitativeResult2 = result->asExplicitQuantitativeCheckResult<double>();

        EXPECT_NEAR(0.3 / 3., quantitativeResult2[0], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(0.0, quantitativeResult2[3], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(1. / 3., quantitativeResult2[6], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(0.0, quantitativeResult2[9], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(0.3 / 3., quantitativeResult2[12], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(.79 / 3., quantitativeResult2[13], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
        EXPECT_NEAR(0.3 / 3., quantitativeResult2[14], storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    }
}
