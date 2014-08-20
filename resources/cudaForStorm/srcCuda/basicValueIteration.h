#ifndef STORM_CUDAFORSTORM_BASICVALUEITERATION_H_
#define STORM_CUDAFORSTORM_BASICVALUEITERATION_H_

#include <cstdint>
#include <vector>
#include <utility>

// Library exports
#include "cudaForStorm_Export.h"

/* Helper declaration to cope with new internal format */
#ifndef STORM_STORAGE_SPARSEMATRIX_H_
namespace storm {
	namespace storage {
template<typename T>
    class MatrixEntry {
    public:
        /*!
            * Constructs a matrix entry with the given column and value.
            *
            * @param column The column of the matrix entry.
            * @param value The value of the matrix entry.
            */
        MatrixEntry(uint_fast64_t column, T value);
            
        /*!
            * Move-constructs the matrix entry fro the given column-value pair.
            *
            * @param pair The column-value pair from which to move-construct the matrix entry.
            */
        MatrixEntry(std::pair<uint_fast64_t, T>&& pair);
            
        //MatrixEntry() = default;
        //MatrixEntry(MatrixEntry const& other) = default;
        //MatrixEntry& operator=(MatrixEntry const& other) = default;
#ifndef WINDOWS
        //MatrixEntry(MatrixEntry&& other) = default;
        //MatrixEntry& operator=(MatrixEntry&& other) = default;
#endif
            
        /*!
            * Retrieves the column of the matrix entry.
            *
            * @return The column of the matrix entry.
            */
        uint_fast64_t const& getColumn() const;
            
        /*!
            * Retrieves the column of the matrix entry.
            *
            * @return The column of the matrix entry.
            */
        uint_fast64_t& getColumn();
            
        /*!
            * Retrieves the value of the matrix entry.
            *
            * @return The value of the matrix entry.
            */
        T const& getValue() const;

        /*!
            * Retrieves the value of the matrix entry.
            *
            * @return The value of the matrix entry.
            */
        T& getValue();
            
        /*!
            * Retrieves a pair of column and value that characterizes this entry.
            *
            * @return A column-value pair that characterizes this entry.
            */
        std::pair<uint_fast64_t, T> const& getColumnValuePair() const;
            
    private:
        // The actual matrix entry.
        std::pair<uint_fast64_t, T> entry;
    };

	}
}
#endif

cudaForStorm_EXPORT size_t basicValueIteration_mvReduce_uint64_double_calculateMemorySize(size_t const rowCount, size_t const rowGroupCount, size_t const nnzCount);
cudaForStorm_EXPORT bool basicValueIteration_mvReduce_uint64_double_minimize(uint_fast64_t const maxIterationCount, double const precision, bool const relativePrecisionCheck, std::vector<uint_fast64_t> const& matrixRowIndices, std::vector<storm::storage::MatrixEntry<double>> const& columnIndicesAndValues, std::vector<double>& x, std::vector<double> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, size_t& iterationCount);
cudaForStorm_EXPORT bool basicValueIteration_mvReduce_uint64_double_maximize(uint_fast64_t const maxIterationCount, double const precision, bool const relativePrecisionCheck, std::vector<uint_fast64_t> const& matrixRowIndices, std::vector<storm::storage::MatrixEntry<double>> const& columnIndicesAndValues, std::vector<double>& x, std::vector<double> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, size_t& iterationCount);

cudaForStorm_EXPORT size_t basicValueIteration_mvReduce_uint64_float_calculateMemorySize(size_t const rowCount, size_t const rowGroupCount, size_t const nnzCount);
cudaForStorm_EXPORT bool basicValueIteration_mvReduce_uint64_float_minimize(uint_fast64_t const maxIterationCount, double const precision, bool const relativePrecisionCheck, std::vector<uint_fast64_t> const& matrixRowIndices, std::vector<storm::storage::MatrixEntry<float>> const& columnIndicesAndValues, std::vector<float>& x, std::vector<float> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, size_t& iterationCount);
cudaForStorm_EXPORT bool basicValueIteration_mvReduce_uint64_float_maximize(uint_fast64_t const maxIterationCount, double const precision, bool const relativePrecisionCheck, std::vector<uint_fast64_t> const& matrixRowIndices, std::vector<storm::storage::MatrixEntry<float>> const& columnIndicesAndValues, std::vector<float>& x, std::vector<float> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, size_t& iterationCount);

cudaForStorm_EXPORT void basicValueIteration_spmv_uint64_double(uint_fast64_t const matrixColCount, std::vector<uint_fast64_t> const& matrixRowIndices, std::vector<storm::storage::MatrixEntry<double>> const& columnIndicesAndValues, std::vector<double> const& x, std::vector<double>& b);
cudaForStorm_EXPORT void basicValueIteration_addVectorsInplace_double(std::vector<double>& a, std::vector<double> const& b);
cudaForStorm_EXPORT void basicValueIteration_reduceGroupedVector_uint64_double_minimize(std::vector<double> const& groupedVector, std::vector<uint_fast64_t> const& grouping, std::vector<double>& targetVector);
cudaForStorm_EXPORT void basicValueIteration_reduceGroupedVector_uint64_double_maximize(std::vector<double> const& groupedVector, std::vector<uint_fast64_t> const& grouping, std::vector<double>& targetVector);
cudaForStorm_EXPORT void basicValueIteration_equalModuloPrecision_double_Relative(std::vector<double> const& x, std::vector<double> const& y, double& maxElement);
cudaForStorm_EXPORT void basicValueIteration_equalModuloPrecision_double_NonRelative(std::vector<double> const& x, std::vector<double> const& y, double& maxElement);

cudaForStorm_EXPORT void basicValueIteration_spmv_uint64_float(uint_fast64_t const matrixColCount, std::vector<uint_fast64_t> const& matrixRowIndices, std::vector<storm::storage::MatrixEntry<float>> const& columnIndicesAndValues, std::vector<float> const& x, std::vector<float>& b);
cudaForStorm_EXPORT void basicValueIteration_addVectorsInplace_float(std::vector<float>& a, std::vector<float> const& b);
cudaForStorm_EXPORT void basicValueIteration_reduceGroupedVector_uint64_float_minimize(std::vector<float> const& groupedVector, std::vector<uint_fast64_t> const& grouping, std::vector<float>& targetVector);
cudaForStorm_EXPORT void basicValueIteration_reduceGroupedVector_uint64_float_maximize(std::vector<float> const& groupedVector, std::vector<uint_fast64_t> const& grouping, std::vector<float>& targetVector);
cudaForStorm_EXPORT void basicValueIteration_equalModuloPrecision_float_Relative(std::vector<float> const& x, std::vector<float> const& y, float& maxElement);
cudaForStorm_EXPORT void basicValueIteration_equalModuloPrecision_float_NonRelative(std::vector<float> const& x, std::vector<float> const& y, float& maxElement);

#endif // STORM_CUDAFORSTORM_BASICVALUEITERATION_H_