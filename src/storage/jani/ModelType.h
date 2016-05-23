#pragma once

#include <ostream>

namespace storm {
    namespace jani {
        
        enum class ModelType {UNDEFINED = 0, DTMC = 1, CTMC = 2, MDP = 3, MA = 4, PTA = 5, STA = 6};

        ModelType getModelType(std::string const& input);
        std::ostream& operator<<(std::ostream& stream, ModelType const& type);
        
    }
}