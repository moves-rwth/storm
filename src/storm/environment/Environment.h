#pragma once

#include<memory>

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
    
        std::unique_ptr<SolverEnvironment> solverEnvironment;
        
        
    };
}

