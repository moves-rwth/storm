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

#include "storm/utility/macros.h"
#include "storm/utility/vector.h"
#include "storm/utility/graph.h"
#include "storm/utility/numerical.h"

#include "storm/exceptions/InvalidOperationException.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/exceptions/FormatUnsupportedBySolverException.h"
#include "storm/exceptions/UncheckedRequirementException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type>
            std::vector<ValueType> SparseCtmcCslHelper::computeBoundedUntilProbabilities(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal, storm::storage::SparseMatrix<ValueType> const& rateMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, std::vector<ValueType> const& exitRates, bool qualitative, double lowerBound, double upperBound) {
                
                uint_fast64_t numberOfStates = rateMatrix.getRowCount();
                
                // If the time bounds are [0, inf], we rather call untimed reachability.
                if (storm::utility::isZero(lowerBound) && upperBound == storm::utility::infinity<ValueType>()) {
                    return computeUntilProbabilities(env, std::move(goal), rateMatrix, backwardTransitions, exitRates, phiStates, psiStates, qualitative);
                }
                
                // From this point on, we know that we have to solve a more complicated problem [t, t'] with either t != 0
                // or t' != inf.
                
                // Create the result vector.
                std::vector<ValueType> result;
                
                // If we identify the states that have probability 0 of reaching the target states, we can exclude them from the
                // further computations.
                storm::storage::BitVector statesWithProbabilityGreater0 = storm::utility::graph::performProbGreater0(backwardTransitions, phiStates, psiStates);
                STORM_LOG_INFO("Found " << statesWithProbabilityGreater0.getNumberOfSetBits() << " states with probability greater 0.");
                storm::storage::BitVector statesWithProbabilityGreater0NonPsi = statesWithProbabilityGreater0 & ~psiStates;
                STORM_LOG_INFO("Found " << statesWithProbabilityGreater0NonPsi.getNumberOfSetBits() << " 'maybe' states.");
                
                if (!statesWithProbabilityGreater0NonPsi.empty()) {
                    if (storm::utility::isZero(upperBound)) {
                        // In this case, the interval is of the form [0, 0].
                        result = std::vector<ValueType>(numberOfStates, storm::utility::zero<ValueType>());
                        storm::utility::vector::setVectorValues<ValueType>(result, psiStates, storm::utility::one<ValueType>());
                    } else {
                        if (storm::utility::isZero(lowerBound)) {
                            // In this case, the interval is of the form [0, t].
                            // Note that this excludes [0, inf] since this is untimed reachability and we considered this case earlier.
                            
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
                            std::vector<ValueType> subresult = computeTransientProbabilities(env, uniformizedMatrix, &b, upperBound, uniformizationRate, values);
                            result = std::vector<ValueType>(numberOfStates, storm::utility::zero<ValueType>());
                            
                            storm::utility::vector::setVectorValues(result, statesWithProbabilityGreater0NonPsi, subresult);
                            storm::utility::vector::setVectorValues(result, psiStates, storm::utility::one<ValueType>());
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
                            subResult = computeTransientProbabilities<ValueType>(env, uniformizedMatrix, nullptr, lowerBound, uniformizationRate, subResult);
                            
                            // Fill in the correct values.
                            storm::utility::vector::setVectorValues(result, ~relevantStates, storm::utility::zero<ValueType>());
                            storm::utility::vector::setVectorValues(result, relevantStates, subResult);
                        } else {
                            // In this case, the interval is of the form [t, t'] with t != 0 and t' != inf.
                            
                            if (lowerBound != upperBound) {
                                // In this case, the interval is of the form [t, t'] with t != 0, t' != inf and t != t'.
                                
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
                                std::vector<ValueType> subresult = computeTransientProbabilities(env, uniformizedMatrix, &b, upperBound - lowerBound, uniformizationRate, values);
                                
                                storm::storage::BitVector relevantStates = statesWithProbabilityGreater0 & phiStates;
                                std::vector<ValueType> newSubresult = std::vector<ValueType>(relevantStates.getNumberOfSetBits());
                                storm::utility::vector::setVectorValues(newSubresult, statesWithProbabilityGreater0NonPsi % relevantStates, subresult);
                                storm::utility::vector::setVectorValues(newSubresult, psiStates % relevantStates, storm::utility::one<ValueType>());
                                
                                // Then compute the transient probabilities of being in such a state after t time units. For this,
                                // we must re-uniformize the CTMC, so we need to compute the second uniformized matrix.
                                uniformizationRate = storm::utility::zero<ValueType>();
                                for (auto const& state : relevantStates) {
                                    uniformizationRate = std::max(uniformizationRate, exitRates[state]);
                                }
                                uniformizationRate *= 1.02;
                                STORM_LOG_THROW(uniformizationRate > 0, storm::exceptions::InvalidStateException, "The uniformization rate must be positive.");
                                
                                // Finally, we compute the second set of transient probabilities.
                                uniformizedMatrix = computeUniformizedMatrix(rateMatrix, relevantStates, uniformizationRate, exitRates);
                                newSubresult = computeTransientProbabilities<ValueType>(env, uniformizedMatrix, nullptr, lowerBound, uniformizationRate, newSubresult);
                                
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
                                newSubresult = computeTransientProbabilities<ValueType>(env, uniformizedMatrix, nullptr, lowerBound, uniformizationRate, newSubresult);
                                
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
                
                // If the time-bound is not zero, we need to perform a transient analysis.
                if (timeBound > 0) {
                    ValueType uniformizationRate = 0;
                    for (auto const& rate : exitRateVector) {
                        uniformizationRate = std::max(uniformizationRate, rate);
                    }
                    uniformizationRate *= 1.02;
                    STORM_LOG_THROW(uniformizationRate > 0, storm::exceptions::InvalidStateException, "The uniformization rate must be positive.");
                    
                    storm::storage::SparseMatrix<ValueType> uniformizedMatrix = computeUniformizedMatrix(rateMatrix, storm::storage::BitVector(numberOfStates, true), uniformizationRate, exitRateVector);
                    result = computeTransientProbabilities<ValueType>(env, uniformizedMatrix, nullptr, timeBound, uniformizationRate, result);
                }
                
                return result;
            }
            
            template <typename ValueType, typename RewardModelType, typename std::enable_if<!storm::NumberTraits<ValueType>::SupportsExponential, int>::type>
            std::vector<ValueType> SparseCtmcCslHelper::computeInstantaneousRewards(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal, storm::storage::SparseMatrix<ValueType> const&, std::vector<ValueType> const&, RewardModelType const&, double) {
                STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Computing instantaneous rewards is unsupported for this value type.");
            }
            
            template <typename ValueType, typename RewardModelType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type>
            std::vector<ValueType> SparseCtmcCslHelper::computeCumulativeRewards(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal, storm::storage::SparseMatrix<ValueType> const& rateMatrix, std::vector<ValueType> const& exitRateVector, RewardModelType const& rewardModel, double timeBound) {
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
                
                // Finally, compute the transient probabilities.
                return computeTransientProbabilities<ValueType, true>(env, uniformizedMatrix, nullptr, timeBound, uniformizationRate, totalRewardVector);
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
            std::vector<ValueType> SparseCtmcCslHelper::computeLongRunAverageProbabilities(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal, storm::storage::SparseMatrix<ValueType> const& probabilityMatrix, storm::storage::BitVector const& psiStates, std::vector<ValueType> const* exitRateVector) {
                
                // If there are no goal states, we avoid the computation and directly return zero.
                uint_fast64_t numberOfStates = probabilityMatrix.getRowCount();
                if (psiStates.empty()) {
                    return std::vector<ValueType>(numberOfStates, storm::utility::zero<ValueType>());
                }
                
                // Likewise, if all bits are set, we can avoid the computation.
                if (psiStates.full()) {
                    return std::vector<ValueType>(numberOfStates, storm::utility::one<ValueType>());
                }
                
                ValueType zero = storm::utility::zero<ValueType>();
                ValueType one = storm::utility::one<ValueType>();
                
                return computeLongRunAverages<ValueType>(env, std::move(goal), probabilityMatrix,
                                              [&zero, &one, &psiStates] (storm::storage::sparse::state_type const& state) -> ValueType {
                                                  if (psiStates.get(state)) {
                                                      return one;
                                                  }
                                                  return zero;
                                              },
                                              exitRateVector);
            }
            
            template <typename ValueType, typename RewardModelType>
            std::vector<ValueType> SparseCtmcCslHelper::computeLongRunAverageRewards(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal, storm::storage::SparseMatrix<ValueType> const& probabilityMatrix, RewardModelType const& rewardModel, std::vector<ValueType> const* exitRateVector) {
                // Only compute the result if the model has a state-based reward model.
                STORM_LOG_THROW(!rewardModel.empty(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");

                return computeLongRunAverageRewards(env, std::move(goal), probabilityMatrix, rewardModel.getTotalRewardVector(probabilityMatrix, *exitRateVector), exitRateVector);
            }
            
            template <typename ValueType>
            std::vector<ValueType> SparseCtmcCslHelper::computeLongRunAverageRewards(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal, storm::storage::SparseMatrix<ValueType> const& probabilityMatrix, std::vector<ValueType> const& stateRewardVector, std::vector<ValueType> const* exitRateVector) {
                return computeLongRunAverages<ValueType>(env, std::move(goal), probabilityMatrix,
                                                         [&stateRewardVector] (storm::storage::sparse::state_type const& state) -> ValueType {
                                                             return stateRewardVector[state];
                                                         },
                                                         exitRateVector);
            }
            
            template <typename ValueType>
            std::vector<ValueType> SparseCtmcCslHelper::computeLongRunAverages(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal, storm::storage::SparseMatrix<ValueType> const& probabilityMatrix, std::function<ValueType (storm::storage::sparse::state_type const& state)> const& valueGetter, std::vector<ValueType> const* exitRateVector){
                uint_fast64_t numberOfStates = probabilityMatrix.getRowCount();

                // Start by decomposing the CTMC into its BSCCs.
                storm::storage::StronglyConnectedComponentDecomposition<ValueType> bsccDecomposition(probabilityMatrix, storm::storage::BitVector(probabilityMatrix.getRowCount(), true), false, true);
                
                STORM_LOG_DEBUG("Found " << bsccDecomposition.size() << " BSCCs.");
                
                // Get some data members for convenience.
                ValueType one = storm::utility::one<ValueType>();
                ValueType zero = storm::utility::zero<ValueType>();
                
                // Prepare the vector holding the LRA values for each of the BSCCs.
                std::vector<ValueType> bsccLra(bsccDecomposition.size(), zero);
                
                // First we check which states are in BSCCs.
                storm::storage::BitVector statesInBsccs(numberOfStates);
                storm::storage::BitVector firstStatesInBsccs(numberOfStates);
                
                for (uint_fast64_t currentBsccIndex = 0; currentBsccIndex < bsccDecomposition.size(); ++currentBsccIndex) {
                    storm::storage::StronglyConnectedComponent const& bscc = bsccDecomposition[currentBsccIndex];
                    
                    // Gather information for later use.
                    bool first = true;
                    for (auto const& state : bscc) {
                        statesInBsccs.set(state);
                        if (first) {
                            firstStatesInBsccs.set(state);
                        }
                        first = false;
                    }
                }
                storm::storage::BitVector statesNotInBsccs = ~statesInBsccs;
                
                STORM_LOG_DEBUG("Found " << statesInBsccs.getNumberOfSetBits() << " states in BSCCs.");
                
                // Prepare a vector holding the index within all states that are in BSCCs for every state.
                std::vector<uint_fast64_t> indexInStatesInBsccs;
                
                // Prepare a vector that maps the index within the set of all states in BSCCs to the index of the containing BSCC.
                std::vector<uint_fast64_t> stateToBsccIndexMap;
                
                if (!statesInBsccs.empty()) {
                    firstStatesInBsccs = firstStatesInBsccs % statesInBsccs;
                    
                    // Then we construct an equation system that yields the steady state probabilities for all states in BSCCs.
                    storm::storage::SparseMatrix<ValueType> bsccEquationSystem = probabilityMatrix.getSubmatrix(false, statesInBsccs, statesInBsccs, true);
                    
                    // Since in the fix point equation, we need to multiply the vector from the left, we convert this to a
                    // multiplication from the right by transposing the system.
                    bsccEquationSystem = bsccEquationSystem.transpose(false, true);
                    
                    // Create an auxiliary structure that makes it easy to look up the indices within the set of BSCC states.
                    uint_fast64_t lastIndex = 0;
                    uint_fast64_t currentNumberOfSetBits = 0;
                    indexInStatesInBsccs.reserve(probabilityMatrix.getRowCount());
                    for (auto index : statesInBsccs) {
                        while (lastIndex <= index) {
                            indexInStatesInBsccs.push_back(currentNumberOfSetBits);
                            ++lastIndex;
                        }
                        ++currentNumberOfSetBits;
                    }
                    
                    stateToBsccIndexMap.resize(statesInBsccs.getNumberOfSetBits());
                    for (uint_fast64_t currentBsccIndex = 0; currentBsccIndex < bsccDecomposition.size(); ++currentBsccIndex) {
                        storm::storage::StronglyConnectedComponent const& bscc = bsccDecomposition[currentBsccIndex];
                        for (auto const& state : bscc) {
                            stateToBsccIndexMap[indexInStatesInBsccs[state]] = currentBsccIndex;
                        }
                    }
                    
                    // Build a different system depending on the problem format of the equation solver.
                    // Check solver requirements.
                    storm::solver::GeneralLinearEquationSolverFactory<ValueType> linearEquationSolverFactory;
                    auto requirements = linearEquationSolverFactory.getRequirements(env);
                    STORM_LOG_THROW(!requirements.hasEnabledCriticalRequirement(), storm::exceptions::UncheckedRequirementException, "Solver requirements " + requirements.getEnabledRequirementsAsString() + " not checked.");
                    
                    bool fixedPointSystem = false;
                    if (linearEquationSolverFactory.getEquationProblemFormat(env) == storm::solver::LinearEquationSolverProblemFormat::FixedPointSystem) {
                        fixedPointSystem = true;
                    }

                    // Now build the final equation system matrix, the initial guess and the right-hand side in one go.
                    std::vector<ValueType> bsccEquationSystemRightSide(bsccEquationSystem.getColumnCount(), zero);
                    storm::storage::SparseMatrixBuilder<ValueType> builder;
                    for (uint_fast64_t row = 0; row < bsccEquationSystem.getRowCount(); ++row) {
                    
                        // If the current row is the first one belonging to a BSCC, we substitute it by the constraint that the
                        // values for states of this BSCC must sum to one. However, in order to have a non-zero value on the
                        // diagonal, we add the constraint of the BSCC that produces a 1 on the diagonal.
                        if (firstStatesInBsccs.get(row)) {
                            uint_fast64_t requiredBscc = stateToBsccIndexMap[row];
                            storm::storage::StronglyConnectedComponent const& bscc = bsccDecomposition[requiredBscc];

                            if (fixedPointSystem) {
                                for (auto const& state : bscc) {
                                    if (row == indexInStatesInBsccs[state]) {
                                        builder.addNextValue(row, indexInStatesInBsccs[state], zero);
                                    } else {
                                        builder.addNextValue(row, indexInStatesInBsccs[state], -one);
                                    }
                                }
                            } else {
                                for (auto const& state : bscc) {
                                    builder.addNextValue(row, indexInStatesInBsccs[state], one);
                                }
                            }
                            
                            bsccEquationSystemRightSide[row] = one;
                        } else {
                            // Otherwise, we copy the row, and subtract 1 from the diagonal (only for the equation solver format).
                            for (auto& entry : bsccEquationSystem.getRow(row)) {
                                if (fixedPointSystem || entry.getColumn() != row) {
                                    builder.addNextValue(row, entry.getColumn(), entry.getValue());
                                } else {
                                    builder.addNextValue(row, entry.getColumn(), entry.getValue() - one);
                                }
                            }
                        }
                    }
                    
                    // Create the initial guess for the LRAs. We take a uniform distribution over all states in a BSCC.
                    std::vector<ValueType> bsccEquationSystemSolution(bsccEquationSystem.getColumnCount(), zero);
                    for (uint_fast64_t bsccIndex = 0; bsccIndex < bsccDecomposition.size(); ++bsccIndex) {
                        storm::storage::StronglyConnectedComponent const& bscc = bsccDecomposition[bsccIndex];
                        
                        for (auto const& state : bscc) {
                            bsccEquationSystemSolution[indexInStatesInBsccs[state]] = one / bscc.size();
                        }
                    }
                    
                    bsccEquationSystem = builder.build();
                    {
                        std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = linearEquationSolverFactory.create(env, std::move(bsccEquationSystem));
                        solver->solveEquations(env, bsccEquationSystemSolution, bsccEquationSystemRightSide);
                    }
                    
                    // If exit rates were given, we need to 'fix' the results to also account for the timing behaviour.
                    if (exitRateVector != nullptr) {
                        std::vector<ValueType> bsccTotalValue(bsccDecomposition.size(), zero);
                        for (auto stateIter = statesInBsccs.begin(); stateIter != statesInBsccs.end(); ++stateIter) {
                            bsccTotalValue[stateToBsccIndexMap[indexInStatesInBsccs[*stateIter]]] += bsccEquationSystemSolution[indexInStatesInBsccs[*stateIter]] * (one / (*exitRateVector)[*stateIter]);
                        }
                        
                        for (auto stateIter = statesInBsccs.begin(); stateIter != statesInBsccs.end(); ++stateIter) {
                            bsccEquationSystemSolution[indexInStatesInBsccs[*stateIter]] = (bsccEquationSystemSolution[indexInStatesInBsccs[*stateIter]] * (one / (*exitRateVector)[*stateIter])) / bsccTotalValue[stateToBsccIndexMap[indexInStatesInBsccs[*stateIter]]];
                        }
                    }
                                        
                    // Calculate LRA Value for each BSCC from steady state distribution in BSCCs.
                    for (uint_fast64_t bsccIndex = 0; bsccIndex < bsccDecomposition.size(); ++bsccIndex) {
                        storm::storage::StronglyConnectedComponent const& bscc = bsccDecomposition[bsccIndex];
                        
                        for (auto const& state : bscc) {
                            bsccLra[stateToBsccIndexMap[indexInStatesInBsccs[state]]] += valueGetter(state) * bsccEquationSystemSolution[indexInStatesInBsccs[state]];
                        }
                    }
                    
                    for (uint_fast64_t bsccIndex = 0; bsccIndex < bsccDecomposition.size(); ++bsccIndex) {
                        STORM_LOG_DEBUG("Found LRA " << bsccLra[bsccIndex] << " for BSCC " << bsccIndex << ".");
                    }
                } else {
                    for (uint_fast64_t bsccIndex = 0; bsccIndex < bsccDecomposition.size(); ++bsccIndex) {
                        storm::storage::StronglyConnectedComponent const& bscc = bsccDecomposition[bsccIndex];
                        
                        // At this point, all BSCCs are known to contain exactly one state, which is why we can set all values
                        // directly (based on whether or not the contained state is a psi state).
                        bsccLra[bsccIndex] = valueGetter(*bscc.begin());
                    }
                    
                    for (uint_fast64_t bsccIndex = 0; bsccIndex < bsccDecomposition.size(); ++bsccIndex) {
                        STORM_LOG_DEBUG("Found LRA " << bsccLra[bsccIndex] << " for BSCC " << bsccIndex << ".");
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
                        ValueType reward = zero;
                        for (auto entry : probabilityMatrix.getRow(state)) {
                            if (statesInBsccs.get(entry.getColumn())) {
                                reward += entry.getValue() * bsccLra[stateToBsccIndexMap[indexInStatesInBsccs[entry.getColumn()]]];
                            }
                        }
                        rewardRightSide.push_back(reward);
                    }
                    
                    storm::storage::SparseMatrix<ValueType> rewardEquationSystemMatrix = probabilityMatrix.getSubmatrix(false, statesNotInBsccs, statesNotInBsccs, true);
                    rewardEquationSystemMatrix.convertToEquationSystem();
                    
                    rewardSolution = std::vector<ValueType>(rewardEquationSystemMatrix.getColumnCount(), one);
                    
                    {
                        storm::solver::GeneralLinearEquationSolverFactory<ValueType> linearEquationSolverFactory;
                        // Check solver requirements
                        auto requirements = linearEquationSolverFactory.getRequirements(env);
                        STORM_LOG_THROW(!requirements.hasEnabledCriticalRequirement(), storm::exceptions::UncheckedRequirementException, "Solver requirements " + requirements.getEnabledRequirementsAsString() + " not checked.");
                        // Check whether we have the right input format for the solver.
                        STORM_LOG_THROW(linearEquationSolverFactory.getEquationProblemFormat(env) == storm::solver::LinearEquationSolverProblemFormat::EquationSystem, storm::exceptions::FormatUnsupportedBySolverException, "The selected solver does not support the required format.");
                        std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = linearEquationSolverFactory.create(env, std::move(rewardEquationSystemMatrix));
                        solver->solveEquations(env, rewardSolution, rewardRightSide);
                    }
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
            std::vector<ValueType> SparseCtmcCslHelper::computeTransientProbabilities(Environment const& env, storm::storage::SparseMatrix<ValueType> const& uniformizedMatrix, std::vector<ValueType> const* addVector, ValueType timeBound, ValueType uniformizationRate, std::vector<ValueType> values) {
                
                ValueType lambda = timeBound * uniformizationRate;
                
                // If no time can pass, the current values are the result.
                if (storm::utility::isZero(lambda)) {
                    return values;
                }
                
                // Use Fox-Glynn to get the truncation points and the weights.
//                std::tuple<uint_fast64_t, uint_fast64_t, ValueType, std::vector<ValueType>> foxGlynnResult = storm::utility::numerical::getFoxGlynnCutoff(lambda, 1e+300, storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision() / 8.0);
                
                storm::utility::numerical::FoxGlynnResult<ValueType> foxGlynnResult = storm::utility::numerical::foxGlynn(lambda, storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision() / 8.0);
                STORM_LOG_DEBUG("Fox-Glynn cutoff points: left=" << foxGlynnResult.left << ", right=" << foxGlynnResult.right);
                
                // Scale the weights so they add up to one.
                for (auto& element : foxGlynnResult.weights) {
                    element /= foxGlynnResult.totalWeight;
                }
                
                // If the cumulative reward is to be computed, we need to adjust the weights.
                if (useMixedPoissonProbabilities) {
                    ValueType sum = storm::utility::zero<ValueType>();
                    
                    for (auto& element : foxGlynnResult.weights) {
                        sum += element;
                        element = (1 - sum) / uniformizationRate;
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

            template std::vector<double> SparseCtmcCslHelper::computeNextProbabilities(Environment const& env, storm::storage::SparseMatrix<double> const& rateMatrix, std::vector<double> const& exitRateVector, storm::storage::BitVector const& nextStates);
            
            template std::vector<double> SparseCtmcCslHelper::computeInstantaneousRewards(Environment const& env, storm::solver::SolveGoal<double>&& goal, storm::storage::SparseMatrix<double> const& rateMatrix, std::vector<double> const& exitRateVector, storm::models::sparse::StandardRewardModel<double> const& rewardModel, double timeBound);
            
            template std::vector<double> SparseCtmcCslHelper::computeReachabilityTimes(Environment const& env, storm::solver::SolveGoal<double>&& goal, storm::storage::SparseMatrix<double> const& rateMatrix, storm::storage::SparseMatrix<double> const& backwardTransitions, std::vector<double> const& exitRateVector, storm::storage::BitVector const& targetStates, bool qualitative);
            
            template std::vector<double> SparseCtmcCslHelper::computeReachabilityRewards(Environment const& env, storm::solver::SolveGoal<double>&& goal, storm::storage::SparseMatrix<double> const& rateMatrix, storm::storage::SparseMatrix<double> const& backwardTransitions, std::vector<double> const& exitRateVector, storm::models::sparse::StandardRewardModel<double> const& rewardModel, storm::storage::BitVector const& targetStates, bool qualitative);
            
            template std::vector<double> SparseCtmcCslHelper::computeTotalRewards(Environment const& env, storm::solver::SolveGoal<double>&& goal, storm::storage::SparseMatrix<double> const& rateMatrix, storm::storage::SparseMatrix<double> const& backwardTransitions, std::vector<double> const& exitRateVector, storm::models::sparse::StandardRewardModel<double> const& rewardModel, bool qualitative);
            
            template std::vector<double> SparseCtmcCslHelper::computeLongRunAverageProbabilities(Environment const& env, storm::solver::SolveGoal<double>&& goal, storm::storage::SparseMatrix<double> const& probabilityMatrix, storm::storage::BitVector const& psiStates, std::vector<double> const* exitRateVector);
            template std::vector<double> SparseCtmcCslHelper::computeLongRunAverageRewards(Environment const& env, storm::solver::SolveGoal<double>&& goal, storm::storage::SparseMatrix<double> const& probabilityMatrix, storm::models::sparse::StandardRewardModel<double> const& rewardModel, std::vector<double> const* exitRateVector);
            template std::vector<double> SparseCtmcCslHelper::computeLongRunAverageRewards(Environment const& env, storm::solver::SolveGoal<double>&& goal, storm::storage::SparseMatrix<double> const& probabilityMatrix, std::vector<double> const& stateRewardVector, std::vector<double> const* exitRateVector);
            
            template std::vector<double> SparseCtmcCslHelper::computeCumulativeRewards(Environment const& env, storm::solver::SolveGoal<double>&& goal, storm::storage::SparseMatrix<double> const& rateMatrix, std::vector<double> const& exitRateVector, storm::models::sparse::StandardRewardModel<double> const& rewardModel, double timeBound);
            
            template storm::storage::SparseMatrix<double> SparseCtmcCslHelper::computeUniformizedMatrix(storm::storage::SparseMatrix<double> const& rateMatrix, storm::storage::BitVector const& maybeStates, double uniformizationRate, std::vector<double> const& exitRates);
            
            template std::vector<double> SparseCtmcCslHelper::computeTransientProbabilities(Environment const& env, storm::storage::SparseMatrix<double> const& uniformizedMatrix, std::vector<double> const* addVector, double timeBound, double uniformizationRate, std::vector<double> values);

#ifdef STORM_HAVE_CARL
            template std::vector<storm::RationalNumber> SparseCtmcCslHelper::computeBoundedUntilProbabilities(Environment const& env, storm::solver::SolveGoal<storm::RationalNumber>&& goal, storm::storage::SparseMatrix<storm::RationalNumber> const& rateMatrix, storm::storage::SparseMatrix<storm::RationalNumber> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, std::vector<storm::RationalNumber> const& exitRates, bool qualitative, double lowerBound, double upperBound);
            template std::vector<storm::RationalFunction> SparseCtmcCslHelper::computeBoundedUntilProbabilities(Environment const& env, storm::solver::SolveGoal<storm::RationalFunction>&& goal, storm::storage::SparseMatrix<storm::RationalFunction> const& rateMatrix, storm::storage::SparseMatrix<storm::RationalFunction> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, std::vector<storm::RationalFunction> const& exitRates, bool qualitative, double lowerBound, double upperBound);

            template std::vector<storm::RationalNumber> SparseCtmcCslHelper::computeUntilProbabilities(Environment const& env, storm::solver::SolveGoal<storm::RationalNumber>&& goal, storm::storage::SparseMatrix<storm::RationalNumber> const& rateMatrix, storm::storage::SparseMatrix<storm::RationalNumber> const& backwardTransitions, std::vector<storm::RationalNumber> const& exitRateVector, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative);
            template std::vector<storm::RationalFunction> SparseCtmcCslHelper::computeUntilProbabilities(Environment const& env, storm::solver::SolveGoal<storm::RationalFunction>&& goal, storm::storage::SparseMatrix<storm::RationalFunction> const& rateMatrix, storm::storage::SparseMatrix<storm::RationalFunction> const& backwardTransitions, std::vector<storm::RationalFunction> const& exitRateVector, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative);

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

            template std::vector<storm::RationalNumber> SparseCtmcCslHelper::computeLongRunAverageProbabilities(Environment const& env, storm::solver::SolveGoal<storm::RationalNumber>&& goal, storm::storage::SparseMatrix<storm::RationalNumber> const& probabilityMatrix, storm::storage::BitVector const& psiStates, std::vector<storm::RationalNumber> const* exitRateVector);
            template std::vector<storm::RationalFunction> SparseCtmcCslHelper::computeLongRunAverageProbabilities(Environment const& env, storm::solver::SolveGoal<storm::RationalFunction>&& goal, storm::storage::SparseMatrix<storm::RationalFunction> const& probabilityMatrix, storm::storage::BitVector const& psiStates, std::vector<storm::RationalFunction> const* exitRateVector);
            
            template std::vector<storm::RationalNumber> SparseCtmcCslHelper::computeLongRunAverageRewards(Environment const& env, storm::solver::SolveGoal<storm::RationalNumber>&& goal, storm::storage::SparseMatrix<storm::RationalNumber> const& probabilityMatrix, storm::models::sparse::StandardRewardModel<RationalNumber> const& rewardModel, std::vector<storm::RationalNumber> const* exitRateVector);
            template std::vector<storm::RationalFunction> SparseCtmcCslHelper::computeLongRunAverageRewards(Environment const& env, storm::solver::SolveGoal<storm::RationalFunction>&& goal, storm::storage::SparseMatrix<storm::RationalFunction> const& probabilityMatrix, storm::models::sparse::StandardRewardModel<RationalFunction> const& rewardModel, std::vector<storm::RationalFunction> const* exitRateVector);

            template std::vector<storm::RationalNumber> SparseCtmcCslHelper::computeLongRunAverageRewards(Environment const& env, storm::solver::SolveGoal<storm::RationalNumber>&& goal, storm::storage::SparseMatrix<storm::RationalNumber> const& probabilityMatrix, std::vector<storm::RationalNumber> const& stateRewardVector, std::vector<storm::RationalNumber> const* exitRateVector);
            template std::vector<storm::RationalFunction> SparseCtmcCslHelper::computeLongRunAverageRewards(Environment const& env, storm::solver::SolveGoal<storm::RationalFunction>&& goal, storm::storage::SparseMatrix<storm::RationalFunction> const& probabilityMatrix, std::vector<storm::RationalFunction> const& stateRewardVector, std::vector<storm::RationalFunction> const* exitRateVector);

            template std::vector<storm::RationalNumber> SparseCtmcCslHelper::computeCumulativeRewards(Environment const& env, storm::solver::SolveGoal<storm::RationalNumber>&& goal, storm::storage::SparseMatrix<storm::RationalNumber> const& rateMatrix, std::vector<storm::RationalNumber> const& exitRateVector, storm::models::sparse::StandardRewardModel<storm::RationalNumber> const& rewardModel, double timeBound);
            template std::vector<storm::RationalFunction> SparseCtmcCslHelper::computeCumulativeRewards(Environment const& env, storm::solver::SolveGoal<storm::RationalFunction>&& goal, storm::storage::SparseMatrix<storm::RationalFunction> const& rateMatrix, std::vector<storm::RationalFunction> const& exitRateVector, storm::models::sparse::StandardRewardModel<storm::RationalFunction> const& rewardModel, double timeBound);

            template storm::storage::SparseMatrix<double> SparseCtmcCslHelper::computeProbabilityMatrix(storm::storage::SparseMatrix<double> const& rateMatrix, std::vector<double> const& exitRates);
            template storm::storage::SparseMatrix<storm::RationalNumber> SparseCtmcCslHelper::computeProbabilityMatrix(storm::storage::SparseMatrix<storm::RationalNumber> const& rateMatrix, std::vector<storm::RationalNumber> const& exitRates);
            template storm::storage::SparseMatrix<storm::RationalFunction> SparseCtmcCslHelper::computeProbabilityMatrix(storm::storage::SparseMatrix<storm::RationalFunction> const& rateMatrix, std::vector<storm::RationalFunction> const& exitRates);

#endif
        }
    }
}
