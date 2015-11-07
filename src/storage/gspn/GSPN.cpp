#include "src/storage/gspn/GSPN.h"

storm::gspn::GSPN::GSPN() : initialMarking(0, 0) {
}

void storm::gspn::GSPN::setInitialTokens(uint64_t place, uint64_t token) {
    if (initialMarking.getMaxNumberOfTokens() < token) {
        initialMarking.setMaxNumberOfTokens(token);
    }
    initialMarking.setNumberOfTokensAt(place, token);
}
