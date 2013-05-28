#ifndef STORM_STORAGE_SPARSEMATRIX_H_
#define STORM_STORAGE_SPARSEMATRIX_H_

#include <exception>
#include <new>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <set>
#include "boost/integer/integer_mask.hpp"

#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/OutOfRangeException.h"
#include "src/exceptions/FileIoException.h"
#include "src/storage/BitVector.h"
#include "src/storage/JacobiDecomposition.h"

#include "src/utility/ConstTemplates.h"
#include "Eigen/Sparse"
#include "gmm/gmm_matrix.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

extern log4cplus::Logger logger;

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

/*!
 * A sparse matrix class with a constant number of non-zero entries.
 * NOTE: Addressing *is* zero-based, so the valid range for getValue and addNextValue is 0..(rows - 1) where rows is the
 * first argument to the constructor.
 */
template<class T>
class SparseMatrix {
public:
	/*!
	 * Declare adapter classes as friends to use internal data.
	 */
	friend class storm::adapters::GmmxxAdapter;
	friend class storm::adapters::EigenAdapter;
	friend class storm::adapters::StormAdapter;

	/*!
	 * If we only want to iterate over the columns or values of the non-zero entries of
	 * a row, we can simply iterate over the array (part) itself.
	 */
	typedef uint_fast64_t const* ConstIndexIterator;
	typedef T const* ConstValueIterator;

	/*!
	 * Iterator class that is able to iterate over the non-zero elements of a matrix and return the position
	 * (row, column) in addition to the value itself. This is a const iterator in the sense that it is not possible to
	 * modify the matrix with it.
	 */
	class ConstRowsIterator {
	public:
		/*!
		 * Constructs an iterator over the elements of the given matrix.
		 *
		 * @param matrix The matrix on which this iterator operates.
		 */
		ConstRowsIterator(SparseMatrix<T> const& matrix, uint_fast64_t row = 0) : matrix(matrix), posIndex(matrix.rowIndications[row]), rowIndex(row) {
			// Intentionally left empty.
		}

		/*!
		 * Moves the iterator the next non-zero element of the matrix.
		 *
		 * @returns A reference to itself.
		 */
		ConstRowsIterator& operator++() {
			++posIndex;
			if (posIndex >= matrix.rowIndications[rowIndex + 1]) {
				++rowIndex;
			}
			return *this;
		}
		
		/*!
		 * Dereferencing operator for this iterator. Actually returns a reference to itself. This is
		 * needed, because the range-based for-loop in C++11 dereferences the iterator automatically.
		 *
		 * @returns A reference to itself.
		 */
		ConstRowsIterator& operator*() {
			return *this;
		}

		/*!
		 * Comparison operator that compares the current iterator with the given one in terms of
         * the indices they point to. Note that this does not check whether the iterators are
         * interpreted over the same matrix.
		 *
		 * @return True iff the given iterator points to the same index as the current iterator.
		 */
		bool operator==(ConstRowsIterator const& other) const {
			return this->posIndex == other.posIndex;
		}

		/*!
		 * Comparison operator that compares the current iterator with the given one in terms of
         * the indices they point to. Note that this does not check whether the iterators are
         * interpreted over the same matrix.
		 *
		 * @return True iff the given iterator points to a differnent index as the current iterator.
		 */
		bool operator!=(ConstRowsIterator const& other) const {
			return this->posIndex != other.posIndex;
		}

        /*!
         * Retrieves the row that is associated with the current non-zero element this iterator
         * points to.
		 *
		 * @returns The row of the current non-zero element this iterator points to.
         */
		uint_fast64_t row() const {
			return this->rowIndex;
		}

        /*!
         * Retrieves the column that is associated with the current non-zero element this iterator
         * points to.
		 *
		 * @returns The column of the current non-zero element this iterator points to.
         */
		uint_fast64_t column() const {
			return matrix.columnIndications[posIndex];
		}
		
		/*!
		 * Retrieves the internal index of this iterator. This index corresponds to the position of
		 * the current element in the vector of non-zero values of the matrix.
		 */
		uint_fast64_t index() const {
			return this->posIndex;
		}
		
        /*!
         * Retrieves the value of the current non-zero element this iterator points to.
		 *
		 * @returns The value of the current non-zero element this iterator points to.
         */
		T const& value() const {
			return matrix.valueStorage[posIndex];
		}

        /*!
         * Moves the iterator to the beginning of the given row.
         *
         * @param row The row this iterator is to be moved to.
         */
		void moveToRow(uint_fast64_t row) {
			this->rowIndex = row;
			this->posIndex = matrix.rowIndications[row];
		}

        /*!
         * Moves the iterator to the beginning of the next row.
         */
		void moveToNextRow() {
			moveToRow(rowIndex + 1);
		}

	private:
        // A constant reference to the matrix this iterator is associated with.
		SparseMatrix<T> const& matrix;
        
        // The current index in the list of all non-zero elements of the matrix this iterator points to.
		uint_fast64_t posIndex;
        
        // The row of the element this iterator currently points to.
		uint_fast64_t rowIndex;
	};

    // Declare the iterator as a friend class to grant access to private data members of the matrix.
	friend class ConstRowsIterator;

	/*!
	 * An enum representing the internal state of the Matrix
	 * After creating the Matrix using the Constructor, the Object is in state UnInitialized. After
	 * calling initialize(), that state changes to Initialized and after all entries have been
	 * entered and finalize() has been called, to ReadReady.
	 * Should a critical error occur in any of the former functions, the state will change to Error.
	 *
	 * @see getState()
	 * @see isReadReady()
	 * @see isInitialized()
	 * @see hasError()
	 */
	enum MatrixStatus {
		Error = -1, UnInitialized = 0, Initialized = 1, ReadReady = 2
	};

	/*!
	 * Constructs a sparse matrix object with the given number of rows.
     *
	 * @param rows The number of rows of the matrix
	 */
	SparseMatrix(uint_fast64_t rows, uint_fast64_t cols) : rowCount(rows), colCount(cols),
		nonZeroEntryCount(0), internalStatus(MatrixStatus::UnInitialized), currentSize(0), lastRow(0) {
        // Intentionally left empty.
    }

