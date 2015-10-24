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
#include "src/exceptions/InvalidOptionException.h"
#include "src/exceptions/InvalidArgumentException.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/modules/GeneralSettings.h"

namespace storm {
    namespace storage {
        
        using namespace bisimulation;
        
        template<typename ModelType>
        DeterministicModelBisimulationDecomposition<ModelType>::DeterministicModelBisimulationDecomposition(ModelType const& model, typename BisimulationDecomposition<ModelType>::Options const& options) : BisimulationDecomposition<ModelType>(model, options.weak ? BisimulationType::Weak : BisimulationType::Strong) {
            STORM_LOG_THROW(!model.hasRewardModel() || model.hasUniqueRewardModel(), storm::exceptions::IllegalFunctionCallException, "Bisimulation currently only supports models with at most one reward model.");
            STORM_LOG_THROW(!model.hasRewardModel() || model.getUniqueRewardModel()->second.hasOnlyStateRewards(), storm::exceptions::IllegalFunctionCallException, "Bisimulation is currently supported for models with state rewards only. Consider converting the transition rewards to state rewards (via suitable function calls).");
            STORM_LOG_THROW(!options.weak || !options.bounded, storm::exceptions::IllegalFunctionCallException, "Weak bisimulation cannot preserve bounded properties.");

            // Extract the set of respected atomic propositions from the options.
            if (options.respectedAtomicPropositions) {
                this->selectedAtomicPropositions = options.respectedAtomicPropositions.get();
            } else {
                this->selectedAtomicPropositions = model.getStateLabeling().getLabels();
            }
            
            // initialize the initial partition.
            if (options.measureDrivenInitialPartition) {
                STORM_LOG_THROW(options.phiStates, storm::exceptions::InvalidOptionException, "Unable to compute measure-driven initial partition without phi states.");
                STORM_LOG_THROW(options.psiStates, storm::exceptions::InvalidOptionException, "Unable to compute measure-driven initial partition without psi states.");
                this->initializeMeasureDrivenPartition();
            } else {
                this->initializeLabelBasedPartition();
            }
            
            this->computeBisimulationDecomposition();
        }
        
        template<typename ModelType>
        std::pair<storm::storage::BitVector, storm::storage::BitVector> DeterministicModelBisimulationDecomposition<ModelType>::getStatesWithProbability01(storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) {
            return storm::utility::graph::performProb01(backwardTransitions, phiStates, psiStates);
        }
        
        template<typename ModelType>
        void DeterministicModelBisimulationDecomposition<ModelType>::splitOffDivergentStates() {
            std::vector<storm::storage::sparse::state_type> stateStack;
            stateStack.reserve(this->model.getNumberOfStates());
            storm::storage::BitVector nondivergentStates(this->model.getNumberOfStates());
            
            for (auto& block : this->partition.getBlocks()) {
                nondivergentStates.clear();
                
                for (auto stateIt = this->partition.begin(block), stateIte = this->partition.end(block); stateIt != stateIte; ++stateIt) {
                    if (nondivergentStates.get(stateIt->first)) {
                        continue;
                    }
                    
                    // Now traverse the forward transitions of the current state and check whether there is a
                    // transition to some other block.
                    bool isDirectlyNonDivergent = false;
                    for (auto const& successor : this->model.getRows(stateIt->first)) {
                        // If there is such a transition, then we can mark all states in the current block that can
                        // reach the state as non-divergent.
                        if (&this->partition.getBlock(successor.getColumn()) != &block) {
                            isDirectlyNonDivergent = true;
                            break;
                        }
                    }
                    
                    if (isDirectlyNonDivergent) {
                        stateStack.push_back(stateIt->first);
                        
                        while (!stateStack.empty()) {
                            storm::storage::sparse::state_type currentState = stateStack.back();
                            stateStack.pop_back();
                            nondivergentStates.set(currentState);
                            
                            for (auto const& predecessor : this->backwardTransitions.getRow(currentState)) {
                                if (&this->partition.getBlock(predecessor.getColumn()) == &block && !nondivergentStates.get(predecessor.getColumn())) {
                                    stateStack.push_back(predecessor.getColumn());
                                }
                            }
                        }
                    }
                }
                
                
                if (nondivergentStates.getNumberOfSetBits() > 0 && nondivergentStates.getNumberOfSetBits() != block.getNumberOfStates()) {
                    // After performing the split, the current block will contain the divergent states only.
                    this->partition.splitStates(block, nondivergentStates);
                    
                    // Since the remaining states in the block are divergent, we can mark the block as absorbing.
                    // This also guarantees that the self-loop will be added to the state of the quotient
                    // representing this block of states.
                    block.setAbsorbing(true);
                } else if (nondivergentStates.getNumberOfSetBits() == 0) {
                    // If there are only diverging states in the block, we need to make it absorbing.
                    block.setAbsorbing(true);
                }
            }
        }
        
