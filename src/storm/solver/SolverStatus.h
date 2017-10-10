#pragma once

#include <ostream>

namespace storm {
    namespace solver {
        
        enum class SolverStatus {
            Converged, TerminatedEarly, MaximalIterationsExceeded, InProgress
        };
     
        std::ostream& operator<<(std::ostream& out, SolverStatus const& status);
        
    }
}
