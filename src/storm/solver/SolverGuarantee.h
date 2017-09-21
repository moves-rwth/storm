#pragma once

#include <ostream>

namespace storm {
    namespace solver {
        
        enum class SolverGuarantee {
            GreaterOrEqual, LessOrEqual, None
        };
        
        std::ostream& operator<<(std::ostream& out, SolverGuarantee const& guarantee);
        
    }
}