        template<typename ModelType>
        void DeterministicModelBisimulationDecomposition<ModelType>::initializeSilentProbabilities() {
            silentProbabilities.resize(this->model.getNumberOfStates(), storm::utility::zero<ValueType>());
            for (storm::storage::sparse::state_type state = 0; state < this->model.getNumberOfStates(); ++state) {
                Block const* currentBlockPtr = &this->partition.getBlock(state);
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
            BisimulationDecomposition<ModelType>::initializeMeasureDrivenPartition();

            if (this->options.type == BisimulationType::Weak && this->model.getModelType() == ModelType::Dtmc) {
                this->initializeWeakDtmcBisimulation();
            }
        }
        
        template<typename ModelType>
        void DeterministicModelBisimulationDecomposition<ModelType>::initializeLabelBasedPartition() {
            BisimulationDecomposition<ModelType>::initializeLabelBasedPartition();
            
            if (this->options.type == BisimulationType::Weak && this->model.getModelType() == ModelType::Dtmc) {
                this->initializeWeakDtmcBisimulation();
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
            std::set<std::string> atomicPropositionsSet = this->options.selectedAtomicPropositions.get();
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
                        if (!partition.isSilent(state, comparator)) {
                            representativeState = state;
                            break;
                        }
                    }
                }
                
                Block const& oldBlock = partition.getBlock(representativeState);
                
                // If the block is absorbing, we simply add a self-loop.
                if (oldBlock.isAbsorbing()) {
                    builder.addNextValue(blockIndex, blockIndex, storm::utility::one<ValueType>());
                    
                    // If the block has a special representative state, we retrieve it now.
                    if (oldBlock.hasRepresentativeState()) {
                        representativeState = oldBlock.getRepresentativeState();
                    }

                    // Add all of the selected atomic propositions that hold in the representative state to the state
                    // representing the block.
                    for (auto const& ap : atomicPropositions) {
                        if (model.getStateLabeling().getStateHasLabel(ap, representativeState)) {
                            newLabeling.addLabelToState(ap, blockIndex);
                        }
                    }
                } else {
                    // Compute the outgoing transitions of the block.
                    std::map<storm::storage::sparse::state_type, ValueType> blockProbability;
                    for (auto const& entry : model.getTransitionMatrix().getRow(representativeState)) {
                        storm::storage::sparse::state_type targetBlock = partition.getBlock(entry.getColumn()).getId();
                        
                        // If we are computing a weak bisimulation quotient, there is no need to add self-loops.
                        if ((bisimulationType == BisimulationType::WeakDtmc || bisimulationType == BisimulationType::WeakCtmc) && targetBlock == blockIndex) {
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
                        if (bisimulationType == BisimulationType::WeakDtmc) {
                            builder.addNextValue(blockIndex, probabilityEntry.first, probabilityEntry.second / (storm::utility::one<ValueType>() - partition.getSilentProbability(representativeState)));
                        } else {
                            builder.addNextValue(blockIndex, probabilityEntry.first, probabilityEntry.second);
                        }
                    }
                    
                    // Otherwise add all atomic propositions to the equivalence class that the representative state
                    // satisfies.
                    for (auto const& ap : atomicPropositions) {
                        if (model.getStateLabeling().getStateHasLabel(ap, representativeState)) {
                            newLabeling.addLabelToState(ap, blockIndex);
                        }
                    }
                }
                
                // If the model has state rewards, we simply copy the state reward of the representative state, because
                // all states in a block are guaranteed to have the same state reward.
                if (keepRewards && model.hasRewardModel()) {
                    typename std::unordered_map<std::string, typename ModelType::RewardModelType>::const_iterator nameRewardModelPair = model.getUniqueRewardModel();
                    stateRewards.get()[blockIndex] = nameRewardModelPair->second.getStateRewardVector()[representativeState];
                }
            }
            
            // Now check which of the blocks of the partition contain at least one initial state.
            for (auto initialState : model.getInitialStates()) {
                Block const& initialBlock = partition.getBlock(initialState);
                newLabeling.addLabelToState("init", initialBlock.getId());
            }
            
            // Construct the reward model mapping.
            std::unordered_map<std::string, typename ModelType::RewardModelType> rewardModels;
            if (keepRewards && model.hasRewardModel()) {
                typename std::unordered_map<std::string, typename ModelType::RewardModelType>::const_iterator nameRewardModelPair = model.getUniqueRewardModel();
                rewardModels.insert(std::make_pair(nameRewardModelPair->first, typename ModelType::RewardModelType(stateRewards)));
            }
            
            // Finally construct the quotient model.
            this->quotient = std::shared_ptr<storm::models::sparse::DeterministicModel<ValueType, typename ModelType::RewardModelType>>(new ModelType(builder.build(), std::move(newLabeling), std::move(rewardModels)));
        }
        
