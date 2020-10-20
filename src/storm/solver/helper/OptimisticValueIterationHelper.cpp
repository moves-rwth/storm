#include "OptimisticValueIterationHelper.h"

#include "storm/utility/vector.h"
#include "storm/utility/SignalHandler.h"
#include "storm/environment/solver/OviSolverEnvironment.h"
#include "storm/utility/ProgressMeasurement.h"

#include "storm/exceptions/NotSupportedException.h"

#include "storm/utility/macros.h"

namespace storm {
    
    namespace solver {
        namespace helper {
            namespace oviinternal {
    
                template<typename ValueType>
                ValueType updateIterationPrecision(storm::Environment const& env, ValueType const& diff) {
                    auto factor = storm::utility::convertNumber<ValueType>(env.solver().ovi().getPrecisionUpdateFactor());
                    return factor * diff;
                }
    
                template<typename ValueType>
                void guessUpperBoundRelative(std::vector<ValueType> const& x, std::vector<ValueType> &target, ValueType const& relativeBoundGuessingScaler) {
                    storm::utility::vector::applyPointwise<ValueType, ValueType>(x, target, [&relativeBoundGuessingScaler] (ValueType const& argument) -> ValueType { return argument * relativeBoundGuessingScaler; });
                }
    
                template<typename ValueType>
                void guessUpperBoundAbsolute(std::vector<ValueType> const& x, std::vector<ValueType> &target, ValueType const& precision) {
                    storm::utility::vector::applyPointwise<ValueType, ValueType>(x, target, [&precision] (ValueType const& argument) -> ValueType { return argument + precision; });
                }
                
                template <typename ValueType>
                IterationHelper<ValueType>::IterationHelper(storm::storage::SparseMatrix<ValueType> const& matrix) {
                    STORM_LOG_THROW(static_cast<uint64_t>(std::numeric_limits<uint32_t>::max()) > matrix.getRowCount() + 1, storm::exceptions::NotSupportedException, "Matrix dimensions too large.");
                    STORM_LOG_THROW(static_cast<uint64_t>(std::numeric_limits<uint32_t>::max()) > matrix.getEntryCount(), storm::exceptions::NotSupportedException, "Matrix dimensions too large.");
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
                    if (!matrix.hasTrivialRowGrouping()) {
                        rowGroupIndices = &matrix.getRowGroupIndices();
                    }
                }
                
                template <typename ValueType>
                ValueType IterationHelper<ValueType>::singleIterationWithDiff(std::vector<ValueType>& x, std::vector<ValueType> const& b, bool computeRelativeDiff) {
                    return singleIterationWithDiffInternal<false, storm::solver::OptimizationDirection::Minimize>(x, b, computeRelativeDiff);
                }
                
                template <typename ValueType>
                ValueType IterationHelper<ValueType>::singleIterationWithDiff(storm::solver::OptimizationDirection const& dir, std::vector<ValueType>& x, std::vector<ValueType> const& b, bool computeRelativeDiff) {
                    if (minimize(dir)) {
                        return singleIterationWithDiffInternal<true, storm::solver::OptimizationDirection::Minimize>(x, b, computeRelativeDiff);
                    } else {
                        return singleIterationWithDiffInternal<true, storm::solver::OptimizationDirection::Maximize>(x, b, computeRelativeDiff);
                    }
                }
                
