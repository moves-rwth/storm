#ifndef STORM_MARKING_H
#define STORM_MARKING_H

#include <cmath>

#include "src/storage/BitVector.h"

namespace storm {
    namespace gspn {
        class Marking {
        public:
            /*!
             * Creates an empty marking (at all places are 0 tokens).
             *
             * @param numberOfPlaces The number of places in the gspn.
             * @param maxNumberOfTokens The maximal number of tokens in one place.
             */
            Marking(uint_fast64_t numberOfPlaces, uint_fast64_t maxNumberOfTokens);

            /*!
             * Retrieves the number of places for which the tokens are stored.
             *
             * @return The number of places.
             */
            uint_fast64_t getNumberOfPlaces();

            /*!
             * Retrieves the maximal number of tokens which can be stored in one place.
             *
             * @return The maximal number of tokens.
             */
            uint_fast64_t getMaxNumberOfTokens();

            /*!
             * Set the number of tokens for the given place to the given amount.
             *
             * @param place Place must be a valid place for which the number of tokens is changed.
             * @param numberOfTokens The new number of tokens at the place.
             */
            void setNumberOfTokensAt(uint_fast64_t place, uint_fast64_t numberOfTokens);

            /*!
             * Get the number of tokens for the given place.
             *
             * @param place The place from which the tokens are counted.
             * @return The number of tokens at the place.
             */
            uint_fast64_t getNumberOfTokensAt(uint_fast64_t place);

            /*!
             * Reset the number of places.
             * If the new number of places is larger than the old one, the new places start with 0 tokens.
             * If the new number of places is smaller than the old one, the places which are going to be
             * erased must not contain any tokens.
             *
             * @param numberOfPlaces The new number of places.
             * @return Return True if the change is made.
             */
            bool setNumberOfPlaces(uint_fast64_t numberOfPlaces);

            /*!
             *
             */
            bool setMaxNumberOfTokens(uint_fast64_t maxNumberOfTokens);
        private:
            // the maximal number of places in the gspn
            uint_fast64_t numberOfPlaces;

            // the maximal number of tokens in one place
            uint_fast64_t maxNumberOfTokens;

            // contains the information of the marking
            storm::storage::BitVector marking;

            // number of bits which are needed to store the tokens for one place
            uint_fast64_t numberOfBits;
        };
    }
}


#endif //STORM_MARKING_H
