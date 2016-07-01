#pragma once

#include <ostream>

namespace storm {
    namespace jani {
        
        enum class ModelType {UNDEFINED = 0, DTMC = 1, CTMC = 2, MDP = 3, CTMDP = 4, MA = 5, PTA = 6, STA = 7};

        ModelType getModelType(std::string const& input);
        std::ostream& operator<<(std::ostream& stream, ModelType const& type);
        
    }
}