                template <typename ValueType>
                template<bool HasRowGroups, storm::solver::OptimizationDirection Dir>
                ValueType IterationHelper<ValueType>::singleIterationWithDiffInternal(std::vector<ValueType>& x, std::vector<ValueType> const& b, bool computeRelativeDiff) {
                    STORM_LOG_ASSERT(x.size() > 0, "Empty equation system not expected.");
                    ValueType diff = storm::utility::zero<ValueType>();
                    
                    IndexType i = x.size();
                    while (i > 0) {
                        --i;
                        ValueType newXi = HasRowGroups ? multiplyRowGroup<Dir>(i, b, x) : multiplyRow(i, b[i], x);
                        ValueType& oldXi = x[i];
                        if (computeRelativeDiff) {
                            if (storm::utility::isZero(newXi)) {
                                if (storm::utility::isZero(oldXi)) {
                                    // this operation has no effect:
                                    // diff = std::max(diff, storm::utility::zero<ValueType>());
                                } else {
                                    diff = std::max(diff, storm::utility::one<ValueType>());
                                }
                            } else {
                                diff = std::max(diff, storm::utility::abs<ValueType>((newXi - oldXi) / newXi));
                            }
                        } else {
                            diff = std::max(diff, storm::utility::abs<ValueType>(newXi - oldXi));
                        }
                        oldXi = std::move(newXi);
                    }
                    return diff;
                }
                
                template <typename ValueType>
                uint64_t IterationHelper<ValueType>::repeatedIterate(storm::solver::OptimizationDirection const& dir, std::vector<ValueType>& x, std::vector<ValueType> const& b, ValueType precision, bool relative) {
                    if (minimize(dir)) {
                        return repeatedIterateInternal<true, storm::solver::OptimizationDirection::Minimize>(x, b, precision, relative);
                    } else {
                        return repeatedIterateInternal<true, storm::solver::OptimizationDirection::Maximize>(x, b, precision, relative);
                    }
                }
                
                template <typename ValueType>
                uint64_t IterationHelper<ValueType>::repeatedIterate(std::vector<ValueType>& x, const std::vector<ValueType>& b, ValueType precision, bool relative) {
                    return repeatedIterateInternal<false, storm::solver::OptimizationDirection::Minimize>(x, b, precision, relative);
                }
                
                template <typename ValueType>
                template<bool HasRowGroups, storm::solver::OptimizationDirection Dir>
                uint64_t IterationHelper<ValueType>::repeatedIterateInternal(std::vector<ValueType>& x, std::vector<ValueType> const& b, ValueType precision, bool relative) {
                    // Do a backwards gauss-seidel style iteration
                    bool convergence = true;
                    IndexType i = x.size();
                    while (i > 0) {
                        --i;
                        ValueType newXi = HasRowGroups ? multiplyRowGroup<Dir>(i, b, x) : multiplyRow(i, b[i], x);
                        ValueType& oldXi = x[i];
                        // Check if we converged
                        if (relative) {
                            if (storm::utility::isZero(oldXi)) {
                                if (!storm::utility::isZero(newXi)) {
                                    convergence = false;
                                    break;
                                }
                            } else if (storm::utility::abs<ValueType>((newXi - oldXi) / oldXi) > precision) {
                                convergence = false;
                                break;
                            }
                        } else {
                            if (storm::utility::abs<ValueType>((newXi - oldXi)) > precision) {
                                convergence = false;
                                break;
                            }
                        }
                    }
                    if (!convergence) {
                        // we now know that we did not converge. We still need to set the remaining values
                        while (i > 0) {
                            --i;
                            x[i] = HasRowGroups ? multiplyRowGroup<Dir>(i, b, x) : multiplyRow(i, b[i], x);
                        }
                    }
                    return convergence;
                }
                
                template <typename ValueType>
                typename IterationHelper<ValueType>::IterateResult IterationHelper<ValueType>::iterateUpper(storm::solver::OptimizationDirection const& dir, std::vector<ValueType>& x, std::vector<ValueType> const& b, bool takeMinOfOldAndNew) {
                    if (minimize(dir)) {
                        return iterateUpperInternal<true, storm::solver::OptimizationDirection::Minimize>(x, b, takeMinOfOldAndNew);
                    } else {
                        return iterateUpperInternal<true, storm::solver::OptimizationDirection::Maximize>(x, b, takeMinOfOldAndNew);
                    }
                }
                
