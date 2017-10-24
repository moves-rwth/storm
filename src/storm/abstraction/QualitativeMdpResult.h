#pragma once

#include "storm/utility/graph.h"

namespace storm {
    namespace abstraction {
        
        template <storm::dd::DdType Type>
        using QualitativeMdpResult = std::pair<storm::dd::Bdd<Type>, storm::dd::Bdd<Type>>;
        
    }
}

