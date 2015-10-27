#include "src/storage/bisimulation/Block.h"

#include <iostream>
#include <iomanip>

#include "src/storage/bisimulation/Partition.h"

#include "src/exceptions/InvalidOperationException.h"
#include "src/utility/macros.h"

namespace storm {
    namespace storage {
        namespace bisimulation {

            Block::Block(storm::storage::sparse::state_type beginIndex, storm::storage::sparse::state_type endIndex, Block* previousBlock, Block* nextBlock, std::size_t id) : nextBlock(nextBlock), previousBlock(previousBlock), beginIndex(beginIndex), endIndex(endIndex), markedAsSplitter(false), needsRefinementFlag(false), absorbing(false), id(id) {
                if (nextBlock != nullptr) {
                    nextBlock->previousBlock = this;
                }
                if (previousBlock != nullptr) {
                    previousBlock->nextBlock = this;
                }
                STORM_LOG_ASSERT(this->beginIndex < this->endIndex, "Unable to create block of illegal size.");
            }
            
            bool Block::operator==(Block const& other) const {
                return this == &other;
            }
            
            bool Block::operator!=(Block const& other) const {
                return this != &other;
            }
            
            void Block::print(Partition const& partition) const {
                std::cout << "block [" << this << "] " << this->id << " from " << this->beginIndex << " to " << this->endIndex << std::endl;
            }
            
            void Block::setBeginIndex(storm::storage::sparse::state_type beginIndex) {
//                std::cout << "setting beg index to " << beginIndex << " [" << this << "]" << std::endl;
                this->beginIndex = beginIndex;
                STORM_LOG_ASSERT(beginIndex < endIndex, "Unable to resize block to illegal size.");
            }
            
            void Block::setEndIndex(storm::storage::sparse::state_type endIndex) {
                this->endIndex = endIndex;
                STORM_LOG_ASSERT(beginIndex < endIndex, "Unable to resize block to illegal size.");
            }
            
            storm::storage::sparse::state_type Block::getBeginIndex() const {
                return this->beginIndex;
            }
            
            storm::storage::sparse::state_type Block::getEndIndex() const {
                return this->endIndex;
            }
            
            Block const& Block::getNextBlock() const {
                return *this->nextBlock;
            }
            
            bool Block::hasNextBlock() const {
                return this->nextBlock != nullptr;
            }

            Block* Block::getNextBlockPointer() {
                return this->nextBlock;
            }
            
            Block const* Block::getNextBlockPointer() const {
                return this->nextBlock;
            }

            Block const& Block::getPreviousBlock() const {
                return *this->previousBlock;
            }

            Block* Block::getPreviousBlockPointer() {
                return this->previousBlock;
            }

            Block const* Block::getPreviousBlockPointer() const {
                return this->previousBlock;
            }
            
            bool Block::hasPreviousBlock() const {
                return this->previousBlock != nullptr;
            }
            
            bool Block::check() const {
                STORM_LOG_ASSERT(this->beginIndex < this->endIndex, "Block has negative size.");
                STORM_LOG_ASSERT(!this->hasPreviousBlock() || this->getPreviousBlock().getNextBlockPointer() == this, "Illegal previous block.");
                STORM_LOG_ASSERT(!this->hasNextBlock() || this->getNextBlock().getPreviousBlockPointer() == this, "Illegal next block.");
                return true;
            }
            
            std::size_t Block::getNumberOfStates() const {
                return (this->endIndex - this->beginIndex);
            }
            
            bool Block::isMarkedAsSplitter() const {
                return this->markedAsSplitter;
            }
            
            void Block::markAsSplitter() {
                this->markedAsSplitter = true;
            }
            
            void Block::unmarkAsSplitter() {
                this->markedAsSplitter = false;
            }
            
            std::size_t Block::getId() const {
                return this->id;
            }
            
            void Block::setAbsorbing(bool absorbing) {
                this->absorbing = absorbing;
            }
            
            bool Block::isAbsorbing() const {
                return this->absorbing;
            }
            
            void Block::setRepresentativeState(storm::storage::sparse::state_type representativeState) {
                this->representativeState = representativeState;
            }
            
            bool Block::hasRepresentativeState() const {
                return static_cast<bool>(representativeState);
            }
            
            storm::storage::sparse::state_type Block::getRepresentativeState() const {
                STORM_LOG_THROW(representativeState, storm::exceptions::InvalidOperationException, "Unable to retrieve representative state for block.");
                return representativeState.get();
            }
            
            // Retrieves whether the block is marked as a predecessor.
            bool Block::needsRefinement() const {
                return needsRefinementFlag;
            }
            
            // Marks the block as needing refinement (or not).
            void Block::setNeedsRefinement(bool value) {
                needsRefinementFlag = value;
            }
        }
    }
}