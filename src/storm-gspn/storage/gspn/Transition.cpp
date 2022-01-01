#include "Transition.h"

#include "storm/utility/macros.h"

namespace storm {
namespace gspn {

void Transition::setInputArcMultiplicity(storm::gspn::Place const& place, uint64_t multiplicity) {
    inputMultiplicities[place.getID()] = multiplicity;
}

bool Transition::removeInputArc(storm::gspn::Place const& place) {
    if (existsInputArc(place)) {
        inputMultiplicities.erase(place.getID());
        return true;
    } else {
        return false;
    }
}

bool Transition::existsInputArc(storm::gspn::Place const& place) const {
    return inputMultiplicities.end() != inputMultiplicities.find(place.getID());
}

void Transition::setOutputArcMultiplicity(storm::gspn::Place const& place, uint64_t multiplicity) {
    outputMultiplicities[place.getID()] = multiplicity;
}

bool Transition::removeOutputArc(storm::gspn::Place const& place) {
    if (existsOutputArc(place)) {
        outputMultiplicities.erase(place.getID());
        return true;
    } else {
        return false;
    }
}

bool Transition::existsOutputArc(storm::gspn::Place const& place) const {
    return outputMultiplicities.end() != outputMultiplicities.find(place.getID());
}

void Transition::setInhibitionArcMultiplicity(storm::gspn::Place const& place, uint64_t multiplicity) {
    inhibitionMultiplicities[place.getID()] = multiplicity;
}

bool Transition::removeInhibitionArc(storm::gspn::Place const& place) {
    if (existsInhibitionArc(place)) {
        inhibitionMultiplicities.erase(place.getID());
        return true;
    } else {
        return false;
    }
}

bool Transition::existsInhibitionArc(storm::gspn::Place const& place) const {
    return inhibitionMultiplicities.end() != inhibitionMultiplicities.find(place.getID());
}

bool Transition::isEnabled(storm::gspn::Marking const& marking) const {
    auto inputIterator = inputMultiplicities.cbegin();
    while (inputIterator != inputMultiplicities.cend()) {
        if (marking.getNumberOfTokensAt(inputIterator->first) < inputIterator->second) {
            return false;
        }

        ++inputIterator;
    }

    auto inhibitionIterator = inhibitionMultiplicities.cbegin();
    while (inhibitionIterator != inhibitionMultiplicities.cend()) {
        if (marking.getNumberOfTokensAt(inhibitionIterator->first) >= inhibitionIterator->second) {
            return false;
        }

        ++inhibitionIterator;
    }

    return true;
}

storm::gspn::Marking Transition::fire(storm::gspn::Marking const& marking) const {
    storm::gspn::Marking newMarking(marking);

    auto inputIterator = inputMultiplicities.cbegin();
    while (inputIterator != inputMultiplicities.cend()) {
        newMarking.setNumberOfTokensAt(inputIterator->first, newMarking.getNumberOfTokensAt(inputIterator->first) - inputIterator->second);
        ++inputIterator;
    }

    auto outputIterator = outputMultiplicities.cbegin();
    while (outputIterator != outputMultiplicities.cend()) {
        newMarking.setNumberOfTokensAt(outputIterator->first, newMarking.getNumberOfTokensAt(outputIterator->first) + outputIterator->second);
        ++outputIterator;
    }

    return newMarking;
}

void Transition::setName(std::string const& name) {
    this->name = name;
}

std::string const& Transition::getName() const {
    return this->name;
}

std::unordered_map<uint64_t, uint64_t> const& Transition::getInputPlaces() const {
    return inputMultiplicities;
}

std::unordered_map<uint64_t, uint64_t> const& Transition::getOutputPlaces() const {
    return outputMultiplicities;
}

std::unordered_map<uint64_t, uint64_t> const& Transition::getInhibitionPlaces() const {
    return inhibitionMultiplicities;
}

uint64_t Transition::getInputArcMultiplicity(storm::gspn::Place const& place) const {
    if (existsInputArc(place)) {
        return inputMultiplicities.at(place.getID());
    } else {
        return 0;
    }
}

uint64_t Transition::getInhibitionArcMultiplicity(storm::gspn::Place const& place) const {
    if (existsInhibitionArc(place)) {
        return inhibitionMultiplicities.at(place.getID());
    } else {
        return 0;
    }
}

uint64_t Transition::getOutputArcMultiplicity(storm::gspn::Place const& place) const {
    if (existsOutputArc(place)) {
        return outputMultiplicities.at(place.getID());
    } else {
        return 0;
    }
}

void Transition::setPriority(uint64_t const& priority) {
    this->priority = priority;
}

uint64_t Transition::getPriority() const {
    return this->priority;
}
}  // namespace gspn
}  // namespace storm
