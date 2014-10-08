#include "src/storage/BisimulationDecomposition.h"

#include <unordered_map>
#include <chrono>

namespace storm {
    namespace storage {
        
        template<typename ValueType>
        BisimulationDecomposition<ValueType>::BisimulationDecomposition(storm::models::Dtmc<ValueType> const& dtmc, bool weak) {
            computeBisimulationEquivalenceClasses(dtmc, weak);
        }
        
        template<typename ValueType>
        void BisimulationDecomposition<ValueType>::computeBisimulationEquivalenceClasses(storm::models::Dtmc<ValueType> const& dtmc, bool weak) {
            std::chrono::high_resolution_clock::time_point totalStart = std::chrono::high_resolution_clock::now();
            // We start by computing the initial partition. In particular, we also keep a mapping of states to their blocks.
            std::vector<std::size_t> stateToBlockMapping(dtmc.getNumberOfStates());
            storm::storage::BitVector labeledStates = dtmc.getLabeledStates("observe0Greater1");
            this->blocks.emplace_back(labeledStates.begin(), labeledStates.end());
            std::for_each(labeledStates.begin(), labeledStates.end(), [&] (storm::storage::sparse::state_type const& state) { stateToBlockMapping[state] = 0; } );
            labeledStates.complement();
            this->blocks.emplace_back(labeledStates.begin(), labeledStates.end());
            std::for_each(labeledStates.begin(), labeledStates.end(), [&] (storm::storage::sparse::state_type const& state) { stateToBlockMapping[state] = 1; } );
            
            // Create empty distributions for the two initial blocks.
            std::vector<storm::storage::Distribution<ValueType>> distributions(2);
            
            // Retrieve the backward transitions to allow for better checking of states that need to be re-examined.
            storm::storage::SparseMatrix<ValueType> const& backwardTransitions = dtmc.getBackwardTransitions();
            
            // Initially, both blocks are potential splitters. A splitter is marked as a pair in which the first entry
            // is the ID of the parent block of the splitter and the second entry is the block ID of the splitter itself.
            std::deque<std::size_t> refinementQueue;
            storm::storage::BitVector blocksInRefinementQueue(this->size());
            refinementQueue.push_back(0);
            refinementQueue.push_back(1);
            
            // As long as there are blocks to refine, well, refine them.
            uint_fast64_t iteration = 0;
            while (!refinementQueue.empty()) {
                ++iteration;
                
                // Optionally sort the queue to potentially speed up the convergence.
                // std::sort(refinementQueue.begin(), refinementQueue.end(), [=] (std::size_t const& a, std::size_t const& b) { return this->blocks[a].size() > this->blocks[b].size(); });
                
                std::size_t currentBlock = refinementQueue.front();
                refinementQueue.pop_front();
                blocksInRefinementQueue.set(currentBlock, false);
                
                splitBlock(dtmc, backwardTransitions, currentBlock, stateToBlockMapping, distributions, blocksInRefinementQueue, refinementQueue);
            }
            
            std::chrono::high_resolution_clock::duration totalTime = std::chrono::high_resolution_clock::now() - totalStart;
            
            std::cout << "Bisimulation quotient has " << this->blocks.size() << " blocks and took " << iteration << " iterations and " << std::chrono::duration_cast<std::chrono::milliseconds>(totalTime).count() << "ms." << std::endl;
        }
        
        template<typename ValueType>
        std::size_t BisimulationDecomposition<ValueType>::splitPredecessorsGraphBased(storm::models::Dtmc<ValueType> const& dtmc, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::size_t const& blockId, std::vector<std::size_t>& stateToBlockMapping, std::vector<storm::storage::Distribution<ValueType>>& distributions, storm::storage::BitVector& blocksInRefinementQueue, std::deque<std::size_t>& graphRefinementQueue) {
            
            // This method tries to split the blocks of predecessors of the provided block by checking whether there is
            // a transition into the current block or not.
            std::unordered_map<std::size_t, typename BisimulationDecomposition<ValueType>::block_type> predecessorBlockToNewBlock;
            
            // Now for each predecessor block which state could actually reach the current block.
            for (auto const& state : this->blocks[blockId]) {
                for (auto const& predecessorEntry : backwardTransitions.getRow(state)) {
                    storm::storage::sparse::state_type predecessor = predecessorEntry.getColumn();
                    predecessorBlockToNewBlock[stateToBlockMapping[predecessor]].insert(predecessor);
                }
            }
            
            // Now, we can check for each predecessor block whether it needs to be split.
            for (auto const& blockNewBlockPair : predecessorBlockToNewBlock) {
                if (this->blocks[blockNewBlockPair.first].size() > blockNewBlockPair.second.size()) {
                    // Add the states which have a successor in the current block to a totally new block.
                    this->blocks.emplace_back(std::move(blockNewBlockPair.second));
                    
                    // Compute the set of states that remains in the old block;
                    typename BisimulationDecomposition<ValueType>::block_type newBlock;
                    std::set_difference(this->blocks[blockId].begin(), this->blocks[blockId].end(), this->blocks.back().begin(), this->blocks.back().end(), std::inserter(newBlock, newBlock.begin()));
                    this->blocks[blockNewBlockPair.first] = std::move(newBlock);
                    
                    blocksInRefinementQueue.resize(this->blocks.size());
                    
                    // Add the smaller part of the old block to the queue.
                    std::size_t blockToAddToQueue = this->blocks.back().size() < this->blocks[blockNewBlockPair.first].size() ? this->blocks.size() - 1 : blockNewBlockPair.first;
                    if (!blocksInRefinementQueue.get(blockToAddToQueue)) {
                        graphRefinementQueue.push_back(blockToAddToQueue);
                        blocksInRefinementQueue.set(blockToAddToQueue, true);
                    }
                }
            }
            
            // FIXME
            return 0;
        }
        
