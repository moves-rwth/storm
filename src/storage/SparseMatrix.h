#ifndef STORM_STORAGE_SPARSEMATRIX_H_
#define STORM_STORAGE_SPARSEMATRIX_H_

#include <algorithm>
#include <iostream>
#include <cstdint>
#include <iterator>

#include "src/storage/BitVector.h"
#include "src/utility/constants.h"
#include "src/utility/OsDetection.h"

#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/OutOfRangeException.h"

// Forward declaration for adapter classes.
namespace storm {
	namespace adapters {
		class GmmxxAdapter;
		class EigenAdapter;
		class StormAdapter;
	}
}

namespace storm {
    namespace storage {
        
        // Forward declare matrix class.
        template<typename T> class SparseMatrix;
        
        template<typename T>
        class MatrixEntry {
        public:
            /*!
             * Constructs a matrix entry with the given column and value.
             *
             * @param column The column of the matrix entry.
             * @param value The value of the matrix entry.
             */
            MatrixEntry(uint_fast64_t column, T value);
            
            /*!
             * Move-constructs the matrix entry fro the given column-value pair.
             *
             * @param pair The column-value pair from which to move-construct the matrix entry.
             */
            MatrixEntry(std::pair<uint_fast64_t, T>&& pair);
            
            MatrixEntry() = default;
            MatrixEntry(MatrixEntry const& other) = default;
            MatrixEntry& operator=(MatrixEntry const& other) = default;
#ifndef WINDOWS
            MatrixEntry(MatrixEntry&& other) = default;
            MatrixEntry& operator=(MatrixEntry&& other) = default;
#endif
            
            /*!
             * Retrieves the column of the matrix entry.
             *
             * @return The column of the matrix entry.
             */
            uint_fast64_t const& getColumn() const;
            
            /*!
             * Retrieves the column of the matrix entry.
             *
             * @return The column of the matrix entry.
             */
            uint_fast64_t& getColumn();
            
            /*!
             * Retrieves the value of the matrix entry.
             *
             * @return The value of the matrix entry.
             */
            T const& getValue() const;

            /*!
             * Retrieves the value of the matrix entry.
             *
             * @return The value of the matrix entry.
             */
            T& getValue();
            
            /*!
             * Retrieves a pair of column and value that characterizes this entry.
             *
             * @return A column-value pair that characterizes this entry.
             */
            std::pair<uint_fast64_t, T> const& getColumnValuePair() const;
            
        private:
            // The actual matrix entry.
            std::pair<uint_fast64_t, T> entry;
        };
        
        /*!
         * Computes the hash value of a matrix entry.
         */
        template<typename T>
        std::size_t hash_value(MatrixEntry<T> const& matrixEntry) {
            std::size_t seed = 0;
            boost::hash_combine(seed, matrixEntry.getColumn());
            boost::hash_combine(seed, matrixEntry.getValue());
            return seed;
        }
        
        /*!
         * A class that can be used to build a sparse matrix by adding value by value.
         */
        template<typename T>
        class SparseMatrixBuilder {
        public:
            /*!
             * Constructs a sparse matrix builder producing a matrix with the given number of rows, columns and entries.
             *
             * @param rows The number of rows of the resulting matrix.
             * @param columns The number of columns of the resulting matrix.
             * @param entries The number of entries of the resulting matrix.
             * @param hasCustomRowGrouping A flag indicating whether the builder is used to create a non-canonical
             * grouping of rows for this matrix.
             * @param rowGroups The number of row groups of the resulting matrix. This is only relevant if the matrix
             * has a custom row grouping.
             */
            SparseMatrixBuilder(uint_fast64_t rows = 0, uint_fast64_t columns = 0, uint_fast64_t entries = 0, bool hasCustomRowGrouping = false, uint_fast64_t rowGroups = 0);
            