	/*!
	 * Constructs a square sparse matrix object with the given number rows.
	 *
	 * @param size The number of rows and columns of the matrix.
	 */
	SparseMatrix(uint_fast64_t size = 0)
			: rowCount(size), colCount(size), nonZeroEntryCount(0),
			  internalStatus(MatrixStatus::UnInitialized), currentSize(0), lastRow(0) {
        // Intentionally left empty.
	}
    
    /*!
     * Constructs a sparse matrix object with the given (moved) contents.
     *
     * @param rowCount The number of rows.
     * @param colCount The number of columns.
     * @param nonZeroEntryCount The number of non-zero entries.
     * @param rowIndications The vector indicating where the rows start.
     * @param columnIndications The vector indicating the column for each non-zero element.
     * @param values The vector containing the non-zero values.
     */
    SparseMatrix(uint_fast64_t rowCount, uint_fast64_t colCount, uint_fast64_t nonZeroEntryCount,
                 std::vector<uint_fast64_t>&& rowIndications,
                 std::vector<uint_fast64_t>&& columnIndications, std::vector<T>&& values)
                    : rowCount(rowCount), colCount(colCount), nonZeroEntryCount(nonZeroEntryCount),
                    valueStorage(values), columnIndications(columnIndications),
                    rowIndications(rowIndications), internalStatus(MatrixStatus::Initialized),
                    currentSize(0), lastRow(0) {
        // Intentionally left empty.
    }

	/*!
	 * Initializes the sparse matrix with the given number of non-zero entries
	 * and prepares it for use with addNextValue() and finalize().
	 * Note: Calling this method before any other member function is mandatory.
	 * This version is to be used together with addNextValue().
	 *
	 * @param nonZeroEntries The number of non-zero entries this matrix is going to hold.
	 */
	void initialize(uint_fast64_t nonZeroEntries = 0) {
		// Check whether initializing the matrix is safe.
		if (internalStatus != MatrixStatus::UnInitialized) {
			triggerErrorState();
			LOG4CPLUS_ERROR(logger, "Trying to initialize matrix that is not uninitialized.");
			throw storm::exceptions::InvalidStateException("Trying to initialize matrix that is not uninitialized.");
		} else if ((rowCount == 0) || (colCount == 0)) {
			triggerErrorState();
			LOG4CPLUS_ERROR(logger, "Trying to create initialize a matrix with 0 rows or 0 columns.");
			throw storm::exceptions::InvalidArgumentException("Trying to create initialize a matrix with 0 rows or 0 columns.");
		} else if ((rowCount * colCount) < nonZeroEntries) {
			triggerErrorState();
			LOG4CPLUS_ERROR(logger, "Trying to initialize a matrix with more non-zero entries than there can be.");
			throw storm::exceptions::InvalidArgumentException("Trying to initialize a matrix with more non-zero entries than there can be.");
		} else {
			// If it is safe, initialize necessary members and prepare the
			// internal storage.
			nonZeroEntryCount = nonZeroEntries;
			lastRow = 0;

			if (!prepareInternalStorage()) {
				triggerErrorState();
				throw std::bad_alloc();
			} else {
				setState(MatrixStatus::Initialized);
			}
		}
	}
    
	/*!
	 * Sets the matrix element at the given row and column to the given value. After all elements have been added,
     * a call to finalize() is mandatory.
	 * NOTE: This is a linear setter. It must be called consecutively for each element,
	 * row by row *and* column by column.
     * NOTE: This method is different from insertNextValue(...) in that the number of nonzero elements must be known
     * in advance (and passed to initialize()), because adding elements will not automatically increase the size of the
     * underlying storage.
	 *
	 * @param row The row in which the matrix element is to be set.
	 * @param col The column in which the matrix element is to be set.
	 * @param value The value that is to be set.
	 */
	void addNextValue(const uint_fast64_t row, const uint_fast64_t col,	T const& value) {
		// Check whether the given row and column positions are valid and throw
		// error otherwise.
		if ((row > rowCount) || (col > colCount)) {
			triggerErrorState();
			LOG4CPLUS_ERROR(logger, "Trying to add a value at illegal position (" << row << ", " << col << ") in matrix of size (" << rowCount << ", " << colCount << ").");
			throw storm::exceptions::OutOfRangeException() << "Trying to add a value at illegal position (" << row << ", " << col << ") in matrix of size (" << rowCount << ", " << colCount << ").";
		}

		// If we switched to another row, we have to adjust the missing
		// entries in the row_indications array.
		if (row != lastRow) {
			for (uint_fast64_t i = lastRow + 1; i <= row; ++i) {
				rowIndications[i] = currentSize;
			}
			lastRow = row;
		}

		// Finally, set the element and increase the current size.
		valueStorage[currentSize] = value;
		columnIndications[currentSize] = col;

		++currentSize;
	}
    
    /*!
     * Inserts a value at the given row and column with the given value. After all elements have been added,
     * a call to finalize() is mandatory.
     * NOTE: This is a linear inserter. It must be called consecutively for each element, row by row *and* column by
     * column.
     * NOTE: This method is different from addNextValue(...) in that the number of nonzero elements need not be known
     * in advance, because inserting elements will automatically increase the size of the underlying storage.
	 *
	 * @param row The row in which the matrix element is to be set.
	 * @param col The column in which the matrix element is to be set.
	 * @param value The value that is to be set.
     */
    void insertNextValue(const uint_fast64_t row, const uint_fast64_t col,	T const& value) {
		// Check whether the given row and column positions are valid and throw
		// error otherwise.
		if ((row > rowCount) || (col > colCount)) {
			triggerErrorState();
			LOG4CPLUS_ERROR(logger, "Trying to insert a value at illegal position (" << row << ", " << col << ") in matrix of size (" << rowCount << ", " << colCount << ").");
			throw storm::exceptions::OutOfRangeException() << "Trying to insert a value at illegal position (" << row << ", " << col << ") in matrix of size (" << rowCount << ", " << colCount << ").";
		}
        
		// If we switched to another row, we have to adjust the missing entries in the rowIndications array.
		if (row != lastRow) {
			for (uint_fast64_t i = lastRow + 1; i <= row; ++i) {
				rowIndications[i] = currentSize;
			}
			lastRow = row;
		}
        
		// Finally, set the element and increase the current size.
		valueStorage.push_back(value);
		columnIndications.push_back(col);
        ++nonZeroEntryCount;
		++currentSize;
	}


