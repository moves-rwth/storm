#include "src/storage/sparse/StateStorage.h"

namespace storm {
    namespace storage {
        namespace sparse {
                        
            template <typename StateType>
            StateStorage<StateType>::StateStorage(uint64_t bitsPerState) : stateToId(bitsPerState, 10000000), initialStateIndices(), bitsPerState(bitsPerState), numberOfStates() {
                // Intentionally left empty.
            }

            template class StateStorage<uint32_t>;
        }
    }
}