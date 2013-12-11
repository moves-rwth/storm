#include "src/storage/MaximalEndComponent.h"
#include "src/exceptions/InvalidStateException.h"

namespace storm {
    namespace storage {
        
        std::ostream& operator<<(std::ostream& out, boost::container::flat_set<uint_fast64_t> const& block);

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
            stateToChoicesMapping[state] = boost::container::flat_set<uint_fast64_t>(choices.begin(), choices.end());
        }
        
        void MaximalEndComponent::addState(uint_fast64_t state, std::vector<uint_fast64_t>&& choices) {
            stateToChoicesMapping.emplace(state, boost::container::flat_set<uint_fast64_t>(choices.begin(), choices.end()));
        }
        
        boost::container::flat_set<uint_fast64_t> const& MaximalEndComponent::getChoicesForState(uint_fast64_t state) const {
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
        
        void MaximalEndComponent::removeChoice(uint_fast64_t state, uint_fast64_t choice) {
            auto stateChoicePair = stateToChoicesMapping.find(state);
            
            if (stateChoicePair == stateToChoicesMapping.end()) {
                throw storm::exceptions::InvalidStateException() << "Cannot delete choice for state not contained in MEC.";
            }

            stateChoicePair->second.erase(choice);
        }
        
        void MaximalEndComponent::removeState(uint_fast64_t state) {
            auto stateChoicePair = stateToChoicesMapping.find(state);
            
            if (stateChoicePair == stateToChoicesMapping.end()) {
                throw storm::exceptions::InvalidStateException() << "Cannot delete choice for state not contained in MEC.";
            }
            
            stateToChoicesMapping.erase(stateChoicePair);
        }
        
        bool MaximalEndComponent::containsChoice(uint_fast64_t state, uint_fast64_t choice) const {
            auto stateChoicePair = stateToChoicesMapping.find(state);
            
            if (stateChoicePair == stateToChoicesMapping.end()) {
                throw storm::exceptions::InvalidStateException() << "Cannot delete choice for state not contained in MEC.";
            }
            
            return stateChoicePair->second.find(choice) != stateChoicePair->second.end();
        }
        
        boost::container::flat_set<uint_fast64_t> MaximalEndComponent::getStateSet() const {
            std::vector<uint_fast64_t> states;
            states.reserve(stateToChoicesMapping.size());
            
            for (auto const& stateChoicesPair : stateToChoicesMapping) {
                states.push_back(stateChoicesPair.first);
            }
            
            return boost::container::flat_set<uint_fast64_t>(states.begin(), states.end());
        }
        
        std::ostream& operator<<(std::ostream& out, MaximalEndComponent const& component) {
            out << "{";
            for (auto const& stateChoicesPair : component.stateToChoicesMapping) {
                out << "{" << stateChoicesPair.first << ", " << stateChoicesPair.second << "}";
            }
            out << "}";
            
            return out;
        }
        
        MaximalEndComponent::iterator MaximalEndComponent::begin() {
            return stateToChoicesMapping.begin();
        }
        
        MaximalEndComponent::iterator MaximalEndComponent::end() {
            return stateToChoicesMapping.end();
        }
        
        MaximalEndComponent::const_iterator MaximalEndComponent::begin() const {
            return stateToChoicesMapping.begin();
        }
        
        MaximalEndComponent::const_iterator MaximalEndComponent::end() const {
            return stateToChoicesMapping.end();
        }
    }
}