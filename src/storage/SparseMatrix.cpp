/*
 * SparseMatrix.cpp
 *
 *  Created on: Nov 18, 2013
 *      Author: Manuel Sascha Weiand
 */

#include <iomanip>
#include <boost/functional/hash.hpp>

#include "src/storage/SparseMatrix.h"
#include "src/exceptions/InvalidStateException.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

namespace storm {
namespace storage {

	// Functions of the nested ConstIterator class.

	template<typename T>
	SparseMatrix<T>::ConstIterator::ConstIterator(T const* valuePtr, uint_fast64_t const* columnPtr) : valuePtr(valuePtr), columnPtr(columnPtr) {
		// Intentionally left empty.
	}

	template<typename T>
	typename SparseMatrix<T>::ConstIterator& SparseMatrix<T>::ConstIterator::operator++() {
		this->valuePtr++;
		this->columnPtr++;
		return *this;
	}

	template<typename T>
	typename SparseMatrix<T>::ConstIterator& SparseMatrix<T>::ConstIterator::operator*() {
		return *this;
	}

	template<typename T>
	bool SparseMatrix<T>::ConstIterator::operator!=(ConstIterator const& other) const {
		return this->valuePtr != other.valuePtr;
	}

	template<typename T>
	typename SparseMatrix<T>::ConstIterator& SparseMatrix<T>::ConstIterator::operator=(ConstIterator const& other) {
		this->valuePtr = other.valuePtr;
		this->columnPtr = other.columnPtr;
		return *this;
	}

	template<typename T>
	uint_fast64_t SparseMatrix<T>::ConstIterator::column() const {
		return *columnPtr;
	}

	template<typename T>
	T const& SparseMatrix<T>::ConstIterator::value() const {
		return *valuePtr;
	}


	// Functions of the nested Rows class.

	template<typename T>
	SparseMatrix<T>::Rows::Rows(T const* valuePtr, uint_fast64_t const* columnPtr, uint_fast64_t entryCount) : valuePtr(valuePtr), columnPtr(columnPtr), entryCount(entryCount) {
		// Intentionally left empty.
	}

	template<typename T>
	typename SparseMatrix<T>::ConstIterator SparseMatrix<T>::Rows::begin() const {
		return ConstIterator(valuePtr, columnPtr);
	}

	template<typename T>
	typename SparseMatrix<T>::ConstIterator SparseMatrix<T>::Rows::end() const {
		return ConstIterator(valuePtr + entryCount, columnPtr + entryCount);
	}


	// Functions of the nested ConstRowsIterator class.

	template<typename T>
	SparseMatrix<T>::ConstRowIterator::ConstRowIterator(T const* startValuePtr, uint_fast64_t const* startColumnPtr, uint_fast64_t const* rowPtr) : startValuePtr(startValuePtr), startColumnPtr(startColumnPtr), rowPtr(rowPtr) {
		// Intentionally left empty.
	}

	template<typename T>
	typename SparseMatrix<T>::ConstRowIterator& SparseMatrix<T>::ConstRowIterator::operator++() {
		++rowPtr;
		return *this;
	}

	template<typename T>
	bool SparseMatrix<T>::ConstRowIterator::operator!=(ConstRowIterator const& other) const {
		return this->rowPtr != other.rowPtr;
	}

	template<typename T>
	typename SparseMatrix<T>::ConstIterator SparseMatrix<T>::ConstRowIterator::begin() const {
		return ConstIterator(startValuePtr + *rowPtr, startColumnPtr + *rowPtr);
	}

	template<typename T>
	typename SparseMatrix<T>::ConstIterator SparseMatrix<T>::ConstRowIterator::end() const {
		return ConstIterator(startValuePtr + *(rowPtr + 1), startColumnPtr + *(rowPtr + 1));
    }


	// Functions of the SparseMatrix class.

	template<typename T>
	SparseMatrix<T>::SparseMatrix(uint_fast64_t rows, uint_fast64_t cols) : rowCount(rows), colCount(cols),
		nonZeroEntryCount(0), internalStatus(MatrixStatus::UnInitialized), currentSize(0), lastRow(0) {
        // Intentionally left empty.
    }

