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
		\param rows Row-Count and therefore column-count of the square matrix
	 */
	StaticSparseMatrix(uint_fast64_t rows) {
		// Using direct access instead of setState() because of undefined initialization value
		// setState() stays in Error should Error be the current value
		internal_status = MatrixStatus::UnInitialized;
		current_size = 0;
		
		value_storage = NULL;
		diagonal_storage = NULL;
		column_indications = NULL;
		row_indications = NULL;

		row_count = rows;
		non_zero_entry_count = 0;

		//initialize(rows, non_zero_entries);
	}

	//! Copy Constructor
	/*!
		Copy Constructor. Creates an exact copy of the source sparse matrix ssm. Modification of either matrix does not affect the other.
		@param ssm A reference to the matrix that should be copied from
	 */
	StaticSparseMatrix(const StaticSparseMatrix<T> &ssm) : internal_status(ssm.internal_status), current_size(ssm.current_size), row_count(ssm.row_count), non_zero_entry_count(ssm.non_zero_entry_count)
	{
		pantheios::log_DEBUG("StaticSparseMatrix::CopyCTor: Using Copy() Ctor.");
		if (!ssm.hasError()) {
			pantheios::log_ERROR("StaticSparseMatrix::CopyCtor: Throwing invalid_argument: Can not Copy from matrix in Error state.");
			throw mrmc::exceptions::invalid_argument();
		} else {
			if (!prepareInternalStorage()) {
				pantheios::log_ERROR("StaticSparseMatrix::CopyCtor: Throwing bad_alloc: memory allocation failed.");
				throw std::bad_alloc();
			} else {
				for (uint_fast64_t i = 0; i < non_zero_entry_count; ++i) {
					// use T() to force use of the copy constructor for complex T types
					value_storage[i] = T(ssm.value_storage[i]);
				}
				for (uint_fast64_t i = 0; i <= row_count; ++i) {
					// same here
					diagonal_storage[i] = T(ssm.diagonal_storage[i]);
				}

				memcpy(column_indications, ssm.column_indications, sizeof(column_indications[0]) * non_zero_entry_count);
				memcpy(row_indications, ssm.row_indications, sizeof(row_indications[0]) * (row_count + 1));
			}
		}
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

	//! Mandatory initialization of the matrix, variant for initialize(), addNextValue() and finalize()
	/*!
		Mandatory initialization of the matrix, must be called before using any other member function.
		This version is to be used together with addNextValue().
		For initialization from a Eigen SparseMatrix, use initialize(Eigen::SparseMatrix<T> &).
		\param non_zero_entries The exact count of entries that will be submitted through addNextValue *excluding* those on the diagonal (A_{i,j} with i = j)
	 */
	void initialize(uint_fast64_t non_zero_entries) {
		if (internal_status != MatrixStatus::UnInitialized) {
			triggerErrorState();
			pantheios::log_ERROR("StaticSparseMatrix::initialize: Throwing invalid state for status flag != 0 (is ", pantheios::integer(internal_status)," - Already initialized?");
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

	//! Mandatory initialization of the matrix, variant for initialize(), addNextValue() and finalize()
	/*!
		Mandatory initialization of the matrix, must be called before using any other member function.
		This version is to be used for initialization from a Eigen SparseMatrix, use initialize(uint_fast32_t) for addNextValue.
		\param eigen_sparse_matrix The Eigen Sparse Matrix to be copied/ initialized from. MUST BE in compressed form!
	 */
	template<int _Options, typename _Index>
	void initialize(const Eigen::SparseMatrix<T, _Options, _Index> &eigen_sparse_matrix) {
		if (!eigen_sparse_matrix.isCompressed()) {
			triggerErrorState();
			pantheios::log_ERROR("StaticSparseMatrix::initialize: Throwing invalid_argument: eigen_sparse_matrix is not in Compressed form.");
			throw mrmc::exceptions::invalid_argument("StaticSparseMatrix::initialize: Throwing invalid_argument: eigen_sparse_matrix is not in Compressed form.");
		}

		non_zero_entry_count = getEigenSparseMatrixCorrectNonZeroEntryCount(eigen_sparse_matrix);
		last_row = 0;

		if (!prepareInternalStorage()) {
			triggerErrorState();
			pantheios::log_ERROR("StaticSparseMatrix::initialize: Throwing bad_alloc: memory allocation failed");
			throw std::bad_alloc();
		} else {
			// easy case, we can simply copy the data
			// RowMajor: Easy, ColMajor: Hmm. But how to detect?
			const T* valuePtr = eigen_sparse_matrix.valuePtr();
			const _Index* indexPtr = eigen_sparse_matrix.innerIndexPtr();
			const _Index* outerPtr = eigen_sparse_matrix.outerIndexPtr();


			const _Index entryCount = eigen_sparse_matrix.nonZeros();
			const _Index outerCount = eigen_sparse_matrix.outerSize();
			if (isEigenRowMajor(eigen_sparse_matrix)) {
				// Easy case, all data can be copied with some minor changes.
				// We have to separate diagonal entries from others
				for (_Index row = 1; row <= outerCount; ++row) {
					for (_Index col = outerPtr[row - 1]; col < outerPtr[row]; ++col) {
						addNextValue(row, indexPtr[col] + 1, valuePtr[col]);
					}
				}
			} else {
				// temp copies, anyone?
				const _Index eigen_col_count = eigen_sparse_matrix.cols();
				const _Index eigen_row_count = eigen_sparse_matrix.rows();
				
				// initialise all column-start positions to known lower boundarys
				_Index* positions = new _Index[eigen_col_count]();
				for (_Index i = 0; i < eigen_col_count; ++i) {
					positions[i] = outerPtr[i];
				}

				int i = 0;
				int currentRow = 0;
				int currentColumn = 0;
				while (i < entryCount) {
					if ((positions[currentColumn] < outerPtr[currentColumn + 1]) && (indexPtr[positions[currentColumn]] == currentRow)) {
						addNextValue(currentRow + 1, currentColumn + 1, valuePtr[positions[currentColumn]]);
						// one more found
						++i;
						// mark this position as "used"
						++positions[currentColumn];
					}
					// advance to next column
					++currentColumn;
					if (currentColumn == eigen_col_count) {
						currentColumn = 0;
						++currentRow;
					}
				}
				delete[] positions;
			}
			setState(MatrixStatus::Initialized);
		}
	}

	//! Linear Setter for matrix entry A_{row, col} to value
	/*!
		Linear Setter function for matrix entry A_{row, col} to value. Must be called consecutively for each element in a row in ascending order of columns AND in ascending order of rows.
		Diagonal entries may be set at any time.
	 */
	void addNextValue(const uint_fast64_t row, const uint_fast64_t col, const T &value) {
		if ((row > row_count) || (col > row_count) || (row == 0) || (col == 0)) {
			triggerErrorState();
			pantheios::log_ERROR("StaticSparseMatrix::addNextValue: Throwing out_of_range: row or col not in 1 .. rows (is ", pantheios::integer(row), " x ", pantheios::integer(col), ", max is ", pantheios::integer(row_count), " x ", pantheios::integer(row_count), ").");
			throw mrmc::exceptions::out_of_range("StaticSparseMatrix::addNextValue: row or col not in 1 .. rows");
		}
		
		if (row == col) {
			diagonal_storage[row] = value;
		} else {
			if (row != last_row) {
				for (uint_fast64_t i = last_row; i < row; ++i) {
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
			triggerErrorState();
			pantheios::log_ERROR("StaticSparseMatrix::finalize: Throwing invalid state for internal state not Initialized (is ", pantheios::integer(internal_status)," - Already finalized?");
			throw mrmc::exceptions::invalid_state("StaticSparseMatrix::finalize: Invalid state for internal state not Initialized - Already finalized?");
		} else if (current_size != non_zero_entry_count) {
			triggerErrorState();
			pantheios::log_ERROR("StaticSparseMatrix::finalize: Throwing invalid_state: Wrong call count for addNextValue");
			throw mrmc::exceptions::invalid_state("StaticSparseMatrix::finalize: Wrong call count for addNextValue");
		} else {
			if (last_row != row_count) {
				for (uint_fast64_t i = last_row; i < row_count; ++i) {
					row_indications[i] = current_size;
				}
			}

			row_indications[row_count] = non_zero_entry_count;

			setState(MatrixStatus::ReadReady);
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
	inline bool getValue(uint_fast64_t row, uint_fast64_t col, T* const target) {
		
		if (row == col) {
			// storage is row_count + 1 large for direct access without the -1
			*target = diagonal_storage[row];
			return true;
		}

		if ((row > row_count) || (col > row_count) || (row == 0) || (col == 0)) {
			pantheios::log_ERROR("StaticSparseMatrix::getValue: row or col not in 1 .. rows (is ", pantheios::integer(row), " x ", pantheios::integer(col), ", max is ", pantheios::integer(row_count), " x ", pantheios::integer(row_count), ").");
			throw mrmc::exceptions::out_of_range("StaticSparseMatrix::getValue: row or col not in 1 .. rows");
			return false;
		}

		uint_fast64_t row_start = row_indications[row - 1];
		uint_fast64_t row_end = row_indications[row];

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

	uint_fast64_t getRowCount() const {
		return row_count;
	}

	T* getStoragePointer() const {
		return value_storage;
	}

	T* getDiagonalStoragePointer() const {
		return diagonal_storage;
	}

	uint_fast64_t* getRowIndicationsPointer() const {
		return row_indications;
	}

	uint_fast64_t* getColumnIndicationsPointer() const {
		return column_indications;
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

	//! Converts this matrix to an equivalent sparse matrix in Eigens format.
	/*!
		Exports this sparse matrix to Eigens SparseMatrix format.
		Required this matrix to be in the ReadReady state.

		@return The Eigen SparseMatrix

	 */
	Eigen::SparseMatrix<T, Eigen::RowMajor, int_fast32_t>* toEigenSparseMatrix() {
		int_fast32_t eigenRows = static_cast<int_fast32_t>(row_count);
		Eigen::SparseMatrix<T, Eigen::RowMajor, int_fast32_t>* mat = new Eigen::SparseMatrix<T, Eigen::RowMajor, int_fast32_t>(eigenRows, eigenRows);

		if (!isReadReady()) {
			triggerErrorState();
			pantheios::log_ERROR("StaticSparseMatrix::toEigenSparseMatrix: Throwing invalid state for internal state not ReadReady (is ", pantheios::integer(internal_status),").");
			throw mrmc::exceptions::invalid_state("StaticSparseMatrix::toEigenSparseMatrix: Invalid state for internal state not ReadReady.");
		} else {

#			ifdef MRMC_USE_TRIPLETCONVERT
			typedef Eigen::Triplet<T> IntTriplet;
			std::vector<IntTriplet> tripletList;
			tripletList.reserve(non_zero_entry_count + row_count);

			uint_fast64_t row_start;
			uint_fast64_t row_end;
			for (uint_fast64_t row = 1; row <= row_count; ++row) {
				row_start	= row_indications[row - 1];
				row_end		= row_indications[row];
				while (row_start < row_end) {
					tripletList.push_back(IntTriplet(row - 1, column_indications[row_start] - 1, value_storage[row_start]));
					++row_start;
				}
			}

			for (uint_fast64_t i = 1; i <= row_count; ++i) {
				tripletList.push_back(IntTriplet(i - 1, i - 1, diagonal_storage[i]));
			}

			mat->setFromTriplets(tripletList.begin(), tripletList.end());

#			else // NOT MRMC_USE_TRIPLETCONVERT

			// In most cases, this is faster (about 1/2 of the above). But if there are "heavy" rows that are several times larger than an average row, the other solution might be faster.

			mat->reserve(Eigen::VectorXi::Constant(static_cast<int_fast32_t>(mat->outerSize()), static_cast<int_fast32_t>((non_zero_entry_count + row_count) / mat->outerSize())));

			uint_fast64_t row_start;
			uint_fast64_t row_end;
			for (uint_fast64_t row = 1; row <= row_count; ++row) {
				row_start	= row_indications[row - 1];
				row_end		= row_indications[row];

				// insert the diagonal entry
				mat->insert(row - 1, row - 1) = diagonal_storage[row];

				while (row_start < row_end) {
					//tripletList.push_back(IntTriplet(row - 1, column_indications[row_start] - 1, value_storage[row_start]));
					mat->insert(row - 1, column_indications[row_start] - 1) = value_storage[row_start];
					++row_start;
				}
			}
#			endif // MRMC_USE_TRIPLETCONVERT

			mat->makeCompressed();
		}
		
		return mat;
	}

	//! Returns the exact count of explicit entries in the sparse matrix.
	/*!
		Retuns the exact count of explicit entries in the sparse matrix. While it is called "nonZero" count, the fields may of course be 0 (the 0-value of T).
		@returns explicit entry count in the matrix
	 */
	uint_fast64_t getNonZeroEntryCount() const {
		return non_zero_entry_count;
	}

	//! Converts a state into a final state.
	/*!
		This function allows for states to be made final. This means that all entries in row "state" will be changed to 0 but for the one on the diagonal, which will be set to 1 to create a loop on itself.
		@param state The number of the state to be converted. Must be in 1 <= state <= rows.
		@returns Whether the conversion was successful.
	 */
	bool makeStateFinal(const uint_fast64_t state) {
		if ((state > row_count) || (state == 0)) {
			pantheios::log_ERROR("StaticSparseMatrix::makeStateFinal: state not in 1 .. rows (is ", pantheios::integer(state), ", max is ", pantheios::integer(row_count), ").");
			throw mrmc::exceptions::out_of_range("StaticSparseMatrix::makeStateFinal: state not in 1 .. rows");
			return false;
		}

		uint_fast64_t row_start = row_indications[state - 1];
		uint_fast64_t row_end = row_indications[state];


		while (row_start < row_end) {
			value_storage[row_start] = mrmc::misc::constGetZero(value_storage);
			row_start++;
		}

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
	uint_fast64_t current_size;

	uint_fast64_t row_count;
	uint_fast64_t non_zero_entry_count;
	uint_fast64_t last_row;

	/*! Array containing all non-zero values, apart from the diagonal entries */
	T* value_storage;
	/*! Array containing all diagonal values */
	T* diagonal_storage;

	/*! Array containing the column number of the corresponding value_storage entry */
	uint_fast64_t* column_indications;
	/*! Array containing the row boundaries of valueStorage */
	uint_fast64_t* row_indications;

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

	/*!
		Prepares the internal CSR storage.
		Requires non_zero_entry_count and row_count to be set.
		@return true on success, false otherwise (allocation failed).
	 */
	bool prepareInternalStorage() {
		value_storage = new (std::nothrow) T[non_zero_entry_count]();

		column_indications = new (std::nothrow) uint_fast64_t[non_zero_entry_count]();

		row_indications = new (std::nothrow) uint_fast64_t[row_count + 1]();

		// row_count + 1 so that access with 1-based indices can be direct without the overhead of a -1 each time
		diagonal_storage = new (std::nothrow) T[row_count + 1]();

		return ((value_storage != NULL) && (column_indications != NULL) && (row_indications != NULL) && (diagonal_storage != NULL));
	}

	//! 


	template <typename _Scalar, typename _Index>
	bool isEigenRowMajor(Eigen::SparseMatrix<_Scalar, Eigen::RowMajor, _Index>) {
		return true;
	}
	
	template <typename _Scalar, typename _Index>
	bool isEigenRowMajor(Eigen::SparseMatrix<_Scalar, Eigen::ColMajor, _Index>) {
		return false;
	}

	template<typename _Scalar, int _Options, typename _Index>
	_Index getEigenSparseMatrixCorrectNonZeroEntryCount(const Eigen::SparseMatrix<_Scalar, _Options, _Index> &eigen_sparse_matrix) {
		const _Index* indexPtr = eigen_sparse_matrix.innerIndexPtr();
		const _Index* outerPtr = eigen_sparse_matrix.outerIndexPtr();

		const _Index entryCount = eigen_sparse_matrix.nonZeros();
		const _Index outerCount = eigen_sparse_matrix.outerSize();
		
		uint_fast64_t diag_non_zeros = 0;
		
		// for RowMajor, row is the current Row and col the column
		// for ColMajor, row is the current Col and col the row
		_Index innerStart = 0;
		_Index innerEnd   = 0;
		_Index innerMid   = 0;
		for (_Index row = 0; row < outerCount; ++row) {
			innerStart = outerPtr[row];
			innerEnd   = outerPtr[row + 1] - 1;
			
			// Now with super fancy binary search, deferred equality detection
			while (innerStart < innerEnd) {
				innerMid = innerStart + ((innerEnd - innerStart) / 2);

				if (indexPtr[innerMid] < row) {
					innerStart = innerMid + 1;
				} else {
					innerEnd = innerMid;
				}
			}

			if ((innerStart == innerEnd) && (indexPtr[innerStart] == row)) {
				// found a diagonal entry
				++diag_non_zeros;
			}
		}

		return static_cast<_Index>(entryCount - diag_non_zeros);
	}

};

} // namespace sparse

} // namespace mrmc

#endif // MRMC_SPARSE_STATIC_SPARSE_MATRIX_H_
