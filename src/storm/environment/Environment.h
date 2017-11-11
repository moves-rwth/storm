#pragma once

#include<memory>

namespace storm {
    
    
    // Forward declare sub-environments
    class SolverEnvironment;
    
    class Environment {
    public:
        
        Environment();
        Environment(Environment const& other);
        
        virtual ~Environment();

        SolverEnvironment& solver();
        SolverEnvironment const& solver() const;
        
    private:
    
        std::unique_ptr<SolverEnvironment> solverEnvironment;
        
        
    };
}

