#include <numeric>
#include <queue>
#include "GuessingValueIterationHelper.h"

#include "storm/environment/solver/OviSolverEnvironment.h"
#include "storm/utility/ProgressMeasurement.h"
#include "storm/utility/SignalHandler.h"
#include "storm/utility/vector.h"

#include "storm/exceptions/NotSupportedException.h"

#include "storm/utility/macros.h"

namespace storm {
namespace solver {
namespace helper {

namespace gviinternal {

template<typename ValueType>
IterationHelper<ValueType>::IterationHelper(const storage::SparseMatrix<ValueType>& matrix) {
    STORM_LOG_THROW(static_cast<uint64_t>(std::numeric_limits<IndexType>::max()) > matrix.getRowCount() + 1, storm::exceptions::NotSupportedException,
                    "Matrix dimensions too large.");
    STORM_LOG_THROW(static_cast<uint64_t>(std::numeric_limits<IndexType>::max()) > matrix.getEntryCount(), storm::exceptions::NotSupportedException,
                    "Matrix dimensions too large.");
    matrixValues.reserve(matrix.getNonzeroEntryCount());
    matrixColumns.reserve(matrix.getColumnCount());
    rowIndications.reserve(matrix.getRowCount() + 1);
    rowIndications.push_back(0);
    for (IndexType r = 0; r < static_cast<IndexType>(matrix.getRowCount()); ++r) {
        for (auto const& entry : matrix.getRow(r)) {
            matrixValues.push_back(entry.getValue());
            matrixColumns.push_back(entry.getColumn());
        }
        rowIndications.push_back(matrixValues.size());
    }
    rowGroupIndices = &matrix.getRowGroupIndices();
    //    fillSortedRowGroups()
}

template<typename ValueType>
void IterationHelper<ValueType>::swipeWeights(std::vector<ValueType>& weights) {
    IndexType i = weights.size();
    while (i > 0) {
        --i;

        auto den = storm::utility::convertNumber<ValueType>((*rowGroupIndices)[i + 1] - (*rowGroupIndices)[i]);
        auto weightOnGroup = weights[i] / den;
        weights[i] = 0;

        for (auto row = (*rowGroupIndices)[i]; row < (*rowGroupIndices)[i + 1]; ++row) {
            auto itV = matrixValues.begin() + rowIndications[row];
            auto itVEnd = matrixValues.begin() + rowIndications[row + 1];
            auto itC = matrixColumns.begin() + rowIndications[row];
            for (; itV != itVEnd; ++itV, ++itC) {
                weights[*itC] += weightOnGroup * *itV;
            }
        }
    }
}

template<typename ValueType>
IndexType IterationHelper<ValueType>::selectRowGroupToGuess(const std::vector<ValueType>& lowerX, const std::vector<ValueType>& upperX) {
    std::vector<ValueType> weights(upperX);
    // upper - lower
    storm::utility::vector::addScaledVector<ValueType, ValueType>(weights, lowerX, -1);

    std::vector<ValueType> additiveWeights(weights.size(), 0);
    for (int i = 0; i < 50; i++) {
        swipeWeights(weights);
        storm::utility::vector::addScaledVector(additiveWeights, weights, 1);
    }

    return static_cast<IndexType>(std::max_element(additiveWeights.begin(), additiveWeights.end()) - additiveWeights.begin());
}

template<typename ValueType>
template<bool minimizeDir, bool lower>
ValueType IterationHelper<ValueType>::iterate(std::vector<ValueType>& x, const std::vector<ValueType>& b, const IndexType& guessRowGroup) {
    auto i = static_cast<IndexType>(x.size());
    while (i > guessRowGroup + 1) {
        --i;
        ValueType newX = multiplyRowGroup<minimizeDir>(i, x, b);
        x[i] = lower ? std::max(x[i], newX) : std::min(x[i], newX);
    }
    --i;
    while (i > 0) {
        --i;
        ValueType newX = multiplyRowGroup<minimizeDir>(i, x, b);
        x[i] = lower ? std::max(x[i], newX) : std::min(x[i], newX);
    }
    return multiplyRowGroup<minimizeDir>(guessRowGroup, x, b);
}

template<typename ValueType>
template<bool minimizeDir, bool lower>
void IterationHelper<ValueType>::iterate(std::vector<ValueType>& x, const std::vector<ValueType>& b) {
    auto i = static_cast<IndexType>(x.size());
    while (i > 0) {
        --i;
        ValueType newX = multiplyRowGroup<minimizeDir>(i, x, b);

        x[i] = lower ? std::max(x[i], newX) : std::min(x[i], newX);
    }
}

template<typename ValueType>
template<bool minimizeDir>
ValueType IterationHelper<ValueType>::multiplyRowGroup(const IndexType& rowGroupIndex, const std::vector<ValueType>& x, const std::vector<ValueType>& b) {
    auto row = (*rowGroupIndices)[rowGroupIndex];
    const auto rowEnd = (*rowGroupIndices)[rowGroupIndex + 1];
    STORM_LOG_ASSERT(row < rowEnd, "Empty row group not expected.");
    ValueType xRes = multiplyRow(row, x, b[row]);
    for (; row != rowEnd; ++row) {
        ValueType xCur = multiplyRow(row, x, b[row]);
        xRes = minimizeDir ? std::min(xRes, xCur) : std::max(xRes, xCur);
    }
    return xRes;
}
template<typename ValueType>
ValueType IterationHelper<ValueType>::multiplyRow(const IndexType& rowIndex, const std::vector<ValueType>& x, const ValueType& bi) {
    auto xRes = bi;

    auto itV = matrixValues.begin() + rowIndications[rowIndex];
    const auto itVEnd = matrixValues.begin() + rowIndications[rowIndex + 1];
    auto itC = matrixColumns.begin() + rowIndications[rowIndex];

    for (; itV != itVEnd; ++itV, ++itC) {
        xRes += *itV * x[*itC];
    }
    return xRes;
}
template<typename ValueType>
template<bool lower>
ValueType IterationHelper<ValueType>::iterate(storm::solver::OptimizationDirection dir, std::vector<ValueType>& x, const std::vector<ValueType>& b,
                                              const IndexType& guessRowGroup) {
    if (minimize(dir))
        return iterate<true, lower>(x, b, guessRowGroup);
    else
        return iterate<false, lower>(x, b, guessRowGroup);
}
template<typename ValueType>
template<bool lower>
void IterationHelper<ValueType>::iterate(storm::solver::OptimizationDirection dir, std::vector<ValueType>& x, const std::vector<ValueType>& b) {
    if (minimize(dir))
        iterate<true, lower>(x, b);
    else
        iterate<false, lower>(x, b);
}
template<typename ValueType>
ValueType IterationHelper<ValueType>::getMaxLength(std::vector<ValueType>& lower, std::vector<ValueType>& upper) {
    auto itL = lower.begin();
    const auto itLEnd = lower.end();
    auto itU = upper.begin();

    ValueType result = *itU - *itL;
    for (++itL, ++itU; itL != itLEnd; ++itL, ++itU) {
        ValueType length = *itU - *itL;
        result = std::max(result, length);
    }
    return result;
}

template<typename ValueType>
void IterationHelper<ValueType>::sortRows(std::vector<ValueType>& b) {
    sortedRows.clear();
    sortedRows.reserve(rowGroupIndices->size() - 1);
    storm::storage::BitVector visited(rowGroupIndices->size() - 1);
    for (IndexType r = 0; r < rowGroupIndices->size() - 1; r++) {
        for (auto row = (*rowGroupIndices)[r]; row < (*rowGroupIndices)[r + 1]; row++) {
            if (b[row] != 0)
                sortedRows.push_back(r);
        }
    }
}

template<typename ValueType>
ValueType IterationHelper<ValueType>::getSumLength(std::vector<ValueType>& lower, std::vector<ValueType>& upper) {
    ValueType sum = 0;
    auto itL = lower.begin();
    const auto itLEnd = lower.end();
    auto itU = upper.begin();

    for (; itL != itLEnd; ++itL, ++itU) {
        sum += *itU - *itL;
    }
    return sum;
}

}  // namespace gviinternal

template<typename ValueType>
typename GuessingValueIterationHelper<ValueType>::IndexType GuessingValueIterationHelper<ValueType>::selectRowGroupToGuess(
    std::vector<ValueType>& lowerX, std::vector<ValueType>& upperX) {
    return iterationHelper.selectRowGroupToGuess(lowerX, upperX);
}

template<typename ValueType>
std::pair<SolverStatus, uint64_t> GuessingValueIterationHelper<ValueType>::tryVerify(
    std::vector<ValueType>& lowerX, std::vector<ValueType>& upperX, const std::vector<ValueType>& b, boost::optional<storm::solver::OptimizationDirection> dir,
    GuessingValueIterationHelper::IndexType rowGroupToGuess, ValueType guessValue, uint64_t maxOverallIterations, ValueType precision) {
    copy(lowerX, guessLower);
    copy(upperX, guessUpper);
    guessLower[rowGroupToGuess] = guessUpper[rowGroupToGuess] = guessValue;
    uint64_t iterations = 0;
    ValueType sumLengthBefore = 0, maxLengthBefore = 0;

    bool shouldCheckConvergence = false;
    while (iterations < maxOverallIterations) {
        ++iterations;
        iterationHelper.template iterate<true>(dir.get(), lowerX, b);
        iterationHelper.template iterate<false>(dir.get(), upperX, b);
        auto rowGroupLower = iterationHelper.template iterate<true>(dir.get(), guessLower, b, rowGroupToGuess);
        if (guessValue <= rowGroupLower) {
            copy(guessLower, lowerX);
            lowerX[rowGroupToGuess] = rowGroupLower;
            return {SolverStatus::TerminatedEarly, iterations};
        }
        auto rowGroupUpper = iterationHelper.template iterate<false>(dir.get(), guessUpper, b, rowGroupToGuess);
        if (rowGroupUpper <= guessValue) {
            copy(guessUpper, upperX);
            upperX[rowGroupToGuess] = rowGroupUpper;
            return {SolverStatus::TerminatedEarly, iterations};
        }

        auto maxLength = iterationHelper.getMaxLength(guessLower, guessUpper);

        if (maxLength == maxLengthBefore) {
            auto sumLength = iterationHelper.getSumLength(guessLower, guessUpper);
            if (shouldCheckConvergence && sumLength == sumLengthBefore) {
                return {SolverStatus::Aborted, iterations};
            }
            shouldCheckConvergence = true;
            sumLengthBefore = sumLength;
        }
        maxLengthBefore = maxLength;

        if (iterationHelper.getMaxLength(lowerX, upperX) < 2 * precision) {
            return {SolverStatus::Converged, iterations};
        }
    }
    return {SolverStatus::MaximalIterationsExceeded, iterations};
}

template<typename ValueType>
std::pair<SolverStatus, uint64_t> GuessingValueIterationHelper<ValueType>::solveEquations(std::vector<ValueType>& lowerX, std::vector<ValueType>& upperX,
                                                                                              const std::vector<ValueType>& b, ValueType precision,
                                                                                              uint64_t maxOverallIterations,
                                                                                              boost::optional<storm::solver::OptimizationDirection> dir) {
    if (!dir)
        dir = OptimizationDirection::Minimize;
    //    std::cerr << std::fixed << std::setprecision(15);
    auto [status, iterations] = doIterations(dir.get(), lowerX, upperX, b, std::min((uint64_t)lowerX.size(), maxOverallIterations), precision);
    if (status == SolverStatus::Converged || status == SolverStatus::TerminatedEarly)
        return {SolverStatus::Converged, iterations};
    while (iterations < maxOverallIterations) {
        auto rowGroupToGuess = selectRowGroupToGuess(lowerX, upperX);
        bool didVerify = false;
        for (int den = 2; den < 30 && !didVerify; ++den) {
            for (int num = 1; num < den && !didVerify; num++) {
                if (std::gcd(num, den) != 1)
                    continue;
                auto guessValue = (lowerX[rowGroupToGuess] * num + upperX[rowGroupToGuess] * (den - num)) / den;
                auto [solverStatus, verifyIterations] =
                    this->tryVerify(lowerX, upperX, b, dir, rowGroupToGuess, guessValue, maxOverallIterations - iterations, precision);
                iterations += verifyIterations;
                if (solverStatus == SolverStatus::TerminatedEarly || solverStatus == SolverStatus::Converged) {
                    didVerify = true;
                    break;
                }
            }
        }
        if (!didVerify && iterations < maxOverallIterations) {
            auto [status, iterationsDone] = doIterations(dir.get(), lowerX, upperX, b, maxOverallIterations - iterations, precision);
            iterations += iterationsDone;
            if (status == SolverStatus::Converged || status == SolverStatus::TerminatedEarly)
                break;
        } else {
            auto maxLength = iterationHelper.getMaxLength(lowerX, upperX);
            if (maxLength < 2 * precision)
                break;
        }
    }

    return {SolverStatus::Converged, iterations};
}

template<typename ValueType>
void GuessingValueIterationHelper<ValueType>::copy(std::vector<ValueType>& from, std::vector<ValueType>& to) {
    if (to.size() != from.size())
        to.resize(from.size());
    auto itF = from.begin();
    auto itFEnd = from.end();
    auto itT = to.begin();

    for (; itF != itFEnd; ++itF, ++itT) {
        *itT = *itF;
    }
}

template<typename ValueType>
GuessingValueIterationHelper<ValueType>::GuessingValueIterationHelper(const storage::SparseMatrix<ValueType>& matrix) : iterationHelper(matrix) {
    // Intentionally left empty.
}
template<typename ValueType>
std::pair<SolverStatus, uint64_t> GuessingValueIterationHelper<ValueType>::doIterations(storm::solver::OptimizationDirection dir,
                                                                                            std::vector<ValueType>& lowerX, std::vector<ValueType>& upperX,
                                                                                            const std::vector<ValueType>& b, uint64_t maxIterations,
                                                                                            const ValueType& precision) {
    ValueType sumLengthBefore = 0, maxLengthBefore = 0;
    bool shouldCheckConvergence = false;
    uint64_t iterations = 0;
    while (iterations < maxIterations) {
        iterationHelper.template iterate<true>(dir, lowerX, b);
        iterationHelper.template iterate<false>(dir, upperX, b);
        ++iterations;

        auto maxLength = iterationHelper.getMaxLength(lowerX, upperX);
        if (maxLength == maxLengthBefore) {
            auto sumLength = iterationHelper.getSumLength(lowerX, upperX);
            if (shouldCheckConvergence && sumLengthBefore == sumLength)
                return {SolverStatus::TerminatedEarly, iterations};
            shouldCheckConvergence = true;
            sumLengthBefore = sumLength;
        }
        maxLengthBefore = maxLength;

        if (maxLength < 2 * precision) {
            return {SolverStatus::Converged, iterations};
        }
    }

    return {SolverStatus::MaximalIterationsExceeded, iterations};
}

template class GuessingValueIterationHelper<double>;
template class GuessingValueIterationHelper<storm::RationalNumber>;
}  // namespace helper
}  // namespace solver
}  // namespace storm