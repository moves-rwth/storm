#ifndef STORM_STORAGE_BISIMULATION_DETERMINISTICBLOCKDATA_H_
#define STORM_STORAGE_BISIMULATION_DETERMINISTICBLOCKDATA_H_

#include <cstdint>

#include "src/storage/bisimulation/Block.h"

namespace storm {
    namespace storage {
        namespace bisimulation {
            class DeterministicBlockData {
            public:
                DeterministicBlockData();
                DeterministicBlockData(uint_fast64_t newBeginIndex, uint_fast64_t newEndIndex);
                
                /*!
                 * Retrieves the new begin index.
                 *
                 * @return The new begin index.
                 */
                uint_fast64_t getNewBeginIndex() const;
                
                /*!
                 * Increases the new begin index by one.
                 */
                void increaseNewBeginIndex();

                /*!
                 * Retrieves the new end index.
                 *
                 * @return The new end index.
                 */
                uint_fast64_t getNewEndIndex() const;
                
                /*!
                 * Decreases the new end index.
                 */
                void decreaseNewEndIndex();
                
                /*!
                 * Increases the new end index.
                 */
                void increaseNewEndIndex();
                
                /*!
                 * This method needs to be called whenever the block was modified to notify the data of the change.
                 *
                 * @param block The block that this data belongs to.
                 * @return True iff the data changed as a consequence of notifying it.
                 */
                bool notify(Block<DeterministicBlockData> const& block);
                
            public:
                // A marker that can be used to mark a the new beginning of the block.
                uint_fast64_t newBeginIndex;
                
                // A marker that can be used to mark a the new end of the block.
                uint_fast64_t newEndIndex;
            };
        }
    }
}

#endif /* STORM_STORAGE_BISIMULATION_DETERMINISTICBLOCKDATA_H_ */
