#include "storm/storage/RobustMaximalEndComponent.h"
#include "storm/storage/BoostTypes.h"
#include "storm/storage/sparse/StateType.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/storage/BitVector.h"

namespace storm {
namespace storage {

std::ostream& operator<<(std::ostream& out, storm::storage::FlatSet<uint_fast64_t> const& block);

RobustMaximalEndComponent::RobustMaximalEndComponent() : stateToChoicesMapping() {
    // Intentionally left empty.
}

RobustMaximalEndComponent::RobustMaximalEndComponent(RobustMaximalEndComponent const& other) : stateToChoicesMapping(other.stateToChoicesMapping) {
    // Intentionally left empty.
}

RobustMaximalEndComponent& RobustMaximalEndComponent::operator=(RobustMaximalEndComponent const& other) {
    stateToChoicesMapping = other.stateToChoicesMapping;
    return *this;
}

RobustMaximalEndComponent::RobustMaximalEndComponent(RobustMaximalEndComponent&& other) : stateToChoicesMapping(std::move(other.stateToChoicesMapping)) {
    // Intentionally left empty.
}

RobustMaximalEndComponent& RobustMaximalEndComponent::operator=(RobustMaximalEndComponent&& other) {
    stateToChoicesMapping = std::move(other.stateToChoicesMapping);
    return *this;
}

bool RobustMaximalEndComponent::operator==(RobustMaximalEndComponent const& other) {
    return stateToChoicesMapping == other.stateToChoicesMapping;
}

bool RobustMaximalEndComponent::operator!=(RobustMaximalEndComponent const& other) {
    return stateToChoicesMapping != other.stateToChoicesMapping;
}

void RobustMaximalEndComponent::addState(uint_fast64_t state, set_type const& choices) {
    stateToChoicesMapping[state] = choices;
}

void RobustMaximalEndComponent::addState(uint_fast64_t state, set_type&& choices) {
    stateToChoicesMapping.emplace(state, std::move(choices));
}

std::size_t RobustMaximalEndComponent::size() const {
    return stateToChoicesMapping.size();
}

RobustMaximalEndComponent::set_type const& RobustMaximalEndComponent::getChoicesForState(uint_fast64_t state) const {
    auto stateChoicePair = stateToChoicesMapping.find(state);

    if (stateChoicePair == stateToChoicesMapping.end()) {
        throw storm::exceptions::InvalidStateException()
            << "Invalid call to RobustMaximalEndComponent::getChoicesForState: cannot retrieve choices for state not contained in MEC.";
    }

    return stateChoicePair->second;
}

RobustMaximalEndComponent::set_type& RobustMaximalEndComponent::getChoicesForState(uint_fast64_t state) {
    auto stateChoicePair = stateToChoicesMapping.find(state);

    if (stateChoicePair == stateToChoicesMapping.end()) {
        throw storm::exceptions::InvalidStateException()
            << "Invalid call to RobustMaximalEndComponent::getChoicesForState: cannot retrieve choices for state not contained in MEC.";
    }

    return stateChoicePair->second;
}

bool RobustMaximalEndComponent::containsState(uint_fast64_t state) const {
    auto stateChoicePair = stateToChoicesMapping.find(state);

    if (stateChoicePair == stateToChoicesMapping.end()) {
        return false;
    }
    return true;
}

bool RobustMaximalEndComponent::containsAnyState(storm::storage::BitVector stateSet) const {
    for (auto const& stateChoicesPair : stateToChoicesMapping) {
        if (stateSet.get(stateChoicesPair.first)) {
            return true;
        }
    }
    return false;
}

void RobustMaximalEndComponent::removeState(uint_fast64_t state) {
    auto stateChoicePair = stateToChoicesMapping.find(state);

    if (stateChoicePair == stateToChoicesMapping.end()) {
        throw storm::exceptions::InvalidStateException() << "Invalid call to RobustMaximalEndComponent::removeState: cannot remove state not contained in MEC.";
    }

    stateToChoicesMapping.erase(stateChoicePair);
}

bool RobustMaximalEndComponent::containsChoice(uint_fast64_t state, std::vector<uint_fast64_t> choice) const {
    auto stateChoicePair = stateToChoicesMapping.find(state);

    if (stateChoicePair == stateToChoicesMapping.end()) {
        throw storm::exceptions::InvalidStateException()
            << "Invalid call to RobustMaximalEndComponent::containsChoice: cannot obtain choices for state not contained in MEC.";
    }

    return stateChoicePair->second.find(choice) != stateChoicePair->second.end();
}

storm::storage::FlatSet<sparse::state_type> RobustMaximalEndComponent::getStateSet() const {
    storm::storage::FlatSet<sparse::state_type> states;
    states.reserve(stateToChoicesMapping.size());

    for (auto const& stateChoicesPair : stateToChoicesMapping) {
        states.insert(stateChoicesPair.first);
    }

    return states;
}

std::ostream& operator<<(std::ostream& out, RobustMaximalEndComponent const& component) {
    out << "{";
    for (auto const& stateChoicesPair : component.stateToChoicesMapping) {
        out << "{" << stateChoicesPair.first << ", {";
        for (auto const& choice : stateChoicesPair.second) {
            out << "[";
            for (uint64_t i = 0; i < choice.size(); i++) {
                out << choice[i];
                if (i != choice.size() - 1) {
                    out << ", ";
                }
            }
            out << "]";
        }
        out << "}}";
    }
    out << "}";

    return out;
}

RobustMaximalEndComponent::iterator RobustMaximalEndComponent::begin() {
    return stateToChoicesMapping.begin();
}

RobustMaximalEndComponent::iterator RobustMaximalEndComponent::end() {
    return stateToChoicesMapping.end();
}

RobustMaximalEndComponent::const_iterator RobustMaximalEndComponent::begin() const {
    return stateToChoicesMapping.begin();
}

RobustMaximalEndComponent::const_iterator RobustMaximalEndComponent::end() const {
    return stateToChoicesMapping.end();
}
}  // namespace storage
}  // namespace storm
