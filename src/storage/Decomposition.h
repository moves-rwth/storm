#ifndef STORM_STORAGE_DECOMPOSITION_H_
#define STORM_STORAGE_DECOMPOSITION_H_

#include <vector>
#include <boost/container/flat_set.hpp>

namespace storm {
    namespace storage {
        // A typedef that specifies the type of a block consisting of states only.
        typedef boost::container::flat_set<uint_fast64_t> StateBlock;

        /*!
         * Writes a string representation of the state block to the given output stream.
         *
         * @param out The output stream to write to.
         * @param block The block to print to the stream.
         * @return The given output stream.
         */
        std::ostream& operator<<(std::ostream& out, StateBlock const& block);
        
        /*!
         * This class represents the decomposition of a model into blocks which are of the template type.
         */
        template <typename BlockType>
        class Decomposition {
        public:
            typedef BlockType Block;
            typedef typename std::vector<Block>::iterator iterator;
            typedef typename std::vector<Block>::const_iterator const_iterator;
            
            /*!
             * Creates an empty decomposition.
             */
            Decomposition();
            
            /*!
             * Creates a decomposition by copying the given decomposition.
             *
             * @param other The decomposition to copy.
             */
            Decomposition(Decomposition const& other);
            
            /*!
             * Assigns the contents of the given decomposition to the current one by copying the contents.
             *
             * @param other The decomposition whose values to copy.
             * @return The current decomposition.
             */
            Decomposition& operator=(Decomposition const& other);
            
            /*!
             * Creates a decomposition by moving the given decomposition.
             *
             * @param other The decomposition to move.
             */
            Decomposition(Decomposition&& other);
            
            /*!
             * Assigns the contents of the given decomposition to the current one by moving the contents.
             *
             * @param other The decomposition whose values to move.
             * @return The current decomposition.
             */
            Decomposition& operator=(Decomposition&& other);
            
            /*!
             * Retrieves the number of blocks of this decomposition.
             *
             * @return The number of blocks of this decomposition.
             */
            size_t size() const;
            
            /*!
             * Retrieves an iterator that points to the first block of this decomposition.
             *
             * @return An iterator that points to the first block of this decomposition.
             */
            iterator begin();
            
            /*!
             * Retrieves an iterator that points past the last block of this decomposition.
             *
             * @return An iterator that points past the last block of this decomposition.
             */
            iterator end();
            
            /*!
             * Retrieves a const iterator that points to the first block of this decomposition.
             *
             * @return A const iterator that points to the first block of this decomposition.
             */
            const_iterator begin() const;
            
            /*!
             * Retrieves a const iterator that points past the last block of this decomposition.
             *
             * @return A const iterator that points past the last block of this decomposition.
             */
            const_iterator end() const;
            
            /*!
             * Retrieves the block with the given index. If the index is out-of-bounds, an exception is thrown.
             *
             * @param index The index of the block to retrieve.
             * @return The block with the given index.
             */
            Block const& getBlock(uint_fast64_t index) const;
            
            /*!
             * Retrieves the block with the given index. If the index is out-of-bounds, an exception is thrown.
             *
             * @param index The index of the block to retrieve.
             * @return The block with the given index.
             */
            Block& getBlock(uint_fast64_t index);
            
            /*!
             * Retrieves the block with the given index. If the index is out-of-bounds, an behaviour is undefined.
             *
             * @param index The index of the block to retrieve.
             * @return The block with the given index.
             */
            Block const& operator[](uint_fast64_t index) const;
            
            /*!
             * Retrieves the block with the given index. If the index is out-of-bounds, an behaviour is undefined.
             *
             * @param index The index of the block to retrieve.
             * @return The block with the given index.
             */
            Block& operator[](uint_fast64_t index);

            // Declare the streaming operator as a friend function to enable output of decompositions.
            template<typename BlockTimePrime>
            friend std::ostream& operator<<(std::ostream& out, Decomposition<BlockTimePrime> const& decomposition);
        protected:
            // The blocks of the decomposition.
            std::vector<Block> blocks;
        };
    }
}

#endif /* STORM_STORAGE_DECOMPOSITION_H_ */
