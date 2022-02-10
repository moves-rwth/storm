#pragma once

#include "storm/environment/SubEnvironment.h"

namespace storm {

// Forward declare sub-environments
class SolverEnvironment;
class ModelCheckerEnvironment;

// Avoid implementing ugly copy constructors for environment by using an internal environment.
struct InternalEnvironment {
    SubEnvironment<SolverEnvironment> solverEnvironment;
    SubEnvironment<ModelCheckerEnvironment> modelcheckerEnvironment;
};

class Environment {
   public:
    Environment();
    virtual ~Environment();
    Environment(Environment const& other);
    Environment& operator=(Environment const& other);

    SolverEnvironment& solver();
    SolverEnvironment const& solver() const;
    ModelCheckerEnvironment& modelchecker();
    ModelCheckerEnvironment const& modelchecker() const;

   private:
    SubEnvironment<InternalEnvironment> internalEnv;
};
}  // namespace storm
