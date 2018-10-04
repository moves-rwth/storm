#include "gtest/gtest.h"
#include "storm-config.h"

#include "storm-parsers/parser/FormulaParser.h"
#include "storm/logic/Formulas.h"
#include "storm/solver/StandardMinMaxLinearEquationSolver.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/settings/SettingsManager.h"

#include "storm/settings/modules/GeneralSettings.h"

#include "storm/settings/modules/NativeEquationSolverSettings.h"
#include "storm-parsers/parser/AutoParser.h"

TEST(LraMdpPrctlModelCheckerTest, LRA_SingleMec) {
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

        storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>> checker(*mdp);

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

        storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>> checker(*mdp);

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

        storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>> checker(*mdp);

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

TEST(LraMdpPrctlModelCheckerTest, LRA) {
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

        storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>> checker(*mdp);

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

        storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>> checker(*mdp);

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
