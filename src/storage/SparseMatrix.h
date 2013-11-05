#ifndef STORM_STORAGE_SPARSEMATRIX_H_
#define STORM_STORAGE_SPARSEMATRIX_H_

// To detect whether the usage of TBB is possible, this include is neccessary
#include "storm-config.h"

#ifdef STORM_HAVE_INTELTBB
#	include "utility/OsDetection.h" // This fixes a potential dependency ordering problem between GMM and TBB
#	include <new>
#	include "tbb/tbb.h"
#	include <iterator>
#endif

#include <exception>
#include <new>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <set>
#include <cstdint>

#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/OutOfRangeException.h"
#include "src/exceptions/FileIoException.h"
#include "src/storage/BitVector.h"

#include "src/utility/ConstTemplates.h"
#include "src/utility/Hash.h"
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

#ifdef STORM_HAVE_INTELTBB
// Forward declaration of the TBB Helper class
template <typename M, typename V, typename T>
class tbbHelper_MatrixRowVectorScalarProduct;
#endif

/*!
 * A sparse matrix class with a constant number of non-zero entries.
 * NOTE: Addressing *is* zero-based, so the valid range for getValue and addNextValue is 0..(rows - 1) where rows is the
 * first argument to the constructor.
 */
template<class T>
class SparseMatrix {
public:
	/*!
	 * Return Type of the Jacobi Decompostion
	 */
	typedef std::pair<storm::storage::SparseMatrix<T>, storm::storage::SparseMatrix<T>> SparseJacobiDecomposition_t;

	/*!
	 * Declare adapter classes as friends to use internal data.
	 */
	friend class storm::adapters::GmmxxAdapter;
	friend class storm::adapters::EigenAdapter;
	friend class storm::adapters::StormAdapter;

#ifdef STORM_HAVE_INTELTBB
	/*!
	 * Declare the helper class for TBB as friend
	 */
	template <typename M, typename V, typename TPrime>
	friend class tbbHelper_MatrixRowVectorScalarProduct;
#endif

	/*!
	 * If we only want to iterate over the columns or values of the non-zero entries of
	 * a row, we can simply iterate over the array (part) itself.
	 */
	typedef uint_fast64_t const* ConstIndexIterator;
	typedef T const* ConstValueIterator;
    
    /*!
	 * A class representing an iterator over a continuous number of rows of the matrix.
	 */
	class ConstIterator {
	public:
		/*!
		 * Constructs an iterator from the given parameters.
		 *
		 * @param valuePtr A pointer to the value of the first element that is to be iterated over.
         * @param columnPtr A pointer to the column of the first element that is to be iterated over.
		 */
		ConstIterator(T const* valuePtr, uint_fast64_t const* columnPtr) : valuePtr(valuePtr), columnPtr(columnPtr) {
			// Intentionally left empty.
		}
        
		/*!
		 * Moves the iterator to the next non-zero element.
		 *
		 * @return A reference to itself.
		 */
		ConstIterator& operator++() {
			++valuePtr;
            ++columnPtr;
			return *this;
		}
        
        /*!
         * Dereferences the iterator by returning a reference to itself. This is needed for making use of the range-based
         * for loop over transitions.
         *
         * @return A reference to itself.
         */
        ConstIterator& operator*() {
            return *this;
        }
        
		/*!
		 * Compares the two iterators for inequality.
		 *
		 * @return True iff the given iterator points to a different index as the current iterator.
		 */
		bool operator!=(ConstIterator const& other) const {
			return this->valuePtr != other.valuePtr;
		}
        
		/*!
		 * Assigns the position of the given iterator to the current iterator.
         *
         * @return A reference to itself.
		 */
		ConstIterator& operator=(ConstIterator const& other) {
			this->valuePtr = other.valuePtr;
			this->columnPtr = other.columnPtr;
			return *this;
		}
        
        /*!
         * Retrieves the column that is associated with the current non-zero element to which this iterator
         * points.
		 *
		 * @return The column of the current non-zero element to which this iterator points.
         */
		uint_fast64_t column() const {
			return *columnPtr;
		}
		
        /*!
         * Retrieves the value of the current non-zero element to which this iterator points.
		 *
		 * @return The value of the current non-zero element to which this iterator points.
         */
		T const& value() const {
			return *valuePtr;
		}
        
    private:
        // A pointer to the value of the current non-zero element.
        T const* valuePtr;
        
        // A pointer to the column of the current non-zero element.
        uint_fast64_t const* columnPtr;
	};
    
    /*!
     * This class represents a number of consecutive rows of the matrix.
     */
    class Rows {
    public:
        /*!
         * Constructs a row from the given parameters.
         *
         * @param valuePtr A pointer to the value of the first non-zero element of the rows.
         * @param columnPtr A pointer to the column of the first non-zero element of the rows.
         * @param entryCount The number of non-zero elements of the rows.
         */
        Rows(T const* valuePtr, uint_fast64_t const* columnPtr, uint_fast64_t entryCount) : valuePtr(valuePtr), columnPtr(columnPtr), entryCount(entryCount) {
            // Intentionally left empty.
        }
        
        /*!
         * Retrieves an iterator that points to the beginning of the rows.
         *
         * @return An iterator that points to the beginning of the rows.
         */
        ConstIterator begin() const {
            return ConstIterator(valuePtr, columnPtr);
        }
        
        /*!
         * Retrieves an iterator that points past the last element of the rows.
         *
         * @return An iterator that points past the last element of the rows.
         */
        ConstIterator end() const {
            return ConstIterator(valuePtr + entryCount, columnPtr + entryCount);
        }
        
    private:
        // The pointer to the value of the first element.
        T const* valuePtr;
        
        // The pointer to the column of the first element.
        uint_fast64_t const* columnPtr;
        
        // The number of non-zero entries in the rows.
        uint_fast64_t entryCount;
    };

    /*!
     * This class represents an iterator to all rows of the matrix.
     */
    class ConstRowIterator {
    public:
        /*!
         * Constructs a new iterator with the given parameters.
         *
         * @param startValuePtr A pointer to the value of the first non-zero element of the matrix.
         * @param startColumnPtr A pointer to the column of the first non-zero element of the matrix.
         * @param rowPtr A pointer to the index at which the first row begins.
         */
        ConstRowIterator(T const* startValuePtr, uint_fast64_t const* startColumnPtr, uint_fast64_t const* rowPtr) : startValuePtr(startValuePtr), startColumnPtr(startColumnPtr), rowPtr(rowPtr) {
            // Intentionally left empty.
        }
        
        /*!
         * This sets the iterator to point to the next row.
         *
         * @return A reference to itself.
         */
        ConstRowIterator& operator++() {
			++rowPtr;
			return *this;
		}
        
        /*!
         * Compares the iterator to the given row iterator.
         */
        bool operator!=(ConstRowIterator const& other) const {
            return this->rowPtr != other.rowPtr;
        }
        
        /*!
         * Retrieves an iterator that points to the beginning of the current row.
         *
         * @return An iterator that points to the beginning of the current row.
         */
        ConstIterator begin() const {
            return ConstIterator(startValuePtr + *rowPtr, startColumnPtr + *rowPtr);
        }
        
        /*!
         * Retrieves an iterator that points past the end of the current row.
         *
         * @return An iterator that points past the end of the current row.
         */
        ConstIterator end() const {
            return ConstIterator(startValuePtr + *(rowPtr + 1), startColumnPtr + *(rowPtr + 1));
        }
        
    private:
        // The pointer to the value of the first non-zero element of the matrix.
        T const* startValuePtr;
        
        // A pointer to the column of the first non-zero element of the matrix.
        uint_fast64_t const* startColumnPtr;
        
        // A pointer to the index at which the current row starts.
        uint_fast64_t const* rowPtr;
    };
    
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
	 * Move Constructor.
	 *
	 * @param other The Matrix from which to move the content
	 */
	SparseMatrix(SparseMatrix<T>&& other)
		: rowCount(other.rowCount), colCount(other.colCount), nonZeroEntryCount(other.nonZeroEntryCount),
		valueStorage(std::move(other.valueStorage)), columnIndications(std::move(other.columnIndications)),
		rowIndications(std::move(other.rowIndications)), internalStatus(other.internalStatus), 
		currentSize(other.currentSize), lastRow(other.lastRow) {
		// Now update the source matrix
		other.rowCount = 0;
		other.colCount = 0;
		other.nonZeroEntryCount = 0;
		other.internalStatus = MatrixStatus::Error;
		other.currentSize = 0;
		other.lastRow = 0;
	}