            /*!
             * Sets the matrix entry at the given row and column to the given value. After all entries have been added,
             * a call to finalize(false) is mandatory.
             *
             * Note: this is a linear setter. That is, it must be called consecutively for each entry, row by row and
             * column by column. As multiple entries per column are admitted, consecutive calls to this method are
             * admitted to mention the same row-column-pair. If rows are skipped entirely, the corresponding rows are
             * treated as empty. If these constraints are not met, an exception is thrown.
             *
             * @param row The row in which the matrix entry is to be set.
             * @param column The column in which the matrix entry is to be set.
             * @param value The value that is to be set at the specified row and column.
             */
            void addNextValue(uint_fast64_t row, uint_fast64_t column, T const& value);
            
            /*!
             * Starts a new row group in the matrix. Note that this needs to be called before any entries in the new row
             * group are added.
             *
             * @param startingRow The starting row of the new row group.
             */
            void newRowGroup(uint_fast64_t startingRow);
            
            /*
             * Finalizes the sparse matrix to indicate that initialization process has been completed and the matrix
             * may now be used. This must be called after all entries have been added to the matrix via addNextValue.
             *
             * @param overriddenRowCount If this is set to a value that is greater than the current number of rows,
             * this will cause the finalize method to add empty rows at the end of the matrix until the given row count
             * has been matched. Note that this will *not* override the row count that has been given upon construction
             * (if any), but will only take effect if the matrix has been created without the number of rows given.
             * @param overriddenColumnCount If this is set to a value that is greater than the current number of columns,
             * this will cause the finalize method to set the number of columns to the given value. Note that this will
             * *not* override the column count that has been given upon construction (if any), but will only take effect
             * if the matrix has been created without the number of columns given. By construction, the matrix will have
             * no entries in the columns that have been added this way.
             * @param overriddenRowGroupCount If this is set to a value that is greater than the current number of row
             * groups, this will cause the method to set the number of row groups to the given value. Note that this will
             * *not* override the row group count that has been given upon construction (if any), but will only take
             * effect if the matrix has been created without the number of row groups given. By construction, the row
             * groups added this way will be empty.
             */
            SparseMatrix<T> build(uint_fast64_t overriddenRowCount = 0, uint_fast64_t overriddenColumnCount = 0, uint_fast64_t overriddenRowGroupCount = 0);
            
        private:
            /*!
             * Prepares the internal storage of the builder. This relies on the number of entries and the number of rows
             * being set correctly. They may, however, be zero, in which case the insertion of elements in the builder
             * will cause occasional reallocations.
             */
            void prepareInternalStorage();

            // A flag indicating whether the number of rows was set upon construction.
            bool rowCountSet;
            
            // The number of rows of the matrix.
            uint_fast64_t rowCount;
            
            // A flag indicating whether the number of columns was set upon construction.
            bool columnCountSet;
            
            // The number of columns of the matrix.
            uint_fast64_t columnCount;
            
            // The number of entries in the matrix.
            uint_fast64_t entryCount;
            
            // A flag indicating whether the builder is to construct a custom row grouping for the matrix.
            bool hasCustomRowGrouping;
            
            // A flag indicating whether the number of row groups was set upon construction.
            bool rowGroupCountSet;
            
            // The number of row groups in the matrix.
            uint_fast64_t rowGroupCount;
            
            std::vector<uint_fast64_t> rowGroupIndices;
            
            // Stores whether the storage of the matrix was preallocated or not.
            bool storagePreallocated;
            
            // The storage for the columns and values of all entries in the matrix.
            std::vector<MatrixEntry<T>> columnsAndValues;
            
            // A vector containing the indices at which each given row begins. This index is to be interpreted as an
            // index in the valueStorage and the columnIndications vectors. Put differently, the values of the entries
            // in row i are valueStorage[rowIndications[i]] to valueStorage[rowIndications[i + 1]] where the last
            // entry is not included anymore.
            std::vector<uint_fast64_t> rowIndications;
            
            // Stores the current number of entries in the matrix. This is used for inserting an entry into a matrix
            // with preallocated storage.
            uint_fast64_t currentEntryCount;
            
