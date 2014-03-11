#ifndef STORM_CUDAFORSTORM_BASICVALUEITERATION_H_
#define STORM_CUDAFORSTORM_BASICVALUEITERATION_H_

#include <cstdint>
#include <vector>
#include <utility>

// Library exports
#include "cudaForStorm_Export.h"

cudaForStorm_EXPORT void cudaForStormTestFunction(int a, int b);
cudaForStorm_EXPORT void basicValueIteration_mvReduce_uint64_double_minimize(uint_fast64_t const maxIterationCount, double const precision, bool const relativePrecisionCheck, std::vector<uint_fast64_t> const& matrixRowIndices, std::vector<std::pair<uint_fast64_t, double>> const& columnIndicesAndValues, std::vector<double>& x, std::vector<double> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices);
cudaForStorm_EXPORT void basicValueIteration_mvReduce_uint64_double_maximize(uint_fast64_t const maxIterationCount, double const precision, bool const relativePrecisionCheck, std::vector<uint_fast64_t> const& matrixRowIndices, std::vector<std::pair<uint_fast64_t, double>> const& columnIndicesAndValues, std::vector<double>& x, std::vector<double> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices);

#endif // STORM_CUDAFORSTORM_BASICVALUEITERATION_H_