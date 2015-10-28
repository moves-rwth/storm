#ifndef STORM_STORAGE_BISIMULATION_BLOCK_H_
#define STORM_STORAGE_BISIMULATION_BLOCK_H_

#include <list>
#include <boost/optional.hpp>

#include "src/storage/sparse/StateType.h"

namespace storm {
    namespace storage {
        namespace bisimulation {
            // Forward-declare partition class.
            template<typename DataType>
            class Partition;
            
            template<typename DataType>
            class Block {
            public:
                friend class Partition<DataType>;
                
                // Creates a new block with the given begin and end.
                Block(storm::storage::sparse::state_type beginIndex, storm::storage::sparse::state_type endIndex, Block* previous, Block* next, std::size_t id);
                
                Block() = default;
                Block(Block const& other) = default;
                Block& operator=(Block const& other) = default;
                Block(Block&& other) = default;
                Block& operator=(Block&& other) = default;
                
                bool operator==(Block const& other) const;
                bool operator!=(Block const& other) const;
                
                // Prints the block to the standard output.
                void print(Partition<DataType> const& partition) const;
                
                // Returns the beginning index of the block.
                storm::storage::sparse::state_type getBeginIndex() const;
                
                // Returns the beginning index of the block.
                storm::storage::sparse::state_type getEndIndex() const;
                
                // Gets the next block (if there is one).
                Block const& getNextBlock() const;

                // Gets a pointer to the next block (if there is one).
                Block* getNextBlockPointer();
                
                // Gets a pointer to the next block (if there is one).
                Block const* getNextBlockPointer() const;
                
                // Retrieves whether the block as a successor block.
                bool hasNextBlock() const;
                
                // Gets the next block (if there is one).
                Block const& getPreviousBlock() const;

                // Gets a pointer to the previous block (if there is one).
                Block* getPreviousBlockPointer();

                // Gets a pointer to the previous block (if there is one).
                Block const* getPreviousBlockPointer() const;
                
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
                
                // Retrieves whether the block is marked as a predecessor.
                bool needsRefinement() const;
                
                // Marks the block as needing refinement (or not).
                void setNeedsRefinement(bool value = true);
                
                // Sets whether or not the block is to be interpreted as absorbing.
                void setAbsorbing(bool absorbing);
                
                // Retrieves whether the block is to be interpreted as absorbing.
                bool isAbsorbing() const;
                
                // Sets the representative state of this block
                void setRepresentativeState(storm::storage::sparse::state_type representativeState);
                
                // Retrieves whether this block has a representative state.
                bool hasRepresentativeState() const;
                
                // Retrieves the representative state for this block.
                storm::storage::sparse::state_type getRepresentativeState() const;
                
                // Retrieves the additional data associated with this block.
                DataType& data();
                
                // Retrieves the additional data associated with this block.
                DataType const& data() const;
                
            private:
                // Sets the beginning index of the block.
                void setBeginIndex(storm::storage::sparse::state_type beginIndex);
                
                // Sets the end index of the block.
                void setEndIndex(storm::storage::sparse::state_type endIndex);
                
                // Pointers to the next and previous block.
                Block* nextBlock;
                Block* previousBlock;
                
                // The begin and end indices of the block in terms of the state vector of the partition.
                storm::storage::sparse::state_type beginIndex;
                storm::storage::sparse::state_type endIndex;
                
                // A field that can be used for marking the block.
                bool markedAsSplitter;
                
                // A field that can be used for marking the block as needing refinement.
                bool needsRefinementFlag;
                
                // A flag indicating whether the block is to be interpreted as absorbing or not.
                bool absorbing;
                
                // The ID of the block. This is only used for debugging purposes.
                std::size_t id;
                
                // An optional representative state for the block. If this is set, this state is used to derive the
                // atomic propositions of the meta state in the quotient model.
                boost::optional<storm::storage::sparse::state_type> representativeState;
                
                // A member that stores additional data that depends on the kind of bisimulation.
                DataType mData;
            };
        }
    }
}

#endif /* STORM_STORAGE_BISIMULATION_BLOCK_H_ */
