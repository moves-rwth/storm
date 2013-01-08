#ifndef STORM_STORAGE_SQUARESPARSEMATRIX_H_
#define STORM_STORAGE_SQUARESPARSEMATRIX_H_

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
namespace storm { namespace adapters{ class GmmxxAdapter; } }

namespace storm {

namespace storage {

/*!
 * A sparse matrix class with a constant number of non-zero entries.
 * NOTE: Addressing *is* zero-based, so the valid range for getValue and addNextValue is 0..(rows - 1)
 * where rows is the first argument to the constructor.
 */
template<class T>
class SquareSparseMatrix {
public:
	/*!
	 * Declare adapter classes as friends to use internal data.
	 */
	friend class storm::adapters::GmmxxAdapter;

	/*!
	 * If we only want to iterate over the columns of the non-zero entries of
	 * a row, we can simply iterate over the array (part) itself.
	 */
	typedef const uint_fast64_t * const constIndexIterator;
	
	/*!
	 *	Iterator type if we want to iterate over elements.
	 */
	typedef const T* const constIterator;

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
	SquareSparseMatrix(uint_fast64_t rows, uint_fast64_t cols)
			: rowCount(rows), colCount(cols), nonZeroEntryCount(0),
			  internalStatus(MatrixStatus::UnInitialized), currentSize(0), lastRow(0) { }

	/* Sadly, Delegate Constructors are not yet available with MSVC2012
	//! Constructor
	/*!
	 * Constructs a square sparse matrix object with the given number rows
	 * @param size The number of rows and cols in the matrix
	 */ /*
	SquareSparseMatrix(uint_fast64_t size) : SquareSparseMatrix(size, size) { }
	*/

	//! Constructor
	/*!
	 * Constructs a square sparse matrix object with the given number rows
	 * @param size The number of rows and cols in the matrix
	 */
	SquareSparseMatrix(uint_fast64_t size) : rowCount(size), colCount(size), nonZeroEntryCount(0),
			  internalStatus(MatrixStatus::UnInitialized), currentSize(0), lastRow(0) { }

	//! Copy Constructor
	/*!
	 * Copy Constructor. Performs a deep copy of the given sparse matrix.
	 * @param ssm A reference to the matrix to be copied.
	 */
	SquareSparseMatrix(const SquareSparseMatrix<T> &ssm)
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
				std::copy(ssm.valueStorage.begin(), ssm.valueStorage.end(), std::back_inserter(valueStorage));

