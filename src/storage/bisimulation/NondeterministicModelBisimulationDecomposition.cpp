#include "src/storage/bisimulation/NondeterministicModelBisimulationDecomposition.h"

#include "src/models/sparse/Mdp.h"
#include "src/models/sparse/StandardRewardModel.h"

#include "src/utility/graph.h"

#include "src/utility/macros.h"
#include "src/exceptions/IllegalFunctionCallException.h"

#include "src/adapters/CarlAdapter.h"

namespace storm {
    namespace storage {
        
        using namespace bisimulation;
        
        template<typename ModelType>
        NondeterministicModelBisimulationDecomposition<ModelType>::NondeterministicModelBisimulationDecomposition(ModelType const& model, typename BisimulationDecomposition<ModelType, NondeterministicModelBisimulationDecomposition::BlockDataType>::Options const& options) : BisimulationDecomposition<ModelType, NondeterministicModelBisimulationDecomposition::BlockDataType>(model, model.getTransitionMatrix().transpose(false), options), choiceToStateMapping(model.getNumberOfChoices()), quotientDistributions(model.getNumberOfChoices()), orderedQuotientDistributions(model.getNumberOfChoices()) {
            STORM_LOG_THROW(options.type == BisimulationType::Strong, storm::exceptions::IllegalFunctionCallException, "Weak bisimulation is currently not supported for nondeterministic models.");
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
        void NondeterministicModelBisimulationDecomposition<ModelType>::initialize() {
            this->createChoiceToStateMapping();
            this->initializeQuotientDistributions();
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
        void NondeterministicModelBisimulationDecomposition<ModelType>::initializeQuotientDistributions() {
            std::vector<uint_fast64_t> nondeterministicChoiceIndices = this->model.getTransitionMatrix().getRowGroupIndices();
            for (auto choice = 0; choice < nondeterministicChoiceIndices.back(); ++choice) {
                for (auto entry : this->model.getTransitionMatrix().getRow(choice)) {
                    if (!this->comparator.isZero(entry.getValue())) {
                        this->quotientDistributions[choice].addProbability(this->partition.getBlock(entry.getColumn()).getId(), entry.getValue());
                    }
                }
                orderedQuotientDistributions[choice] = &this->quotientDistributions[choice];
            }
            
            for (auto state = 0; state < this->model.getNumberOfStates(); ++state) {
                updateOrderedQuotientDistributions(state);
            }
        }
        
        template<typename ModelType>
        void NondeterministicModelBisimulationDecomposition<ModelType>::updateOrderedQuotientDistributions(storm::storage::sparse::state_type state) {
            std::vector<uint_fast64_t> nondeterministicChoiceIndices = this->model.getTransitionMatrix().getRowGroupIndices();
            std::sort(this->orderedQuotientDistributions.begin() + nondeterministicChoiceIndices[state], this->orderedQuotientDistributions.begin() + nondeterministicChoiceIndices[state + 1],
                      [this] (storm::storage::Distribution<ValueType> const* dist1, storm::storage::Distribution<ValueType> const* dist2) {
                          return dist1->less(*dist2, this->comparator);
                      });
        }
        
        template<typename ModelType>
        void NondeterministicModelBisimulationDecomposition<ModelType>::buildQuotient() {
            std::cout << "Found " << this->partition.size() << " blocks" << std::endl;
            this->partition.print();
            STORM_LOG_ASSERT(false, "Not yet implemented");
        }
        
        template<typename ModelType>
        bool NondeterministicModelBisimulationDecomposition<ModelType>::possiblyNeedsRefinement(bisimulation::Block<BlockDataType> const& block) const {
            return block.getNumberOfStates() > 1 && !block.data().absorbing();
        }
        
        template<typename ModelType>
        void NondeterministicModelBisimulationDecomposition<ModelType>::updateQuotientDistributionsOfPredecessors(Block<BlockDataType> const& newBlock, Block<BlockDataType> const& oldBlock, std::deque<Block<BlockDataType>*>& splitterQueue) {
            uint_fast64_t lastState = 0;
            bool lastStateInitialized = false;
            
            for (auto stateIt = this->partition.begin(newBlock), stateIte = this->partition.end(newBlock); stateIt != stateIte; ++stateIt) {
                for (auto predecessorEntry : this->backwardTransitions.getRow(*stateIt)) {
                    if (this->comparator.isZero(predecessorEntry.getValue())) {
                        continue;
                    }
                    
                    storm::storage::sparse::state_type predecessorChoice = predecessorEntry.getColumn();
                    storm::storage::sparse::state_type predecessorState = choiceToStateMapping[predecessorChoice];
                    Block<BlockDataType>& predecessorBlock = this->partition.getBlock(predecessorState);
                    
                    // If the predecessor block is not marked as to-refined, we do so now.
                    if (!predecessorBlock.data().splitter()) {
                        predecessorBlock.data().setSplitter();
                        splitterQueue.push_back(&predecessorBlock);
                    }
                    
                    if (lastStateInitialized) {
                        // If we have skipped to the choices of the next state, we need to repair the order of the
                        // distributions for the last state.
                        if (lastState != predecessorState) {
                            updateOrderedQuotientDistributions(lastState);
                            lastState = predecessorState;
                        }
                    } else {
                        lastStateInitialized = true;
                        lastState = choiceToStateMapping[predecessorChoice];
                    }
                    
                    // Now shift the probability from this transition from the old block to the new one.
                    this->quotientDistributions[predecessorChoice].shiftProbability(oldBlock.getId(), newBlock.getId(), predecessorEntry.getValue());
                }
            }
            
            if (lastStateInitialized) {
                updateOrderedQuotientDistributions(lastState);
            }
        }
        
        template<typename ModelType>
        bool NondeterministicModelBisimulationDecomposition<ModelType>::checkQuotientDistributions() const {
            
        }
        
        template<typename ModelType>
        bool NondeterministicModelBisimulationDecomposition<ModelType>::splitBlockAccordingToCurrentQuotientDistributions(Block<BlockDataType>& block, std::deque<Block<BlockDataType>*>& splitterQueue) {
            bool split = this->partition.splitBlock(block,
                                                    [this] (storm::storage::sparse::state_type state1, storm::storage::sparse::state_type state2) {
                                                        return quotientDistributionsLess(state1, state2);
                                                    },
                                                    [this, &block, &splitterQueue] (Block<BlockDataType>& newBlock) {
                                                        updateQuotientDistributionsOfPredecessors(newBlock, block, splitterQueue);
                                                    });
            
            // The quotient distributions of the predecessors of block do not need to be updated, since the probability
            // will go to the block with the same id as before.
            
//            std::cout << "partition after split: " << std::endl;
//            this->partition.print();
            
            this->checkQuotientDistributions();
            
            return split;
        }
        
        template<typename ModelType>
        bool NondeterministicModelBisimulationDecomposition<ModelType>::quotientDistributionsLess(storm::storage::sparse::state_type state1, storm::storage::sparse::state_type state2) {
            STORM_LOG_TRACE("Comparing the quotient distributions of state " << state1 << " and " << state2 << ".");
            std::vector<uint_fast64_t> nondeterministicChoiceIndices = this->model.getTransitionMatrix().getRowGroupIndices();
            
//            std::cout << "state " << state1 << std::endl;
//            for (int c = nondeterministicChoiceIndices[state1]; c < nondeterministicChoiceIndices[state1 + 1]; ++c) {
//                std::cout << quotientDistributions[c] << std::endl;
//            }
//
//            std::cout << "state " << state2 << std::endl;
//            for (int c = nondeterministicChoiceIndices[state2]; c < nondeterministicChoiceIndices[state2 + 1]; ++c) {
//                std::cout << quotientDistributions[c] << std::endl;
//            }
            
            auto firstIt = orderedQuotientDistributions.begin() + nondeterministicChoiceIndices[state1];
            auto firstIte = orderedQuotientDistributions.begin() + nondeterministicChoiceIndices[state1 + 1];
            auto secondIt = orderedQuotientDistributions.begin() + nondeterministicChoiceIndices[state2];
            auto secondIte = orderedQuotientDistributions.begin() + nondeterministicChoiceIndices[state2 + 1];
            
            for (; firstIt != firstIte && secondIt != secondIte; ++firstIt, ++secondIt) {
                // If the current distributions are in a less-than relationship, we can return a result.
                if ((*firstIt)->less(**secondIt, this->comparator)) {
                    return true;
                } else if ((*secondIt)->less(**firstIt, this->comparator)) {
                    return false;
                }
                
                // If the distributions matched, we need to advance both distribution iterators to the next distribution
                // that is larger.
                while (firstIt != firstIte && std::next(firstIt) != firstIte && !(*firstIt)->less(**std::next(firstIt), this->comparator)) {
                    ++firstIt;
                }
                while (secondIt != secondIte && std::next(secondIt) != secondIte && !(*secondIt)->less(**std::next(secondIt), this->comparator)) {
                    ++secondIt;
                }
            }
            
            if (firstIt == firstIte && secondIt != secondIte) {
                return true;
            }
            return false;
        }
        
        template<typename ModelType>
        void NondeterministicModelBisimulationDecomposition<ModelType>::refinePartitionBasedOnSplitter(bisimulation::Block<BlockDataType>& splitter, std::deque<bisimulation::Block<BlockDataType>*>& splitterQueue) {
            if (!possiblyNeedsRefinement(splitter)) {
                return;
            }
            
            STORM_LOG_TRACE("Refining block " << splitter.getId());
            
            splitBlockAccordingToCurrentQuotientDistributions(splitter, splitterQueue);
        }
        
        template class NondeterministicModelBisimulationDecomposition<storm::models::sparse::Mdp<double>>;
        
#ifdef STORM_HAVE_CARL
        template class NondeterministicModelBisimulationDecomposition<storm::models::sparse::Mdp<storm::RationalFunction>>;
#endif
        
    }
}
