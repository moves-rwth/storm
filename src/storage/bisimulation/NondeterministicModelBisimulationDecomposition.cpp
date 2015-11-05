#include "src/storage/bisimulation/NondeterministicModelBisimulationDecomposition.h"

#include "src/models/sparse/Mdp.h"
#include "src/models/sparse/StandardRewardModel.h"

#include "src/utility/graph.h"

#include "src/exceptions/IllegalFunctionCallException.h"

namespace storm {
    namespace storage {

        using namespace bisimulation;
        
        template<typename ModelType>
        NondeterministicModelBisimulationDecomposition<ModelType>::NondeterministicModelBisimulationDecomposition(ModelType const& model, typename BisimulationDecomposition<ModelType, NondeterministicModelBisimulationDecomposition::BlockDataType>::Options const& options) : BisimulationDecomposition<ModelType, NondeterministicModelBisimulationDecomposition::BlockDataType>(model, model.getTransitionMatrix().transpose(false), options), choiceToStateMapping(model.getNumberOfChoices()), probabilitiesToCurrentSplitter(model.getNumberOfChoices(), storm::utility::zero<ValueType>()) {
            STORM_LOG_THROW(options.type == BisimulationType::Strong, storm::exceptions::IllegalFunctionCallException, "Weak bisimulation is currently not supported for nondeterministic models.");
            this->createChoiceToStateMapping();
        }
    
        template<typename ModelType>
        std::pair<storm::storage::BitVector, storm::storage::BitVector> NondeterministicModelBisimulationDecomposition<ModelType>::getStatesWithProbability01() {
            STORM_LOG_THROW(static_cast<bool>(this->options.optimalityType), storm::exceptions::IllegalFunctionCallException, "Can only compute states with probability 0/1 with an optimization direction (min/max).");
            if (this->options.optimalityType.get() == OptimizationDirection::Minimize) {
                return storm::utility::graph::performProb01Min(this->model.getTransitionMatrix(), this->model.getTransitionMatrix().getRowGroupIndices(), this->model.getBackwardTransitions(), this->options.phiStates.get(), this->options.psiStates.get());
            } else {
                return storm::utility::graph::performProb01Max(this->model.getTransitionMatrix(), this->model.getTransitionMatrix().getRowGroupIndices(), this->model.getBackwardTransitions(), this->options.phiStates.get(), this->options.psiStates.get());
            }
        }

        template<typename ModelType>
        void NondeterministicModelBisimulationDecomposition<ModelType>::createChoiceToStateMapping() {
            std::vector<uint_fast64_t> nondeterministicChoiceIndices = this->model.getTransitionMatrix().getRowGroupIndices();
            for (storm::storage::sparse::state_type state = 0; state < this->model.getNumberOfStates(); ++state) {
                for (uint_fast64_t choice = nondeterministicChoiceIndices[state]; choice < nondeterministicChoiceIndices[state + 1]; ++choice) {
                    choiceToStateMapping[choice] = state;
                }
            }
        }
        
        template<typename ModelType>
        void NondeterministicModelBisimulationDecomposition<ModelType>::buildQuotient() {
            STORM_LOG_ASSERT(false, "Not yet implemented");
        }
        
        template<typename ModelType>
        bool NondeterministicModelBisimulationDecomposition<ModelType>::possiblyNeedsRefinement(bisimulation::Block<BlockDataType> const& predecessorBlock) const {
            return predecessorBlock.getNumberOfStates() > 1 && !predecessorBlock.data().absorbing();
        }

        template<typename ModelType>
        void NondeterministicModelBisimulationDecomposition<ModelType>::clearProbabilitiesToSplitter(storm::storage::sparse::state_type state) {
            std::vector<uint_fast64_t> nondeterministicChoiceIndices = this->model.getTransitionMatrix().getRowGroupIndices();
            for (uint_fast64_t choice = nondeterministicChoiceIndices[state]; choice < nondeterministicChoiceIndices[state + 1]; ++choice) {
                probabilitiesToCurrentSplitter[choice] = storm::utility::zero<ValueType>();
            }
        }
        
        template<typename ModelType>
        void NondeterministicModelBisimulationDecomposition<ModelType>::increaseProbabilityToSplitter(storm::storage::sparse::state_type state, uint_fast64_t choice, bisimulation::Block<BlockDataType> const& predecessorBlock, ValueType const& value) {
            storm::storage::sparse::state_type predecessorPosition = this->partition.getPosition(state);
            
            // If the position of the state is to the right of marker1, we have not seen it before. That means we need to
            // clear the probability associated with all choices of the given state.
            if (predecessorPosition >= predecessorBlock.data().marker1()) {
                clearProbabilitiesToSplitter(state);
            }
            
            // Now increase the probability of the given choice of the given state.
            probabilitiesToCurrentSplitter[choice] += value;
        }
        
        template <typename ModelType>
        void NondeterministicModelBisimulationDecomposition<ModelType>::moveStateToMarker1(storm::storage::sparse::state_type predecessor, bisimulation::Block<BlockDataType>& predecessorBlock) {
            this->partition.swapStates(predecessor, this->partition.getState(predecessorBlock.data().marker1()));
            predecessorBlock.data().incrementMarker1();
        }
        
        template <typename ModelType>
        void NondeterministicModelBisimulationDecomposition<ModelType>::moveStateToMarker2(storm::storage::sparse::state_type predecessor, bisimulation::Block<BlockDataType>& predecessorBlock) {
            this->partition.swapStates(predecessor, this->partition.getState(predecessorBlock.data().marker2()));
            predecessorBlock.data().incrementMarker2();
        }
        
