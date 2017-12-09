#pragma once

#include "storm/environment/solver/SolverEnvironment.h"

#include "storm/solver/SolverSelectionOptions.h"

namespace storm {
    
    class TopologicalLinearEquationSolverEnvironment {
    public:
        
        TopologicalLinearEquationSolverEnvironment();
        ~TopologicalLinearEquationSolverEnvironment();
        
        storm::solver::EquationSolverType const& getUnderlyingSolverType() const;
        bool const& isUnderlyingSolverTypeSetFromDefault() const;
        void setUnderlyingSolverType(storm::solver::EquationSolverType value);
        
    private:
        storm::solver::EquationSolverType underlyingSolverType;
        bool underlyingSolverTypeSetFromDefault;
    };
}