	/*
	 * Finalizes the sparse matrix to indicate that initialization has been completed and the matrix may now be used.
     *
     * @param pushSentinelElement A boolean flag that indicates whether the sentinel element is to be pushed or inserted
     * at a fixed location. If the elements have been added to the matrix via insertNextElement, this needs to be true
     * and false otherwise.
	 */
	void finalize(bool pushSentinelElement = false) {
		// Check whether it's safe to finalize the matrix and throw error otherwise.
		if (!isInitialized()) {
			triggerErrorState();
			LOG4CPLUS_ERROR(logger, "Trying to finalize an uninitialized matrix.");
			throw storm::exceptions::InvalidStateException("Trying to finalize an uninitialized matrix.");
		} else if (currentSize != nonZeroEntryCount) {
			triggerErrorState();
			LOG4CPLUS_ERROR(logger, "Trying to finalize a matrix that was initialized with more non-zero entries than given (expected " << nonZeroEntryCount << " but got " << currentSize << " instead)");
			throw storm::exceptions::InvalidStateException() << "Trying to finalize a matrix that was initialized with more non-zero entries than given (expected " << nonZeroEntryCount << " but got " << currentSize << " instead).";
		} else {
			// Fill in the missing entries in the row_indications array.
			// (Can happen because of empty rows at the end.)
			if (lastRow != rowCount) {
				for (uint_fast64_t i = lastRow + 1; i < rowCount; ++i) {
					rowIndications[i] = currentSize;
				}
			}

			// Set a sentinel element at the last position of the row_indications array. This eases iteration work, as
            // now the indices of row i are always between rowIndications[i] and rowIndications[i + 1], also for the
            // first and last row.
            if (pushSentinelElement) {
                rowIndications.push_back(nonZeroEntryCount);
            } else {
                rowIndications[rowCount] = nonZeroEntryCount;
            }

			setState(MatrixStatus::ReadReady);
		}
	}

	/*!
	 * Gets the matrix element at the given row and column to the given value.
	 * Note: This function does not check the internal status for errors for performance reasons.
	 *
	 * @param row The row in which the element is to be read.
	 * @param col The column in which the element is to be read.
	 * @param target A pointer to the memory location where the read content is
	 * to be put.
	 * @returns True iff the value is set in the matrix, false otherwise.
	 * On false, 0 will be written to *target.
	 */
	inline bool getValue(uint_fast64_t row, uint_fast64_t col, T* const target) const {
		// Check for illegal access indices.
		if ((row > rowCount) || (col > colCount)) {
			LOG4CPLUS_ERROR(logger, "Trying to read a value from illegal position (" << row << ", " << col << ").");
			throw storm::exceptions::OutOfRangeException("Trying to read a value from illegal position.");
			return false;
		}

		// In case the element is not on the diagonal, we have to iterate
		// over the accessed row to find the element.
		uint_fast64_t rowStart = rowIndications[row];
		uint_fast64_t rowEnd = rowIndications[row + 1];
		while (rowStart < rowEnd) {
			// If the lement is found, write the content to the specified
			// position and return true.
			if (columnIndications[rowStart] == col) {
				*target = valueStorage[rowStart];
				return true;
			}
			
			// If the column of the current element is already larger than the
			// requested column, the requested element cannot be contained
			// in the matrix and we may therefore stop searching.
			if (columnIndications[rowStart] > col) {
				break;
			}
			++rowStart;
		}

		// Set 0 as the content and return false in case the element was not found.
		*target = 0;
		return false;
	}

	/*!
	 * Gets the matrix element at the given row and column in the form of a reference to it.
	 * Note: This function does not check the internal status for errors for performance reasons.
	 * Warning: It is possible to modify the matrix through this function. It may only be used
	 * for elements that exist in the sparse matrix. If the value at the requested position does not exist,
	 * an exception will be thrown.
	 
	 * @param row The row in which the element is to be read.
	 * @param col The column in which the element is to be read.
	 *
	 * @return A reference to the value at the given position.
	 */
	inline T& getValue(uint_fast64_t row, uint_fast64_t col) {
		// Check for illegal access indices.
		if ((row > rowCount) || (col > colCount)) {
			LOG4CPLUS_ERROR(logger, "Trying to read a value from illegal position (" << row << ", " << col << ").");
			throw storm::exceptions::OutOfRangeException("Trying to read a value from illegal position.");
		}

		// we have to iterate
		// over the accessed row to find the element.
		uint_fast64_t rowStart = rowIndications[row];
		uint_fast64_t rowEnd = rowIndications[row + 1];
		while (rowStart < rowEnd) {
			// If the lement is found, return it.
			if (columnIndications[rowStart] == col) {
				return valueStorage[rowStart];
			}
			
			// If the column of the current element is already larger than the
			// requested column, the requested element cannot be contained
			// in the matrix and we may therefore stop searching.
			if (columnIndications[rowStart] > col) {
				break;
			}
			++rowStart;
		}

		throw storm::exceptions::InvalidArgumentException("Trying to get a reference to a non-existant value.");
	}

	/*!
	 * Returns the number of rows of the matrix.
	 *
	 * @returns The number of rows of the matrix.
	 */
	uint_fast64_t getRowCount() const {
		return rowCount;
	}

	/*!
	 * Returns the number of columns of the matrix.
	 *
	 * @returns The number of columns of the matrix.
	 */
	uint_fast64_t getColumnCount() const {
		return colCount;
	}

	/*!
	 * Checks whether the internal status of the matrix makes it ready for
	 * reading access.
	 *
	 * @returns True iff the internal status of the matrix makes it ready for
	 * reading access.
	 */
	bool isReadReady() {
		return (internalStatus == MatrixStatus::ReadReady);
	}

