#pragma once

#include <string>
#include <set>

namespace storm {
    namespace jani {

        enum class ModelFeature {Arrays, DerivedOperators, StateExitRewards};

        std::string toString(ModelFeature const& modelFeature);
        
        class ModelFeatures {
        
        public:
            std::string toString() const;
            
            bool hasArrays() const;
            bool hasDerivedOperators() const;
            bool hasStateExitRewards() const;
            
            void add(ModelFeature const& modelFeature);
            void remove(ModelFeature const& modelFeature);

        private:
            std::set<ModelFeature> features;
        };
    }
}
