#ifndef STORM_STORAGE_DECOMPOSITION_H_
#define STORM_STORAGE_DECOMPOSITION_H_

#include <vector>
#include <boost/container/flat_set.hpp>

namespace storm {
    namespace storage {
        typedef boost::container::flat_set<uint_fast64_t> StateBlock;
        
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
            
            /*
             * Creates an empty SCC decomposition.
             */
            Decomposition();
            
            Decomposition(Decomposition const& other);
            
            Decomposition& operator=(Decomposition const& other);
            
            Decomposition(Decomposition&& other);
            
            Decomposition& operator=(Decomposition&& other);
            
            size_t size() const;
            
            iterator begin();
            
            iterator end();
            
            const_iterator begin() const;
            
            const_iterator end() const;
            
            Block const& getBlock(uint_fast64_t index) const;
            
            Block const& operator[](uint_fast64_t index) const;
            
            template<typename BlockTimePrime>
            friend std::ostream& operator<<(std::ostream& out, Decomposition<BlockTimePrime> const& decomposition);

        protected:
            // The blocks of the decomposition.
            std::vector<Block> blocks;
        };
    }
}

#endif /* STORM_STORAGE_DECOMPOSITION_H_ */
