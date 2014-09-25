#ifndef STORM_STORAGE_STRONGLYCONNECTEDCOMPONENT_H_
#define STORM_STORAGE_STRONGLYCONNECTEDCOMPONENT_H_

#include "src/storage/StateBlock.h"
#include "src/storage/Decomposition.h"
#include "src/utility/OsDetection.h"

namespace storm {
    namespace storage {
        
        /*!
         * This class represents a strongly connected component, i.e., a set of states such that every state can reach
         * every other state.
         */
        class StronglyConnectedComponent : public StateBlock {
        public:
            StronglyConnectedComponent() = default;
            StronglyConnectedComponent(StronglyConnectedComponent const& other) = default;
            StronglyConnectedComponent(StronglyConnectedComponent&& other) = default;
#ifndef WINDOWS
            StronglyConnectedComponent& operator=(StronglyConnectedComponent const& other) = default;
            StronglyConnectedComponent& operator=(StronglyConnectedComponent&& other) = default;
#endif
            
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
        };
        
    }
}

#endif /* STORM_STORAGE_STRONGLYCONNECTEDCOMPONENT_H_ */