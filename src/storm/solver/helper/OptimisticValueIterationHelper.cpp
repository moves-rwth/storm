#include "OptimisticValueIterationHelper.h"

#include "storm/utility/vector.h"
#include "storm/utility/SignalHandler.h"
#include "storm/environment/solver/OviSolverEnvironment.h"

#include "storm/exceptions/NotSupportedException.h"

#include "storm/utility/macros.h"

namespace storm {
    
    namespace solver {
        namespace helper {
            namespace oviinternal {
    
                template<typename ValueType>
                ValueType computeMaxAbsDiff(std::vector<ValueType> const& allOldValues, std::vector<ValueType> const& allNewValues, storm::storage::BitVector const& relevantValues) {
                    ValueType result = storm::utility::zero<ValueType>();
                    for (auto value : relevantValues) {
                        result = storm::utility::max<ValueType>(result, storm::utility::abs<ValueType>(allNewValues[value] - allOldValues[value]));
                    }
                    return result;
                }
        
                template<typename ValueType>
                ValueType computeMaxAbsDiff(std::vector<ValueType> const& allOldValues, std::vector<ValueType> const& allNewValues) {
                    ValueType result = storm::utility::zero<ValueType>();
                    for (uint64_t i = 0; i < allOldValues.size(); ++i) {
                        result = storm::utility::max<ValueType>(result, storm::utility::abs<ValueType>(allNewValues[i] - allOldValues[i]));
                    }
                    return result;
                }
            
                template<typename ValueType>
                ValueType computeMaxRelDiff(std::vector<ValueType> const& allOldValues, std::vector<ValueType> const& allNewValues, storm::storage::BitVector const& relevantValues) {
                    ValueType result = storm::utility::zero<ValueType>();
                    for (auto const& i : relevantValues) {
                        STORM_LOG_ASSERT(!storm::utility::isZero(allNewValues[i]) || storm::utility::isZero(allOldValues[i]), "Unexpected entry in iteration vector.");
                        if (!storm::utility::isZero(allNewValues[i])) {
                            result = storm::utility::max<ValueType>(result, storm::utility::abs<ValueType>(allNewValues[i] - allOldValues[i]) / allNewValues[i]);
                        }
                    }
                    return result;
                }
                
                template<typename ValueType>
                ValueType computeMaxRelDiff(std::vector<ValueType> const& allOldValues, std::vector<ValueType> const& allNewValues) {
                    ValueType result = storm::utility::zero<ValueType>();
                    for (uint64_t i = 0; i < allOldValues.size(); ++i) {
                        STORM_LOG_ASSERT(!storm::utility::isZero(allNewValues[i]) || storm::utility::isZero(allOldValues[i]), "Unexpected entry in iteration vector.");
                        if (!storm::utility::isZero(allNewValues[i])) {
                            result = storm::utility::max<ValueType>(result, storm::utility::abs<ValueType>(allNewValues[i] - allOldValues[i]) / allNewValues[i]);
                        }
                    }
                    return result;
                }
    
