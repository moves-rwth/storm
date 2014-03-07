#include <cstdint>
#include <vector>
#include <utility>

// Library exports
#include "cudaForStorm_Export.h"

cudaForStorm_EXPORT void cudaForStormTestFunction(int a, int b);
cudaForStorm_EXPORT void basicValueIteration_mvReduce(uint_fast64_t const maxIterationCount, std::vector<uint_fast64_t> const& matrixRowIndices, std::vector<std::pair<uint_fast64_t, double>> columnIndicesAndValues, std::vector<double>& x, std::vector<double> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices);