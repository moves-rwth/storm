#ifndef STORM_STORAGE_BLOCK_H_
#define STORM_STORAGE_BLOCK_H_

#include <boost/container/flat_set.hpp>

#include "src/storage/sparse/StateType.h"

namespace storm {
    namespace storage {
        
        // Typedef the most common state container
        typedef boost::container::flat_set<sparse::state_type> FlatSetStateContainer;
        
        /*!
         * Writes a string representation of the state block to the given output stream.
         *
         * @param out The output stream to write to.
         * @param block The block to print to the stream.
         * @return The given output stream.
         */
        std::ostream& operator<<(std::ostream& out, FlatSetStateContainer const& block);
        
        template <typename ContainerType = FlatSetStateContainer>
        class StateBlock {
        public:
            typedef ContainerType container_type;
            typedef typename container_type::value_type value_type;
            static_assert(std::is_same<value_type, sparse::state_type>::value, "Illegal value type of container.");
            typedef typename container_type::iterator iterator;
            typedef typename container_type::const_iterator const_iterator;
            
            /*!
             * Returns an iterator to the states in this SCC.
             *
             * @return An iterator to the states in this SCC.
             */
            iterator begin();
            
            /*!
             * Returns a const iterator to the states in this SCC.
             *
             * @return A const iterator to the states in this SCC.
             */
            const_iterator begin() const;
            
            /*!
             * Returns an iterator that points one past the end of the states in this SCC.
             *
             * @return An iterator that points one past the end of the states in this SCC.
             */
            iterator end();
            
            /*!
             * Returns a const iterator that points one past the end of the states in this SCC.
             *
             * @return A const iterator that points one past the end of the states in this SCC.
             */
            const_iterator end() const;
            
            /*!
             * Retrieves whether the given state is in the SCC.
             *
             * @param state The state for which to query membership.
             */
            bool containsState(value_type const& state) const;
            
            /*!
             * Inserts the given element into this SCC.
             *
             * @param state The state to add to this SCC.
             */
            void insert(value_type const& state);
            
            /*!
             * Removes the given element from this SCC.
             *
             * @param state The element to remove.
             */
            void erase(value_type const& state);
            
            /*!
             * Retrieves the number of states in this SCC.
             *
             * @return The number of states in this SCC.
             */
            std::size_t size() const;
            
            /*!
             * Retrieves whether this SCC is empty.
             *
             * @return True iff the SCC is empty.
             */
            bool empty() const;
            
        private:
            // The container that holds the states.
            ContainerType states;
        };
    }
}

#endif /* STORM_STORAGE_BLOCK_H_ */