#include "src/storage/BisimulationDecomposition2.h"

#include <algorithm>
#include <unordered_map>
#include <chrono>

namespace storm {
    namespace storage {
        
        template<typename ValueType>
        BisimulationDecomposition2<ValueType>::Block::Block(storm::storage::sparse::state_type begin, storm::storage::sparse::state_type end, Block* prev, Block* next) : begin(begin), end(end), prev(prev), next(next), numberOfStates(end - begin), isMarked(false) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        void BisimulationDecomposition2<ValueType>::Block::print(Partition const& partition) const {
            std::cout << "this " << this << std::endl;
            std::cout << "begin: " << this->begin << " and end: " << this->end << " (number of states: " << this->numberOfStates << ")" << std::endl;
            std::cout << "next: " << this->next << " and prev " << this->prev << std::endl;
            std::cout << "states:" << std::endl;
            for (storm::storage::sparse::state_type index = this->begin; index < this->end; ++index) {
                std::cout << partition.states[index] << " " << std::endl;
            }
        }
        
        template<typename ValueType>
        BisimulationDecomposition2<ValueType>::Partition::Partition(std::size_t numberOfStates) : stateToBlockMapping(numberOfStates), states(numberOfStates), positions(numberOfStates), values(numberOfStates) {
            this->blocks.back().itToSelf = blocks.emplace(this->blocks.end(), 0, numberOfStates, nullptr, nullptr);
            for (storm::storage::sparse::state_type state = 0; state < numberOfStates; ++state) {
                states[state] = state;
                positions[state] = state;
                stateToBlockMapping[state] = &blocks.back();
            }
        }
        
        template<typename ValueType>
        void BisimulationDecomposition2<ValueType>::Partition::splitLabel(storm::storage::BitVector const& statesWithLabel) {
            for (auto blockIterator = this->blocks.begin(), ite = this->blocks.end(); blockIterator != ite; ) { // The update of the loop was intentionally moved to the bottom of the loop.
                Block& block = *blockIterator;
                
                // Sort the range of the block such that all states that have the label are moved to the front.
                std::sort(this->states.begin() + block.begin, this->states.begin() + block.end, [&statesWithLabel] (storm::storage::sparse::state_type const& a, storm::storage::sparse::state_type const& b) { return statesWithLabel.get(a) && !statesWithLabel.get(b); } );
                
                // Update the positions vector.
                storm::storage::sparse::state_type position = block.begin;
                for (auto stateIt = this->states.begin() + block.begin, stateIte = this->states.begin() + block.end; stateIt != stateIte; ++stateIt, ++position) {
                    this->positions[*stateIt] = position;
                }
                
                // Now we can find the first position in the block that does not have the label and create new blocks.
                std::vector<storm::storage::sparse::state_type>::iterator it = std::find_if(this->states.begin() + block.begin, this->states.begin() + block.end, [&] (storm::storage::sparse::state_type const& a) { return !statesWithLabel.get(a); });
                
                // If not all the states agreed on the validity of the label, we need to split the block.
                if (it != this->states.begin() + block.begin && it != this->states.begin() + block.end) {
                    auto cutPoint = std::distance(this->states.begin(), it);

                    ++blockIterator;
                    auto newBlockIterator = this->blocks.emplace(blockIterator, cutPoint, block.end, &block, block.next);
                    newBlockIterator->itToSelf = newBlockIterator;
                    
                    // Make the old block end at the cut position and insert a new block after it.
                    block.end = cutPoint;
                    block.next = &(*newBlockIterator);
                    block.numberOfStates = block.end - block.begin;
                    
                    // Update the block mapping for all states that we just removed from the block.
                    for (auto it = this->states.begin() + newBlockIterator->begin, ite = this->states.begin() + newBlockIterator->end; it != ite; ++it) {
                        stateToBlockMapping[*it] = &(*newBlockIterator);
                    }
                } else {
                    // Otherwise, we simply proceed to the next block.
                    ++blockIterator;
                }
            }
        }
        
        template<typename ValueType>
        void BisimulationDecomposition2<ValueType>::Partition::print() const {
            for (auto const& block : this->blocks) {
                block.print(*this);
            }
            std::cout << "states" << std::endl;
            for (auto const& state : states) {
                std::cout << state << " ";
            }
            std::cout << std::endl << "positions: " << std::endl;
            for (auto const& index : positions) {
                std::cout << index << " ";
            }
            std::cout << std::endl << "state to block mapping: " << std::endl;
            for (auto const& block : stateToBlockMapping) {
                std::cout << block << " ";
            }
            std::cout << std::endl;
        }
        
        template<typename ValueType>
        std::size_t BisimulationDecomposition2<ValueType>::Partition::size() const {
            return this->blocks.size();
        }
        
