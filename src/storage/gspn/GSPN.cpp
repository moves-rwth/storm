#include "src/storage/gspn/GSPN.h"

storm::gspn::GSPN::GSPN() : initialMarking(0, 0) {
}

void storm::gspn::GSPN::setInitialTokens(uint64_t place, uint64_t token) {
    if (initialMarking.getMaxNumberOfTokens() < token) {
        initialMarking.setMaxNumberOfTokens(token);
    }
    initialMarking.setNumberOfTokensAt(place, token);
}

void storm::gspn::GSPN::setNumberOfPlaces(uint64_t number) {
    initialMarking.setNumberOfPlaces(number);
}

uint64_t storm::gspn::GSPN::getNumberOfPlaces() {
    return initialMarking.getNumberOfPlaces();
}

void storm::gspn::GSPN::addImmediateTransition(std::shared_ptr<storm::gspn::ImmediateTransition<WeightType>> transition) {
    this->immediateTransitions.push_back(transition);
}

void storm::gspn::GSPN::addTimedTransition(std::shared_ptr<storm::gspn::TimedTransition<RateType>> transition) {
    this->timedTransitions.push_back(transition);
}