            // Stores the row of the last entry in the matrix. This is used for correctly inserting an entry into a
            // matrix.
            uint_fast64_t lastRow;
            
            // Stores the column of the currently last entry in the matrix. This is used for correctly inserting an
            // entry into a matrix.
            uint_fast64_t lastColumn;
            
            // Stores the currently active row group. This is used for correctly constructing the row grouping of the
            // matrix.
            uint_fast64_t currentRowGroup;
        };
        
        /*!
         * A class that holds a possibly non-square matrix in the compressed row storage format. That is, it is supposed
         * to store non-zero entries only, but zeros may be explicitly stored if necessary for certain operations.
         * Likewise, the matrix is intended to store one value per column only. However, the functions provided by the
         * matrix are implemented in a way that makes it safe to store several entries per column.
         *
         * The creation of a matrix can be done in several ways. If the number of rows, columns and entries is known
         * prior to creating the matrix, the matrix can be constructed using this knowledge, which saves reallocations.
         * On the other hand, if either one of these values is not known a priori, the matrix can be constructed as an
         * empty matrix that needs to perform reallocations as more and more entries are inserted in the matrix.
         *
         * It should be observed that due to the nature of the sparse matrix format, entries can only be inserted in
         * order, i.e. row by row and column by column.
         */
        template<typename T>
        class SparseMatrix {
        public:
            // Declare adapter classes as friends to use internal data.
            friend class storm::adapters::GmmxxAdapter;
            friend class storm::adapters::EigenAdapter;
            friend class storm::adapters::StormAdapter;
            
            typedef typename std::vector<MatrixEntry<T>>::iterator iterator;
            typedef typename std::vector<MatrixEntry<T>>::const_iterator const_iterator;
            
            /*!
             * This class represents a number of consecutive rows of the matrix.
             */
            class rows {
            public:
                /*!
                 * Constructs an object that represents the rows defined by the value of the first entry, the column
                 * of the first entry and the number of entries in this row set.
                 *
                 * @param begin An iterator that points to the beginning of the row.
                 * @param entryCount The number of entrys in the rows.
                 */
                rows(iterator begin, uint_fast64_t entryCount);
                
                /*!
                 * Retrieves an iterator that points to the beginning of the rows.
                 *
                 * @return An iterator that points to the beginning of the rows.
                 */
                iterator begin();
                
                /*!
                 * Retrieves an iterator that points past the last entry of the rows.
                 *
                 * @return An iterator that points past the last entry of the rows.
                 */
                iterator end();
                
            private:
                // The pointer to the columnd and value of the first entry.
                iterator beginIterator;
                
                // The number of non-zero entries in the rows.
                uint_fast64_t entryCount;
            };

            /*!
             * This class represents a number of consecutive rows of the matrix.
             */
            class const_rows {
            public:
                /*!
                 * Constructs an object that represents the rows defined by the value of the first entry, the column
                 * of the first entry and the number of entries in this row set.
                 *
                 * @param begin An iterator that points to the beginning of the row.
                 * @param entryCount The number of entrys in the rows.
                 */
                const_rows(const_iterator begin, uint_fast64_t entryCount);
                
                /*!
                 * Retrieves an iterator that points to the beginning of the rows.
                 *
                 * @return An iterator that points to the beginning of the rows.
                 */
                const_iterator begin() const;
                
                /*!
                 * Retrieves an iterator that points past the last entry of the rows.
                 *
                 * @return An iterator that points past the last entry of the rows.
                 */
                const_iterator end() const;
                
            private:
                // The pointer to the columnd and value of the first entry.
                const_iterator beginIterator;
                
                // The number of non-zero entries in the rows.
                uint_fast64_t entryCount;
            };
            
            /*!
             * An enum representing the internal state of the matrix. After creation, the matrix is UNINITIALIZED.
             * Only after a call to finalize(), the status of the matrix is set to INITIALIZED and the matrix can be
             * used.
             */
            enum MatrixStatus { UNINITIALIZED, INITIALIZED };
            
