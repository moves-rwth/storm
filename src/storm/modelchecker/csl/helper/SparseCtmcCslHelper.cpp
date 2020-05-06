#include "storm/modelchecker/csl/helper/SparseCtmcCslHelper.h"

#include "storm/modelchecker/prctl/helper/SparseDtmcPrctlHelper.h"
#include "storm/modelchecker/reachability/SparseDtmcEliminationModelChecker.h"

#include "storm/models/sparse/StandardRewardModel.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GeneralSettings.h"

#include "storm/solver/LinearEquationSolver.h"
#include "storm/solver/Multiplier.h"

#include "storm/storage/StronglyConnectedComponentDecomposition.h"

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/environment/solver/LongRunAverageSolverEnvironment.h"
#include "storm/environment/solver/TopologicalSolverEnvironment.h"
#include "storm/environment/solver/TimeBoundedSolverEnvironment.h"

#include "storm/utility/macros.h"
#include "storm/utility/vector.h"
#include "storm/utility/graph.h"
#include "storm/utility/numerical.h"
#include "storm/utility/SignalHandler.h"

#include "storm/exceptions/InvalidOperationException.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/exceptions/FormatUnsupportedBySolverException.h"
#include "storm/exceptions/UncheckedRequirementException.h"
#include "storm/exceptions/NotSupportedException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template <typename ValueType>
            bool SparseCtmcCslHelper::checkAndUpdateTransientProbabilityEpsilon(storm::Environment const& env, ValueType& epsilon, std::vector<ValueType> const& resultVector, storm::storage::BitVector const& relevantPositions) {
                // Check if the check is necessary for the provided settings
                if (!env.solver().isForceSoundness() || !env.solver().timeBounded().getRelativeTerminationCriterion()) {
                    // No need to update epsilon
                    return false;
                }
                
                ValueType precision = storm::utility::convertNumber<ValueType>(env.solver().timeBounded().getPrecision());
                // If we need to compute values with relative precision, it might be necessary to increase the precision requirements (epsilon)
                ValueType newEpsilon = epsilon;
                // Only consider positions that are relevant for the solve goal (e.g. initial states of the model) and are supposed to have a non-zero value
                for (auto const& state : relevantPositions) {
                    if (storm::utility::isZero(resultVector[state])) {
                        newEpsilon = std::min(epsilon * storm::utility::convertNumber<ValueType>(0.1), newEpsilon);
                    } else {
                        ValueType relativeError = epsilon / resultVector[state]; // epsilon is an upper bound for the absolute error we made
                        if (relativeError > precision) {
                            newEpsilon = std::min(resultVector[state] * precision, newEpsilon);
                        }
                    }
                }
                if (newEpsilon < epsilon) {
                    STORM_LOG_INFO("Re-computing transient probabilities with new truncation error " << newEpsilon << " to guarantee sound results with relative precision.");
                    epsilon = newEpsilon;
                    return true;
                } else {
                    return false;
                }
            }
            
            
            template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type>
            std::vector<ValueType> SparseCtmcCslHelper::computeBoundedUntilProbabilities(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal, storm::storage::SparseMatrix<ValueType> const& rateMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, std::vector<ValueType> const& exitRates, bool qualitative, double lowerBound, double upperBound) {
                
                STORM_LOG_THROW(!env.solver().isForceExact(), storm::exceptions::InvalidOperationException, "Exact computations not possible for bounded until probabilities.");
                
                uint_fast64_t numberOfStates = rateMatrix.getRowCount();
                
                // If the time bounds are [0, inf], we rather call untimed reachability.
                if (storm::utility::isZero(lowerBound) && upperBound == storm::utility::infinity<ValueType>()) {
                    return computeUntilProbabilities(env, std::move(goal), rateMatrix, backwardTransitions, exitRates, phiStates, psiStates, qualitative);
                }
                
                // From this point on, we know that we have to solve a more complicated problem [t, t'] with either t != 0
                // or t' != inf.
                
                // Create the result vector.
                std::vector<ValueType> result;
                
                // Set the possible (absolute) error allowed for truncation (epsilon for fox-glynn)
                ValueType epsilon = storm::utility::convertNumber<ValueType>(env.solver().timeBounded().getPrecision()) / 8.0;
                
                // If we identify the states that have probability 0 of reaching the target states, we can exclude them from the
                // further computations.
                storm::storage::BitVector statesWithProbabilityGreater0 = storm::utility::graph::performProbGreater0(backwardTransitions, phiStates, psiStates);
                STORM_LOG_INFO("Found " << statesWithProbabilityGreater0.getNumberOfSetBits() << " states with probability greater 0.");
                storm::storage::BitVector statesWithProbabilityGreater0NonPsi = statesWithProbabilityGreater0 & ~psiStates;
                STORM_LOG_INFO("Found " << statesWithProbabilityGreater0NonPsi.getNumberOfSetBits() << " 'maybe' states.");
                
                // the positions within the result for which the precision needs to be checked
                storm::storage::BitVector relevantValues;
                if (goal.hasRelevantValues()) {
                    relevantValues = std::move(goal.relevantValues());
                    relevantValues &= statesWithProbabilityGreater0;
                } else {
                    relevantValues = statesWithProbabilityGreater0;
                }
                
                do { // Iterate until the desired precision is reached (only relevant for relative precision criterion)
                    if (!statesWithProbabilityGreater0.empty()) {
                        if (storm::utility::isZero(upperBound)) {
                            // In this case, the interval is of the form [0, 0].
                            result = std::vector<ValueType>(numberOfStates, storm::utility::zero<ValueType>());
                            storm::utility::vector::setVectorValues<ValueType>(result, psiStates, storm::utility::one<ValueType>());
                        } else {
                            if (storm::utility::isZero(lowerBound)) {
                                // In this case, the interval is of the form [0, t].
                                // Note that this excludes [0, inf] since this is untimed reachability and we considered this case earlier.
                                
                                result = std::vector<ValueType>(numberOfStates, storm::utility::zero<ValueType>());
                                storm::utility::vector::setVectorValues<ValueType>(result, psiStates, storm::utility::one<ValueType>());
                                if (!statesWithProbabilityGreater0NonPsi.empty()) {
                                    // Find the maximal rate of all 'maybe' states to take it as the uniformization rate.
                                    ValueType uniformizationRate = 0;
                                    for (auto const& state : statesWithProbabilityGreater0NonPsi) {
                                        uniformizationRate = std::max(uniformizationRate, exitRates[state]);
                                    }
                                    uniformizationRate *= 1.02;
                                    STORM_LOG_THROW(uniformizationRate > 0, storm::exceptions::InvalidStateException, "The uniformization rate must be positive.");
                                    
                                    // Compute the uniformized matrix.
                                    storm::storage::SparseMatrix<ValueType> uniformizedMatrix = computeUniformizedMatrix(rateMatrix, statesWithProbabilityGreater0NonPsi, uniformizationRate, exitRates);
                                    
                                    // Compute the vector that is to be added as a compensation for removing the absorbing states.
                                    std::vector<ValueType> b = rateMatrix.getConstrainedRowSumVector(statesWithProbabilityGreater0NonPsi, psiStates);
                                    for (auto& element : b) {
                                        element /= uniformizationRate;
                                    }
                                    
                                    // Finally compute the transient probabilities.
                                    std::vector<ValueType> values(statesWithProbabilityGreater0NonPsi.getNumberOfSetBits(), storm::utility::zero<ValueType>());
                                    std::vector<ValueType> subresult = computeTransientProbabilities(env, uniformizedMatrix, &b, upperBound, uniformizationRate, values, epsilon);
                                    storm::utility::vector::setVectorValues(result, statesWithProbabilityGreater0NonPsi, subresult);
                                }
                            } else if (upperBound == storm::utility::infinity<ValueType>()) {
                                // In this case, the interval is of the form [t, inf] with t != 0.
                                
                                // Start by computing the (unbounded) reachability probabilities of reaching psi states while
                                // staying in phi states.
                                result = computeUntilProbabilities(env, storm::solver::SolveGoal<ValueType>(), rateMatrix, backwardTransitions, exitRates, phiStates, psiStates, qualitative);
                                
                                // Determine the set of states that must be considered further.
                                storm::storage::BitVector relevantStates = statesWithProbabilityGreater0 & phiStates;
                                std::vector<ValueType> subResult(relevantStates.getNumberOfSetBits());
                                storm::utility::vector::selectVectorValues(subResult, relevantStates, result);
                                
                                ValueType uniformizationRate = 0;
                                for (auto const& state : relevantStates) {
                                    uniformizationRate = std::max(uniformizationRate, exitRates[state]);
                                }
                                uniformizationRate *= 1.02;
                                STORM_LOG_THROW(uniformizationRate > 0, storm::exceptions::InvalidStateException, "The uniformization rate must be positive.");
                                
                                // Compute the uniformized matrix.
                                storm::storage::SparseMatrix<ValueType> uniformizedMatrix = computeUniformizedMatrix(rateMatrix, relevantStates, uniformizationRate, exitRates);
                                
                                // Compute the transient probabilities.
                                subResult = computeTransientProbabilities<ValueType>(env, uniformizedMatrix, nullptr, lowerBound, uniformizationRate, subResult, epsilon);
                                
                                // Fill in the correct values.
                                storm::utility::vector::setVectorValues(result, ~relevantStates, storm::utility::zero<ValueType>());
                                storm::utility::vector::setVectorValues(result, relevantStates, subResult);
                            } else {
                                // In this case, the interval is of the form [t, t'] with t != 0 and t' != inf.
                                
                                if (lowerBound != upperBound) {
                                    // In this case, the interval is of the form [t, t'] with t != 0, t' != inf and t != t'.
                                    
                                    
                                    storm::storage::BitVector relevantStates = statesWithProbabilityGreater0 & phiStates;
                                    std::vector<ValueType> newSubresult(relevantStates.getNumberOfSetBits(), storm::utility::zero<ValueType>());
                                    storm::utility::vector::setVectorValues(newSubresult, psiStates % relevantStates, storm::utility::one<ValueType>());
                                    if (!statesWithProbabilityGreater0NonPsi.empty()) {
                                        // Find the maximal rate of all 'maybe' states to take it as the uniformization rate.
                                        ValueType uniformizationRate = storm::utility::zero<ValueType>();
                                        for (auto const& state : statesWithProbabilityGreater0NonPsi) {
                                            uniformizationRate = std::max(uniformizationRate, exitRates[state]);
                                        }
                                        uniformizationRate *= 1.02;
                                        STORM_LOG_THROW(uniformizationRate > 0, storm::exceptions::InvalidStateException, "The uniformization rate must be positive.");
                                        
                                        // Compute the (first) uniformized matrix.
                                        storm::storage::SparseMatrix<ValueType> uniformizedMatrix = computeUniformizedMatrix(rateMatrix, statesWithProbabilityGreater0NonPsi, uniformizationRate, exitRates);
                                        
                                        // Compute the vector that is to be added as a compensation for removing the absorbing states.
                                        std::vector<ValueType> b = rateMatrix.getConstrainedRowSumVector(statesWithProbabilityGreater0NonPsi, psiStates);
                                        for (auto& element : b) {
                                            element /= uniformizationRate;
                                        }
                                        
                                        // Start by computing the transient probabilities of reaching a psi state in time t' - t.
                                        std::vector<ValueType> values(statesWithProbabilityGreater0NonPsi.getNumberOfSetBits(), storm::utility::zero<ValueType>());
                                        // divide the possible error by two since we will make this error two times.
                                        std::vector<ValueType> subresult = computeTransientProbabilities(env, uniformizedMatrix, &b, upperBound - lowerBound, uniformizationRate, values, epsilon / storm::utility::convertNumber<ValueType>(2.0));
                                        storm::utility::vector::setVectorValues(newSubresult, statesWithProbabilityGreater0NonPsi % relevantStates, subresult);
                                    }
                                    
                                    // Then compute the transient probabilities of being in such a state after t time units. For this,
                                    // we must re-uniformize the CTMC, so we need to compute the second uniformized matrix.
                                    ValueType uniformizationRate = storm::utility::zero<ValueType>();
                                    for (auto const& state : relevantStates) {
                                        uniformizationRate = std::max(uniformizationRate, exitRates[state]);
                                    }
                                    uniformizationRate *= 1.02;
                                    STORM_LOG_THROW(uniformizationRate > 0, storm::exceptions::InvalidStateException, "The uniformization rate must be positive.");
                                    
                                    // Finally, we compute the second set of transient probabilities.
                                    storm::storage::SparseMatrix<ValueType> uniformizedMatrix = computeUniformizedMatrix(rateMatrix, relevantStates, uniformizationRate, exitRates);
                                    newSubresult = computeTransientProbabilities<ValueType>(env, uniformizedMatrix, nullptr, lowerBound, uniformizationRate, newSubresult, epsilon / storm::utility::convertNumber<ValueType>(2.0));
                                    
                                    // Fill in the correct values.
                                    result = std::vector<ValueType>(numberOfStates, storm::utility::zero<ValueType>());
                                    storm::utility::vector::setVectorValues(result, ~relevantStates, storm::utility::zero<ValueType>());
                                    storm::utility::vector::setVectorValues(result, relevantStates, newSubresult);
                                } else {
                                    // In this case, the interval is of the form [t, t] with t != 0, t != inf.
                                    
                                    std::vector<ValueType> newSubresult = std::vector<ValueType>(statesWithProbabilityGreater0.getNumberOfSetBits());
                                    storm::utility::vector::setVectorValues(newSubresult, psiStates % statesWithProbabilityGreater0, storm::utility::one<ValueType>());
                                    
                                    // Then compute the transient probabilities of being in such a state after t time units. For this,
                                    // we must re-uniformize the CTMC, so we need to compute the second uniformized matrix.
                                    ValueType uniformizationRate = storm::utility::zero<ValueType>();
                                    for (auto const& state : statesWithProbabilityGreater0) {
                                        uniformizationRate = std::max(uniformizationRate, exitRates[state]);
                                    }
                                    uniformizationRate *= 1.02;
                                    STORM_LOG_THROW(uniformizationRate > 0, storm::exceptions::InvalidStateException, "The uniformization rate must be positive.");
                                    
                                    // Finally, we compute the second set of transient probabilities.
                                    storm::storage::SparseMatrix<ValueType> uniformizedMatrix = computeUniformizedMatrix(rateMatrix, statesWithProbabilityGreater0, uniformizationRate, exitRates);
                                    newSubresult = computeTransientProbabilities<ValueType>(env, uniformizedMatrix, nullptr, lowerBound, uniformizationRate, newSubresult, epsilon);
                                    
                                    // Fill in the correct values.
                                    result = std::vector<ValueType>(numberOfStates, storm::utility::zero<ValueType>());
                                    storm::utility::vector::setVectorValues(result, ~statesWithProbabilityGreater0, storm::utility::zero<ValueType>());
                                    storm::utility::vector::setVectorValues(result, statesWithProbabilityGreater0, newSubresult);
                                }
                            }
                        }
                    } else {
                        result = std::vector<ValueType>(numberOfStates, storm::utility::zero<ValueType>());
                    }
                } while (checkAndUpdateTransientProbabilityEpsilon(env, epsilon, result, relevantValues));
                return result;
            }
            
            template <typename ValueType, typename std::enable_if<!storm::NumberTraits<ValueType>::SupportsExponential, int>::type>
            std::vector<ValueType> SparseCtmcCslHelper::computeBoundedUntilProbabilities(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal, storm::storage::SparseMatrix<ValueType> const&, storm::storage::SparseMatrix<ValueType> const&, storm::storage::BitVector const&, storm::storage::BitVector const&, std::vector<ValueType> const&, bool, double, double) {
                STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Computing bounded until probabilities is unsupported for this value type.");
            }

            template <typename ValueType>
            std::vector<ValueType> SparseCtmcCslHelper::computeUntilProbabilities(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal, storm::storage::SparseMatrix<ValueType> const& rateMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative) {
                return SparseDtmcPrctlHelper<ValueType>::computeUntilProbabilities(env, std::move(goal), computeProbabilityMatrix(rateMatrix, exitRateVector), backwardTransitions, phiStates, psiStates, qualitative);
            }

            template <typename ValueType>
            std::vector<ValueType> SparseCtmcCslHelper::computeAllUntilProbabilities(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal, storm::storage::SparseMatrix<ValueType> const& rateMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& initialStates, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) {
                return SparseDtmcPrctlHelper<ValueType>::computeAllUntilProbabilities(env, std::move(goal), computeProbabilityMatrix(rateMatrix, exitRateVector), initialStates, phiStates, psiStates);
            }

            template <typename ValueType>
            std::vector<ValueType> SparseCtmcCslHelper::computeNextProbabilities(Environment const& env, storm::storage::SparseMatrix<ValueType> const& rateMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& nextStates) {
                return SparseDtmcPrctlHelper<ValueType>::computeNextProbabilities(env, computeProbabilityMatrix(rateMatrix, exitRateVector), nextStates);
            }
            
            template <typename ValueType, typename RewardModelType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type>
            std::vector<ValueType> SparseCtmcCslHelper::computeInstantaneousRewards(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal, storm::storage::SparseMatrix<ValueType> const& rateMatrix, std::vector<ValueType> const& exitRateVector, RewardModelType const& rewardModel, double timeBound) {
                // Only compute the result if the model has a state-based reward model.
                STORM_LOG_THROW(!rewardModel.empty(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
                
                uint_fast64_t numberOfStates = rateMatrix.getRowCount();
                
                // Initialize result to state rewards of the this->getModel().
                std::vector<ValueType> result(rewardModel.getStateRewardVector());
                
                // If the time-bound is zero, just return the current reward vector
                if (storm::utility::isZero(timeBound)) {
                    return result;
                }
                ValueType maxValue = storm::utility::vector::maximumElementAbs(result);
                
                // If all entries are zero, we return the zero-vector
                if (storm::utility::isZero(maxValue)) {
                    return result;
                }
            
                ValueType uniformizationRate = 0;
                for (auto const& rate : exitRateVector) {
                    uniformizationRate = std::max(uniformizationRate, rate);
                }
                uniformizationRate *= 1.02;
                STORM_LOG_THROW(uniformizationRate > 0, storm::exceptions::InvalidStateException, "The uniformization rate must be positive.");
                
                storm::storage::SparseMatrix<ValueType> uniformizedMatrix = computeUniformizedMatrix(rateMatrix, storm::storage::BitVector(numberOfStates, true), uniformizationRate, exitRateVector);

                // Set the possible error allowed for truncation (epsilon for fox-glynn)
                ValueType epsilon = storm::utility::convertNumber<ValueType>(env.solver().timeBounded().getPrecision());
                if (env.solver().timeBounded().getRelativeTerminationCriterion()) {
                    // Be more precise, if the maximum value is very small (precision can/has to be refined later)
                    epsilon *= std::min(storm::utility::one<ValueType>(), maxValue);
                } else {
                    // Be more precise, if the maximal possible value is very large
                    epsilon /= std::max(storm::utility::one<ValueType>(), maxValue);
                }
                
                storm::storage::BitVector relevantValues;
                if (goal.hasRelevantValues()) {
                    relevantValues = std::move(goal.relevantValues());
                } else {
                    relevantValues = storm::storage::BitVector(result.size(), true);
                }
                
                // Loop until the desired precision is reached.
                do {
                    result = computeTransientProbabilities<ValueType>(env, uniformizedMatrix, nullptr, timeBound, uniformizationRate, result, epsilon);
                } while (checkAndUpdateTransientProbabilityEpsilon(env, epsilon, result, relevantValues));
            
                return result;
            }
            
            template <typename ValueType, typename RewardModelType, typename std::enable_if<!storm::NumberTraits<ValueType>::SupportsExponential, int>::type>
            std::vector<ValueType> SparseCtmcCslHelper::computeInstantaneousRewards(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal, storm::storage::SparseMatrix<ValueType> const&, std::vector<ValueType> const&, RewardModelType const&, double) {
                STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Computing instantaneous rewards is unsupported for this value type.");
            }
            
            template <typename ValueType, typename RewardModelType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type>
            std::vector<ValueType> SparseCtmcCslHelper::computeCumulativeRewards(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal, storm::storage::SparseMatrix<ValueType> const& rateMatrix, std::vector<ValueType> const& exitRateVector, RewardModelType const& rewardModel, double timeBound) {
                STORM_LOG_THROW(!env.solver().isForceExact(), storm::exceptions::InvalidOperationException, "Exact computations not possible for cumulative expected rewards.");

                // Only compute the result if the model has a state-based reward model.
                STORM_LOG_THROW(!rewardModel.empty(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
                
                uint_fast64_t numberOfStates = rateMatrix.getRowCount();
                
                // If the time bound is zero, the result is the constant zero vector.
                if (timeBound == 0) {
                    return std::vector<ValueType>(numberOfStates, storm::utility::zero<ValueType>());
                }
                
                // Otherwise, we need to perform some computations.
                
                // Start with the uniformization.
                ValueType uniformizationRate = 0;
                for (auto const& rate : exitRateVector) {
                    uniformizationRate = std::max(uniformizationRate, rate);
                }
                uniformizationRate *= 1.02;
                STORM_LOG_THROW(uniformizationRate > 0, storm::exceptions::InvalidStateException, "The uniformization rate must be positive.");
                
                storm::storage::SparseMatrix<ValueType> uniformizedMatrix = computeUniformizedMatrix(rateMatrix, storm::storage::BitVector(numberOfStates, true), uniformizationRate, exitRateVector);
                
                // Compute the total state reward vector.
                std::vector<ValueType> totalRewardVector = rewardModel.getTotalRewardVector(rateMatrix, exitRateVector);
                ValueType maxReward = storm::utility::vector::maximumElementAbs(totalRewardVector);
                
                // If all rewards are zero, the result is the constant zero vector.
                if (storm::utility::isZero(maxReward)) {
                    return std::vector<ValueType>(numberOfStates, storm::utility::zero<ValueType>());
                }
                
                // Set the possible (absolute) error allowed for truncation (epsilon for fox-glynn)
                ValueType epsilon = storm::utility::convertNumber<ValueType>(env.solver().timeBounded().getPrecision());
                if (env.solver().timeBounded().getRelativeTerminationCriterion()) {
                    // Be more precise, if the value is very small (precision can/has to be refined later)
                    epsilon *= std::min(storm::utility::one<ValueType>(), maxReward);
                } else {
                    // Be more precise, if the maximal possible value is very large
                    epsilon /= std::max(storm::utility::one<ValueType>(), maxReward * timeBound);
                }
                
                storm::storage::BitVector relevantValues;
                if (goal.hasRelevantValues()) {
                    relevantValues = std::move(goal.relevantValues());
                } else {
                    relevantValues = storm::storage::BitVector(totalRewardVector.size(), true);
                }
                
                // Loop until the desired precision is reached.
                std::vector<ValueType> result;
                do {
                    result = computeTransientProbabilities<ValueType, true>(env, uniformizedMatrix, nullptr, timeBound, uniformizationRate, totalRewardVector, epsilon);
                } while (checkAndUpdateTransientProbabilityEpsilon(env, epsilon, result, relevantValues));
    
                return result;
            }
            
            template <typename ValueType, typename RewardModelType, typename std::enable_if<!storm::NumberTraits<ValueType>::SupportsExponential, int>::type>
            std::vector<ValueType> SparseCtmcCslHelper::computeCumulativeRewards(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal, storm::storage::SparseMatrix<ValueType> const&, std::vector<ValueType> const&, RewardModelType const&, double) {
                STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Computing cumulative rewards is unsupported for this value type.");
            }
            
            template <typename ValueType>
            std::vector<ValueType> SparseCtmcCslHelper::computeReachabilityTimes(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal, storm::storage::SparseMatrix<ValueType> const& rateMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& targetStates, bool qualitative) {
                // Compute expected time on CTMC by reduction to DTMC with rewards.
                storm::storage::SparseMatrix<ValueType> probabilityMatrix = computeProbabilityMatrix(rateMatrix, exitRateVector);
                
                // Initialize rewards.
                std::vector<ValueType> totalRewardVector;
                for (size_t i = 0; i < exitRateVector.size(); ++i) {
                    if (targetStates[i] || storm::utility::isZero(exitRateVector[i])) {
                        // Set reward for target states or states without outgoing transitions to 0.
                        totalRewardVector.push_back(storm::utility::zero<ValueType>());
                    } else {
                        // Reward is (1 / exitRate).
                        totalRewardVector.push_back(storm::utility::one<ValueType>() / exitRateVector[i]);
                    }
                }
                
                return storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeReachabilityRewards(env, std::move(goal), probabilityMatrix, backwardTransitions, totalRewardVector, targetStates, qualitative);
            }

            template <typename ValueType, typename RewardModelType>
            std::vector<ValueType> SparseCtmcCslHelper::computeReachabilityRewards(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal, storm::storage::SparseMatrix<ValueType> const& rateMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, RewardModelType const& rewardModel, storm::storage::BitVector const& targetStates, bool qualitative) {
                STORM_LOG_THROW(!rewardModel.empty(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
                
                storm::storage::SparseMatrix<ValueType> probabilityMatrix = computeProbabilityMatrix(rateMatrix, exitRateVector);
                
                std::vector<ValueType> totalRewardVector;
                if (rewardModel.hasStateRewards()) {
                    totalRewardVector = rewardModel.getStateRewardVector();
                    typename std::vector<ValueType>::const_iterator it2 = exitRateVector.begin();
                    for (typename std::vector<ValueType>::iterator it1 = totalRewardVector.begin(), ite1 = totalRewardVector.end(); it1 != ite1; ++it1, ++it2) {
                        *it1 /= *it2;
                    }
                    if (rewardModel.hasStateActionRewards()) {
                        storm::utility::vector::addVectors(totalRewardVector, rewardModel.getStateActionRewardVector(), totalRewardVector);
                    }
                    if (rewardModel.hasTransitionRewards()) {
                        storm::utility::vector::addVectors(totalRewardVector, probabilityMatrix.getPointwiseProductRowSumVector(rewardModel.getTransitionRewardMatrix()), totalRewardVector);
                    }
                } else if (rewardModel.hasTransitionRewards()) {
                    totalRewardVector = probabilityMatrix.getPointwiseProductRowSumVector(rewardModel.getTransitionRewardMatrix());
                    if (rewardModel.hasStateActionRewards()) {
                        storm::utility::vector::addVectors(totalRewardVector, rewardModel.getStateActionRewardVector(), totalRewardVector);
                    }
                } else {
                    totalRewardVector = rewardModel.getStateActionRewardVector();
                }
                
                return storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeReachabilityRewards(env, std::move(goal), probabilityMatrix, backwardTransitions, totalRewardVector, targetStates, qualitative);
            }
            
            template <typename ValueType, typename RewardModelType>
            std::vector<ValueType> SparseCtmcCslHelper::computeTotalRewards(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal, storm::storage::SparseMatrix<ValueType> const& rateMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, RewardModelType const& rewardModel, bool qualitative) {
                STORM_LOG_THROW(!rewardModel.empty(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
                
                storm::storage::SparseMatrix<ValueType> probabilityMatrix = computeProbabilityMatrix(rateMatrix, exitRateVector);
                
                std::vector<ValueType> totalRewardVector;
                if (rewardModel.hasStateRewards()) {
                    totalRewardVector = rewardModel.getStateRewardVector();
                    typename std::vector<ValueType>::const_iterator it2 = exitRateVector.begin();
                    for (typename std::vector<ValueType>::iterator it1 = totalRewardVector.begin(), ite1 = totalRewardVector.end(); it1 != ite1; ++it1, ++it2) {
                        *it1 /= *it2;
                    }
                    if (rewardModel.hasStateActionRewards()) {
                        storm::utility::vector::addVectors(totalRewardVector, rewardModel.getStateActionRewardVector(), totalRewardVector);
                    }
                    if (rewardModel.hasTransitionRewards()) {
                        storm::utility::vector::addVectors(totalRewardVector, probabilityMatrix.getPointwiseProductRowSumVector(rewardModel.getTransitionRewardMatrix()), totalRewardVector);
                    }
                } else if (rewardModel.hasTransitionRewards()) {
                    totalRewardVector = probabilityMatrix.getPointwiseProductRowSumVector(rewardModel.getTransitionRewardMatrix());
                    if (rewardModel.hasStateActionRewards()) {
                        storm::utility::vector::addVectors(totalRewardVector, rewardModel.getStateActionRewardVector(), totalRewardVector);
                    }
                } else {
                    totalRewardVector = rewardModel.getStateActionRewardVector();
                }
                
                RewardModelType dtmcRewardModel(std::move(totalRewardVector));
                return storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeTotalRewards(env, std::move(goal), probabilityMatrix, backwardTransitions, dtmcRewardModel, qualitative);
            }
            
            template <typename ValueType>
            std::vector<ValueType> SparseCtmcCslHelper::computeLongRunAverageProbabilities(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal, storm::storage::SparseMatrix<ValueType> const& rateMatrix, storm::storage::BitVector const& psiStates, std::vector<ValueType> const* exitRateVector) {
                
                // If there are no goal states, we avoid the computation and directly return zero.
                uint_fast64_t numberOfStates = rateMatrix.getRowCount();
                if (psiStates.empty()) {
                    return std::vector<ValueType>(numberOfStates, storm::utility::zero<ValueType>());
                }
                
                // Likewise, if all bits are set, we can avoid the computation.
                if (psiStates.full()) {
                    return std::vector<ValueType>(numberOfStates, storm::utility::one<ValueType>());
                }
                
                ValueType zero = storm::utility::zero<ValueType>();
                ValueType one = storm::utility::one<ValueType>();
                
                return computeLongRunAverages<ValueType>(env, std::move(goal), rateMatrix,
                                              [&zero, &one, &psiStates] (storm::storage::sparse::state_type const& state) -> ValueType {
                                                  if (psiStates.get(state)) {
                                                      return one;
                                                  }
                                                  return zero;
                                              },
                                              exitRateVector);
            }
            
            template <typename ValueType, typename RewardModelType>
            std::vector<ValueType> SparseCtmcCslHelper::computeLongRunAverageRewards(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal, storm::storage::SparseMatrix<ValueType> const& rateMatrix, RewardModelType const& rewardModel, std::vector<ValueType> const* exitRateVector) {
                // Only compute the result if the model has a state-based reward model.
                STORM_LOG_THROW(!rewardModel.empty(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");

                return computeLongRunAverages<ValueType>(env, std::move(goal), rateMatrix,
                        [&] (storm::storage::sparse::state_type const& state) -> ValueType {
                            ValueType result = rewardModel.hasStateRewards() ? rewardModel.getStateReward(state) : storm::utility::zero<ValueType>();
                            if (rewardModel.hasStateActionRewards()) {
                                // State action rewards are multiplied with the exit rate r(s). Then, multiplying the reward with the expected time we stay at s (i.e. 1/r(s)) yields the original state reward
                                if (exitRateVector) {
                                    result += rewardModel.getStateActionReward(state) * (*exitRateVector)[state];
                                } else {
                                    result += rewardModel.getStateActionReward(state);
                                }
                            }
                            if (rewardModel.hasTransitionRewards()) {
                                // Transition rewards are already multiplied with the rates
                                result += rateMatrix.getPointwiseProductRowSum(rewardModel.getTransitionRewardMatrix(), state);
                            }
                            return result;
                        },
                        exitRateVector);
            }
            
            template <typename ValueType>
            std::vector<ValueType> SparseCtmcCslHelper::computeLongRunAverageRewards(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal, storm::storage::SparseMatrix<ValueType> const& rateMatrix, std::vector<ValueType> const& stateRewardVector, std::vector<ValueType> const* exitRateVector) {
                return computeLongRunAverages<ValueType>(env, std::move(goal), rateMatrix,
                                                         [&stateRewardVector] (storm::storage::sparse::state_type const& state) -> ValueType {
                                                             return stateRewardVector[state];
                                                         },
                                                         exitRateVector);
            }
            
            template <typename ValueType>
            std::vector<ValueType> SparseCtmcCslHelper::computeLongRunAverages(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal, storm::storage::SparseMatrix<ValueType> const& rateMatrix, std::function<ValueType (storm::storage::sparse::state_type const& state)> const& valueGetter, std::vector<ValueType> const* exitRateVector){
                storm::storage::SparseMatrix<ValueType> probabilityMatrix;
                if (exitRateVector) {
                    probabilityMatrix = computeProbabilityMatrix(rateMatrix, *exitRateVector);
                } else {
                    probabilityMatrix = rateMatrix;
                }
                uint_fast64_t numberOfStates = rateMatrix.getRowCount();
            
                // Start by decomposing the CTMC into its BSCCs.
                storm::storage::StronglyConnectedComponentDecomposition<ValueType> bsccDecomposition(rateMatrix, storm::storage::StronglyConnectedComponentDecompositionOptions().onlyBottomSccs());
                
                STORM_LOG_DEBUG("Found " << bsccDecomposition.size() << " BSCCs.");

                // Prepare the vector holding the LRA values for each of the BSCCs.
                std::vector<ValueType> bsccLra;
                bsccLra.reserve(bsccDecomposition.size());
                
                auto underlyingSolverEnvironment = env;
                auto precision = env.solver().lra().getPrecision();
                if (env.solver().isForceSoundness()) {
                    // For sound computations, the error in the MECS plus the error in the remaining system should be less then the user defined precision.
                    precision /= storm::utility::convertNumber<storm::RationalNumber>(2);
                    underlyingSolverEnvironment.solver().lra().setPrecision(precision);
                }
                underlyingSolverEnvironment.solver().setLinearEquationSolverPrecision(precision, env.solver().lra().getRelativeTerminationCriterion());
                
                // Keep track of the maximal and minimal value occuring in one of the BSCCs
                ValueType maxValue, minValue;
                storm::storage::BitVector statesInBsccs(numberOfStates);
                for (auto const& bscc : bsccDecomposition) {
                    for (auto const& state : bscc) {
                        statesInBsccs.set(state);
                    }
                    bsccLra.push_back(computeLongRunAveragesForBscc<ValueType>(underlyingSolverEnvironment, bscc, rateMatrix, valueGetter, exitRateVector));
                    if (bsccLra.size() == 1) {
                        maxValue = bsccLra.back();
                        minValue = bsccLra.back();
                    } else {
                        maxValue = std::max(bsccLra.back(), maxValue);
                        minValue = std::min(bsccLra.back(), minValue);
                    }
                }
                
                storm::storage::BitVector statesNotInBsccs = ~statesInBsccs;
                STORM_LOG_DEBUG("Found " << statesInBsccs.getNumberOfSetBits() << " states in BSCCs.");
                
                std::vector<uint64_t> stateToBsccMap(statesInBsccs.size(), -1);
                for (uint64_t bsccIndex = 0; bsccIndex < bsccDecomposition.size(); ++bsccIndex) {
                    for (auto const& state : bsccDecomposition[bsccIndex]) {
                        stateToBsccMap[state] = bsccIndex;
                    }
                }
                
                std::vector<ValueType> rewardSolution;
                if (!statesNotInBsccs.empty()) {
                    // Calculate LRA for states not in bsccs as expected reachability rewards.
                    // Target states are states in bsccs, transition reward is the lra of the bscc for each transition into a
                    // bscc and 0 otherwise. This corresponds to the sum of LRAs in BSCC weighted by the reachability probability
                    // of the BSCC.
                    
                    std::vector<ValueType> rewardRightSide;
                    rewardRightSide.reserve(statesNotInBsccs.getNumberOfSetBits());
                    
                    for (auto state : statesNotInBsccs) {
                        ValueType reward = storm::utility::zero<ValueType>();
                        for (auto entry : rateMatrix.getRow(state)) {
                            if (statesInBsccs.get(entry.getColumn())) {
                                if (exitRateVector) {
                                    reward += (entry.getValue() / (*exitRateVector)[state]) * bsccLra[stateToBsccMap[entry.getColumn()]];
                                } else {
                                    reward += entry.getValue() * bsccLra[stateToBsccMap[entry.getColumn()]];
                                }
                            }
                        }
                        rewardRightSide.push_back(reward);
                    }
                    
                    // Compute reachability rewards
                    storm::solver::GeneralLinearEquationSolverFactory<ValueType> linearEquationSolverFactory;
                    bool isEqSysFormat = linearEquationSolverFactory.getEquationProblemFormat(underlyingSolverEnvironment) == storm::solver::LinearEquationSolverProblemFormat::EquationSystem;
                    storm::storage::SparseMatrix<ValueType> rewardEquationSystemMatrix = rateMatrix.getSubmatrix(false, statesNotInBsccs, statesNotInBsccs, isEqSysFormat);
                    if (exitRateVector) {
                        uint64_t localRow = 0;
                        for (auto const& globalRow : statesNotInBsccs) {
                            for (auto& entry : rewardEquationSystemMatrix.getRow(localRow)) {
                                entry.setValue(entry.getValue() / (*exitRateVector)[globalRow]);
                            }
                            ++localRow;
                        }
                    }
                    if (isEqSysFormat) {
                        rewardEquationSystemMatrix.convertToEquationSystem();
                    }
                    rewardSolution = std::vector<ValueType>(rewardEquationSystemMatrix.getColumnCount(), (maxValue + minValue) / storm::utility::convertNumber<ValueType,uint64_t>(2));
                    std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = linearEquationSolverFactory.create(underlyingSolverEnvironment, std::move(rewardEquationSystemMatrix));
                    solver->setBounds(minValue, maxValue);
                    // Check solver requirements
                    auto requirements = solver->getRequirements(underlyingSolverEnvironment);
                    STORM_LOG_THROW(!requirements.hasEnabledCriticalRequirement(), storm::exceptions::UncheckedRequirementException, "Solver requirements " + requirements.getEnabledRequirementsAsString() + " not checked.");
                    solver->solveEquations(underlyingSolverEnvironment, rewardSolution, rewardRightSide);
                }
                
                // Fill the result vector.
                std::vector<ValueType> result(numberOfStates);
                auto rewardSolutionIter = rewardSolution.begin();
                
                for (uint_fast64_t bsccIndex = 0; bsccIndex < bsccDecomposition.size(); ++bsccIndex) {
                    storm::storage::StronglyConnectedComponent const& bscc = bsccDecomposition[bsccIndex];
                    
                    for (auto const& state : bscc) {
                        result[state] = bsccLra[bsccIndex];
                    }
                }
                for (auto state : statesNotInBsccs) {
                    STORM_LOG_ASSERT(rewardSolutionIter != rewardSolution.end(), "Too few elements in solution.");
                    // Take the value from the reward computation. Since the n-th state not in any bscc is the n-th
                    // entry in rewardSolution we can just take the next value from the iterator.
                    result[state] = *rewardSolutionIter;
                    ++rewardSolutionIter;
                }
                
                return result;
            }

            template <typename ValueType>
            ValueType SparseCtmcCslHelper::computeLongRunAveragesForBscc(Environment const& env, storm::storage::StronglyConnectedComponent const& bscc, storm::storage::SparseMatrix<ValueType> const& rateMatrix, std::function<ValueType (storm::storage::sparse::state_type const& state)> const& valueGetter, std::vector<ValueType> const* exitRateVector) {

                // Catch the case where all values are the same (this includes the special case where the bscc is of size 1)
                auto it = bscc.begin();
                ValueType val = valueGetter(*it);
                for (++it; it != bscc.end(); ++it) {
                    if (valueGetter(*it) != val) {
                        break;
                    }
                }
                if (it == bscc.end()) {
                    // All entries have the same LRA
                    return val;
                }
                
                storm::solver::LraMethod method = env.solver().lra().getDetLraMethod();
                if ((storm::NumberTraits<ValueType>::IsExact || env.solver().isForceExact()) && env.solver().lra().isDetLraMethodSetFromDefault() && method == storm::solver::LraMethod::ValueIteration) {
                    method = storm::solver::LraMethod::GainBiasEquations;
                    STORM_LOG_INFO("Selecting " << storm::solver::toString(method) << " as the solution technique for long-run properties to guarantee exact results. If you want to override this, please explicitly specify a different LRA method.");
                } else if (env.solver().isForceSoundness() && env.solver().lra().isDetLraMethodSetFromDefault() && method != storm::solver::LraMethod::ValueIteration) {
                    method = storm::solver::LraMethod::ValueIteration;
                    STORM_LOG_INFO("Selecting " << storm::solver::toString(method) << " as the solution technique for long-run properties to guarantee sound results. If you want to override this, please explicitly specify a different LRA method.");
                }
                STORM_LOG_TRACE("Computing LRA for BSCC of size " << bscc.size() << " using '" << storm::solver::toString(method) << "'.");
                if (method == storm::solver::LraMethod::ValueIteration) {
                    return computeLongRunAveragesForBsccVi<ValueType>(env, bscc, rateMatrix, valueGetter, exitRateVector);
                } else if (method == storm::solver::LraMethod::LraDistributionEquations) {
                    // We only need the first element of the pair as the lra distribution is not relevant at this point.
                    return computeLongRunAveragesForBsccLraDistr<ValueType>(env, bscc, rateMatrix, valueGetter, exitRateVector).first;
                }
                STORM_LOG_WARN_COND(method == storm::solver::LraMethod::GainBiasEquations, "Unsupported lra method selected. Defaulting to " << storm::solver::toString(storm::solver::LraMethod::GainBiasEquations) << ".");
                // We don't need the bias values
                return computeLongRunAveragesForBsccGainBias<ValueType>(env, bscc, rateMatrix, valueGetter, exitRateVector).first;
            }
            
            template <>
            storm::RationalFunction SparseCtmcCslHelper::computeLongRunAveragesForBsccVi<storm::RationalFunction>(Environment const&, storm::storage::StronglyConnectedComponent const&, storm::storage::SparseMatrix<storm::RationalFunction> const&, std::function<storm::RationalFunction (storm::storage::sparse::state_type const& state)> const&, std::vector<storm::RationalFunction> const*) {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The requested Method for LRA computation is not supported for parametric models.");
            }
                
            template <typename ValueType>
            ValueType SparseCtmcCslHelper::computeLongRunAveragesForBsccVi(Environment const& env, storm::storage::StronglyConnectedComponent const& bscc, storm::storage::SparseMatrix<ValueType> const& rateMatrix, std::function<ValueType (storm::storage::sparse::state_type const& state)> const& valueGetter, std::vector<ValueType> const* exitRateVector) {
                
                // Initialize data about the bscc
                storm::storage::BitVector bsccStates(rateMatrix.getRowGroupCount(), false);
                for (auto const& state : bscc) {
                    bsccStates.set(state);
                }
                
                // Get the uniformization rate
                ValueType uniformizationRate = storm::utility::one<ValueType>();
                if (exitRateVector) {
                    uniformizationRate = storm::utility::vector::max_if(*exitRateVector, bsccStates);
                }
                // To ensure that the model is aperiodic, we need to make sure that every Markovian state gets a self loop.
                // Hence, we increase the uniformization rate a little.
                uniformizationRate *= (storm::utility::one<ValueType>() + storm::utility::convertNumber<ValueType>(env.solver().lra().getAperiodicFactor()));

                // Get the transitions of the submodel
                typename storm::storage::SparseMatrix<ValueType> bsccMatrix = rateMatrix.getSubmatrix(true, bsccStates, bsccStates, true);
                
                // Uniformize the transitions
                uint64_t subState = 0;
                for (auto state : bsccStates) {
                    for (auto& entry : bsccMatrix.getRow(subState)) {
                        if (entry.getColumn() == subState) {
                            if (exitRateVector) {
                                entry.setValue(storm::utility::one<ValueType>() + (entry.getValue() - (*exitRateVector)[state]) / uniformizationRate);
                            } else {
                                entry.setValue(storm::utility::one<ValueType>() + (entry.getValue() - storm::utility::one<ValueType>()) / uniformizationRate);
                            }
                        } else {
                            entry.setValue(entry.getValue() / uniformizationRate);
                        }
                    }
                    ++subState;
                }

                // Compute the rewards obtained in a single uniformization step
                std::vector<ValueType> markovianRewards;
                markovianRewards.reserve(bsccMatrix.getRowCount());
                for (auto const& state : bsccStates) {
                    markovianRewards.push_back(valueGetter(state) / uniformizationRate);
                }
                
                // start the iterations
                ValueType precision = storm::utility::convertNumber<ValueType>(env.solver().lra().getPrecision()) / uniformizationRate;
                bool relative = env.solver().lra().getRelativeTerminationCriterion();
                if (!relative) {
                    precision /= uniformizationRate;
                }
                std::vector<ValueType> x(bsccMatrix.getRowCount(), storm::utility::zero<ValueType>());
                std::vector<ValueType> xPrime(x.size());
                
                auto multiplier = storm::solver::MultiplierFactory<ValueType>().create(env, bsccMatrix);
                ValueType maxDiff, minDiff;
                uint64_t iter = 0;
                boost::optional<uint64_t> maxIter;
                if (env.solver().lra().isMaximalIterationCountSet()) {
                    maxIter = env.solver().lra().getMaximalIterationCount();
                }
                while (!maxIter.is_initialized() || iter < maxIter.get()) {
                    ++iter;
                    // Compute the values for the markovian states. We also keep track of the maximal and minimal difference between two values (for convergence checking)
                    multiplier->multiply(env, x, &markovianRewards, xPrime);
                    
                    // update xPrime and check for convergence
                    // to avoid large (and numerically unstable) x-values, we substract a reference value.
                    auto xIt = x.begin();
                    auto xPrimeIt = xPrime.begin();
                    ValueType refVal = *xPrimeIt;
                    maxDiff = *xPrimeIt - *xIt;
                    minDiff = maxDiff;
                    *xPrimeIt -= refVal;
                    *xIt = *xPrimeIt;
                    for (++xIt, ++xPrimeIt; xIt != x.end(); ++xIt, ++xPrimeIt) {
                        ValueType diff = *xPrimeIt - *xIt;
                        maxDiff = std::max(maxDiff, diff);
                        minDiff = std::min(minDiff, diff);
                        *xPrimeIt -= refVal;
                        *xIt = *xPrimeIt;
                    }
                    
                    // Check for convergence. The uniformization rate is already incorporated into the precision parameter
                    if ((maxDiff - minDiff) <= (relative ? (precision * minDiff) : precision)) {
                        break;
                    }
                    if (storm::utility::resources::isTerminate()) {
                        break;
                    }
                }
                if (maxIter.is_initialized() && iter == maxIter.get()) {
                    STORM_LOG_WARN("LRA computation did not converge within " << iter << " iterations.");
                } else {
                    STORM_LOG_TRACE("LRA computation converged after " << iter << " iterations.");
                }
                return (maxDiff + minDiff) * uniformizationRate / (storm::utility::convertNumber<ValueType>(2.0));
            }
            
            template <typename ValueType>
            std::pair<ValueType, std::vector<ValueType>> SparseCtmcCslHelper::computeLongRunAveragesForBsccGainBias(Environment const& env, storm::storage::StronglyConnectedComponent const& bscc, storm::storage::SparseMatrix<ValueType> const& rateMatrix, std::function<ValueType (storm::storage::sparse::state_type const& state)> const& valueGetter, std::vector<ValueType> const* exitRateVector) {
                // We build the equation system as in Line 3 of Algorithm 3 from
                // Kretinsky, Meggendorfer: Efficient Strategy Iteration for Mean Payoff in Markov Decision Processes (ATVA 2017)
                // The first variable corresponds to the gain of the bscc whereas the subsequent variables yield the bias for each state s_1, s_2, ....
                // No bias variable for s_0 is needed since it is always set to zero, yielding an nxn equation system matrix
                // To make this work for CTMC, we could uniformize the model. This preserves LRA and ensures that we can compute the
                // LRA as for a DTMC (the soujourn time in each state is the same). If we then multiply the equations with the uniformization rate,
                // the uniformization rate cancels out. Hence, we obtain the equation system below.
                
                // Get a mapping from global state indices to local ones.
                std::unordered_map<uint64_t, uint64_t> toLocalIndexMap;
                uint64_t localIndex = 0;
                for (auto const& globalIndex : bscc) {
                    toLocalIndexMap[globalIndex] = localIndex;
                    ++localIndex;
                }
                
                // Prepare an environment for the underlying equation solver
                auto subEnv = env;
                if (subEnv.solver().getLinearEquationSolverType() == storm::solver::EquationSolverType::Topological) {
                    // Topological solver does not make any sense since the BSCC is connected.
                    subEnv.solver().setLinearEquationSolverType(subEnv.solver().topological().getUnderlyingEquationSolverType(), subEnv.solver().topological().isUnderlyingEquationSolverTypeSetFromDefault());
                }
                subEnv.solver().setLinearEquationSolverPrecision(env.solver().lra().getPrecision(), env.solver().lra().getRelativeTerminationCriterion());
                
                // Build the equation system matrix and vector.
                storm::solver::GeneralLinearEquationSolverFactory<ValueType> linearEquationSolverFactory;
                bool isEquationSystemFormat = linearEquationSolverFactory.getEquationProblemFormat(subEnv) == storm::solver::LinearEquationSolverProblemFormat::EquationSystem;
                storm::storage::SparseMatrixBuilder<ValueType> builder(bscc.size(), bscc.size());
                std::vector<ValueType> eqSysVector;
                eqSysVector.reserve(bscc.size());
                // The first row asserts that the weighted bias variables and the reward at s_0 sum up to the gain
                uint64_t row = 0;
                ValueType entryValue;
                for (auto const& globalState : bscc) {
                    // Coefficient for the gain variable
                    if (isEquationSystemFormat) {
                        // '1-0' in row 0 and -(-1) in other rows
                        builder.addNextValue(row, 0, storm::utility::one<ValueType>());
                    } else if (row > 0) {
                        // No coeficient in row 0, othwerise substract the gain
                        builder.addNextValue(row, 0, -storm::utility::one<ValueType>());
                    }
                    // Compute weighted sum over successor state. As this is a BSCC, each successor state will again be in the BSCC.
                    auto diagonalValue = storm::utility::zero<ValueType>();
                    if (row > 0) {
                        if (isEquationSystemFormat) {
                            diagonalValue = exitRateVector ? (*exitRateVector)[globalState] : storm::utility::one<ValueType>();
                        } else {
                            diagonalValue = storm::utility::one<ValueType>() - (exitRateVector ? (*exitRateVector)[globalState] : storm::utility::one<ValueType>());
                        }
                    }
                    bool needDiagonalEntry = !storm::utility::isZero(diagonalValue);
                    for (auto const& entry : rateMatrix.getRow(globalState)) {
                        uint64_t col = toLocalIndexMap[entry.getColumn()];
                        if (col == 0) {
                            //Skip transition to state_0. This corresponds to setting the bias of state_0 to zero
                            continue;
                        }
                        entryValue = entry.getValue();
                        if (isEquationSystemFormat) {
                            entryValue = -entryValue;
                        }
                        if (needDiagonalEntry && col >= row) {
                            if (col == row) {
                                entryValue += diagonalValue;
                            } else { // col > row
                                builder.addNextValue(row, row, diagonalValue);
                            }
                            needDiagonalEntry = false;
                        }
                        builder.addNextValue(row, col, entryValue);
                    }
                    if (needDiagonalEntry) {
                        builder.addNextValue(row, row, diagonalValue);
                    }
                    eqSysVector.push_back(valueGetter(globalState));
                    ++row;
                }

                // Create a linear equation solver
                auto solver = linearEquationSolverFactory.create(subEnv, builder.build());
                // Check solver requirements.
                auto requirements = solver->getRequirements(subEnv);
                STORM_LOG_THROW(!requirements.hasEnabledCriticalRequirement(), storm::exceptions::UncheckedRequirementException, "Solver requirements " + requirements.getEnabledRequirementsAsString() + " not checked.");
                // Todo: Find bounds on the bias variables. Just inserting the maximal value from the vector probably does not work.
                
                std::vector<ValueType> eqSysSol(bscc.size(), storm::utility::zero<ValueType>());
                // Take the mean of the rewards as an initial guess for the gain
                //eqSysSol.front() = std::accumulate(eqSysVector.begin(), eqSysVector.end(), storm::utility::zero<ValueType>()) / storm::utility::convertNumber<ValueType, uint64_t>(bscc.size());
                solver->solveEquations(subEnv, eqSysSol, eqSysVector);
                
                ValueType gain = eqSysSol.front();
                // insert bias value for state 0
                eqSysSol.front() = storm::utility::zero<ValueType>();
                // Return the gain and the bias values
                return std::pair<ValueType, std::vector<ValueType>>(std::move(gain), std::move(eqSysSol));
            }
            
            template <typename ValueType>
            std::pair<ValueType, std::vector<ValueType>> SparseCtmcCslHelper::computeLongRunAveragesForBsccLraDistr(Environment const& env, storm::storage::StronglyConnectedComponent const& bscc, storm::storage::SparseMatrix<ValueType> const& rateMatrix, std::function<ValueType (storm::storage::sparse::state_type const& state)> const& valueGetter, std::vector<ValueType> const* exitRateVector) {

                // Let A be ab auxiliary Matrix with A[s,s] =  R(s,s) - r(s) & A[s,s'] = R(s,s') for s,s' in BSCC and s!=s'.
                // We build and solve the equation system for
                // x*A=0 &  x_0+...+x_n=1  <=>  A^t*x=0=x-x & x_0+...+x_n=1  <=> (1+A^t)*x = x & 1-x_0-...-x_n-1=x_n
                // Then, x[i] will be the fraction of the time we are in state i.
                
                // This method assumes that this BSCC consist of more than one state
                if (bscc.size() == 1) {
                    return { valueGetter(*bscc.begin()), {storm::utility::one<ValueType>()} };
                }
                
                // Prepare an environment for the underlying linear equation solver
                auto subEnv = env;
                if (subEnv.solver().getLinearEquationSolverType() == storm::solver::EquationSolverType::Topological) {
                    // Topological solver does not make any sense since the BSCC is connected.
                    subEnv.solver().setLinearEquationSolverType(subEnv.solver().topological().getUnderlyingEquationSolverType(), subEnv.solver().topological().isUnderlyingEquationSolverTypeSetFromDefault());
                }
                subEnv.solver().setLinearEquationSolverPrecision(env.solver().lra().getPrecision(), env.solver().lra().getRelativeTerminationCriterion());
                
                // Get a mapping from global state indices to local ones as well as a bitvector containing states within the BSCC.
                std::unordered_map<uint64_t, uint64_t> toLocalIndexMap;
                storm::storage::BitVector bsccStates(rateMatrix.getRowCount(), false);
                uint64_t localIndex = 0;
                for (auto const& globalIndex : bscc) {
                    bsccStates.set(globalIndex, true);
                    toLocalIndexMap[globalIndex] = localIndex;
                    ++localIndex;
                }
                
                // Build the auxiliary Matrix A.
                auto auxMatrix = rateMatrix.getSubmatrix(false, bsccStates, bsccStates, true); // add diagonal entries!
                uint64_t row = 0;
                for (auto const& globalIndex : bscc) {
                    for (auto& entry : auxMatrix.getRow(row)) {
                        if (entry.getColumn() == row) {
                            // This value is non-zero since we have a BSCC with more than one state
                            if (exitRateVector) {
                                entry.setValue(entry.getValue() - (*exitRateVector)[globalIndex]);
                            } else {
                                entry.setValue(entry.getValue() - storm::utility::one<ValueType>());
                            }
                        }
                    }
                    ++row;
                }
                assert(row == auxMatrix.getRowCount());
                
                // We need to consider A^t. This will not delete diagonal entries since they are non-zero.
                auxMatrix = auxMatrix.transpose();
                
                // Check whether we need the fixpoint characterization
                storm::solver::GeneralLinearEquationSolverFactory<ValueType> linearEquationSolverFactory;
                bool isFixpointFormat = linearEquationSolverFactory.getEquationProblemFormat(subEnv) == storm::solver::LinearEquationSolverProblemFormat::FixedPointSystem;
                if (isFixpointFormat) {
                    // Add a 1 on the diagonal
                    for (row = 0; row < auxMatrix.getRowCount(); ++row) {
                        for (auto& entry : auxMatrix.getRow(row)) {
                            if (entry.getColumn() == row) {
                                entry.setValue(storm::utility::one<ValueType>() + entry.getValue());
                            }
                        }
                    }
                }
                
                // We now build the equation system matrix.
                // We can drop the last row of A and add ones in this row instead to assert that the variables sum up to one
                // Phase 1: replace the existing entries of the last row with ones
                uint64_t col = 0;
                uint64_t lastRow = auxMatrix.getRowCount() - 1;
                for (auto& entry : auxMatrix.getRow(lastRow)) {
                    entry.setColumn(col);
                    if (isFixpointFormat) {
                        if (col == lastRow) {
                            entry.setValue(storm::utility::zero<ValueType>());
                        } else {
                            entry.setValue(-storm::utility::one<ValueType>());
                        }
                    } else {
                        entry.setValue(storm::utility::one<ValueType>());
                    }
                    ++col;
                }
                storm::storage::SparseMatrixBuilder<ValueType> builder(std::move(auxMatrix));
                for (; col <= lastRow; ++col) {
                    if (isFixpointFormat) {
                        if (col != lastRow) {
                            builder.addNextValue(lastRow, col, -storm::utility::one<ValueType>());
                        }
                    } else {
                        builder.addNextValue(lastRow, col, storm::utility::one<ValueType>());
                    }
                }
                
                std::vector<ValueType> bsccEquationSystemRightSide(bscc.size(), storm::utility::zero<ValueType>());
                bsccEquationSystemRightSide.back() = storm::utility::one<ValueType>();
                
                // Create a linear equation solver
                auto solver = linearEquationSolverFactory.create(subEnv,  builder.build());
                solver->setBounds(storm::utility::zero<ValueType>(), storm::utility::one<ValueType>());
                // Check solver requirements.
                auto requirements = solver->getRequirements(subEnv);
                requirements.clearLowerBounds();
                requirements.clearUpperBounds();
                STORM_LOG_THROW(!requirements.hasEnabledCriticalRequirement(), storm::exceptions::UncheckedRequirementException, "Solver requirements " + requirements.getEnabledRequirementsAsString() + " not checked.");
                
                std::vector<ValueType> lraDistr(bscc.size(), storm::utility::one<ValueType>() / storm::utility::convertNumber<ValueType, uint64_t>(bscc.size()));
                solver->solveEquations(subEnv, lraDistr, bsccEquationSystemRightSide);
                
                // Calculate final LRA Value
                ValueType result = storm::utility::zero<ValueType>();
                auto solIt = lraDistr.begin();
                for (auto const& globalState : bscc) {
                    result += valueGetter(globalState) * (*solIt);
                    ++solIt;
                }
                assert(solIt == lraDistr.end());

                return std::pair<ValueType, std::vector<ValueType>>(std::move(result), std::move(lraDistr));
            }
            
            template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type>
            std::vector<ValueType> SparseCtmcCslHelper::computeAllTransientProbabilities(Environment const& env, storm::storage::SparseMatrix<ValueType> const& rateMatrix, storm::storage::BitVector const& initialStates, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, std::vector<ValueType> const& exitRates, double timeBound) {

                // Compute transient probabilities going from initial state
                // Instead of y=Px we now compute y=xP <=> y^T=P^Tx^T via transposition
                uint_fast64_t numberOfStates = rateMatrix.getRowCount();

                // Create the result vector.
                std::vector<ValueType> result = std::vector<ValueType>(numberOfStates, storm::utility::zero<ValueType>());

                storm::storage::SparseMatrix<ValueType> transposedMatrix(rateMatrix);
                transposedMatrix.makeRowsAbsorbing(psiStates);
                std::vector<ValueType> newRates = exitRates;
                for (auto const& state : psiStates) {
                    newRates[state] = storm::utility::one<ValueType>();
                }

                // Identify all maybe states which have a probability greater than 0 to be reached from the initial state.
                //storm::storage::BitVector statesWithProbabilityGreater0 = storm::utility::graph::performProbGreater0(transposedMatrix, phiStates, initialStates);
                //STORM_LOG_INFO("Found " << statesWithProbabilityGreater0.getNumberOfSetBits() << " states with probability greater 0.");

                //storm::storage::BitVector relevantStates = statesWithProbabilityGreater0 & ~initialStates;//phiStates | psiStates;
                storm::storage::BitVector relevantStates(numberOfStates, true);
                STORM_LOG_DEBUG(relevantStates.getNumberOfSetBits() << " relevant states.");

                if (!relevantStates.empty()) {
                    // Find the maximal rate of all relevant states to take it as the uniformization rate.
                    ValueType uniformizationRate = 0;
                    for (auto const& state : relevantStates) {
                        uniformizationRate = std::max(uniformizationRate, newRates[state]);
                    }
                    uniformizationRate *= 1.02;
                    STORM_LOG_THROW(uniformizationRate > 0, storm::exceptions::InvalidStateException, "The uniformization rate must be positive.");

                    transposedMatrix = transposedMatrix.transpose();

                    // Compute the uniformized matrix.
                    storm::storage::SparseMatrix<ValueType> uniformizedMatrix = computeUniformizedMatrix(transposedMatrix, relevantStates, uniformizationRate, newRates);

                    // Compute the vector that is to be added as a compensation for removing the absorbing states.
                    /*std::vector<ValueType> b = transposedMatrix.getConstrainedRowSumVector(relevantStates, initialStates);
                    for (auto& element : b) {
                        element /= uniformizationRate;
                        std::cout << element << std::endl;
                    }*/
                    
                    ValueType epsilon = storm::utility::convertNumber<ValueType>(env.solver().timeBounded().getPrecision()) / 8.0;
                    STORM_LOG_WARN_COND(!env.solver().timeBounded().getRelativeTerminationCriterion(), "Computation of transient probabilities with relative precision not supported. Using absolute precision instead.");
                    std::vector<ValueType> values(relevantStates.getNumberOfSetBits(), storm::utility::zero<ValueType>());
                    // Set initial states
                    size_t i = 0;
                    ValueType initDist = storm::utility::one<ValueType>() / initialStates.getNumberOfSetBits();
                    for (auto const& state : relevantStates) {
                        if (initialStates.get(state)) {
                            values[i] = initDist;
                        }
                        ++i;
                    }
                    // Finally compute the transient probabilities.
                    std::vector<ValueType> subresult = computeTransientProbabilities<ValueType>(env, uniformizedMatrix, nullptr, timeBound, uniformizationRate, values, epsilon);

                    storm::utility::vector::setVectorValues(result, relevantStates, subresult);
                }

                return result;
            }

            template <typename ValueType, typename std::enable_if<!storm::NumberTraits<ValueType>::SupportsExponential, int>::type>
            std::vector<ValueType> SparseCtmcCslHelper::computeAllTransientProbabilities(Environment const&, storm::storage::SparseMatrix<ValueType> const&, storm::storage::BitVector const&, storm::storage::BitVector const&, storm::storage::BitVector const&, std::vector<ValueType> const&, double) {
                STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Computing bounded until probabilities is unsupported for this value type.");
            }

            template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type>
            storm::storage::SparseMatrix<ValueType> SparseCtmcCslHelper::computeUniformizedMatrix(storm::storage::SparseMatrix<ValueType> const& rateMatrix, storm::storage::BitVector const& maybeStates, ValueType uniformizationRate, std::vector<ValueType> const& exitRates) {
                STORM_LOG_DEBUG("Computing uniformized matrix using uniformization rate " << uniformizationRate << ".");
                STORM_LOG_DEBUG("Keeping " << maybeStates.getNumberOfSetBits() << " rows.");
                
                // Create the submatrix that only contains the states with a positive probability (including the
                // psi states) and reserve space for elements on the diagonal.
                storm::storage::SparseMatrix<ValueType> uniformizedMatrix = rateMatrix.getSubmatrix(false, maybeStates, maybeStates, true);
                
                // Now we need to perform the actual uniformization. That is, all entries need to be divided by
                // the uniformization rate, and the diagonal needs to be set to the negative exit rate of the
                // state plus the self-loop rate and then increased by one.
                uint_fast64_t currentRow = 0;
                for (auto const& state : maybeStates) {
                    for (auto& element : uniformizedMatrix.getRow(currentRow)) {
                        if (element.getColumn() == currentRow) {
                            element.setValue((element.getValue() - exitRates[state]) / uniformizationRate + storm::utility::one<ValueType>());
                        } else {
                            element.setValue(element.getValue() / uniformizationRate);
                        }
                    }
                    ++currentRow;
                }
                
                return uniformizedMatrix;
            }

            template<typename ValueType, bool useMixedPoissonProbabilities, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type>
            std::vector<ValueType> SparseCtmcCslHelper::computeTransientProbabilities(Environment const& env, storm::storage::SparseMatrix<ValueType> const& uniformizedMatrix, std::vector<ValueType> const* addVector, ValueType timeBound, ValueType uniformizationRate, std::vector<ValueType> values, ValueType epsilon) {
                STORM_LOG_WARN_COND(epsilon > storm::utility::convertNumber<ValueType>(1e-20), "Very low truncation error " << epsilon << " requested. Numerical inaccuracies are possible.");
                ValueType lambda = timeBound * uniformizationRate;
                
                // If no time can pass, the current values are the result.
                if (storm::utility::isZero(lambda)) {
                    return values;
                }
                
                // Use Fox-Glynn to get the truncation points and the weights.
                storm::utility::numerical::FoxGlynnResult<ValueType> foxGlynnResult = storm::utility::numerical::foxGlynn(lambda, epsilon);
                STORM_LOG_DEBUG("Fox-Glynn cutoff points: left=" << foxGlynnResult.left << ", right=" << foxGlynnResult.right);
                // foxGlynnResult.weights do not sum up to one. This is to enhance numerical stability.
                
                // If the cumulative reward is to be computed, we need to adjust the weights.
                if (useMixedPoissonProbabilities) {
                    ValueType sum = storm::utility::zero<ValueType>();
                    
                    for (auto& element : foxGlynnResult.weights) {
                        sum += element;
                        element = (foxGlynnResult.totalWeight - sum) / uniformizationRate;
                    }
                }
                
                STORM_LOG_DEBUG("Starting iterations with " << uniformizedMatrix.getRowCount() << " x " << uniformizedMatrix.getColumnCount() << " matrix.");
                
                // Initialize result.
                std::vector<ValueType> result;
                uint_fast64_t startingIteration = foxGlynnResult.left;
                if (startingIteration == 0) {
                    result = values;
                    storm::utility::vector::scaleVectorInPlace(result, foxGlynnResult.weights.front());
                    ++startingIteration;
                } else {
                    if (useMixedPoissonProbabilities) {
                        result = std::vector<ValueType>(values.size());
                        std::function<ValueType (ValueType const&)> scaleWithUniformizationRate = [&uniformizationRate] (ValueType const& a) -> ValueType { return a / uniformizationRate; };
                        storm::utility::vector::applyPointwise(values, result, scaleWithUniformizationRate);
                    } else {
                        result = std::vector<ValueType>(values.size());
                    }
                }
                
                auto multiplier = storm::solver::MultiplierFactory<ValueType>().create(env, uniformizedMatrix);
                if (!useMixedPoissonProbabilities && foxGlynnResult.left > 1) {
                    // Perform the matrix-vector multiplications (without adding).
                    multiplier->repeatedMultiply(env, values, addVector, foxGlynnResult.left - 1);
                } else if (useMixedPoissonProbabilities) {
                    std::function<ValueType(ValueType const&, ValueType const&)> addAndScale = [&uniformizationRate] (ValueType const& a, ValueType const& b) { return a + b / uniformizationRate; };
                    
                    // For the iterations below the left truncation point, we need to add and scale the result with the uniformization rate.
                    for (uint_fast64_t index = 1; index < startingIteration; ++index) {
                        multiplier->multiply(env, values, nullptr, values);
                        storm::utility::vector::applyPointwise(result, values, result, addAndScale);
                    }
                    // To make sure that the values obtained before the left truncation point have the same 'impact' on the total result as the values obtained
                    // between the left and right truncation point, we scale them here with the total sum of the weights.
                    // Note that we divide with this value afterwards. This is to improve numerical stability.
                    storm::utility::vector::scaleVectorInPlace<ValueType, ValueType>(result, foxGlynnResult.totalWeight);
                }
                
                // For the indices that fall in between the truncation points, we need to perform the matrix-vector
                // multiplication, scale and add the result.
                ValueType weight = 0;
                std::function<ValueType(ValueType const&, ValueType const&)> addAndScale = [&weight] (ValueType const& a, ValueType const& b) { return a + weight * b; };
                for (uint_fast64_t index = startingIteration; index <= foxGlynnResult.right; ++index) {
                    multiplier->multiply(env, values, addVector, values);
                    
                    weight = foxGlynnResult.weights[index - foxGlynnResult.left];
                    storm::utility::vector::applyPointwise(result, values, result, addAndScale);
                }
                
                // Finally, divide the result by the total weight
                storm::utility::vector::scaleVectorInPlace<ValueType, ValueType>(result, storm::utility::one<ValueType>() / foxGlynnResult.totalWeight);
                return result;
            }
            
            template <typename ValueType>
            storm::storage::SparseMatrix<ValueType> SparseCtmcCslHelper::computeProbabilityMatrix(storm::storage::SparseMatrix<ValueType> const& rateMatrix, std::vector<ValueType> const& exitRates) {
                // Turn the rates into probabilities by scaling each row with the exit rate of the state.
                storm::storage::SparseMatrix<ValueType> result(rateMatrix);
                for (uint_fast64_t row = 0; row < result.getRowCount(); ++row) {
                    for (auto& entry : result.getRow(row)) {
                        entry.setValue(entry.getValue() / exitRates[row]);
                    }
                }
                return result;
            }
            
            template <typename ValueType>
            storm::storage::SparseMatrix<ValueType> SparseCtmcCslHelper::computeGeneratorMatrix(storm::storage::SparseMatrix<ValueType> const& rateMatrix, std::vector<ValueType> const& exitRates) {
                storm::storage::SparseMatrix<ValueType> generatorMatrix(rateMatrix, true);
                
                // Place the negative exit rate on the diagonal.
                for (uint_fast64_t row = 0; row < generatorMatrix.getRowCount(); ++row) {
                    for (auto& entry : generatorMatrix.getRow(row)) {
                        if (entry.getColumn() == row) {
                            entry.setValue(-exitRates[row] + entry.getValue());
                        }
                    }
                }
                
                return generatorMatrix;
            }
            
            
            template std::vector<double> SparseCtmcCslHelper::computeBoundedUntilProbabilities(Environment const& env, storm::solver::SolveGoal<double>&& goal, storm::storage::SparseMatrix<double> const& rateMatrix, storm::storage::SparseMatrix<double> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, std::vector<double> const& exitRates, bool qualitative, double lowerBound, double upperBound);
            
            template std::vector<double> SparseCtmcCslHelper::computeUntilProbabilities(Environment const& env, storm::solver::SolveGoal<double>&& goal, storm::storage::SparseMatrix<double> const& rateMatrix, storm::storage::SparseMatrix<double> const& backwardTransitions, std::vector<double> const& exitRateVector, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative);

            template std::vector<double> SparseCtmcCslHelper::computeAllUntilProbabilities(Environment const& env, storm::solver::SolveGoal<double>&& goal, storm::storage::SparseMatrix<double> const& rateMatrix, std::vector<double> const& exitRateVector, storm::storage::BitVector const& initialStates, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates);

            template std::vector<double> SparseCtmcCslHelper::computeNextProbabilities(Environment const& env, storm::storage::SparseMatrix<double> const& rateMatrix, std::vector<double> const& exitRateVector, storm::storage::BitVector const& nextStates);
            
            template std::vector<double> SparseCtmcCslHelper::computeInstantaneousRewards(Environment const& env, storm::solver::SolveGoal<double>&& goal, storm::storage::SparseMatrix<double> const& rateMatrix, std::vector<double> const& exitRateVector, storm::models::sparse::StandardRewardModel<double> const& rewardModel, double timeBound);
            
            template std::vector<double> SparseCtmcCslHelper::computeReachabilityTimes(Environment const& env, storm::solver::SolveGoal<double>&& goal, storm::storage::SparseMatrix<double> const& rateMatrix, storm::storage::SparseMatrix<double> const& backwardTransitions, std::vector<double> const& exitRateVector, storm::storage::BitVector const& targetStates, bool qualitative);
            
            template std::vector<double> SparseCtmcCslHelper::computeReachabilityRewards(Environment const& env, storm::solver::SolveGoal<double>&& goal, storm::storage::SparseMatrix<double> const& rateMatrix, storm::storage::SparseMatrix<double> const& backwardTransitions, std::vector<double> const& exitRateVector, storm::models::sparse::StandardRewardModel<double> const& rewardModel, storm::storage::BitVector const& targetStates, bool qualitative);
            
            template std::vector<double> SparseCtmcCslHelper::computeTotalRewards(Environment const& env, storm::solver::SolveGoal<double>&& goal, storm::storage::SparseMatrix<double> const& rateMatrix, storm::storage::SparseMatrix<double> const& backwardTransitions, std::vector<double> const& exitRateVector, storm::models::sparse::StandardRewardModel<double> const& rewardModel, bool qualitative);
            
            template std::vector<double> SparseCtmcCslHelper::computeLongRunAverageProbabilities(Environment const& env, storm::solver::SolveGoal<double>&& goal, storm::storage::SparseMatrix<double> const& rateMatrix, storm::storage::BitVector const& psiStates, std::vector<double> const* exitRateVector);
            template std::vector<double> SparseCtmcCslHelper::computeLongRunAverageRewards(Environment const& env, storm::solver::SolveGoal<double>&& goal, storm::storage::SparseMatrix<double> const& rateMatrix, storm::models::sparse::StandardRewardModel<double> const& rewardModel, std::vector<double> const* exitRateVector);
            template std::vector<double> SparseCtmcCslHelper::computeLongRunAverageRewards(Environment const& env, storm::solver::SolveGoal<double>&& goal, storm::storage::SparseMatrix<double> const& rateMatrix, std::vector<double> const& stateRewardVector, std::vector<double> const* exitRateVector);
            
            template std::vector<double> SparseCtmcCslHelper::computeCumulativeRewards(Environment const& env, storm::solver::SolveGoal<double>&& goal, storm::storage::SparseMatrix<double> const& rateMatrix, std::vector<double> const& exitRateVector, storm::models::sparse::StandardRewardModel<double> const& rewardModel, double timeBound);

            template std::vector<double> SparseCtmcCslHelper::computeAllTransientProbabilities(Environment const& env, storm::storage::SparseMatrix<double> const& rateMatrix, storm::storage::BitVector const& initialStates, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, std::vector<double> const& exitRates, double timeBound);
            
            template storm::storage::SparseMatrix<double> SparseCtmcCslHelper::computeUniformizedMatrix(storm::storage::SparseMatrix<double> const& rateMatrix, storm::storage::BitVector const& maybeStates, double uniformizationRate, std::vector<double> const& exitRates);
            
            template std::vector<double> SparseCtmcCslHelper::computeTransientProbabilities(Environment const& env, storm::storage::SparseMatrix<double> const& uniformizedMatrix, std::vector<double> const* addVector, double timeBound, double uniformizationRate, std::vector<double> values, double epsilon);

#ifdef STORM_HAVE_CARL
            template std::vector<storm::RationalNumber> SparseCtmcCslHelper::computeBoundedUntilProbabilities(Environment const& env, storm::solver::SolveGoal<storm::RationalNumber>&& goal, storm::storage::SparseMatrix<storm::RationalNumber> const& rateMatrix, storm::storage::SparseMatrix<storm::RationalNumber> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, std::vector<storm::RationalNumber> const& exitRates, bool qualitative, double lowerBound, double upperBound);
            template std::vector<storm::RationalFunction> SparseCtmcCslHelper::computeBoundedUntilProbabilities(Environment const& env, storm::solver::SolveGoal<storm::RationalFunction>&& goal, storm::storage::SparseMatrix<storm::RationalFunction> const& rateMatrix, storm::storage::SparseMatrix<storm::RationalFunction> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, std::vector<storm::RationalFunction> const& exitRates, bool qualitative, double lowerBound, double upperBound);

            template std::vector<storm::RationalNumber> SparseCtmcCslHelper::computeUntilProbabilities(Environment const& env, storm::solver::SolveGoal<storm::RationalNumber>&& goal, storm::storage::SparseMatrix<storm::RationalNumber> const& rateMatrix, storm::storage::SparseMatrix<storm::RationalNumber> const& backwardTransitions, std::vector<storm::RationalNumber> const& exitRateVector, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative);
            template std::vector<storm::RationalFunction> SparseCtmcCslHelper::computeUntilProbabilities(Environment const& env, storm::solver::SolveGoal<storm::RationalFunction>&& goal, storm::storage::SparseMatrix<storm::RationalFunction> const& rateMatrix, storm::storage::SparseMatrix<storm::RationalFunction> const& backwardTransitions, std::vector<storm::RationalFunction> const& exitRateVector, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative);

            template std::vector<storm::RationalNumber> SparseCtmcCslHelper::computeAllUntilProbabilities(Environment const& env, storm::solver::SolveGoal<storm::RationalNumber>&& goal, storm::storage::SparseMatrix<storm::RationalNumber> const& rateMatrix, std::vector<storm::RationalNumber> const& exitRateVector, storm::storage::BitVector const& initialStates, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates);
            template std::vector<storm::RationalFunction> SparseCtmcCslHelper::computeAllUntilProbabilities(Environment const& env, storm::solver::SolveGoal<storm::RationalFunction>&& goal, storm::storage::SparseMatrix<storm::RationalFunction> const& rateMatrix, std::vector<storm::RationalFunction> const& exitRateVector, storm::storage::BitVector const& initialStates, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates);

            template std::vector<storm::RationalNumber> SparseCtmcCslHelper::computeNextProbabilities(Environment const& env, storm::storage::SparseMatrix<storm::RationalNumber> const& rateMatrix, std::vector<storm::RationalNumber> const& exitRateVector, storm::storage::BitVector const& nextStates);
            template std::vector<storm::RationalFunction> SparseCtmcCslHelper::computeNextProbabilities(Environment const& env, storm::storage::SparseMatrix<storm::RationalFunction> const& rateMatrix, std::vector<storm::RationalFunction> const& exitRateVector, storm::storage::BitVector const& nextStates);

            template std::vector<storm::RationalNumber> SparseCtmcCslHelper::computeInstantaneousRewards(Environment const& env, storm::solver::SolveGoal<storm::RationalNumber>&& goal, storm::storage::SparseMatrix<storm::RationalNumber> const& rateMatrix, std::vector<storm::RationalNumber> const& exitRateVector, storm::models::sparse::StandardRewardModel<storm::RationalNumber> const& rewardModel, double timeBound);
            template std::vector<storm::RationalFunction> SparseCtmcCslHelper::computeInstantaneousRewards(Environment const& env, storm::solver::SolveGoal<storm::RationalFunction>&& goal, storm::storage::SparseMatrix<storm::RationalFunction> const& rateMatrix, std::vector<storm::RationalFunction> const& exitRateVector, storm::models::sparse::StandardRewardModel<storm::RationalFunction> const& rewardModel, double timeBound);

            template std::vector<storm::RationalNumber> SparseCtmcCslHelper::computeReachabilityTimes(Environment const& env, storm::solver::SolveGoal<storm::RationalNumber>&& goal, storm::storage::SparseMatrix<storm::RationalNumber> const& rateMatrix, storm::storage::SparseMatrix<storm::RationalNumber> const& backwardTransitions, std::vector<storm::RationalNumber> const& exitRateVector, storm::storage::BitVector const& targetStates, bool qualitative);
            template std::vector<storm::RationalFunction> SparseCtmcCslHelper::computeReachabilityTimes(Environment const& env, storm::solver::SolveGoal<storm::RationalFunction>&& goal, storm::storage::SparseMatrix<storm::RationalFunction> const& rateMatrix, storm::storage::SparseMatrix<storm::RationalFunction> const& backwardTransitions, std::vector<storm::RationalFunction> const& exitRateVector, storm::storage::BitVector const& targetStates, bool qualitative);

            template std::vector<storm::RationalNumber> SparseCtmcCslHelper::computeReachabilityRewards(Environment const& env, storm::solver::SolveGoal<storm::RationalNumber>&& goal, storm::storage::SparseMatrix<storm::RationalNumber> const& rateMatrix, storm::storage::SparseMatrix<storm::RationalNumber> const& backwardTransitions, std::vector<storm::RationalNumber> const& exitRateVector, storm::models::sparse::StandardRewardModel<storm::RationalNumber> const& rewardModel, storm::storage::BitVector const& targetStates, bool qualitative);
            template std::vector<storm::RationalFunction> SparseCtmcCslHelper::computeReachabilityRewards(Environment const& env, storm::solver::SolveGoal<storm::RationalFunction>&& goal, storm::storage::SparseMatrix<storm::RationalFunction> const& rateMatrix, storm::storage::SparseMatrix<storm::RationalFunction> const& backwardTransitions, std::vector<storm::RationalFunction> const& exitRateVector, storm::models::sparse::StandardRewardModel<storm::RationalFunction> const& rewardModel, storm::storage::BitVector const& targetStates, bool qualitative);

            template std::vector<storm::RationalNumber> SparseCtmcCslHelper::computeTotalRewards(Environment const& env, storm::solver::SolveGoal<storm::RationalNumber>&& goal, storm::storage::SparseMatrix<storm::RationalNumber> const& rateMatrix, storm::storage::SparseMatrix<storm::RationalNumber> const& backwardTransitions, std::vector<storm::RationalNumber> const& exitRateVector, storm::models::sparse::StandardRewardModel<storm::RationalNumber> const& rewardModel, bool qualitative);
            template std::vector<storm::RationalFunction> SparseCtmcCslHelper::computeTotalRewards(Environment const& env, storm::solver::SolveGoal<storm::RationalFunction>&& goal, storm::storage::SparseMatrix<storm::RationalFunction> const& rateMatrix, storm::storage::SparseMatrix<storm::RationalFunction> const& backwardTransitions, std::vector<storm::RationalFunction> const& exitRateVector, storm::models::sparse::StandardRewardModel<storm::RationalFunction> const& rewardModel, bool qualitative);

            template std::vector<storm::RationalNumber> SparseCtmcCslHelper::computeLongRunAverageProbabilities(Environment const& env, storm::solver::SolveGoal<storm::RationalNumber>&& goal, storm::storage::SparseMatrix<storm::RationalNumber> const& rateMatrix, storm::storage::BitVector const& psiStates, std::vector<storm::RationalNumber> const* exitRateVector);
            template std::vector<storm::RationalFunction> SparseCtmcCslHelper::computeLongRunAverageProbabilities(Environment const& env, storm::solver::SolveGoal<storm::RationalFunction>&& goal, storm::storage::SparseMatrix<storm::RationalFunction> const& rateMatrix, storm::storage::BitVector const& psiStates, std::vector<storm::RationalFunction> const* exitRateVector);
            
            template std::vector<storm::RationalNumber> SparseCtmcCslHelper::computeLongRunAverageRewards(Environment const& env, storm::solver::SolveGoal<storm::RationalNumber>&& goal, storm::storage::SparseMatrix<storm::RationalNumber> const& rateMatrix, storm::models::sparse::StandardRewardModel<RationalNumber> const& rewardModel, std::vector<storm::RationalNumber> const* exitRateVector);
            template std::vector<storm::RationalFunction> SparseCtmcCslHelper::computeLongRunAverageRewards(Environment const& env, storm::solver::SolveGoal<storm::RationalFunction>&& goal, storm::storage::SparseMatrix<storm::RationalFunction> const& rateMatrix, storm::models::sparse::StandardRewardModel<RationalFunction> const& rewardModel, std::vector<storm::RationalFunction> const* exitRateVector);

            template std::vector<storm::RationalNumber> SparseCtmcCslHelper::computeLongRunAverageRewards(Environment const& env, storm::solver::SolveGoal<storm::RationalNumber>&& goal, storm::storage::SparseMatrix<storm::RationalNumber> const& rateMatrix, std::vector<storm::RationalNumber> const& stateRewardVector, std::vector<storm::RationalNumber> const* exitRateVector);
            template std::vector<storm::RationalFunction> SparseCtmcCslHelper::computeLongRunAverageRewards(Environment const& env, storm::solver::SolveGoal<storm::RationalFunction>&& goal, storm::storage::SparseMatrix<storm::RationalFunction> const& rateMatrix, std::vector<storm::RationalFunction> const& stateRewardVector, std::vector<storm::RationalFunction> const* exitRateVector);

            template std::vector<storm::RationalNumber> SparseCtmcCslHelper::computeCumulativeRewards(Environment const& env, storm::solver::SolveGoal<storm::RationalNumber>&& goal, storm::storage::SparseMatrix<storm::RationalNumber> const& rateMatrix, std::vector<storm::RationalNumber> const& exitRateVector, storm::models::sparse::StandardRewardModel<storm::RationalNumber> const& rewardModel, double timeBound);
            template std::vector<storm::RationalFunction> SparseCtmcCslHelper::computeCumulativeRewards(Environment const& env, storm::solver::SolveGoal<storm::RationalFunction>&& goal, storm::storage::SparseMatrix<storm::RationalFunction> const& rateMatrix, std::vector<storm::RationalFunction> const& exitRateVector, storm::models::sparse::StandardRewardModel<storm::RationalFunction> const& rewardModel, double timeBound);

            template std::vector<storm::RationalNumber> SparseCtmcCslHelper::computeAllTransientProbabilities(Environment const& env, storm::storage::SparseMatrix<storm::RationalNumber> const& rateMatrix, storm::storage::BitVector const& initialStates, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, std::vector<storm::RationalNumber> const& exitRates, double timeBound);
            template std::vector<storm::RationalFunction> SparseCtmcCslHelper::computeAllTransientProbabilities(Environment const& env, storm::storage::SparseMatrix<storm::RationalFunction> const& rateMatrix, storm::storage::BitVector const& initialStates, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, std::vector<storm::RationalFunction> const& exitRates, double timeBound);

            template storm::storage::SparseMatrix<double> SparseCtmcCslHelper::computeProbabilityMatrix(storm::storage::SparseMatrix<double> const& rateMatrix, std::vector<double> const& exitRates);
            template storm::storage::SparseMatrix<storm::RationalNumber> SparseCtmcCslHelper::computeProbabilityMatrix(storm::storage::SparseMatrix<storm::RationalNumber> const& rateMatrix, std::vector<storm::RationalNumber> const& exitRates);
            template storm::storage::SparseMatrix<storm::RationalFunction> SparseCtmcCslHelper::computeProbabilityMatrix(storm::storage::SparseMatrix<storm::RationalFunction> const& rateMatrix, std::vector<storm::RationalFunction> const& exitRates);

#endif
        }
    }
}
