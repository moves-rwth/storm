#ifndef STORM_STORAGE_SPARSEMATRIX_H_
#define STORM_STORAGE_SPARSEMATRIX_H_

#include <exception>
#include <new>
#include <algorithm>
#include <iostream>
#include <iterator>
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
	namespace adapters{ 
		class GmmxxAdapter; 
		class EigenAdapter; 
		class StormAdapter;
	} 
}

namespace storm {
namespace storage {

/*!
 * A sparse matrix class with a constant number of non-zero entries.
 * NOTE: Addressing *is* zero-based, so the valid range for getValue and addNextValue is 0..(rows - 1)
 * where rows is the first argument to the constructor.
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
	 * If we only want to iterate over the columns of the non-zero entries of
	 * a row, we can simply iterate over the array (part) itself.
	 */
	typedef const uint_fast64_t * const constIndexIterator;

	/*!
	 *	Iterator type if we want to iterate over elements.
	 */
	typedef const T* const constIterator;

	class constRowsIterator {
	public:
		constRowsIterator(SparseMatrix<T> const& matrix) : matrix(matrix), offset(0) {
			// Intentionally left empty.
		}

		constRowsIterator& operator++() {
			++offset;
			return *this;
		}

		bool operator==(constRowsIterator const& other) const {
			return offset == other.offset;
		}

		bool operator!=(uint_fast64_t offset) const {
			return this->offset != offset;
		}

		uint_fast64_t column() const {
			return matrix.columnIndications[offset];
		}

		T const& value() const {
			return matrix.valueStorage[offset];
		}

		void setOffset(uint_fast64_t offset) {
			this->offset = offset;
		}

	private:
		SparseMatrix<T> const& matrix;
		uint_fast64_t offset;
	};

	friend class constRowsIterator;

	/*!
	 * An enum representing the internal state of the Matrix.
	 * After creating the Matrix using the Constructor, the Object is in state UnInitialized. After calling initialize(), that state changes to Initialized and after all entries have been entered and finalize() has been called, to ReadReady.
	 * Should a critical error occur in any of the former functions, the state will change to Error.
	 * @see getState()
	 * @see isReadReady()
	 * @see isInitialized()
	 * @see hasError()
	 */
	enum MatrixStatus {
		Error = -1, UnInitialized = 0, Initialized = 1, ReadReady = 2
	};

	//! Constructor
	/*!
	 * Constructs a sparse matrix object with the given number of rows.
	 * @param rows The number of rows of the matrix
	 */
	SparseMatrix(uint_fast64_t rows, uint_fast64_t cols)
			: rowCount(rows), colCount(cols), nonZeroEntryCount(0),
			  internalStatus(MatrixStatus::UnInitialized), currentSize(0), lastRow(0) { }

	/* Sadly, Delegate Constructors are not yet available with MSVC2012 */
	//! Constructor
	/*!
	 * Constructs a square sparse matrix object with the given number rows
	 * @param size The number of rows and cols in the matrix
	 */ /*
	SparseMatrix(uint_fast64_t size) : SparseMatrix(size, size) { }
	*/

	//! Constructor
	/*!
	 * Constructs a square sparse matrix object with the given number rows
	 * @param size The number of rows and cols in the matrix
	 */
	SparseMatrix(uint_fast64_t size)
			: rowCount(size), colCount(size), nonZeroEntryCount(0),
			  internalStatus(MatrixStatus::UnInitialized), currentSize(0), lastRow(0) { }

