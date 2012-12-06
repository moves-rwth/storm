#ifndef MRMC_SPARSE_STATIC_SPARSE_MATRIX_H_
#define MRMC_SPARSE_STATIC_SPARSE_MATRIX_H_

#include <exception>
#include <new>
#include <algorithm>
#include <iostream>
#include "boost/integer/integer_mask.hpp"

#include "src/exceptions/invalid_state.h"
#include "src/exceptions/invalid_argument.h"
#include "src/exceptions/out_of_range.h"
#include "src/exceptions/file_IO_exception.h"
#include "src/storage/BitVector.h"

#include "src/utility/const_templates.h"
#include "Eigen/Sparse"
#include "gmm/gmm_matrix.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

extern log4cplus::Logger logger;

namespace mrmc {

namespace storage {

/*!
 * A sparse matrix class with a constant number of non-zero entries on the non-diagonal fields
 * and a separate dense storage for the diagonal elements.
 * NOTE: Addressing *is* zero-based, so the valid range for getValue and addNextValue is 0..(rows - 1)
 * where rows is the first argument to the constructor.
 */
template<class T>
class SquareSparseMatrix {
public:

	/*!
	 * If we only want to iterate over the columns of the non-zero entries of
	 * a row, we can simply iterate over the array (part) itself.
	 */
	typedef const uint_fast64_t * const constIndexIterator;

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
	SquareSparseMatrix(uint_fast64_t rows)
			: rowCount(rows), nonZeroEntryCount(0), valueStorage(nullptr),
			  diagonalStorage(nullptr),columnIndications(nullptr), rowIndications(nullptr),
			  internalStatus(MatrixStatus::UnInitialized), currentSize(0), lastRow(0) { }

	//! Copy Constructor
	/*!
	 * Copy Constructor. Performs a deep copy of the given sparse matrix.
	 * @param ssm A reference to the matrix to be copied.
	 */
	SquareSparseMatrix(const SquareSparseMatrix<T> &ssm)
			: rowCount(ssm.rowCount), nonZeroEntryCount(ssm.nonZeroEntryCount),
			  internalStatus(ssm.internalStatus), currentSize(ssm.currentSize), lastRow(ssm.lastRow) {
		LOG4CPLUS_WARN(logger, "Invoking copy constructor.");
		// Check whether copying the matrix is safe.
		if (!ssm.hasError()) {
			LOG4CPLUS_ERROR(logger, "Trying to copy sparse matrix in error state.");
			throw mrmc::exceptions::invalid_argument("Trying to copy sparse matrix in error state.");
		} else {
			// Try to prepare the internal storage and throw an error in case
			// of a failure.
			if (!prepareInternalStorage()) {
				LOG4CPLUS_ERROR(logger, "Unable to allocate internal storage.");
				throw std::bad_alloc();
			} else {
				// Now that all storages have been prepared, copy over all
				// elements. Start by copying the elements of type value and
				// copy them seperately in order to invoke copy their copy
				// constructor. This may not be necessary, but it is safer to
				// do so in any case.
				for (uint_fast64_t i = 0; i < nonZeroEntryCount; ++i) {
					// use T() to force use of the copy constructor for complex T types
					valueStorage[i] = T(ssm.valueStorage[i]);
				}
				for (uint_fast64_t i = 0; i <= rowCount; ++i) {
					// use T() to force use of the copy constructor for complex T types
					diagonalStorage[i] = T(ssm.diagonalStorage[i]);
				}

				// The elements that are not of the value type but rather the
				// index type may be copied directly.
				std::copy(ssm.columnIndications, ssm.columnIndications + nonZeroEntryCount, columnIndications);
				std::copy(ssm.rowIndications, ssm.rowIndications + rowCount + 1, rowIndications);
			}
		}
	}

	//! Destructor
	/*!
	 * Destructor. Performs deletion of the reserved storage arrays.
	 */
	~SquareSparseMatrix() {
		setState(MatrixStatus::UnInitialized);
		if (valueStorage != nullptr) {
			delete[] valueStorage;
		}
		if (columnIndications != nullptr) {
			delete[] columnIndications;
		}
		if (rowIndications != nullptr) {
			delete[] rowIndications;
		}
		if (diagonalStorage != nullptr) {
			delete[] diagonalStorage;
		}
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
			throw mrmc::exceptions::invalid_state("Trying to initialize matrix that is not uninitialized.");
		} else if (rowCount == 0) {
			triggerErrorState();
			LOG4CPLUS_ERROR(logger, "Trying to create initialize a matrix with 0 rows.");
			throw mrmc::exceptions::invalid_argument("Trying to create initialize a matrix with 0 rows.");
		} else if (((rowCount * rowCount) - rowCount) < nonZeroEntries) {
			triggerErrorState();
			LOG4CPLUS_ERROR(logger, "Trying to initialize a matrix with more non-zero entries than there can be.");
			throw mrmc::exceptions::invalid_argument("Trying to initialize a matrix with more non-zero entries than there can be.");
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
			throw mrmc::exceptions::invalid_argument("Trying to initialize from an Eigen matrix that is not in compressed form.");
		}

