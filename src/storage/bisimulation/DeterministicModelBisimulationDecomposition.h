#ifndef STORM_STORAGE_BISIMULATION_DETERMINISTICMODELBISIMULATIONDECOMPOSITION_H_
#define STORM_STORAGE_BISIMULATION_DETERMINISTICMODELBISIMULATIONDECOMPOSITION_H_

#include "src/storage/bisimulation/BisimulationDecomposition.h"
#include "src/storage/bisimulation/DeterministicBlockData.h"

namespace storm {
    namespace utility {
        template <typename ValueType> class ConstantsComparator;
    }
    
    namespace storage {
        
        /*!
         * This class represents the decomposition of a deterministic model into its bisimulation quotient.
         */
        template<typename ModelType>
        class DeterministicModelBisimulationDecomposition : public BisimulationDecomposition<ModelType, bisimulation::DeterministicBlockData> {
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
            DeterministicModelBisimulationDecomposition(ModelType const& model, typename BisimulationDecomposition<ModelType, BlockDataType>::Options const& options = typename BisimulationDecomposition<ModelType, BlockDataType>::Options());
            
        protected:
            virtual std::pair<storm::storage::BitVector, storm::storage::BitVector> getStatesWithProbability01() override;

            virtual void initializeMeasureDrivenPartition() override;
            
            virtual void initializeLabelBasedPartition() override;
            
            virtual void buildQuotient() override;
            
            virtual void refinePartitionBasedOnSplitter(bisimulation::Block<BlockDataType>& splitter, std::deque<bisimulation::Block<BlockDataType>*>& splitterQueue) override;

        private:
            virtual void refinePredecessorBlocksOfSplitter(std::list<bisimulation::Block<BlockDataType>*>& predecessorBlocks, std::deque<bisimulation::Block<BlockDataType>*>& splitterQueue);

            /*!
             * Performs the necessary steps to compute a weak bisimulation on a DTMC.
             */
            void initializeWeakDtmcBisimulation();
            
            /*!
             * Splits all blocks of the current partition into a block that contains all divergent states and another
             * block containing the non-divergent states.
             */
            void splitOffDivergentStates();
            
            /*!
             * Initializes the vector of silent probabilities.
             */
            void initializeSilentProbabilities();
            
            // Retrieves the probability of going into the splitter for the given state.
            ValueType const& getProbabilityToSplitter(storm::storage::sparse::state_type const& state) const;
            
            // Retrieves the silent probability for the given state.
            ValueType getSilentProbability(storm::storage::sparse::state_type const& state) const;
            
            // Retrieves whether the given state is silent.
            bool isSilent(storm::storage::sparse::state_type const& state) const;
            
            // Retrieves whether the given predecessor of the splitters possibly needs refinement.
            bool possiblyNeedsRefinement(bisimulation::Block<BlockDataType> const& predecessorBlock) const;
            
            // Moves the given state to the position marked by marker1 moves the marker one step further.
            void moveStateToMarker1(storm::storage::sparse::state_type predecessor, bisimulation::Block<BlockDataType>& predecessorBlock);

            // Moves the given state to the position marked by marker2 the marker one step further.
            void moveStateToMarker2(storm::storage::sparse::state_type predecessor, bisimulation::Block<BlockDataType>& predecessorBlock);
            
            // Moves the given state to a proper place in the splitter, depending on where the predecessor is located.
            void moveStateInSplitter(storm::storage::sparse::state_type predecessor, bisimulation::Block<BlockDataType>& predecessorBlock, storm::storage::sparse::state_type currentPositionInSplitter, uint_fast64_t& elementsToSkip);
            
            // Increases the probability of moving to the current splitter for the given state.
            void increaseProbabilityToSplitter(storm::storage::sparse::state_type predecessor, bisimulation::Block<BlockDataType> const& predecessorBlock, ValueType const& value);
            
            // Explores the remaining predecessors of the splitter.
            void exploreRemainingStatesOfSplitter(bisimulation::Block<BlockDataType>& splitter);
            
            // A vector that holds the probabilities of states going into the splitter. This is used by the method that
            // refines a block based on probabilities.
            std::vector<ValueType> probabilitiesToCurrentSplitter;
            
            // A vector mapping each state to its silent probability.
            std::vector<ValueType> silentProbabilities;
        };
    }
}

#endif /* STORM_STORAGE_BISIMULATION_DETERMINISTICMODELBISIMULATIONDECOMPOSITION_H_ */