#ifndef STORM_STORAGE_GSPN_PLACE_H_
#define STORM_STORAGE_GSPN_PLACE_H_

#include <string>
#include "boost/optional.hpp"

namespace storm {
    namespace gspn {
        /*!
         * This class provides methods to store and retrieve data for a place in a gspn.
         */
        class Place {
        public:
            /*!
             * Sets the name of this place. The name is not used to identify a place (and therefore do not have to be unique).
             * Some input and output formats use the name to identify a place. If you want to use the export or import
             * features make sure that the names a unique if necessary.
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
            void setCapacity(uint64_t capacity);

            /*!
             * Returns the capacity of tokens of this place.
             *
             * @return The capacity of the place. Only valid if the capacity is restricted.
             */
            uint64_t getCapacity() const;
            
            /*!
             *
             */
            bool hasRestrictedCapacity() const;
        private:
            // contains the number of initial tokens of this place
            uint_fast64_t numberOfInitialTokens = 0;

            // unique id (is used to refer to a specific place in a bitvector)
            uint_fast64_t id = 0;

            // name which is used in pnml file
            std::string name;

            // capacity of this place
            // -1 indicates that the capacity is not set
            // other non-negative values represents the capacity
            boost::optional<uint64_t> capacity = boost::none;
        };
    }
}

#endif //STORM_STORAGE_GSPN_PLACE_H_