	//! Copy Constructor
	/*!
	 * Copy Constructor. Performs a deep copy of the given sparse matrix.
	 * @param ssm A reference to the matrix to be copied.
	 */
	SparseMatrix(const SparseMatrix<T> &ssm)
			: rowCount(ssm.rowCount), colCount(ssm.colCount), nonZeroEntryCount(ssm.nonZeroEntryCount),
			  internalStatus(ssm.internalStatus), currentSize(ssm.currentSize), lastRow(ssm.lastRow) {
		LOG4CPLUS_WARN(logger, "Invoking copy constructor.");
		// Check whether copying the matrix is safe.
		if (ssm.hasError()) {
			LOG4CPLUS_ERROR(logger, "Trying to copy sparse matrix in error state.");
			throw storm::exceptions::InvalidArgumentException("Trying to copy sparse matrix in error state.");
		} else {
			// Try to prepare the internal storage and throw an error in case
			// of a failure.
			if (!prepareInternalStorage()) {
				LOG4CPLUS_ERROR(logger, "Unable to allocate internal storage.");
				throw std::bad_alloc();
			} else {
				std::copy(ssm.valueStorage.begin(), ssm.valueStorage.end(), valueStorage.begin());

				// The elements that are not of the value type but rather the
				// index type may be copied directly.
				std::copy(ssm.columnIndications.begin(), ssm.columnIndications.end(), columnIndications.begin());
				std::copy(ssm.rowIndications.begin(), ssm.rowIndications.end(), rowIndications.begin());
			}
		}
	}

	//! Destructor
	/*!
	 * Destructor. Performs deletion of the reserved storage arrays.
	 */
	~SparseMatrix() {
		setState(MatrixStatus::UnInitialized);
		valueStorage.resize(0);
		columnIndications.resize(0);
		rowIndications.resize(0);
	}

