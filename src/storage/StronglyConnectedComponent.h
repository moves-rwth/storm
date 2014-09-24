#ifndef STORM_STORAGE_STRONGLYCONNECTEDCOMPONENT_H_
#define STORM_STORAGE_STRONGLYCONNECTEDCOMPONENT_H_

#include "src/storage/Block.h"
#include "src/storage/Decomposition.h"

namespace storm {
    namespace storage {
        
        typedef FlatSetStateContainer SccBlock;
        
        /*!
         * This class represents a strongly connected component, i.e., a set of states such that every state can reach
         * every other state.
         */
        class StronglyConnectedComponent : public Block<SccBlock> {
        public:
            typedef SccBlock block_type;
            typedef block_type::value_type value_type;
            typedef block_type::iterator iterator;
            typedef block_type::const_iterator const_iterator;
            
            /*!
             * Creates an empty strongly connected component.
             */
            StronglyConnectedComponent();
            
            /*!
             * Sets whether this SCC is trivial or not.
             *
             * @param trivial A flag indicating whether this SCC is trivial or not.
             */
            void setIsTrivial(bool trivial);
            
            /*!
             * Retrieves whether this SCC is trivial.
             *
             * @return True iff this SCC is trivial.
             */
            bool isTrivial() const;
            
        private:
            // Stores whether this SCC is trivial.
            bool isTrivialScc;
            
            // The states in this SCC.
            block_type states;
        };
        
    }
}

#endif /* STORM_STORAGE_STRONGLYCONNECTEDCOMPONENT_H_ */