        template<typename ValueType>
        BisimulationDecomposition2<ValueType>::BisimulationDecomposition2(storm::models::Dtmc<ValueType> const& dtmc, bool weak) {
            computeBisimulationEquivalenceClasses(dtmc, weak);
        }
        
        template<typename ValueType>
        void BisimulationDecomposition2<ValueType>::computeBisimulationEquivalenceClasses(storm::models::Dtmc<ValueType> const& dtmc, bool weak) {
            std::chrono::high_resolution_clock::time_point totalStart = std::chrono::high_resolution_clock::now();

            // We start by computing the initial partition.
            Partition partition(dtmc.getNumberOfStates());
            partition.print();
            for (auto const& label : dtmc.getStateLabeling().getAtomicPropositions()) {
                if (label == "init") {
                    continue;
                }
                partition.splitLabel(dtmc.getLabeledStates(label));
            }
            
            std::cout << "initial partition:" << std::endl;
            partition.print();
            
            // Initially, all blocks are potential splitter, so we insert them in the splitterQueue.
            std::deque<Block*> splitterQueue;
            std::for_each(partition.blocks.begin(), partition.blocks.end(), [&] (Block& a) { splitterQueue.push_back(&a); });
            
            storm::storage::SparseMatrix<ValueType> backwardTransitions = dtmc.getBackwardTransitions();
            
            // Then perform the actual splitting until there are no more splitters.
            while (!splitterQueue.empty()) {
                splitPartition(backwardTransitions, *splitterQueue.front(), partition, splitterQueue);
                splitterQueue.pop_front();
                
                std::cout << "####### updated partition ##############" << std::endl;
                partition.print();
                std::cout << "####### end of updated partition #######" << std::endl;
            }
            
            std::chrono::high_resolution_clock::duration totalTime = std::chrono::high_resolution_clock::now() - totalStart;
            std::cout << "Bisimulation took " << std::chrono::duration_cast<std::chrono::milliseconds>(totalTime).count() << "ms." << std::endl;
        }
        
        template<typename ValueType>
        std::size_t BisimulationDecomposition2<ValueType>::splitBlockProbabilities(Block* block, Partition& partition, std::deque<Block*>& splitterQueue) {
            Block& currentBlock = *block;
            
            // Sort the states in the block based on their probabilities.
            std::sort(partition.states.begin() + currentBlock.begin, partition.states.begin() + currentBlock.end, [&partition] (storm::storage::sparse::state_type const& a, storm::storage::sparse::state_type const& b) { return partition.values[a] < partition.values[b]; } );
            
            // FIXME: This can probably be done more efficiently.
            std::sort(partition.values.begin() + currentBlock.begin, partition.values.begin() + currentBlock.end);
            
            // Update the positions vector.
            storm::storage::sparse::state_type position = currentBlock.begin;
            for (auto stateIt = partition.states.begin() + currentBlock.begin, stateIte = partition.states.begin() + currentBlock.end; stateIt != stateIte; ++stateIt, ++position) {
                partition.positions[*stateIt] = position;
            }

            // Finally, we need to scan the ranges of states that agree on the probability.
            storm::storage::sparse::state_type beginIndex = currentBlock.begin;
            storm::storage::sparse::state_type currentIndex = beginIndex;
            storm::storage::sparse::state_type endIndex = currentBlock.end;
            
            Block* prevBlock = block->prev;
            
            std::list<Block*> createdBlocks;
            std::cout << currentIndex << " < " << endIndex << std::endl;
            while (currentIndex < endIndex) {
                ValueType& currentValue = *(partition.values.begin() + currentIndex);
                
                ++currentIndex;
                ValueType* nextValuePtr = &currentValue;
                while (currentIndex < endIndex && std::abs(currentValue - *nextValuePtr) < 1e-6) {
                    ++currentIndex;
                    ++nextValuePtr;
                }
                
                // Create a new block from the states that agree on the values.
                typename std::list<Block>::iterator newBlockIterator = partition.blocks.emplace(currentBlock.itToSelf, beginIndex, endIndex, prevBlock, currentBlock.next);
                newBlockIterator->itToSelf = newBlockIterator;
                if (prevBlock != nullptr) {
                    prevBlock->next = &(*newBlockIterator);
                }
                prevBlock = &(*newBlockIterator);
                if (prevBlock->numberOfStates > 1) {
                    createdBlocks.emplace_back(prevBlock);
                }
            }

            for (auto block : createdBlocks) {
                splitterQueue.push_back(block);
            }
            
            return createdBlocks.size();
        }
        
