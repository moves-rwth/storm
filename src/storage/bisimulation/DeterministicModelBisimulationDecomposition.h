#ifndef STORM_STORAGE_BISIMULATION_DETERMINISTICMODELBISIMULATIONDECOMPOSITION_H_
#define STORM_STORAGE_BISIMULATION_DETERMINISTICMODELBISIMULATIONDECOMPOSITION_H_

#include "src/storage/bisimulation/BisimulationDecomposition.h"

namespace storm {
    namespace utility {
        template <typename ValueType> class ConstantsComparator;
    }
    
    namespace storage {
        
        /*!
         * This class represents the decomposition of a deterministic model into its bisimulation quotient.
         */
        template<typename ModelType>
        class DeterministicModelBisimulationDecomposition : public BisimulationDecomposition<ModelType> {
        public:
            typedef typename ModelType::ValueType ValueType;
            typedef typename ModelType::RewardModelType RewardModelType;
            
            /*!
             * Computes the bisimulation relation for the given model. Which kind of bisimulation is computed, is
             * customizable via the given options.
             *
             * @param model The model to decompose.
             * @param options The options that customize the computed bisimulation.
             */
            DeterministicModelBisimulationDecomposition(ModelType const& model, typename BisimulationDecomposition<ModelType>::Options const& options = typename BisimulationDecomposition<ModelType>::Options());
            
        protected:
            virtual std::pair<storm::storage::BitVector, storm::storage::BitVector> getStatesWithProbability01() override;

            virtual void initializeMeasureDrivenPartition() override;
            
            virtual void initializeLabelBasedPartition() override;
            
            virtual void buildQuotient() override;
            
            virtual void refinePartitionBasedOnSplitter(bisimulation::Block const& splitter, std::deque<bisimulation::Block*>& splitterQueue) override;

        private:
            virtual void refinePredecessorBlocksOfSplitter(std::list<bisimulation::Block*>& predecessorBlocks, std::deque<bisimulation::Block*>& splitterQueue);

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
            
            // Retrieves whether the given state is a predecessor of the current splitter.
            bool isPredecessorOfCurrentSplitter(storm::storage::sparse::state_type const& state) const;
            
            // A vector that holds the probabilities of states going into the splitter. This is used by the method that
            // refines a block based on probabilities.
            std::vector<ValueType> probabilitiesToCurrentSplitter;
            
            // A bit vector storing the predecessors of the current splitter.
            storm::storage::BitVector predecessorsOfCurrentSplitter;
            
            // A vector mapping each state to its silent probability.
            std::vector<ValueType> silentProbabilities;
        };
    }
}

#endif /* STORM_STORAGE_BISIMULATION_DETERMINISTICMODELBISIMULATIONDECOMPOSITION_H_ */