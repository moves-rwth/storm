#pragma once

#include <vector>
#include <boost/optional.hpp>

#include "storm/solver/OptimizationDirection.h"
#include "storm/solver/SolverStatus.h"
#include "storm/utility/vector.h"
#include "storm/utility/ProgressMeasurement.h"
#include "storm/utility/SignalHandler.h"
#include "storm/storage/BitVector.h"
#include "storm/environment/solver/MinMaxSolverEnvironment.h"
#include "storm/environment/solver/OviSolverEnvironment.h"

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
            
            }
            
            
            /*!
             * Performs Optimistic value iteration.
             * See https://arxiv.org/abs/1910.01100 for more information on this algorithm
             *
             * @tparam ValueType
             * @tparam ValueType
             * @param env
             * @param lowerX Needs to be some arbitrary lower bound on the actual values initially
             * @param upperX Does not need to be an upper bound initially
             * @param auxVector auxiliary storage
             * @param valueIterationCallback  Function that should perform standard value iteration on the input vector
             * @param singleIterationCallback Function that should perform a single value iteration step on the input vector e.g. ( x' = min/max(A*x + b))
             * @param relevantValues If given, we only check the precision at the states with the given indices.
             * @return The status upon termination as well as the number of iterations Also, the maximum (relative/absolute) difference between lowerX and upperX will be 2*epsilon
             * with precision parameters as given by the environment env.
             */
            template<typename ValueType, typename ValueIterationCallback, typename SingleIterationCallback>
            std::pair<SolverStatus, uint64_t> solveEquationsOptimisticValueIteration(Environment const& env, std::vector<ValueType>* lowerX, std::vector<ValueType>* upperX, std::vector<ValueType>* auxVector, ValueIterationCallback const& valueIterationCallback, SingleIterationCallback const& singleIterationCallback, boost::optional<storm::storage::BitVector> relevantValues = boost::none) {
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
                // Relative errors
                bool relative = env.solver().minMax().getRelativeTerminationCriterion();
                // Goal precision
                ValueType precision = storm::utility::convertNumber<ValueType>(env.solver().minMax().getPrecision());
                // Desired max difference between upperX and lowerX
                ValueType doublePrecision = precision * two;
                // Upper bound only iterations
                uint64_t upperBoundOnlyIterations = env.solver().ovi().getUpperBoundOnlyIterations();
                // Maximum number of iterations done overall
                uint64_t maxOverallIterations = env.solver().minMax().getMaximalNumberOfIterations();
                ValueType relativeBoundGuessingScaler = (storm::utility::one<ValueType>() + storm::utility::convertNumber<ValueType>(env.solver().ovi().getUpperBoundGuessingFactor()) * precision);
                // Initial precision for the value iteration calls
                ValueType iterationPrecision = precision;
    
                SolverStatus status = SolverStatus::InProgress;
    
                while (status == SolverStatus::InProgress && overallIterations < maxOverallIterations) {
    
                    // Perform value iteration until convergence
                    ++valueIterationInvocations;
                    auto result = valueIterationCallback(lowerX, auxVector, iterationPrecision, relative, overallIterations, maxOverallIterations);
                    lastValueIterationIterations = result.iterations;
                    overallIterations += result.iterations;
    
                    if (result.status != SolverStatus::Converged) {
                        status = result.status;
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
                            singleIterationCallback(upperX, auxVector, overallIterations);
                            // At this point, auxVector contains the old values for the upper bound whereas upperX contains the new ones.
                            
                            // Compare the new upper bound candidate with the old one
                            bool newUpperBoundAlwaysHigherEqual = true;
                            bool newUpperBoundAlwaysLowerEqual = true;
                            for (uint64_t i = 0; i < upperX->size(); ++i) {
                                if ((*auxVector)[i] > (*upperX)[i]) {
                                    newUpperBoundAlwaysHigherEqual = false;
                                } else if ((*auxVector)[i] != (*upperX)[i]) {
                                    newUpperBoundAlwaysLowerEqual = false;
                                }
                            }
                            if (newUpperBoundAlwaysHigherEqual & !newUpperBoundAlwaysLowerEqual) {
                                // All values moved up or stayed the same (but are not the same)
                                // That means the guess for an upper bound is actually a lower bound
                                iterationPrecision = oviinternal::updateIterationPrecision(env, *auxVector, *upperX, relative, relevantValues);
                                // We assume to have a single fixed point. We can thus safely set the new lower bound, to the wrongly guessed upper bound
                                // Set lowerX to the upper bound candidate
                                std::swap(lowerX, upperX);
                                break;
                            } else if (newUpperBoundAlwaysLowerEqual) {
                                // All values moved down or stayed the same and we have a maximum difference of twice the requested precision
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
                            }
                            // At this point, the old upper bounds (auxVector) are not needed anymore.
                            
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
        }
    }
}