            /*!
             * Constructs an empty sparse matrix.
             */
            SparseMatrix();
            
            /*!
             * Constructs a sparse matrix by performing a deep-copy of the given matrix.
             *
             * @param other The matrix from which to copy the content.
             */
            SparseMatrix(SparseMatrix<T> const& other);
            
            /*!
             * Constructs a sparse matrix by moving the contents of the given matrix to the newly created one.
             *
             * @param other The matrix from which to move the content.
             */
            SparseMatrix(SparseMatrix<T>&& other);
            
            /*!
             * Constructs a sparse matrix by copying the given contents.
             *
             * @param columnCount The number of columns of the matrix.
             * @param rowIndications The row indications vector of the matrix to be constructed.
             * @param columnsAndValues The vector containing the columns and values of the entries in the matrix.
             * @param rowGroupIndices The vector representing the row groups in the matrix.
             */
            SparseMatrix(uint_fast64_t columnCount, std::vector<uint_fast64_t> const& rowIndications, std::vector<MatrixEntry<T>> const& columnsAndValues, std::vector<uint_fast64_t> const& rowGroupIndices);
            
            /*!
             * Constructs a sparse matrix by moving the given contents.
             *
             * @param columnCount The number of columns of the matrix.
             * @param rowIndications The row indications vector of the matrix to be constructed.
             * @param columnsAndValues The vector containing the columns and values of the entries in the matrix.
             * @param rowGroupIndices The vector representing the row groups in the matrix.
             */
            SparseMatrix(uint_fast64_t columnCount, std::vector<uint_fast64_t>&& rowIndications, std::vector<MatrixEntry<T>>&& columnsAndValues, std::vector<uint_fast64_t>&& rowGroupIndices);

            /*!
             * Assigns the contents of the given matrix to the current one by deep-copying its contents.
             *
             * @param other The matrix from which to copy-assign.
             */
            SparseMatrix<T>& operator=(SparseMatrix<T> const& other);
            
            /*!
             * Assigns the contents of the given matrix to the current one by moving its contents.
             *
             * @param other The matrix from which to move to contents.
             */
            SparseMatrix<T>& operator=(SparseMatrix<T>&& other);
            
            /*!
             * Determines whether the current and the given matrix are semantically equal.
             *
             * @param other The matrix with which to compare the current matrix.
             * @return True iff the given matrix is semantically equal to the current one.
             */
            bool operator==(SparseMatrix<T> const& other) const;
            
            /*!
             * Returns the number of rows of the matrix.
             *
             * @return The number of rows of the matrix.
             */
            uint_fast64_t getRowCount() const;
            
            /*!
             * Returns the number of columns of the matrix.
             *
             * @return The number of columns of the matrix.
             */
            uint_fast64_t getColumnCount() const;
            
            /*!
             * Returns the number of entries in the matrix.
             *
             * @return The number of entries in the matrix.
             */
            uint_fast64_t getEntryCount() const;
            
            /*!
             * Returns the number of nonzero entries in the matrix.
             *
             * @return The number of nonzero entries in the matrix.
             */
            uint_fast64_t getNonzeroEntryCount() const;
            
            /*!
             * Returns the number of row groups in the matrix.
             *
             * @return The number of row groups in the matrix.
             */
            uint_fast64_t getRowGroupCount() const;
            
            /*!
             * Returns the size of the given row group.
             *
             * @param group The group whose size to retrieve.
             * @return The number of rows that belong to the given row group.
             */
            uint_fast64_t getRowGroupSize(uint_fast64_t group) const;
            
            /*!
             * Returns the grouping of rows of this matrix.
             *
             * @return The grouping of rows of this matrix.
             */
            std::vector<uint_fast64_t> const& getRowGroupIndices() const;
            