	/*!
	 * Checks whether the matrix was initialized previously. The matrix may
	 * still require to be finalized, even if this check returns true.
	 *
	 * @returns True iff the matrix was initialized previously.
	 */
	bool isInitialized() {
		return (internalStatus == MatrixStatus::Initialized || internalStatus == MatrixStatus::ReadReady);
	}

	/*!
	 * Returns the internal state of the matrix.
	 *
	 * @returns The internal state of the matrix.
	 */
	MatrixStatus getState() {
		return internalStatus;
	}

	/*!
	 * Checks whether the internal state of the matrix signals an error.
	 *
	 * @returns True iff the internal state of the matrix signals an error.
	 */
	bool hasError() const {
		return (internalStatus == MatrixStatus::Error);
	}

	/*!
	 * Returns the number of non-zero entries in the matrix.
	 *
	 * @returns The number of non-zero entries in the matrix.
	 */
	uint_fast64_t getNonZeroEntryCount() const {
		return nonZeroEntryCount;
	}

	/*!
	 * This function makes the rows given by the bit vector absorbing.
	 *
	 * @param rows A bit vector indicating which rows to make absorbing.
	 * @returns True iff the operation was successful.
	 */
	bool makeRowsAbsorbing(storm::storage::BitVector const& rows) {
		bool result = true;
		for (auto row : rows) {
			result &= makeRowAbsorbing(row, row);
		}
		return result;
	}

	/*!
	 * This function makes the groups of rows given by the bit vector absorbing.
	 *
	 * @param rowGroupConstraint A bit vector indicating which row groups to make absorbing.
	 * @param rowGroupIndices A vector indicating which rows belong to a given row group.
	 * @return True iff the operation was successful.
	 */
	bool makeRowsAbsorbing(storm::storage::BitVector const& rowGroupConstraint, std::vector<uint_fast64_t> const& rowGroupIndices) {
		bool result = true;
		for (auto rowGroup : rowGroupConstraint) {
			for (uint_fast64_t row = rowGroupIndices[rowGroup]; row < rowGroupIndices[rowGroup + 1]; ++row) {
				result &= makeRowAbsorbing(row, rowGroup);
			}
		}
		return result;
	}


	/*!
	 * This function makes the given row absorbing. This means that all entries will be set to 0
	 * except the one at the specified column, which is set to 1 instead.
	 *
	 * @param row The row to be made absorbing.
	 * @param column The index of the column whose value is to be set to 1.
	 * @returns True iff the operation was successful.
	 */
	bool makeRowAbsorbing(const uint_fast64_t row, const uint_fast64_t column) {
		// Check whether the accessed state exists.
		if (row > rowCount) {
			LOG4CPLUS_ERROR(logger, "Trying to make an illegal row " << row << " absorbing.");
			throw storm::exceptions::OutOfRangeException("Trying to make an illegal row absorbing.");
			return false;
		}

		// Iterate over the elements in the row that are not on the diagonal
		// and set them to zero.
		uint_fast64_t rowStart = rowIndications[row];
		uint_fast64_t rowEnd = rowIndications[row + 1];

		// If the row has no elements in it, we cannot make it absorbing, because we would need to
		// move all elements in the vector of nonzeros otherwise.
		if (rowStart >= rowEnd) {
			LOG4CPLUS_ERROR(logger, "Cannot make row " << row << " absorbing, because there is no entry in this row.");
			throw storm::exceptions::InvalidStateException() << "Cannot make row " << row << " absorbing, because there is no entry in this row.";
		}

		// If there is at least one nonzero entry in this row, we can just set it to one, modify its
		// column indication to the one given by the parameter and set all subsequent elements of this
		// row to zero.
		valueStorage[rowStart] = storm::utility::constGetOne<T>();
		columnIndications[rowStart] = column;
		for (uint_fast64_t index = rowStart + 1; index < rowEnd; ++index) {
			valueStorage[index] = storm::utility::constGetZero<T>();
			columnIndications[index] = 0;
		}

		return true;
	}

	/*
	 * Computes the sum of the elements in the given row whose column bits are set to one on the
	 * given constraint.
	 *
	 * @param row The row whose elements to add.
	 * @param constraint A bit vector that indicates which columns to add.
	 * @returns The sum of the elements in the given row whose column bits
	 * are set to one on the given constraint.
	 */
	T getConstrainedRowSum(uint_fast64_t row, storm::storage::BitVector const& constraint) const {
		T result(0);
		for (uint_fast64_t i = rowIndications[row]; i < rowIndications[row + 1]; ++i) {
			if (constraint.get(columnIndications[i])) {
				result += valueStorage[i];
			}
		}
		return result;
	}

	/*!
	 * Computes a vector whose ith element is the sum of the elements in the ith row where only
	 * those elements are added whose bits are set to true in the given column constraint and only
	 * those rows are treated whose bits are set to true in the given row constraint.
	 *
	 * @param rowConstraint A bit vector that indicates for which rows to perform summation.
	 * @param columnConstraint A bit vector that indicates which columns to add.
	 *
	 * @returns A vector whose ith element is the sum of the elements in the ith row where only
	 * those elements are added whose bits are set to true in the given column constraint and only
	 * those rows are treated whose bits are set to true in the given row constraint.
	 */
	std::vector<T> getConstrainedRowSumVector(storm::storage::BitVector const& rowConstraint, storm::storage::BitVector const& columnConstraint) const {
		std::vector<T> result(rowConstraint.getNumberOfSetBits());
		uint_fast64_t currentRowCount = 0;
		for (auto row : rowConstraint) {
			result[currentRowCount++] = getConstrainedRowSum(row, columnConstraint);
		}
		return result;
	}