		// Compute the actual (i.e. non-diagonal) number of non-zero entries.
		nonZeroEntryCount = getEigenSparseMatrixCorrectNonZeroEntryCount(eigenSparseMatrix);
		lastRow = 0;

		// Try to prepare the internal storage and throw an error in case of
		// failure.
		if (!prepareInternalStorage()) {
			triggerErrorState();
			LOG4CPLUS_ERROR(logger, "Unable to allocate internal storage.");
			throw std::bad_alloc();
		} else {
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
				const _Index rowCount = eigenSparseMatrix.outerSize();
				for (_Index row = 0; row < rowCount; ++row) {
					for (_Index col = outerPtr[row]; col < outerPtr[row + 1]; ++col) {
						addNextValue(row, indexPtr[col], valuePtr[col]);
					}
				}
			} else {
				const _Index entryCount = eigenSparseMatrix.nonZeros();
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
						addNextValue(currentRow, currentColumn,
								valuePtr[positions[currentColumn]]);
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
			setState(MatrixStatus::Initialized);
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
		if ((row > rowCount) || (col > rowCount)) {
			triggerErrorState();
			LOG4CPLUS_ERROR(logger, "Trying to add a value at illegal position (" << row << ", " << col << ").");
			throw mrmc::exceptions::out_of_range("Trying to add a value at illegal position.");
		}

		if (row == col) { // Set a diagonal element.
			diagonalStorage[row] = value;
		} else { // Set a non-diagonal element.
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
			throw mrmc::exceptions::invalid_state("Trying to finalize an uninitialized matrix.");
		} else if (currentSize != nonZeroEntryCount) {
			triggerErrorState();
			LOG4CPLUS_ERROR(logger, "Trying to finalize a matrix that was initialized with more non-zero entries than given.");
			throw mrmc::exceptions::invalid_state("Trying to finalize a matrix that was initialized with more non-zero entries than given.");
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
	inline bool getValue(uint_fast64_t row, uint_fast64_t col, T* const target) {
		// Check for illegal access indices.
		if ((row > rowCount) || (col > rowCount)) {
			LOG4CPLUS_ERROR(logger, "Trying to read a value from illegal position (" << row << ", " << col << ").");
			throw mrmc::exceptions::out_of_range("Trying to read a value from illegal position.");
			return false;
		}

		// Read elements on the diagonal directly.
		if (row == col) {
			*target = diagonalStorage[row];
			return true;
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
	 * Returns the number of rows of the matrix.
	 */
	uint_fast64_t getRowCount() const {
		return rowCount;
	}

	/*!
	 * Returns a pointer to the value storage of the matrix. This storage does
	 * *not* include elements on the diagonal.
	 * @return A pointer to the value storage of the matrix.
	 */
	T* getStoragePointer() const {
		return valueStorage;
	}

	/*!
	 * Returns a pointer to the storage of elements on the diagonal.
	 * @return A pointer to the storage of elements on the diagonal.
	 */
	T* getDiagonalStoragePointer() const {
		return diagonalStorage;
	}

	/*!
	 * Returns a pointer to the array that stores the start indices of non-zero
	 * entries in the value storage for each row.
	 * @return A pointer to the array that stores the start indices of non-zero
	 * entries in the value storage for each row.
	 */
	uint_fast64_t* getRowIndicationsPointer() const {
		return rowIndications;
	}

	/*!
	 * Returns a pointer to an array that stores the column of each non-zero
	 * element that is not on the diagonal.
	 * @return A pointer to an array that stores the column of each non-zero
	 * element that is not on the diagonal.
	 */
	uint_fast64_t* getColumnIndicationsPointer() const {
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
			throw mrmc::exceptions::invalid_state("Trying to convert a matrix that is not in a readable state to an Eigen matrix.");
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

#define MRMC_USE_TRIPLETCONVERT
#			ifdef MRMC_USE_TRIPLETCONVERT

			// FIXME: Wouldn't it be more efficient to add the elements in
			// order including the diagonal elements? Otherwise, Eigen has to
			// perform some sorting.

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
					tripletList.push_back(IntTriplet(row, columnIndications[rowStart], valueStorage[rowStart]));
					++rowStart;
				}
			}

			// Then add the elements on the diagonal.
			for (uint_fast64_t i = 0; i < rowCount; ++i) {
				if (diagonalStorage[i] == 0) zeroCount++;
				// tripletList.push_back(IntTriplet(i, i, diagonalStorage[i]));
			}

			// Let Eigen create a matrix from the given list of triplets.
			mat->setFromTriplets(tripletList.begin(), tripletList.end());

#			else // NOT MRMC_USE_TRIPLETCONVERT

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

				// Insert the element on the diagonal.
				mat->insert(row, row) = diagonalStorage[row];
				count++;

				// Insert the elements that are not on the diagonal
				while (rowStart < rowEnd) {
					mat->insert(row, columnIndications[rowStart]) = valueStorage[rowStart];
					count++;
					++rowStart;
				}
			}
#			endif // MRMC_USE_TRIPLETCONVERT

			// Make the matrix compressed, i.e. remove remaining zero-entries.
			mat->makeCompressed();

			return mat;
		}

		// This point can never be reached as both if-branches end in a return
		// statement.
		return nullptr;
	}