            /*!
             * This function makes the given rows absorbing.
             *
             * @param rows A bit vector indicating which rows are to be made absorbing.
             */
            void makeRowsAbsorbing(storm::storage::BitVector const& rows);
            
            /*!
             * This function makes the groups of rows given by the bit vector absorbing.
             *
             * @param rowGroupConstraint A bit vector indicating which row groups to make absorbing.
             */
            void makeRowGroupsAbsorbing(storm::storage::BitVector const& rowGroupConstraint);
            
            /*!
             * This function makes the given row Dirac. This means that all entries will be set to 0 except the one
             * at the specified column, which is set to 1 instead.
             *
             * @param row The row to be made Dirac.
             * @param column The index of the column whose value is to be set to 1.
             */
            void makeRowDirac(const uint_fast64_t row, const uint_fast64_t column);
            
            /*
             * Sums the entries in the given row and columns.
             *
             * @param row The row whose entries to add.
             * @param columns A bit vector that indicates which columns to add.
             * @return The sum of the entries in the given row and columns.
             */
            T getConstrainedRowSum(uint_fast64_t row, storm::storage::BitVector const& columns) const;
            
            /*!
             * Computes a vector whose i-th entry is the sum of the entries in the i-th selected row where only those
             * entries are added that are in selected columns.
             *
             * @param rowConstraint A bit vector that indicates for which rows to compute the constrained sum.
             * @param columnConstraint A bit vector that indicates which columns to add in the selected rows.
             *
             * @return A vector whose elements are the sums of the selected columns in each row.
             */
            std::vector<T> getConstrainedRowSumVector(storm::storage::BitVector const& rowConstraint, storm::storage::BitVector const& columnConstraint) const;
            
            /*!
             * Computes a vector whose entries represent the sums of selected columns for all rows in selected row
             * groups.
             *
             * @param rowGroupConstraint A bit vector that indicates which row groups are to be considered.
             * @param columnConstraint A bit vector that indicates which columns to sum.
             * @return A vector whose entries represent the sums of selected columns for all rows in selected row
             * groups.
             */
            std::vector<T> getConstrainedRowGroupSumVector(storm::storage::BitVector const& rowGroupConstraint, storm::storage::BitVector const& columnConstraint) const;
            
            /*!
             * Creates a submatrix of the current matrix by dropping all rows and columns whose bits are not
             * set to one in the given bit vector.
             *
             * @param useGroups If set to true, the constraint for the rows is interpreted as selecting whole row groups.
             * @param constraint A bit vector indicating which rows to keep.
             * @param columnConstraint A bit vector indicating which columns to keep.
             * @param insertDiagonalEntries If set to true, the resulting matrix will have zero entries in column i for
             * each row i, if there is no value yet. This can then be used for inserting other values later.
             * @return A matrix corresponding to a submatrix of the current matrix in which only rows and columns given
             * by the constraints are kept and all others are dropped.
             */
            SparseMatrix getSubmatrix(bool useGroups, storm::storage::BitVector const& rowConstraint, storm::storage::BitVector const& columnConstraint, bool insertDiagonalEntries = false) const;
            
            /*!
             * Selects exactly one row from each row group of this matrix and returns the resulting matrix.
             *
             * @param rowGroupToRowIndexMapping A mapping from each row group index to a selected row in this group.
             * @param insertDiagonalEntries If set to true, the resulting matrix will have zero entries in column i for
             * each row in row group i. This can then be used for inserting other values later.
             * @return A submatrix of the current matrix by selecting one row out of each row group.
             */
            SparseMatrix selectRowsFromRowGroups(std::vector<uint_fast64_t> const& rowGroupToRowIndexMapping, bool insertDiagonalEntries = true) const;
            
            /*!
             * Transposes the matrix.
             *
             * @param joinGroups A flag indicating whether the row groups are supposed to be treated as single rows.
             *
             * @return A sparse matrix that represents the transpose of this matrix.
             */
            storm::storage::SparseMatrix<T> transpose(bool joinGroups = false) const;
            