	/*!
	 * Computes a vector whose elements represent the sums of selected (given by the column
	 * constraint) entries for all rows in selected row groups given by the row group constraint.
	 *
	 * @param rowGroupConstraint A bit vector that indicates which row groups are to be considered.
	 * @param rowGroupIndices A vector indicating which rows belong to a given row group.
	 * @param columnConstraint A bit vector that indicates which columns to add.
	 * @returns 
	 */
	std::vector<T> getConstrainedRowSumVector(storm::storage::BitVector const& rowGroupConstraint, std::vector<uint_fast64_t> const& rowGroupIndices, storm::storage::BitVector const& columnConstraint, uint_fast64_t numberOfRows) const {
		std::vector<T> result(numberOfRows);
		uint_fast64_t currentRowCount = 0;
		for (auto rowGroup : rowGroupConstraint) {
			for (uint_fast64_t row = rowGroupIndices[rowGroup]; row < rowGroupIndices[rowGroup + 1]; ++row) {
				result[currentRowCount++] = getConstrainedRowSum(row, columnConstraint);
			}
		}
		return result;
	}

	/*!
	 * Creates a submatrix of the current matrix by dropping all rows and columns whose bits are not
	 * set to one in the given bit vector.
	 *
	 * @param constraint A bit vector indicating which rows and columns to keep.
	 * @returns A matrix corresponding to a submatrix of the current matrix in which only rows and
	 * columns given by the constraint are kept and all others are dropped.
	 */
	SparseMatrix getSubmatrix(storm::storage::BitVector const& constraint) const {
		LOG4CPLUS_DEBUG(logger, "Creating a sub-matrix with " << constraint.getNumberOfSetBits() << " rows.");

		// Check for valid constraint.
		if (constraint.getNumberOfSetBits() == 0) {
			LOG4CPLUS_ERROR(logger, "Trying to create a sub-matrix of size 0.");
			throw storm::exceptions::InvalidArgumentException("Trying to create a sub-matrix of size 0.");
		}

		// First, we need to determine the number of non-zero entries of the
		// sub-matrix.
		uint_fast64_t subNonZeroEntries = 0;
		for (auto rowIndex : constraint) {
			for (uint_fast64_t i = rowIndications[rowIndex]; i < rowIndications[rowIndex + 1]; ++i) {
				if (constraint.get(columnIndications[i])) {
					++subNonZeroEntries;
				}
			}
		}

		// Create and initialize resulting matrix.
		SparseMatrix result(constraint.getNumberOfSetBits());
		result.initialize(subNonZeroEntries);

		// Create a temporary array that stores for each index whose bit is set
		// to true the number of bits that were set before that particular index.
		uint_fast64_t* bitsSetBeforeIndex = new uint_fast64_t[colCount];
		uint_fast64_t lastIndex = 0;
		uint_fast64_t currentNumberOfSetBits = 0;
		for (auto index : constraint) {
			while (lastIndex <= index) {
				bitsSetBeforeIndex[lastIndex++] = currentNumberOfSetBits;
			}
			++currentNumberOfSetBits;
		}

		// Copy over selected entries.
		uint_fast64_t rowCount = 0;
		for (auto rowIndex : constraint) {
			for (uint_fast64_t i = rowIndications[rowIndex]; i < rowIndications[rowIndex + 1]; ++i) {
				if (constraint.get(columnIndications[i])) {
					result.addNextValue(rowCount, bitsSetBeforeIndex[columnIndications[i]], valueStorage[i]);
				}
			}

			++rowCount;
		}

		// Dispose of the temporary array.
		delete[] bitsSetBeforeIndex;

		// Finalize sub-matrix and return result.
		result.finalize();
		LOG4CPLUS_DEBUG(logger, "Done creating sub-matrix.");
		return result;
	}

	/*!
	 * Creates a submatrix of the current matrix by keeping only row groups and columns in the given
	 * row group constraint.
	 *
	 * @param rowGroupConstraint A bit vector indicating which row groups and columns to keep.
	 * @param rowGroupIndices A vector indicating which rows belong to a given row group.
	 * @returns A matrix corresponding to a submatrix of the current matrix in which only row groups
	 * and columns given by the row group constraint are kept and all others are dropped.
	 */
	SparseMatrix getSubmatrix(storm::storage::BitVector const& rowGroupConstraint, std::vector<uint_fast64_t> const& rowGroupIndices) const {
		LOG4CPLUS_DEBUG(logger, "Creating a sub-matrix (of unknown size).");

		// First, we need to determine the number of non-zero entries and the number of rows of the
		// sub-matrix.
		uint_fast64_t subNonZeroEntries = 0;
		uint_fast64_t subRowCount = 0;
		for (auto index : rowGroupConstraint) {
			subRowCount += rowGroupIndices[index + 1] - rowGroupIndices[index];
			for (uint_fast64_t i = rowGroupIndices[index]; i < rowGroupIndices[index + 1]; ++i) {
				for (uint_fast64_t j = rowIndications[i]; j < rowIndications[i + 1]; ++j) {
					if (rowGroupConstraint.get(columnIndications[j])) {
						++subNonZeroEntries;
					}
				}
			}
		}

		LOG4CPLUS_DEBUG(logger, "Determined size of submatrix to be " << subRowCount << "x" << rowGroupConstraint.getNumberOfSetBits() << ".");

		// Create and initialize resulting matrix.
		SparseMatrix result(subRowCount, rowGroupConstraint.getNumberOfSetBits());
		result.initialize(subNonZeroEntries);

		// Create a temporary array that stores for each index whose bit is set
		// to true the number of bits that were set before that particular index.
		uint_fast64_t* bitsSetBeforeIndex = new uint_fast64_t[colCount];
		uint_fast64_t lastIndex = 0;
		uint_fast64_t currentNumberOfSetBits = 0;
		for (auto index : rowGroupConstraint) {
			while (lastIndex <= index) {
				bitsSetBeforeIndex[lastIndex++] = currentNumberOfSetBits;
			}
			++currentNumberOfSetBits;
		}

		// Copy over selected entries.
		uint_fast64_t rowCount = 0;
		for (auto index : rowGroupConstraint) {
			for (uint_fast64_t i = rowGroupIndices[index]; i < rowGroupIndices[index + 1]; ++i) {
				for (uint_fast64_t j = rowIndications[i]; j < rowIndications[i + 1]; ++j) {
					if (rowGroupConstraint.get(columnIndications[j])) {
						result.addNextValue(rowCount, bitsSetBeforeIndex[columnIndications[j]], valueStorage[j]);
					}
				}
				++rowCount;
			}
		}

		// Dispose of the temporary array.
		delete[] bitsSetBeforeIndex;

		// Finalize sub-matrix and return result.
		result.finalize();
		LOG4CPLUS_DEBUG(logger, "Done creating sub-matrix.");
		return result;
	}