	/*!
	 * Initializes the sparse matrix with the given number of non-zero entries
	 * and prepares it for use with addNextValue() and finalize().
	 * NOTE: Calling this method before any other member function is mandatory.
	 * This version is to be used together with addNextValue().
	 * @param nonZeroEntries The number of non-zero entries that are not on the
	 * diagonal.
	 */
	void initialize(uint_fast64_t nonZeroEntries) {
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
	 * Sets the matrix element at the given row and column to the given value.
	 * NOTE: This is a linear setter. It must be called consecutively for each element,
	 * row by row *and* column by column. Only diagonal entries may be set at any time.
	 * @param row The row in which the matrix element is to be set.
	 * @param col The column in which the matrix element is to be set.
	 * @param value The value that is to be set.
	 */
	void addNextValue(const uint_fast64_t row, const uint_fast64_t col,	const T& value) {
		// Check whether the given row and column positions are valid and throw
		// error otherwise.
		if ((row > rowCount) || (col > colCount)) {
			triggerErrorState();
			LOG4CPLUS_ERROR(logger, "Trying to add a value at illegal position (" << row << ", " << col << ").");
			throw storm::exceptions::OutOfRangeException("Trying to add a value at illegal position.");
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

	/*
	 * Finalizes the sparse matrix to indicate that initialization has been
	 * completed and the matrix may now be used.
	 */
	void finalize() {
		// Check whether it's safe to finalize the matrix and throw error
		// otherwise.
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

			// Set a sentinel element at the last position of the row_indications
			// array. This eases iteration work, as now the indices of row i
			// are always between row_indications[i] and row_indications[i + 1],
			// also for the first and last row.
			rowIndications[rowCount] = nonZeroEntryCount;

			setState(MatrixStatus::ReadReady);
		}
	}

	/*!
	 * Gets the matrix element at the given row and column to the given value.
	 * NOTE: This function does not check the internal status for errors for performance reasons.
	 * @param row The row in which the element is to be read.
	 * @param col The column in which the element is to be read.
	 * @param target A pointer to the memory location where the read content is
	 * to be put.
	 * @return True iff the value is set in the matrix, false otherwise.
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
	 * Gets the matrix element at the given row and column to the given value.
	 * NOTE: This function does not check the internal status for errors for performance reasons.
	 * WARNING: It is possible to modify through this function. Usage only valid
	 * for elements EXISTING in the sparse matrix! If the requested value does not exist,
	 * an exception will be thrown.
	 * @param row The row in which the element is to be read.
	 * @param col The column in which the element is to be read.
	 *
	 * @return A reference to the value
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
			// If the lement is found, write the content to the specified
			// position and return true.
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
	 */
	uint_fast64_t getRowCount() const {
		return rowCount;
	}

	/*!
	 * Returns the number of columns of the matrix.
	 */
	uint_fast64_t getColumnCount() const {
		return colCount;
	}

	/*!
	 * Returns a pointer to the value storage of the matrix.
	 * @return A pointer to the value storage of the matrix.
	 */
	std::vector<T> const& getStorage() const {
		return valueStorage;
	}

	/*!
	 * Returns a pointer to the array that stores the start indices of non-zero
	 * entries in the value storage for each row.
	 * @return A pointer to the array that stores the start indices of non-zero
	 * entries in the value storage for each row.
	 */
	std::vector<uint_fast64_t> const& getRowIndications() const {
		return rowIndications;
	}

	/*!
	 * Returns a pointer to an array that stores the column of each non-zero
	 * element.
	 * @return A pointer to an array that stores the column of each non-zero
	 * element.
	 */
	std::vector<uint_fast64_t> const& getColumnIndications() const {
		return columnIndications;
	}

	/*!
	 * Checks whether the internal status of the matrix makes it ready for
	 * reading access.
	 * @return True iff the internal status of the matrix makes it ready for
	 * reading access.
	 */
	bool isReadReady() {
		return (internalStatus == MatrixStatus::ReadReady);
	}

	/*!
	 * Checks whether the matrix was initialized previously. The matrix may
	 * still require to be finalized, even if this check returns true.
	 * @return True iff the matrix was initialized previously.
	 */
	bool isInitialized() {
		return (internalStatus == MatrixStatus::Initialized || internalStatus == MatrixStatus::ReadReady);
	}

	/*!
	 * Returns the internal state of the matrix.
	 * @return The internal state of the matrix.
	 */
	MatrixStatus getState() {
		return internalStatus;
	}

	/*!
	 * Checks whether the internal state of the matrix signals an error.
	 * @return True iff the internal state of the matrix signals an error.
	 */
	bool hasError() const {
		return (internalStatus == MatrixStatus::Error);
	}

	/*!
	 * Returns the number of non-zero entries that are not on the diagonal.
	 * @returns The number of non-zero entries that are not on the diagonal.
	 */
	uint_fast64_t getNonZeroEntryCount() const {
		return nonZeroEntryCount;
	}

	/*!
	 * This function makes the rows given by the bit vector absorbing.
	 * @param rows A bit vector indicating which rows to make absorbing.
	 * @return True iff the operation was successful.
	 */
	bool makeRowsAbsorbing(const storm::storage::BitVector rows) {
		bool result = true;
		for (auto row : rows) {
			result &= makeRowAbsorbing(row, row);
		}

		return result;
	}

	/*!
	 * This function makes the groups of rows given by the bit vector absorbing.
	 * @param rows A bit vector indicating which row groups to make absorbing.
	 * @return True iff the operation was successful.
	 */
	bool makeRowsAbsorbing(const storm::storage::BitVector rows, std::vector<uint_fast64_t> const& nondeterministicChoices) {
		bool result = true;
		for (auto index : rows) {
			for (uint_fast64_t row = nondeterministicChoices[index]; row < nondeterministicChoices[index + 1]; ++row) {
				result &= makeRowAbsorbing(row, index);
			}
		}

		return result;
	}


	/*!
	 * This function makes the given row absorbing. This means that all
	 * entries in will be set to 0 and the value 1 will be written
	 * to the element on the (pseudo-) diagonal.
	 * @param row The row to be made absorbing.
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

		if (rowStart >= rowEnd) {
			LOG4CPLUS_ERROR(logger, "Cannot make row " << row << " absorbing, because there is no entry in this row.");
			throw storm::exceptions::InvalidStateException() << "Cannot make row " << row << " absorbing, because there is no entry in this row.";
		}

		valueStorage[rowStart] = storm::utility::constGetOne<T>();
		columnIndications[rowStart] = column;

		for (uint_fast64_t index = rowStart + 1; index < rowEnd; ++index) {
			valueStorage[index] = storm::utility::constGetZero<T>();
			columnIndications[index] = 0;
		}

		return true;
	}

	/*
	 * Computes the sum of the elements in the given row whose column bits
	 * are set to one on the given constraint.
	 * @param row The row whose elements to add.
	 * @param constraint A bit vector that indicates which columns to add.
	 * @return The sum of the elements in the given row whose column bits
	 * are set to one on the given constraint.
	 */
	T getConstrainedRowSum(const uint_fast64_t row, const storm::storage::BitVector& constraint) const {
		T result(0);
		for (uint_fast64_t i = rowIndications[row]; i < rowIndications[row + 1]; ++i) {
			if (constraint.get(columnIndications[i])) {
				result += valueStorage[i];
			}
		}
		return result;
	}

	/*!
	 * Computes a vector in which each element is the sum of those elements in the
	 * corresponding row whose column bits are set to one in the given constraint.
	 * @param rowConstraint A bit vector that indicates for which rows to perform summation.
	 * @param columnConstraint A bit vector that indicates which columns to add.
	 * @param resultVector A pointer to the resulting vector that has at least
	 * as many elements as there are bits set to true in the constraint.
	 */
	void getConstrainedRowSumVector(const storm::storage::BitVector& rowConstraint, const storm::storage::BitVector& columnConstraint, std::vector<T>* resultVector) const {
		uint_fast64_t currentRowCount = 0;
		for (auto row : rowConstraint) {
			(*resultVector)[currentRowCount++] = getConstrainedRowSum(row, columnConstraint);
		}
	}

	/*!
	 * Computes a vector in which each element is the sum of those elements in the
	 * corresponding row whose column bits are set to one in the given constraint.
	 * @param rowConstraint A bit vector that indicates for which rows to perform summation.
	 * @param columnConstraint A bit vector that indicates which columns to add.
	 * @param resultVector A pointer to the resulting vector that has at least
	 * as many elements as there are bits set to true in the constraint.
	 */
	void getConstrainedRowSumVector(const storm::storage::BitVector& rowConstraint, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, const storm::storage::BitVector& columnConstraint, std::vector<T>* resultVector) const {
		uint_fast64_t currentRowCount = 0;
		for (auto index : rowConstraint) {
			for (uint_fast64_t row = nondeterministicChoiceIndices[index]; row < nondeterministicChoiceIndices[index + 1]; ++row) {
				(*resultVector)[currentRowCount++] = getConstrainedRowSum(row, columnConstraint);
			}
		}
	}

	/*!
	 * Creates a sub-matrix of the current matrix by dropping all rows and
	 * columns whose bits are not set to one in the given bit vector.
	 * @param constraint A bit vector indicating which rows and columns to drop.
	 * @return A pointer to a sparse matrix that is a sub-matrix of the current one.
	 */
	SparseMatrix* getSubmatrix(storm::storage::BitVector& constraint) const {
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
		SparseMatrix* result = new SparseMatrix(constraint.getNumberOfSetBits());
		result->initialize(subNonZeroEntries);

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
					result->addNextValue(rowCount, bitsSetBeforeIndex[columnIndications[i]], valueStorage[i]);
				}
			}

			++rowCount;
		}

