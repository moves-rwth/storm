#include "src/storage/bisimulation/DeterministicModelBisimulationDecomposition.h"

#include <algorithm>
#include <unordered_map>
#include <chrono>
#include <iomanip>
#include <boost/iterator/transform_iterator.hpp>

#include "src/adapters/CarlAdapter.h"
#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"

#include "src/models/sparse/Dtmc.h"
#include "src/models/sparse/Ctmc.h"
#include "src/models/sparse/StandardRewardModel.h"

#include "src/utility/graph.h"
#include "src/utility/constants.h"
#include "src/utility/ConstantsComparator.h"
#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/exceptions/InvalidArgumentException.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/modules/GeneralSettings.h"

namespace storm {
    namespace storage {
        
        using namespace bisimulation;
        
        template<typename ModelType>
        DeterministicModelBisimulationDecomposition<ModelType>::DeterministicModelBisimulationDecomposition(ModelType const& model, typename BisimulationDecomposition<ModelType, DeterministicModelBisimulationDecomposition::BlockDataType>::Options const& options) : BisimulationDecomposition<ModelType, DeterministicModelBisimulationDecomposition::BlockDataType>(model, options), probabilitiesToCurrentSplitter(model.getNumberOfStates(), storm::utility::zero<ValueType>()) {
            STORM_LOG_THROW(!options.keepRewards || !model.hasRewardModel() || model.hasUniqueRewardModel(), storm::exceptions::IllegalFunctionCallException, "Bisimulation currently only supports models with at most one reward model.");
            STORM_LOG_THROW(!options.keepRewards || !model.hasRewardModel() || model.getUniqueRewardModel()->second.hasOnlyStateRewards(), storm::exceptions::IllegalFunctionCallException, "Bisimulation is currently supported for models with state rewards only. Consider converting the transition rewards to state rewards (via suitable function calls).");
            STORM_LOG_THROW(options.type != BisimulationType::Weak || !options.bounded, storm::exceptions::IllegalFunctionCallException, "Weak bisimulation cannot preserve bounded properties.");
        }
        
        template<typename ModelType>
        std::pair<storm::storage::BitVector, storm::storage::BitVector> DeterministicModelBisimulationDecomposition<ModelType>::getStatesWithProbability01() {
            return storm::utility::graph::performProb01(this->backwardTransitions, this->options.phiStates.get(), this->options.psiStates.get());
        }
        
        template<typename ModelType>
        void DeterministicModelBisimulationDecomposition<ModelType>::splitOffDivergentStates() {
            std::vector<storm::storage::sparse::state_type> stateStack;
            stateStack.reserve(this->model.getNumberOfStates());
            storm::storage::BitVector nondivergentStates(this->model.getNumberOfStates());
            
            for (auto const& blockPtr : this->partition.getBlocks()) {
                nondivergentStates.clear();
                
                for (auto stateIt = this->partition.begin(*blockPtr), stateIte = this->partition.end(*blockPtr); stateIt != stateIte; ++stateIt) {
                    if (nondivergentStates.get(*stateIt)) {
                        continue;
                    }
                    
                    // Now traverse the forward transitions of the current state and check whether there is a
                    // transition to some other block.
                    bool isDirectlyNonDivergent = false;
                    for (auto const& successor : this->model.getRows(*stateIt)) {
                        // If there is such a transition, then we can mark all states in the current block that can
                        // reach the state as non-divergent.
                        if (this->partition.getBlock(successor.getColumn()) != *blockPtr) {
                            isDirectlyNonDivergent = true;
                            break;
                        }
                    }
                    
                    if (isDirectlyNonDivergent) {
                        stateStack.push_back(*stateIt);
                        
                        while (!stateStack.empty()) {
                            storm::storage::sparse::state_type currentState = stateStack.back();
                            stateStack.pop_back();
                            nondivergentStates.set(currentState);
                            
                            for (auto const& predecessor : this->backwardTransitions.getRow(currentState)) {
                                if (this->partition.getBlock(predecessor.getColumn()) == *blockPtr && !nondivergentStates.get(predecessor.getColumn())) {
                                    stateStack.push_back(predecessor.getColumn());
                                }
                            }
                        }
                    }
                }
                
                
                if (nondivergentStates.getNumberOfSetBits() > 0 && nondivergentStates.getNumberOfSetBits() != blockPtr->getNumberOfStates()) {
                    // After performing the split, the current block will contain the divergent states only.
                    this->partition.splitStates(*blockPtr, nondivergentStates);
                    
                    // Since the remaining states in the block are divergent, we can mark the block as absorbing.
                    // This also guarantees that the self-loop will be added to the state of the quotient
                    // representing this block of states.
                    blockPtr->data().setAbsorbing(true);
                } else if (nondivergentStates.getNumberOfSetBits() == 0) {
                    // If there are only diverging states in the block, we need to make it absorbing.
                    blockPtr->data().setAbsorbing(true);
                }
            }
        }
        