	/*!
	 * Performs a change to the matrix that is needed if this matrix is to be interpreted as a
	 * set of linear equations. In particular, it transforms A to (1-A), meaning that the elements
	 * on the diagonal are inverted with respect to addition and the other elements are negated.
	 */
	void convertToEquationSystem() {
		invertDiagonal();
		negateAllNonDiagonalElements();
	}

	/*!
	 * Inverts all elements on the diagonal, i.e. sets the diagonal values to 1 minus their previous
	 * value. Requires the matrix to contain each diagonal element and to be square.
	 */
	void invertDiagonal() {
		// Check if the matrix is square, because only then it makes sense to perform this
		// transformation.
		if (this->getRowCount() != this->getColumnCount()) {
			throw storm::exceptions::InvalidArgumentException() << "SparseMatrix::invertDiagonal requires the Matrix to be square!";
		}
		
		// Now iterate over all rows and set the diagonal elements to the inverted value.
		// If there is a row without the diagonal element, an exception is thrown.
		T one = storm::utility::constGetOne<T>();
		bool foundDiagonalElement = false;
		for (uint_fast64_t row = 0; row < rowCount; ++row) {
			uint_fast64_t rowStart = rowIndications[row];
			uint_fast64_t rowEnd = rowIndications[row + 1];
			foundDiagonalElement = false;
			while (rowStart < rowEnd) {
				if (columnIndications[rowStart] == row) {
					valueStorage[rowStart] = one - valueStorage[rowStart];
					foundDiagonalElement = true;
					break;
				}
				++rowStart;
			}
			
			// Throw an exception if a row did not have an element on the diagonal.
			if (!foundDiagonalElement) {
				throw storm::exceptions::InvalidArgumentException() << "SparseMatrix::invertDiagonal requires the Matrix to contain all diagonal entries!";
			}
		}
	}

	/*!
	 * Negates all non-zero elements that are not on the diagonal.
	 */
	void negateAllNonDiagonalElements() {
		// Check if the matrix is square, because only then it makes sense to perform this
		// transformation.
		if (this->getRowCount() != this->getColumnCount()) {
			throw storm::exceptions::InvalidArgumentException() << "SparseMatrix::invertDiagonal requires the Matrix to be square!";
		}
		
		// Iterate over all rows and negate all the elements that are not on the diagonal.
		for (uint_fast64_t row = 0; row < rowCount; ++row) {
			uint_fast64_t rowStart = rowIndications[row];
			uint_fast64_t rowEnd = rowIndications[row + 1];
			while (rowStart < rowEnd) {
				if (columnIndications[rowStart] != row) {
					valueStorage[rowStart] = - valueStorage[rowStart];
				}
				++rowStart;
			}
		}
	}

	/*!
	 * Calculates the Jacobi-Decomposition of this sparse matrix.
	 * The source Sparse Matrix must be square.
	 * @return A pointer to a class containing the matrix L+U and the inverted diagonal matrix D^-1
	 */
	storm::storage::JacobiDecomposition<T>* getJacobiDecomposition() const {
		uint_fast64_t rowCount = this->getRowCount();
		uint_fast64_t colCount = this->getColumnCount();
		if (rowCount != colCount) {
			throw storm::exceptions::InvalidArgumentException() << "SparseMatrix::getJacobiDecomposition requires the Matrix to be square.";
		}
		storm::storage::SparseMatrix<T> *resultLU = new storm::storage::SparseMatrix<T>(*this);
		storm::storage::SparseMatrix<T> *resultDinv = new storm::storage::SparseMatrix<T>(rowCount, colCount);
		// no entries apart from those on the diagonal (rowCount)
		resultDinv->initialize(rowCount);

		// constant 1 for diagonal inversion
		T constOne = storm::utility::constGetOne<T>();

		// copy diagonal entries to other matrix
		for (unsigned int i = 0; i < rowCount; ++i) {
			resultDinv->addNextValue(i, i, constOne / resultLU->getValue(i, i));
			resultLU->getValue(i, i) = storm::utility::constGetZero<T>();
		}

		return new storm::storage::JacobiDecomposition<T>(resultLU, resultDinv);
	}

	/*!
	 * Performs a pointwise matrix multiplication of the matrix with the given matrix and returns a
	 * vector containing the sum of the elements in each row of the resulting matrix.
	 *
	 * @param otherMatrix A reference to the matrix with which to perform the pointwise multiplication.
	 * This matrix must be a submatrix of the current matrix in the sense that it may not have
	 * non-zero entries at indices where there is a zero in the current matrix.
	 * @returns A vector containing the sum of the elements in each row of the matrix resulting from
	 * pointwise multiplication of the current matrix with the given matrix.
	 */
	std::vector<T> getPointwiseProductRowSumVector(storm::storage::SparseMatrix<T> const& otherMatrix) {
		// Prepare result.
		std::vector<T> result(rowCount, storm::utility::constGetZero<T>());

		// Iterate over all elements of the current matrix and either continue with the next element
		// in case the given matrix does not have a non-zero element at this column position, or
		// multiply the two entries and add the result to the corresponding position in the vector.
		for (uint_fast64_t row = 0; row < rowCount && row < otherMatrix.rowCount; ++row) {
			for (uint_fast64_t element = rowIndications[row], nextOtherElement = otherMatrix.rowIndications[row]; element < rowIndications[row + 1] && nextOtherElement < otherMatrix.rowIndications[row + 1]; ++element) {
				if (columnIndications[element] < otherMatrix.columnIndications[nextOtherElement]) {
					continue;
				} else {
					// If the precondition of this method (i.e. that the given matrix is a submatrix
					// of the current one) was fulfilled, we know now that the two elements are in
					// the same column, so we can multiply and add them to the row sum vector.
					result[row] += otherMatrix.valueStorage[nextOtherElement] * valueStorage[element];
					++nextOtherElement;
				}
			}
		}

		return result;
	}
	
