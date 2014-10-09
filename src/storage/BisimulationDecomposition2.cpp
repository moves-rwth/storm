#include "src/storage/BisimulationDecomposition2.h"

#include <algorithm>
#include <unordered_map>
#include <chrono>

namespace storm {
    namespace storage {
        
        template<typename ValueType>
        BisimulationDecomposition2<ValueType>::Block::Block(storm::storage::sparse::state_type begin, storm::storage::sparse::state_type end, Block* prev, Block* next) : begin(begin), end(end), prev(prev), next(next), numberOfStates(end - begin) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        BisimulationDecomposition2<ValueType>::Partition::Partition(std::size_t numberOfStates) : stateToBlockMapping(numberOfStates), states(numberOfStates), positions(numberOfStates), values(numberOfStates) {
            this->blocks.back().itToSelf = blocks.emplace(this->blocks.end(), 0, numberOfStates, nullptr);
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
                    auto newBlockIterator = this->blocks.emplace(blockIterator, cutPoint, block.end, &(*blockIterator), block.next);
                    
                    // Make the old block end at the cut position and insert a new block after it.
                    block.end = cutPoint;
                    block.next = &(*newBlockIterator);
                    block.numberOfStates = block.end - block.begin;
                } else {
                    // Otherwise, we simply proceed to the next block.
                    ++blockIterator;
                }
            }
        }
        
        template<typename ValueType>
        void BisimulationDecomposition2<ValueType>::Partition::print() const {
            for (auto const& block : this->blocks) {
                std::cout << "begin: " << block.begin << " and end: " << block.end << " (number of states: " << block.numberOfStates << ")" << std::endl;
                std::cout << "states:" << std::endl;
                for (storm::storage::sparse::state_type index = block.begin; index < block.end; ++index) {
                    std::cout << this->states[index] << " " << std::endl;
                }
            }
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
            }
            
            std::chrono::high_resolution_clock::duration totalTime = std::chrono::high_resolution_clock::now() - totalStart;
            std::cout << "Bisimulation took " << std::chrono::duration_cast<std::chrono::milliseconds>(totalTime).count() << "ms." << std::endl;
        }
        
        template<typename ValueType>
        std::size_t BisimulationDecomposition2<ValueType>::splitPartition(storm::storage::SparseMatrix<ValueType> const& backwardTransitions, Block const& splitter, Partition& partition, std::deque<Block*>& splitterQueue) {
            
            std::list<Block*> predecessorBlocks;
            
            // Iterate over all states of the splitter and check its predecessors.
            for (auto stateIterator = partition.states.begin() + splitter.begin, stateIte = partition.states.begin() + splitter.end; stateIterator != stateIte; ++stateIterator) {
                storm::storage::sparse::state_type& state = *stateIterator;
                
                for (auto const& predecessorEntry : backwardTransitions.getRow(state)) {
                    storm::storage::sparse::state_type predecessor = predecessorEntry.getColumn();
                    Block* predecessorBlock = partition.stateToBlockMapping[predecessor];
                    storm::storage::sparse::state_type predecessorPosition = partition.positions[predecessor];
                    
                    // If we have not seen this predecessor before, we move it to a part near the beginning of the block.
                    if (predecessorPosition < predecessorBlock->begin) {
                        std::swap(partition.states[predecessorPosition], partition.states[predecessorBlock->begin]);
                        std::swap(partition.positions[predecessor], partition.positions[predecessorBlock->begin]);
                        ++predecessorBlock->begin;
                        partition.values[predecessor] = predecessorEntry.getValue();
                    } else {
                        // Otherwise, we just need to update the probability for this predecessor.
                        partition.values[predecessor] += predecessorEntry.getValue();
                    }
                    
                    if (!predecessorBlock->isMarked) {
                        predecessorBlocks.emplace_back(predecessorBlock);
                        predecessorBlock->isMarked = true;
                    }
                }
            }
            
            // Now, we can iterate over the predecessor blocks and see whether we have to create a new block for
            // predecessors of the splitter.
            for (auto block : predecessorBlocks) {
                // If we have moved the begin of the block to somewhere in the middle of the block, we need to split it.
                if (block->begin != block->end) {
                    
                    
                    storm::storage::sparse::state_type tmpBegin = block->begin;
                    storm::storage::sparse::state_type tmpEnd = block->end;
                    
                    
                }
                
                
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