        template<typename ModelType>
        void DeterministicModelBisimulationDecomposition<ModelType>::initializeSilentProbabilities() {
            silentProbabilities.resize(this->model.getNumberOfStates(), storm::utility::zero<ValueType>());
            for (storm::storage::sparse::state_type state = 0; state < this->model.getNumberOfStates(); ++state) {
                Block<BlockDataType> const* currentBlockPtr = &this->partition.getBlock(state);
                for (auto const& successorEntry : this->model.getRows(state)) {
                    if (&this->partition.getBlock(successorEntry.getColumn()) == currentBlockPtr) {
                        silentProbabilities[state] += successorEntry.getValue();
                    }
                }
            }
        }
        
        template<typename ModelType>
        void DeterministicModelBisimulationDecomposition<ModelType>::initializeWeakDtmcBisimulation() {
            // If we are creating the initial partition for weak bisimulation on DTMCs, we need to (a) split off all
            // divergent states of each initial block and (b) initialize the vector of silent probabilities.
            this->splitOffDivergentStates();
            this->initializeSilentProbabilities();
        }
        
        template<typename ModelType>
        void DeterministicModelBisimulationDecomposition<ModelType>::initializeMeasureDrivenPartition() {
            BisimulationDecomposition<ModelType, BlockDataType>::initializeMeasureDrivenPartition();
            
            if (this->options.type == BisimulationType::Weak && this->model.getType() == storm::models::ModelType::Dtmc) {
                this->initializeWeakDtmcBisimulation();
            }
        }
        
        template<typename ModelType>
        void DeterministicModelBisimulationDecomposition<ModelType>::initializeLabelBasedPartition() {
            BisimulationDecomposition<ModelType, BlockDataType>::initializeLabelBasedPartition();
            
            if (this->options.type == BisimulationType::Weak && this->model.getType() == storm::models::ModelType::Dtmc) {
                this->initializeWeakDtmcBisimulation();
            }
        }
        
        template<typename ModelType>
        typename DeterministicModelBisimulationDecomposition<ModelType>::ValueType const& DeterministicModelBisimulationDecomposition<ModelType>::getProbabilityToSplitter(storm::storage::sparse::state_type const& state) const {
            return probabilitiesToCurrentSplitter[state];
        }
        
        template<typename ModelType>
        bool DeterministicModelBisimulationDecomposition<ModelType>::isSilent(storm::storage::sparse::state_type const& state) const {
            return this->comparator.isOne(silentProbabilities[state]);
        }
        
        template<typename ModelType>
        typename DeterministicModelBisimulationDecomposition<ModelType>::ValueType DeterministicModelBisimulationDecomposition<ModelType>::getSilentProbability(storm::storage::sparse::state_type const& state) const {
            return silentProbabilities[state];
        }
        