	/*!
	 * Multiplies the matrix with the given vector and writes the result to given result vector.
	 *
	 * @param vector The vector with which to multiply the matrix.
	 * @param result The vector that is supposed to hold the result of the multiplication after the
	 * operation.
	 * @returns The product of the matrix and the given vector as the content of the given result
	 * vector.
	 */
	void multiplyWithVector(std::vector<T> const& vector, std::vector<T>& result) const {
		// Initialize two iterators that 
		ConstRowsIterator elementIt(*this);
		ConstRowsIterator elementIte(*this, 1);
		
		// Iterate over all positions of the result vector and compute its value as the scalar
		// product of the corresponding row with the input vector.
		// Note that only the end iterator has to be moved by one row, because the other iterator
		// is automatically moved forward one row by the inner loop.
		for (auto it = result.begin(), ite = result.end(); it != ite; ++it, elementIte.moveToNextRow()) {
			*it = storm::utility::constGetZero<T>();
			
			// Perform the scalar product.
			for (; elementIt != elementIte; ++elementIt) {
				*it += elementIt.value() * vector[elementIt.column()];
			}
		}
	}

	/*!
	 * Returns the size of the matrix in memory measured in bytes.
	 *
	 * @returns The size of the matrix in memory measured in bytes.
	 */
	uint_fast64_t getSizeInMemory() const {
		uint_fast64_t size = sizeof(*this);
		// Add value_storage size.
		size += sizeof(T) * valueStorage.capacity();
		// Add column_indications size.
		size += sizeof(uint_fast64_t) * columnIndications.capacity();
		// Add row_indications size.
		size += sizeof(uint_fast64_t) * rowIndications.capacity();
		return size;
	}

	/*!
	 * Returns a const iterator to the elements of the matrix.
	 * @param row If given, this specifies the starting row of the iterator.
	 *
	 * @returns An iterator to the elements of the matrix that cannot be used for modifying the
	 * contents.
	 */
	ConstRowsIterator begin(uint_fast64_t row = 0) const {
		return ConstRowsIterator(*this, row);
	}
	
	/*!
	 * Returns a const iterator that points to the first element after the matrix.
	 *
	 * @returns A const iterator that points past the last element of the matrix.
	 */
	ConstRowsIterator end() const {
		return ConstRowsIterator(this->rowCount);
	}
	
	/*!
	 * Returns a const iterator that points to the first element after the given row.
	 *
	 * @returns row The row past which this iterator has to point.
	 * @returns A const iterator that points to the first element after the given row.
	 */
	ConstRowsIterator end(uint_fast64_t row) const {
		return ConstRowsIterator(row + 1);
	}

	/*!
	 * Returns an iterator to the columns of the non-zero entries of the matrix.
	 *
	 * @param row If given, the iterator will start at the specified row.
	 * @returns An iterator to the columns of the non-zero entries of the matrix.
	 */
	ConstIndexIterator constColumnIteratorBegin(uint_fast64_t row = 0) const {
		return &(this->columnIndications[0]) + this->rowIndications[row];
	}

	/*!
	 * Returns an iterator that points to the first element after the matrix.
	 *
	 * @returns An iterator that points to the first element after the matrix.
	 */
	ConstIndexIterator constColumnIteratorEnd() const {
		return &(this->columnIndications[0]) + this->rowIndications[rowCount];
	}
	
	/*!
	 * Returns an iterator that points to the first element after the given row.
	 *
	 * @param row The row past which this iterator has to point.
	 * @returns An iterator that points to the first element after the matrix.
	 */
	ConstIndexIterator constColumnIteratorEnd(uint_fast64_t row) const {
		return &(this->columnIndications[0]) + this->rowIndications[row + 1];
	}
	
	/*!
	 * Returns an iterator to the values of the non-zero entries of the matrix.
	 *
	 * @param row If given, the iterator will start at the specified row.
	 * @returns An iterator to the values of the non-zero entries of the matrix.
	 */
	ConstValueIterator constValueIteratorBegin(uint_fast64_t row = 0) const {
		return &(this->valueStorage[0]) + this->rowIndications[row];
	}
	
	/*!
	 * Returns an iterator that points to the first element after the matrix.
	 *
	 * @returns An iterator that points to the first element after the matrix.
	 */
	ConstValueIterator constValueIteratorEnd() const {
		return &(this->valueStorage[0]) + this->rowIndications[rowCount];
	}
	
	/*!
	 * Returns an iterator that points to the first element after the given row.
	 *
	 * @param row The row past which this iterator has to point.
	 * @returns An iterator that points to the first element after the matrix.
	 */
	ConstValueIterator constValueIteratorEnd(uint_fast64_t row) const {
		return &(this->valueStorage[0]) + this->rowIndications[row + 1];
	}
	
	/*!
	 * Computes the sum of the elements in a given row.
	 *
	 * @param row The row that should be summed.
	 * @return Sum of the row.
	 */
	T getRowSum(uint_fast64_t row) const {
		T sum = storm::utility::constGetZero<T>();
		for (auto it = this->constValueIteratorBegin(row), ite = this->constValueIteratorEnd(row); it != ite; ++it) {
			sum += *it;
		}
		return sum;
	}

	/*!
	 * Checks if the given matrix is a submatrix of the current matrix, where A matrix A is called a
	 * submatrix of B if a value in A is only nonzero, if the value in B at the same position is
	 * also nonzero. Furthermore, A and B have to have the same size.
	 *
	 * @param matrix The matrix that is possibly a submatrix of the current matrix.
	 * @returns True iff the given matrix is a submatrix of the current matrix.
	 */
	bool containsAllPositionsOf(SparseMatrix<T> const& matrix) const {
		// Check for mismatching sizes.
		if (this->getRowCount() != matrix.getRowCount()) return false;
		if (this->getColumnCount() != matrix.getColumnCount()) return false;

		// Check the subset property for all rows individually.
		for (uint_fast64_t row = 0; row < this->getRowCount(); ++row) {
			for (uint_fast64_t elem = rowIndications[row], elem2 = matrix.rowIndications[row]; elem < rowIndications[row + 1] && elem < matrix.rowIndications[row + 1]; ++elem) {
				// Skip over all entries of the other matrix that are before the current entry in
				// the current matrix.
				while (elem2 < matrix.rowIndications[row + 1] && matrix.columnIndications[elem2] < columnIndications[elem]) {
					++elem2;
				}
				if (!(elem2 < matrix.rowIndications[row + 1]) || columnIndications[elem] != matrix.columnIndications[elem2]) return false;
			}
		}
		return true;
	}

