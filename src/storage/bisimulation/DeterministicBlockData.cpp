#include "src/storage/bisimulation/DeterministicBlockData.h"

namespace storm {
    namespace storage {
        namespace bisimulation {
            
            DeterministicBlockData::DeterministicBlockData() : newBeginIndex(0), newEndIndex(0) {
                // Intentionally left empty.
            }
            
            DeterministicBlockData::DeterministicBlockData(uint_fast64_t newBeginIndex, uint_fast64_t newEndIndex) : newBeginIndex(newBeginIndex), newEndIndex(newEndIndex) {
                // Intentionally left empty.
            }
            
            uint_fast64_t DeterministicBlockData::getNewBeginIndex() const {
                return newBeginIndex;
            }
         
            void DeterministicBlockData::increaseNewBeginIndex() {
                ++newBeginIndex;
            }
            
            uint_fast64_t DeterministicBlockData::getNewEndIndex() const {
                return newEndIndex;
            }
            
            void DeterministicBlockData::decreaseNewEndIndex() {
                --newEndIndex;
            }

            void DeterministicBlockData::increaseNewEndIndex() {
                ++newEndIndex;
            }
            
            bool DeterministicBlockData::notify(Block<DeterministicBlockData> const& block) {
                bool result = block.getBeginIndex() != this->newBeginIndex || block.getEndIndex() != this->newEndIndex;
                this->newBeginIndex = block.getBeginIndex();
                this->newEndIndex = block.getEndIndex();
                return result;
            }

        }
    }
}
