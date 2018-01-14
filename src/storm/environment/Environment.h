#pragma once

#include "storm/environment/SubEnvironment.h"

namespace storm {
    
    // Forward declare sub-environments
    class SolverEnvironment;
    
    class Environment {
    public:
        
        Environment();
        virtual ~Environment();

        SolverEnvironment& solver();
        SolverEnvironment const& solver() const;
        
    private:
    
        SubEnvironment<SolverEnvironment> solverEnvironment;
    };
}