	/*!
	 * Converts the matrix into a sparse matrix in the GMMXX format.
	 * @return A pointer to a column-major sparse matrix in GMMXX format.
	 */
	gmm::csr_matrix<T>* toGMMXXSparseMatrix() {
		uint_fast64_t realNonZeros = getNonZeroEntryCount() + getDiagonalNonZeroEntryCount();
		LOG4CPLUS_DEBUG(logger, "Converting matrix with " << realNonZeros << " non-zeros to gmm++ format.");

		// Prepare the resulting matrix.
		gmm::csr_matrix<T>* result = new gmm::csr_matrix<T>(rowCount, rowCount);

		// Reserve enough elements for the row indications.
		result->jc.reserve(rowCount + 1);

		// For the column indications and the actual values, we have to gather
		// the values in a temporary array first, as we have to integrate
		// the values from the diagonal. For the row indications, we can just count the number of
		// inserted diagonal elements and add it to the previous value.
		uint_fast64_t* tmpColumnIndicationsArray = new uint_fast64_t[realNonZeros];
		T* tmpValueArray = new T[realNonZeros];
		T zero(0);
		uint_fast64_t currentPosition = 0;
		uint_fast64_t insertedDiagonalElements = 0;
		for (uint_fast64_t i = 0; i < rowCount; ++i) {
			// Compute correct start index of row.
			result->jc[i] = rowIndications[i] + insertedDiagonalElements;

			// If the current row has no non-zero which is not on the diagonal, we have to check the
			// diagonal element explicitly.
			if (rowIndications[i + 1] - rowIndications[i] == 0) {
				if (diagonalStorage[i] != zero) {
					tmpColumnIndicationsArray[currentPosition] = i;
					tmpValueArray[currentPosition] = diagonalStorage[i];
					++currentPosition; ++insertedDiagonalElements;
				}
			} else {
				// Otherwise, we can just enumerate the non-zeros which are not on the diagonal
				// and fit in the diagonal element where appropriate.
				bool includedDiagonal = false;
				for (uint_fast64_t j = rowIndications[i]; j < rowIndications[i + 1]; ++j) {
					if (diagonalStorage[i] != zero && !includedDiagonal && columnIndications[j] > i) {
						includedDiagonal = true;
						tmpColumnIndicationsArray[currentPosition] = i;
						tmpValueArray[currentPosition] = diagonalStorage[i];
						++currentPosition; ++insertedDiagonalElements;
					}
					tmpColumnIndicationsArray[currentPosition] = columnIndications[j];
					tmpValueArray[currentPosition] = valueStorage[j];
					++currentPosition;
				}

				// If the diagonal element is non-zero and was not inserted until now (i.e. all
				// off-diagonal elements in the row are before the diagonal element.
				if (!includedDiagonal && diagonalStorage[i] != zero) {
					tmpColumnIndicationsArray[currentPosition] = i;
					tmpValueArray[currentPosition] = diagonalStorage[i];
					++currentPosition; ++insertedDiagonalElements;
				}
			}
		}
		// Fill in sentinel element at the end.
		result->jc[rowCount] = realNonZeros;

		// Now, we can copy the temporary array to the GMMXX format.
		result->ir.resize(realNonZeros);
		std::copy(tmpColumnIndicationsArray, tmpColumnIndicationsArray + realNonZeros, result->ir.begin());
		delete[] tmpColumnIndicationsArray;

		// And do the same thing with the actual values.
		result->pr.resize(realNonZeros);
		std::copy(tmpValueArray, tmpValueArray + realNonZeros, result->pr.begin());
		delete[] tmpValueArray;

		LOG4CPLUS_DEBUG(logger, "Done converting matrix to gmm++ format.");

		return result;
	}