		// Dispose of the temporary array.
		delete[] bitsSetBeforeIndex;

		// Finalize sub-matrix and return result.
		result->finalize();
		LOG4CPLUS_DEBUG(logger, "Done creating sub-matrix.");
		return result;
	}

	SparseMatrix* getSubmatrix(storm::storage::BitVector& constraint, std::vector<uint_fast64_t> const& rowIndices) const {
		LOG4CPLUS_DEBUG(logger, "Creating a sub-matrix (of unknown size).");

		// Check for valid constraint.
		if (constraint.getNumberOfSetBits() == 0) {
			LOG4CPLUS_ERROR(logger, "Trying to create a sub-matrix of size 0.");
			throw storm::exceptions::InvalidArgumentException("Trying to create a sub-matrix of size 0.");
		}

		// First, we need to determine the number of non-zero entries and the number of rows of the
		// sub-matrix.
		uint_fast64_t subNonZeroEntries = 0;
		uint_fast64_t subRowCount = 0;
		for (auto index : constraint) {
			subRowCount += rowIndices[index + 1] - rowIndices[index];
			for (uint_fast64_t i = rowIndices[index]; i < rowIndices[index + 1]; ++i) {
				for (uint_fast64_t j = rowIndications[i]; j < rowIndications[i + 1]; ++j) {
					if (constraint.get(columnIndications[j])) {
						++subNonZeroEntries;
					}
				}
			}
		}

		LOG4CPLUS_DEBUG(logger, "Determined size of submatrix to be " << subRowCount << "x" << constraint.getNumberOfSetBits() << ".");

		// Create and initialize resulting matrix.
		SparseMatrix* result = new SparseMatrix(subRowCount, constraint.getNumberOfSetBits());
		result->initialize(subNonZeroEntries);

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
		for (auto index : constraint) {
			for (uint_fast64_t i = rowIndices[index]; i < rowIndices[index + 1]; ++i) {
				for (uint_fast64_t j = rowIndications[i]; j < rowIndications[i + 1]; ++j) {
					if (constraint.get(columnIndications[j])) {
						result->addNextValue(rowCount, bitsSetBeforeIndex[columnIndications[j]], valueStorage[j]);
					}
				}
				++rowCount;
			}
		}

		// Dispose of the temporary array.
		delete[] bitsSetBeforeIndex;

		// Finalize sub-matrix and return result.
		result->finalize();
		LOG4CPLUS_DEBUG(logger, "Done creating sub-matrix.");
		return result;
	}

