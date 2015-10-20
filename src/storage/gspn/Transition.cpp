#include "src/storage/gspn/Transition.h"

namespace storm {
    namespace gspn {

        void Transition::setInputArcCardinality(uint_fast64_t place, uint_fast64_t multiplicity) {
            inputMultiplicities[place] = multiplicity;
        }

        bool Transition::removeInputArc(uint_fast64_t place) {
            if (existsInputArc(place)) {
                inputMultiplicities.erase(place);
                return true;
            } else {
                return false;
            }
        }

        bool Transition::existsInputArc(uint_fast64_t place) {
            return inputMultiplicities.end() != inputMultiplicities.find(place);
        }

        void Transition::setOutputArcCardinality(uint_fast64_t place, uint_fast64_t multiplicity) {
            outputMultiplicities[place] = multiplicity;
        }

        bool Transition::removeOutputArc(uint_fast64_t place) {
            if (existsOutputArc(place)) {
                outputMultiplicities.erase(place);
                return true;
            } else {
                return false;
            }
        }

        bool Transition::existsOutputArc(uint_fast64_t place) {
            return outputMultiplicities.end() != outputMultiplicities.find(place);
        }

        void Transition::setInhibitionArcCardinality(uint_fast64_t place, uint_fast64_t multiplicity) {
            inhibitionMultiplicities[place] = multiplicity;
        }

        bool Transition::removeInhibitionArc(uint_fast64_t place) {
            if (existsInhibitionArc(place)) {
                inhibitionMultiplicities.erase(place);
                return true;
            } else {
                return false;
            }
        }

        bool Transition::existsInhibitionArc(uint_fast64_t place) {
            return inhibitionMultiplicities.end() != inhibitionMultiplicities.find(place);
        }

        bool Transition::isEnabled(storm::gspn::Marking marking) {
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

        storm::gspn::Marking Transition::fire(const storm::gspn::Marking marking) {
            //check if transition is enabled

            return gspn::Marking(0, 0);
        }
    }
}
