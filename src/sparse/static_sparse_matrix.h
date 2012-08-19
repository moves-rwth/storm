#ifndef MRMC_SPARSE_STATIC_SPARSE_MATRIX_H_
#define MRMC_SPARSE_STATIC_SPARSE_MATRIX_H_

#include <exception>
#include "boost/integer/integer_mask.hpp"

#include <pantheios/pantheios.hpp>
#include <pantheios/inserters/integer.hpp>

#include "src/exceptions/invalid_state.h"
#include "src/exceptions/invalid_argument.h"
#include "src/exceptions/out_of_range.h"

namespace mrmc {

namespace sparse {

//! A sparse Matrix for DTMCs with a constant number of non-zero entries on the non-diagonal fields.
/*! 
	The sparse matrix used for calculation on the DTMCs.
	Adressing is NOT zero-based! The valid range for getValue and addNextValue is 1 to rows (first argument to constructor).
 */
template <class T>
class StaticSparseMatrix {
 public:
		
	//! Constructor
	 /*!
		\param rows Row-Count and therefor column-count of the symetric matrix
		\param non_zero_entries The exact count of entries that will be submitted through addNextValue *excluding* those on the diagonal (A_{i,j} with i = j)
	 */
	StaticSparseMatrix(uint_fast32_t rows, uint_fast32_t non_zero_entries) {
		current_size = 0;
		storage_size = 0;
		
		row_count = rows;
		non_zero_entry_count = non_zero_entries;

		//initialize(rows, non_zero_entries);
	}

	~StaticSparseMatrix() {
		if (value_storage != NULL) {
			free(value_storage);
		}
		if (column_indications != NULL) {
			free(column_indications);
		}
		if (row_indications != NULL) {
			free(row_indications);
		}
		if (diagonal_storage != NULL) {
			free(diagonal_storage);
		}
	}

	//! Getter for saving matrix entry A_{row,col} to target
	/*!
		Getter function for the matrix. 
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
			throw new mrmc::exceptions::out_of_range("mrmc::StaticSparseMatrix::getValue: row or col not in 1 .. rows");
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

		if (row_count == 0) {
			pantheios::log_ERROR("StaticSparseMatrix::initialize: Throwing invalid_argument for row_count = 0");
			throw new mrmc::exceptions::invalid_argument("mrmc::StaticSparseMatrix::initialize: Matrix with 0 rows is not reasonable");
		}

		if (((row_count * row_count) - row_count) < non_zero_entry_count) {
			pantheios::log_ERROR("StaticSparseMatrix::initialize: Throwing invalid_argument: More non-zero entries than entries in target matrix");
			throw new mrmc::exceptions::invalid_argument("mrmc::StaticSparseMatrix::initialize: More non-zero entries than entries in target matrix");
		}

		storage_size = non_zero_entry_count;
		last_row = 0;

		value_storage = static_cast<T*>(calloc(storage_size, sizeof(*value_storage)));

		column_indications = static_cast<uint_fast32_t*>(calloc(storage_size, sizeof(*column_indications)));

		row_indications = static_cast<uint_fast32_t*>(calloc(row_count + 1, sizeof(*row_indications)));

		// row_count + 1 so that access with 1-based indices can be direct without the overhead of a -1 each time
		diagonal_storage = static_cast<T*>(calloc(row_count + 1, sizeof(*diagonal_storage)));
		
		if ((value_storage == NULL) || (column_indications == NULL) || (row_indications == NULL) || (diagonal_storage == NULL)) {
			pantheios::log_ERROR("StaticSparseMatrix::initialize: Throwing bad_alloc: memory allocation failed");
#ifdef __linux__
			throw new std::bad_alloc();
#else
			throw new std::bad_alloc("mrmc::StaticSparseMatrix::initialize: memory allocation failed");
#endif
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
			throw new mrmc::exceptions::out_of_range("mrmc::StaticSparseMatrix::addNextValue: row or col not in 1 .. rows");
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
		if (storage_size != current_size) {
			pantheios::log_ERROR("StaticSparseMatrix::finalize: Throwing invalid_state: Wrong call count for addNextValue");
			throw new mrmc::exceptions::invalid_state("mrmc::StaticSparseMatrix::finalize: Wrong call count for addNextValue");
		}

		if (last_row != row_count) {
			for (uint_fast32_t i = last_row; i < row_count; ++i) {
				row_indications[i] = current_size;
			}
		}

		row_indications[row_count] = storage_size;
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
	/*! Array containing the row boundarys of valueStorage */
	uint_fast32_t* row_indications;
};

} // namespace sparse

} // namespace mrmc

#endif // MRMC_SPARSE_STATIC_SPARSE_MATRIX_H_
