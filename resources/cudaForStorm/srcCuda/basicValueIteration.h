#include <cstdint>
#include <vector>

void basicValueIteration_mvReduce(uint_fast64_t const maxIterationCount, std::vector<uint_fast64_t> const& matrixRowIndices, std::vector<uint_fast64_t> const& matrixColumnIndices, std::vector<double> const& matrixValues, std::vector<double>& x, std::vector<double> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices);