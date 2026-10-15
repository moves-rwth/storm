#include "environment.h"

#include "storm/environment/Environment.h"
#include "storm/environment/solver/AllSolverEnvironments.h"
#include "storm/environment/solver/SolverEnvironment.h"

namespace storm::utility::environment {

storm::Environment EnvironmentFactory::create() const {
    return storm::Environment();
}

std::unique_ptr<EnvironmentFactory> EnvironmentFactory::clone() const {
    return std::make_unique<EnvironmentFactory>(*this);
}

storm::Environment SoundEnvironmentFactory::create() const {
    storm::Environment env = EnvironmentFactory::create();
    storm::SolverEnvironment& solver = env.solver();
    solver.setForceSoundness(true);  // The same as --sound from the CLI

    // The linear equation solver type and the min-max method default to topological, which is sound and thus kept.
    // Instead, the solvers underlying the topological solver are set.
    solver.topological().setUnderlyingEquationSolverType(storm::solver::EquationSolverType::Native, true);
    solver.topological().setUnderlyingMinMaxMethod(storm::solver::MinMaxMethod::OptimisticValueIteration, true);
    solver.native().setMethod(storm::solver::NativeLinearEquationSolverMethod::OptimisticValueIteration, true);
    solver.eigen().setMethod(storm::solver::EigenLinearEquationSolverMethod::SparseLU, true);
    solver.game().setMethod(storm::solver::GameMethod::PolicyIteration, true);
    solver.lra().setDetLraMethod(storm::solver::LraMethod::ValueIteration, true);
    solver.lra().setNondetLraMethod(storm::solver::LraMethod::ValueIteration, true);
    return env;
}

std::unique_ptr<EnvironmentFactory> SoundEnvironmentFactory::clone() const {
    return std::make_unique<SoundEnvironmentFactory>(*this);
}

storm::Environment getSoundEnvironment() {
    return SoundEnvironmentFactory().create();
}

}  // namespace storm::utility::environment
