#ifndef STORM_STORAGE_DeterministicModelBisimulationDecomposition_H_
#define STORM_STORAGE_DeterministicModelBisimulationDecomposition_H_

#include <queue>
#include <deque>

#include "src/storage/sparse/StateType.h"
#include "src/storage/Decomposition.h"
#include "src/storage/StateBlock.h"
#include "src/logic/Formulas.h"
#include "src/models/sparse/Dtmc.h"
#include "src/models/sparse/Ctmc.h"
#include "src/storage/Distribution.h"
#include "src/utility/constants.h"
#include "src/utility/OsDetection.h"

namespace storm {
    namespace utility {
        template <typename ValueType> class ConstantsComparator;
    }
    
    namespace storage {
        
        /*!
         * This class represents the decomposition model into its (strong) bisimulation quotient.
         */
        template <typename ValueType>
        class DeterministicModelBisimulationDecomposition : public Decomposition<StateBlock> {
        public:
            // A class that offers the possibility to customize the bisimulation.
            struct Options {
                // Creates an object representing the default values for all options.
                Options();

                /*!
                 * Creates an object representing the options necessary to obtain the quotient while still preserving
                 * the given formula.
                 *
                 * @param The model for which the quotient model shall be computed. This needs to be given in order to
                 * derive a suitable initial partition.
                 * @param formula The formula that is to be preserved.
                 */
                Options(storm::models::sparse::Model<ValueType> const& model, storm::logic::Formula const& formula);

                /*!
                 * Creates an object representing the options necessary to obtain the smallest quotient while still
                 * preserving the given formula.
                 *
                 * @param The model for which the quotient model shall be computed. This needs to be given in order to
                 * derive a suitable initial partition.
                 * @param formulas The formula that is to be preserved.
                 */
                Options(storm::models::sparse::Model<ValueType> const& model, std::vector<std::shared_ptr<storm::logic::Formula>> const& formulas);
                
                // A flag that indicates whether a measure driven initial partition is to be used. If this flag is set
                // to true, the two optional pairs phiStatesAndLabel and psiStatesAndLabel must be set. Then, the
                // measure driven initial partition wrt. to the states phi and psi is taken.
                bool measureDrivenInitialPartition;
                boost::optional<storm::storage::BitVector> phiStates;
                boost::optional<storm::storage::BitVector> psiStates;
                
                // An optional set of strings that indicate which of the atomic propositions of the model are to be
                // respected and which may be ignored. If not given, all atomic propositions of the model are respected.
                boost::optional<std::set<std::string>> respectedAtomicPropositions;
                
                // A flag that indicates whether or not the state-rewards of the model are to be respected (and should
                // be kept in the quotient model, if one is built).
                bool keepRewards;
                
                // A flag that indicates whether a strong or a weak bisimulation is to be computed.
                bool weak;
                
                // A flag that indicates whether step-bounded properties are to be preserved. This may only be set to tru
                // when computing strong bisimulation equivalence.
                bool bounded;
                
                // A flag that governs whether the quotient model is actually built or only the decomposition is computed.
                bool buildQuotient;
                
            private:
                /*!
                 * Sets the options under the assumption that the given formula is the only one that is to be checked.
                 *
                 * @param model The model for which to preserve the formula.
                 * @param formula The only formula to check.
                 */
                void preserveSingleFormula(storm::models::sparse::Model<ValueType> const& model, storm::logic::Formula const& formula);
            };
            
            /*!
             * Decomposes the given DTMC into equivalance classes of a bisimulation. Which kind of bisimulation is
             * computed, is customizable via the given options.
             *
             * @param model The model to decompose.
             * @param options The options that customize the computed bisimulation.
             */
            DeterministicModelBisimulationDecomposition(storm::models::sparse::Dtmc<ValueType> const& model, Options const& options = Options());
            
            /*!
             * Decomposes the given CTMC into equivalance classes of a bisimulation. Which kind of bisimulation is
             * computed, is customizable via the given options.
             *
             * @param model The model to decompose.
             * @param options The options that customize the computed bisimulation.
             */
            DeterministicModelBisimulationDecomposition(storm::models::sparse::Ctmc<ValueType> const& model, Options const& options = Options());
            
