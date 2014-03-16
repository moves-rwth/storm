#ifndef STORM_CUDAFORSTORM_BASICVALUEITERATION_H_
#define STORM_CUDAFORSTORM_BASICVALUEITERATION_H_

#include <cstdint>
#include <vector>
#include <utility>

// Library exports
#include "cudaForStorm_Export.h"

cudaForStorm_EXPORT size_t basicValueIteration_mvReduce_uint64_double_calculateMemorySize(size_t const rowCount, size_t const rowGroupCount, size_t const nnzCount);
cudaForStorm_EXPORT void basicValueIteration_mvReduce_uint64_double_minimize(uint_fast64_t const maxIterationCount, double const precision, bool const relativePrecisionCheck, std::vector<uint_fast64_t> const& matrixRowIndices, std::vector<std::pair<uint_fast64_t, double>> const& columnIndicesAndValues, std::vector<double>& x, std::vector<double> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices);
cudaForStorm_EXPORT void basicValueIteration_mvReduce_uint64_double_maximize(uint_fast64_t const maxIterationCount, double const precision, bool const relativePrecisionCheck, std::vector<uint_fast64_t> const& matrixRowIndices, std::vector<std::pair<uint_fast64_t, double>> const& columnIndicesAndValues, std::vector<double>& x, std::vector<double> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices);
cudaForStorm_EXPORT void basicValueIteration_spmv_uint64_double(uint_fast64_t const matrixColCount, std::vector<uint_fast64_t> const& matrixRowIndices, std::vector<std::pair<uint_fast64_t, double>> const& columnIndicesAndValues, std::vector<double> const& x, std::vector<double>& b);
cudaForStorm_EXPORT void basicValueIteration_addVectorsInplace_double(std::vector<double>& a, std::vector<double> const& b);
cudaForStorm_EXPORT void basicValueIteration_reduceGroupedVector_uint64_double_minimize(std::vector<double> const& groupedVector, std::vector<uint_fast64_t> const& grouping, std::vector<double>& targetVector);
cudaForStorm_EXPORT void basicValueIteration_reduceGroupedVector_uint64_double_maximize(std::vector<double> const& groupedVector, std::vector<uint_fast64_t> const& grouping, std::vector<double>& targetVector);
cudaForStorm_EXPORT void basicValueIteration_equalModuloPrecision_double_Relative(std::vector<double> const& x, std::vector<double> const& y, double& maxElement);
cudaForStorm_EXPORT void basicValueIteration_equalModuloPrecision_double_NonRelative(std::vector<double> const& x, std::vector<double> const& y, double& maxElement);

#endif // STORM_CUDAFORSTORM_BASICVALUEITERATION_H_