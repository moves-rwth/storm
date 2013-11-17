#ifndef STORM_STORAGE_DECOMPOSITION_H_
#define STORM_STORAGE_DECOMPOSITION_H_

#include <vector>

#include "src/storage/VectorSet.h"

namespace storm {
    namespace storage {
        
        /*!
         * This class represents the decomposition of a model into state sets.
         */
        class Decomposition {
        public:
            typedef storm::storage::VectorSet<uint_fast64_t> Block;
            typedef std::vector<Block>::iterator iterator;
            typedef std::vector<Block>::const_iterator const_iterator;
            
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
            
        protected:
            // The blocks of the decomposition.
            std::vector<Block> blocks;
        };
    }
}

#endif /* STORM_STORAGE_DECOMPOSITION_H_ */
