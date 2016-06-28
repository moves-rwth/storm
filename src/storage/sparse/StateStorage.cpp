#include "src/storage/sparse/StateStorage.h"

namespace storm {
    namespace storage {
        namespace sparse {
                        
            template <typename StateType>
            StateStorage<StateType>::StateStorage(uint64_t bitsPerState) : stateToId(bitsPerState, 10000000), initialStateIndices(), deadlockStateIndices(), bitsPerState(bitsPerState) {
                // Intentionally left empty.
            }

            template <typename StateType>
            uint_fast64_t StateStorage<StateType>::getNumberOfStates() const {
                return stateToId.size();
            }
            
            template class StateStorage<uint32_t>;
        }
    }
}