#pragma once

#include <ostream>

namespace storm {
    namespace jani {
        
        enum class ModelType {UNDEFINED = 0, DTMC = 1, CTMC = 2, MDP = 3, MA = 4};
     
        std::ostream& operator<<(std::ostream& stream, ModelType const& type);
        
    }
}