#pragma once

#include <iostream>

namespace storm {
    namespace solver {
        
        enum class LinearEquationSolverTask { Unspecified, SolveEquations, Multiply };
     
        std::ostream& operator<<(std::ostream& out, LinearEquationSolverTask const& style);
        
    }
}
