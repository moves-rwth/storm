#ifndef STORM_STORAGE_DISTRIBUTION_H_
#define STORM_STORAGE_DISTRIBUTION_H_

#include <vector>
#include <ostream>
#include <boost/container/flat_map.hpp>

#include "src/storage/sparse/StateType.h"

namespace storm {
    namespace storage {
        
        template<typename ValueType>
        class Distribution {
        public:
            typedef boost::container::flat_map<storm::storage::sparse::state_type, ValueType> container_type;
            typedef typename container_type::iterator iterator;
            typedef typename container_type::const_iterator const_iterator;
            
            /*!
             * Creates an empty distribution.
             */
            Distribution();
            
            /*!
             * Checks whether the two distributions specify the same probabilities to go to the same states.
             *
             * @param other The distribution with which the current distribution is to be compared.
             * @return True iff the two distributions are equal.
             */
            bool operator==(Distribution const& other) const;
            
            /*!
             * Assigns the given state the given probability under this distribution.
             *
             * @param state The state to which to assign the probability.
             * @param probability The probability to assign.
             */
            void addProbability(storm::storage::sparse::state_type const& state, ValueType const& probability);
            
            /*!
             * Retrieves an iterator to the elements in this distribution.
             *
             * @return The iterator to the elements in this distribution.
             */
            iterator begin();

            /*!
             * Retrieves an iterator to the elements in this distribution.
             *
             * @return The iterator to the elements in this distribution.
             */
            const_iterator begin() const;
            
            /*!
             * Retrieves an iterator past the elements in this distribution.
             *
             * @return The iterator past the elements in this distribution.
             */
            iterator end();

            /*!
             * Retrieves an iterator past the elements in this distribution.
             *
             * @return The iterator past the elements in this distribution.
             */
            const_iterator end() const;
            
            /*!
             * Retrieves the hash value of the distribution.
             *
             * @return The hash value of the distribution.
             */
            std::size_t getHash() const;
            
        private:
            // A list of states and the probabilities that are assigned to them.
            container_type distribution;
            
            // A hash value that is maintained to allow for quicker equality comparison between distribution.s
            std::size_t hash;
        };
        
        template<typename ValueType>
            std::ostream& operator<<(std::ostream& out, Distribution<ValueType> const& distribution);
    }
}

namespace std {
    
    template <typename ValueType>
    struct hash<storm::storage::Distribution<ValueType>> {
        std::size_t operator()(storm::storage::Distribution<ValueType> const& distribution) const {
            return (distribution.getHash());
        }
    };
    
}


#endif /* STORM_STORAGE_DISTRIBUTION_H_ */