	/*!
	 * Retrieves a compressed string representation of the matrix.
	 *
	 * @returns a compressed string representation of the matrix.
	 */
	std::string toStringCompressed() const {
		std::stringstream result;
		result << rowIndications << std::endl;
		result << columnIndications << std::endl;
		result << valueStorage << std::endl;
		return result.str();
	}

	/*!
	 * Retrieves a (non-compressed) string representation of the matrix.
	 * Note: the matrix is presented densely. That is, all zeros are filled in and are part of the
	 * string representation, so calling this method on big matrices should be done with care.
	 *
	 * @param rowGroupIndices A vector indicating to which group of rows a given row belongs. If
	 * given, rows of different groups will be separated by a dashed line.
	 * @returns A (non-compressed) string representation of the matrix.
	 */
	std::string toString(std::vector<uint_fast64_t> const* rowGroupIndices = nullptr) const {
		std::stringstream result;
		uint_fast64_t currentNondeterministicChoiceIndex = 0;

		// Print column numbers in header.
		result << "\t\t";
		for (uint_fast64_t i = 0; i < colCount; ++i) {
			result << i << "\t";
		}
		result << std::endl;

		// Iterate over all rows.
		for (uint_fast64_t i = 0; i < rowCount; ++i) {
			uint_fast64_t nextIndex = rowIndications[i];

			// If we need to group rows, print a dashed line in case we have moved to the next group of rows.
			if (rowGroupIndices != nullptr) {
				if (i == (*rowGroupIndices)[currentNondeterministicChoiceIndex]) {
					if (i != 0) {
						result << "\t(\t";
						for (uint_fast64_t j = 0; j < colCount - 2; ++j) {
							result << "----";
							if (j == 1) {
								result << "\t" << currentNondeterministicChoiceIndex << "\t";
							}
						}
						result << "\t)" << std::endl;
					}
					++currentNondeterministicChoiceIndex;
				}
			}

			// Print the actual row.
			result << i << "\t(\t";
			uint_fast64_t currentRealIndex = 0;
			while (currentRealIndex < colCount) {
				if (nextIndex < rowIndications[i + 1] && currentRealIndex == columnIndications[nextIndex]) {
					result << valueStorage[nextIndex] << "\t";
					++nextIndex;
				} else {
					result << "0\t";
				}
				++currentRealIndex;
			}
			result << "\t)\t" << i << std::endl;
		}

		// Print column numbers in footer.
		result << "\t\t";
		for (uint_fast64_t i = 0; i < colCount; ++i) {
			result << i << "\t";
		}
		result << std::endl;

		// Return final result.
		return result.str();
	}

private:

	/*!
	 * The number of rows of the matrix.
	 */
	uint_fast64_t rowCount;

	/*!
	 * The number of columns of the matrix.
	 */
	uint_fast64_t colCount;

	/*!
	 * The number of non-zero elements.
	 */
	uint_fast64_t nonZeroEntryCount;

	/*!
	 * Stores all non-zero values.
	 */
	std::vector<T> valueStorage;

	/*!
	 * Stores the column for each non-zero element.
	 */
	std::vector<uint_fast64_t> columnIndications;

	/*!
	 * Vector containing the boundaries (indices) in the value_storage array
	 * for each row. All elements of value_storage with indices between the
	 * i-th and the (i+1)-st element of this array belong to row i.
	 */
	std::vector<uint_fast64_t> rowIndications;

	/*!
	 * The internal status of the matrix.
	 */
	MatrixStatus internalStatus;

	/*!
	 * Stores the current number of non-zero elements that have been added to
	 * the matrix. Used for correctly inserting elements in the matrix.
	 */
	uint_fast64_t currentSize;

	/*!
	 * Stores the row in which the last element was inserted. Used for correctly
	 * inserting elements in the matrix .
	 */
	uint_fast64_t lastRow;

	/*!
	 * Sets the internal status to signal an error.
	 */
	void triggerErrorState() {
		setState(MatrixStatus::Error);
	}

	/*!
	 * Sets the internal status to the given state if the current state is not
	 * the error state.
	 * @param new_state The new state to be switched to.
	 */
	void setState(const MatrixStatus new_state) {
		internalStatus = (internalStatus == MatrixStatus::Error) ? internalStatus : new_state;
	}

	/*!
	 * Prepares the internal storage. For this, it requires the number of non-zero entries and the
	 * amount of rows to be set correctly.
	 *
	 * @param initializeElements If set to true, all entries are initialized.
	 * @return True on success, false otherwise (allocation failed).
	 */
	bool prepareInternalStorage(bool initializeElements = true) {
		if (initializeElements) {
			// Set up the arrays for the elements that are not on the diagonal.
			valueStorage.resize(nonZeroEntryCount, storm::utility::constGetZero<T>());
			columnIndications.resize(nonZeroEntryCount, 0);

			// Set up the rowIndications vector and reserve one element more than there are rows in
			// order to put a sentinel element at the end, which eases iteration process.
			rowIndications.resize(rowCount + 1, 0);

			// Return whether all the allocations could be made without error.
			return ((valueStorage.capacity() >= nonZeroEntryCount) && (columnIndications.capacity() >= nonZeroEntryCount)
				&& (rowIndications.capacity() >= (rowCount + 1)));
		} else {
			// If it was not requested to initialize the elements, we simply reserve the space.
			valueStorage.reserve(nonZeroEntryCount);
			columnIndications.reserve(nonZeroEntryCount);
			rowIndications.reserve(rowCount + 1);
			return true;
		}
	}
};

} // namespace storage
} // namespace storm

#endif // STORM_STORAGE_SPARSEMATRIX_H_
