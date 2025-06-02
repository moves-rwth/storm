#pragma once

#include <boost/optional.hpp>
#include <vector>

#include "storm/storage/SparseMatrix.h"

#include "storm/solver/OptimizationDirection.h"
#include "storm/solver/SolverStatus.h"
#include "storm/storage/BitVector.h"

namespace storm {
class Environment;

namespace solver {
namespace helper {

namespace gviinternal {

typedef uint32_t IndexType;

template<typename ValueType>
class IterationHelper {
   public:
    IterationHelper(storm::storage::SparseMatrix<ValueType> const& matrix);

    IndexType selectRowGroupToGuess(const std::vector<ValueType>& lowerX, const std::vector<ValueType>& upperX);

    template<bool lower>
    ValueType iterate(storm::solver::OptimizationDirection dir, std::vector<ValueType>& x, const std::vector<ValueType>& b, const IndexType& guessRowGroup);

    template<bool lower>
    void iterate(storm::solver::OptimizationDirection dir, std::vector<ValueType>& x, const std::vector<ValueType>& b);

    template<bool minimizeDir>
    ValueType multiplyRowGroup(const IndexType& rowGroupIndex, const std::vector<ValueType>& x, const std::vector<ValueType>& b);

    ValueType multiplyRow(const IndexType& rowIndex, const std::vector<ValueType>& x, const ValueType& bi);

    ValueType getMaxLength(std::vector<ValueType>& lower, std::vector<ValueType>& upper);
    ValueType getSumLength(std::vector<ValueType>& lower, std::vector<ValueType>& upper);

   private:
    std::vector<ValueType> matrixValues;
    std::vector<IndexType> matrixColumns;
    std::vector<IndexType> rowIndications;
    std::vector<uint64_t> const* rowGroupIndices;
    std::vector<IndexType> sortedRows;

    void swipeWeights(std::vector<ValueType>& weights);

    template<bool minimizeDir, bool lower>
    ValueType iterate(std::vector<ValueType>& x, const std::vector<ValueType>& b, const IndexType& guessRowGroup);

    template<bool minimizeDir, bool lower>
    void iterate(std::vector<ValueType>& x, const std::vector<ValueType>& b);

    void sortRows(std::vector<ValueType>& b);
};

}  // namespace gviinternal

template<typename ValueType>
class GuessingValueIterationHelper {
   public:
    typedef gviinternal::IndexType IndexType;
    enum class VerifyResult { Lower, Upper, Converged, Unverified };
    explicit GuessingValueIterationHelper(storm::storage::SparseMatrix<ValueType> const& matrix);

    /*!
     * @param env
     * @param lowerX Needs to be some arbitrary lower bound on the actual values initially
     * @param upperX Does not need to be an upper bound initially
     * @param b the values added to each matrix row (the b in A*x+b)
     * @param dir The optimization direction
     * @param relevantValues If given, we only check the precision at the states with the given indices.
     * @return The status upon termination as well as the number of iterations Also, the maximum (relative/absolute) difference between lowerX and upperX will
     * be 2*epsilon with the provided precision parameters.
     */
    std::pair<SolverStatus, uint64_t> solveEquations(std::vector<ValueType>& lowerX, std::vector<ValueType>& upperX, const std::vector<ValueType>& b,
                                                     ValueType precision, uint64_t maxOverallIterations,
                                                     boost::optional<storm::solver::OptimizationDirection> dir);

   private:
    gviinternal::IterationHelper<ValueType> iterationHelper;
    std::vector<ValueType> guessLower, guessUpper;
    IndexType selectRowGroupToGuess(std::vector<ValueType>& lowerX, std::vector<ValueType>& upperX);
    std::pair<SolverStatus, uint64_t> tryVerify(std::vector<ValueType>& lowerX, std::vector<ValueType>& upperX, const std::vector<ValueType>& b,
                                                boost::optional<storm::solver::OptimizationDirection> dir, IndexType rowGroupToGuess, ValueType guessValue,
                                                uint64_t maxOverallIterations, ValueType precision);

    void copy(std::vector<ValueType>& from, std::vector<ValueType>& to);
    std::pair<SolverStatus, uint64_t> doIterations(storm::solver::OptimizationDirection dir, std::vector<ValueType>& lowerX, std::vector<ValueType>& upperX,
                                                   const std::vector<ValueType>& b, uint64_t maxIterations, const ValueType& precision);
};
}  // namespace helper
}  // namespace solver
}  // namespace storm