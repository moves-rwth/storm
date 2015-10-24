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
            DeterministicModelBisimulationDecomposition(ModelType const& model, typename BisimulationDecomposition<ModelType>::Options const& options = Options());
            
        private:
            virtual std::pair<storm::storage::BitVector, storm::storage::BitVector> getStatesWithProbability01(storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) override;
            
            virtual void initializeLabelBasedPartition() override;
            
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
            
            virtual void initializeMeasureDrivenPartition() override;
            
            virtual void initializeLabelBasedPartition() override;
            
            virtual void buildQuotient() override;
            
            /*!
             * Refines the partition based on the provided splitter. After calling this method all blocks are stable
             * with respect to the splitter.
             *
             * @param forwardTransitions The forward transitions of the model.
             * @param backwardTransitions A matrix that can be used to retrieve the predecessors (and their
             * probabilities).
             * @param splitter The splitter to use.
             * @param partition The partition to split.
             * @param bisimulationType The kind of bisimulation that is to be computed.
             * @param splitterQueue A queue into which all blocks that were split are inserted so they can be treated
             * as splitters in the future.
             * @param comparator A comparator used for comparing constants.
             */
            void refinePartition(storm::storage::SparseMatrix<ValueType> const& forwardTransitions, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, Block& splitter, Partition& partition, BisimulationType bisimulationType, std::deque<Block*>& splitterQueue, storm::utility::ConstantsComparator<ValueType> const& comparator);
            
            /*!
             * Refines the block based on their probability values (leading into the splitter).
             *
             * @param block The block to refine.
             * @param partition The partition that contains the block.
             * @param bisimulationType The kind of bisimulation that is to be computed.
             * @param splitterQueue A queue into which all blocks that were split are inserted so they can be treated
             * as splitters in the future.
             * @param comparator A comparator used for comparing constants.
             */
            void refineBlockProbabilities(Block& block, Partition& partition, BisimulationType bisimulationType, std::deque<Block*>& splitterQueue, storm::utility::ConstantsComparator<ValueType> const& comparator);
            
            void refineBlockWeak(Block& block, Partition& partition, storm::storage::SparseMatrix<ValueType> const& forwardTransitions, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::deque<Block*>& splitterQueue, storm::utility::ConstantsComparator<ValueType> const& comparator);
            
            /*!
             * Determines the split offsets in the given block.
             *
             * @param block The block that is to be analyzed for splits.
             * @param partition The partition that contains the block.
             * @param comparator A comparator used for comparing constants.
             */
            std::vector<uint_fast64_t> getSplitPointsWeak(Block& block, Partition& partition, storm::utility::ConstantsComparator<ValueType> const& comparator);
            

            
            
            
            
            
            
            
            /*!
             * Creates the measure-driven initial partition for reaching psi states from phi states.
             *
             * @param model The model whose state space is partitioned based on reachability of psi states from phi
             * states.
             * @param backwardTransitions The backward transitions of the model.
             * @param phiStates The phi states in the model.
             * @param psiStates The psi states in the model.
             * @param bisimulationType The kind of bisimulation that is to be computed.
             * @param bounded If set to true, the initial partition will be chosen in such a way that preserves bounded
             * reachability queries.
             * @param comparator A comparator used for comparing constants.
             * @return The resulting partition.
             */
            template<typename ModelType>
            Partition getMeasureDrivenInitialPartition(ModelType const& model, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, BisimulationType bisimulationType, bool keepRewards = true, bool bounded = false, storm::utility::ConstantsComparator<ValueType> const& comparator = storm::utility::ConstantsComparator<ValueType>());
            
            /*!
             * Creates the initial partition based on all the labels in the given model.
             *
             * @param model The model whose state space is partitioned based on its labels.
             * @param backwardTransitions The backward transitions of the model.
             * @param bisimulationType The kind of bisimulation that is to be computed.
             * @param atomicPropositions The set of atomic propositions to respect. If not given, then all atomic
             * propositions of the model are respected.
             * @param comparator A comparator used for comparing constants.
             * @return The resulting partition.
             */
            template<typename ModelType>
            Partition getLabelBasedInitialPartition(ModelType const& model, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, BisimulationType bisimulationType, std::set<std::string> const& atomicPropositions, bool keepRewards = true, storm::utility::ConstantsComparator<ValueType> const& comparator = storm::utility::ConstantsComparator<ValueType>());

            // A vector mapping each state to its silent probability.
            std::vector<ValueType> silentProbabilities;
        };
    }
}

#endif /* STORM_STORAGE_BISIMULATION_DETERMINISTICMODELBISIMULATIONDECOMPOSITION_H_ */