            /*!
             * Transforms the matrix into an equation system. That is, it transforms the matrix A into a matrix (1-A).
             */
            void convertToEquationSystem();

            /*!
             * Inverts all entries on the diagonal, i.e. sets the diagonal values to one minus their previous value.
             * Requires the matrix to contain each diagonal entry and to be square.
             */
            void invertDiagonal();
            
            /*!
             * Negates (w.r.t. addition) all entries that are not on the diagonal.
             */
            void negateAllNonDiagonalEntries();
            
            /*!
             * Sets all diagonal elements to zero.
             */
            void deleteDiagonalEntries();
            
            /*!
             * Calculates the Jacobi decomposition of this sparse matrix. For this operation, the matrix must be square.
             *
             * @return A pair (L+U, D^-1) containing the matrix L+U and the inverted diagonal matrix D^-1.
             */
            std::pair<storm::storage::SparseMatrix<T>, storm::storage::SparseMatrix<T>> getJacobiDecomposition() const;
            
            /*!
             * Performs a pointwise matrix multiplication of the matrix with the given matrix and returns a vector
             * containing the sum of the entries in each row of the resulting matrix.
             *
             * @param otherMatrix A reference to the matrix with which to perform the pointwise multiplication. This
             * matrix must be a submatrix of the current matrix in the sense that it may not have entries at indices
             * where there is no entry in the current matrix.
             * @return A vector containing the sum of the entries in each row of the matrix resulting from pointwise
             * multiplication of the current matrix with the given matrix.
             */
            std::vector<T> getPointwiseProductRowSumVector(storm::storage::SparseMatrix<T> const& otherMatrix) const;
            
            /*!
             * Multiplies the matrix with the given vector and writes the result to given result vector.
             *
             * @param vector The vector with which to multiply the matrix.
             * @param result The vector that is supposed to hold the result of the multiplication after the operation.
             * @return The product of the matrix and the given vector as the content of the given result vector.
             */
            void multiplyWithVector(std::vector<T> const& vector, std::vector<T>& result) const;
            
            /*!
             * Computes the sum of the entries in a given row.
             *
             * @param row The row that is to be summed.
             * @return The sum of the selected row.
             */
            T getRowSum(uint_fast64_t row) const;
            
            /*!
             * Checks if the current matrix is a submatrix of the given matrix, where a matrix A is called a submatrix
             * of B if B has no entries in position where A has none. Additionally, the matrices must be of equal size.
             *
             * @param matrix The matrix that possibly is a supermatrix of the current matrix.
             * @return True iff the current matrix is a submatrix of the given matrix.
             */
            bool isSubmatrixOf(SparseMatrix<T> const& matrix) const;
            
            template<typename TPrime>
            friend std::ostream& operator<<(std::ostream& out, SparseMatrix<TPrime> const& matrix);
            
            /*!
             * Returns the size of the matrix in memory measured in bytes.
             *
             * @return The size of the matrix in memory measured in bytes.
             */
            uint_fast64_t getSizeInMemory() const;
            
            /*!
             * Calculates a hash value over all values contained in the matrix.
             *
             * @return size_t A hash value for this matrix.
             */
            std::size_t hash() const;
            
            /*!
             * Returns an object representing the consecutive rows given by the parameters.
             *
             * @param startRow The starting row.
             * @param endRow The ending row (which is included in the result).
             * @return An object representing the consecutive rows given by the parameters.
             */
            const_rows getRows(uint_fast64_t startRow, uint_fast64_t endRow) const;

            /*!
             * Returns an object representing the consecutive rows given by the parameters.
             *
             * @param startRow The starting row.
             * @param endRow The ending row (which is included in the result).
             * @return An object representing the consecutive rows given by the parameters.
             */
            rows getRows(uint_fast64_t startRow, uint_fast64_t endRow);

            /*!
             * Returns an object representing the given row.
             *
             * @param row The row to get.
             * @return An object representing the given row.
             */
            const_rows getRow(uint_fast64_t row) const;