        template<typename ModelType>
        void DeterministicModelBisimulationDecomposition<ModelType>::refinePredecessorBlocksOfSplitter(std::list<Block<BlockDataType>*>& predecessorBlocks, std::deque<bisimulation::Block<BlockDataType>*>& splitterQueue) {
            for (auto block : predecessorBlocks) {
                // Depending on the actions we need to take, the block to refine changes, so we need to keep track of it.
                Block<BlockDataType>* blockToRefineProbabilistically = block;
                
                bool split = false;
                // If the new begin index has shifted to a non-trivial position, we need to split the block.
                if (block->getBeginIndex() != block->data().marker1() && block->getEndIndex() != block->data().marker1()) {
                    auto result = this->partition.splitBlock(*block, block->data().marker1());
                    STORM_LOG_ASSERT(result.second, "Expected to split block, but that did not happen.");
                    split = true;
                    blockToRefineProbabilistically = block->getPreviousBlockPointer();
                }
                
                split |= this->partition.splitBlock(*blockToRefineProbabilistically,
                                                    [this] (storm::storage::sparse::state_type state1, storm::storage::sparse::state_type state2) {
                                                        return getProbabilityToSplitter(state1) < getProbabilityToSplitter(state2);
                                                    },
                                                    [&splitterQueue] (Block<BlockDataType>& block) {
                                                        splitterQueue.emplace_back(&block); block.data().setSplitter();
                                                    });
                
                
                // If the predecessor block was split, we need to insert it into the splitter queue if it is not already
                // marked as a splitter.
                if (split && !blockToRefineProbabilistically->data().splitter()) {
                    splitterQueue.emplace_back(blockToRefineProbabilistically);
                    blockToRefineProbabilistically->data().setSplitter();
                }
                
                // If the block was *not* split, we need to reset the markers by notifying the data.
                block->resetMarkers();
                
                // Remember that we have refined the block.
                block->data().setNeedsRefinement(false);
            }
        }
        
        template<typename ModelType>
        bool DeterministicModelBisimulationDecomposition<ModelType>::possiblyNeedsRefinement(bisimulation::Block<BlockDataType> const& predecessorBlock) const {
            return predecessorBlock.getNumberOfStates() > 1 && !predecessorBlock.data().absorbing();
        }
        
        template<typename ModelType>
        void DeterministicModelBisimulationDecomposition<ModelType>::increaseProbabilityToSplitter(storm::storage::sparse::state_type predecessor, bisimulation::Block<BlockDataType> const& predecessorBlock, ValueType const& value) {
            storm::storage::sparse::state_type predecessorPosition = this->partition.getPosition(predecessor);
            
            // If the position of the state is to the right of marker1, we have not seen it before.
            if (predecessorPosition >= predecessorBlock.data().marker1()) {
                // Then, we just set the value.
                probabilitiesToCurrentSplitter[predecessor] = value;
            } else {
                // If the state was seen as a predecessor before, we add the value to the existing value.
                probabilitiesToCurrentSplitter[predecessor] += value;
            }
        }
        
        template <typename ModelType>
        void DeterministicModelBisimulationDecomposition<ModelType>::moveStateToMarker1(storm::storage::sparse::state_type predecessor, bisimulation::Block<BlockDataType>& predecessorBlock) {
            this->partition.swapStates(predecessor, this->partition.getState(predecessorBlock.data().marker1()));
            predecessorBlock.data().incrementMarker1();
        }
        
        template <typename ModelType>
        void DeterministicModelBisimulationDecomposition<ModelType>::moveStateToMarker2(storm::storage::sparse::state_type predecessor, bisimulation::Block<BlockDataType>& predecessorBlock) {
            this->partition.swapStates(predecessor, this->partition.getState(predecessorBlock.data().marker2()));
            predecessorBlock.data().incrementMarker2();
        }
        
        template <typename ModelType>
        void DeterministicModelBisimulationDecomposition<ModelType>::moveStateInSplitter(storm::storage::sparse::state_type predecessor, bisimulation::Block<BlockDataType>& predecessorBlock, storm::storage::sparse::state_type currentPositionInSplitter, uint_fast64_t& elementsToSkip) {
            storm::storage::sparse::state_type predecessorPosition = this->partition.getPosition(predecessor);
            
            // If the predecessors of the given predecessor were already explored, we can move it easily.
            if (predecessorPosition <= currentPositionInSplitter + elementsToSkip) {
                this->partition.swapStates(predecessor, this->partition.getState(predecessorBlock.data().marker1()));
                predecessorBlock.data().incrementMarker1();
            } else {
                // Otherwise, we need to move the predecessor, but we need to make sure that we explore its
                // predecessors later. We do this by moving it to a range at the beginning of the block that will hold
                // all predecessors in the splitter whose predecessors have yet to be explored.
                if (predecessorBlock.data().marker2() == predecessorBlock.data().marker1()) {
                    this->partition.swapStatesAtPositions(predecessorBlock.data().marker2(), predecessorPosition);
                    this->partition.swapStatesAtPositions(predecessorPosition, currentPositionInSplitter + elementsToSkip + 1);
                } else {
                    this->partition.swapStatesAtPositions(predecessorBlock.data().marker2(), predecessorPosition);
                    this->partition.swapStatesAtPositions(predecessorPosition, predecessorBlock.data().marker1());
                    this->partition.swapStatesAtPositions(predecessorPosition, currentPositionInSplitter + elementsToSkip + 1);
                }
                
                // Since we had to move an already explored state to the right of the current position, 
                ++elementsToSkip;
                predecessorBlock.data().incrementMarker1();
                predecessorBlock.data().incrementMarker2();
            }
        }
        
