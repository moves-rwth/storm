#include "src/storage/gspn/Marking.h"

namespace storm {
    namespace gspn {
        Marking::Marking(uint_fast64_t numberOfPlaces, uint_fast64_t maxNumberOfTokens) {
            this->numberOfPlaces = numberOfPlaces;
            this->maxNumberOfTokens = maxNumberOfTokens;

            this->numberOfBits = calculateNumberOfBits(maxNumberOfTokens);
            this->marking = storm::storage::BitVector(numberOfBits * numberOfPlaces);
        }

        uint_fast64_t Marking::getNumberOfPlaces() {
            return this->numberOfPlaces;
        }

        uint_fast64_t  Marking::getMaxNumberOfTokens() {
            return this->maxNumberOfTokens;
        }

        void Marking::setNumberOfTokensAt(uint_fast64_t place, uint_fast64_t numberOfTokens) {
            // TODO check range (place < getNumberOfPlaces(), numberOfTokens < getMaxNumberOfTokens())
            for (uint_fast64_t i = place * numberOfBits; i <(place * numberOfBits) + numberOfBits; ++i) {
                if (numberOfTokens % 2 == 0) {
                    marking.set(i, false);
                } else {
                    marking.set(i, true);
                }
                numberOfTokens /= 2;
            }
        }

        uint_fast64_t Marking::getNumberOfTokensAt(uint_fast64_t place) {
            uint_fast64_t tokens = 0;
            for (uint_fast64_t i = place * numberOfBits, mult = 0; i < (place * numberOfBits) + numberOfBits; ++i, ++mult) {
                if (marking.get(i)) {
                    tokens += std::pow(2, mult);
                }
            }
            return tokens;
        }

        bool Marking::setNumberOfPlaces(uint_fast64_t numberOfPlaces) {
            if (numberOfPlaces == this->numberOfPlaces) {
                return true;
            }
            if (numberOfPlaces > this->numberOfPlaces) {
                marking.resize(numberOfPlaces * numberOfBits);
                return true;
            } else {
                auto diff = this->numberOfPlaces - numberOfPlaces;
                for (uint64_t i = 0; i < diff; ++i) {
                    if (getNumberOfTokensAt(numberOfPlaces-1-i) != 0) {
                        // TODO error
                        return false;
                    }
                }
                marking.resize(numberOfPlaces * numberOfBits);
                return true;
            }
        }

        bool Marking::setMaxNumberOfTokens(uint_fast64_t maxNumberOfTokens) {
            for (uint64_t i = 0; i < getNumberOfPlaces(); ++i) {
                if (getNumberOfTokensAt(i) > maxNumberOfTokens) {
                    return false;
                }
            }

            if (maxNumberOfTokens == getMaxNumberOfTokens()) {
                return true;
            }

            uint_fast64_t newNumberOfBits = calculateNumberOfBits(maxNumberOfTokens);
            if (maxNumberOfTokens < getMaxNumberOfTokens()) {
                for (uint_fast64_t i = 0; i < getNumberOfPlaces(); ++i) {
                    for (uint_fast64_t j = 0; j < numberOfBits; ++j) {
                        marking.set(i*newNumberOfBits + j , marking.get(i*numberOfBits + j));
                    }
                }
                marking.resize(getNumberOfPlaces() * newNumberOfBits);
            } else {
                marking.resize(getNumberOfPlaces() * newNumberOfBits);
                for (uint_fast64_t i = getNumberOfPlaces()-1; i >= 0; --i) {
                    for (uint_fast64_t j = numberOfBits-1; j >= 0; --j) {
                        for (uint_fast64_t diff = 0; diff < newNumberOfBits-numberOfBits; ++diff) {
                            marking.set(i*newNumberOfBits+j+diff+1, 0);
                        }
                        marking.set(i*newNumberOfBits+j, marking.get(i*numberOfBits+j));
                    }
                }
            }
            numberOfBits = newNumberOfBits;
            return true;
        }

        void Marking::incNumberOfPlaces() {
            setNumberOfPlaces(getNumberOfPlaces()+1);
        }

        uint_fast64_t Marking::calculateNumberOfBits(uint_fast64_t maxNumber) {
            return std::floor(std::log2(maxNumber)) + 1;
        }
    }
}