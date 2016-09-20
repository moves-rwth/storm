#ifndef STORM_STORAGE_GSPN_PLACE_H_
#define STORM_STORAGE_GSPN_PLACE_H_

#include <string>

namespace storm {
    namespace gspn {
        /*!
         * This class provides methods to store and retrieve data for a place in a gspn.
         */
        class Place {
        public:
            /*!
             * Sets the name of this place. The name must be unique for a gspn.
             *
             * @param name The new name for the place.
             */
            void setName(std::string const& name);

            /*!
             * Returns the name of this place.
             *
             * @return The name of this place.
             */
            std::string getName() const;

            /*!
             * Sets the id of this place. The id must be unique for a gspn.
             *
             * @param id The new id of this place.
             */
            void setID(uint_fast64_t const& id);

            /*!
             * Returns the id of this place.
             *
             * @return The id of this place.
             */
            uint_fast64_t getID() const;

            /*!
             * Sets the number of initial tokens of this place.
             *
             * @param tokens The number of initial tokens.
             */
            void setNumberOfInitialTokens(uint_fast64_t const& tokens);

            /*!
             * Returns the number of initial tokens of this place.
             *
             * @return The number of initial tokens of this place.
             */
            uint_fast64_t getNumberOfInitialTokens() const;

            /*!
             * Sets the capacity of tokens of this place.
             *
             * @param capacity The capacity of this place. A non-negative number represents the capacity.
             *                 The value -1 indicates that the capacity is not set.
             */
            void setCapacity(int_fast64_t const& capacity);

            /*!
             * Returns the capacity of tokens of this place.
             *
             * @return The capacity of the place. The value -1 indicates that the capacity is not set.
             */
            int_fast64_t getCapacity() const;
        private:
            // contains the number of initial tokens of this place
            uint_fast64_t numberOfInitialTokens;

            // unique id (is used to refer to a specific place in a bitvector)
            uint_fast64_t id;

            // name which is used in pnml file
            std::string name;

            // capacity of this place
            // -1 indicates that the capacity is not set
            // other non-negative values represents the capacity
            int_fast64_t capacity;
        };
    }
}

#endif //STORM_STORAGE_GSPN_PLACE_H_
