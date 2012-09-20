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

#include "Eigen/Sparse"

namespace mrmc {

namespace sparse {

//! A sparse Matrix for DTMCs with a constant number of non-zero entries on the non-diagonal fields.
/*! 
	The sparse matrix used for calculation on the DTMCs.
	Addressing is NOT zero-based! The valid range for getValue and addNextValue is 1 to rows (first argument to constructor).
 */
template <class T>
class StaticSparseMatrix {
 public:
	//! Status enum
	/*!
		An Enum representing the internal state of the Matrix. 
		After creating the Matrix using the Constructor, the Object is in state UnInitialized. After calling initialize(), that state changes to Initialized and after all entries have been entered and finalize() has been called, to ReadReady.
		Should a critical error occur in any of the former functions, the state will change to Error.
		@see getState()
		@see isReadReady()
		@see isInitialized()
		@see hasError()
	 */
	 enum MatrixStatus {
		Error = -1,
		UnInitialized = 0,
		Initialized = 1,
		ReadReady = 2,
	 };


	//! Constructor
	 /*!
		\param rows Row-Count and therefore column-count of the symmetric matrix
		\param non_zero_entries The exact count of entries that will be submitted through addNextValue *excluding* those on the diagonal (A_{i,j} with i = j)
	 */
	StaticSparseMatrix(uint_fast32_t rows, uint_fast32_t non_zero_entries) {
		setState(MatrixStatus::UnInitialized);
		current_size = 0;
		storage_size = 0;
		
		value_storage = NULL;
		diagonal_storage = NULL;
		column_indications = NULL;
		row_indications = NULL;

		row_count = rows;
		non_zero_entry_count = non_zero_entries;

		//initialize(rows, non_zero_entries);
	}

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

	//! Getter for saving matrix entry A_{row,col} to target
	/*!
		Getter function for the matrix. This function does not check the internal status for errors for performance reasons.
		\param row 1-based index of the requested row
		\param col 1-based index of the requested column
		\param target pointer to where the result will be stored
		\return True iff the value was set, false otherwise. On false, 0 will be written to *target.
	*/
	inline bool getValue(uint_fast32_t row, uint_fast32_t col, T* const target) {
		
		if (row == col) {
			// storage is row_count + 1 large for direct access without the -1
			*target = diagonal_storage[row];
			return true;
		}

		if ((row > row_count) || (col > row_count) || (row == 0) || (col == 0)) {
			throw mrmc::exceptions::out_of_range("mrmc::StaticSparseMatrix::getValue: row or col not in 1 .. rows");
		}

		uint_fast32_t row_start = row_indications[row - 1];
		uint_fast32_t row_end = row_indications[row];

		while (row_start < row_end) {
			if (column_indications[row_start] == col) {
				*target = value_storage[row_start];
				return true;
			}
			if (column_indications[row_start] > col) {
				break;
			}
			row_start++;
		}

		*target = 0;
		return false;
	}

	//! Mandatory initialization of the matrix
	/*!
		Mandatory initialization of the matrix, must be called before using any other member function.
	 */
	void initialize() {
		if (internal_status != MatrixStatus::UnInitialized) {
			pantheios::log_ERROR("StaticSparseMatrix::initialize: Throwing invalid state for status flag != 0 (is ", pantheios::integer(internal_status)," - Already initialized?");
			throw mrmc::exceptions::invalid_state("StaticSparseMatrix::initialize: Invalid state for status flag != 0 - Already initialized?");
			triggerErrorState();
		} else if (row_count == 0) {
			pantheios::log_ERROR("StaticSparseMatrix::initialize: Throwing invalid_argument for row_count = 0");
			throw mrmc::exceptions::invalid_argument("mrmc::StaticSparseMatrix::initialize: Matrix with 0 rows is not reasonable");
			triggerErrorState();
		} else if (((row_count * row_count) - row_count) < non_zero_entry_count) {
			pantheios::log_ERROR("StaticSparseMatrix::initialize: Throwing invalid_argument: More non-zero entries than entries in target matrix");
			throw mrmc::exceptions::invalid_argument("mrmc::StaticSparseMatrix::initialize: More non-zero entries than entries in target matrix");
			triggerErrorState();
		} else {
			storage_size = non_zero_entry_count;
			last_row = 0;

			//value_storage = static_cast<T*>(calloc(storage_size, sizeof(*value_storage)));
			value_storage = new (std::nothrow) T[storage_size]();

			//column_indications = static_cast<uint_fast32_t*>(calloc(storage_size, sizeof(*column_indications)));
			column_indications = new (std::nothrow) uint_fast32_t[storage_size]();

			//row_indications = static_cast<uint_fast32_t*>(calloc(row_count + 1, sizeof(*row_indications)));
			row_indications = new (std::nothrow) uint_fast32_t[row_count + 1]();

			// row_count + 1 so that access with 1-based indices can be direct without the overhead of a -1 each time
			//diagonal_storage = static_cast<T*>(calloc(row_count + 1, sizeof(*diagonal_storage)));
			diagonal_storage = new (std::nothrow) T[row_count + 1]();
		
			if ((value_storage == NULL) || (column_indications == NULL) || (row_indications == NULL) || (diagonal_storage == NULL)) {
				pantheios::log_ERROR("StaticSparseMatrix::initialize: Throwing bad_alloc: memory allocation failed");
				throw std::bad_alloc();
				triggerErrorState();
			} else {
				setState(MatrixStatus::Initialized);
			}
		}
	}