                template <typename ValueType>
                typename IterationHelper<ValueType>::IterateResult IterationHelper<ValueType>::iterateUpper(std::vector<ValueType>& x, std::vector<ValueType> const& b, bool takeMinOfOldAndNew) {
                    return iterateUpperInternal<false, storm::solver::OptimizationDirection::Minimize>(x, b, takeMinOfOldAndNew);
                }
                
                template <typename ValueType>
                template<bool HasRowGroups, storm::solver::OptimizationDirection Dir>
                typename IterationHelper<ValueType>::IterateResult IterationHelper<ValueType>::iterateUpperInternal(std::vector<ValueType>& x, std::vector<ValueType> const& b, bool takeMinOfOldAndNew) {
                    // For each row compare the new upper bound candidate with the old one
                    bool newUpperBoundAlwaysHigherEqual = true;
                    bool newUpperBoundAlwaysLowerEqual = true;
                    // Do a backwards gauss-seidel style iteration
                    for (IndexType i = x.size(); i > 0;) {
                        --i;
                        ValueType newXi = HasRowGroups ? multiplyRowGroup<Dir>(i, b, x) : multiplyRow(i, b[i], x);
                        ValueType& oldXi = x[i];
                        if (newXi > oldXi) {
                            newUpperBoundAlwaysLowerEqual = false;
                            if (!takeMinOfOldAndNew) {
                                oldXi = newXi;
                            }
                        } else if (newXi != oldXi) {
                            assert(newXi < oldXi);
                            newUpperBoundAlwaysHigherEqual = false;
                            oldXi = newXi;
                        }
                    }
                    // Return appropriate result
                    if (newUpperBoundAlwaysLowerEqual) {
                        if (newUpperBoundAlwaysHigherEqual) {
                            return IterateResult::Equal;
                        } else {
                            return IterateResult::AlwaysLowerOrEqual;
                        }
                    } else {
                        if (newUpperBoundAlwaysHigherEqual) {
                            return IterateResult::AlwaysHigherOrEqual;
                        } else {
                            return IterateResult::Incomparable;
                        }
                    }
                }
                
                template <typename ValueType>
                ValueType IterationHelper<ValueType>::multiplyRow(IndexType const& rowIndex, ValueType const& bi, std::vector<ValueType> const& x) {
                    assert(rowIndex < rowIndications.size());
                    ValueType xRes = bi;
                    
                    auto entryIt = matrixValues.begin() + rowIndications[rowIndex];
                    auto entryItE = matrixValues.begin() + rowIndications[rowIndex + 1];
                    auto colIt = matrixColumns.begin() + rowIndications[rowIndex];
                    for (; entryIt != entryItE; ++entryIt, ++colIt) {
                        xRes += *entryIt * x[*colIt];
                    }
                    return xRes;
                }
                
                template <typename ValueType>
                template<storm::solver::OptimizationDirection Dir>
                ValueType IterationHelper<ValueType>::multiplyRowGroup(IndexType const& rowGroupIndex, std::vector<ValueType> const& b, std::vector<ValueType> const& x) {
                    STORM_LOG_ASSERT(rowGroupIndices != nullptr, "No row group indices available.");
                    auto row = (*rowGroupIndices)[rowGroupIndex];
                    auto const& groupEnd = (*rowGroupIndices)[rowGroupIndex + 1];
                    STORM_LOG_ASSERT(row < groupEnd, "Empty row group not expected.");
                    ValueType xRes = multiplyRow(row, b[row], x);
                    for (++row; row < groupEnd; ++row) {
                        ValueType xCur = multiplyRow(row, b[row], x);
                        xRes = minimize(Dir) ? std::min(xRes, xCur) : std::max(xRes, xCur);
                    }
                    return xRes;
                }
            }
            
            template<typename ValueType>
            OptimisticValueIterationHelper<ValueType>::OptimisticValueIterationHelper(storm::storage::SparseMatrix<ValueType> const& matrix) : iterationHelper(matrix) {
                    // Intentionally left empty.
            }
                