        template<typename ValueType>
        void DeterministicModelBisimulationDecomposition<ValueType>::refineBlockProbabilities(Block& block, Partition& partition, BisimulationType bisimulationType, std::deque<Block*>& splitterQueue, storm::utility::ConstantsComparator<ValueType> const& comparator) {
            // Sort the states in the block based on their probabilities.
            std::sort(partition.getBegin(block), partition.getEnd(block), [&partition] (std::pair<storm::storage::sparse::state_type, ValueType> const& a, std::pair<storm::storage::sparse::state_type, ValueType> const& b) { return a.second < b.second; } );
            
            // Update the positions vector.
            storm::storage::sparse::state_type position = block.getBegin();
            for (auto stateIt = partition.getBegin(block), stateIte = partition.getEnd(block); stateIt != stateIte; ++stateIt, ++position) {
                partition.setPosition(stateIt->first, position);
            }
            
            // Finally, we need to scan the ranges of states that agree on the probability.
            typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::const_iterator begin = partition.getBegin(block);
            typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::const_iterator end = partition.getEnd(block) - 1;
            storm::storage::sparse::state_type currentIndex = block.getBegin();
            
            // Now we can check whether the block needs to be split, which is the case iff the probabilities for the
            // first and the last state are different.
            bool blockSplit = !comparator.isEqual(begin->second, end->second);
            while (!comparator.isEqual(begin->second, end->second)) {
                // Now we scan for the first state in the block that disagrees on the probability value.
                // Note that we do not have to check currentIndex for staying within bounds, because we know the matching
                // state is within bounds.
                ValueType const& currentValue = begin->second;
                
                ++begin;
                ++currentIndex;
                while (begin != end && comparator.isEqual(begin->second, currentValue)) {
                    ++begin;
                    ++currentIndex;
                }
                
                // Now we split the block and mark it as a potential splitter.
                Block& newBlock = partition.splitBlock(block, currentIndex);
                if (!newBlock.isMarkedAsSplitter()) {
                    splitterQueue.push_back(&newBlock);
                    newBlock.markAsSplitter();
                }
            }
            
            // If the block was split, we also need to insert itself into the splitter queue.
            if (blockSplit) {
                if (!block.isMarkedAsSplitter()) {
                    splitterQueue.push_back(&block);
                    block.markAsSplitter();
                }
            }
        }
        