	//! Linear Setter for matrix entry A_{row, col} to value
	/*!
		Linear Setter function for matrix entry A_{row, col} to value. Must be called consecutively for each element in a row in ascending order of columns AND in ascending order of rows.
		Diagonal entries may be set at any time.
	 */
	void addNextValue(const uint_fast32_t row, const uint_fast32_t col, const T value) {
		if ((row > row_count) || (col > row_count) || (row == 0) || (col == 0)) {
			pantheios::log_ERROR("StaticSparseMatrix::addNextValue: Throwing out_of_range: row or col not in 1 .. rows");
			throw mrmc::exceptions::out_of_range("mrmc::StaticSparseMatrix::addNextValue: row or col not in 1 .. rows");
			triggerErrorState();
		}
		
		if (row == col) {
			diagonal_storage[row] = value;
		} else {
			if (row != last_row) {
				for (uint_fast32_t i = last_row; i < row; ++i) {
					row_indications[i] = current_size;
				}
				last_row = row;
			}

			value_storage[current_size] = value;
			column_indications[current_size] = col;

			// Increment counter for stored elements
			current_size++;
		}
	}

	void finalize() {
		if (!isInitialized()) {
			pantheios::log_ERROR("StaticSparseMatrix::finalize: Throwing invalid state for internal state not Initialized (is ", pantheios::integer(internal_status)," - Already finalized?");
			throw mrmc::exceptions::invalid_state("StaticSparseMatrix::finalize: Invalid state for internal state not Initialized - Already finalized?");
			triggerErrorState();
		} else if (storage_size != current_size) {
			pantheios::log_ERROR("StaticSparseMatrix::finalize: Throwing invalid_state: Wrong call count for addNextValue");
			throw mrmc::exceptions::invalid_state("mrmc::StaticSparseMatrix::finalize: Wrong call count for addNextValue");
			triggerErrorState();
		} else {
			if (last_row != row_count) {
				for (uint_fast32_t i = last_row; i < row_count; ++i) {
					row_indications[i] = current_size;
				}
			}

			row_indications[row_count] = storage_size;

			setState(MatrixStatus::ReadReady);
		}
	}

	uint_fast32_t getRowCount() const {
		return row_count;
	}

	T* getStoragePointer() const {
		return value_storage;
	}

	bool isReadReady() {
		return (internal_status == MatrixStatus::ReadReady);
	}

	bool isInitialized() {
		return (internal_status == MatrixStatus::Initialized || internal_status == MatrixStatus::ReadReady);
	}

	MatrixStatus getState() {
		return internal_status;
	}

	bool hasError() {
		return (internal_status == MatrixStatus::Error);
	}

	Eigen::SparseMatrix<T> toEigenSparseMatrix() {
		Eigen::SparseMatrix<T> mat(row_count, row_count);

		if (!isReadReady()) {
			pantheios::log_ERROR("StaticSparseMatrix::toEigenSparseMatrix: Throwing invalid state for internal state not ReadReady (is ", pantheios::integer(internal_status),").");
			throw mrmc::exceptions::invalid_state("StaticSparseMatrix::toEigenSparseMatrix: Invalid state for internal state not ReadReady.");
			triggerErrorState();
		} else {
			typedef Eigen::Triplet<double> ETd;
			std::vector<ETd> tripletList;
			tripletList.reserve(non_zero_entry_count + row_count);

			uint_fast32_t row_start;
			uint_fast32_t row_end;
			for (uint_fast32_t row = 1; row <= row_count; ++row) {
				row_start	= row_indications[row - 1];
				row_end		= row_indications[row];
				while (row_start < row_end) {
					tripletList.push_back(ETd(row - 1, column_indications[row_start] - 1, value_storage[row_start]));
					++row_start;
				}
			}
			for (uint_fast32_t i = 1; i <= row_count; ++i) {
				tripletList.push_back(ETd(i, i, diagonal_storage[i]));
			}

			mat.setFromTriplets(tripletList.begin(), tripletList.end());
			mat.makeCompressed();
		}
		
		return mat;
	}

 private:
	uint_fast32_t storage_size;
	uint_fast32_t current_size;

	uint_fast32_t row_count;
	uint_fast32_t non_zero_entry_count;
	uint_fast32_t last_row;

	/*! Array containing all non-zero values, apart from the diagonal entries */
	T* value_storage;
	/*! Array containing all diagonal values */
	T* diagonal_storage;

	/*! Array containing the column number of the corresponding value_storage entry */
	uint_fast32_t* column_indications;
	/*! Array containing the row boundaries of valueStorage */
	uint_fast32_t* row_indications;

	/*! Internal status enum, 0 for constructed, 1 for initialized and 2 for finalized, -1 on errors */
	MatrixStatus internal_status;

	/*! Sets the internal status to Error */
	void triggerErrorState() {
		setState(MatrixStatus::Error);
	}

	/*! 
		Sets the internal status to new_state iff the current state is not the Error state.
		@param new_state the new state to be switched to
	*/
	void setState(const MatrixStatus new_state) {
		internal_status = (internal_status == MatrixStatus::Error) ? internal_status : new_state;
	}
};

} // namespace sparse

} // namespace mrmc

#endif // MRMC_SPARSE_STATIC_SPARSE_MATRIX_H_