            /*!
             * Retrieves the quotient of the model under the previously computed bisimulation.
             *
             * @return The quotient model.
             */
            std::shared_ptr<storm::models::sparse::DeterministicModel<ValueType>> getQuotient() const;
            
        private:
            enum class BisimulationType { Strong, WeakDtmc, WeakCtmc };
            
            class Partition;
            
            class Block {
            public:
                typedef typename std::list<Block>::iterator iterator;
                typedef typename std::list<Block>::const_iterator const_iterator;
                
                // Creates a new block with the given begin and end.
                Block(storm::storage::sparse::state_type begin, storm::storage::sparse::state_type end, Block* prev, Block* next, std::size_t id);
                
                // Prints the block.
                void print(Partition const& partition) const;

                // Sets the beginning index of the block.
                void setBegin(storm::storage::sparse::state_type begin);

                // Moves the beginning index of the block one step further.
                void incrementBegin();
                
                // Sets the end index of the block.
                void setEnd(storm::storage::sparse::state_type end);

                // Returns the beginning index of the block.
                storm::storage::sparse::state_type getBegin() const;
                
                // Returns the beginning index of the block.
                storm::storage::sparse::state_type getEnd() const;
                
                // Retrieves the original beginning index of the block in case the begin index has been moved.
                storm::storage::sparse::state_type getOriginalBegin() const;
                
                // Returns the iterator the block in the list of overall blocks.
                iterator getIterator() const;

                // Returns the iterator the block in the list of overall blocks.
                void setIterator(iterator it);
                
                // Returns the iterator the next block in the list of overall blocks if it exists.
                iterator getNextIterator() const;

                // Returns the iterator the next block in the list of overall blocks if it exists.
                iterator getPreviousIterator() const;

                // Gets the next block (if there is one).
                Block& getNextBlock();

                // Gets the next block (if there is one).
                Block const& getNextBlock() const;
                
                // Gets a pointer to the next block (if there is one).
                Block* getNextBlockPointer();

                // Retrieves whether the block as a successor block.
                bool hasNextBlock() const;

                // Gets the previous block (if there is one).
                Block& getPreviousBlock();

                // Gets a pointer to the previous block (if there is one).
                Block* getPreviousBlockPointer();
                
                // Gets the next block (if there is one).
                Block const& getPreviousBlock() const;

                // Retrieves whether the block as a successor block.
                bool hasPreviousBlock() const;
                
                // Checks consistency of the information in the block.
                bool check() const;
                
                // Retrieves the number of states in this block.
                std::size_t getNumberOfStates() const;
                
                // Checks whether the block is marked as a splitter.
                bool isMarkedAsSplitter() const;
                
                // Marks the block as being a splitter.
                void markAsSplitter();
                
                // Removes the mark.
                void unmarkAsSplitter();
                
                // Retrieves the ID of the block.
                std::size_t getId() const;
                
                // Retrieves the marked position in the block.
                storm::storage::sparse::state_type getMarkedPosition() const;

                // Sets the marked position to the given value..
                void setMarkedPosition(storm::storage::sparse::state_type position);

                // Increases the marked position by one.
                void incrementMarkedPosition();
                
                // Resets the marked position to the begin of the block.
                void resetMarkedPosition();
                
                // Retrieves whether the block is marked as a predecessor.
                bool isMarkedAsPredecessor() const;
                
                // Marks the block as being a predecessor block.
                void markAsPredecessorBlock();
                
                // Removes the marking.
                void unmarkAsPredecessorBlock();
                
                // Sets whether or not the block is to be interpreted as absorbing.
                void setAbsorbing(bool absorbing);
                
                // Retrieves whether the block is to be interpreted as absorbing.
                bool isAbsorbing() const;
                
                // Sets the representative state of this block
                void setRepresentativeState(storm::storage::sparse::state_type representativeState);
                
                // Retrieves the representative state for this block.
                bool hasRepresentativeState() const;

                // Retrieves the representative state for this block.
                storm::storage::sparse::state_type getRepresentativeState() const;
                
