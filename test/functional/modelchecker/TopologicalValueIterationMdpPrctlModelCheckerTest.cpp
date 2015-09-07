#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/parser/FormulaParser.h"
#include "src/logic/Formulas.h"
#include "src/utility/solver.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "src/settings/SettingsManager.h"

#include "src/settings/modules/GeneralSettings.h"
#include "src/settings/modules/TopologicalValueIterationEquationSolverSettings.h"
#include "src/settings/SettingMemento.h"
#include "src/parser/AutoParser.h"

#include "storm-config.h"

TEST(TopologicalValueIterationMdpPrctlModelCheckerTest, Dice) {
    //storm::settings::Settings* s = storm::settings::Settings::getInstance();    
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = storm::parser::AutoParser<>::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.tra", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.lab", "", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.trans.rew")->as<storm::models::sparse::Mdp<double>>();

    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser formulaParser;

    ASSERT_EQ(mdp->getNumberOfStates(), 169ull);
    ASSERT_EQ(mdp->getNumberOfTransitions(), 436ull);

    storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>> mc(*mdp, std::unique_ptr<storm::utility::solver::MinMaxLinearEquationSolverFactory<double>>(new storm::utility::solver::MinMaxLinearEquationSolverFactory<double>(storm::solver::EquationSolverTypeSelection::Topological)));

    std::shared_ptr<storm::logic::Formula> formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F \"two\"]");

    std::unique_ptr<storm::modelchecker::CheckResult> result = mc.check(*formula);

    ASSERT_NEAR(0.0277777612209320068, result->asExplicitQuantitativeCheckResult<double>()[0], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("Pmax=? [F \"two\"]");

    result = mc.check(*formula);

    ASSERT_NEAR(0.0277777612209320068, result->asExplicitQuantitativeCheckResult<double>()[0], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F \"three\"]");

    result = mc.check(*formula);

    ASSERT_NEAR(0.0555555224418640136, result->asExplicitQuantitativeCheckResult<double>()[0], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("Pmax=? [F \"three\"]");

    result = mc.check(*formula);

    ASSERT_NEAR(0.0555555224418640136, result->asExplicitQuantitativeCheckResult<double>()[0], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F \"four\"]");

    result = mc.check(*formula);

    ASSERT_NEAR(0.083333283662796020508, result->asExplicitQuantitativeCheckResult<double>()[0], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("Pmax=? [F \"four\"]");

    result = mc.check(*formula);

    ASSERT_NEAR(0.083333283662796020508, result->asExplicitQuantitativeCheckResult<double>()[0], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("Rmin=? [F \"done\"]");

    result = mc.check(*formula);

#ifdef STORM_HAVE_CUDA
    ASSERT_NEAR(7.333329499, result->asExplicitQuantitativeCheckResult<double>()[0], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
#else
    ASSERT_NEAR(7.33332904, result->asExplicitQuantitativeCheckResult<double>()[0], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
#endif

    formula = formulaParser.parseSingleFormulaFromString("Rmax=? [F \"done\"]");

    result = mc.check(*formula);

#ifdef STORM_HAVE_CUDA
    ASSERT_NEAR(7.333329499, result->asExplicitQuantitativeCheckResult<double>()[0], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
#else
    ASSERT_NEAR(7.33333151, result->asExplicitQuantitativeCheckResult<double>()[0], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
#endif

    // ------------- state rewards --------------
    std::shared_ptr<storm::models::sparse::Mdp<double>> stateRewardMdp = storm::parser::AutoParser<>::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.tra", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.lab", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.state.rew", "")->as<storm::models::sparse::Mdp<double>>();

    storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>> stateRewardModelChecker(*stateRewardMdp, std::unique_ptr<storm::utility::solver::MinMaxLinearEquationSolverFactory<double>>(new storm::utility::solver::MinMaxLinearEquationSolverFactory<double>(storm::solver::EquationSolverTypeSelection::Topological)));

    formula = formulaParser.parseSingleFormulaFromString("Rmin=? [F \"done\"]");

    result = stateRewardModelChecker.check(*formula);

#ifdef STORM_HAVE_CUDA
    ASSERT_NEAR(7.333329499, result->asExplicitQuantitativeCheckResult<double>()[0], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
#else
    ASSERT_NEAR(7.33332904, result->asExplicitQuantitativeCheckResult<double>()[0], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
#endif

    formula = formulaParser.parseSingleFormulaFromString("Rmax=? [F \"done\"]");

    result = stateRewardModelChecker.check(*formula);

#ifdef STORM_HAVE_CUDA
    ASSERT_NEAR(7.333329499, result->asExplicitQuantitativeCheckResult<double>()[0], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
#else
    ASSERT_NEAR(7.33333151, result->asExplicitQuantitativeCheckResult<double>()[0], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
#endif

    // -------------------------------- state and transition reward ------------------------
    std::shared_ptr<storm::models::sparse::Mdp<double>> stateAndTransitionRewardMdp = storm::parser::AutoParser<>::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.tra", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.lab", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.state.rew", STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.flip.trans.rew")->as<storm::models::sparse::Mdp<double>>();

    storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>> stateAndTransitionRewardModelChecker(*stateAndTransitionRewardMdp, std::unique_ptr<storm::utility::solver::MinMaxLinearEquationSolverFactory<double>>(new storm::utility::solver::MinMaxLinearEquationSolverFactory<double>(storm::solver::EquationSolverTypeSelection::Topological)));

    formula = formulaParser.parseSingleFormulaFromString("Rmin=? [F \"done\"]");

    result = stateAndTransitionRewardModelChecker.check(*formula);

#ifdef STORM_HAVE_CUDA
    ASSERT_NEAR(14.666658998, result->asExplicitQuantitativeCheckResult<double>()[0], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
#else
    ASSERT_NEAR(14.6666581, result->asExplicitQuantitativeCheckResult<double>()[0], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
#endif
    formula = formulaParser.parseSingleFormulaFromString("Rmax=? [F \"done\"]");

    result = stateAndTransitionRewardModelChecker.check(*formula);

#ifdef STORM_HAVE_CUDA
    ASSERT_NEAR(14.666658998, result->asExplicitQuantitativeCheckResult<double>()[0], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
#else
    ASSERT_NEAR(14.666663, result->asExplicitQuantitativeCheckResult<double>()[0], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
#endif
}

TEST(TopologicalValueIterationMdpPrctlModelCheckerTest, AsynchronousLeader) {
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = storm::parser::AutoParser<>::parseModel(STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader4.tra", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader4.lab", "", STORM_CPP_BASE_PATH "/examples/mdp/asynchronous_leader/leader4.trans.rew")->as<storm::models::sparse::Mdp<double>>();

    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser formulaParser;

    ASSERT_EQ(mdp->getNumberOfStates(), 3172ull);
    ASSERT_EQ(mdp->getNumberOfTransitions(), 7144ull);

    storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<double>> mc(*mdp, std::unique_ptr<storm::utility::solver::MinMaxLinearEquationSolverFactory<double>>(new storm::utility::solver::MinMaxLinearEquationSolverFactory<double>(storm::solver::EquationSolverTypeSelection::Topological)));

    std::shared_ptr<storm::logic::Formula> formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F \"elected\"]");

    std::unique_ptr<storm::modelchecker::CheckResult> result = mc.check(*formula);

    ASSERT_LT(std::abs(result->asExplicitQuantitativeCheckResult<double>()[0] - 1),
            storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("Pmax=? [F \"elected\"]");

    result = mc.check(*formula);

    ASSERT_NEAR(1, result->asExplicitQuantitativeCheckResult<double>()[0], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("Pmax=? [F<=25 \"elected\"]");

    result = mc.check(*formula);

    ASSERT_NEAR(0.0625, result->asExplicitQuantitativeCheckResult<double>()[0], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F<=25 \"elected\"]");

    result = mc.check(*formula);

    ASSERT_NEAR(0.0625, result->asExplicitQuantitativeCheckResult<double>()[0], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());

    formula = formulaParser.parseSingleFormulaFromString("Rmin=? [F \"elected\"]");

    result = mc.check(*formula);

#ifdef STORM_HAVE_CUDA
    ASSERT_NEAR(4.285689611, result->asExplicitQuantitativeCheckResult<double>()[0], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
#else
    ASSERT_NEAR(4.285701547, result->asExplicitQuantitativeCheckResult<double>()[0], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
#endif

    formula = formulaParser.parseSingleFormulaFromString("Rmax=? [F \"elected\"]");

    result = mc.check(*formula);

#ifdef STORM_HAVE_CUDA
    ASSERT_NEAR(4.285689611, result->asExplicitQuantitativeCheckResult<double>()[0], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
#else
    ASSERT_NEAR(4.285703591, result->asExplicitQuantitativeCheckResult<double>()[0], storm::settings::topologicalValueIterationEquationSolverSettings().getPrecision());
#endif
}