	template<typename T>
	SparseMatrix<T>::SparseMatrix(uint_fast64_t size)
			: rowCount(size), colCount(size), nonZeroEntryCount(0),
			  internalStatus(MatrixStatus::UnInitialized), currentSize(0), lastRow(0) {
        // Intentionally left empty.
	}

	template<typename T>
	SparseMatrix<T>::SparseMatrix(SparseMatrix<T>&& other)
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

	template<typename T>
	SparseMatrix<T>::SparseMatrix(const SparseMatrix<T> & other)
		: rowCount(other.rowCount), colCount(other.colCount), nonZeroEntryCount(other.nonZeroEntryCount),
		valueStorage(other.valueStorage), columnIndications(other.columnIndications),
		rowIndications(other.rowIndications), internalStatus(other.internalStatus),
		currentSize(other.currentSize), lastRow(other.lastRow) {
	}

	template<typename T>
	SparseMatrix<T>::SparseMatrix(uint_fast64_t rowCount, uint_fast64_t colCount, uint_fast64_t nonZeroEntryCount,
                 std::vector<uint_fast64_t>&& rowIndications,
                 std::vector<uint_fast64_t>&& columnIndications, std::vector<T>&& values)
                    : rowCount(rowCount), colCount(colCount), nonZeroEntryCount(nonZeroEntryCount),
                    valueStorage(values), columnIndications(columnIndications),
                    rowIndications(rowIndications), internalStatus(MatrixStatus::Initialized),
                    currentSize(0), lastRow(0) {
        // Intentionally left empty.
    }