        template<typename ValueType>
        std::size_t BisimulationDecomposition<ValueType>::splitBlock(storm::models::Dtmc<ValueType> const& dtmc, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::size_t const& blockId, std::vector<std::size_t>& stateToBlockMapping, std::vector<storm::storage::Distribution<ValueType>>& distributions, storm::storage::BitVector& blocksInRefinementQueue, std::deque<std::size_t>& refinementQueue) {
            std::chrono::high_resolution_clock::time_point totalStart = std::chrono::high_resolution_clock::now();
            std::unordered_map<storm::storage::Distribution<ValueType>, typename BisimulationDecomposition<ValueType>::block_type> distributionToNewBlocks;
            
            // Traverse all states of the block and check whether they have different distributions.
            for (auto const& state : this->blocks[blockId]) {
                // Now construct the distribution of this state wrt. to the current partitioning.
                storm::storage::Distribution<ValueType> distribution;
                for (auto const& successorEntry : dtmc.getTransitionMatrix().getRow(state)) {
                    distribution.addProbability(static_cast<storm::storage::sparse::state_type>(stateToBlockMapping[successorEntry.getColumn()]), successorEntry.getValue());
                }
                
                // If the distribution already exists, we simply add the state. Otherwise, we open a new block.
                auto distributionIterator = distributionToNewBlocks.find(distribution);
                if (distributionIterator != distributionToNewBlocks.end()) {
                    distributionIterator->second.insert(state);
                } else {
                    distributionToNewBlocks[distribution].insert(state);
                }
            }
            
            // Now we are ready to split the block.
            if (distributionToNewBlocks.size() == 1) {
                // If there is just one behavior, we just set the distribution as the new one for this block.
                distributions[blockId] = std::move(distributionToNewBlocks.begin()->first);
            } else {
                // In this case, we need to split the block.
                typename BisimulationDecomposition<ValueType>::block_type tmpBlock;
                
                auto distributionIterator = distributionToNewBlocks.begin();
                distributions[blockId] = std::move(distributionIterator->first);
                tmpBlock = std::move(distributionIterator->second);
                std::swap(this->blocks[blockId], tmpBlock);
                ++distributionIterator;
                
                // Remember the number of blocks prior to splitting for later use.
                std::size_t beforeNumberOfBlocks = this->blocks.size();
                
                for (; distributionIterator != distributionToNewBlocks.end(); ++distributionIterator) {
                    // In this case, we need to move the newly created block to the end of the list of actual blocks.
                    this->blocks.emplace_back(std::move(distributionIterator->second));
                    distributions.emplace_back(std::move(distributionIterator->first));
                    
                    // Update the mapping of states to their blocks.
                    std::size_t newBlockId = this->blocks.size() - 1;
                    for (auto const& state : this->blocks.back()) {
                        stateToBlockMapping[state] = newBlockId;
                    }
                }
                
                // Now that we have split the block, we try to trigger a chain of graph-based splittings. That is, we
                // try to split the predecessors of the current block by checking whether they have some transition
                // to one given sub-block of the current block.
                std::deque<std::size_t> localRefinementQueue;
                storm::storage::BitVector blocksInLocalRefinementQueue(this->size());
                localRefinementQueue.push_back(blockId);
                for (std::size_t i = beforeNumberOfBlocks; i < this->blocks.size(); ++i) {
                    localRefinementQueue.push_back(i);
                }
                
                while (!localRefinementQueue.empty()) {
                    std::size_t currentBlock = localRefinementQueue.front();
                    localRefinementQueue.pop_front();
                    blocksInLocalRefinementQueue.set(currentBlock, false);
                    
                    splitPredecessorsGraphBased(dtmc, backwardTransitions, blockId, stateToBlockMapping, distributions, blocksInLocalRefinementQueue, localRefinementQueue);
                }
                
                // Since we created some new blocks, we need to extend the bit vector storing the blocks in the
                // refinement queue.
                blocksInRefinementQueue.resize(blocksInRefinementQueue.size() + (distributionToNewBlocks.size() - 1));
                
                // Insert blocks that possibly need a refinement into the queue.
                for (auto const& state : tmpBlock) {
                    for (auto const& predecessor : backwardTransitions.getRow(state)) {
                        if (!blocksInRefinementQueue.get(stateToBlockMapping[predecessor.getColumn()])) {
                            blocksInRefinementQueue.set(stateToBlockMapping[predecessor.getColumn()]);
                            refinementQueue.push_back(stateToBlockMapping[predecessor.getColumn()]);
                        }
                    }
                }
            }
            
            std::chrono::high_resolution_clock::duration totalTime = std::chrono::high_resolution_clock::now() - totalStart;
            std::cout << "refinement of block " << blockId << " took " << std::chrono::duration_cast<std::chrono::milliseconds>(totalTime).count() << "ms." << std::endl;
            return distributionToNewBlocks.size();
        }
        
        template class BisimulationDecomposition<double>;
    }
}