        template<typename ValueType>
        void DeterministicModelBisimulationDecomposition<ValueType>::refineBlockWeak(Block& block, Partition& partition, storm::storage::SparseMatrix<ValueType> const& forwardTransitions, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::deque<Block*>& splitterQueue, storm::utility::ConstantsComparator<ValueType> const& comparator) {
            std::vector<uint_fast64_t> splitPoints = getSplitPointsWeak(block, partition, comparator);
            
            // Restore the original begin of the block.
            block.setBegin(block.getOriginalBegin());
            
            // Now that we have the split points of the non-silent states, we perform a backward search from
            // each non-silent state and label the predecessors with the class of the non-silent state.
            std::vector<storm::storage::BitVector> stateLabels(block.getEnd() - block.getBegin(), storm::storage::BitVector(splitPoints.size() - 1));
            
            std::vector<storm::storage::sparse::state_type> stateStack;
            stateStack.reserve(block.getEnd() - block.getBegin());
            for (uint_fast64_t stateClassIndex = 0; stateClassIndex < splitPoints.size() - 1; ++stateClassIndex) {
                for (auto stateIt = partition.getStatesAndValues().begin() + splitPoints[stateClassIndex], stateIte = partition.getStatesAndValues().begin() + splitPoints[stateClassIndex + 1]; stateIt != stateIte; ++stateIt) {
                    
                    stateStack.push_back(stateIt->first);
                    stateLabels[partition.getPosition(stateIt->first) - block.getBegin()].set(stateClassIndex);
                    while (!stateStack.empty()) {
                        storm::storage::sparse::state_type currentState = stateStack.back();
                        stateStack.pop_back();
                        
                        for (auto const& predecessorEntry : backwardTransitions.getRow(currentState)) {
                            if (comparator.isZero(predecessorEntry.getValue())) {
                                continue;
                            }
                            
                            storm::storage::sparse::state_type predecessor = predecessorEntry.getColumn();
                            
                            // Only if the state is in the same block, is a silent state and it has not yet been
                            // labeled with the current label.
                            if (&partition.getBlock(predecessor) == &block && partition.isSilent(predecessor, comparator) && !stateLabels[partition.getPosition(predecessor) - block.getBegin()].get(stateClassIndex)) {
                                stateStack.push_back(predecessor);
                                stateLabels[partition.getPosition(predecessor) - block.getBegin()].set(stateClassIndex);
                            }
                        }
                    }
                }
            }
            
            // Now that all states were appropriately labeled, we can sort the states according to their labels and then
            // scan for ranges that agree on the label.
            std::sort(partition.getBegin(block), partition.getEnd(block), [&] (std::pair<storm::storage::sparse::state_type, ValueType> const& a, std::pair<storm::storage::sparse::state_type, ValueType> const& b) { return stateLabels[partition.getPosition(a.first) - block.getBegin()] < stateLabels[partition.getPosition(b.first) - block.getBegin()]; });
            
            // Note that we do not yet repair the positions vector, but for the sake of efficiency temporariliy keep the
            // data structure in an inconsistent state.
            
            // Now we have everything in place to actually split the block by just scanning for ranges of equal label.
            typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::const_iterator begin = partition.getBegin(block);
            typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::const_iterator end = partition.getEnd(block) - 1;
            storm::storage::sparse::state_type currentIndex = block.getBegin();
            
            // Now we can check whether the block needs to be split, which is the case iff the labels for the first and
            // the last state are different. Store the offset of the block seperately, because it will potentially
            // modified by splits.
            storm::storage::sparse::state_type blockOffset = block.getBegin();
            bool blockSplit = stateLabels[partition.getPosition(begin->first) - blockOffset] != stateLabels[partition.getPosition(end->first) - blockOffset];
            while (stateLabels[partition.getPosition(begin->first) - blockOffset] != stateLabels[partition.getPosition(end->first) - blockOffset]) {
                // Now we scan for the first state in the block that disagrees on the labeling value.
                // Note that we do not have to check currentIndex for staying within bounds, because we know the matching
                // state is within bounds.
                storm::storage::BitVector const& currentValue = stateLabels[partition.getPosition(begin->first) - blockOffset];
                
                ++begin;
                ++currentIndex;
                while (begin != end && stateLabels[partition.getPosition(begin->first) - blockOffset] == currentValue) {
                    ++begin;
                    ++currentIndex;
                }
                
                // Now we split the block and mark it as a potential splitter.
                Block& newBlock = partition.splitBlock(block, currentIndex);
                
                // Update the silent probabilities for all the states in the new block.
                for (auto stateIt = partition.getBegin(newBlock), stateIte = partition.getEnd(newBlock); stateIt != stateIte; ++stateIt) {
                    if (partition.hasSilentProbability(stateIt->first, comparator)) {
                        ValueType newSilentProbability = storm::utility::zero<ValueType>();
                        for (auto const& successorEntry : forwardTransitions.getRow(stateIt->first)) {
                            if (&partition.getBlock(successorEntry.getColumn()) == &newBlock) {
                                newSilentProbability += successorEntry.getValue();
                            }
                        }
                        partition.setSilentProbability(stateIt->first, newSilentProbability);
                    }
                }
                
                if (!newBlock.isMarkedAsSplitter()) {
                    splitterQueue.push_back(&newBlock);
                    newBlock.markAsSplitter();
                }
            }
            
            // If the block was split, we also need to insert itself into the splitter queue.
            if (blockSplit) {
                if (!block.isMarkedAsSplitter()) {
                    splitterQueue.push_back(&block);
                    block.markAsSplitter();
                }
                
                // Update the silent probabilities for all the states in the old block.
                for (auto stateIt = partition.getBegin(block), stateIte = partition.getEnd(block); stateIt != stateIte; ++stateIt) {
                    if (partition.hasSilentProbability(stateIt->first, comparator)) {
                        ValueType newSilentProbability = storm::utility::zero<ValueType>();
                        for (auto const& successorEntry : forwardTransitions.getRow(stateIt->first)) {
                            if (&partition.getBlock(successorEntry.getColumn()) == &block) {
                                newSilentProbability += successorEntry.getValue();
                            }
                        }
                        partition.setSilentProbability(stateIt->first, newSilentProbability);
                    }
                }
            }
            
            // Finally update the positions vector.
            storm::storage::sparse::state_type position = blockOffset;
            for (auto stateIt = partition.getStatesAndValues().begin() + blockOffset, stateIte = partition.getEnd(block); stateIt != stateIte; ++stateIt, ++position) {
                partition.setPosition(stateIt->first, position);
            }
        }
        