	/*!
	 * Copy Constructor.
	 *
	 * @param other The Matrix from which to copy the content
	 */
	SparseMatrix(const SparseMatrix<T> & other)
		: rowCount(other.rowCount), colCount(other.colCount), nonZeroEntryCount(other.nonZeroEntryCount),
		valueStorage(other.valueStorage), columnIndications(other.columnIndications),
		rowIndications(other.rowIndications), internalStatus(other.internalStatus), 
		currentSize(other.currentSize), lastRow(other.lastRow) {
	}

	/*!
	 * Copy Assignment Operator.
	 *
	 * @param other The Matrix from which to copy the content
	 */
	storm::storage::SparseMatrix<T>& operator=(SparseMatrix<T> const& other) {
		this->rowCount = other.rowCount;
		this->colCount = other.colCount;
		this->nonZeroEntryCount = other.nonZeroEntryCount;

		this->valueStorage = other.valueStorage;
		this->columnIndications = other.columnIndications;
		this->rowIndications = other.rowIndications;

		this->internalStatus = other.internalStatus;
		this->currentSize = other.currentSize;
		this->lastRow = other.lastRow;

		return *this;
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
     * a call to finalize(false) is mandatory.
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
     * Inserts a value at the given row and column with the given value. After all elements have been inserted,
     * a call to finalize(true) is mandatory.
     * NOTE: This is a linear inserter. It must be called consecutively for each element, row by row *and* column by
     * column.
     * NOTE: This method is different from addNextValue(...) in that the number of nonzero elements need not be known
     * in advance, because inserting elements will automatically increase the size of the underlying storage.
	 *
	 * @param row The row in which the matrix element is to be set.
	 * @param col The column in which the matrix element is to be set.
	 * @param value The value that is to be set.
     * @param pushRowIndication If set to true, the next row indication value is pushed, otherwise it is added. If the
     * number of rows was not set in the beginning, then this needs to be true and false otherwise.
     */
    void insertNextValue(const uint_fast64_t row, const uint_fast64_t col,	T const& value, bool pushRowIndication = false) {
		// Check whether the given row and column positions are valid and throw
		// error otherwise.
		if (row < lastRow) {
			triggerErrorState();
			LOG4CPLUS_ERROR(logger, "Trying to insert a value at illegal position (" << row << ", " << col << ").");
			throw storm::exceptions::OutOfRangeException() << "Trying to insert a value at illegal position (" << row << ", " << col << ").";
		}
        
		// If we switched to another row, we have to adjust the missing entries in the rowIndications array.
		if (row != lastRow) {
			for (uint_fast64_t i = lastRow + 1; i <= row; ++i) {
                if (pushRowIndication) {
                    rowIndications.push_back(currentSize);
                } else {
                    rowIndications[i] = currentSize;
                }
			}
            rowCount = row + 1;
            lastRow = row;
		}
        
		// Finally, set the element and increase the current size.
		valueStorage.push_back(value);
		columnIndications.push_back(col);
        ++nonZeroEntryCount;
		++currentSize;
        
        // Check that we also have the correct number of columns.
        colCount = std::max(colCount, col + 1);
	}
    
    /*!
     * Inserts an empty row in the matrix.
     *
     * @param pushRowIndication If set to true, the next row indication value is pushed, otherwise it is added. If the
     * number of rows was not set in the beginning, then this needs to be true and false otherwise.
     */
    void insertEmptyRow(bool pushRowIndication = false) {
        if (pushRowIndication) {
            rowIndications.push_back(currentSize);
        } else {
            rowIndications[lastRow + 1] = currentSize;
        }
        
        ++rowCount;
        ++lastRow;
    }
    
	/*
	 * Finalizes the sparse matrix to indicate that initialization has been completed and the matrix may now be used.
     *
     * @param pushSentinelElement A boolean flag that indicates whether the sentinel element is to be pushed or inserted
     * at a fixed location. If the elements have been added to the matrix via insertNextValue, this needs to be true
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
			for (uint_fast64_t i = lastRow + 1; i < rowCount; ++i) {
				rowIndications[i] = currentSize;
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
			throw storm::exceptions::OutOfRangeException() << "Trying to make an illegal row " << row << " absorbing.";
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

		// Create a temporary vecotr that stores for each index whose bit is set
        // to true the number of bits that were set before that particular index.
        std::vector<uint_fast64_t> bitsSetBeforeIndex;
        bitsSetBeforeIndex.reserve(colCount);
        
        // Compute the information to fill this vector.
		uint_fast64_t lastIndex = 0;
		uint_fast64_t currentNumberOfSetBits = 0;
		for (auto index : constraint) {
			while (lastIndex <= index) {
				bitsSetBeforeIndex.push_back(currentNumberOfSetBits);
                ++lastIndex;
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

		// First, we need to determine the number of non-zero entries and the number of rows of the sub-matrix.
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

		// Create a temporary vector that stores for each index whose bit is set
		// to true the number of bits that were set before that particular index.
        std::vector<uint_fast64_t> bitsSetBeforeIndex;
        bitsSetBeforeIndex.reserve(colCount);
        
        // Compute the information to fill this vector.
		uint_fast64_t lastIndex = 0;
		uint_fast64_t currentNumberOfSetBits = 0;
		for (auto index : rowGroupConstraint) {
			while (lastIndex <= index) {
				bitsSetBeforeIndex.push_back(currentNumberOfSetBits);
                ++lastIndex;
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

		// Finalize sub-matrix and return result.
		result.finalize();
		LOG4CPLUS_DEBUG(logger, "Done creating sub-matrix.");
		return result;
	}
    
    SparseMatrix getSubmatrix(std::vector<uint_fast64_t> const& rowGroupToRowIndexMapping, std::vector<uint_fast64_t> const& rowGroupIndices, bool insertDiagonalEntries = true) const {
        LOG4CPLUS_DEBUG(logger, "Creating a sub-matrix (of unknown size).");
        
        // First, we need to count how many non-zero entries the resulting matrix will have and reserve space for diagonal
        // entries.
        uint_fast64_t subNonZeroEntries = 0;
        for (uint_fast64_t rowGroupIndex = 0, rowGroupIndexEnd = rowGroupToRowIndexMapping.size(); rowGroupIndex < rowGroupIndexEnd; ++rowGroupIndex) {
            // Determine which row we need to select from the current row group.
            uint_fast64_t rowToCopy = rowGroupIndices[rowGroupIndex] + rowGroupToRowIndexMapping[rowGroupIndex];
            
            // Iterate through that row and count the number of slots we have to reserve for copying.
            bool foundDiagonalElement = false;
            for (uint_fast64_t i = rowIndications[rowToCopy], rowEnd = rowIndications[rowToCopy + 1]; i < rowEnd; ++i) {
                if (columnIndications[i] == rowGroupIndex) {
                    foundDiagonalElement = true;
                }
                ++subNonZeroEntries;
            }
            if (insertDiagonalEntries && !foundDiagonalElement) {
                ++subNonZeroEntries;
            }
        }
        
        LOG4CPLUS_DEBUG(logger, "Determined size of submatrix to be " << (rowGroupIndices.size() - 1) << "x" << colCount << " with " << subNonZeroEntries << " non-zero elements.");
        
        // Now create the matrix to be returned with the appropriate size.
        SparseMatrix<T> submatrix(rowGroupIndices.size() - 1, colCount);
        submatrix.initialize(subNonZeroEntries);
        
        // Copy over the selected lines from the source matrix.
        for (uint_fast64_t rowGroupIndex = 0, rowGroupIndexEnd = rowGroupToRowIndexMapping.size(); rowGroupIndex < rowGroupIndexEnd; ++rowGroupIndex) {
            // Determine which row we need to select from the current row group.
            uint_fast64_t rowToCopy = rowGroupIndices[rowGroupIndex] + rowGroupToRowIndexMapping[rowGroupIndex];
            
            // Iterate through that row and copy the entries. This also inserts a zero element on the diagonal if there
            // is no entry yet.
            bool insertedDiagonalElement = false;
            for (uint_fast64_t i = rowIndications[rowToCopy], rowEnd = rowIndications[rowToCopy + 1]; i < rowEnd; ++i) {
                if (columnIndications[i] == rowGroupIndex) {
                    insertedDiagonalElement = true;
                } else if (insertDiagonalEntries && !insertedDiagonalElement && columnIndications[i] > rowGroupIndex) {
                    submatrix.addNextValue(rowGroupIndex, rowGroupIndex, storm::utility::constGetZero<T>());
                    insertedDiagonalElement = true;
                }
                submatrix.addNextValue(rowGroupIndex, columnIndications[i], valueStorage[i]);
            }
            if (insertDiagonalEntries && !insertedDiagonalElement) {
                submatrix.addNextValue(rowGroupIndex, rowGroupIndex, storm::utility::constGetZero<T>());
            }
        }
        
        // Finalize created matrix and return result.
        submatrix.finalize();
        LOG4CPLUS_DEBUG(logger, "Done creating sub-matrix.");
        return submatrix;
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
	 * @return A std::pair containing the matrix L+U and the inverted diagonal matrix D^-1
	 */
	SparseJacobiDecomposition_t getJacobiDecomposition() const {
		uint_fast64_t rowCount = this->getRowCount();
		uint_fast64_t colCount = this->getColumnCount();
		if (rowCount != colCount) {
			throw storm::exceptions::InvalidArgumentException() << "SparseMatrix::getJacobiDecomposition requires the Matrix to be square.";
		}
		storm::storage::SparseMatrix<T> resultLU(*this);
		storm::storage::SparseMatrix<T> resultDinv(rowCount, colCount);
		// no entries apart from those on the diagonal (rowCount)
		resultDinv.initialize(rowCount);

		// constant 1 for diagonal inversion
		T constOne = storm::utility::constGetOne<T>();

		// copy diagonal entries to other matrix
		for (uint_fast64_t i = 0; i < rowCount; ++i) {
			resultDinv.addNextValue(i, i, constOne / resultLU.getValue(i, i));
			resultLU.getValue(i, i) = storm::utility::constGetZero<T>();
		}
		resultDinv.finalize();

		return std::make_pair(std::move(resultLU), std::move(resultDinv));
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
	std::vector<T> getPointwiseProductRowSumVector(storm::storage::SparseMatrix<T> const& otherMatrix) const {
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
#ifdef STORM_HAVE_INTELTBB
		tbb::parallel_for(tbb::blocked_range<uint_fast64_t>(0, result.size()), tbbHelper_MatrixRowVectorScalarProduct<storm::storage::SparseMatrix<T>, std::vector<T>, T>(this, &vector, &result));
#else
        ConstRowIterator rowIt = this->begin();
        
        for (auto resultIt = result.begin(), resultIte = result.end(); resultIt != resultIte; ++resultIt, ++rowIt) {
            *resultIt = storm::utility::constGetZero<T>();

            for (auto elementIt = rowIt.begin(), elementIte = rowIt.end(); elementIt != elementIte; ++elementIt) {
                *resultIt += elementIt.value() * vector[elementIt.column()];
            }
        }
#endif
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
     * Returns an object representing the consecutive rows given by the parameters.
     *
     * @param startRow The starting row.
     * @param endRow The ending row (which is included in the result).
     * @return An object representing the consecutive rows given by the parameters.
     */
    Rows getRows(uint_fast64_t startRow, uint_fast64_t endRow) const {
        return Rows(this->valueStorage.data() + this->rowIndications[startRow], this->columnIndications.data() + this->rowIndications[startRow], this->rowIndications[endRow + 1] - this->rowIndications[startRow]);
    }
    
    /*!
     * Returns an object representing the given row.
     *
     * @param row The chosen row.
     * @return An object representing the given row.
     */
    Rows getRow(uint_fast64_t row) const {
        return getRows(row, row);
    }
    
	/*!
	 * Returns a const iterator to the rows of the matrix.
	 *
     * @param initialRow The initial row to which this iterator points.
	 * @return A const iterator to the rows of the matrix.
	 */
	ConstRowIterator begin(uint_fast64_t initialRow = 0) const {
		return ConstRowIterator(this->valueStorage.data(), this->columnIndications.data(), this->rowIndications.data() + initialRow);
	}
	
	/*!
	 * Returns a const iterator that points to past the last row of the matrix.
	 *
	 * @return A const iterator that points to past the last row of the matrix
	 */
    ConstRowIterator end() const {
		return ConstRowIterator(this->valueStorage.data(), this->columnIndications.data(), this->rowIndications.data() + rowCount);
	}
	
	/*!
	 * Returns a const iterator that points past the given row.
	 *
     * @param row The row past which this iterator points.
	 * @return A const iterator that points past the given row.
	 */
    ConstRowIterator end(uint_fast64_t row) const {
		return ConstRowIterator(this->valueStorage.data(), this->columnIndications.data(), this->rowIndications.data() + row + 1);
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
	 * Checks if the current matrix is a submatrix of the given matrix, where a matrix A is called a
	 * submatrix of B if a value in A is only nonzero, if the value in B at the same position is
	 * also nonzero. Additionally, the matrices must be of equal size.
	 *
	 * @param matrix The matrix that possibly is a "supermatrix" of the current matrix.
	 * @returns True iff the current matrix is a submatrix of the given matrix.
	 */
	bool isSubmatrixOf(SparseMatrix<T> const& matrix) const {
        // Check for matching sizes.
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
				if (!(elem2 < matrix.rowIndications[row + 1]) || columnIndications[elem] != matrix.columnIndications[elem2]) {
                    return false;
                }
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

	/*!
	 * Calculates a hash over all values contained in this Sparse Matrix.
	 * @return size_t A Hash Value
	 */
	std::size_t getHash() const {
		std::size_t result = 0;

		boost::hash_combine(result, rowCount);
		boost::hash_combine(result, colCount);
		boost::hash_combine(result, nonZeroEntryCount);
		boost::hash_combine(result, currentSize);
		boost::hash_combine(result, lastRow);
		boost::hash_combine(result, storm::utility::Hash<T>::getHash(valueStorage));
		boost::hash_combine(result, storm::utility::Hash<uint_fast64_t>::getHash(columnIndications));
		boost::hash_combine(result, storm::utility::Hash<uint_fast64_t>::getHash(rowIndications));

		return result;
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
	 * Prepares the internal storage. It relies on the number of non-zero entries and the
	 * amount of rows to be set correctly. They may, however, be zero, but then insertNextValue needs to be used rather
     * than addNextElement for filling the matrix.
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

#ifdef STORM_HAVE_INTELTBB
	/*!
	 *	This function is a helper for Parallel Execution of the multipliyWithVector functionality.
	 *  It uses Intels TBB parallel_for paradigm to split up the row/vector multiplication and summation
	 */
	template <typename M, typename V, typename T>
	class tbbHelper_MatrixRowVectorScalarProduct {
	private:
		V * resultVector;
		V const* vectorX;
		M const* matrixA;

	public:
		tbbHelper_MatrixRowVectorScalarProduct(M const* matrixA, V const* vectorX, V * resultVector) : matrixA(matrixA), vectorX(vectorX), resultVector(resultVector) {}

		void operator() (const tbb::blocked_range<uint_fast64_t>& r) const {
			for (uint_fast64_t row = r.begin(); row < r.end(); ++row) {
				uint_fast64_t index = matrixA->rowIndications.at(row);
				uint_fast64_t indexEnd = matrixA->rowIndications.at(row + 1);
				
				// Initialize the result to be 0.
				T element = storm::utility::constGetZero<T>();
				
				for (; index != indexEnd; ++index) {
					element += matrixA->valueStorage.at(index) * vectorX->at(matrixA->columnIndications.at(index));
				}

				// Write back to the result Vector
				resultVector->at(row) = element;
			}
		}
	
	};
#endif


} // namespace storage
} // namespace storm

#endif // STORM_STORAGE_SPARSEMATRIX_H_