            private:
                // An iterator to itself. This is needed to conveniently insert elements in the overall list of blocks
                // kept by the partition.
                iterator selfIt;
                
                // Pointers to the next and previous block.
                Block* next;
                Block* prev;
                
                // The begin and end indices of the block in terms of the state vector of the partition.
                storm::storage::sparse::state_type begin;
                storm::storage::sparse::state_type end;
                
                // A field that can be used for marking the block.
                bool markedAsSplitter;
                
                // A field that can be used for marking the block as a predecessor block.
                bool markedAsPredecessorBlock;
                
                // A position that can be used to store a certain position within the block.
                storm::storage::sparse::state_type markedPosition;
                
                // A flag indicating whether the block is to be interpreted as absorbing or not.
                bool absorbing;
                
                // The ID of the block. This is only used for debugging purposes.
                std::size_t id;
                
                // An optional representative state for the block. If this is set, this state is used to derive the
                // atomic propositions of the meta state in the quotient model.
                boost::optional<storm::storage::sparse::state_type> representativeState;
            };
            
            class Partition {
            public:
                friend class Block;
                
                /*!
                 * Creates a partition with one block consisting of all the states.
                 *
                 * @param numberOfStates The number of states the partition holds.
                 * @param keepSilentProbabilities A flag indicating whether or not silent probabilities are to be tracked.
                 */
                Partition(std::size_t numberOfStates, bool keepSilentProbabilities = false);

                /*!
                 * Creates a partition with three blocks: one with all phi states, one with all psi states and one with
                 * all other states. The former two blocks are marked as being absorbing, because their outgoing
                 * transitions shall not be taken into account for future refinement.
                 *
                 * @param numberOfStates The number of states the partition holds.
                 * @param prob0States The states which have probability 0 of satisfying phi until psi.
                 * @param prob1States The states which have probability 1 of satisfying phi until psi.
                 * @param representativeProb1State If the set of prob1States is non-empty, this needs to be a state
                 * that is representative for this block in the sense that the state representing this block in the quotient
                 * model will receive exactly the atomic propositions of the representative state.
                 * @param keepSilentProbabilities A flag indicating whether or not silent probabilities are to be tracked.
                 */
                Partition(std::size_t numberOfStates, storm::storage::BitVector const& prob0States, storm::storage::BitVector const& prob1States, boost::optional<storm::storage::sparse::state_type> representativeProb1State, bool keepSilentProbabilities = false);
                
                Partition() = default;
                Partition(Partition const& other) = default;
                Partition& operator=(Partition const& other) = default;

                // Define move-construct and move-assignment explicitly to make sure they exist (as they are vital to
                // keep validity of pointers.
				Partition(Partition&& other) : blocks(std::move(other.blocks)), numberOfBlocks(other.numberOfBlocks), stateToBlockMapping(std::move(other.stateToBlockMapping)), statesAndValues(std::move(other.statesAndValues)), positions(std::move(other.positions)), keepSilentProbabilities(other.keepSilentProbabilities), silentProbabilities(std::move(other.silentProbabilities)) {
                    // Intentionally left empty.
				}
                
				Partition& operator=(Partition&& other) {
                    if (this != &other) {
                        blocks = std::move(other.blocks);
                        numberOfBlocks = other.numberOfBlocks;
                        stateToBlockMapping = std::move(other.stateToBlockMapping);
                        statesAndValues = std::move(other.statesAndValues);
                        positions = std::move(other.positions);
                        keepSilentProbabilities = other.keepSilentProbabilities;
                        silentProbabilities = std::move(other.silentProbabilities);
                    }

					return *this;
				}
                
                /*!
                 * Splits all blocks of the partition such that afterwards all blocks contain only states with the label
                 * or no labeled state at all.
                 */
                void splitLabel(storm::storage::BitVector const& statesWithLabel);
                
                // Retrieves the size of the partition, i.e. the number of blocks.
                std::size_t size() const;
                
                // Prints the partition to the standard output.
                void print() const;

                // Splits the block at the given position and inserts a new block after the current one holding the rest
                // of the states.
                Block& splitBlock(Block& block, storm::storage::sparse::state_type position);
                
