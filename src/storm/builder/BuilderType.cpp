#include "storm/builder/BuilderType.h"

#include "storm/storage/jani/ModelFeatures.h"

namespace storm {
    namespace builder {
        
        storm::jani::ModelFeatures getSupportedJaniFeatures(BuilderType const& builderType) {
            storm::jani::ModelFeatures features;
            features.add(storm::jani::ModelFeature::DerivedOperators);
            features.add(storm::jani::ModelFeature::StateExitRewards);
            if (builderType == BuilderType::Explicit) {
                features.add(storm::jani::ModelFeature::Arrays);
            }
            return features;
        }
        
        bool canHandle(BuilderType const& builderType, storm::storage::SymbolicModelDescription const& modelDescription) {
            return true;
        }
        
    }
}