	/*!
	 * Returns the number of non-zero entries that are not on the diagonal.
	 * @returns The number of non-zero entries that are not on the diagonal.
	 */
	uint_fast64_t getNonZeroEntryCount() const {
		return nonZeroEntryCount;
	}

	/*!
	 * Returns the number of non-zero entries on the diagonal.
	 * @return The number of non-zero entries on the diagonal.
	 */
	uint_fast64_t getDiagonalNonZeroEntryCount() const {
		uint_fast64_t result = 0;
		T zero(0);
		for (uint_fast64_t i = 0; i < rowCount; ++i) {
			if (diagonalStorage[i] != zero) ++result;
		}
		return result;
	}

	/*!
	 * This function makes the rows given by the bit vector absorbing.
	 * @param rows A bit vector indicating which rows to make absorbing.
	 * @return True iff the operation was successful.
	 */
	bool makeRowsAbsorbing(const mrmc::storage::BitVector rows) {
		bool result = true;
		for (auto row : rows) {
			result &= makeRowAbsorbing(row);
		}

		return result;
	}

	/*!
	 * This function makes the given row absorbing. This means that all
	 * entries in will be set to 0 and the value 1 will be written
	 * to the element on the diagonal.
	 * @param row The row to be made absorbing.
	 * @returns True iff the operation was successful.
	 */
	bool makeRowAbsorbing(const uint_fast64_t row) {
		// Check whether the accessed state exists.
		if (row > rowCount) {
			LOG4CPLUS_ERROR(logger, "Trying to make an illegal row " << row << " absorbing.");
			throw mrmc::exceptions::out_of_range("Trying to make an illegal row absorbing.");
			return false;
		}

		// Iterate over the elements in the row that are not on the diagonal
		// and set them to zero.
		uint_fast64_t rowStart = rowIndications[row];
		uint_fast64_t rowEnd = rowIndications[row + 1];

		while (rowStart < rowEnd) {
			valueStorage[rowStart] = mrmc::utility::constGetZero(valueStorage[rowStart]);
			++rowStart;
		}

		// Set the element on the diagonal to one.
		diagonalStorage[row] = mrmc::utility::constGetOne(diagonalStorage[row]);
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
	T getConstrainedRowSum(const uint_fast64_t row, const mrmc::storage::BitVector& constraint) {
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
	void getConstrainedRowCountVector(const mrmc::storage::BitVector& rowConstraint, const mrmc::storage::BitVector& columnConstraint, std::vector<T>* resultVector) {
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
	SquareSparseMatrix* getSubmatrix(mrmc::storage::BitVector& constraint) {
		LOG4CPLUS_DEBUG(logger, "Creating a sub-matrix with " << constraint.getNumberOfSetBits() << " rows.");

		// Check for valid constraint.
		if (constraint.getNumberOfSetBits() == 0) {
			LOG4CPLUS_ERROR(logger, "Trying to create a sub-matrix of size 0.");
			throw mrmc::exceptions::invalid_argument("Trying to create a sub-matrix of size 0.");
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
			result->addNextValue(rowCount, rowCount, diagonalStorage[rowIndex]);

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
		for (uint_fast64_t i = 0; i < rowCount; ++i) {
			diagonalStorage[i] = one - diagonalStorage[i];
		}
	}

	/*!
	 * Negates all non-zero elements that are not on the diagonal.
	 */
	void negateAllNonDiagonalElements() {
		for (uint_fast64_t i = 0; i < nonZeroEntryCount; ++i) {
			valueStorage[i] = - valueStorage[i];
		}
	}

	/*!
	 * Returns the size of the matrix in memory measured in bytes.
	 * @return The size of the matrix in memory measured in bytes.
	 */
	uint_fast64_t getSizeInMemory() {
		uint_fast64_t size = sizeof(*this);
		// Add value_storage size.
		size += sizeof(T) * nonZeroEntryCount;
		// Add diagonal_storage size.
		size += sizeof(T) * (rowCount + 1);
		// Add column_indications size.
		size += sizeof(uint_fast64_t) * nonZeroEntryCount;
		// Add row_indications size.
		size += sizeof(uint_fast64_t) * (rowCount + 1);
		return size;
	}

	/*!
	 * Returns an iterator to the columns of the non-zero entries of the given
	 * row that are not on the diagonal.
	 * @param row The row whose columns the iterator will return.
	 * @return An iterator to the columns of the non-zero entries of the given
	 * row that are not on the diagonal.
	 */
	constIndexIterator beginConstColumnNoDiagIterator(uint_fast64_t row) const {
		return this->columnIndications + this->rowIndications[row];
	}

	/*!
	 * Returns an iterator referring to the element after the given row.
	 * @param row The row for which the iterator should point to the past-the-end
	 * element.
	 */
	constIndexIterator endConstColumnNoDiagIterator(uint_fast64_t row) const {
		return this->columnIndications + this->rowIndications[row + 1];
	}

	void print() {
		std::cout << "diag: --------------------------------" << std::endl;
		for (uint_fast64_t i = 0; i < rowCount; ++i) {
			std::cout << "(" << i << "," << i << ") = " << diagonalStorage[i] << std::endl;
		}
		std::cout << "non diag: ----------------------------" << std::endl;
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
	 * The number of non-zero elements that are not on the diagonal.
	 */
	uint_fast64_t nonZeroEntryCount;

	/*!
	 * Stores all non-zero values that are not on the diagonal.
	 */
	T* valueStorage;

	/*!
	 * Stores all elements on the diagonal, even the ones that are zero.
	 */
	T* diagonalStorage;

	/*!
	 * Stores the column for each non-zero element that is not on the diagonal.
	 */
	uint_fast64_t* columnIndications;

	/*!
	 * Array containing the boundaries (indices) in the value_storage array
	 * for each row. All elements of value_storage with indices between the
	 * i-th and the (i+1)-st element of this array belong to row i.
	 */
	uint_fast64_t* rowIndications;

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
	 * @return True on success, false otherwise (allocation failed).
	 */
	bool prepareInternalStorage() {
		// Set up the arrays for the elements that are not on the diagonal.
		valueStorage = new (std::nothrow) T[nonZeroEntryCount]();
		columnIndications = new (std::nothrow) uint_fast64_t[nonZeroEntryCount]();

		// Set up the row_indications array and reserve one element more than
		// there are rows in order to put a sentinel element at the end,
		// which eases iteration process.
		rowIndications = new (std::nothrow) uint_fast64_t[rowCount + 1]();

		// Set up the array for the elements on the diagonal.
		diagonalStorage = new (std::nothrow) T[rowCount]();

		// Return whether all the allocations could be made without error.
		return ((valueStorage != NULL) && (columnIndications != NULL)
				&& (rowIndications != NULL) && (diagonalStorage != NULL));
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

	/*!
	 * Helper function to determine the number of non-zero elements that are
	 * not on the diagonal of the given Eigen matrix.
	 * @param eigen_sparse_matrix The Eigen matrix to analyze.
	 * @return The number of non-zero elements that are not on the diagonal of
	 * the given Eigen matrix.
	 */
	template<typename _Scalar, int _Options, typename _Index>
	_Index getEigenSparseMatrixCorrectNonZeroEntryCount(const Eigen::SparseMatrix<_Scalar, _Options, _Index>& eigen_sparse_matrix) {
		const _Index* indexPtr = eigen_sparse_matrix.innerIndexPtr();
		const _Index* outerPtr = eigen_sparse_matrix.outerIndexPtr();

		const _Index entryCount = eigen_sparse_matrix.nonZeros();
		const _Index outerCount = eigen_sparse_matrix.outerSize();

		uint_fast64_t diagNonZeros = 0;

		// For RowMajor, row is the current row and col the column and for
		// ColMajor, row is the current column and col the row, but this is
		// not important as we are only looking for elements on the diagonal.
		_Index innerStart = 0;
		_Index innerEnd = 0;
		_Index innerMid = 0;
		for (_Index row = 0; row < outerCount; ++row) {
			innerStart = outerPtr[row];
			innerEnd = outerPtr[row + 1] - 1;

			// Now use binary search (but defer equality detection).
			while (innerStart < innerEnd) {
				innerMid = innerStart + ((innerEnd - innerStart) / 2);

				if (indexPtr[innerMid] < row) {
					innerStart = innerMid + 1;
				} else {
					innerEnd = innerMid;
				}
			}

			// Check whether we have found an element on the diagonal.
			if ((innerStart == innerEnd) && (indexPtr[innerStart] == row)) {
				++diagNonZeros;
			}
		}

		return static_cast<_Index>(entryCount - diagNonZeros);
	}

};

} // namespace sparse

} // namespace mrmc

#endif // MRMC_SPARSE_STATIC_SPARSE_MATRIX_H_