                // Inserts a block before the given block. The new block will cover all states between the beginning
                // of the given block and the end of the previous block.
                Block& insertBlock(Block& block);
                
                // Retrieves the blocks of the partition.
                std::list<Block> const& getBlocks() const;

                // Retrieves the blocks of the partition.
                std::list<Block>& getBlocks();

                // Checks the partition for internal consistency.
                bool check() const;
                
                // Returns an iterator to the beginning of the states of the given block.
                typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::iterator getBegin(Block const& block);
                
                // Returns an iterator to the beginning of the states of the given block.
                typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::iterator getEnd(Block const& block);

                // Returns an iterator to the beginning of the states of the given block.
                typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::const_iterator getBegin(Block const& block) const;
                
                // Returns an iterator to the beginning of the states of the given block.
                typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::const_iterator getEnd(Block const& block) const;

                // Swaps the positions of the two given states.
                void swapStates(storm::storage::sparse::state_type state1, storm::storage::sparse::state_type state2);

                // Swaps the positions of the two states given by their positions.
                void swapStatesAtPositions(storm::storage::sparse::state_type position1, storm::storage::sparse::state_type position2);

                // Retrieves the block of the given state.
                Block& getBlock(storm::storage::sparse::state_type state);

                // Retrieves the block of the given state.
                Block const& getBlock(storm::storage::sparse::state_type state) const;

                // Retrieves the position of the given state.
                storm::storage::sparse::state_type const& getPosition(storm::storage::sparse::state_type state) const;

                // Retrieves the position of the given state.
                void setPosition(storm::storage::sparse::state_type state, storm::storage::sparse::state_type position);
                
                // Sets the position of the state to the given position.
                storm::storage::sparse::state_type const& getState(storm::storage::sparse::state_type position) const;

                // Retrieves the value for the given state.
                ValueType const& getValue(storm::storage::sparse::state_type state) const;
                
                // Retrieves the value at the given position.
                ValueType const& getValueAtPosition(storm::storage::sparse::state_type position) const;
                
                // Sets the given value for the given state.
                void setValue(storm::storage::sparse::state_type state, ValueType value);
                
                // Retrieves the vector with the probabilities going into the current splitter.
                std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>& getStatesAndValues();

                // Increases the value for the given state by the specified amount.
                void increaseValue(storm::storage::sparse::state_type state, ValueType value);
                
                // Updates the block mapping for the given range of states to the specified block.
                void updateBlockMapping(Block& block, typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::iterator first, typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::iterator last);
                
                // Retrieves the first block of the partition.
                Block& getFirstBlock();
                
                // Retrieves whether the given state is fully silent (only in case the silent probabilities are tracked).
                bool isSilent(storm::storage::sparse::state_type state, storm::utility::ConstantsComparator<ValueType> const& comparator) const;
                
                // Retrieves whether the given state has a non-zero silent probability.
                bool hasSilentProbability(storm::storage::sparse::state_type state, storm::utility::ConstantsComparator<ValueType> const& comparator) const;
                
                // Retrieves the silent probability (i.e. the probability to stay within the own equivalence class).
                ValueType const& getSilentProbability(storm::storage::sparse::state_type state) const;
                
                // Sets the silent probabilities for all the states in the range to their values in the range.
                void setSilentProbabilities(typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::iterator first, typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::iterator last);
                
                // Sets the silent probabilities for all states in the range to zero.
                void setSilentProbabilitiesToZero(typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::iterator first, typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::iterator last);

                // Sets the silent probability for the given state to the given value.
                void setSilentProbability(storm::storage::sparse::state_type state, ValueType const& value);

            private:
                // The list of blocks in the partition.
                std::list<Block> blocks;
                
                // The number of blocks of the partition. Although this could be retrieved via block.size(), we want to
                // guarantee constant access time (which is currently not guaranteed by gcc'c standard libary).
                uint_fast64_t numberOfBlocks;
                
                // A mapping of states to their blocks.
                std::vector<Block*> stateToBlockMapping;
                
                // A vector containing all the states and their values. It is ordered in a special way such that the
                // blocks only need to define their start/end indices.
                std::vector<std::pair<storm::storage::sparse::state_type, ValueType>> statesAndValues;
                
