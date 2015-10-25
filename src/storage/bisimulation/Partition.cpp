#include "src/storage/bisimulation/Partition.h"

#include <iostream>

#include "src/utility/macros.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace storage {
        namespace bisimulation {
            Partition::Partition(std::size_t numberOfStates) : stateToBlockMapping(numberOfStates), states(numberOfStates), positions(numberOfStates) {
                blocks.emplace_back(new Block(0, numberOfStates, nullptr, nullptr, blocks.size()));
                
                // Set up the different parts of the internal structure.
                for (storm::storage::sparse::state_type state = 0; state < numberOfStates; ++state) {
                    states[state] = state;
                    positions[state] = state;
                    stateToBlockMapping[state] = blocks.back().get();
                }
            }
            
            Partition::Partition(std::size_t numberOfStates, storm::storage::BitVector const& prob0States, storm::storage::BitVector const& prob1States, boost::optional<storm::storage::sparse::state_type> representativeProb1State) : stateToBlockMapping(numberOfStates), states(numberOfStates), positions(numberOfStates) {
                storm::storage::sparse::state_type position = 0;
                Block* firstBlock = nullptr;
                Block* secondBlock = nullptr;
                Block* thirdBlock = nullptr;
                if (!prob0States.empty()) {
                    blocks.emplace_back(new Block(0, prob0States.getNumberOfSetBits(), nullptr, nullptr, blocks.size()));
                    firstBlock = blocks.front().get();
                    
                    for (auto state : prob0States) {
                        states[position] = state;
                        positions[state] = position;
                        stateToBlockMapping[state] = firstBlock;
                        ++position;
                    }
                    firstBlock->setAbsorbing(true);
                    firstBlock->markAsSplitter();
                }
                
                if (!prob1States.empty()) {
                    blocks.emplace_back(new Block(position, position + prob1States.getNumberOfSetBits(), firstBlock, nullptr, blocks.size()));
                    secondBlock = blocks[1].get();
                    
                    for (auto state : prob1States) {
                        states[position] = state;
                        positions[state] = position;
                        stateToBlockMapping[state] = secondBlock;
                        ++position;
                    }
                    secondBlock->setAbsorbing(true);
                    secondBlock->setRepresentativeState(representativeProb1State.get());
                    secondBlock->markAsSplitter();
                }
                
                storm::storage::BitVector otherStates = ~(prob0States | prob1States);
                if (!otherStates.empty()) {
                    blocks.emplace_back(new Block(position, numberOfStates, secondBlock, nullptr, blocks.size()));
                    thirdBlock = blocks[2].get();
                    
                    for (auto state : otherStates) {
                        states[position] = state;
                        positions[state] = position;
                        stateToBlockMapping[state] = thirdBlock;
                        ++position;
                    }
                    thirdBlock->markAsSplitter();
                }
            }
            
            void Partition::swapStates(storm::storage::sparse::state_type state1, storm::storage::sparse::state_type state2) {
                std::swap(this->states[this->positions[state1]], this->states[this->positions[state2]]);
                std::swap(this->positions[state1], this->positions[state2]);
            }
            
            void Partition::swapStatesAtPositions(storm::storage::sparse::state_type position1, storm::storage::sparse::state_type position2) {
                storm::storage::sparse::state_type state1 = this->states[position1];
                storm::storage::sparse::state_type state2 = this->states[position2];
                
                std::swap(this->states[position1], this->states[position2]);
                this->positions[state1] = position2;
                this->positions[state2] = position1;
            }
            
            storm::storage::sparse::state_type const& Partition::getPosition(storm::storage::sparse::state_type state) const {
                return this->positions[state];
            }
            
            void Partition::setPosition(storm::storage::sparse::state_type state, storm::storage::sparse::state_type position) {
                this->positions[state] = position;
            }
            
            storm::storage::sparse::state_type const& Partition::getState(storm::storage::sparse::state_type position) const {
                return this->states[position];
            }
            
            void Partition::mapStatesToBlock(Block& block, std::vector<storm::storage::sparse::state_type>::iterator first, std::vector<storm::storage::sparse::state_type>::iterator last) {
                for (; first != last; ++first) {
                    this->stateToBlockMapping[*first] = &block;
                }
            }
            
            void Partition::mapStatesToPositions(Block const& block) {
                storm::storage::sparse::state_type position = block.getBeginIndex();
                for (auto stateIt = this->begin(block), stateIte = this->end(block); stateIt != stateIte; ++stateIt, ++position) {
                    this->positions[*stateIt] = position;
                }
            }
            
            Block& Partition::getBlock(storm::storage::sparse::state_type state) {
                return *this->stateToBlockMapping[state];
            }
            
            Block const& Partition::getBlock(storm::storage::sparse::state_type state) const {
                return *this->stateToBlockMapping[state];
            }

            std::vector<storm::storage::sparse::state_type>::iterator Partition::begin(Block const& block) {
                auto it = this->states.begin();
                std::advance(it, block.getBeginIndex());
                return it;
            }
            
            std::vector<storm::storage::sparse::state_type>::const_iterator Partition::begin(Block const& block) const {
                auto it = this->states.begin();
                std::advance(it, block.getBeginIndex());
                return it;
            }

            std::vector<storm::storage::sparse::state_type>::iterator Partition::end(Block const& block) {
                auto it = this->states.begin();
                std::advance(it, block.getEndIndex());
                return it;
            }

            std::vector<storm::storage::sparse::state_type>::const_iterator Partition::end(Block const& block) const {
                auto it = this->states.begin();
                std::advance(it, block.getEndIndex());
                return it;
            }
            
            std::pair<std::vector<std::unique_ptr<Block>>::iterator, bool> Partition::splitBlock(Block& block, storm::storage::sparse::state_type position) {
                STORM_LOG_THROW(position >= block.getBeginIndex() && position <= block.getEndIndex(), storm::exceptions::InvalidArgumentException, "Cannot split block at illegal position.");

                std::cout << "splitting " << block.getId() << " at pos " << position << " (was " << block.getBeginIndex() << " to " << block.getEndIndex() << ")" << std::endl;
                
                // In case one of the resulting blocks would be empty, we simply return the current block and do not create
                // a new one.
                if (position == block.getBeginIndex() || position == block.getEndIndex()) {
                    auto it = blocks.begin();
                    std::advance(it, block.getId());
                    return std::make_pair(it, false);
                }
                
                // Actually create the new block.
                blocks.emplace_back(new Block(block.getBeginIndex(), position, block.getPreviousBlockPointer(), &block, blocks.size()));
                auto newBlockIt = std::prev(blocks.end());
                
                // Resize the current block appropriately.
                std::cout << "setting begin pos of block " << block.getId() << " to " << position << std::endl;
                block.setBeginIndex(position);
                
                // Mark both blocks as splitters.
                block.markAsSplitter();
                (*newBlockIt)->markAsSplitter();
                
                // Update the mapping of the states in the newly created block.
                this->mapStatesToBlock(**newBlockIt, this->begin(**newBlockIt), this->end(**newBlockIt));
                
                return std::make_pair(newBlockIt, true);
            }
            
            bool Partition::splitBlock(Block& block, std::function<bool (storm::storage::sparse::state_type, storm::storage::sparse::state_type)> const& less, std::function<void (Block&)> const& newBlockCallback) {
                std::cout << "sorting the block [" << block.getId() << "]" << std::endl;
                // Sort the range of the block such that all states that have the label are moved to the front.
                std::sort(this->begin(block), this->end(block), less);
                
//                std::cout << "after" << std::endl;
//                for (auto it = this->begin(block), ite = this->end(block); it != ite; ++it) {
//                    std::cout << *it << " ";
//                }
//                std::cout << std::endl;
                
                // Update the positions vector.
                mapStatesToPositions(block);

//                for (auto it = this->positions.begin() + block.getBeginIndex(), ite = this->positions.begin() + block.getEndIndex(); it != ite; ++it) {
//                    std::cout << *it << " ";
//                }
//                std::cout << std::endl;
                
                // Now we can check whether the block needs to be split, which is the case iff the changed function returns
                // true for the first and last element of the remaining state range.
                storm::storage::sparse::state_type begin = block.getBeginIndex();
                storm::storage::sparse::state_type end = block.getEndIndex() - 1;
                bool wasSplit = false;
                while (less(states[begin], states[end])) {
                    wasSplit = true;
                    // Now we scan for the first state in the block for which the changed function returns true.
                    // Note that we do not have to check currentIndex for staying within bounds, because we know the matching
                    // state is within bounds.
                    storm::storage::sparse::state_type currentIndex = begin + 1;
                    while (begin != end && !less(states[begin], states[currentIndex])) {
                        ++currentIndex;
                    }
                    begin = currentIndex;
                    
                    auto result = this->splitBlock(block, currentIndex);
                    if (result.second) {
                        newBlockCallback(**result.first);
                    }
                }
                return wasSplit;
            }
            
            bool Partition::split(std::function<bool (storm::storage::sparse::state_type, storm::storage::sparse::state_type)> const& less, std::function<void (Block&)> const& newBlockCallback) {
                bool result = false;
                // Since the underlying storage of the blocks may change during iteration, we remember the current size
                // and iterate over indices. This assumes that new blocks will be added at the end of the blocks vector.
                std::size_t currentSize = this->size();
                for (uint_fast64_t index = 0; index < currentSize; ++index) {
                    result |= splitBlock(*blocks[index], less, newBlockCallback);
                }
                return result;
            }
            
            void Partition::splitStates(Block& block, storm::storage::BitVector const& states) {
                this->splitBlock(block, [&states] (storm::storage::sparse::state_type const& a, storm::storage::sparse::state_type const& b) { return states.get(a) && !states.get(b); });
            }
            
            void Partition::splitStates(storm::storage::BitVector const& states) {
                this->split([&states] (storm::storage::sparse::state_type const& a, storm::storage::sparse::state_type const& b) { return states.get(a) && !states.get(b); });
            }
            
            void Partition::sortBlock(Block const& block) {
                std::sort(this->begin(block), this->end(block), [] (storm::storage::sparse::state_type const& a, storm::storage::sparse::state_type const& b) { return a < b; });
                mapStatesToPositions(block);
            }
            
            Block& Partition::insertBlock(Block& block) {
                // Find the beginning of the new block.
                storm::storage::sparse::state_type begin = block.hasPreviousBlock() ? block.getPreviousBlock().getEndIndex() : 0;
                
                // Actually insert the new block.
                blocks.emplace_back(new Block(begin, block.getBeginIndex(), block.getPreviousBlockPointer(), &block, blocks.size()));
                Block& newBlock = *blocks.back();
                
                // Update the mapping of the states in the newly created block.
                for (auto it = this->begin(newBlock), ite = this->end(newBlock); it != ite; ++it) {
                    stateToBlockMapping[*it] = &newBlock;
                }
                
                return newBlock;
            }
            
            std::vector<std::unique_ptr<Block>> const& Partition::getBlocks() const {
                return this->blocks;
            }
            
            std::vector<std::unique_ptr<Block>>& Partition::getBlocks() {
                return this->blocks;
            }
            
            bool Partition::check() const {
                for (uint_fast64_t state = 0; state < this->positions.size(); ++state) {
                    STORM_LOG_ASSERT(this->states[this->positions[state]] == state, "Position mapping corrupted.");
                }
                for (auto const& blockPtr : this->blocks) {
                    STORM_LOG_ASSERT(blockPtr->check(), "Block corrupted.");
                    for (auto stateIt = this->begin(*blockPtr), stateIte = this->end(*blockPtr); stateIt != stateIte; ++stateIt) {
                        STORM_LOG_ASSERT(this->stateToBlockMapping[*stateIt] == blockPtr.get(), "Block mapping corrupted.");
                    }
                }
                return true;
            }
            
            void Partition::print() const {
                for (auto const& block : this->blocks) {
                    block->print(*this);
                }
                std::cout << "states in partition" << std::endl;
                for (auto const& state : states) {
                    std::cout << state << " ";
                }
                std::cout << std::endl << "positions: " << std::endl;
                for (auto const& index : positions) {
                    std::cout << index << " ";
                }
                std::cout << std::endl << "state to block mapping: " << std::endl;
                for (auto const& block : stateToBlockMapping) {
                    std::cout << block << "[" << block->getId() <<"] ";
                }
                std::cout << std::endl;
                std::cout << "size: " << blocks.size() << std::endl;
                STORM_LOG_ASSERT(this->check(), "Partition corrupted.");
            }
            
            std::size_t Partition::size() const {
                return blocks.size();
            }

        }
    }
}