            template<typename ValueType>
                std::pair<SolverStatus, uint64_t> OptimisticValueIterationHelper<ValueType>::solveEquations(Environment const& env, std::vector<ValueType>* lowerX, std::vector<ValueType>* upperX, std::vector<ValueType> const& b, bool relative, ValueType precision, uint64_t maxOverallIterations, boost::optional<storm::solver::OptimizationDirection> dir, boost::optional<storm::storage::BitVector> const& relevantValues) {
                STORM_LOG_ASSERT(lowerX->size() == upperX->size(), "Dimension missmatch.");
                
                // As we will shuffle pointers around, let's store the original positions here.
                std::vector<ValueType>* initLowerX = lowerX;
                std::vector<ValueType>* initUpperX = upperX;
                
                uint64_t overallIterations = 0;
                uint64_t lastValueIterationIterations = 0;
                uint64_t currentVerificationIterations = 0;
                
                // Get some parameters for the algorithm
                // 2
                ValueType two = storm::utility::convertNumber<ValueType>(2.0);
                // Use no termination guaranteed upper bound iteration method
                bool noTerminationGuarantee = env.solver().ovi().useNoTerminationGuaranteeMinimumMethod();
                // Desired max difference between upperX and lowerX
                ValueType doublePrecision = precision * two;
                // Upper bound only iterations
                uint64_t upperBoundOnlyIterations = env.solver().ovi().getUpperBoundOnlyIterations();
                ValueType relativeBoundGuessingScaler = (storm::utility::one<ValueType>() + storm::utility::convertNumber<ValueType>(env.solver().ovi().getUpperBoundGuessingFactor()) * precision);
                // Initial precision for the value iteration calls
                ValueType iterationPrecision = precision;
    
                SolverStatus status = SolverStatus::InProgress;
    
                storm::utility::ProgressMeasurement progress("iterations.");
                progress.startNewMeasurement(0);
                while (status == SolverStatus::InProgress && overallIterations < maxOverallIterations) {
                    // Perform value iteration until convergence
                    lastValueIterationIterations = dir ? iterationHelper.repeatedIterate(dir.get(), *lowerX, b, iterationPrecision, relative) : iterationHelper.repeatedIterate(*lowerX, b, iterationPrecision, relative);
                    overallIterations += lastValueIterationIterations;

                    bool intervalIterationNeeded = false;
                    currentVerificationIterations = 0;

                    if (relative) {
                        oviinternal::guessUpperBoundRelative(*lowerX, *upperX, relativeBoundGuessingScaler);
                    } else {
                        oviinternal::guessUpperBoundAbsolute(*lowerX, *upperX, precision);
                    }

                    bool cancelGuess = false;
                    while (status == SolverStatus::InProgress && overallIterations < maxOverallIterations) {
                        if (storm::utility::resources::isTerminate()) {
                            status = SolverStatus::Aborted;
                        }
                        ++overallIterations;
                        ++currentVerificationIterations;
                        // Perform value iteration stepwise for lower bound and guessed upper bound

                        // Upper bound iteration
                        auto upperBoundIterResult = dir ? iterationHelper.iterateUpper(dir.get(), *upperX, b, !noTerminationGuarantee) : iterationHelper.iterateUpper(*upperX, b, !noTerminationGuarantee);

                        if (upperBoundIterResult == oviinternal::IterationHelper<ValueType>::IterateResult::AlwaysHigherOrEqual) {
                            // All values moved up (and did not stay the same)
                            // That means the guess for an upper bound is actually a lower bound
                            auto diff = dir ? iterationHelper.singleIterationWithDiff(dir.get(), *upperX, b, relative) : iterationHelper.singleIterationWithDiff(*upperX, b, relative);
                            iterationPrecision = oviinternal::updateIterationPrecision(env, diff);
                            // We assume to have a single fixed point. We can thus safely set the new lower bound, to the wrongly guessed upper bound
                            // Set lowerX to the upper bound candidate
                            std::swap(lowerX, upperX);
                            break;
                        } else if (upperBoundIterResult == oviinternal::IterationHelper<ValueType>::IterateResult::AlwaysLowerOrEqual) {
                            // All values moved down (and stayed not the same)
                            // This is a valid upper bound. We still need to check the precision.
                            // We can safely use twice the requested precision, as we calculate the center of both vectors
                            bool reachedPrecision;
                            if (relevantValues) {
                                reachedPrecision = storm::utility::vector::equalModuloPrecision(*lowerX, *upperX, relevantValues.get(), doublePrecision, relative);
                            } else {
                                reachedPrecision = storm::utility::vector::equalModuloPrecision(*lowerX, *upperX, doublePrecision, relative);
                            }
                            if (reachedPrecision) {
                                status = SolverStatus::Converged;
                                break;
                            } else {
                                // From now on, we keep updating both bounds
                                intervalIterationNeeded = true;
                            }
                        // The following case below covers that both vectors (old and new) are equal.
                        // Theoretically, this means that the precise fixpoint has been reached. However, numerical instabilities can be tricky and this detection might be incorrect (see the haddad-monmege model).
                        // We therefore disable it. It is very unlikely that we guessed the right fixpoint anyway.
                        //} else if (upperBoundIterResult == oviinternal::IterationHelper<ValueType>::IterateResult::Equal) {
                            // In this case, the guessed upper bound is the precise fixpoint
                        //    status = SolverStatus::Converged;
                        //    break;
                        }

                        // Check whether we tried this guess for too long
                        ValueType scaledIterationCount = storm::utility::convertNumber<ValueType>(currentVerificationIterations) * storm::utility::convertNumber<ValueType>(env.solver().ovi().getMaxVerificationIterationFactor());
                        if (!intervalIterationNeeded && scaledIterationCount * iterationPrecision >= storm::utility::one<ValueType>()) {
                            cancelGuess = true;
                            // In this case we will make one more iteration on the lower bound (mainly to obtain a new iterationPrecision)
                        }

                        // Lower bound iteration (only if needed)
                        if (cancelGuess || intervalIterationNeeded || currentVerificationIterations > upperBoundOnlyIterations) {
                            auto diff = dir ? iterationHelper.singleIterationWithDiff(dir.get(), *lowerX, b, relative) : iterationHelper.singleIterationWithDiff(*lowerX, b, relative);

                            // Check whether the upper and lower bounds have crossed, i.e., the upper bound is smaller than the lower bound.
                            bool valuesCrossed = false;
                            for (uint64_t i = 0; i < lowerX->size(); ++i) {
                                if ((*upperX)[i] < (*lowerX)[i]) {
                                    valuesCrossed = true;
                                    break;
                                }
                            }

                            if (cancelGuess || valuesCrossed) {
                                // A new guess is needed.
                                iterationPrecision = oviinternal::updateIterationPrecision(env, diff);
                                break;
                            }
                        }
                    }
                    if (storm::utility::resources::isTerminate()) {
                        status = SolverStatus::Aborted;
                    }
                    progress.updateProgress(overallIterations);
                } // end while
                // Swap the results into the output vectors (if necessary).
                assert(initLowerX != lowerX || (initLowerX == lowerX && initUpperX == upperX));
                if (initLowerX != lowerX) {
                    assert(initUpperX == lowerX);
                    assert(initLowerX == upperX);
                    lowerX->swap(*upperX);
                }
                
                if (overallIterations > maxOverallIterations) {
                    status = SolverStatus::MaximalIterationsExceeded;
                }
                
                return {status, overallIterations};
            }

            
            template class OptimisticValueIterationHelper<double>;
            template class OptimisticValueIterationHelper<storm::RationalNumber>;
        }
    }
}

