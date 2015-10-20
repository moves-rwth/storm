#include "src/storage/gspn/Marking.h"

namespace storm {
    namespace gspn {
        Marking::Marking(uint_fast64_t numberOfPlaces, uint_fast64_t maxNumberOfTokens) {
            this->numberOfPlaces = numberOfPlaces;
            this->maxNumberOfTokens = maxNumberOfTokens;

            this->numberOfBits = std::floor(std::log2(maxNumberOfTokens)) + 1;
            this->marking = storm::storage::BitVector(numberOfBits * numberOfPlaces);
        }

        uint_fast64_t Marking::getNumberOfPlaces() {
            return this->numberOfPlaces;
        }

        uint_fast64_t  Marking::getMaxNumberOfTokens() {
            return this->maxNumberOfTokens;
        }

        void Marking::setNumberOfTokensAt(uint_fast64_t place, uint_fast64_t numberOfTokens) {
            //check range
            for (uint_fast64_t i = getNumberOfPlaces() * numberOfBits; i <(getNumberOfPlaces() * numberOfBits) + numberOfBits; ++i) {
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
            for (uint_fast64_t i = getNumberOfPlaces() * numberOfBits, mult = 0; i <(getNumberOfPlaces() * numberOfBits) + numberOfBits; ++i, ++mult) {
                if (marking.get(i)) {
                    tokens += std::pow(2, mult);
                }
            }
            return tokens;
        }
    }
}