#pragma once

#include <iostream>

namespace storm {
    namespace solver {
        
        enum class MultiplicationStyle { AllowGaussSeidel, Regular };
     
        std::ostream& operator<<(std::ostream& out, MultiplicationStyle const& style);
        
    }
}