        template<typename ValueType>
        std::vector<uint_fast64_t> DeterministicModelBisimulationDecomposition<ValueType>::getSplitPointsWeak(Block& block, Partition& partition, storm::utility::ConstantsComparator<ValueType> const& comparator) {
            std::vector<uint_fast64_t> result;
            // We first scale all probabilities with (1-p[s]) where p[s] is the silent probability of state s.
            std::for_each(partition.getStatesAndValues().begin() + block.getOriginalBegin(), partition.getStatesAndValues().begin() + block.getBegin(), [&] (std::pair<storm::storage::sparse::state_type, ValueType>& stateValuePair) {
                ValueType const& silentProbability = partition.getSilentProbability(stateValuePair.first);
                if (!comparator.isOne(silentProbability) && !comparator.isZero(silentProbability)) {
                    stateValuePair.second /= storm::utility::one<ValueType>() - silentProbability;
                }
            });
            
            // Now sort the states based on their probabilities.
            std::sort(partition.getStatesAndValues().begin() + block.getOriginalBegin(), partition.getStatesAndValues().begin() + block.getBegin(), [&partition] (std::pair<storm::storage::sparse::state_type, ValueType> const& a, std::pair<storm::storage::sparse::state_type, ValueType> const& b) { return a.second < b.second; } );
            
            // Update the positions vector.
            storm::storage::sparse::state_type position = block.getOriginalBegin();
            for (auto stateIt = partition.getStatesAndValues().begin() + block.getOriginalBegin(), stateIte = partition.getStatesAndValues().begin() + block.getBegin(); stateIt != stateIte; ++stateIt, ++position) {
                partition.setPosition(stateIt->first, position);
            }
            
            // Then, we scan for the ranges of states that agree on the probability.
            typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::const_iterator begin = partition.getStatesAndValues().begin() + block.getOriginalBegin();
            typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::const_iterator end = partition.getStatesAndValues().begin() + block.getBegin() - 1;
            storm::storage::sparse::state_type currentIndex = block.getOriginalBegin();
            result.push_back(currentIndex);
            
            // Now we can check whether the block needs to be split, which is the case iff the probabilities for the
            // first and the last state are different.
            while (!comparator.isEqual(begin->second, end->second)) {
                // Now we scan for the first state in the block that disagrees on the probability value.
                // Note that we do not have to check currentIndex for staying within bounds, because we know the matching
                // state is within bounds.
                ValueType const& currentValue = begin->second;
                
                ++begin;
                ++currentIndex;
                while (begin != end && comparator.isEqual(begin->second, currentValue)) {
                    ++begin;
                    ++currentIndex;
                }
                
                // Remember the index at which the probabilities were different.
                result.push_back(currentIndex);
            }
            
            // Push a sentinel element and return result.
            result.push_back(block.getBegin());
            return result;
        }
        