        template <typename ModelType>
        void NondeterministicModelBisimulationDecomposition<ModelType>::moveStateInSplitter(storm::storage::sparse::state_type predecessor, bisimulation::Block<BlockDataType>& predecessorBlock, storm::storage::sparse::state_type currentPositionInSplitter, uint_fast64_t& elementsToSkip) {
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
        
        template<typename ModelType>
        void NondeterministicModelBisimulationDecomposition<ModelType>::insertIntoPredecessorList(bisimulation::Block<BlockDataType>& predecessorBlock, std::list<bisimulation::Block<BlockDataType>*>& predecessorBlocks) {
            // Insert the block into the list of blocks to refine (if that has not already happened).
            if (!predecessorBlock.data().needsRefinement()) {
                predecessorBlocks.emplace_back(&predecessorBlock);
                predecessorBlock.data().setNeedsRefinement();
            }
        }
        
        template <typename ModelType>
        void NondeterministicModelBisimulationDecomposition<ModelType>::exploreRemainingStatesOfSplitter(bisimulation::Block<BlockDataType>& splitter, std::list<bisimulation::Block<BlockDataType>*>& predecessorBlocks) {
            for (auto splitterIt = this->partition.begin(splitter), splitterIte = this->partition.begin(splitter) + (splitter.data().marker2() - splitter.getBeginIndex()); splitterIt != splitterIte; ++splitterIt) {
                storm::storage::sparse::state_type currentState = *splitterIt;
                
                for (auto const& predecessorEntry : this->backwardTransitions.getRow(currentState)) {
                    storm::storage::sparse::state_type choice = predecessorEntry.getColumn();
                    storm::storage::sparse::state_type predecessor = choiceToStateMapping[choice];
                    Block<BlockDataType>& predecessorBlock = this->partition.getBlock(predecessor);
                    
                    // If the block does not need to be refined, we skip it.
                    if (!possiblyNeedsRefinement(predecessorBlock)) {
                        continue;
                    }
                    
                    // We keep track of the probability of the predecessor moving to the splitter.
                    increaseProbabilityToSplitter(predecessor, choice, predecessorBlock, predecessorEntry.getValue());
                    
                    // Only move the state if it has not been seen as a predecessor before.
                    storm::storage::sparse::state_type predecessorPosition = this->partition.getPosition(predecessor);
                    if (predecessorPosition >= predecessorBlock.data().marker1()) {
                        moveStateToMarker1(predecessor, predecessorBlock);
                    }
                    
                    insertIntoPredecessorList(predecessorBlock, predecessorBlocks);
                }
            }
            
            // Finally, we can reset the second marker.
            splitter.data().setMarker2(splitter.getBeginIndex());
        }
        
        template<typename ModelType>
        void NondeterministicModelBisimulationDecomposition<ModelType>::refinePredecessorBlocksOfSplitter(std::list<bisimulation::Block<BlockDataType>*> const& predecessorBlocks, std::deque<bisimulation::Block<BlockDataType>*>& splitterQueue) {
            // TODO
        }
        
        template<typename ModelType>
        void NondeterministicModelBisimulationDecomposition<ModelType>::refinePartitionBasedOnSplitter(bisimulation::Block<BlockDataType>& splitter, std::deque<bisimulation::Block<BlockDataType>*>& splitterQueue) {
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
            std::list<Block<BlockDataType>*> predecessorBlocks;
            storm::storage::sparse::state_type currentPosition = splitter.getBeginIndex();
            bool splitterIsPredecessorBlock = false;
            for (auto splitterIt = this->partition.begin(splitter), splitterIte = this->partition.end(splitter); splitterIt != splitterIte; ++splitterIt, ++currentPosition) {
                storm::storage::sparse::state_type currentState = *splitterIt;
                
                uint_fast64_t elementsToSkip = 0;
                for (auto const& predecessorEntry : this->backwardTransitions.getRow(currentState)) {
                    storm::storage::sparse::state_type choice = predecessorEntry.getColumn();
                    storm::storage::sparse::state_type predecessor = choiceToStateMapping[choice];
                    storm::storage::sparse::state_type predecessorPosition = this->partition.getPosition(predecessor);
                    Block<BlockDataType>& predecessorBlock = this->partition.getBlock(predecessor);
                    
                    // If the block does not need to be refined, we skip it.
                    if (!possiblyNeedsRefinement(predecessorBlock)) {
                        continue;
                    }
                    
                    // We keep track of the probability of the predecessor moving to the splitter.
                    increaseProbabilityToSplitter(predecessor, choice, predecessorBlock, predecessorEntry.getValue());
                    
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
                        
                        insertIntoPredecessorList(predecessorBlock, predecessorBlocks);
                    }
                }
                
                // If, as a consequence of shifting states, we need to skip some elements, do so now.
                splitterIt += elementsToSkip;
                currentPosition += elementsToSkip;
            }
            
            // If the splitter was a predecessor block of itself, we potentially need to explore some states that have
            // not been explored yet.
            if (splitterIsPredecessorBlock) {
                exploreRemainingStatesOfSplitter(splitter, predecessorBlocks);
            }
            
            // Finally, we split the block based on the precomputed probabilities and the chosen bisimulation type.
            refinePredecessorBlocksOfSplitter(predecessorBlocks, splitterQueue);
        }
        
        template class NondeterministicModelBisimulationDecomposition<storm::models::sparse::Mdp<double>>;
        
    }
}