            /*!
             * Returns an object representing the given row.
             *
             * @param row The row to get.
             * @return An object representing the given row.
             */
            rows getRow(uint_fast64_t row);
            
            /*!
             * Returns an object representing the given row group.
             *
             * @param rowGroup The row group to get.
             * @return An object representing the given row group.
             */
            const_rows getRowGroup(uint_fast64_t rowGroup) const;
            
            /*!
             * Returns an object representing the given row group.
             *
             * @param rowGroup The row group to get.
             * @return An object representing the given row group.
             */
            rows getRowGroup(uint_fast64_t rowGroup);
            
            /*!
             * Retrieves an iterator that points to the beginning of the given row.
             *
             * @param row The row to the beginning of which the iterator has to point.
             * @return An iterator that points to the beginning of the given row.
             */
            const_iterator begin(uint_fast64_t row = 0) const;
            
            /*!
             * Retrieves an iterator that points to the beginning of the given row.
             *
             * @param row The row to the beginning of which the iterator has to point.
             * @return An iterator that points to the beginning of the given row.
             */
            iterator begin(uint_fast64_t row = 0);
            
            /*!
             * Retrieves an iterator that points past the end of the given row.
             *
             * @param row The row past the end of which the iterator has to point.
             * @return An iterator that points past the end of the given row.
             */
            const_iterator end(uint_fast64_t row) const;

            /*!
             * Retrieves an iterator that points past the end of the given row.
             *
             * @param row The row past the end of which the iterator has to point.
             * @return An iterator that points past the end of the given row.
             */
            iterator end(uint_fast64_t row);

            /*!
             * Retrieves an iterator that points past the end of the last row of the matrix.
             *
             * @return An iterator that points past the end of the last row of the matrix.
             */
            const_iterator end() const;
            
            /*!
             * Retrieves an iterator that points past the end of the last row of the matrix.
             *
             * @return An iterator that points past the end of the last row of the matrix.
             */
            iterator end();
            
        private:
            /*!
             * Creates a submatrix of the current matrix by keeping only row groups and columns in the given row group
             * and column constraint, respectively.
             *
             * @param rowGroupConstraint A bit vector indicating which row groups to keep.
             * @param columnConstraint A bit vector indicating which columns to keep.
             * @param rowGroupIndices A vector indicating which rows belong to a given row group.
             * @param insertDiagonalEntries If set to true, the resulting matrix will have zero entries in column i for
             * each row in row group i. This can then be used for inserting other values later.
             * @return A matrix corresponding to a submatrix of the current matrix in which only row groups and columns
             * given by the row group constraint are kept and all others are dropped.
             */
            SparseMatrix getSubmatrix(storm::storage::BitVector const& rowGroupConstraint, storm::storage::BitVector const& columnConstraint, std::vector<uint_fast64_t> const& rowGroupIndices, bool insertDiagonalEntries = false) const;
            
            // The number of rows of the matrix.
            uint_fast64_t rowCount;
            
            // The number of columns of the matrix.
            uint_fast64_t columnCount;
            
            // The number of entries in the matrix.
            uint_fast64_t entryCount;
            
            // The number of nonzero entries in the matrix.
            uint_fast64_t nonzeroEntryCount;
            
            // The storage for the columns and values of all entries in the matrix.
            std::vector<MatrixEntry<T>> columnsAndValues;
            
            // A vector containing the indices at which each given row begins. This index is to be interpreted as an
            // index in the valueStorage and the columnIndications vectors. Put differently, the values of the entries
            // in row i are valueStorage[rowIndications[i]] to valueStorage[rowIndications[i + 1]] where the last
            // entry is not included anymore.
            std::vector<uint_fast64_t> rowIndications;
            
            // A vector indicating the row groups of the matrix.
            std::vector<uint_fast64_t> rowGroupIndices;
        };

    } // namespace storage
} // namespace storm

#endif // STORM_STORAGE_SPARSEMATRIX_H_
