#ifndef MRMC_SPARSE_STATIC_SPARSE_MATRIX_H_
#define MRMC_SPARSE_STATIC_SPARSE_MATRIX_H_

#include <exception>
#include <new>
#include "boost/integer/integer_mask.hpp"

#include <pantheios/pantheios.hpp>
#include <pantheios/inserters/integer.hpp>

#include "src/exceptions/invalid_state.h"
#include "src/exceptions/invalid_argument.h"
#include "src/exceptions/out_of_range.h"
#include "src/exceptions/file_IO_exception.h"

#include "src/misc/const_templates.h"

#include "Eigen/Sparse"

namespace mrmc {

namespace sparse {

/*!
 * A sparse matrix class with a constant number of non-zero entries on the non-diagonal fields
 * and a seperate dense storage for the diagonal elements.
 * NOTE: Addressing *is* zero-based, so the valid range for getValue and addNextValue is 0..(rows - 1)
 * where rows is the first argument to the constructor.
 */
template<class T>
class StaticSparseMatrix {
public:
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
		Error = -1, UnInitialized = 0, Initialized = 1, ReadReady = 2,
	};

	//! Constructor
	/*!
	 * Constructs a sparse matrix object with the given number of rows.
	 * @param rows The number of rows of the matrix
	 */
	StaticSparseMatrix(uint_fast64_t rows)
			: internal_status(MatrixStatus::UnInitialized),
			  current_size(0), last_row(0), value_storage(nullptr),
			  diagonal_storage(nullptr), column_indications(nullptr),
			  row_indications(nullptr), row_count(rows), non_zero_entry_count(0) { }

	//! Copy Constructor
	/*!
	 * Copy Constructor. Performs a deep copy of the given sparse matrix.
	 * @param ssm A reference to the matrix to be copied.
	 */
	StaticSparseMatrix(const StaticSparseMatrix<T> &ssm)
			: internal_status(ssm.internal_status),
			  current_size(ssm.current_size), last_row(ssm.last_row),
			  row_count(ssm.row_count),
			  non_zero_entry_count(ssm.non_zero_entry_count) {
		pantheios::log_DEBUG("StaticSparseMatrix::CopyConstructor: Using copy constructor.");
		// Check whether copying the matrix is safe.
		if (!ssm.hasError()) {
			pantheios::log_ERROR("StaticSparseMatrix::CopyCtor: Throwing invalid_argument: Can not Copy from matrix in error state.");
			throw mrmc::exceptions::invalid_argument();
		} else {
			// Try to prepare the internal storage and throw an error in case
			// of a failure.
			if (!prepareInternalStorage()) {
				pantheios::log_ERROR("StaticSparseMatrix::CopyConstructor: Throwing bad_alloc: memory allocation failed.");
				throw std::bad_alloc();
			} else {
				// Now that all storages have been prepared, copy over all
				// elements. Start by copying the elements of type value and
				// copy them seperately in order to invoke copy their copy
				// constructor. This may not be necessary, but it is safer to
				// do so in any case.
				for (uint_fast64_t i = 0; i < non_zero_entry_count; ++i) {
					// use T() to force use of the copy constructor for complex T types
					value_storage[i] = T(ssm.value_storage[i]);
				}
				for (uint_fast64_t i = 0; i <= row_count; ++i) {
					// use T() to force use of the copy constructor for complex T types
					diagonal_storage[i] = T(ssm.diagonal_storage[i]);
				}

				// The elements that are not of the value type but rather the
				// index type may be copied with memcpy.
				memcpy(column_indications, ssm.column_indications, sizeof(column_indications[0]) * non_zero_entry_count);
				memcpy(row_indications, ssm.row_indications, sizeof(row_indications[0]) * (row_count + 1));
			}
		}
	}

	//! Destructor
	/*!
	 * Destructor. Performs deletion of the reserved storage arrays.
	 */
	~StaticSparseMatrix() {
		setState(MatrixStatus::UnInitialized);
		if (value_storage != NULL) {
			//free(value_storage);
			delete[] value_storage;
		}
		if (column_indications != NULL) {
			//free(column_indications);
			delete[] column_indications;
		}
		if (row_indications != NULL) {
			//free(row_indications);
			delete[] row_indications;
		}
		if (diagonal_storage != NULL) {
			//free(diagonal_storage);
			delete[] diagonal_storage;
		}
	}

