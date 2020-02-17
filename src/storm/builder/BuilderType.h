#pragma once

#include "storm/storage/SymbolicModelDescription.h"

namespace storm {
    namespace jani {
        class ModelFeatures;
    }
    
    namespace builder {
        enum class BuilderType {
            Explicit,
            Dd,
            Jit
        };
        
        storm::jani::ModelFeatures getSupportedJaniFeatures(BuilderType const& builderType);
        
        template <typename ValueType>
        bool canHandle(BuilderType const& builderType, storm::storage::SymbolicModelDescription const& modelDescription);
    }
}