	void convertToEquationSystem() {
		invertDiagonal();
		negateAllNonDiagonalElements();
	}

	/*!
	 * Inverts all elements on the diagonal, i.e. sets the diagonal values to 1 minus their previous
	 * value.
	 * Requires the matrix to contain each diagonal element AND to be square!
	 */
	void invertDiagonal() {
		if (this->getRowCount() != this->getColumnCount()) {
			throw storm::exceptions::InvalidArgumentException() << "SparseMatrix::invertDiagonal requires the Matrix to be square!";
		}
		T one = storm::utility::constGetOne<T>();
		bool foundRow;
		for (uint_fast64_t row = 0; row < rowCount; ++row) {
			uint_fast64_t rowStart = rowIndications[row];
			uint_fast64_t rowEnd = rowIndications[row + 1];
			foundRow = false;
			while (rowStart < rowEnd) {
				if (columnIndications[rowStart] == row) {
					valueStorage[rowStart] = one - valueStorage[rowStart];
					foundRow = true;
					break;
				}
				++rowStart;
			}
			if (!foundRow) {
				throw storm::exceptions::InvalidArgumentException() << "SparseMatrix::invertDiagonal requires the Matrix to contain all diagonal entries!";
			}
		}
	}

	/*!
	 * Negates all non-zero elements that are not on the diagonal.
	 */
	void negateAllNonDiagonalElements() {
		if (this->getRowCount() != this->getColumnCount()) {
			throw storm::exceptions::InvalidArgumentException() << "SparseMatrix::invertDiagonal requires the Matrix to be square!";
		}
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
			throw storm::exceptions::InvalidArgumentException() << "SparseMatrix::getJacobiDecomposition requires the Matrix to be square!";
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
	 * @param otherMatrix A reference to the matrix with which to perform the pointwise multiplication.
	 * This matrix must be a submatrix of the current matrix in the sense that it may not have
	 * non-zero entries at indices where there is a zero in the current matrix.
	 * @return A vector containing the sum of the elements in each row of the matrix resulting from
	 * pointwise multiplication of the current matrix with the given matrix.
	 */
	std::vector<T>* getPointwiseProductRowSumVector(storm::storage::SparseMatrix<T> const& otherMatrix) {
		// Prepare result.
		std::vector<T>* result = new std::vector<T>(rowCount, storm::utility::constGetZero<T>());

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
					(*result)[row] += otherMatrix.valueStorage[nextOtherElement] * valueStorage[element];
					++nextOtherElement;
				}
			}
		}

		return result;
	}

	T multiplyRowWithVector(constRowsIterator& rowsIt, uint_fast64_t rowsIte, std::vector<T>& vector) const {
		T result = storm::utility::constGetZero<T>();
		for (; rowsIt != rowsIte; ++rowsIt) {
			result += (rowsIt.value()) * vector[rowsIt.column()];
		}
		return result;
	}

	void multiplyWithVector(std::vector<T>& vector, std::vector<T>& result) const {
		typename std::vector<T>::iterator resultIt = result.begin();
		typename std::vector<T>::iterator resultIte = result.end();
		constRowsIterator rowIt = this->constRowsIteratorBegin();
		uint_fast64_t nextRow = 1;

		for (; resultIt != resultIte; ++resultIt, ++nextRow) {
			*resultIt = multiplyRowWithVector(rowIt, this->rowIndications[nextRow], vector);
		}
	}

	void multiplyWithVector(std::vector<uint_fast64_t> const& states, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, std::vector<T>& vector, std::vector<T>& result) const {
		constRowsIterator rowsIt = this->constRowsIteratorBegin();
		uint_fast64_t nextRow = 1;

		for (auto stateIt = states.cbegin(), stateIte = states.cend(); stateIt != stateIte; ++stateIt) {
			rowsIt.setOffset(this->rowIndications[nondeterministicChoiceIndices[*stateIt]]);
			nextRow = nondeterministicChoiceIndices[*stateIt] + 1;
			for (auto rowIt = nondeterministicChoiceIndices[*stateIt], rowIte = nondeterministicChoiceIndices[*stateIt + 1]; rowIt != rowIte; ++rowIt, ++nextRow) {
				result[rowIt] = multiplyRowWithVector(rowsIt, this->rowIndications[nextRow], vector);
			}
		}
	}

	/*!
	 * Returns the size of the matrix in memory measured in bytes.
	 * @return The size of the matrix in memory measured in bytes.
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

	constRowsIterator constRowsIteratorBegin() const {
		return constRowsIterator(*this);
	}

	/*!
	 * Returns an iterator to the columns of the non-zero entries of the given
	 * row.
	 * @param row The row whose columns the iterator will return.
	 * @return An iterator to the columns of the non-zero entries of the given
	 * row.
	 */
	constIndexIterator beginConstColumnIterator(uint_fast64_t row) const {
		return &(this->columnIndications[0]) + this->rowIndications[row];
	}

	/*!
	 * Returns an iterator referring to the element after the given row.
	 * @param row The row for which the iterator should point to the past-the-end
	 * element.
	 */
	constIndexIterator endConstColumnIterator(uint_fast64_t row) const {
		return &(this->columnIndications[0]) + this->rowIndications[row + 1];
	}

	/*!
	 *	Returns an iterator over the elements of the given row. The iterator
	 *	will include no zero entries.
	 *	@param row The row whose elements the iterator will return.
	 *	@return An iterator over the elements of the given row.
	 */
	constIterator beginConstIterator(uint_fast64_t row) const {
		return &(this->valueStorage[0]) + this->rowIndications[row];
	}
	/*!
	 *	Returns an iterator pointing to the first element after the given
	 *	row.
	 *	@param row The row for which the iterator should point to the
	 *	past-the-end element.
	 *	@return An iterator to the element after the given row.
	 */
	constIterator endConstIterator(uint_fast64_t row) const {
		return &(this->valueStorage[0]) + this->rowIndications[row + 1];
	}

	/*!
	 *	@brief Calculate sum of all entries in given row.
	 *
	 *	Adds up all values in the given row
	 *	and returns the sum.
	 *	@param row The row that should be added up.
	 *	@return Sum of the row.
	 */
	T getRowSum(uint_fast64_t row) const {
		T sum = storm::utility::constGetZero<T>();
		for (auto it = this->beginConstIterator(row); it != this->endConstIterator(row); it++) {
			sum += *it;
		}
		return sum;
	}

	/*!
	 *	@brief Checks if it is a submatrix of the given matrix.
	 *
	 *	A matrix A is a submatrix of B if a value in A is only nonzero, if
	 *	the value in B at the same position is also nonzero. Furthermore, A
	 *	and B have to have the same size.
	 *	@param matrix Matrix to check against.
	 *	@return True iff this is a submatrix of matrix.
	 */
	bool isSubmatrixOf(SparseMatrix<T> const& matrix) const {
		if (this->getRowCount() != matrix.getRowCount()) return false;
		if (this->getColumnCount() != matrix.getColumnCount()) return false;

		/*
		for (uint_fast64_t row = 0; row < this->getRowCount(); ++row) {
			for (uint_fast64_t elem = rowIndications[row], elem2 = matrix.rowIndications[row]; elem < rowIndications[row + 1] && elem < matrix.rowIndications[row + 1]; ++elem, ++elem2) {
				if (columnIndications[elem] < matrix.columnIndications[elem2]) return false;
			}
		}
		*/

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
	 * @return a compressed string representation of the matrix.
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
	 * Note: the matrix is presented densely. That is, all zeros are filled in and are part of the string
	 * representation.
	 * @param nondeterministicChoiceIndices A vector indicating which rows belong together. If given, rows belonging
	 * to separate groups will be separated by a dashed line.
	 * @return a (non-compressed) string representation of the matrix.
	 */
	std::string toString(std::vector<uint_fast64_t> const* nondeterministicChoiceIndices) const {
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

			// If we need to group of rows, print a dashed line in case we have moved to the next group of rows.
			if (nondeterministicChoiceIndices != nullptr) {
				if (i == (*nondeterministicChoiceIndices)[currentNondeterministicChoiceIndex]) {
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
	 * Prepares the internal CSR storage. For this, it requires
	 * non_zero_entry_count and row_count to be set correctly.
	 * @param alsoPerformAllocation If set to true, all entries are pre-allocated. This is the default.
	 * @return True on success, false otherwise (allocation failed).
	 */
	bool prepareInternalStorage(const bool alsoPerformAllocation) {
		if (alsoPerformAllocation) {
			// Set up the arrays for the elements that are not on the diagonal.
			valueStorage.resize(nonZeroEntryCount, storm::utility::constGetZero<T>());
			columnIndications.resize(nonZeroEntryCount, 0);

			// Set up the row_indications vector and reserve one element more than
			// there are rows in order to put a sentinel element at the end,
			// which eases iteration process.
			rowIndications.resize(rowCount + 1, 0);

			// Return whether all the allocations could be made without error.
			return ((valueStorage.capacity() >= nonZeroEntryCount) && (columnIndications.capacity() >= nonZeroEntryCount)
				&& (rowIndications.capacity() >= (rowCount + 1)));
		} else {
			valueStorage.reserve(nonZeroEntryCount);
			columnIndications.reserve(nonZeroEntryCount);
			rowIndications.reserve(rowCount + 1);
			return true;
		}
	}

	/*!
	 * Shorthand for prepareInternalStorage(true)
	 * @see prepareInternalStorage(const bool)
	 */
	bool prepareInternalStorage() {
		return this->prepareInternalStorage(true);
	}

};

} // namespace storage

} // namespace storm

#endif // STORM_STORAGE_SPARSEMATRIX_H_
