#ifndef STORM_STORAGE_FLEXIBLESPARSEMATRIX_H_
#define STORM_STORAGE_FLEXIBLESPARSEMATRIX_H_

#include <cstdint>
#include <vector>

#include "src/storage/sparse/StateType.h"
#include "src/storage/SparseMatrix.h"

namespace storm {
    namespace storage {
        template <typename IndexType, typename ValueType>
        class MatrixEntry;
        
        class BitVector;
        
        /*!
         * The flexible sparse matrix is used during state elimination.
         */
        template<typename ValueType>
        class FlexibleSparseMatrix {
        public:
            // TODO: make this class a bit more consistent with the big sparse matrix and improve it:
            // * add output iterator and improve the way the matrix is printed
            // * add stuff like clearRow, multiplyRowWithScalar
            
            typedef uint_fast64_t index_type;
            typedef ValueType value_type;
            typedef std::vector<storm::storage::MatrixEntry<index_type, value_type>> row_type;
            typedef typename row_type::iterator iterator;
            typedef typename row_type::const_iterator const_iterator;
            
            /*!
             * Constructs an empty flexible sparse matrix.
             */
            FlexibleSparseMatrix() = default;

            /*!
             * Constructs a flexible sparse matrix with rows many rows.
             * @param rows number of rows.
             */
            FlexibleSparseMatrix(index_type rows);

            /*!
             * Constructs a flexible sparse matrix from a sparse matrix.
             * @param matrix Sparse matrix to construct from.
             * @param setAllValuesToOne If true, all set entries are set to one. Default is false.
             */
            FlexibleSparseMatrix(storm::storage::SparseMatrix<ValueType> const& matrix, bool setAllValuesToOne = false);

            /*!
             * Reserves space for elements in row.
             * @param row Row to reserve in.
             * @param numberOfElements Number of elements to reserve space for.
             */
            void reserveInRow(index_type row, index_type numberOfElements);

            /*!
             * Returns an object representing the given row.
             *
             * @param row The row to get.
             * @return An object representing the given row.
             */
            row_type& getRow(index_type);

            /*!
             * Returns an object representing the given row.
             *
             * @param row The row to get.
             * @return An object representing the given row.
             */
            row_type const& getRow(index_type) const;

            /*!
             * Returns the number of rows of the matrix.
             *
             * @return The number of rows of the matrix.
             */
            index_type getRowCount() const;

            /*!
             * Returns the number of columns of the matrix.
             *
             * @return The number of columns of the matrix.
             */
            index_type getColumnCount() const;

            /*!
             * Returns the cached number of nonzero entries in the matrix.
             *
             * @return The number of nonzero entries in the matrix.
             */
            index_type getNonzeroEntryCount() const;

            /*!
             * Recomputes the number of columns and the number of non-zero entries.
             */
            void updateDimensions();

            /*!
             * Checks if the matrix has no elements.
             * @return True, if the matrix is empty.
             */
            bool empty() const;

            /*!
             * Creates a submatrix of the current matrix in place by dropping all rows and columns whose bits are not
             * set to one in the given bit vector.
             *
             * @param rowConstraint A bit vector indicating which rows to keep.
             * @param columnConstraint A bit vector indicating which columns to keep.
             */
            void createSubmatrix(storm::storage::BitVector const& rowConstraint, storm::storage::BitVector const& columnConstraint);

            /*!
             * Creates a sparse matrix from the flexible sparse matrix.
             * @return The sparse matrix.
             */
            storm::storage::SparseMatrix<ValueType> createSparseMatrix();

            /*!
             * Checks whether the given state has a self-loop with an arbitrary probability in the probability matrix.
             *
             * @param state The state for which to check whether it possesses a self-loop.
             * @return True iff the given state has a self-loop with an arbitrary probability in the probability matrix.
             */
            bool rowHasDiagonalElement(storm::storage::sparse::state_type state);

            template<typename TPrime>
            friend std::ostream& operator<<(std::ostream& out, FlexibleSparseMatrix<TPrime> const& matrix);

        private:
            std::vector<row_type> data;

            // The number of columns of the matrix.
            index_type columnCount;

            // The number of entries in the matrix.
            index_type nonzeroEntryCount;
        };
    }
}

#endif /* STORM_STORAGE_FLEXIBLESPARSEMATRIX_H_ */