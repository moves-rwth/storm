#pragma once

#include "storm/utility/graph.h"

namespace storm {
    namespace abstraction {
        
        template <storm::dd::DdType Type>
        using QualitativeGameResult = storm::utility::graph::GameProb01Result<Type>;
        
    }
}