        template<typename ValueType>
        void DeterministicModelBisimulationDecomposition<ValueType>::refinePartition(storm::storage::SparseMatrix<ValueType> const& forwardTransitions, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, Block& splitter, Partition& partition, BisimulationType bisimulationType, std::deque<Block*>& splitterQueue, storm::utility::ConstantsComparator<ValueType> const& comparator) {
            std::list<Block*> predecessorBlocks;
            
            // Iterate over all states of the splitter and check its predecessors.
            bool splitterIsPredecessor = false;
            storm::storage::sparse::state_type currentPosition = splitter.getBegin();
            for (auto stateIterator = partition.getBegin(splitter), stateIte = partition.getEnd(splitter); stateIterator != stateIte; ++stateIterator, ++currentPosition) {
                storm::storage::sparse::state_type currentState = stateIterator->first;
                
                uint_fast64_t elementsToSkip = 0;
                for (auto const& predecessorEntry : backwardTransitions.getRow(currentState)) {
                    storm::storage::sparse::state_type predecessor = predecessorEntry.getColumn();
                    
                    // Get predecessor block and remember if the splitter was a predecessor of itself.
                    Block& predecessorBlock = partition.getBlock(predecessor);
                    if (&predecessorBlock == &splitter) {
                        splitterIsPredecessor = true;
                    }
                    
                    // If the predecessor block has just one state or is marked as being absorbing, we must not split it.
                    if (predecessorBlock.getNumberOfStates() <= 1 || predecessorBlock.isAbsorbing()) {
                        continue;
                    }
                    
                    storm::storage::sparse::state_type predecessorPosition = partition.getPosition(predecessor);
                    
                    // If we have not seen this predecessor before, we move it to a part near the beginning of the block.
                    if (predecessorPosition >= predecessorBlock.getBegin()) {
                        if (&predecessorBlock == &splitter) {
                            // If the predecessor we just found was already processed (in terms of visiting its predecessors),
                            // we swap it with the state that is currently at the beginning of the block and move the start
                            // of the block one step further.
                            if (predecessorPosition <= currentPosition + elementsToSkip) {
                                partition.swapStates(predecessor, partition.getState(predecessorBlock.getBegin()));
                                predecessorBlock.incrementBegin();
                            } else {
                                // Otherwise, we need to move the predecessor, but we need to make sure that we explore its
                                // predecessors later.
                                if (predecessorBlock.getMarkedPosition() == predecessorBlock.getBegin()) {
                                    partition.swapStatesAtPositions(predecessorBlock.getMarkedPosition(), predecessorPosition);
                                    partition.swapStatesAtPositions(predecessorPosition, currentPosition + elementsToSkip + 1);
                                } else {
                                    partition.swapStatesAtPositions(predecessorBlock.getMarkedPosition(), predecessorPosition);
                                    partition.swapStatesAtPositions(predecessorPosition, predecessorBlock.getBegin());
                                    partition.swapStatesAtPositions(predecessorPosition, currentPosition + elementsToSkip + 1);
                                }
                                
                                ++elementsToSkip;
                                predecessorBlock.incrementMarkedPosition();
                                predecessorBlock.incrementBegin();
                            }
                        } else {
                            partition.swapStates(predecessor, partition.getState(predecessorBlock.getBegin()));
                            predecessorBlock.incrementBegin();
                        }
                        partition.setValue(predecessor, predecessorEntry.getValue());
                    } else {
                        // Otherwise, we just need to update the probability for this predecessor.
                        partition.increaseValue(predecessor, predecessorEntry.getValue());
                    }
                    
                    if (!predecessorBlock.isMarkedAsPredecessor()) {
                        predecessorBlocks.emplace_back(&predecessorBlock);
                        predecessorBlock.markAsPredecessorBlock();
                    }
                }
                
                // If we had to move some elements beyond the current element, we may have to skip them.
                if (elementsToSkip > 0) {
                    stateIterator += elementsToSkip;
                    currentPosition += elementsToSkip;
                }
            }
            
            // Now we can traverse the list of states of the splitter whose predecessors we have not yet explored.
            for (auto stateIterator = partition.getStatesAndValues().begin() + splitter.getOriginalBegin(), stateIte = partition.getStatesAndValues().begin() + splitter.getMarkedPosition(); stateIterator != stateIte; ++stateIterator) {
                storm::storage::sparse::state_type currentState = stateIterator->first;
                
                for (auto const& predecessorEntry : backwardTransitions.getRow(currentState)) {
                    storm::storage::sparse::state_type predecessor = predecessorEntry.getColumn();
                    Block& predecessorBlock = partition.getBlock(predecessor);
                    storm::storage::sparse::state_type predecessorPosition = partition.getPosition(predecessor);
                    
                    if (predecessorPosition >= predecessorBlock.getBegin()) {
                        partition.swapStatesAtPositions(predecessorPosition, predecessorBlock.getBegin());
                        predecessorBlock.incrementBegin();
                        partition.setValue(predecessor, predecessorEntry.getValue());
                    } else {
                        partition.increaseValue(predecessor, predecessorEntry.getValue());
                    }
                    
                    if (!predecessorBlock.isMarkedAsPredecessor()) {
                        predecessorBlocks.emplace_back(&predecessorBlock);
                        predecessorBlock.markAsPredecessorBlock();
                    }
                }
            }
            
            if (bisimulationType == BisimulationType::Strong || bisimulationType == BisimulationType::WeakCtmc) {
                std::vector<Block*> blocksToSplit;
                
                // Now, we can iterate over the predecessor blocks and see whether we have to create a new block for
                // predecessors of the splitter.
                for (auto blockPtr : predecessorBlocks) {
                    Block& block = *blockPtr;
                    
                    block.unmarkAsPredecessorBlock();
                    block.resetMarkedPosition();
                    
                    // If we have moved the begin of the block to somewhere in the middle of the block, we need to split it.
                    if (block.getBegin() != block.getEnd()) {
                        Block& newBlock = partition.insertBlock(block);
                        if (!newBlock.isMarkedAsSplitter()) {
                            splitterQueue.push_back(&newBlock);
                            newBlock.markAsSplitter();
                        }
                        
                        // Schedule the block of predecessors for refinement based on probabilities.
                        blocksToSplit.emplace_back(&newBlock);
                    } else {
                        // In this case, we can keep the block by setting its begin to the old value.
                        block.setBegin(block.getOriginalBegin());
                        blocksToSplit.emplace_back(&block);
                    }
                }
                
                // Finally, we walk through the blocks that have a transition to the splitter and split them using
                // probabilistic information.
                for (auto blockPtr : blocksToSplit) {
                    if (blockPtr->getNumberOfStates() <= 1) {
                        continue;
                    }
                    
                    // In the case of weak bisimulation for CTMCs, we don't need to make sure the rate of staying inside
                    // the own block is the same.
                    if (bisimulationType == BisimulationType::WeakCtmc && blockPtr == &splitter) {
                        continue;
                    }
                    
                    refineBlockProbabilities(*blockPtr, partition, bisimulationType, splitterQueue, comparator);
                }
            } else { // In this case, we are computing a weak bisimulation on a DTMC.
                // If the splitter was a predecessor of itself and we are computing a weak bisimulation, we need to update
                // the silent probabilities.
                if (splitterIsPredecessor) {
                    partition.setSilentProbabilities(partition.getStatesAndValues().begin() + splitter.getOriginalBegin(), partition.getStatesAndValues().begin() + splitter.getBegin());
                    partition.setSilentProbabilitiesToZero(partition.getStatesAndValues().begin() + splitter.getBegin(), partition.getStatesAndValues().begin() + splitter.getEnd());
                }
                
                // Now refine all predecessor blocks in a weak manner. That is, we split according to the criterion of
                // weak bisimulation.
                for (auto blockPtr : predecessorBlocks) {
                    Block& block = *blockPtr;
                    
                    // If the splitter is also the predecessor block, we must not refine it at this point.
                    if (&block != &splitter) {
                        refineBlockWeak(block, partition, forwardTransitions, backwardTransitions, splitterQueue, comparator);
                    } else {
                        // Restore the begin of the block.
                        block.setBegin(block.getOriginalBegin());
                    }
                    
                    block.unmarkAsPredecessorBlock();
                    block.resetMarkedPosition();
                }
            }
            
            STORM_LOG_ASSERT(partition.check(), "Partition became inconsistent.");
        }
        
        template class DeterministicModelBisimulationDecomposition<double>;
        
#ifdef STORM_HAVE_CARL
        template class DeterministicModelBisimulationDecomposition<storm::RationalFunction>;
#endif
    }
}
