#ifndef STORM_STORAGE_DETERMINISTICMODELBISIMULATIONDECOMPOSITION_H_
#define STORM_STORAGE_DETERMINISTICMODELBISIMULATIONDECOMPOSITION_H_

#include <queue>
#include <deque>

#include "src/storage/sparse/StateType.h"
#include "src/storage/Decomposition.h"
#include "src/models/Dtmc.h"
#include "src/storage/Distribution.h"

namespace storm {
    namespace storage {
        
        /*!
         * This class represents the decomposition model into its bisimulation quotient.
         */
        template <typename ValueType>
        class DeterministicModelBisimulationDecomposition : public Decomposition<StateBlock> {
        public:
            /*!
             * Decomposes the given DTMC into equivalence classes under weak or strong bisimulation.
             */
            DeterministicModelBisimulationDecomposition(storm::models::Dtmc<ValueType> const& model, bool weak = false, bool buildQuotient = false);
            
            /*!
             * Retrieves the quotient of the model under the previously computed bisimulation.
             *
             * @return The quotient model.
             */
            std::shared_ptr<storm::models::AbstractDeterministicModel<ValueType>> getQuotient() const;
            
        private:
            class Partition;
            
            class Block {
            public:
                typedef typename std::list<Block>::const_iterator const_iterator;
                
                Block(storm::storage::sparse::state_type begin, storm::storage::sparse::state_type end, Block* prev, Block* next);
                
                // Prints the block.
                void print(Partition const& partition) const;

                // Sets the beginning index of the block.
                void setBegin(storm::storage::sparse::state_type begin);

                // Moves the beginning index of the block one step further.
                void incrementBegin();
                
                // Sets the end index of the block.
                void setEnd(storm::storage::sparse::state_type end);

                // Moves the end index of the block one step to the front.
                void decrementEnd();
                
                // Returns the beginning index of the block.
                storm::storage::sparse::state_type getBegin() const;
                
                // Returns the beginning index of the block.
                storm::storage::sparse::state_type getEnd() const;
                
                // Retrieves the original beginning index of the block in case the begin index has been moved.
                storm::storage::sparse::state_type getOriginalBegin() const;
                
                // Returns the iterator the block in the list of overall blocks.
                const_iterator getIterator() const;

                // Returns the iterator the block in the list of overall blocks.
                void setIterator(const_iterator it);
                
                // Returns the iterator the next block in the list of overall blocks if it exists.
                const_iterator getNextIterator() const;

                // Returns the iterator the next block in the list of overall blocks if it exists.
                const_iterator getPreviousIterator() const;

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
                
            private:
                // An iterator to itself. This is needed to conveniently insert elements in the overall list of blocks
                // kept by the partition.
                const_iterator selfIt;
                
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
                
                // The ID of the block. This is only used for debugging purposes.
                std::size_t id;
            };
            
            class Partition {
            public:
                friend class Block;
                
                /*!
                 * Creates a partition with one block consisting of all the states.
                 */
                Partition(std::size_t numberOfStates);
                
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
                
                // Retrieves the vector of all the states.
                std::vector<storm::storage::sparse::state_type>& getStates();

                // Checks the partition for internal consistency.
                bool check() const;
                
                // Returns an iterator to the beginning of the states of the given block.
                std::vector<storm::storage::sparse::state_type>::iterator getBeginOfStates(Block const& block);
                
                // Returns an iterator to the beginning of the states of the given block.
                std::vector<storm::storage::sparse::state_type>::iterator getEndOfStates(Block const& block);

                // Returns an iterator to the beginning of the states of the given block.
                std::vector<storm::storage::sparse::state_type>::const_iterator getBeginOfStates(Block const& block) const;
                
                // Returns an iterator to the beginning of the states of the given block.
                std::vector<storm::storage::sparse::state_type>::const_iterator getEndOfStates(Block const& block) const;
                
                // Returns an iterator to the beginning of the states of the given block.
                typename std::vector<ValueType>::iterator getBeginOfValues(Block const& block);
                
                // Returns an iterator to the beginning of the states of the given block.
                typename std::vector<ValueType>::iterator getEndOfValues(Block const& block);

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
                std::vector<ValueType>& getValues();

                // Increases the value for the given state by the specified amount.
                void increaseValue(storm::storage::sparse::state_type state, ValueType value);
                
                // Updates the block mapping for the given range of states to the specified block.
                void updateBlockMapping(Block& block, std::vector<storm::storage::sparse::state_type>::iterator first, std::vector<storm::storage::sparse::state_type>::iterator end);
                
            private:
                // The list of blocks in the partition.
                std::list<Block> blocks;
                
                // A mapping of states to their blocks.
                std::vector<Block*> stateToBlockMapping;
                
                // A vector containing all the states. It is ordered in a special way such that the blocks only need to
                // define their start/end indices.
                std::vector<storm::storage::sparse::state_type> states;
                
                // This vector keeps track of the position of each state in the state vector.
                std::vector<storm::storage::sparse::state_type> positions;
                
                // This vector stores the probabilities of going to the current splitter.
                std::vector<ValueType> values;
            };
            
            void computeBisimulationEquivalenceClasses(storm::models::Dtmc<ValueType> const& model, bool weak, bool buildQuotient);
            
            std::size_t splitPartition(storm::storage::SparseMatrix<ValueType> const& backwardTransitions, Block& splitter, Partition& partition, std::deque<Block*>& splitterQueue);
            
            std::size_t splitBlockProbabilities(Block& block, Partition& partition, std::deque<Block*>& splitterQueue);
            
            void buildQuotient(storm::models::Dtmc<ValueType> const& dtmc, Partition const& partition);

            // If required, a quotient model is built and stored in this member.
            std::shared_ptr<storm::models::AbstractDeterministicModel<ValueType>> quotient;
        };
    }
}

#endif /* STORM_STORAGE_DETERMINISTICMODELBISIMULATIONDECOMPOSITION_H_ */