        template <typename ModelType>
        void DeterministicModelBisimulationDecomposition<ModelType>::exploreRemainingStatesOfSplitter(bisimulation::Block<BlockDataType>& splitter) {
            for (auto splitterIt = this->partition.begin(splitter), splitterIte = this->partition.begin(splitter) + (splitter.data().marker2() - splitter.getBeginIndex()); splitterIt != splitterIte; ++splitterIt) {
                storm::storage::sparse::state_type currentState = *splitterIt;
                
                for (auto const& predecessorEntry : this->backwardTransitions.getRow(currentState)) {
                    storm::storage::sparse::state_type predecessor = predecessorEntry.getColumn();
                    Block<BlockDataType>& predecessorBlock = this->partition.getBlock(predecessor);
                    
                    // If the block does not need to be refined, we skip it.
                    if (!possiblyNeedsRefinement(predecessorBlock)) {
                        continue;
                    }
                    
                    // We keep track of the probability of the predecessor moving to the splitter.
                    increaseProbabilityToSplitter(predecessor, predecessorBlock, predecessorEntry.getValue());

                    // Only move the state if it has not been seen as a predecessor before.
                    storm::storage::sparse::state_type predecessorPosition = this->partition.getPosition(predecessor);
                    if (predecessorPosition >= predecessorBlock.data().marker1()) {
                        moveStateToMarker1(predecessor, predecessorBlock);
                    }
                }
            }
            
            // Finally, we can reset the second marker.
            splitter.data().setMarker2(splitter.getBeginIndex());
        }
        
