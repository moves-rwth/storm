#include "storm/environment/Environment.h"
#include "storm/environment/SubEnvironment.h"
#include "storm/environment/modelchecker/ModelCheckerEnvironment.h"
#include "storm/environment/solver/SolverEnvironment.h"

namespace storm {

Environment::Environment() {
    // Intentionally left empty.
}

Environment::~Environment() {
    // Intentionally left empty.
}

Environment::Environment(Environment const& other) : internalEnv(other.internalEnv) {
    // Intentionally left empty.
}

Environment& Environment::operator=(Environment const& other) {
    internalEnv = other.internalEnv;
    return *this;
}

SolverEnvironment& Environment::solver() {
    return internalEnv.get().solverEnvironment.get();
}

SolverEnvironment const& Environment::solver() const {
    return internalEnv.get().solverEnvironment.get();
}

ModelCheckerEnvironment& Environment::modelchecker() {
    return internalEnv.get().modelcheckerEnvironment.get();
}

ModelCheckerEnvironment const& Environment::modelchecker() const {
    return internalEnv.get().modelcheckerEnvironment.get();
}
}  // namespace storm