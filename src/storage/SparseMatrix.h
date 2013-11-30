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
    typedef T* ValueIterator;
    
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
		ConstIterator(T const* valuePtr, uint_fast64_t const* columnPtr);
        
		/*!
		 * Moves the iterator to the next non-zero element.
		 *
		 * @return A reference to itself.
		 */
		ConstIterator& operator++();
        
        /*!
         * Dereferences the iterator by returning a reference to itself. This is needed for making use of the range-based
         * for loop over transitions.
         *
         * @return A reference to itself.
         */
        ConstIterator& operator*();
        
		/*!
		 * Compares the two iterators for inequality.
		 *
		 * @return True iff the given iterator points to a different index as the current iterator.
		 */
		bool operator!=(ConstIterator const& other) const;
        
		/*!
		 * Assigns the position of the given iterator to the current iterator.
         *
         * @return A reference to itself.
		 */
		ConstIterator& operator=(ConstIterator const& other);
        
        /*!
         * Retrieves the column that is associated with the current non-zero element to which this iterator
         * points.
		 *
		 * @return The column of the current non-zero element to which this iterator points.
         */
		uint_fast64_t column() const;
		
        /*!
         * Retrieves the value of the current non-zero element to which this iterator points.
		 *
		 * @return The value of the current non-zero element to which this iterator points.
         */
		T const& value() const;
        
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
        Rows(T const* valuePtr, uint_fast64_t const* columnPtr, uint_fast64_t entryCount);
        
        /*!
         * Retrieves an iterator that points to the beginning of the rows.
         *
         * @return An iterator that points to the beginning of the rows.
         */
        ConstIterator begin() const;
        
        /*!
         * Retrieves an iterator that points past the last element of the rows.
         *
         * @return An iterator that points past the last element of the rows.
         */
        ConstIterator end() const;
        
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
        ConstRowIterator(T const* startValuePtr, uint_fast64_t const* startColumnPtr, uint_fast64_t const* rowPtr);
        
        /*!
         * This sets the iterator to point to the next row.
         *
         * @return A reference to itself.
         */
        ConstRowIterator& operator++();
        
        /*!
         * Compares the iterator to the given row iterator.
         */
        bool operator!=(ConstRowIterator const& other) const;
        
        /*!
         * Retrieves an iterator that points to the beginning of the current row.
         *
         * @return An iterator that points to the beginning of the current row.
         */
        ConstIterator begin() const;
        
        /*!
         * Retrieves an iterator that points past the end of the current row.
         *
         * @return An iterator that points past the end of the current row.
         */
        ConstIterator end() const;
        
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
	SparseMatrix(uint_fast64_t rows, uint_fast64_t cols);

	/*!
	 * Constructs a square sparse matrix object with the given number rows.
	 *
	 * @param size The number of rows and columns of the matrix.
	 */
	SparseMatrix(uint_fast64_t size = 0);

	/*!
	 * Move Constructor.
	 *
	 * @param other The Matrix from which to move the content
	 */
	SparseMatrix(SparseMatrix<T>&& other);

	/*!
	 * Copy Constructor.
	 *
	 * @param other The Matrix from which to copy the content
	 */
	SparseMatrix(const SparseMatrix<T> & other);

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
                 std::vector<uint_fast64_t>&& columnIndications, std::vector<T>&& values);

    /*!
	 * Copy Assignment Operator.
	 *
	 * @param other The Matrix from which to copy the content
	 */
	storm::storage::SparseMatrix<T>& operator=(SparseMatrix<T> const& other);

	/*!
	 * Initializes the sparse matrix with the given number of non-zero entries
	 * and prepares it for use with addNextValue() and finalize().
	 * Note: Calling this method before any other member function is mandatory.
	 * This version is to be used together with addNextValue().
	 *
	 * @param nonZeroEntries The number of non-zero entries this matrix is going to hold.
	 */
	void initialize(uint_fast64_t nonZeroEntries = 0);
    
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
	void addNextValue(const uint_fast64_t row, const uint_fast64_t col,	T const& value);
    
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
    void insertNextValue(const uint_fast64_t row, const uint_fast64_t col,	T const& value, bool pushRowIndication = false);
    
    /*!
     * Inserts an empty row in the matrix.
     *
     * @param pushRowIndication If set to true, the next row indication value is pushed, otherwise it is added. If the
     * number of rows was not set in the beginning, then this needs to be true and false otherwise.
     */
    void insertEmptyRow(bool pushRowIndication = false);
    
	/*
	 * Finalizes the sparse matrix to indicate that initialization has been completed and the matrix may now be used.
     *
     * @param pushSentinelElement A boolean flag that indicates whether the sentinel element is to be pushed or inserted
     * at a fixed location. If the elements have been added to the matrix via insertNextValue, this needs to be true
     * and false otherwise.
	 */
	void finalize(bool pushSentinelElement = false);

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
	inline bool getValue(uint_fast64_t row, uint_fast64_t col, T* const target) const;

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
	inline T& getValue(uint_fast64_t row, uint_fast64_t col);

	/*!
	 * Returns the number of rows of the matrix.
	 *
	 * @returns The number of rows of the matrix.
	 */
	uint_fast64_t getRowCount() const;

	/*!
	 * Returns the number of columns of the matrix.
	 *
	 * @returns The number of columns of the matrix.
	 */
	uint_fast64_t getColumnCount() const;

	/*!
	 * Checks whether the internal status of the matrix makes it ready for
	 * reading access.
	 *
	 * @returns True iff the internal status of the matrix makes it ready for
	 * reading access.
	 */
	bool isReadReady();

	/*!
	 * Checks whether the matrix was initialized previously. The matrix may
	 * still require to be finalized, even if this check returns true.
	 *
	 * @returns True iff the matrix was initialized previously.
	 */
	bool isInitialized();

	/*!
	 * Returns the internal state of the matrix.
	 *
	 * @returns The internal state of the matrix.
	 */
	MatrixStatus getState();

	/*!
	 * Checks whether the internal state of the matrix signals an error.
	 *
	 * @returns True iff the internal state of the matrix signals an error.
	 */
	bool hasError() const;

	/*!
	 * Returns the number of non-zero entries in the matrix.
	 *
	 * @returns The number of non-zero entries in the matrix.
	 */
	uint_fast64_t getNonZeroEntryCount() const;

	/*!
	 * This function makes the rows given by the bit vector absorbing.
	 *
	 * @param rows A bit vector indicating which rows to make absorbing.
	 * @returns True iff the operation was successful.
	 */
	bool makeRowsAbsorbing(storm::storage::BitVector const& rows);

	/*!
	 * This function makes the groups of rows given by the bit vector absorbing.
	 *
	 * @param rowGroupConstraint A bit vector indicating which row groups to make absorbing.
	 * @param rowGroupIndices A vector indicating which rows belong to a given row group.
	 * @return True iff the operation was successful.
	 */
	bool makeRowsAbsorbing(storm::storage::BitVector const& rowGroupConstraint, std::vector<uint_fast64_t> const& rowGroupIndices);

	/*!
	 * This function makes the given row absorbing. This means that all entries will be set to 0
	 * except the one at the specified column, which is set to 1 instead.
	 *
	 * @param row The row to be made absorbing.
	 * @param column The index of the column whose value is to be set to 1.
	 * @returns True iff the operation was successful.
	 */
	bool makeRowAbsorbing(const uint_fast64_t row, const uint_fast64_t column);

	/*
	 * Computes the sum of the elements in the given row whose column bits are set to one on the
	 * given constraint.
	 *
	 * @param row The row whose elements to add.
	 * @param constraint A bit vector that indicates which columns to add.
	 * @returns The sum of the elements in the given row whose column bits
	 * are set to one on the given constraint.
	 */
	T getConstrainedRowSum(uint_fast64_t row, storm::storage::BitVector const& constraint) const;

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
	std::vector<T> getConstrainedRowSumVector(storm::storage::BitVector const& rowConstraint, storm::storage::BitVector const& columnConstraint) const;

	/*!
	 * Computes a vector whose elements represent the sums of selected (given by the column
	 * constraint) entries for all rows in selected row groups given by the row group constraint.
	 *
	 * @param rowGroupConstraint A bit vector that indicates which row groups are to be considered.
	 * @param rowGroupIndices A vector indicating which rows belong to a given row group.
	 * @param columnConstraint A bit vector that indicates which columns to add.
	 * @returns 
	 */
	std::vector<T> getConstrainedRowSumVector(storm::storage::BitVector const& rowGroupConstraint, std::vector<uint_fast64_t> const& rowGroupIndices, storm::storage::BitVector const& columnConstraint, uint_fast64_t numberOfRows) const;

	/*!
	 * Creates a submatrix of the current matrix by dropping all rows and columns whose bits are not
	 * set to one in the given bit vector.
	 *
	 * @param constraint A bit vector indicating which rows and columns to keep.
	 * @returns A matrix corresponding to a submatrix of the current matrix in which only rows and
	 * columns given by the constraint are kept and all others are dropped.
	 */
	SparseMatrix getSubmatrix(storm::storage::BitVector const& constraint) const;

	/*!
	 * Creates a submatrix of the current matrix by keeping only row groups and columns in the given
	 * row group constraint.
	 *
	 * @param rowGroupConstraint A bit vector indicating which row groups and columns to keep.
	 * @param rowGroupIndices A vector indicating which rows belong to a given row group.
	 * @returns A matrix corresponding to a submatrix of the current matrix in which only row groups
	 * and columns given by the row group constraint are kept and all others are dropped.
	 */
	SparseMatrix getSubmatrix(storm::storage::BitVector const& rowGroupConstraint, std::vector<uint_fast64_t> const& rowGroupIndices) const;
    
    SparseMatrix getSubmatrix(std::vector<uint_fast64_t> const& rowGroupToRowIndexMapping, std::vector<uint_fast64_t> const& rowGroupIndices, bool insertDiagonalEntries = true) const;

	/*!
	 * Performs a change to the matrix that is needed if this matrix is to be interpreted as a
	 * set of linear equations. In particular, it transforms A to (1-A), meaning that the elements
	 * on the diagonal are inverted with respect to addition and the other elements are negated.
	 */
	void convertToEquationSystem();

	/*!
	 * Transposes the matrix.
	 *
	 * @return A sparse matrix that represents the transpose of this matrix.
	 */
	storm::storage::SparseMatrix<T> transpose() const;

	/*!
	 * Inverts all elements on the diagonal, i.e. sets the diagonal values to 1 minus their previous
	 * value. Requires the matrix to contain each diagonal element and to be square.
	 */
	void invertDiagonal();

	/*!
	 * Negates all non-zero elements that are not on the diagonal.
	 */
	void negateAllNonDiagonalElements();

	/*!
	 * Calculates the Jacobi-Decomposition of this sparse matrix.
	 * The source Sparse Matrix must be square.
	 * @return A std::pair containing the matrix L+U and the inverted diagonal matrix D^-1
	 */
	SparseJacobiDecomposition_t getJacobiDecomposition() const;

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
	std::vector<T> getPointwiseProductRowSumVector(storm::storage::SparseMatrix<T> const& otherMatrix) const;
	
	/*!
	 * Multiplies the matrix with the given vector and writes the result to given result vector.
	 *
	 * @param vector The vector with which to multiply the matrix.
	 * @param result The vector that is supposed to hold the result of the multiplication after the
	 * operation.
	 * @returns The product of the matrix and the given vector as the content of the given result
	 * vector.
	 */
	void multiplyWithVector(std::vector<T> const& vector, std::vector<T>& result) const;

	/*!
	 * Returns the size of the matrix in memory measured in bytes.
	 *
	 * @returns The size of the matrix in memory measured in bytes.
	 */
	uint_fast64_t getSizeInMemory() const;
    
    /*!
     * Returns an object representing the consecutive rows given by the parameters.
     *
     * @param startRow The starting row.
     * @param endRow The ending row (which is included in the result).
     * @return An object representing the consecutive rows given by the parameters.
     */
    Rows getRows(uint_fast64_t startRow, uint_fast64_t endRow) const;
    
    /*!
     * Returns an object representing the given row.
     *
     * @param row The chosen row.
     * @return An object representing the given row.
     */
    Rows getRow(uint_fast64_t row) const;
    
	/*!
	 * Returns a const iterator to the rows of the matrix.
	 *
     * @param initialRow The initial row to which this iterator points.
	 * @return A const iterator to the rows of the matrix.
	 */
	ConstRowIterator begin(uint_fast64_t initialRow = 0) const;
	
	/*!
	 * Returns a const iterator that points to past the last row of the matrix.
	 *
	 * @return A const iterator that points to past the last row of the matrix
	 */
    ConstRowIterator end() const;
	
	/*!
	 * Returns a const iterator that points past the given row.
	 *
     * @param row The row past which this iterator points.
	 * @return A const iterator that points past the given row.
	 */
    ConstRowIterator end(uint_fast64_t row) const;

	/*!
	 * Returns an iterator to the columns of the non-zero entries of the matrix.
	 *
	 * @param row If given, the iterator will start at the specified row.
	 * @returns An iterator to the columns of the non-zero entries of the matrix.
	 */
	ConstIndexIterator constColumnIteratorBegin(uint_fast64_t row = 0) const;

	/*!
	 * Returns an iterator that points to the first element after the matrix.
	 *
	 * @returns An iterator that points to the first element after the matrix.
	 */
	ConstIndexIterator constColumnIteratorEnd() const;
	
	/*!
	 * Returns an iterator that points to the first element after the given row.
	 *
	 * @param row The row past which this iterator has to point.
	 * @returns An iterator that points to the first element after the matrix.
	 */
	ConstIndexIterator constColumnIteratorEnd(uint_fast64_t row) const;
	
	/*!
	 * Returns an iterator to the values of the non-zero entries of the matrix.
	 *
	 * @param row If given, the iterator will start at the specified row.
	 * @returns An iterator to the values of the non-zero entries of the matrix.
	 */
	ConstValueIterator constValueIteratorBegin(uint_fast64_t row = 0) const;
	
	/*!
	 * Returns an iterator that points to the first element after the matrix.
	 *
	 * @returns An iterator that points to the first element after the matrix.
	 */
	ConstValueIterator constValueIteratorEnd() const;
	
	/*!
	 * Returns an iterator that points to the first element after the given row.
	 *
	 * @param row The row past which this iterator has to point.
	 * @returns An iterator that points to the first element after the matrix.
	 */
	ConstValueIterator constValueIteratorEnd(uint_fast64_t row) const;
	
	/*!
	 * Returns an iterator that points to the first element after the matrix.
	 *
	 * @returns An iterator that points to the first element after the matrix.
	 */
    ValueIterator valueIteratorBegin(uint_fast64_t row = 0);
    
    /*!
	 * Returns an iterator that points to the first element after the given row.
	 *
	 * @param row The row past which this iterator has to point.
	 * @returns An iterator that points to the first element after the matrix.
	 */
	ValueIterator valueIteratorEnd(uint_fast64_t row);
    
	/*!
	 * Computes the sum of the elements in a given row.
	 *
	 * @param row The row that should be summed.
	 * @return Sum of the row.
	 */
	T getRowSum(uint_fast64_t row) const;

	/*!
	 * Checks if the current matrix is a submatrix of the given matrix, where a matrix A is called a
	 * submatrix of B if a value in A is only nonzero, if the value in B at the same position is
	 * also nonzero. Additionally, the matrices must be of equal size.
	 *
	 * @param matrix The matrix that possibly is a "supermatrix" of the current matrix.
	 * @returns True iff the current matrix is a submatrix of the given matrix.
	 */
	bool isSubmatrixOf(SparseMatrix<T> const& matrix) const;

	/*!
	 * Retrieves a compressed string representation of the matrix.
	 *
	 * @returns a compressed string representation of the matrix.
	 */
	std::string toStringCompressed() const;

	/*!
	 * Retrieves a (non-compressed) string representation of the matrix.
	 * Note: the matrix is presented densely. That is, all zeros are filled in and are part of the
	 * string representation, so calling this method on big matrices should be done with care.
	 *
	 * @param rowGroupIndices A vector indicating to which group of rows a given row belongs. If
	 * given, rows of different groups will be separated by a dashed line.
	 * @returns A (non-compressed) string representation of the matrix.
	 */
	std::string toString(std::vector<uint_fast64_t> const* rowGroupIndices = nullptr) const;

	/*!
	 * Calculates a hash over all values contained in this Sparse Matrix.
	 * @return size_t A Hash Value
	 */
	std::size_t getHash() const;

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
	void triggerErrorState();

	/*!
	 * Sets the internal status to the given state if the current state is not
	 * the error state.
	 * @param new_state The new state to be switched to.
	 */
	void setState(const MatrixStatus new_state);

	/*!
	 * Prepares the internal storage. It relies on the number of non-zero entries and the
	 * amount of rows to be set correctly. They may, however, be zero, but then insertNextValue needs to be used rather
     * than addNextElement for filling the matrix.
	 *
	 * @param initializeElements If set to true, all entries are initialized.
	 * @return True on success, false otherwise (allocation failed).
	 */
	bool prepareInternalStorage(bool initializeElements = true);
};

// Extern template declaration to tell the compiler that there already is an instanciation of this template somewhere.
// The extern instance will be found by the linker. Prevents multiple instantiations of the same type.
extern template class SparseMatrix<double>;
extern template class SparseMatrix<int>;

#ifdef STORM_HAVE_INTELTBB
	/*!
	 *	This function is a helper for Parallel Execution of the multipliyWithVector functionality.
	 *  It uses Intels TBB parallel_for paradigm to split up the row/vector multiplication and summation
	 */
	template <typename M, typename V, typename T>
	class tbbHelper_MatrixRowVectorScalarProduct {
	public:
		tbbHelper_MatrixRowVectorScalarProduct(M const* matrixA, V const* vectorX, V * resultVector);

		void operator() (const tbb::blocked_range<uint_fast64_t>& r) const;

	private:
		V * resultVector;
		V const* vectorX;
		M const* matrixA;
	
	};

	// Extern template declaration to tell the compiler that there already is an instanciation of this template somewhere.
	extern template class tbbHelper_MatrixRowVectorScalarProduct<storm::storage::SparseMatrix<double>, std::vector<double>, double>;
#endif


} // namespace storage
} // namespace storm

#endif // STORM_STORAGE_SPARSEMATRIX_H_
