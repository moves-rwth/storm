#include "storm/solver/helper/SoundValueIterationHelper.h"

#include "storm/storage/BitVector.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/utility/NumberTraits.h"
#include "storm/utility/macros.h"
#include "storm/utility/vector.h"

#include "storm/exceptions/NotSupportedException.h"

namespace storm {
namespace solver {
namespace helper {

template<typename ValueType>
SoundValueIterationHelper<ValueType>::SoundValueIterationHelper(storm::storage::SparseMatrix<ValueType> const& matrix, std::vector<ValueType>& x,
                                                                std::vector<ValueType>& y, bool relative, ValueType const& precision)
    : x(x),
      y(y),
      hasLowerBound(false),
      hasUpperBound(false),
      hasDecisionValue(false),
      convergencePhase1(true),
      decisionValueBlocks(false),
      firstIndexViolatingConvergence(0),
      minIndex(0),
      maxIndex(0),
      relative(relative),
      precision(precision),
      rowGroupIndices(nullptr) {
    STORM_LOG_THROW(matrix.getEntryCount() < std::numeric_limits<IndexType>::max(), storm::exceptions::NotSupportedException,
                    "The number of matrix entries is too large for the selected index type.");
    if (!matrix.hasTrivialRowGrouping()) {
        rowGroupIndices = &matrix.getRowGroupIndices();
        uint64_t sizeOfLargestRowGroup = matrix.getSizeOfLargestRowGroup();
        xTmp.resize(sizeOfLargestRowGroup);
        yTmp.resize(sizeOfLargestRowGroup);
    }
    x.assign(x.size(), storm::utility::zero<ValueType>());
    y.assign(x.size(), storm::utility::one<ValueType>());

    numRows = matrix.getRowCount();
    matrixValues.clear();
    matrixColumns.clear();
    rowIndications.clear();
    matrixValues.reserve(matrix.getNonzeroEntryCount());
    matrixColumns.reserve(matrix.getColumnCount());
    rowIndications.reserve(numRows + 1);
    rowIndications.push_back(0);
    for (IndexType r = 0; r < numRows; ++r) {
        for (auto const& entry : matrix.getRow(r)) {
            matrixValues.push_back(entry.getValue());
            matrixColumns.push_back(entry.getColumn());
        }
        rowIndications.push_back(matrixValues.size());
    }
}

template<typename ValueType>
SoundValueIterationHelper<ValueType>::SoundValueIterationHelper(SoundValueIterationHelper<ValueType>&& oldHelper, std::vector<ValueType>& x,
                                                                std::vector<ValueType>& y, bool relative, ValueType const& precision)
    : x(x),
      y(y),
      xTmp(std::move(oldHelper.xTmp)),
      yTmp(std::move(oldHelper.yTmp)),
      hasLowerBound(false),
      hasUpperBound(false),
      hasDecisionValue(false),
      convergencePhase1(true),
      decisionValueBlocks(false),
      firstIndexViolatingConvergence(0),
      minIndex(0),
      maxIndex(0),
      relative(relative),
      precision(precision),
      numRows(std::move(oldHelper.numRows)),
      matrixValues(std::move(oldHelper.matrixValues)),
      matrixColumns(std::move(oldHelper.matrixColumns)),
      rowIndications(std::move(oldHelper.rowIndications)),
      rowGroupIndices(oldHelper.rowGroupIndices) {
    // If x0 is the obtained result, we want x0-eps <= x <= x0+eps for the actual solution x. Hence, the difference between the lower and upper bounds can be
    // 2*eps.
    this->precision *= storm::utility::convertNumber<ValueType>(2.0);
    x.assign(x.size(), storm::utility::zero<ValueType>());
    y.assign(x.size(), storm::utility::one<ValueType>());
}

template<typename ValueType>
void SoundValueIterationHelper<ValueType>::setLowerBound(ValueType const& value) {
    STORM_LOG_TRACE("Set lower bound to " << value << ".");
    hasLowerBound = true;
    lowerBound = value;
}

template<typename ValueType>
void SoundValueIterationHelper<ValueType>::setUpperBound(ValueType const& value) {
    STORM_LOG_TRACE("Set upper bound to " << value << ".");
    hasUpperBound = true;
    upperBound = value;
}

template<typename ValueType>
void SoundValueIterationHelper<ValueType>::multiplyRow(IndexType const& rowIndex, ValueType const& bi, ValueType& xi, ValueType& yi) {
    assert(rowIndex < numRows);
    ValueType xRes = bi;
    ValueType yRes = storm::utility::zero<ValueType>();

    auto entryIt = matrixValues.begin() + rowIndications[rowIndex];
    auto entryItE = matrixValues.begin() + rowIndications[rowIndex + 1];
    auto colIt = matrixColumns.begin() + rowIndications[rowIndex];
    for (; entryIt != entryItE; ++entryIt, ++colIt) {
        xRes += *entryIt * x[*colIt];
        yRes += *entryIt * y[*colIt];
    }
    xi = std::move(xRes);
    yi = std::move(yRes);
}

template<typename ValueType>
void SoundValueIterationHelper<ValueType>::performIterationStep(OptimizationDirection const& dir, std::vector<ValueType> const& b,
                                                                boost::optional<storage::BitVector> const& schedulerFixedForRowgroup,
                                                                boost::optional<std::vector<uint_fast64_t>> const& scheduler) {
    STORM_LOG_ASSERT((!schedulerFixedForRowgroup && !scheduler) || (schedulerFixedForRowgroup && scheduler),
                     "Expecting scheduler and schedulerFixedForRowgroup to be both set or unset");
    if (rowGroupIndices) {
        if (minimize(dir)) {
            performIterationStep<InternalOptimizationDirection::Minimize>(b, schedulerFixedForRowgroup, scheduler);
        } else {
            performIterationStep<InternalOptimizationDirection::Maximize>(b, schedulerFixedForRowgroup, scheduler);
        }
    } else {
        performIterationStep(b);
    }
}

template<typename ValueType>
void SoundValueIterationHelper<ValueType>::performIterationStep(std::vector<ValueType> const& b) {
    auto xIt = x.rbegin();
    auto yIt = y.rbegin();
    IndexType row = numRows;
    while (row > 0) {
        --row;
        multiplyRow(row, b[row], *xIt, *yIt);
        ++xIt;
        ++yIt;
    }
}

template<typename ValueType>
template<typename SoundValueIterationHelper<ValueType>::InternalOptimizationDirection dir>
void SoundValueIterationHelper<ValueType>::performIterationStep(std::vector<ValueType> const& b,
                                                                boost::optional<storm::storage::BitVector> const& schedulerFixedForRowgroup,
                                                                boost::optional<std::vector<uint_fast64_t>> const& scheduler) {
    STORM_LOG_ASSERT((!schedulerFixedForRowgroup && !scheduler) || (schedulerFixedForRowgroup && scheduler),
                     "Expecting scheduler and schedulerFixedForRowgroup to be both set or unset");

    if (!decisionValueBlocks) {
        performIterationStepUpdateDecisionValue<dir>(b, schedulerFixedForRowgroup, scheduler);
    } else {
        assert(decisionValue == getPrimaryBound<dir>());
        auto xIt = x.rbegin();
        auto yIt = y.rbegin();
        auto groupStartIt = rowGroupIndices->rbegin();
        uint64_t groupEnd = *groupStartIt;
        ++groupStartIt;
        auto rowGroupIndex = rowGroupIndices->size();

        for (auto groupStartIte = rowGroupIndices->rend(); groupStartIt != groupStartIte; groupEnd = *(groupStartIt++), ++xIt, ++yIt) {
            rowGroupIndex--;

            if (schedulerFixedForRowgroup && schedulerFixedForRowgroup.get()[rowGroupIndex]) {
                // The scheduler is fixed for this rowgroup so we perform iteration only on this row.
                IndexType row = *groupStartIt + scheduler.get()[rowGroupIndex];
                multiplyRow(row, b[row], *xIt, *yIt);
            } else {
                // Perform the iteration for the first row in the group
                IndexType row = *groupStartIt;
                ValueType xBest, yBest;
                multiplyRow(row, b[row], xBest, yBest);
                ++row;
                // Only do more work if there are still rows in this row group
                if (row != groupEnd) {
                    ValueType xi, yi;
                    ValueType bestValue = xBest + yBest * getPrimaryBound<dir>();
                    for (; row < groupEnd; ++row) {
                        // Get the multiplication results
                        multiplyRow(row, b[row], xi, yi);
                        ValueType currentValue = xi + yi * getPrimaryBound<dir>();
                        // Check if the current row is better then the previously found one
                        if (better<dir>(currentValue, bestValue)) {
                            xBest = std::move(xi);
                            yBest = std::move(yi);
                            bestValue = std::move(currentValue);
                        } else if (currentValue == bestValue && yBest > yi) {
                            // If the value for this row is not strictly better, it might still be equal and have a better y value
                            xBest = std::move(xi);
                            yBest = std::move(yi);
                        }
                    }
                }
                *xIt = std::move(xBest);
                *yIt = std::move(yBest);
            }
        }
    }
}

template<typename ValueType>
template<typename SoundValueIterationHelper<ValueType>::InternalOptimizationDirection dir>
void SoundValueIterationHelper<ValueType>::performIterationStepUpdateDecisionValue(std::vector<ValueType> const& b,
                                                                                   boost::optional<storm::storage::BitVector> const& schedulerFixedForRowgroup,
                                                                                   boost::optional<std::vector<uint_fast64_t>> const& scheduler) {
    STORM_LOG_ASSERT((!schedulerFixedForRowgroup && !scheduler) || (schedulerFixedForRowgroup && scheduler),
                     "Expecting scheduler and schedulerFixedForRowgroup to be both set or unset");
    auto xIt = x.rbegin();
    auto yIt = y.rbegin();
    auto groupStartIt = rowGroupIndices->rbegin();

    uint64_t groupEnd = *groupStartIt;
    ++groupStartIt;
    auto rowGroupIndex = x.size() - 1;

    for (auto groupStartIte = rowGroupIndices->rend(); groupStartIt != groupStartIte; groupEnd = *(groupStartIt++), ++xIt, ++yIt, --rowGroupIndex) {
        if (schedulerFixedForRowgroup && schedulerFixedForRowgroup.get()[rowGroupIndex]) {
            // The scheduler is fixed for this rowgroup so we perform iteration only on this entry.
            IndexType row = *groupStartIt + scheduler.get()[rowGroupIndex];
            ValueType xBest, yBest;
            multiplyRow(row, b[row], *xIt, *yIt);
        } else {
            // Perform the iteration for the first row in the group
            uint64_t row = *groupStartIt;
            ValueType xBest, yBest;
            multiplyRow(row, b[row], xBest, yBest);
            ++row;
            // Only do more work if there are still rows in this row group
            if (row != groupEnd) {
                ValueType xi, yi;
                uint64_t xyTmpIndex = 0;
                if (hasPrimaryBound<dir>()) {
                    ValueType bestValue = xBest + yBest * getPrimaryBound<dir>();
                    for (; row < groupEnd; ++row) {
                        // Get the multiplication results
                        multiplyRow(row, b[row], xi, yi);
                        ValueType currentValue = xi + yi * getPrimaryBound<dir>();
                        // Check if the current row is better then the previously found one
                        if (better<dir>(currentValue, bestValue)) {
                            if (yBest < yi) {
                                // We need to store the 'old' best value as it might be relevant for the decision value
                                xTmp[xyTmpIndex] = std::move(xBest);
                                yTmp[xyTmpIndex] = std::move(yBest);
                                ++xyTmpIndex;
                            }
                            xBest = std::move(xi);
                            yBest = std::move(yi);
                            bestValue = std::move(currentValue);
                        } else if (yBest > yi) {
                            // If the value for this row is not strictly better, it might still be equal and have a better y value
                            if (currentValue == bestValue) {
                                xBest = std::move(xi);
                                yBest = std::move(yi);
                            } else {
                                xTmp[xyTmpIndex] = std::move(xi);
                                yTmp[xyTmpIndex] = std::move(yi);
                                ++xyTmpIndex;
                            }
                        }
                    }
                } else {
                    for (; row < groupEnd; ++row) {
                        multiplyRow(row, b[row], xi, yi);
                        // Update the best choice
                        if (yi > yBest || (yi == yBest && better<dir>(xi, xBest))) {
                            xTmp[xyTmpIndex] = std::move(xBest);
                            yTmp[xyTmpIndex] = std::move(yBest);
                            ++xyTmpIndex;
                            xBest = std::move(xi);
                            yBest = std::move(yi);
                        } else {
                            xTmp[xyTmpIndex] = std::move(xi);
                            yTmp[xyTmpIndex] = std::move(yi);
                            ++xyTmpIndex;
                        }
                    }
                }

                // Update the decision value
                for (uint64_t i = 0; i < xyTmpIndex; ++i) {
                    ValueType deltaY = yBest - yTmp[i];
                    if (deltaY > storm::utility::zero<ValueType>()) {
                        ValueType newDecisionValue = (xTmp[i] - xBest) / deltaY;
                        if (!hasDecisionValue || better<dir>(newDecisionValue, decisionValue)) {
                            decisionValue = std::move(newDecisionValue);
                            STORM_LOG_TRACE("Update decision value to " << decisionValue);
                            hasDecisionValue = true;
                        }
                    }
                }
            }
            *xIt = std::move(xBest);
            *yIt = std::move(yBest);
        }
    }
}

template<typename ValueType>
bool SoundValueIterationHelper<ValueType>::checkConvergenceUpdateBounds(OptimizationDirection const& dir, storm::storage::BitVector const* relevantValues) {
    if (rowGroupIndices) {
        if (minimize(dir)) {
            return checkConvergenceUpdateBounds<InternalOptimizationDirection::Minimize>(relevantValues);
        } else {
            return checkConvergenceUpdateBounds<InternalOptimizationDirection::Maximize>(relevantValues);
        }
    } else {
        return checkConvergenceUpdateBounds(relevantValues);
    }
}

template<typename ValueType>
bool SoundValueIterationHelper<ValueType>::checkConvergenceUpdateBounds(storm::storage::BitVector const* relevantValues) {
    return checkConvergenceUpdateBounds<InternalOptimizationDirection::None>(relevantValues);
}

template<typename ValueType>
template<typename SoundValueIterationHelper<ValueType>::InternalOptimizationDirection dir>
bool SoundValueIterationHelper<ValueType>::checkConvergenceUpdateBounds(storm::storage::BitVector const* relevantValues) {
    if (convergencePhase1) {
        if (checkConvergencePhase1()) {
            firstIndexViolatingConvergence = 0;
            if (relevantValues != nullptr) {
                firstIndexViolatingConvergence = relevantValues->getNextSetIndex(firstIndexViolatingConvergence);
            }
        } else {
            return false;
        }
    }
    STORM_LOG_ASSERT(!std::any_of(y.begin(), y.end(), [](ValueType value) { return storm::utility::isOne(value); }),
                     "Did not expect staying-probability 1 at this point.");

    // Reaching this point means that we are in Phase 2:
    // The difference between lower and upper bound has to be < precision at every (relevant) value

    // For efficiency reasons we first check whether it is worth to compute the actual bounds. We do so by considering possibly too tight bounds
    ValueType lowerBoundCandidate, upperBoundCandidate;
    if (preliminaryConvergenceCheck<dir>(lowerBoundCandidate, upperBoundCandidate)) {
        updateLowerUpperBound<dir>(lowerBoundCandidate, upperBoundCandidate);
        if (dir != InternalOptimizationDirection::None) {
            checkIfDecisionValueBlocks<dir>();
        }
        return checkConvergencePhase2(relevantValues);
    }
    return false;
}

template<typename ValueType>
void SoundValueIterationHelper<ValueType>::setSolutionVector() {
    // Due to a custom termination criterion it might be the case that one of the bounds was not yet established.
    ValueType meanBound;
    if (!hasLowerBound) {
        STORM_LOG_WARN("No lower result bound was computed during sound value iteration.");
        if (hasUpperBound) {
            meanBound = upperBound;
        } else {
            STORM_LOG_WARN("No upper result bound was computed during sound value iteration.");
            meanBound = storm::utility::zero<ValueType>();
        }
    } else if (!hasUpperBound) {
        STORM_LOG_WARN("No upper result bound was computed during sound value iteration.");
        meanBound = lowerBound;
    } else {
        meanBound = (upperBound + lowerBound) / storm::utility::convertNumber<ValueType>(2.0);
    }

    storm::utility::vector::applyPointwise(x, y, x, [&meanBound](ValueType const& xi, ValueType const& yi) -> ValueType { return xi + yi * meanBound; });

    STORM_LOG_INFO("Sound Value Iteration terminated with lower bound (over all states) "
                   << (hasLowerBound ? lowerBound : storm::utility::zero<ValueType>()) << (hasLowerBound ? "" : "(none)")
                   << " and upper bound (over all states) " << (hasUpperBound ? upperBound : storm::utility::infinity<ValueType>())
                   << (hasUpperBound ? "" : "(none)") << ". Decision value is " << (hasDecisionValue ? decisionValue : -storm::utility::infinity<ValueType>())
                   << (hasDecisionValue ? "" : "(none)") << ".");
}

template<typename ValueType>
bool SoundValueIterationHelper<ValueType>::checkCustomTerminationCondition(storm::solver::TerminationCondition<ValueType> const& condition) {
    if (condition.requiresGuarantee(storm::solver::SolverGuarantee::GreaterOrEqual)) {
        if (hasUpperBound &&
            condition.terminateNow([&](uint64_t const& i) { return x[i] + y[i] * upperBound; }, storm::solver::SolverGuarantee::GreaterOrEqual)) {
            return true;
        }
    } else if (condition.requiresGuarantee(storm::solver::SolverGuarantee::LessOrEqual)) {
        if (hasLowerBound && condition.terminateNow([&](uint64_t const& i) { return x[i] + y[i] * lowerBound; }, storm::solver::SolverGuarantee::LessOrEqual)) {
            return true;
        }
    }
    return false;
}

template<typename ValueType>
bool SoundValueIterationHelper<ValueType>::checkConvergencePhase1() {
    // Return true if y ('the probability to stay within the matrix') is  < 1 at every entry
    for (; firstIndexViolatingConvergence != y.size(); ++firstIndexViolatingConvergence) {
        static_assert(NumberTraits<ValueType>::IsExact || std::is_same<ValueType, double>::value, "Considered ValueType not handled.");
        if (NumberTraits<ValueType>::IsExact) {
            if (storm::utility::isOne(y[firstIndexViolatingConvergence])) {
                return false;
            }
        } else {
            if (storm::utility::isAlmostOne(storm::utility::convertNumber<double>(y[firstIndexViolatingConvergence]))) {
                return false;
            }
        }
    }
    STORM_LOG_TRACE("Phase 1 converged: all y values are < 1.");
    convergencePhase1 = false;
    return true;
}

template<typename ValueType>
bool SoundValueIterationHelper<ValueType>::isPreciseEnough(ValueType const& xi, ValueType const& yi, ValueType const& lb, ValueType const& ub) {
    return yi * (ub - lb) <= storm::utility::abs<ValueType>((relative ? (precision * xi) : (precision)));
}

template<typename ValueType>
template<typename SoundValueIterationHelper<ValueType>::InternalOptimizationDirection dir>
bool SoundValueIterationHelper<ValueType>::preliminaryConvergenceCheck(ValueType& lowerBoundCandidate, ValueType& upperBoundCandidate) {
    lowerBoundCandidate = x[minIndex] / (storm::utility::one<ValueType>() - y[minIndex]);
    upperBoundCandidate = x[maxIndex] / (storm::utility::one<ValueType>() - y[maxIndex]);
    // Make sure that these candidates are at least as tight as the already known bounds
    if (hasLowerBound && lowerBoundCandidate < lowerBound) {
        lowerBoundCandidate = lowerBound;
    }
    if (hasUpperBound && upperBoundCandidate > upperBound) {
        upperBoundCandidate = upperBound;
    }
    if (isPreciseEnough(x[firstIndexViolatingConvergence], y[firstIndexViolatingConvergence], lowerBoundCandidate, upperBoundCandidate)) {
        return true;
    }
    if (dir != InternalOptimizationDirection::None && !decisionValueBlocks) {
        return hasDecisionValue && better<dir>(decisionValue, getPrimaryBound<dir>());
    }
    return false;
}

template<typename ValueType>
template<typename SoundValueIterationHelper<ValueType>::InternalOptimizationDirection dir>
void SoundValueIterationHelper<ValueType>::updateLowerUpperBound(ValueType& lowerBoundCandidate, ValueType& upperBoundCandidate) {
    auto xIt = x.begin();
    auto xIte = x.end();
    auto yIt = y.begin();
    for (uint64_t index = 0; xIt != xIte; ++xIt, ++yIt, ++index) {
        ValueType currentBound = *xIt / (storm::utility::one<ValueType>() - *yIt);
        if (dir != InternalOptimizationDirection::None && decisionValueBlocks) {
            if (better<dir>(getSecondaryBound<dir>(), currentBound)) {
                getSecondaryIndex<dir>() = index;
                getSecondaryBound<dir>() = std::move(currentBound);
            }
        } else {
            if (currentBound < lowerBoundCandidate) {
                minIndex = index;
                lowerBoundCandidate = std::move(currentBound);
            } else if (currentBound > upperBoundCandidate) {
                maxIndex = index;
                upperBoundCandidate = std::move(currentBound);
            }
        }
    }
    if ((dir != InternalOptimizationDirection::Minimize || !decisionValueBlocks) && (!hasLowerBound || lowerBoundCandidate > lowerBound)) {
        setLowerBound(lowerBoundCandidate);
    }
    if ((dir != InternalOptimizationDirection::Maximize || !decisionValueBlocks) && (!hasUpperBound || upperBoundCandidate < upperBound)) {
        setUpperBound(upperBoundCandidate);
    }
}

template<typename ValueType>
template<typename SoundValueIterationHelper<ValueType>::InternalOptimizationDirection dir>
void SoundValueIterationHelper<ValueType>::checkIfDecisionValueBlocks() {
    // Check whether the decision value blocks now (i.e. further improvement of the primary bound would lead to a non-optimal scheduler).
    if (!decisionValueBlocks && hasDecisionValue && better<dir>(decisionValue, getPrimaryBound<dir>())) {
        getPrimaryBound<dir>() = decisionValue;
        decisionValueBlocks = true;
        STORM_LOG_TRACE("Decision value blocks primary bound to " << decisionValue << ".");
    }
}

template<typename ValueType>
bool SoundValueIterationHelper<ValueType>::checkConvergencePhase2(storm::storage::BitVector const* relevantValues) {
    // Check whether the desired precision is reached
    if (isPreciseEnough(x[firstIndexViolatingConvergence], y[firstIndexViolatingConvergence], lowerBound, upperBound)) {
        // The current index satisfies the desired bound. We now move to the next index that violates it
        while (true) {
            ++firstIndexViolatingConvergence;
            if (relevantValues != nullptr) {
                firstIndexViolatingConvergence = relevantValues->getNextSetIndex(firstIndexViolatingConvergence);
            }
            if (firstIndexViolatingConvergence == x.size()) {
                // Converged!
                return true;
            } else {
                if (!isPreciseEnough(x[firstIndexViolatingConvergence], y[firstIndexViolatingConvergence], lowerBound, upperBound)) {
                    // not converged yet
                    return false;
                }
            }
        }
    }
    return false;
}

template class SoundValueIterationHelper<double>;
template class SoundValueIterationHelper<storm::RationalNumber>;
}  // namespace helper

}  // namespace solver
}  // namespace storm
