#include "src/storage/MaximalEndComponent.h"
#include "src/exceptions/InvalidStateException.h"

namespace storm {
    namespace storage {
        
        MaximalEndComponent::MaximalEndComponent() : stateToChoicesMapping() {
            // Intentionally left empty.
        }
        
        MaximalEndComponent::MaximalEndComponent(MaximalEndComponent const& other) : stateToChoicesMapping(other.stateToChoicesMapping) {
            // Intentionally left empty.
        }
        
        MaximalEndComponent& MaximalEndComponent::operator=(MaximalEndComponent const& other) {
            stateToChoicesMapping = other.stateToChoicesMapping;
            return *this;
        }
        
        MaximalEndComponent::MaximalEndComponent(MaximalEndComponent&& other) : stateToChoicesMapping(std::move(other.stateToChoicesMapping)) {
            // Intentionally left empty.
        }
        
        MaximalEndComponent& MaximalEndComponent::operator=(MaximalEndComponent&& other) {
            stateToChoicesMapping = std::move(other.stateToChoicesMapping);
            return *this;
        }
        
        void MaximalEndComponent::addState(uint_fast64_t state, std::vector<uint_fast64_t> const& choices) {
            stateToChoicesMapping[state] = choices;
        }
        
        std::vector<uint_fast64_t> const& MaximalEndComponent::getChoicesForState(uint_fast64_t state) const {
            auto stateChoicePair = stateToChoicesMapping.find(state);
            
            if (stateChoicePair == stateToChoicesMapping.end()) {
                throw storm::exceptions::InvalidStateException() << "Cannot retrieve choices for state not contained in MEC.";
            }
            
            return stateChoicePair->second;
        }
        
        bool MaximalEndComponent::containsState(uint_fast64_t state) const {
            auto stateChoicePair = stateToChoicesMapping.find(state);
            
            if (stateChoicePair == stateToChoicesMapping.end()) {
                return false;
            }
            return true;
        }
    }
}