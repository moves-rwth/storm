#ifndef STORM_STORAGE_BISIMULATIONDECOMPOSITION2_H_
#define STORM_STORAGE_BISIMULATIONDECOMPOSITION2_H_

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
        class BisimulationDecomposition2 : public Decomposition<StateBlock> {
        public:
            BisimulationDecomposition2() = default;
            
            /*!
             * Decomposes the given DTMC into equivalence classes under weak or strong bisimulation.
             */
            BisimulationDecomposition2(storm::models::Dtmc<ValueType> const& model, bool weak = false);
            
        private:
            class Block {
            public:
                Block(storm::storage::sparse::state_type begin, storm::storage::sparse::state_type end, Block* prev, Block* next);
                
                // An iterator to itself. This is needed to conveniently insert elements in the overall list of blocks
                // kept by the partition.
                typename std::list<Block>::iterator itToSelf;
                
                // The begin and end indices of the block in terms of the state vector of the partition.
                storm::storage::sparse::state_type begin;
                storm::storage::sparse::state_type end;
                
                // The block before and after the current one.
                Block* prev;
                Block* next;
                
                // The number of states in the block.
                std::size_t numberOfStates;
                
                // A field that can be used for marking the block.
                bool isMarked;
            };
            
            class Partition {
            public:
                /*!
                 * Creates a partition with one block consisting of all the states.
                 */
                Partition(std::size_t numberOfStates);
                
                /*!
                 * Splits all blocks of the partition such that afterwards all blocks contain only states with the label
                 * or no labeled state at all.
                 */
                void splitLabel(storm::storage::BitVector const& statesWithLabel);
                
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
                
                std::size_t size() const;
                
                void print() const;
            };
            
            void computeBisimulationEquivalenceClasses(storm::models::Dtmc<ValueType> const& model, bool weak);
            
            std::size_t splitPartition(storm::storage::SparseMatrix<ValueType> const& backwardTransitions, Block const& splitter, Partition& partition, std::deque<Block*>& splitterQueue);
        };
    }
}

#endif /* STORM_STORAGE_BISIMULATIONDECOMPOSITION2_H_ */