				// The elements that are not of the value type but rather the
				// index type may be copied directly.
				std::copy(ssm.columnIndications.begin(), ssm.columnIndications.end(), std::back_inserter(columnIndications));
				std::copy(ssm.rowIndications.begin(), ssm.rowIndications.end(), std::back_inserter(rowIndications));
			}
		}
	}

	//! Destructor
	/*!
	 * Destructor. Performs deletion of the reserved storage arrays.
	 */
	~SquareSparseMatrix() {
		setState(MatrixStatus::UnInitialized);
		valueStorage.resize(0);
		columnIndications.resize(0);
		rowIndications.resize(0);
	}

	/*!
	 * Initializes the sparse matrix with the given number of non-zero entries
	 * and prepares it for use with addNextValue() and finalize().
	 * NOTE: Calling this method before any other member function is mandatory.
	 * This version is to be used together with addNextValue(). For
	 * initialization from an Eigen SparseMatrix, use initialize(Eigen::SparseMatrix<T> &).
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
	 * Initializes the sparse matrix with the given Eigen sparse matrix.
	 * NOTE: Calling this method before any other member function is mandatory.
	 * This version is only to be used when copying an Eigen sparse matrix. For
	 * initialization with addNextValue() and finalize() use initialize(uint_fast32_t)
	 * instead.
	 * @param eigenSparseMatrix The Eigen sparse matrix to be copied.
	 * *NOTE* Has to be in compressed form!
	 */
	template<int _Options, typename _Index>
	void initialize(const Eigen::SparseMatrix<T, _Options, _Index>& eigenSparseMatrix) {
		// Throw an error in case the matrix is not in compressed format.
		if (!eigenSparseMatrix.isCompressed()) {
			triggerErrorState();
			LOG4CPLUS_ERROR(logger, "Trying to initialize from an Eigen matrix that is not in compressed form.");
			throw storm::exceptions::InvalidArgumentException("Trying to initialize from an Eigen matrix that is not in compressed form.");
		}

		if (eigenSparseMatrix.rows() > this->rowCount) {
			triggerErrorState();
			LOG4CPLUS_ERROR(logger, "Trying to initialize from an Eigen matrix that has more rows than the target matrix.");
			throw storm::exceptions::InvalidArgumentException("Trying to initialize from an Eigen matrix that has more rows than the target matrix.");
		}
		if (eigenSparseMatrix.cols() > this->colCount) {
			triggerErrorState();
			LOG4CPLUS_ERROR(logger, "Trying to initialize from an Eigen matrix that has more columns than the target matrix.");
			throw storm::exceptions::InvalidArgumentException("Trying to initialize from an Eigen matrix that has more columns than the target matrix.");
		}

		const _Index entryCount = eigenSparseMatrix.nonZeros();
		nonZeroEntryCount = entryCount;
		lastRow = 0;

		// Try to prepare the internal storage and throw an error in case of
		// failure.
		
		// Get necessary pointers to the contents of the Eigen matrix.
		const T* valuePtr = eigenSparseMatrix.valuePtr();
		const _Index* indexPtr = eigenSparseMatrix.innerIndexPtr();
		const _Index* outerPtr = eigenSparseMatrix.outerIndexPtr();

		// If the given matrix is in RowMajor format, copying can simply
		// be done by adding all values in order.
		// Direct copying is, however, prevented because we have to
		// separate the diagonal entries from others.
		if (isEigenRowMajor(eigenSparseMatrix)) {
			// Because of the RowMajor format outerSize evaluates to the
			// number of rows.
			if (!prepareInternalStorage(false)) {
				triggerErrorState();
				LOG4CPLUS_ERROR(logger, "Unable to allocate internal storage.");
				throw std::bad_alloc();
			} else {
				if ((eigenSparseMatrix.innerSize() > nonZeroEntryCount) || (entryCount > nonZeroEntryCount)) {
					triggerErrorState();
					LOG4CPLUS_ERROR(logger, "Invalid internal composition of Eigen Sparse Matrix");
					throw storm::exceptions::InvalidArgumentException("Invalid internal composition of Eigen Sparse Matrix");
				}
				std::vector<uint_fast64_t> eigenColumnTemp;
				std::vector<uint_fast64_t> eigenRowTemp;
				std::vector<T> eigenValueTemp;
				uint_fast64_t outerSize = eigenSparseMatrix.outerSize() + 1;

				for (uint_fast64_t i = 0; i < entryCount; ++i) {
					eigenColumnTemp.push_back(indexPtr[i]);
					eigenValueTemp.push_back(valuePtr[i]);
				}
				for (uint_fast64_t i = 0; i < outerSize; ++i) {
					eigenRowTemp.push_back(outerPtr[i]);
				}

				std::copy(eigenRowTemp.begin(), eigenRowTemp.end(), std::back_inserter(this->rowIndications));
				std::copy(eigenColumnTemp.begin(), eigenColumnTemp.end(), std::back_inserter(this->columnIndications));
				std::copy(eigenValueTemp.begin(), eigenValueTemp.end(), std::back_inserter(this->valueStorage));

				currentSize = entryCount;
				lastRow = rowCount;
			}
		} else {
			if (!prepareInternalStorage()) {
				triggerErrorState();
				LOG4CPLUS_ERROR(logger, "Unable to allocate internal storage.");
				throw std::bad_alloc();
			} else {
				// Because of the ColMajor format outerSize evaluates to the
				// number of columns.
				const _Index colCount = eigenSparseMatrix.outerSize();

				// Create an array to remember which elements have to still
				// be searched in each column and initialize it with the starting
				// index for every column.
				_Index* positions = new _Index[colCount]();
				for (_Index i = 0; i < colCount; ++i) {
					positions[i] = outerPtr[i];
				}

				// Now copy the elements. As the matrix is in ColMajor format,
				// we need to iterate over the columns to find the next non-zero
				// entry.
				int i = 0;
				int currentRow = 0;
				int currentColumn = 0;
				while (i < entryCount) {
					// If the current element belongs the the current column,
					// add it in case it is also in the current row.
					if ((positions[currentColumn] < outerPtr[currentColumn + 1])
							&& (indexPtr[positions[currentColumn]] == currentRow)) {
						addNextValue(currentRow, currentColumn,	valuePtr[positions[currentColumn]]);
						// Remember that we found one more non-zero element.
						++i;
						// Mark this position as "used".
						++positions[currentColumn];
					}

					// Now we can advance to the next column and also row,
					// in case we just iterated through the last column.
					++currentColumn;
					if (currentColumn == colCount) {
						currentColumn = 0;
						++currentRow;
					}
				}
				delete[] positions;
			}
		}
		setState(MatrixStatus::Initialized);
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
			LOG4CPLUS_ERROR(logger, "Trying to finalize a matrix that was initialized with more non-zero entries than given.");
			throw storm::exceptions::InvalidStateException("Trying to finalize a matrix that was initialized with more non-zero entries than given.");
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
	std::vector<T> const & getStoragePointer() const {
		return valueStorage;
	}

	/*!
	 * Returns a pointer to the array that stores the start indices of non-zero
	 * entries in the value storage for each row.
	 * @return A pointer to the array that stores the start indices of non-zero
	 * entries in the value storage for each row.
	 */
	std::vector<uint_fast64_t> const & getRowIndicationsPointer() const {
		return rowIndications;
	}

	/*!
	 * Returns a pointer to an array that stores the column of each non-zero
	 * element.
	 * @return A pointer to an array that stores the column of each non-zero
	 * element.
	 */
	std::vector<uint_fast64_t> const & getColumnIndicationsPointer() const {
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
	 * Exports this sparse matrix to Eigens sparse matrix format.
	 * NOTE: this requires this matrix to be in the ReadReady state.
	 * @return The sparse matrix in Eigen format.
	 */
	Eigen::SparseMatrix<T, Eigen::RowMajor, int_fast32_t>* toEigenSparseMatrix() {
		// Check whether it is safe to export this matrix.
		if (!isReadReady()) {
			triggerErrorState();
			LOG4CPLUS_ERROR(logger, "Trying to convert a matrix that is not in a readable state to an Eigen matrix.");
			throw storm::exceptions::InvalidStateException("Trying to convert a matrix that is not in a readable state to an Eigen matrix.");
		} else {
			// Create the resulting matrix.
			int_fast32_t eigenRows = static_cast<int_fast32_t>(rowCount);
			Eigen::SparseMatrix<T, Eigen::RowMajor, int_fast32_t>* mat = new Eigen::SparseMatrix<T, Eigen::RowMajor, int_fast32_t>(eigenRows, eigenRows);

			// There are two ways of converting this matrix to Eigen's format.
			// 1. Compute a list of triplets (row, column, value) for all
			// non-zero elements and pass it to Eigen to create a sparse matrix.
			// 2. Tell Eigen to reserve the average number of non-zero elements
			// per row in advance and insert the values via a call to Eigen's
			// insert method then. As the given reservation number is only an
			// estimate, the actual number may be different and Eigen may have
			// to shift a lot.
			// In most cases, the second alternative is faster (about 1/2 of the
			// first, but if there are "heavy" rows that are several times larger
			// than an average row, the other solution might be faster.
			// The desired conversion method may be set by an appropriate define.

#define STORM_USE_TRIPLETCONVERT
#			ifdef STORM_USE_TRIPLETCONVERT

			// Prepare the triplet storage.
			typedef Eigen::Triplet<T> IntTriplet;
			std::vector<IntTriplet> tripletList;
			tripletList.reserve(nonZeroEntryCount + rowCount);

			// First, iterate over all elements that are not on the diagonal
			// and add the corresponding triplet.
			uint_fast64_t rowStart;
			uint_fast64_t rowEnd;
			uint_fast64_t zeroCount = 0;
			for (uint_fast64_t row = 0; row < rowCount; ++row) {
				rowStart = rowIndications[row];
				rowEnd = rowIndications[row + 1];
				while (rowStart < rowEnd) {
					if (valueStorage[rowStart] == 0) zeroCount++;
					tripletList.push_back(IntTriplet(static_cast<int_fast32_t>(row), static_cast<int_fast32_t>(columnIndications[rowStart]), valueStorage[rowStart]));
					++rowStart;
				}
			}

			// Let Eigen create a matrix from the given list of triplets.
			mat->setFromTriplets(tripletList.begin(), tripletList.end());

#			else // NOT STORM_USE_TRIPLETCONVERT

			// Reserve the average number of non-zero elements per row for each
			// row.
			mat->reserve(Eigen::VectorXi::Constant(eigenRows, static_cast<int_fast32_t>((nonZeroEntryCount + rowCount) / eigenRows)));

			// Iterate over the all non-zero elements in this matrix and add
			// them to the matrix individually.
			uint_fast64_t rowStart;
			uint_fast64_t rowEnd;
			uint_fast64_t count = 0;
			for (uint_fast64_t row = 0; row < rowCount; ++row) {
				rowStart = rowIndications[row];
				rowEnd = rowIndications[row + 1];

				// Insert the elements that are not on the diagonal
				while (rowStart < rowEnd) {
					mat->insert(row, columnIndications[rowStart]) = valueStorage[rowStart];
					count++;
					++rowStart;
				}
			}
#			endif // STORM_USE_TRIPLETCONVERT

			// Make the matrix compressed, i.e. remove remaining zero-entries.
			mat->makeCompressed();

			return mat;
		}

		// This point can never be reached as both if-branches end in a return
		// statement.
		return nullptr;
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
			result &= makeRowAbsorbing(row);
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
	bool makeRowAbsorbing(const uint_fast64_t row) {
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
			LOG4CPLUS_ERROR(logger, "The row " << row << " can not be made absorbing, no state in row, would have to recreate matrix!");
			throw storm::exceptions::InvalidStateException("A row can not be made absorbing, no state in row, would have to recreate matrix!");
		}
		uint_fast64_t pseudoDiagonal = row % colCount;

		bool foundDiagonal = false;
		while (rowStart < rowEnd) {
			if (!foundDiagonal && columnIndications[rowStart] >= pseudoDiagonal) {
				foundDiagonal = true;
				// insert/replace the diagonal entry
				columnIndications[rowStart] = pseudoDiagonal;
				valueStorage[rowStart] = storm::utility::constGetOne<T>();
			} else {
				valueStorage[rowStart] = storm::utility::constGetZero<T>();
			}
			++rowStart;
		}

		if (!foundDiagonal) {
			--rowStart;
			columnIndications[rowStart] = pseudoDiagonal;
			valueStorage[rowStart] = storm::utility::constGetOne<T>();
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
	void getConstrainedRowCountVector(const storm::storage::BitVector& rowConstraint, const storm::storage::BitVector& columnConstraint, std::vector<T>* resultVector) const {
		uint_fast64_t currentRowCount = 0;
		for (auto row : rowConstraint) {
			(*resultVector)[currentRowCount++] = getConstrainedRowSum(row, columnConstraint);
		}
	}

	/*!
	 * Creates a sub-matrix of the current matrix by dropping all rows and
	 * columns whose bits are not set to one in the given bit vector.
	 * @param constraint A bit vector indicating which rows and columns to drop.
	 * @return A pointer to a sparse matrix that is a sub-matrix of the current one.
	 */
	SquareSparseMatrix* getSubmatrix(storm::storage::BitVector& constraint) const {
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
		SquareSparseMatrix* result = new SquareSparseMatrix(constraint.getNumberOfSetBits());
		result->initialize(subNonZeroEntries);

		// Create a temporary array that stores for each index whose bit is set
		// to true the number of bits that were set before that particular index.
		uint_fast64_t* bitsSetBeforeIndex = new uint_fast64_t[rowCount];
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

	void convertToEquationSystem() {
		invertDiagonal();
		negateAllNonDiagonalElements();
	}

	/*!
	 * Inverts all elements on the diagonal, i.e. sets the diagonal values to 1 minus their previous
	 * value.
	 */
	void invertDiagonal() {
		T one(1);
		for (uint_fast64_t row = 0; row < rowCount; ++row) {
			uint_fast64_t rowStart = rowIndications[row];
			uint_fast64_t rowEnd = rowIndications[row + 1];
			uint_fast64_t pseudoDiagonal = row % colCount;
			while (rowStart < rowEnd) {
				if (columnIndications[rowStart] == pseudoDiagonal) {
					valueStorage[rowStart] = one - valueStorage[rowStart];
					break;
				}
				++rowStart;
			}
		}
	}

	/*!
	 * Negates all non-zero elements that are not on the diagonal.
	 */
	void negateAllNonDiagonalElements() {
		for (uint_fast64_t row = 0; row < rowCount; ++row) {
			uint_fast64_t rowStart = rowIndications[row];
			uint_fast64_t rowEnd = rowIndications[row + 1];
			uint_fast64_t pseudoDiagonal = row % colCount;
			while (rowStart < rowEnd) {
				if (columnIndications[rowStart] != pseudoDiagonal) {
					valueStorage[rowStart] = - valueStorage[rowStart];
				}
				++rowStart;
			}
		}
	}

	/*!
	 * Calculates the Jacobi-Decomposition of this sparse matrix.
	 * @return A pointer to a class containing the matrix L+U and the inverted diagonal matrix D^-1
	 */
	storm::storage::JacobiDecomposition<T>* getJacobiDecomposition() const {
		uint_fast64_t rowCount = this->getRowCount();
		SquareSparseMatrix<T> *resultLU = new SquareSparseMatrix<T>(this);
		SquareSparseMatrix<T> *resultDinv = new SquareSparseMatrix<T>(rowCount);
		// no entries apart from those on the diagonal
		resultDinv->initialize(0);
		// copy diagonal entries to other matrix
		for (int i = 0; i < rowCount; ++i) {
			resultDinv->addNextValue(i, i, resultLU->getValue(i, i));
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
	std::vector<T>* getPointwiseProductRowSumVector(storm::storage::SquareSparseMatrix<T> const& otherMatrix) {
		// Prepare result.
		std::vector<T>* result = new std::vector<T>(rowCount);

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
					(*result)[row] += otherMatrix.valueStorage[element] * valueStorage[nextOtherElement];
					++nextOtherElement;
				}
			}
		}

		return result;
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

	void print() const {
		std::cout << "entries: ----------------------------" << std::endl;
		for (uint_fast64_t i = 0; i < rowCount; ++i) {
			for (uint_fast64_t j = rowIndications[i]; j < rowIndications[i + 1]; ++j) {
				std::cout << "(" << i << "," << columnIndications[j] << ") = " << valueStorage[j] << std::endl;
			}
		}
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

	/*!
	 * Helper function to determine whether the given Eigen matrix is in RowMajor
	 * format. Always returns true, but is overloaded, so the compiler will
	 * only call it in case the Eigen matrix is in RowMajor format.
	 * @return True.
	 */
	template<typename _Scalar, typename _Index>
	bool isEigenRowMajor(Eigen::SparseMatrix<_Scalar, Eigen::RowMajor, _Index>) {
		return true;
	}

	/*!
	 * Helper function to determine whether the given Eigen matrix is in RowMajor
	 * format. Always returns false, but is overloaded, so the compiler will
	 * only call it in case the Eigen matrix is in ColMajor format.
	 * @return False.
	 */
	template<typename _Scalar, typename _Index>
	bool isEigenRowMajor(
			Eigen::SparseMatrix<_Scalar, Eigen::ColMajor, _Index>) {
		return false;
	}

};

} // namespace storage

} // namespace storm

#endif // STORM_STORAGE_SQUARESPARSEMATRIX_H_
