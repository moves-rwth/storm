#include "storm/environment/Environment.h"
#include "storm/environment/solver/SolverEnvironment.h"
#include "storm/environment/solver/MinMaxSolverEnvironment.h"

namespace storm {


    Environment::Environment() : solverEnvironment(std::make_unique<SolverEnvironment>()) {
        // Intentionally left empty.
    }
    
    Environment::Environment(Environment const& other) :
            solverEnvironment(new SolverEnvironment(*other.solverEnvironment)) {
        // Intentionally left empty.
    }

    Environment::~Environment() {
        // Intentionally left empty.
    }
    
    SolverEnvironment& Environment::solver() {
        return *solverEnvironment;
    }
    
    SolverEnvironment const& Environment::solver() const {
        return *solverEnvironment;
    }
}