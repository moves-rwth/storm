#ifndef STORM_CUDAFORSTORM_BASICVALUEITERATION_H_
#define STORM_CUDAFORSTORM_BASICVALUEITERATION_H_

#include <cstdint>
#include <vector>
#include <utility>

// Library exports
#include "cudaForStorm.h"

/* Helper declaration to cope with new internal format */
#ifndef STORM_STORAGE_SPARSEMATRIX_H_
namespace storm {
	namespace storage {
		template<typename IndexType, typename ValueType>
		class MatrixEntry {
		public:
			typedef IndexType index_type;
			typedef ValueType value_type;

			/*!
			* Constructs a matrix entry with the given column and value.
			*
			* @param column The column of the matrix entry.
			* @param value The value of the matrix entry.
			*/
			MatrixEntry(index_type column, value_type value);

			/*!
			* Move-constructs the matrix entry fro the given column-value pair.
			*
			* @param pair The column-value pair from which to move-construct the matrix entry.
			*/
			MatrixEntry(std::pair<index_type, value_type>&& pair);

			MatrixEntry();
			MatrixEntry(MatrixEntry const& other);
			MatrixEntry& operator=(MatrixEntry const& other);
#ifndef WINDOWS
			MatrixEntry(MatrixEntry&& other);
			MatrixEntry& operator=(MatrixEntry&& other);
#endif

			/*!
			* Retrieves the column of the matrix entry.
			*
			* @return The column of the matrix entry.
			*/
			index_type const& getColumn() const;

			/*!
			* Sets the column of the current entry.
			*
			* @param column The column to set for this entry.
			*/
			void setColumn(index_type const& column);

			/*!
			* Retrieves the value of the matrix entry.
			*
			* @return The value of the matrix entry.
			*/
			value_type const& getValue() const;

			/*!
			* Sets the value of the entry in the matrix.
			*
			* @param value The value that is to be set for this entry.
			*/
			void setValue(value_type const& value);

			/*!
			* Retrieves a pair of column and value that characterizes this entry.
			*
			* @return A column-value pair that characterizes this entry.
			*/
			std::pair<index_type, value_type> const& getColumnValuePair() const;

			/*!
			* Multiplies the entry with the given factor and returns the result.
			*
			* @param factor The factor with which to multiply the entry.
			*/
			MatrixEntry operator*(value_type factor) const;

			template<typename IndexTypePrime, typename ValueTypePrime>
			friend std::ostream& operator<<(std::ostream& out, MatrixEntry<IndexTypePrime, ValueTypePrime> const& entry);
		private:
			// The actual matrix entry.
			std::pair<index_type, value_type> entry;
		};

	}
}
#endif

size_t basicValueIteration_mvReduce_uint64_double_calculateMemorySize(size_t const rowCount, size_t const rowGroupCount, size_t const nnzCount);
bool basicValueIteration_mvReduce_uint64_double_minimize(uint_fast64_t const maxIterationCount, double const precision, bool const relativePrecisionCheck, std::vector<uint_fast64_t> const& matrixRowIndices, std::vector<storm::storage::MatrixEntry<uint_fast64_t, double>> const& columnIndicesAndValues, std::vector<double>& x, std::vector<double> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, size_t& iterationCount);
bool basicValueIteration_mvReduce_uint64_double_maximize(uint_fast64_t const maxIterationCount, double const precision, bool const relativePrecisionCheck, std::vector<uint_fast64_t> const& matrixRowIndices, std::vector<storm::storage::MatrixEntry<uint_fast64_t, double>> const& columnIndicesAndValues, std::vector<double>& x, std::vector<double> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, size_t& iterationCount);

size_t basicValueIteration_mvReduce_uint64_float_calculateMemorySize(size_t const rowCount, size_t const rowGroupCount, size_t const nnzCount);
bool basicValueIteration_mvReduce_uint64_float_minimize(uint_fast64_t const maxIterationCount, double const precision, bool const relativePrecisionCheck, std::vector<uint_fast64_t> const& matrixRowIndices, std::vector<storm::storage::MatrixEntry<uint_fast64_t, float>> const& columnIndicesAndValues, std::vector<float>& x, std::vector<float> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, size_t& iterationCount);
bool basicValueIteration_mvReduce_uint64_float_maximize(uint_fast64_t const maxIterationCount, double const precision, bool const relativePrecisionCheck, std::vector<uint_fast64_t> const& matrixRowIndices, std::vector<storm::storage::MatrixEntry<uint_fast64_t, float>> const& columnIndicesAndValues, std::vector<float>& x, std::vector<float> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, size_t& iterationCount);

void basicValueIteration_spmv_uint64_double(uint_fast64_t const matrixColCount, std::vector<uint_fast64_t> const& matrixRowIndices, std::vector<storm::storage::MatrixEntry<uint_fast64_t, double>> const& columnIndicesAndValues, std::vector<double> const& x, std::vector<double>& b);
void basicValueIteration_addVectorsInplace_double(std::vector<double>& a, std::vector<double> const& b);
void basicValueIteration_reduceGroupedVector_uint64_double_minimize(std::vector<double> const& groupedVector, std::vector<uint_fast64_t> const& grouping, std::vector<double>& targetVector);
void basicValueIteration_reduceGroupedVector_uint64_double_maximize(std::vector<double> const& groupedVector, std::vector<uint_fast64_t> const& grouping, std::vector<double>& targetVector);
void basicValueIteration_equalModuloPrecision_double_Relative(std::vector<double> const& x, std::vector<double> const& y, double& maxElement);
void basicValueIteration_equalModuloPrecision_double_NonRelative(std::vector<double> const& x, std::vector<double> const& y, double& maxElement);

void basicValueIteration_spmv_uint64_float(uint_fast64_t const matrixColCount, std::vector<uint_fast64_t> const& matrixRowIndices, std::vector<storm::storage::MatrixEntry<uint_fast64_t, float>> const& columnIndicesAndValues, std::vector<float> const& x, std::vector<float>& b);
void basicValueIteration_addVectorsInplace_float(std::vector<float>& a, std::vector<float> const& b);
void basicValueIteration_reduceGroupedVector_uint64_float_minimize(std::vector<float> const& groupedVector, std::vector<uint_fast64_t> const& grouping, std::vector<float>& targetVector);
void basicValueIteration_reduceGroupedVector_uint64_float_maximize(std::vector<float> const& groupedVector, std::vector<uint_fast64_t> const& grouping, std::vector<float>& targetVector);
void basicValueIteration_equalModuloPrecision_float_Relative(std::vector<float> const& x, std::vector<float> const& y, float& maxElement);
void basicValueIteration_equalModuloPrecision_float_NonRelative(std::vector<float> const& x, std::vector<float> const& y, float& maxElement);

#endif // STORM_CUDAFORSTORM_BASICVALUEITERATION_H_