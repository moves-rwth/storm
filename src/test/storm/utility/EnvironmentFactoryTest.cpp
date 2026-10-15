#include "storm-config.h"
#include "test/storm_gtest.h"

#include "storm-parsers/api/model_descriptions.h"
#include "storm-parsers/api/properties.h"
#include "storm/api/builder.h"
#include "storm/api/properties.h"
#include "storm/api/verification.h"
#include "storm/environment/Environment.h"
#include "storm/environment/solver/AllSolverEnvironments.h"
#include "storm/environment/solver/SolverEnvironment.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/models/sparse/Model.h"
#include "storm/settings/EnvironmentBuilder.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/storage/jani/Property.h"
#include "storm/storage/prism/Program.h"
#include "storm/utility/environment.h"

namespace {

double checkInitialState(storm::Environment const& env, std::string const& file, std::string const& formulaString) {
    auto program = storm::api::parseProgram(file);
    auto formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaString, program));
    auto model = storm::api::buildSparseModel<double>(program, formulas);
    auto task = storm::api::createTask<double>(formulas.front(), true);
    auto result = storm::api::verifyWithSparseEngine<double>(env, model, task);
    return result->asExplicitQuantitativeCheckResult<double>()[*model->getInitialStates().begin()];
}

}  // namespace

TEST(EnvironmentFactoryTest, DefaultEnvironmentIsNotSound) {
    storm::Environment const env = storm::utility::environment::EnvironmentFactory().create();
    EXPECT_FALSE(env.solver().isForceSoundness());
}

TEST(EnvironmentFactoryTest, SoundEnvironmentSelectsSoundMethods) {
    storm::Environment const env = storm::utility::environment::getSoundEnvironment();
    storm::SolverEnvironment const& solver = env.solver();

    EXPECT_TRUE(solver.isForceSoundness());
    EXPECT_FALSE(solver.isForceExact());

    // Defaults which are already sound remain untouched
    EXPECT_EQ(storm::solver::EquationSolverType::Topological, solver.getLinearEquationSolverType());
    EXPECT_EQ(storm::solver::MinMaxMethod::Topological, solver.minMax().getMethod());

    EXPECT_EQ(storm::solver::EquationSolverType::Native, solver.topological().getUnderlyingEquationSolverType());
    EXPECT_EQ(storm::solver::MinMaxMethod::OptimisticValueIteration, solver.topological().getUnderlyingMinMaxMethod());
    EXPECT_EQ(storm::solver::NativeLinearEquationSolverMethod::OptimisticValueIteration, solver.native().getMethod());
    EXPECT_EQ(storm::solver::EigenLinearEquationSolverMethod::SparseLU, solver.eigen().getMethod());
    EXPECT_EQ(storm::solver::GameMethod::PolicyIteration, solver.game().getMethod());
    EXPECT_EQ(storm::solver::LraMethod::ValueIteration, solver.lra().getDetLraMethod());
    EXPECT_EQ(storm::solver::LraMethod::ValueIteration, solver.lra().getNondetLraMethod());

    // The choices count as defaults, such that e.g. exact mode can still override them
    EXPECT_TRUE(solver.isLinearEquationSolverTypeSetFromDefaultValue());
    EXPECT_TRUE(solver.topological().isUnderlyingEquationSolverTypeSetFromDefault());
    EXPECT_TRUE(solver.topological().isUnderlyingMinMaxMethodSetFromDefault());
    EXPECT_TRUE(solver.native().isMethodSetFromDefault());
    EXPECT_TRUE(solver.eigen().isMethodSetFromDefault());
    EXPECT_TRUE(solver.minMax().isMethodSetFromDefault());
    EXPECT_TRUE(solver.game().isMethodSetFromDefault());
    EXPECT_TRUE(solver.lra().isDetLraMethodSetFromDefault());
    EXPECT_TRUE(solver.lra().isNondetLraMethodSetFromDefault());
}

TEST(EnvironmentFactoryTest, ClonedFactoryCreatesSoundEnvironment) {
    std::unique_ptr<storm::utility::environment::EnvironmentFactory> factory = storm::utility::environment::SoundEnvironmentFactory().clone();
    EXPECT_TRUE(factory->create().solver().isForceSoundness());
}

TEST(EnvironmentFactoryTest, MatchesSoundCommandLineOption) {
    storm::settings::mutableManager().setFromExplodedString({"--sound"});
    storm::Environment const cliEnv = storm::settings::EnvironmentBuilder::buildEnvironment();
    storm::settings::mutableManager().getModule(storm::settings::modules::GeneralSettings::moduleName).restoreDefaults();

    storm::Environment const env = storm::utility::environment::getSoundEnvironment();
    EXPECT_EQ(cliEnv.solver().isForceSoundness(), env.solver().isForceSoundness());

    double const precision = 1e-6;
    std::string const dtmcFile = STORM_TEST_RESOURCES_DIR "/dtmc/die.pm";
    std::string const dtmcFormula = "P=? [F \"one\"]";
    EXPECT_NEAR(1.0 / 6.0, checkInitialState(env, dtmcFile, dtmcFormula), precision);
    EXPECT_NEAR(checkInitialState(cliEnv, dtmcFile, dtmcFormula), checkInitialState(env, dtmcFile, dtmcFormula), precision);

    std::string const mdpFile = STORM_TEST_RESOURCES_DIR "/mdp/two_dice.nm";
    std::string const mdpFormula = "Pmin=? [F \"two\"]";
    EXPECT_NEAR(1.0 / 36.0, checkInitialState(env, mdpFile, mdpFormula), precision);
    EXPECT_NEAR(checkInitialState(cliEnv, mdpFile, mdpFormula), checkInitialState(env, mdpFile, mdpFormula), precision);
}