	template<typename T>
	storm::storage::SparseMatrix<T>& SparseMatrix<T>::operator=(SparseMatrix<T> const& other) {
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

	template<typename T>
	void SparseMatrix<T>::initialize(uint_fast64_t nonZeroEntries) {
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

	template<typename T>
	void SparseMatrix<T>::addNextValue(const uint_fast64_t row, const uint_fast64_t col, T const& value) {
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

	template<typename T>
    void SparseMatrix<T>::insertNextValue(const uint_fast64_t row, const uint_fast64_t col,	T const& value, bool pushRowIndication) {
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

	template<typename T>
    void SparseMatrix<T>::insertEmptyRow(bool pushRowIndication) {
        if (pushRowIndication) {
            rowIndications.push_back(currentSize);
        } else {
            rowIndications[lastRow + 1] = currentSize;
        }

        ++rowCount;
        ++lastRow;
    }

	template<typename T>
	void SparseMatrix<T>::finalize(bool pushSentinelElement) {
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

	template<typename T>
	inline bool SparseMatrix<T>::getValue(uint_fast64_t row, uint_fast64_t col, T* const target) const {
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

	template<typename T>
	inline T& SparseMatrix<T>::getValue(uint_fast64_t row, uint_fast64_t col) {
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

	template<typename T>
	uint_fast64_t SparseMatrix<T>::getRowCount() const {
		return rowCount;
	}

	template<typename T>
	uint_fast64_t SparseMatrix<T>::getColumnCount() const {
		return colCount;
	}

	template<typename T>
	bool SparseMatrix<T>::isReadReady() {
		return (internalStatus == MatrixStatus::ReadReady);
	}

	template<typename T>
	bool SparseMatrix<T>::isInitialized() {
		return (internalStatus == MatrixStatus::Initialized || internalStatus == MatrixStatus::ReadReady);
	}

	template<typename T>
	typename SparseMatrix<T>::MatrixStatus SparseMatrix<T>::getState() {
		return internalStatus;
	}

	template<typename T>
	bool SparseMatrix<T>::hasError() const {
		return (internalStatus == MatrixStatus::Error);
	}

	template<typename T>
	uint_fast64_t SparseMatrix<T>::getNonZeroEntryCount() const {
		return nonZeroEntryCount;
	}

	template<typename T>
	bool SparseMatrix<T>::makeRowsAbsorbing(storm::storage::BitVector const& rows) {
		bool result = true;
		for (auto row : rows) {
			result &= makeRowAbsorbing(row, row);
		}
		return result;
	}

	template<typename T>
	bool SparseMatrix<T>::makeRowsAbsorbing(storm::storage::BitVector const& rowGroupConstraint, std::vector<uint_fast64_t> const& rowGroupIndices) {
		bool result = true;
		for (auto rowGroup : rowGroupConstraint) {
			for (uint_fast64_t row = rowGroupIndices[rowGroup]; row < rowGroupIndices[rowGroup + 1]; ++row) {
				result &= makeRowAbsorbing(row, rowGroup);
			}
		}
		return result;
	}

	template<typename T>
	bool SparseMatrix<T>::makeRowAbsorbing(const uint_fast64_t row, const uint_fast64_t column) {
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
		valueStorage[rowStart] = storm::utility::constantOne<T>();
		columnIndications[rowStart] = column;
		for (uint_fast64_t index = rowStart + 1; index < rowEnd; ++index) {
			valueStorage[index] = storm::utility::constantZero<T>();
			columnIndications[index] = 0;
		}

		return true;
	}

	template<typename T>
	T SparseMatrix<T>::getConstrainedRowSum(uint_fast64_t row, storm::storage::BitVector const& constraint) const {
		T result(0);
		for (uint_fast64_t i = rowIndications[row]; i < rowIndications[row + 1]; ++i) {
			if (constraint.get(columnIndications[i])) {
				result += valueStorage[i];
			}
		}
		return result;
	}

	template<typename T>
	std::vector<T> SparseMatrix<T>::getConstrainedRowSumVector(storm::storage::BitVector const& rowConstraint, storm::storage::BitVector const& columnConstraint) const {
		std::vector<T> result(rowConstraint.getNumberOfSetBits());
		uint_fast64_t currentRowCount = 0;
		for (auto row : rowConstraint) {
			result[currentRowCount++] = getConstrainedRowSum(row, columnConstraint);
		}
		return result;
	}

	template<typename T>
	std::vector<T> SparseMatrix<T>::getConstrainedRowSumVector(storm::storage::BitVector const& rowGroupConstraint, std::vector<uint_fast64_t> const& rowGroupIndices, storm::storage::BitVector const& columnConstraint, uint_fast64_t numberOfRows) const {
		std::vector<T> result(numberOfRows);
		uint_fast64_t currentRowCount = 0;
		for (auto rowGroup : rowGroupConstraint) {
			for (uint_fast64_t row = rowGroupIndices[rowGroup]; row < rowGroupIndices[rowGroup + 1]; ++row) {
				result[currentRowCount++] = getConstrainedRowSum(row, columnConstraint);
			}
		}
		return result;
	}

	template<typename T>
	SparseMatrix<T> SparseMatrix<T>::getSubmatrix(storm::storage::BitVector const& constraint) const {
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

	template<typename T>
    SparseMatrix<T> SparseMatrix<T>::getSubmatrix(storm::storage::BitVector const& rowGroupConstraint, std::vector<uint_fast64_t> const& rowGroupIndices, bool insertDiagonalEntries) const {
        return getSubmatrix(rowGroupConstraint, rowGroupConstraint, rowGroupIndices, insertDiagonalEntries);
	}
    
    template<typename T>
    SparseMatrix<T> SparseMatrix<T>::getSubmatrix(storm::storage::BitVector const& rowGroupConstraint, storm::storage::BitVector const& columnConstraint, std::vector<uint_fast64_t> const& rowGroupIndices, bool insertDiagonalEntries) const {
		LOG4CPLUS_DEBUG(logger, "Creating a sub-matrix (of unknown size).");
        
		// First, we need to determine the number of non-zero entries and the number of rows of the sub-matrix.
		uint_fast64_t subNonZeroEntries = 0;
		uint_fast64_t subRowCount = 0;
		for (auto index : rowGroupConstraint) {
			subRowCount += rowGroupIndices[index + 1] - rowGroupIndices[index];
			for (uint_fast64_t i = rowGroupIndices[index]; i < rowGroupIndices[index + 1]; ++i) {
                bool foundDiagonalElement = false;
                
				for (uint_fast64_t j = rowIndications[i]; j < rowIndications[i + 1]; ++j) {
					if (columnConstraint.get(columnIndications[j])) {
						++subNonZeroEntries;
                        
                        if (index == columnIndications[j]) {
                            foundDiagonalElement = true;
                        }
					}
				}
                
                if (insertDiagonalEntries && !foundDiagonalElement) {
                    ++subNonZeroEntries;
                }
			}
		}
        
		LOG4CPLUS_DEBUG(logger, "Determined size of submatrix to be " << subRowCount << "x" << rowGroupConstraint.getNumberOfSetBits() << ".");
        
		// Create and initialize resulting matrix.
		SparseMatrix result(subRowCount, columnConstraint.getNumberOfSetBits());
		result.initialize(subNonZeroEntries);
        
		// Create a temporary vector that stores for each index whose bit is set
		// to true the number of bits that were set before that particular index.
        std::vector<uint_fast64_t> bitsSetBeforeIndex;
        bitsSetBeforeIndex.reserve(colCount);
        
        // Compute the information to fill this vector.
		uint_fast64_t lastIndex = 0;
		uint_fast64_t currentNumberOfSetBits = 0;
        
        // If we are requested to add missing diagonal entries, we need to make sure the corresponding rows
        storm::storage::BitVector columnBitCountConstraint = columnConstraint;
        if (insertDiagonalEntries) {
            columnBitCountConstraint |= rowGroupConstraint;
        }
		for (auto index : columnBitCountConstraint) {
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
                bool insertedDiagonalElement = false;
                
				for (uint_fast64_t j = rowIndications[i]; j < rowIndications[i + 1]; ++j) {
					if (columnConstraint.get(columnIndications[j])) {
                        if (index == columnIndications[j]) {
                            insertedDiagonalElement = true;
                        } else if (insertDiagonalEntries && !insertedDiagonalElement && columnIndications[j] > index) {
                            result.addNextValue(rowCount, bitsSetBeforeIndex[index], storm::utility::constantZero<T>());
                            insertedDiagonalElement = true;
                        }
						result.addNextValue(rowCount, bitsSetBeforeIndex[columnIndications[j]], valueStorage[j]);
					}
				}
                if (insertDiagonalEntries && !insertedDiagonalElement) {
                    result.addNextValue(rowCount, bitsSetBeforeIndex[index], storm::utility::constantZero<T>());
                }
                
				++rowCount;
			}
		}
        
		// Finalize sub-matrix and return result.
		result.finalize();
		LOG4CPLUS_DEBUG(logger, "Done creating sub-matrix.");
		return result;
	}
    
	template<typename T>
    SparseMatrix<T> SparseMatrix<T>::getSubmatrix(std::vector<uint_fast64_t> const& rowGroupToRowIndexMapping, std::vector<uint_fast64_t> const& rowGroupIndices, bool insertDiagonalEntries) const {
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
                    submatrix.addNextValue(rowGroupIndex, rowGroupIndex, storm::utility::constantZero<T>());
                    insertedDiagonalElement = true;
                }
                submatrix.addNextValue(rowGroupIndex, columnIndications[i], valueStorage[i]);
            }
            if (insertDiagonalEntries && !insertedDiagonalElement) {
                submatrix.addNextValue(rowGroupIndex, rowGroupIndex, storm::utility::constantZero<T>());
            }
        }

        // Finalize created matrix and return result.
        submatrix.finalize();
        LOG4CPLUS_DEBUG(logger, "Done creating sub-matrix.");
        return submatrix;
    }

	template<typename T>
	void SparseMatrix<T>::convertToEquationSystem() {
		invertDiagonal();
		negateAllNonDiagonalElements();
	}

	template <typename T>
	SparseMatrix<T> SparseMatrix<T>::transpose() const {

		uint_fast64_t rowCount = this->colCount;
		uint_fast64_t colCount = this->rowCount;
		uint_fast64_t nonZeroEntryCount = this->nonZeroEntryCount;

		std::vector<uint_fast64_t> rowIndications(rowCount + 1);
		std::vector<uint_fast64_t> columnIndications(nonZeroEntryCount);
		std::vector<T> values(nonZeroEntryCount, T());

		// First, we need to count how many entries each column has.
		for (uint_fast64_t i = 0; i < this->rowCount; ++i) {
			typename storm::storage::SparseMatrix<T>::Rows rows = this->getRow(i);
			for (auto const& transition : rows) {
				if (transition.value() > 0) {
					++rowIndications[transition.column() + 1];
				}
			}
		}

		// Now compute the accumulated offsets.
		for (uint_fast64_t i = 1; i < rowCount + 1; ++i) {
			rowIndications[i] = rowIndications[i - 1] + rowIndications[i];
		}

		// Create an array that stores the index for the next value to be added for
		// each row in the transposed matrix. Initially this corresponds to the previously
		// computed accumulated offsets.
		std::vector<uint_fast64_t> nextIndices = rowIndications;

		// Now we are ready to actually fill in the values of the transposed matrix.
		for (uint_fast64_t i = 0; i < this->rowCount; ++i) {
			typename storm::storage::SparseMatrix<T>::Rows rows = this->getRow(i);
			for (auto& transition : rows) {
				if (transition.value() > 0) {
					values[nextIndices[transition.column()]] = transition.value();
					columnIndications[nextIndices[transition.column()]++] = i;
				}
			}
		}

		storm::storage::SparseMatrix<T> transposedMatrix(rowCount, colCount,
														 nonZeroEntryCount,
														 std::move(rowIndications),
														 std::move(columnIndications),
														 std::move(values));

		return transposedMatrix;
	}

	template<typename T>
	void SparseMatrix<T>::invertDiagonal() {
		// Check if the matrix is square, because only then it makes sense to perform this
		// transformation.
		if (this->getRowCount() != this->getColumnCount()) {
			throw storm::exceptions::InvalidArgumentException() << "SparseMatrix::invertDiagonal requires the Matrix to be square!";
		}

		// Now iterate over all rows and set the diagonal elements to the inverted value.
		// If there is a row without the diagonal element, an exception is thrown.
		T one = storm::utility::constantOne<T>();
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

	template<typename T>
	void SparseMatrix<T>::negateAllNonDiagonalElements() {
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

	template<typename T>
	typename std::pair<storm::storage::SparseMatrix<T>, storm::storage::SparseMatrix<T>> SparseMatrix<T>::getJacobiDecomposition() const {
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
		T constOne = storm::utility::constantOne<T>();

		// copy diagonal entries to other matrix
		for (uint_fast64_t i = 0; i < rowCount; ++i) {
			resultDinv.addNextValue(i, i, constOne / resultLU.getValue(i, i));
			resultLU.getValue(i, i) = storm::utility::constantZero<T>();
		}
		resultDinv.finalize();

		return std::make_pair(std::move(resultLU), std::move(resultDinv));
	}

	template<typename T>
	std::vector<T> SparseMatrix<T>::getPointwiseProductRowSumVector(storm::storage::SparseMatrix<T> const& otherMatrix) const {
		// Prepare result.
		std::vector<T> result(rowCount, storm::utility::constantZero<T>());

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


	template<typename T>
	void SparseMatrix<T>::multiplyWithVector(std::vector<T> const& vector, std::vector<T>& result) const {
#ifdef STORM_HAVE_INTELTBB
		tbb::parallel_for(tbb::blocked_range<uint_fast64_t>(0, result.size()), tbbHelper_MatrixRowVectorScalarProduct<storm::storage::SparseMatrix<T>, std::vector<T>, T>(this, &vector, &result));
#else
        ConstRowIterator rowIt = this->begin();

        for (auto resultIt = result.begin(), resultIte = result.end(); resultIt != resultIte; ++resultIt, ++rowIt) {
            *resultIt = storm::utility::constantZero<T>();

            for (auto elementIt = rowIt.begin(), elementIte = rowIt.end(); elementIt != elementIte; ++elementIt) {
                *resultIt += elementIt.value() * vector[elementIt.column()];
            }
        }
#endif
	}

	template<typename T>
	uint_fast64_t SparseMatrix<T>::getSizeInMemory() const {
		uint_fast64_t size = sizeof(*this);
		// Add value_storage size.
		size += sizeof(T) * valueStorage.capacity();
		// Add column_indications size.
		size += sizeof(uint_fast64_t) * columnIndications.capacity();
		// Add row_indications size.
		size += sizeof(uint_fast64_t) * rowIndications.capacity();
		return size;
	}

	template<typename T>
	typename SparseMatrix<T>::Rows SparseMatrix<T>::getRows(uint_fast64_t startRow, uint_fast64_t endRow) const {
        return Rows(this->valueStorage.data() + this->rowIndications[startRow], this->columnIndications.data() + this->rowIndications[startRow], this->rowIndications[endRow + 1] - this->rowIndications[startRow]);
    }
    
    template<typename T>
    typename SparseMatrix<T>::MutableRows SparseMatrix<T>::getMutableRows(uint_fast64_t startRow, uint_fast64_t endRow) {
        return MutableRows(this->valueStorage.data() + this->rowIndications[startRow], this->columnIndications.data() + this->rowIndications[startRow], this->rowIndications[endRow + 1] - this->rowIndications[startRow]);
    }
    
    template<typename T>
    typename SparseMatrix<T>::MutableRows SparseMatrix<T>::getMutableRow(uint_fast64_t row) {
        return getMutableRows(row, row);
    }

	template<typename T>
	typename SparseMatrix<T>::Rows SparseMatrix<T>::getRow(uint_fast64_t row) const {
        return getRows(row, row);
    }

	template<typename T>
	typename SparseMatrix<T>::ConstRowIterator SparseMatrix<T>::begin(uint_fast64_t initialRow) const {
		return ConstRowIterator(this->valueStorage.data(), this->columnIndications.data(), this->rowIndications.data() + initialRow);
	}

	template<typename T>
	typename SparseMatrix<T>::ConstRowIterator SparseMatrix<T>::end() const {
		return ConstRowIterator(this->valueStorage.data(), this->columnIndications.data(), this->rowIndications.data() + rowCount);
	}

	template<typename T>
	typename SparseMatrix<T>::ConstRowIterator SparseMatrix<T>::end(uint_fast64_t row) const {
		return ConstRowIterator(this->valueStorage.data(), this->columnIndications.data(), this->rowIndications.data() + row + 1);
	}

	template<typename T>
	typename SparseMatrix<T>::ConstIndexIterator SparseMatrix<T>::constColumnIteratorBegin(uint_fast64_t row) const {
		return &(this->columnIndications[0]) + this->rowIndications[row];
	}

	template<typename T>
	typename SparseMatrix<T>::ConstIndexIterator SparseMatrix<T>::constColumnIteratorEnd() const {
		return &(this->columnIndications[0]) + this->rowIndications[rowCount];
	}

	template<typename T>
	typename SparseMatrix<T>::ConstIndexIterator SparseMatrix<T>::constColumnIteratorEnd(uint_fast64_t row) const {
		return &(this->columnIndications[0]) + this->rowIndications[row + 1];
	}

	template<typename T>
	typename SparseMatrix<T>::ConstValueIterator SparseMatrix<T>::constValueIteratorBegin(uint_fast64_t row) const {
		return &(this->valueStorage[0]) + this->rowIndications[row];
	}

	template<typename T>
	typename SparseMatrix<T>::ConstValueIterator SparseMatrix<T>::constValueIteratorEnd() const {
		return &(this->valueStorage[0]) + this->rowIndications[rowCount];
	}

	template<typename T>
	typename SparseMatrix<T>::ConstValueIterator SparseMatrix<T>::constValueIteratorEnd(uint_fast64_t row) const {
		return &(this->valueStorage[0]) + this->rowIndications[row + 1];
	}
	
	template<typename T>
	typename SparseMatrix<T>::ValueIterator SparseMatrix<T>::valueIteratorBegin(uint_fast64_t row) {
		return &(this->valueStorage[0]) + this->rowIndications[row];
	}

	template<typename T>
	typename SparseMatrix<T>::ValueIterator SparseMatrix<T>::valueIteratorEnd(uint_fast64_t row) {
		return &(this->valueStorage[0]) + this->rowIndications[row + 1];
	}

	template<typename T>
	T SparseMatrix<T>::getRowSum(uint_fast64_t row) const {
		T sum = storm::utility::constantZero<T>();
		for (auto it = this->constValueIteratorBegin(row), ite = this->constValueIteratorEnd(row); it != ite; ++it) {
			sum += *it;
		}
		return sum;
	}

	template<typename T>
	bool SparseMatrix<T>::isSubmatrixOf(SparseMatrix<T> const& matrix) const {
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

	template<typename T>
	std::string SparseMatrix<T>::toString(std::vector<uint_fast64_t> const* rowGroupIndices) const {
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
					result << std::setprecision(8) << valueStorage[nextIndex] << "\t";
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

	template<typename T>
	std::size_t SparseMatrix<T>::getHash() const {
		std::size_t result = 0;

		boost::hash_combine(result, rowCount);
		boost::hash_combine(result, colCount);
		boost::hash_combine(result, nonZeroEntryCount);
		boost::hash_combine(result, currentSize);
		boost::hash_combine(result, lastRow);
		boost::hash_combine(result, boost::hash_range(valueStorage.begin(), valueStorage.end()));
		boost::hash_combine(result, boost::hash_range(columnIndications.begin(), columnIndications.end()));
		boost::hash_combine(result, boost::hash_range(rowIndications.begin(), rowIndications.end()));

		return result;
	}

	template<typename T>
	void SparseMatrix<T>::triggerErrorState() {
		setState(MatrixStatus::Error);
	}

	template<typename T>
	void SparseMatrix<T>::setState(const MatrixStatus new_state) {
		internalStatus = (internalStatus == MatrixStatus::Error) ? internalStatus : new_state;
	}

	template<typename T>
	bool SparseMatrix<T>::prepareInternalStorage(bool initializeElements) {
		if (initializeElements) {
			// Set up the arrays for the elements that are not on the diagonal.
			valueStorage.resize(nonZeroEntryCount, storm::utility::constantZero<T>());
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

	// Explicit instantiations of specializations of this template here.
	template class SparseMatrix<double>;
	template class SparseMatrix<int>;

	// Functions of the tbbHelper_MatrixRowVectorScalarProduct friend class.

#ifdef STORM_HAVE_INTELTBB

	template <typename M, typename V, typename T>
	tbbHelper_MatrixRowVectorScalarProduct<typename M, typename V, typename T>::tbbHelper_MatrixRowVectorScalarProduct(M const* matrixA, V const* vectorX, V * resultVector) : matrixA(matrixA), vectorX(vectorX), resultVector(resultVector) {}

	template <typename M, typename V, typename T>
	void tbbHelper_MatrixRowVectorScalarProduct<typename M, typename V, typename T>::operator() (const tbb::blocked_range<uint_fast64_t>& r) const {
		for (uint_fast64_t row = r.begin(); row < r.end(); ++row) {
			uint_fast64_t index = matrixA->rowIndications.at(row);
			uint_fast64_t indexEnd = matrixA->rowIndications.at(row + 1);

			// Initialize the result to be 0.
			T element = storm::utility::constantZero<T>();

			for (; index != indexEnd; ++index) {
				element += matrixA->valueStorage.at(index) * vectorX->at(matrixA->columnIndications.at(index));
			}

			// Write back to the result Vector
			resultVector->at(row) = element;
		}
	}

	// Explicit instanciations of specializations of this template here.
	template class tbbHelper_MatrixRowVectorScalarProduct<storm::storage::SparseMatrix<double>, std::vector<double>, double>;

#endif


} // namespace storage
} // namespace storm



