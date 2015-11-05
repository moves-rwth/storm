#ifndef STORM_STORAGE_BISIMULATION_NONDETERMINISTICMODELBISIMULATIONDECOMPOSITION_H_
#define STORM_STORAGE_BISIMULATION_NONDETERMINISTICMODELBISIMULATIONDECOMPOSITION_H_

#include "src/storage/bisimulation/BisimulationDecomposition.h"
#include "src/storage/bisimulation/DeterministicBlockData.h"

namespace storm {
    namespace utility {
        template <typename ValueType> class ConstantsComparator;
    }
    
    namespace storage {
        
        /*!
         * This class represents the decomposition of a nondeterministic model into its bisimulation quotient.
         */
        template<typename ModelType>
        class NondeterministicModelBisimulationDecomposition : public BisimulationDecomposition<ModelType, bisimulation::DeterministicBlockData> {
        public:
            typedef bisimulation::DeterministicBlockData BlockDataType;
            typedef typename ModelType::ValueType ValueType;
            typedef typename ModelType::RewardModelType RewardModelType;
            
            /*!
             * Computes the bisimulation relation for the given model. Which kind of bisimulation is computed, is
             * customizable via the given options.
             *
             * @param model The model to decompose.
             * @param options The options that customize the computed bisimulation.
             */
            NondeterministicModelBisimulationDecomposition(ModelType const& model, typename BisimulationDecomposition<ModelType, BlockDataType>::Options const& options = typename BisimulationDecomposition<ModelType, BlockDataType>::Options());
            
        protected:
            virtual std::pair<storm::storage::BitVector, storm::storage::BitVector> getStatesWithProbability01() override;
            
            virtual void buildQuotient() override;
            
            virtual void refinePartitionBasedOnSplitter(bisimulation::Block<BlockDataType>& splitter, std::deque<bisimulation::Block<BlockDataType>*>& splitterQueue) override;
            
        private:
            // Creates the mapping from the choice indices to the states.
            void createChoiceToStateMapping();
            
            // Retrieves whether the given predecessor of the splitters possibly needs refinement.
            bool possiblyNeedsRefinement(bisimulation::Block<BlockDataType> const& predecessorBlock) const;
            
            // Increases the probability of moving to the current splitter for the given choice of the given state.
            void increaseProbabilityToSplitter(storm::storage::sparse::state_type state, uint_fast64_t choice, bisimulation::Block<BlockDataType> const& predecessorBlock, ValueType const& value);
            
            // Clears the probabilities of all choices of the given state.
            void clearProbabilitiesToSplitter(storm::storage::sparse::state_type state);
            
            // Moves the given state to the position marked by marker1 moves the marker one step further.
            void moveStateToMarker1(storm::storage::sparse::state_type predecessor, bisimulation::Block<BlockDataType>& predecessorBlock);
            
            // Moves the given state to the position marked by marker2 the marker one step further.
            void moveStateToMarker2(storm::storage::sparse::state_type predecessor, bisimulation::Block<BlockDataType>& predecessorBlock);
            
            // Moves the given state to a proper place in the splitter, depending on where the predecessor is located.
            void moveStateInSplitter(storm::storage::sparse::state_type predecessor, bisimulation::Block<BlockDataType>& predecessorBlock, storm::storage::sparse::state_type currentPositionInSplitter, uint_fast64_t& elementsToSkip);
            
            // Inserts the block into the list of predecessors if it is not already contained.
            void insertIntoPredecessorList(bisimulation::Block<BlockDataType>& predecessorBlock, std::list<bisimulation::Block<BlockDataType>*>& predecessorBlocks);
            
            // Explores the remaining states of the splitter.
            void exploreRemainingStatesOfSplitter(bisimulation::Block<BlockDataType>& splitter, std::list<bisimulation::Block<BlockDataType>*>& predecessorBlocks);
            
            // Refines the predecessor blocks wrt. strong bisimulation.
            void refinePredecessorBlocksOfSplitter(std::list<bisimulation::Block<BlockDataType>*> const& predecessorBlocks, std::deque<bisimulation::Block<BlockDataType>*>& splitterQueue);
            
            // A mapping from choice indices to the state state that has this choice.
            std::vector<storm::storage::sparse::state_type> choiceToStateMapping;
            
            // A vector that holds the probabilities for all nondeterministic choices of all states of going into the
            // splitter. This is used by the method that refines a block based on probabilities.
            std::vector<ValueType> probabilitiesToCurrentSplitter;
            
        };
    }
}

#endif /* STORM_STORAGE_BISIMULATION_NONDETERMINISTICMODELBISIMULATIONDECOMPOSITION_H_ */