        template<typename ValueType>
        std::size_t BisimulationDecomposition2<ValueType>::splitPartition(storm::storage::SparseMatrix<ValueType> const& backwardTransitions, Block const& splitter, Partition& partition, std::deque<Block*>& splitterQueue) {
            std::cout << "getting block " << &splitter << " as splitter" << std::endl;
            splitter.print(partition);
            std::list<Block*> predecessorBlocks;
            
            // Iterate over all states of the splitter and check its predecessors.
            for (auto stateIterator = partition.states.begin() + splitter.begin, stateIte = partition.states.begin() + splitter.end; stateIterator != stateIte; ++stateIterator) {
                storm::storage::sparse::state_type& state = *stateIterator;
                
                for (auto const& predecessorEntry : backwardTransitions.getRow(state)) {
                    storm::storage::sparse::state_type predecessor = predecessorEntry.getColumn();
                    std::cout << "found pred " << predecessor << std::endl;
                    Block* predecessorBlock = partition.stateToBlockMapping[predecessor];
                    std::cout << "predecessor block " << std::endl;
                    predecessorBlock->print(partition);
                    
                    // If the predecessor block has just one state, there is no point in splitting it.
                    if (predecessorBlock->numberOfStates <= 1) {
                        std::cout << "continuing" << std::endl;
                        continue;
                    }
                    
                    storm::storage::sparse::state_type predecessorPosition = partition.positions[predecessor];
                    
                    // If we have not seen this predecessor before, we move it to a part near the beginning of the block.
                    std::cout << "predecessor position: " << predecessorPosition << " and begin " << predecessorBlock->begin << std::endl;
                    if (predecessorPosition >= predecessorBlock->begin) {
                        std::swap(partition.states[predecessorPosition], partition.states[predecessorBlock->begin]);
                        std::cout << "swapping positions of " << predecessor << " and " << partition.states[predecessorPosition] << std::endl;
                        storm::storage::sparse::state_type tmp = partition.positions[partition.states[predecessorPosition]];
                        partition.positions[partition.states[predecessorPosition]] = partition.positions[predecessor];
                        partition.positions[predecessor] = tmp;
                        
//                        std::swap(partition.positions[predecessor], partition.positions[predecessorBlock->begin]);
                        
                        ++predecessorBlock->begin;
                        std::cout << "incrementing begin... " << std::endl;
                        partition.values[predecessor] = predecessorEntry.getValue();
                    } else {
                        // Otherwise, we just need to update the probability for this predecessor.
                        std::cout << "updating probability" << std::endl;
                        partition.values[predecessor] += predecessorEntry.getValue();
                    }
                    
                    if (!predecessorBlock->isMarked) {
                        predecessorBlocks.emplace_back(predecessorBlock);
                        predecessorBlock->isMarked = true;
                    }
                }
            }
            
            std::list<Block*> blocksToSplit;
            
            // Now, we can iterate over the predecessor blocks and see whether we have to create a new block for
            // predecessors of the splitter.
            for (auto block : predecessorBlocks) {
                block->isMarked = false;
                
                // If we have moved the begin of the block to somewhere in the middle of the block, we need to split it.
                if (block->begin != block->end) {
                    std::cout << "moved begin to " << block->begin << " and end to " << block->end << std::endl;
                    storm::storage::sparse::state_type tmpBegin = block->begin;
                    storm::storage::sparse::state_type tmpEnd = block->end;
                    
                    block->begin = block->prev != nullptr ? block->prev->end : 0;
                    std::cout << "begin: " << block->begin << " and not-null? " << (block->prev != nullptr) << ": " << block->prev << std::endl;
                    block->end = tmpBegin;
                    block->numberOfStates = block->end - block->begin;
                    
                    // Create a new block that holds all states that do not have a successor in the current splitter.
                    typename std::list<Block>::iterator it = partition.blocks.emplace(block->next != nullptr ? block->next->itToSelf : partition.blocks.end(), tmpBegin, tmpEnd, block, block->next);
                    Block* newBlock = &(*it);
                    newBlock->itToSelf = it;
                    if (block->next != nullptr) {
                        block->next->prev = newBlock;
                    }
                    block->next = newBlock;
                    
                    std::cout << "created new block " << std::endl;
                    newBlock->print(partition);
                    
                    // Update the block mapping in the partition.
                    for (auto it = partition.states.begin() + newBlock->begin, ite = partition.states.begin() + newBlock->end; it != ite; ++it) {
                        partition.stateToBlockMapping[*it] = newBlock;
                    }
                    
                    // Mark the half of the block that can be further refined using the probability information.
                    blocksToSplit.emplace_back(block);
                    block->print(partition);
                    
                    splitterQueue.push_back(newBlock);
                } else {
                    std::cout << "found block to split" << std::endl;
                    blocksToSplit.emplace_back(block);
                }
            }
            
            // Finally, we walk through the blocks that have a transition to the splitter and split them using
            // probabilistic information.
            for (auto block : blocksToSplit) {
                if (block->numberOfStates <= 1) {
                    continue;
                }
                
                splitBlockProbabilities(block, partition, splitterQueue);
            }
            
            return 0;
        }
        
//        template<typename ValueType>
//        std::size_t BisimulationDecomposition2<ValueType>::splitPartition(storm::models::Dtmc<ValueType> const& dtmc, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::size_t const& blockId, std::vector<std::size_t>& stateToBlockMapping, storm::storage::BitVector& blocksInSplitterQueue, std::deque<std::size_t>& splitterQueue, bool weakBisimulation) {
//            std::chrono::high_resolution_clock::time_point totalStart = std::chrono::high_resolution_clock::now();
//            std::unordered_map<storm::storage::Distribution<ValueType>, typename BisimulationDecomposition2<ValueType>::block_type> distributionToNewBlocks;
//            
//            // Traverse all states of the block and check whether they have different distributions.
//            std::chrono::high_resolution_clock::time_point gatherStart = std::chrono::high_resolution_clock::now();
//            for (auto const& state : this->blocks[blockId]) {
//                // Now construct the distribution of this state wrt. to the current partitioning.
//                storm::storage::Distribution<ValueType> distribution;
//                for (auto const& successorEntry : dtmc.getTransitionMatrix().getRow(state)) {
//                    distribution.addProbability(static_cast<storm::storage::sparse::state_type>(stateToBlockMapping[successorEntry.getColumn()]), successorEntry.getValue());
//                }
//                
//                // If we are requested to compute a weak bisimulation, we need to scale the distribution with the
//                // self-loop probability.
//                if (weakBisimulation) {
//                    distribution.scale(blockId);
//                }
//                
//                // If the distribution already exists, we simply add the state. Otherwise, we open a new block.
//                auto distributionIterator = distributionToNewBlocks.find(distribution);
//                if (distributionIterator != distributionToNewBlocks.end()) {
//                    distributionIterator->second.insert(state);
//                } else {
//                    distributionToNewBlocks[distribution].insert(state);
//                }
//            }
//            
//            std::chrono::high_resolution_clock::duration gatherTime = std::chrono::high_resolution_clock::now() - gatherStart;
//            std::cout << "time to iterate over all states was " << std::chrono::duration_cast<std::chrono::milliseconds>(gatherTime).count() << "ms." << std::endl;
//            
//            // Now we are ready to split the block.
//            if (distributionToNewBlocks.size() == 1) {
//                // If there is just one behavior, we just set the distribution as the new one for this block.
//                // distributions[blockId] = std::move(distributionToNewBlocks.begin()->first);
//            } else {
//                // In this case, we need to split the block.
//                typename BisimulationDecomposition2<ValueType>::block_type tmpBlock;
//                
//                auto distributionIterator = distributionToNewBlocks.begin();
//                tmpBlock = std::move(distributionIterator->second);
//                std::swap(this->blocks[blockId], tmpBlock);
//                ++distributionIterator;
//                
//                // Remember the number of blocks prior to splitting for later use.
//                std::size_t beforeNumberOfBlocks = this->blocks.size();
//                
//                for (; distributionIterator != distributionToNewBlocks.end(); ++distributionIterator) {
//                    // In this case, we need to move the newly created block to the end of the list of actual blocks.
//                    this->blocks.emplace_back(std::move(distributionIterator->second));
//                    
//                    // Update the mapping of states to their blocks.
//                    std::size_t newBlockId = this->blocks.size() - 1;
//                    for (auto const& state : this->blocks.back()) {
//                        stateToBlockMapping[state] = newBlockId;
//                    }
//                }
//                
//                // Insert blocks that possibly need a refinement into the queue.
//                for (auto const& state : tmpBlock) {
//                    for (auto const& predecessor : backwardTransitions.getRow(state)) {
//                        if (!blocksInRefinementQueue.get(stateToBlockMapping[predecessor.getColumn()])) {
//                            blocksInRefinementQueue.set(stateToBlockMapping[predecessor.getColumn()]);
//                            refinementQueue.push_back(stateToBlockMapping[predecessor.getColumn()]);
//                        }
//                    }
//                }
//            }
//        
//            std::chrono::high_resolution_clock::duration totalTime = std::chrono::high_resolution_clock::now() - totalStart;
//            std::cout << "refinement of block " << blockId << " took " << std::chrono::duration_cast<std::chrono::milliseconds>(totalTime).count() << "ms." << std::endl;
//            return distributionToNewBlocks.size();
//        }
        
        template class BisimulationDecomposition2<double>;
    }
}