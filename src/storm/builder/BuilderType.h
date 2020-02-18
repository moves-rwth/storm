#pragma once

#include <vector>
#include <boost/optional.hpp>

#include "storm/storage/jani/Property.h"
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
        bool canHandle(BuilderType const& builderType, storm::storage::SymbolicModelDescription const& modelDescription, boost::optional<std::vector<storm::jani::Property>> const& properties = boost::none);
    }
}