	/*!
	 * Initializes the sparse matrix with the given number of non-zero entries
	 * and prepares it for use with addNextValue() and finalize().
	 * NOTE: Calling this method before any other member function is mandatory.
	 * This version is to be used together with addNextValue(). For
	 * initialization from an Eigen SparseMatrix, use initialize(Eigen::SparseMatrix<T> &).
	 * @param non_zero_entries
	 */
	void initialize(uint_fast64_t non_zero_entries) {
		// Check whether initializing the matrix is safe.
		if (internal_status != MatrixStatus::UnInitialized) {
			triggerErrorState();
			pantheios::log_ERROR("StaticSparseMatrix::initialize: Throwing invalid state for status flag != 0 (is ", pantheios::integer(internal_status), " - Already initialized?");
			throw mrmc::exceptions::invalid_state("StaticSparseMatrix::initialize: Invalid state for status flag != 0 - Already initialized?");
		} else if (row_count == 0) {
			triggerErrorState();
			pantheios::log_ERROR("StaticSparseMatrix::initialize: Throwing invalid_argument for row_count = 0");
			throw mrmc::exceptions::invalid_argument("mrmc::StaticSparseMatrix::initialize: Matrix with 0 rows is not reasonable");
		} else if (((row_count * row_count) - row_count) < non_zero_entries) {
			triggerErrorState();
			pantheios::log_ERROR("StaticSparseMatrix::initialize: Throwing invalid_argument: More non-zero entries than entries in target matrix");
			throw mrmc::exceptions::invalid_argument("mrmc::StaticSparseMatrix::initialize: More non-zero entries than entries in target matrix");
		} else {
			// If it is safe, initialize necessary members and prepare the
			// internal storage.
			non_zero_entry_count = non_zero_entries;
			last_row = 0;

			if (!prepareInternalStorage()) {
				triggerErrorState();
				pantheios::log_ERROR("StaticSparseMatrix::initialize: Throwing bad_alloc: memory allocation failed");
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
	 * @param eigen_sparse_matrix The Eigen sparse matrix to be copied.
	 * *NOTE* Has to be in compressed form!
	 */
	template<int _Options, typename _Index>
	void initialize(const Eigen::SparseMatrix<T, _Options, _Index> &eigen_sparse_matrix) {
		// Throw an error in case the matrix is not in compressed format.
		if (!eigen_sparse_matrix.isCompressed()) {
			triggerErrorState();
			pantheios::log_ERROR("StaticSparseMatrix::initialize: Throwing invalid_argument: eigen_sparse_matrix is not in Compressed form.");
			throw mrmc::exceptions::invalid_argument("StaticSparseMatrix::initialize: Throwing invalid_argument: eigen_sparse_matrix is not in Compressed form.");
		}

		// Compute the actual (i.e. non-diagonal) number of non-zero entries.
		non_zero_entry_count = getEigenSparseMatrixCorrectNonZeroEntryCount(eigen_sparse_matrix);
		last_row = 0;

		// Try to prepare the internal storage and throw an error in case of
		// failure.
		if (!prepareInternalStorage()) {
			triggerErrorState();
			pantheios::log_ERROR(
					"StaticSparseMatrix::initialize: Throwing bad_alloc: memory allocation failed");
			throw std::bad_alloc();
		} else {
			// Get necessary pointers to the contents of the Eigen matrix.
			const T* valuePtr = eigen_sparse_matrix.valuePtr();
			const _Index* indexPtr = eigen_sparse_matrix.innerIndexPtr();
			const _Index* outerPtr = eigen_sparse_matrix.outerIndexPtr();

			// If the given matrix is in RowMajor format, copying can simply
			// be done by adding all values in order.
			// Direct copying is, however, prevented because we have to
			// separate the diagonal entries from others.
			if (isEigenRowMajor(eigen_sparse_matrix)) {
				// Because of the RowMajor format outerSize evaluates to the
				// number of rows.
				const _Index rowCount = eigen_sparse_matrix.outerSize();
				for (_Index row = 0; row < rowCount; ++row) {
					for (_Index col = outerPtr[row]; col < outerPtr[row + 1];
							++col) {
						addNextValue(row, indexPtr[col], valuePtr[col]);
					}
				}
			} else {
				const _Index entryCount = eigen_sparse_matrix.nonZeros();
				// Because of the ColMajor format outerSize evaluates to the
				// number of columns.
				const _Index colCount = eigen_sparse_matrix.outerSize();

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
						i++;
						// Mark this position as "used".
						positions[currentColumn]++;
					}

					// Now we can advance to the next column and also row,
					// in case we just iterated through the last column.
					currentColumn++;
					if (currentColumn == colCount) {
						currentColumn = 0;
						currentRow++;
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
		if ((row > row_count) || (col > row_count)) {
			triggerErrorState();
			pantheios::log_ERROR("StaticSparseMatrix::addNextValue: Throwing out_of_range: row or col not in 0 .. rows (is ",
					pantheios::integer(row), " x ", pantheios::integer(col), ", max is ",
					pantheios::integer(row_count), " x ", pantheios::integer(row_count), ").");
			throw mrmc::exceptions::out_of_range("StaticSparseMatrix::addNextValue: row or col not in 0 .. rows");
		}

		if (row == col) { // Set a diagonal element.
			diagonal_storage[row] = value;
		} else { // Set a non-diagonal element.
			// If we switched to another row, we have to adjust the missing
			// entries in the row_indications array.
			if (row != last_row) {
				for (uint_fast64_t i = last_row + 1; i <= row; ++i) {
					row_indications[i] = current_size;
				}
				last_row = row;
			}

			// Finally, set the element and increase the current size.
			value_storage[current_size] = value;
			column_indications[current_size] = col;

			current_size++;
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
			pantheios::log_ERROR("StaticSparseMatrix::finalize: Throwing invalid state for internal state not Initialized (is ",
					pantheios::integer(internal_status), " - Already finalized?");
			throw mrmc::exceptions::invalid_state("StaticSparseMatrix::finalize: Invalid state for internal state not Initialized - Already finalized?");
		} else if (current_size != non_zero_entry_count) {
			triggerErrorState();
			pantheios::log_ERROR("StaticSparseMatrix::finalize: Throwing invalid_state: Wrong call count for addNextValue");
			throw mrmc::exceptions::invalid_state("StaticSparseMatrix::finalize: Wrong call count for addNextValue");
		} else {
			// Fill in the missing entries in the row_indications array.
			// (Can happen because of empty rows at the end.)
			if (last_row != row_count) {
				for (uint_fast64_t i = last_row + 1; i <= row_count; ++i) {
					row_indications[i] = current_size;
				}
			}

			// Set a sentinel element at the last position of the row_indications
			// array. This eases iteration work, as now the indices of row i
			// are always between row_indications[i] and row_indications[i + 1],
			// also for the first and last row.
			row_indications[row_count + 1] = non_zero_entry_count;

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
		if ((row > row_count) || (col > row_count)) {
			pantheios::log_ERROR("StaticSparseMatrix::getValue: row or col not in 0 .. rows (is ", pantheios::integer(row), " x ",
					pantheios::integer(col), ", max is ", pantheios::integer(row_count), " x ",	pantheios::integer(row_count), ").");
			throw mrmc::exceptions::out_of_range("StaticSparseMatrix::getValue: row or col not in 0 .. rows");
			return false;
		}

		// Read elements on the diagonal directly.
		if (row == col) {
			*target = diagonal_storage[row];
			return true;
		}

		// In case the element is not on the diagonal, we have to iterate
		// over the accessed row to find the element.
		uint_fast64_t row_start = row_indications[row];
		uint_fast64_t row_end = row_indications[row + 1];
		while (row_start < row_end) {
			// If the lement is found, write the content to the specified
			// position and return true.
			if (column_indications[row_start] == col) {
				*target = value_storage[row_start];
				return true;
			}
			// If the column of the current element is already larger than the
			// requested column, the requested element cannot be contained
			// in the matrix and we may therefore stop searching.
			if (column_indications[row_start] > col) {
				break;
			}
			row_start++;
		}

		// Set 0 as the content and return false in case the element was not found.
		*target = 0;
		return false;
	}

	/*!
	 * Returns the number of rows of the matrix.
	 */
	uint_fast64_t getRowCount() const {
		return row_count;
	}

	/*!
	 * Returns a pointer to the value storage of the matrix. This storage does
	 * *not* include elements on the diagonal.
	 * @return A pointer to the value storage of the matrix.
	 */
	T* getStoragePointer() const {
		return value_storage;
	}

	/*!
	 * Returns a pointer to the storage of elements on the diagonal.
	 * @return A pointer to the storage of elements on the diagonal.
	 */
	T* getDiagonalStoragePointer() const {
		return diagonal_storage;
	}

	/*!
	 * Returns a pointer to the array that stores the start indices of non-zero
	 * entries in the value storage for each row.
	 * @return A pointer to the array that stores the start indices of non-zero
	 * entries in the value storage for each row.
	 */
	uint_fast64_t* getRowIndicationsPointer() const {
		return row_indications;
	}

	/*!
	 * Returns a pointer to an array that stores the column of each non-zero
	 * element that is not on the diagonal.
	 * @return A pointer to an array that stores the column of each non-zero
	 * element that is not on the diagonal.
	 */
	uint_fast64_t* getColumnIndicationsPointer() const {
		return column_indications;
	}

	/*!
	 * Checks whether the internal status of the matrix makes it ready for
	 * reading access.
	 * @return True iff the internal status of the matrix makes it ready for
	 * reading access.
	 */
	bool isReadReady() {
		return (internal_status == MatrixStatus::ReadReady);
	}

	/*!
	 * Checks whether the matrix was initialized previously. The matrix may
	 * still require to be finalized, even if this check returns true.
	 * @return True iff the matrix was initialized previously.
	 */
	bool isInitialized() {
		return (internal_status == MatrixStatus::Initialized || internal_status == MatrixStatus::ReadReady);
	}

	/*!
	 * Returns the internal state of the matrix.
	 * @return The internal state of the matrix.
	 */
	MatrixStatus getState() {
		return internal_status;
	}

	/*!
	 * Checks whether the internal state of the matrix signals an error.
	 * @return True iff the internal state of the matrix signals an error.
	 */
	bool hasError() {
		return (internal_status == MatrixStatus::Error);
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
			pantheios::log_ERROR("StaticSparseMatrix::toEigenSparseMatrix: Throwing invalid state for internal state not ReadReady (is ", pantheios::integer(internal_status), ").");
			throw mrmc::exceptions::invalid_state("StaticSparseMatrix::toEigenSparseMatrix: Invalid state for internal state not ReadReady.");
		} else {
			// Create a
			int_fast32_t eigenRows = static_cast<int_fast32_t>(row_count);
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

#			ifdef MRMC_USE_TRIPLETCONVERT

			// FIXME: Wouldn't it be more efficient to add the elements in
			// order including the diagonal elements? Otherwise, Eigen has to
			// perform some sorting.

			// Prepare the triplet storage.
			typedef Eigen::Triplet<T> IntTriplet;
			std::vector<IntTriplet> tripletList;
			tripletList.reserve(non_zero_entry_count + row_count);

			// First, iterate over all elements that are not on the diagonal
			// and add the corresponding triplet.
			uint_fast64_t row_start;
			uint_fast64_t row_end;
			for (uint_fast64_t row = 0; row <= row_count; ++row) {
				row_start = row_indications[row];
				row_end = row_indications[row + 1];
				while (row_start < row_end) {
					tripletList.push_back(IntTriplet(row, column_indications[row_start], value_storage[row_start]));
					++row_start;
				}
			}

			// Then add the elements on the diagonal.
			for (uint_fast64_t i = 0; i <= row_count; ++i) {
				tripletList.push_back(IntTriplet(i, i, diagonal_storage[i]));
			}

			// Let Eigen create a matrix from the given list of triplets.
			mat->setFromTriplets(tripletList.begin(), tripletList.end());

#			else // NOT MRMC_USE_TRIPLETCONVERT

			// Reserve the average number of non-zero elements per row for each
			// row.
			mat->reserve(Eigen::VectorXi::Constant(eigenRows, static_cast<int_fast32_t>((non_zero_entry_count + row_count) / eigenRows)));

			// Iterate over the all non-zero elements in this matrix and add
			// them to the matrix individually.
			uint_fast64_t row_start;
			uint_fast64_t row_end;
			for (uint_fast64_t row = 0; row <= row_count; ++row) {
				row_start = row_indications[row];
				row_end = row_indications[row + 1];

				// Insert the element on the diagonal.
				mat->insert(row, row) = diagonal_storage[row];

				// Insert the elements that are not on the diagonal
				while (row_start < row_end) {
					mat->insert(row, column_indications[row_start]) = value_storage[row_start];
					++row_start;
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
	 * Returns the number of non-zero entries that are not on the diagonal.
	 * @returns The number of non-zero entries that are not on the diagonal.
	 */
	uint_fast64_t getNonZeroEntryCount() const {
		return non_zero_entry_count;
	}

	/*!
	 * This function makes the given state absorbing. This means that all
	 * entries in its row will be changed to 0 and the value 1 will be written
	 * to the element on the diagonal.
	 * @param state The state to be made absorbing.
	 * @returns True iff the operation was successful.
	 */
	bool makeStateAbsorbing(const uint_fast64_t state) {
		// Check whether the accessed state exists.
		if (state > row_count) {
			pantheios::log_ERROR("StaticSparseMatrix::makeStateFinal: state not in 0 .. rows (is ",	pantheios::integer(state), ", max is ",	pantheios::integer(row_count), ").");
			throw mrmc::exceptions::out_of_range("StaticSparseMatrix::makeStateFinal: state not in 0 .. rows");
			return false;
		}

		// Iterate over the elements in the row that are not on the diagonal
		// and set them to zero.
		uint_fast64_t row_start = row_indications[state];
		uint_fast64_t row_end = row_indications[state + 1];

		while (row_start < row_end) {
			value_storage[row_start] = mrmc::misc::constGetZero(value_storage);
			row_start++;
		}

		// Set the element on the diagonal to one.
		diagonal_storage[state] = mrmc::misc::constGetOne(diagonal_storage);
		return true;
	}

	/*!
	 * Returns the size of the matrix in memory measured in bytes.
	 * @return The size of the matrix in memory measured in bytes.
	 */
	uint_fast64_t getSizeInMemory() {
		uint_fast64_t size = sizeof(*this);
		size += sizeof(T) * non_zero_entry_count; // add value_storage size
		size += sizeof(T) * (row_count + 1); // add diagonal_storage size
		size += sizeof(uint_fast64_t) * non_zero_entry_count; // add column_indications size
		size += sizeof(uint_fast64_t) * (row_count + 1); // add row_indications size
		return size;
	}

private:

	/*!
	 * The number of rows of the matrix.
	 */
	uint_fast64_t row_count;

	/*!
	 * The number of non-zero elements that are not on the diagonal.
	 */
	uint_fast64_t non_zero_entry_count;

	/*!
	 * Stores all non-zero values that are not on the diagonal.
	 */
	T* value_storage;

	/*!
	 * Stores all elements on the diagonal, even the ones that are zero.
	 */
	T* diagonal_storage;

	/*!
	 * Stores the column for each non-zero element that is not on the diagonal.
	 */
	uint_fast64_t* column_indications;

	/*!
	 * Array containing the boundaries (indices) in the value_storage array
	 * for each row. All elements of value_storage with indices between the
	 * i-th and the (i+1)-st element of this array belong to row i.
	 */
	uint_fast64_t* row_indications;

	/*!
	 * The internal status of the matrix.
	 */
	MatrixStatus internal_status;

	/*!
	 * Stores the current number of non-zero elements that have been added to
	 * the matrix. Used for correctly inserting elements in the matrix.
	 */
	uint_fast64_t current_size;

	/*!
	 * Stores the row in which the last element was inserted. Used for correctly
	 * inserting elements in the matrix .
	 */
	uint_fast64_t last_row;

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
		internal_status = (internal_status == MatrixStatus::Error) ? internal_status : new_state;
	}

	/*!
	 * Prepares the internal CSR storage. For this, it requires
	 * non_zero_entry_count and row_count to be set correctly.
	 * @return True on success, false otherwise (allocation failed).
	 */
	bool prepareInternalStorage() {
		// Set up the arrays for the elements that are not on the diagonal.
		value_storage = new (std::nothrow) T[non_zero_entry_count]();
		column_indications = new (std::nothrow) uint_fast64_t[non_zero_entry_count]();

		// Set up the row_indications array and reserve one element more than
		// there are rows in order to put a sentinel element at the end,
		// which eases iteration process.
		row_indications = new (std::nothrow) uint_fast64_t[row_count + 1]();

		// Set up the array for the elements on the diagonal.
		diagonal_storage = new (std::nothrow) T[row_count]();

		// Return whether all the allocations could be made without error.
		return ((value_storage != NULL) && (column_indications != NULL)
				&& (row_indications != NULL) && (diagonal_storage != NULL));
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
	_Index getEigenSparseMatrixCorrectNonZeroEntryCount(
			const Eigen::SparseMatrix<_Scalar, _Options, _Index>& eigen_sparse_matrix) {
		const _Index* indexPtr = eigen_sparse_matrix.innerIndexPtr();
		const _Index* outerPtr = eigen_sparse_matrix.outerIndexPtr();

		const _Index entryCount = eigen_sparse_matrix.nonZeros();
		const _Index outerCount = eigen_sparse_matrix.outerSize();

		uint_fast64_t diag_non_zeros = 0;

		// For RowMajor, row is the current Row and col the column and for
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
				++diag_non_zeros;
			}
		}

		return static_cast<_Index>(entryCount - diag_non_zeros);
	}

};

} // namespace sparse

} // namespace mrmc

#endif // MRMC_SPARSE_STATIC_SPARSE_MATRIX_H_
