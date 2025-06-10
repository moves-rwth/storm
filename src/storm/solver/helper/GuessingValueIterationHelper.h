#pragma once

#include <boost/optional.hpp>
#include <vector>

#include "storm/solver/helper/ValueIterationOperator.h"

#include "storm/solver/OptimizationDirection.h"
#include "storm/solver/SolverStatus.h"
#include "storm/storage/BitVector.h"

namespace storm::solver::helper {

namespace gviinternal {

typedef uint32_t IndexType;

template<typename ValueType>
class IterationHelper {
   public:
    IterationHelper(storm::storage::SparseMatrix<ValueType> const& matrix);

    IndexType selectRowGroupToGuess(const std::vector<ValueType>& lowerX, const std::vector<ValueType>& upperX);

    ValueType getMaxLength(std::vector<ValueType>& lower, std::vector<ValueType>& upper);
    ValueType getSumLength(std::vector<ValueType>& lower, std::vector<ValueType>& upper);

   private:
    std::vector<ValueType> matrixValues;
    std::vector<IndexType> matrixColumns;
    std::vector<IndexType> rowIndications;
    std::vector<uint64_t> const* rowGroupIndices;

    void swipeWeights(std::vector<ValueType>& weights);
};

}  // namespace gviinternal

template<typename ValueType, OptimizationDirection Dir>
class GVIBackend;

using IndexType = gviinternal::IndexType;

template<typename ValueType, bool TrivialRowGrouping>
class GuessingValueIterationHelper {
   public:
    enum class VerifyResult { Lower, Upper, Converged, Unverified };
    explicit GuessingValueIterationHelper(std::shared_ptr<ValueIterationOperator<ValueType, TrivialRowGrouping>> viOperator,
                                          const storage::SparseMatrix<ValueType>& matrix);

    /*!
     * @param lowerX Needs to be some arbitrary lower bound on the actual values initially
     * @param upperX Needs to be some arbitrary upper bound on the actual values initially
     * @param b the values added to each matrix row (the b in A*x+b)
     * @param dir The optimization direction
     * @return The status upon termination as well as the number of iterations Also, the maximum (relative/absolute) difference between lowerX and upperX will
     * be 2*epsilon with the provided precision parameters.
     */
    std::pair<SolverStatus, uint64_t> solveEquations(std::vector<ValueType>& lowerX, std::vector<ValueType>& upperX, const std::vector<ValueType>& b,
                                                     ValueType precision, uint64_t maxOverallIterations,
                                                     boost::optional<storm::solver::OptimizationDirection> dir);

   private:
    std::shared_ptr<ValueIterationOperator<ValueType, TrivialRowGrouping>> viOperator;
    gviinternal::IterationHelper<ValueType> iterationHelper;
    std::vector<ValueType> guessLower, guessUpper;
    IndexType selectRowGroupToGuess(std::vector<ValueType>& lowerX, std::vector<ValueType>& upperX);

    template<OptimizationDirection Dir>
    std::pair<SolverStatus, uint64_t> solveEquations(std::vector<ValueType>& lowerX, std::vector<ValueType>& upperX, const std::vector<ValueType>& b,
                                                     ValueType precision, uint64_t maxOverallIterations);

    template<OptimizationDirection Dir>
    void applyInPlace(std::vector<ValueType>& lowerX, std::vector<ValueType>& upperX, const std::vector<ValueType>& b, GVIBackend<ValueType, Dir>& backend);

    template<OptimizationDirection Dir>
    std::pair<SolverStatus, uint64_t> tryVerify(std::vector<ValueType>& lowerX, std::vector<ValueType>& upperX, const std::vector<ValueType>& b,
                                                IndexType rowGroupToGuess, ValueType guessValue, uint64_t maxOverallIterations, ValueType precision);

    template<OptimizationDirection Dir>
    std::pair<SolverStatus, uint64_t> doIterations(std::vector<ValueType>& lowerX, std::vector<ValueType>& upperX, const std::vector<ValueType>& b,
                                                   uint64_t maxIterations, const ValueType& precision);
};
}  // namespace storm::solver::helper