        template<typename ModelType>
        void DeterministicModelBisimulationDecomposition<ModelType>::refinePartitionBasedOnSplitter(bisimulation::Block<BlockDataType>& splitter, std::deque<bisimulation::Block<BlockDataType>*>& splitterQueue) {
            // The outline of the refinement is as follows.
            //
            // We iterate over all states of the splitter and determine for each predecessor the state the probability
            // entering the splitter. These probabilities are written to a member vector so that after the iteration
            // process we have the probabilities of all predecessors of the splitter of entering the splitter in one
            // step. To directly separate the states having a transition into the splitter from the ones who do not,
            // we move the states to certain locations. That is, on encountering a predecessor of the splitter, it is
            // moved to the beginning of its block. If the predecessor is in the splitter itself, we have to be a bit
            // careful about where to move states.
            //
            // After this iteration, there may be states of the splitter whose predecessors have not yet been explored,
            // so this needs to be done now.
            //
            // Finally, we use the information obtained in the first part for the actual splitting process in which all
            // predecessor blocks of the splitter are split based on the probabilities computed earlier.
            
            // (1)
            std::list<Block<BlockDataType>*> predecessorBlocks;
            
            // (2)
            storm::storage::sparse::state_type currentPosition = splitter.getBeginIndex();
            bool splitterIsPredecessorBlock = false;
            for (auto splitterIt = this->partition.begin(splitter), splitterIte = this->partition.end(splitter); splitterIt != splitterIte; ++splitterIt, ++currentPosition) {
                storm::storage::sparse::state_type currentState = *splitterIt;
                
                uint_fast64_t elementsToSkip = 0;
                for (auto const& predecessorEntry : this->backwardTransitions.getRow(currentState)) {
                    storm::storage::sparse::state_type predecessor = predecessorEntry.getColumn();
                    storm::storage::sparse::state_type predecessorPosition = this->partition.getPosition(predecessor);
                    Block<BlockDataType>& predecessorBlock = this->partition.getBlock(predecessor);
                    
                    // If the block does not need to be refined, we skip it.
                    if (!possiblyNeedsRefinement(predecessorBlock)) {
                        continue;
                    }
                    
                    // We keep track of the probability of the predecessor moving to the splitter.
                    increaseProbabilityToSplitter(predecessor, predecessorBlock, predecessorEntry.getValue());
                    
                    // We only need to move the predecessor if its not already known as a predecessor already.
                    if (predecessorPosition >= predecessorBlock.data().marker1()) {
                        // If the predecessor block is not the splitter, we can move the state easily.
                        if (predecessorBlock != splitter) {
                            moveStateToMarker1(predecessor, predecessorBlock);
                        } else {
                            // If the predecessor is in the splitter, we need to be a bit more careful.
                            splitterIsPredecessorBlock = true;
                            moveStateInSplitter(predecessor, predecessorBlock, currentPosition, elementsToSkip);
                        }

                        // Insert the block into the list of blocks to refine (if that has not already happened).
                        if (!predecessorBlock.data().needsRefinement()) {
                            predecessorBlocks.emplace_back(&predecessorBlock);
                            predecessorBlock.data().setNeedsRefinement();
                        }
                    }
                }
                
                // If, as a consequence of shifting states, we need to skip some elements, do so now.
                splitterIt += elementsToSkip;
                currentPosition += elementsToSkip;
            }
            
            // If the splitter was a predecessor block of itself, we potentially need to explore some states that have
            // not been explored yet.
            if (splitterIsPredecessorBlock) {
                exploreRemainingStatesOfSplitter(splitter);
            }
            
            // Finally, we split the block based on the precomputed probabilities and the chosen bisimulation type.
            if (this->options.type == BisimulationType::Strong || this->model.getType() == storm::models::ModelType::Ctmc) {
                refinePredecessorBlocksOfSplitter(predecessorBlocks, splitterQueue);
            } else {
                assert(false);
            }
        }
        
        template<typename ModelType>
        void DeterministicModelBisimulationDecomposition<ModelType>::buildQuotient() {
            // In order to create the quotient model, we need to construct
            // (a) the new transition matrix,
            // (b) the new labeling,
            // (c) the new reward structures.
            
            // Prepare a matrix builder for (a).
            storm::storage::SparseMatrixBuilder<ValueType> builder(this->size(), this->size());
            
            // Prepare the new state labeling for (b).
            storm::models::sparse::StateLabeling newLabeling(this->size());
            std::set<std::string> atomicPropositionsSet = this->options.respectedAtomicPropositions.get();
            atomicPropositionsSet.insert("init");
            std::vector<std::string> atomicPropositions = std::vector<std::string>(atomicPropositionsSet.begin(), atomicPropositionsSet.end());
            for (auto const& ap : atomicPropositions) {
                newLabeling.addLabel(ap);
            }
            
            // If the model had state rewards, we need to build the state rewards for the quotient as well.
            boost::optional<std::vector<ValueType>> stateRewards;
            if (this->options.keepRewards && this->model.hasRewardModel()) {
                stateRewards = std::vector<ValueType>(this->blocks.size());
            }
            
            // Now build (a) and (b) by traversing all blocks.
            for (uint_fast64_t blockIndex = 0; blockIndex < this->blocks.size(); ++blockIndex) {
                auto const& block = this->blocks[blockIndex];
                
                // Pick one representative state. For strong bisimulation it doesn't matter which state it is, because
                // they all behave equally.
                storm::storage::sparse::state_type representativeState = *block.begin();
                
                // However, for weak bisimulation, we need to make sure the representative state is a non-silent one (if
                // there is any such state).
                if (this->options.type == BisimulationType::Weak) {
                    for (auto const& state : block) {
                        if (!isSilent(state)) {
                            representativeState = state;
                            break;
                        }
                    }
                }
                
                Block<BlockDataType> const& oldBlock = this->partition.getBlock(representativeState);
                
                // If the block is absorbing, we simply add a self-loop.
                if (oldBlock.data().absorbing()) {
                    builder.addNextValue(blockIndex, blockIndex, storm::utility::one<ValueType>());
                    
                    // If the block has a special representative state, we retrieve it now.
                    if (oldBlock.data().hasRepresentativeState()) {
                        representativeState = oldBlock.data().representativeState();
                    }
                    
                    // Add all of the selected atomic propositions that hold in the representative state to the state
                    // representing the block.
                    for (auto const& ap : atomicPropositions) {
                        if (this->model.getStateLabeling().getStateHasLabel(ap, representativeState)) {
                            newLabeling.addLabelToState(ap, blockIndex);
                        }
                    }
                } else {
                    // Compute the outgoing transitions of the block.
                    std::map<storm::storage::sparse::state_type, ValueType> blockProbability;
                    for (auto const& entry : this->model.getTransitionMatrix().getRow(representativeState)) {
                        storm::storage::sparse::state_type targetBlock = this->partition.getBlock(entry.getColumn()).getId();
                        
                        // If we are computing a weak bisimulation quotient, there is no need to add self-loops.
                        if ((this->options.type == BisimulationType::Weak) && targetBlock == blockIndex) {
                            continue;
                        }
                        
                        auto probIterator = blockProbability.find(targetBlock);
                        if (probIterator != blockProbability.end()) {
                            probIterator->second += entry.getValue();
                        } else {
                            blockProbability[targetBlock] = entry.getValue();
                        }
                    }
                    
                    // Now add them to the actual matrix.
                    for (auto const& probabilityEntry : blockProbability) {
                        if (this->options.type == BisimulationType::Weak && this->model.getType() == storm::models::ModelType::Dtmc) {
                            builder.addNextValue(blockIndex, probabilityEntry.first, probabilityEntry.second / (storm::utility::one<ValueType>() - getSilentProbability(representativeState)));
                        } else {
                            builder.addNextValue(blockIndex, probabilityEntry.first, probabilityEntry.second);
                        }
                    }
                    
                    // Otherwise add all atomic propositions to the equivalence class that the representative state
                    // satisfies.
                    for (auto const& ap : atomicPropositions) {
                        if (this->model.getStateLabeling().getStateHasLabel(ap, representativeState)) {
                            newLabeling.addLabelToState(ap, blockIndex);
                        }
                    }
                }
                
                // If the model has state rewards, we simply copy the state reward of the representative state, because
                // all states in a block are guaranteed to have the same state reward.
                if (this->options.keepRewards && this->model.hasRewardModel()) {
                    typename std::unordered_map<std::string, typename ModelType::RewardModelType>::const_iterator nameRewardModelPair = this->model.getUniqueRewardModel();
                    stateRewards.get()[blockIndex] = nameRewardModelPair->second.getStateRewardVector()[representativeState];
                }
            }
            
            // Now check which of the blocks of the partition contain at least one initial state.
            for (auto initialState : this->model.getInitialStates()) {
                Block<BlockDataType> const& initialBlock = this->partition.getBlock(initialState);
                newLabeling.addLabelToState("init", initialBlock.getId());
            }
            
            // Construct the reward model mapping.
            std::unordered_map<std::string, typename ModelType::RewardModelType> rewardModels;
            if (this->options.keepRewards && this->model.hasRewardModel()) {
                typename std::unordered_map<std::string, typename ModelType::RewardModelType>::const_iterator nameRewardModelPair = this->model.getUniqueRewardModel();
                rewardModels.insert(std::make_pair(nameRewardModelPair->first, typename ModelType::RewardModelType(stateRewards)));
            }
            
            // Finally construct the quotient model.
            this->quotient = std::shared_ptr<ModelType>(new ModelType(builder.build(), std::move(newLabeling), std::move(rewardModels)));
        }
        
        template class DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>>;
        template class DeterministicModelBisimulationDecomposition<storm::models::sparse::Ctmc<double>>;
        
#ifdef STORM_HAVE_CARL
        template class DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<storm::RationalFunction>>;
        template class DeterministicModelBisimulationDecomposition<storm::models::sparse::Ctmc<storm::RationalFunction>>;
#endif
    }
}