                // This vector keeps track of the position of each state in the state vector.
                std::vector<storm::storage::sparse::state_type> positions;
                
                // A flag that indicates whether or not the vector with silent probabilities exists.
                bool keepSilentProbabilities;
                
                // This vector keeps track of the silent probabilities (i.e. the probabilities going into the very own
                // equivalence class) of each state. This means that a state is silent iff its entry is non-zero.
                std::vector<ValueType> silentProbabilities;
            };
            
            /*!
             * Performs the partition refinement on the model and thereby computes the equivalence classes under strong
             * bisimulation equivalence. If required, the quotient model is built and may be retrieved using
             * getQuotient().
             *
             * @param model The model on whose state space to compute the coarses strong bisimulation relation.
             * @param atomicPropositions The set of atomic propositions that the bisimulation considers.
             * @param backwardTransitions The backward transitions of the model.
             * @param The initial partition.
             * @param bisimulationType The kind of bisimulation that is to be computed.
             * @param buildQuotient If set, the quotient model is built and may be retrieved using the getQuotient()
             * method.
             * @param comparator A comparator used for comparing constants.
             */
            template<typename ModelType>
            void partitionRefinement(ModelType const& model, std::set<std::string> const& atomicPropositions, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, Partition& partition, BisimulationType bisimulationType, bool keepRewards, bool buildQuotient, storm::utility::ConstantsComparator<ValueType> const& comparator);
            
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
             * Builds the quotient model based on the previously computed equivalence classes (stored in the blocks
             * of the decomposition.
             *
             * @param model The model whose state space was used for computing the equivalence classes. This is used for
             * determining the transitions of each equivalence class.
             * @param selectedAtomicPropositions The set of atomic propositions that was considered by the bisimulation.
             * The quotient will only have these atomic propositions.
             * @param partition The previously computed partition. This is used for quickly retrieving the block of a
             * state.
             * @param bisimulationType The kind of bisimulation that is to be computed.
             * @param comparator A comparator used for comparing constants.
             */
            template<typename ModelType>
            void buildQuotient(ModelType const& model, std::set<std::string> const& selectedAtomicPropositions, Partition const& partition, BisimulationType bisimulationType, bool keepRewards, storm::utility::ConstantsComparator<ValueType> const& comparator);

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
            Partition getLabelBasedInitialPartition(ModelType const& model, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, BisimulationType bisimulationType, boost::optional<std::set<std::string>> const& atomicPropositions = boost::optional<std::set<std::string>>(), bool keepRewards = true, storm::utility::ConstantsComparator<ValueType> const& comparator = storm::utility::ConstantsComparator<ValueType>());
            
            /*!
             * Splits all blocks of the given partition into a block that contains all divergent states and another block
             * containing the non-divergent states.
             *
             * @param model The model from which to look-up the probabilities.
             * @param backwardTransitions The backward transitions of the model.
             * @param partition The partition that holds the silent probabilities.
             */
            template<typename ModelType>
            void splitOffDivergentStates(ModelType const& model, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, Partition& partition);
            
            /*!
             * Initializes the silent probabilities by traversing all blocks and adding the probability of going to
             * the very own equivalence class for each state.
             *
             * @param model The model from which to look-up the probabilities.
             * @param partition The partition that holds the silent probabilities.
             */
            template<typename ModelType>
            void initializeSilentProbabilities(ModelType const& model, Partition& partition);
            
            /*!
             * Splits all blocks of the partition in a way such that the states of each block agree on the rewards.
             *
             * @param stateRewardVector The state reward vector of the model.
             * @param partition The partition that is to be split.
             * @param comparator A comparator used for comparing constants.
             */
            void splitRewards(std::vector<ValueType> const& stateRewardVector, Partition& partition, storm::utility::ConstantsComparator<ValueType> const& comparator);
            
            // If required, a quotient model is built and stored in this member.
            std::shared_ptr<storm::models::sparse::DeterministicModel<ValueType>> quotient;
        };
    }
}

#endif /* STORM_STORAGE_DeterministicModelBisimulationDecomposition_H_ */