#include "storm/environment/Environment.h"
#include "storm/environment/SubEnvironment.h"
#include "storm/environment/solver/SolverEnvironment.h"
namespace storm {


    Environment::Environment() {
        // Intentionally left empty.
    }
    
    Environment::~Environment() {
        // Intentionally left empty.
    }
    
    SolverEnvironment& Environment::solver() {
        return solverEnvironment.get();
    }
    
    SolverEnvironment const& Environment::solver() const {
        return solverEnvironment.get();
    }
}