                template<typename ValueType>
                ValueType updateIterationPrecision(storm::Environment const& env, std::vector<ValueType> const& currentX, std::vector<ValueType> const& newX, bool const& relative, boost::optional<storm::storage::BitVector> const& relevantValues) {
                    auto factor = storm::utility::convertNumber<ValueType>(env.solver().ovi().getPrecisionUpdateFactor());
                    bool useRelevant = relevantValues.is_initialized() && env.solver().ovi().useRelevantValuesForPrecisionUpdate();
                    if (relative) {
                        return (useRelevant ? computeMaxRelDiff(newX, currentX, relevantValues.get()) : computeMaxRelDiff(newX, currentX)) * factor;
                    } else {
                        return (useRelevant ? computeMaxAbsDiff(newX, currentX, relevantValues.get()) : computeMaxAbsDiff(newX, currentX)) * factor;
                    }
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
                UpperBoundIterator<ValueType>::UpperBoundIterator(storm::storage::SparseMatrix<ValueType> const& matrix) {
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
                typename UpperBoundIterator<ValueType>::IterateResult UpperBoundIterator<ValueType>::iterate(std::vector<ValueType>& x, std::vector<ValueType> const& b, bool takeMinOfOldAndNew) {
                    return iterateInternal<false, storm::solver::OptimizationDirection::Minimize>(x, b, takeMinOfOldAndNew);
                }
                
                template <typename ValueType>
                typename UpperBoundIterator<ValueType>::IterateResult UpperBoundIterator<ValueType>::iterate(storm::solver::OptimizationDirection const& dir, std::vector<ValueType>& x, std::vector<ValueType> const& b, bool takeMinOfOldAndNew) {
                    if (minimize(dir)) {
                        return iterateInternal<true, storm::solver::OptimizationDirection::Minimize>(x, b, takeMinOfOldAndNew);
                    } else {
                        return iterateInternal<true, storm::solver::OptimizationDirection::Maximize>(x, b, takeMinOfOldAndNew);
                    }
                }
                
                template <typename ValueType>
                template<bool HasRowGroups, storm::solver::OptimizationDirection Dir>
                typename UpperBoundIterator<ValueType>::IterateResult UpperBoundIterator<ValueType>::iterateInternal(std::vector<ValueType>& x, std::vector<ValueType> const& b, bool takeMinOfOldAndNew) {
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
                ValueType UpperBoundIterator<ValueType>::multiplyRow(IndexType const& rowIndex, ValueType const& bi, std::vector<ValueType> const& x) {
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
                ValueType UpperBoundIterator<ValueType>::multiplyRowGroup(IndexType const& rowGroupIndex, std::vector<ValueType> const& b, std::vector<ValueType> const& x) {
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
            OptimisticValueIterationHelper<ValueType>::OptimisticValueIterationHelper(storm::storage::SparseMatrix<ValueType> const& matrix) : upperBoundIterator(matrix) {
                    // Intentionally left empty.
                }
                
            template<typename ValueType>
                std::pair<SolverStatus, uint64_t> OptimisticValueIterationHelper<ValueType>::solveEquationsOptimisticValueIteration(Environment const& env, std::vector<ValueType>* lowerX, std::vector<ValueType>* upperX, std::vector<ValueType>* auxVector, std::vector<ValueType> const& b, ValueIterationCallBackType const& valueIterationCallback, SingleIterationCallBackType const& singleIterationCallback, bool relative, ValueType precision, uint64_t maxOverallIterations, boost::optional<storm::solver::OptimizationDirection> dir, boost::optional<storm::storage::BitVector> relevantValues) {
                STORM_LOG_ASSERT(lowerX->size() == upperX->size(), "Dimension missmatch.");
                STORM_LOG_ASSERT(lowerX->size() == auxVector->size(), "Dimension missmatch.");
                
                // As we will shuffle pointers around, let's store the original positions here.
                std::vector<ValueType>* initLowerX = lowerX;
                std::vector<ValueType>* initUpperX = upperX;
                std::vector<ValueType>* initAux = auxVector;
                
                uint64_t overallIterations = 0;
                uint64_t lastValueIterationIterations = 0;
                uint64_t currentVerificationIterations = 0;
                uint64_t valueIterationInvocations = 0;
                
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
    
                while (status == SolverStatus::InProgress && overallIterations < maxOverallIterations) {

                    // Perform value iteration until convergence
                    ++valueIterationInvocations;
                    auto result = valueIterationCallback(lowerX, auxVector, iterationPrecision, relative, overallIterations, maxOverallIterations);
                    lastValueIterationIterations = result.first;
                    overallIterations += result.first;

                    if (result.second != SolverStatus::Converged) {
                        status = result.second;
                    } else {
                        bool intervalIterationNeeded = false;
                        currentVerificationIterations = 0;

                        if (relative) {
                            oviinternal::guessUpperBoundRelative(*lowerX, *upperX, relativeBoundGuessingScaler);
                        } else {
                            oviinternal::guessUpperBoundAbsolute(*lowerX, *upperX, precision);
                        }

                        bool cancelGuess = false;
                        while (status == SolverStatus::InProgress && overallIterations < maxOverallIterations) {
                            ++overallIterations;
                            ++currentVerificationIterations;
                            // Perform value iteration stepwise for lower bound and guessed upper bound

                            // Upper bound iteration
                            auto upperBoundIterResult = dir ? upperBoundIterator.iterate(dir.get(), *upperX, b, !noTerminationGuarantee) : upperBoundIterator.iterate(*upperX, b, !noTerminationGuarantee);

                            if (upperBoundIterResult == oviinternal::UpperBoundIterator<ValueType>::IterateResult::AlwaysHigherOrEqual) {
                                // All values moved up (and did not stay the same)
                                // That means the guess for an upper bound is actually a lower bound
                                iterationPrecision = oviinternal::updateIterationPrecision(env, *auxVector, *upperX, relative, relevantValues);
                                // We assume to have a single fixed point. We can thus safely set the new lower bound, to the wrongly guessed upper bound
                                // Set lowerX to the upper bound candidate
                                std::swap(lowerX, upperX);
                                break;
                            } else if (upperBoundIterResult == oviinternal::UpperBoundIterator<ValueType>::IterateResult::AlwaysLowerOrEqual) {
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
                            //} else if (upperBoundIterResult == oviinternal::UpperBoundIterator<ValueType>::IterateResult::Equal) {
                                // In this case, the guessed upper bound is the precise fixpoint
                            //    status = SolverStatus::Converged;
                            //    std::swap(lowerX, auxVector);
                            //    break;
                            }

                            // Check whether we tried this guess for too long
                            ValueType scaledIterationCount = storm::utility::convertNumber<ValueType>(currentVerificationIterations) * storm::utility::convertNumber<ValueType>(env.solver().ovi().getMaxVerificationIterationFactor());
                            if (!intervalIterationNeeded && scaledIterationCount >= storm::utility::convertNumber<ValueType>(lastValueIterationIterations)) {
                                cancelGuess = true;
                                // In this case we will make one more iteration on the lower bound (mainly to obtain a new iterationPrecision)
                            }

                            // Lower bound iteration (only if needed)
                            if (cancelGuess || intervalIterationNeeded || currentVerificationIterations > upperBoundOnlyIterations) {
                                singleIterationCallback(lowerX, auxVector, overallIterations);
                                // At this point, auxVector contains the old values for the lower bound whereas lowerX contains the new ones.

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
                                    iterationPrecision = oviinternal::updateIterationPrecision(env, *auxVector, *lowerX, relative, relevantValues);
                                    break;
                                }
                            }
                        }
                        if (storm::utility::resources::isTerminate()) {
                            status = SolverStatus::Aborted;
                        }
                    }
                } // end while
                
                // Swap the results into the output vectors.
                if (initLowerX == lowerX) {
                    // lowerX is already at the correct position. We still have to care for upperX
                    if (initUpperX != upperX) {
                        // UpperX is not at the correct position. It has to be at the auxVector
                        assert(initAux == upperX);
                        std::swap(*initUpperX, *initAux);
                    }
                } else if (initUpperX == upperX) {
                    // UpperX is already at the correct position.
                    // We already know that lowerX is at the wrong position. It has to be at the auxVector
                    assert(initAux == lowerX);
                    std::swap(*initLowerX, *initAux);
                } else if (initAux == auxVector) {
                    // We know that upperX and lowerX are swapped.
                    assert(initLowerX == upperX);
                    assert(initUpperX == lowerX);
                    std::swap(*initUpperX, *initLowerX);
                } else {
                    // Now we know that all vectors are at the wrong position. There are only two possibilities left
                    if (initLowerX == upperX) {
                        assert(initUpperX == auxVector);
                        assert(initAux == lowerX);
                        std::swap(*initLowerX, *initAux);
                        std::swap(*initUpperX, *initAux);
                    } else {
                        assert(initLowerX == auxVector);
                        assert(initUpperX == lowerX);
                        assert (initAux == upperX);
                        std::swap(*initUpperX, *initAux);
                        std::swap(*initLowerX, *initAux);
                    }
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

