#ifndef STORM_STORAGE_FLEXIBLESPARSEMATRIX_H_
#define STORM_STORAGE_FLEXIBLESPARSEMATRIX_H_

#include <cstdint>
#include <vector>

#include "src/storage/sparse/StateType.h"

namespace storm {
    namespace storage {
        template <typename IndexType, typename ValueType>
        class MatrixEntry;
        
        class BitVector;
        
        template<typename ValueType>
        class FlexibleSparseMatrix {
        public:
            // TODO: make this class a bit more consistent with the big sparse matrix and improve it:
            // * rename getNumberOfRows -> getRowCount
            // * store number of columns to also provide getColumnCount
            // * rename hasSelfLoop -> rowHasDiagonalElement
            // * add output iterator and improve the way the matrix is printed
            // * add conversion functionality from/to sparse matrix
            // * add documentation
            // * rename filter to something more appropriate (getSubmatrix?)
            // * add stuff like clearRow, multiplyRowWithScalar
            
            typedef uint_fast64_t index_type;
            typedef ValueType value_type;
            typedef std::vector<storm::storage::MatrixEntry<index_type, value_type>> row_type;
            typedef typename row_type::iterator iterator;
            typedef typename row_type::const_iterator const_iterator;
            
            FlexibleSparseMatrix() = default;
            FlexibleSparseMatrix(index_type rows);
            
            void reserveInRow(index_type row, index_type numberOfElements);
            
            row_type& getRow(index_type);
            row_type const& getRow(index_type) const;
            
            index_type getNumberOfRows() const;
            
            void print() const;
            
            bool empty() const;
            
            void filter(storm::storage::BitVector const& rowFilter, storm::storage::BitVector const& columnFilter);
            
            /*!
             * Checks whether the given state has a self-loop with an arbitrary probability in the probability matrix.
             *
             * @param state The state for which to check whether it possesses a self-loop.
             * @return True iff the given state has a self-loop with an arbitrary probability in the probability matrix.
             */
            bool hasSelfLoop(storm::storage::sparse::state_type state);
            
        private:
            std::vector<row_type> data;
        };
    }
}

#endif /* STORM_STORAGE_FLEXIBLESPARSEMATRIX_H_ */