#include "storm/modelchecker/csl/helper/HybridMarkovAutomatonCslHelper.h"

#include "storm/modelchecker/csl/helper/SparseMarkovAutomatonCslHelper.h"
#include "storm/modelchecker/prctl/helper/HybridMdpPrctlHelper.h"

#include "storm/storage/dd/DdManager.h"
#include "storm/storage/dd/Add.h"
#include "storm/storage/dd/Bdd.h"

#include "storm/utility/macros.h"
#include "storm/utility/graph.h"
#include "storm/utility/constants.h"

#include "storm/models/symbolic/StandardRewardModel.h"

#include "storm/modelchecker/results/SymbolicQualitativeCheckResult.h"
#include "storm/modelchecker/results/SymbolicQuantitativeCheckResult.h"
#include "storm/modelchecker/results/HybridQuantitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "storm/utility/Stopwatch.h"

#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/exceptions/InvalidOperationException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template<storm::dd::DdType DdType, class ValueType>
            void discretizeRewardModel(typename storm::models::symbolic::Model<DdType, ValueType>::RewardModelType& rewardModel, storm::dd::Add<DdType, ValueType> const& exitRateVector, storm::dd::Bdd<DdType> const& markovianStates) {
                if (rewardModel.hasStateRewards()) {
                    rewardModel.getStateRewardVector() *= markovianStates.ite(exitRateVector.getDdManager().template getAddOne<ValueType>() / exitRateVector, exitRateVector.getDdManager().template getAddZero<ValueType>());
                }
            }
            
            
            template<storm::dd::DdType DdType, class ValueType>
            std::unique_ptr<CheckResult> HybridMarkovAutomatonCslHelper::computeReachabilityRewards(Environment const& env, OptimizationDirection dir, storm::models::symbolic::MarkovAutomaton<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, storm::dd::Bdd<DdType> const& markovianStates, storm::dd::Add<DdType, ValueType> const& exitRateVector, typename storm::models::symbolic::Model<DdType, ValueType>::RewardModelType const& rewardModel, storm::dd::Bdd<DdType> const& targetStates, bool qualitative) {
                
                auto discretizedRewardModel = rewardModel;
                discretizeRewardModel(discretizedRewardModel, exitRateVector, markovianStates);
                return HybridMdpPrctlHelper<DdType, ValueType>::computeReachabilityRewards(env, dir, model, transitionMatrix, discretizedRewardModel, targetStates, qualitative);
            }
            
            /*
            
            template<storm::dd::DdType DdType, class ValueType>
            std::unique_ptr<CheckResult> HybridMarkovAutomatonCslHelper::computeUntilProbabilities(Environment const& env, storm::models::symbolic::MarkovAutomaton<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& rateMatrix, storm::dd::Add<DdType, ValueType> const& exitRateVector, storm::dd::Bdd<DdType> const& phiStates, storm::dd::Bdd<DdType> const& psiStates, bool qualitative) {
                return HybridMdpPrctlHelper<DdType, ValueType>::computeUntilProbabilities(env, model, computeProbabilityMatrix(rateMatrix, exitRateVector), phiStates, psiStates, qualitative);
            }
            
            template<storm::dd::DdType DdType, typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type>
            std::unique_ptr<CheckResult> HybridMarkovAutomatonCslHelper::computeBoundedUntilProbabilities(Environment const& env, storm::models::symbolic::MarkovAutomaton<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& rateMatrix, storm::dd::Add<DdType, ValueType> const& exitRateVector, storm::dd::Bdd<DdType> const& phiStates, storm::dd::Bdd<DdType> const& psiStates, bool qualitative, double lowerBound, double upperBound) {
                
                // If the time bounds are [0, inf], we rather call untimed reachability.
                if (storm::utility::isZero(lowerBound) && upperBound == storm::utility::infinity<ValueType>()) {
                    return computeUntilProbabilities(env, model, rateMatrix, exitRateVector, phiStates, psiStates, qualitative);
                }
                
                // From this point on, we know that we have to solve a more complicated problem [t, t'] with either t != 0
                // or t' != inf.
                
                // If we identify the states that have probability 0 of reaching the target states, we can exclude them from the
                // further computations.
                storm::dd::Bdd<DdType> statesWithProbabilityGreater0 = storm::utility::graph::performProbGreater0(model, rateMatrix.notZero(), phiStates, psiStates);
                STORM_LOG_INFO("Found " << statesWithProbabilityGreater0.getNonZeroCount() << " states with probability greater 0.");
                storm::dd::Bdd<DdType> statesWithProbabilityGreater0NonPsi = statesWithProbabilityGreater0 && !psiStates;
                STORM_LOG_INFO("Found " << statesWithProbabilityGreater0NonPsi.getNonZeroCount() << " 'maybe' states.");
                
                if (!statesWithProbabilityGreater0NonPsi.isZero()) {
                    if (storm::utility::isZero(upperBound)) {
                        // In this case, the interval is of the form [0, 0].
                        return std::unique_ptr<CheckResult>(new SymbolicQuantitativeCheckResult<DdType, ValueType>(model.getReachableStates(), psiStates.template toAdd<ValueType>()));
                    } else {
                        if (storm::utility::isZero(lowerBound)) {
                            // In this case, the interval is of the form [0, t].
                            // Note that this excludes [0, inf] since this is untimed reachability and we considered this case earlier.
                            
                            // Find the maximal rate of all 'maybe' states to take it as the uniformization rate.
                            ValueType uniformizationRate =  1.02 * (statesWithProbabilityGreater0NonPsi.template toAdd<ValueType>() * exitRateVector).getMax();
                            STORM_LOG_THROW(uniformizationRate > 0, storm::exceptions::InvalidStateException, "The uniformization rate must be positive.");
                            
                            // Compute the uniformized matrix.
                            storm::dd::Add<DdType, ValueType> uniformizedMatrix = computeUniformizedMatrix(model, rateMatrix, exitRateVector, statesWithProbabilityGreater0NonPsi, uniformizationRate);
                            
                            // Compute the vector that is to be added as a compensation for removing the absorbing states.
                            storm::dd::Add<DdType, ValueType> b = (statesWithProbabilityGreater0NonPsi.template toAdd<ValueType>() * rateMatrix * psiStates.swapVariables(model.getRowColumnMetaVariablePairs()).template toAdd<ValueType>()).sumAbstract(model.getColumnVariables()) / model.getManager().getConstant(uniformizationRate);
                            
                            storm::utility::Stopwatch conversionWatch(true);
                            
                            // Create an ODD for the translation to an explicit representation.
                            storm::dd::Odd odd = statesWithProbabilityGreater0NonPsi.createOdd();
                            
                            // Convert the symbolic parts to their explicit representation.
                            storm::storage::SparseMatrix<ValueType> explicitUniformizedMatrix = uniformizedMatrix.toMatrix(odd, odd);
                            std::vector<ValueType> explicitB = b.toVector(odd);
                            conversionWatch.stop();
                            STORM_LOG_INFO("Converting symbolic matrix/vector to explicit representation done in " << conversionWatch.getTimeInMilliseconds() << "ms.");
                            
                            // Finally compute the transient probabilities.
                            std::vector<ValueType> values(statesWithProbabilityGreater0NonPsi.getNonZeroCount(), storm::utility::zero<ValueType>());
                            std::vector<ValueType> subresult = storm::modelchecker::helper::SparseMarkovAutomatonCslHelper::computeTransientProbabilities(env, explicitUniformizedMatrix, &explicitB, upperBound, uniformizationRate, values);
                            
                            return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType>(model.getReachableStates(),
                                                                                                          (psiStates || !statesWithProbabilityGreater0) && model.getReachableStates(),
                                                                                                          psiStates.template toAdd<ValueType>(), statesWithProbabilityGreater0NonPsi, odd, subresult));
                        } else if (upperBound == storm::utility::infinity<ValueType>()) {
                            // In this case, the interval is of the form [t, inf] with t != 0.
                            
                            // Start by computing the (unbounded) reachability probabilities of reaching psi states while
                            // staying in phi states.
                            std::unique_ptr<CheckResult> unboundedResult = computeUntilProbabilities(env, model, rateMatrix, exitRateVector, phiStates, psiStates, qualitative);
                            
                            // Compute the set of relevant states.
                            storm::dd::Bdd<DdType> relevantStates = statesWithProbabilityGreater0 && phiStates;
                            
                            // Filter the unbounded result such that it only contains values for the relevant states.
                            unboundedResult->filter(SymbolicQualitativeCheckResult<DdType>(model.getReachableStates(), relevantStates));
                            
                            storm::utility::Stopwatch conversionWatch;
                            
                            // Build an ODD for the relevant states.
                            conversionWatch.start();
                            storm::dd::Odd odd = relevantStates.createOdd();
                            conversionWatch.stop();
                            
                            std::vector<ValueType> result;
                            if (unboundedResult->isHybridQuantitativeCheckResult()) {
                                conversionWatch.start();
                                std::unique_ptr<CheckResult> explicitUnboundedResult = unboundedResult->asHybridQuantitativeCheckResult<DdType, ValueType>().toExplicitQuantitativeCheckResult();
                                conversionWatch.stop();
                                result = std::move(explicitUnboundedResult->asExplicitQuantitativeCheckResult<ValueType>().getValueVector());
                            } else {
                                STORM_LOG_THROW(unboundedResult->isSymbolicQuantitativeCheckResult(), storm::exceptions::InvalidStateException, "Expected check result of different type.");
                                result = unboundedResult->asSymbolicQuantitativeCheckResult<DdType, ValueType>().getValueVector().toVector(odd);
                            }
                            
                            // Determine the uniformization rate for the transient probability computation.
                            ValueType uniformizationRate = 1.02 * (relevantStates.template toAdd<ValueType>() * exitRateVector).getMax();
                            
                            // Compute the uniformized matrix.
                            storm::dd::Add<DdType, ValueType> uniformizedMatrix = computeUniformizedMatrix(model, rateMatrix, exitRateVector, relevantStates, uniformizationRate);
                            conversionWatch.start();
                            storm::storage::SparseMatrix<ValueType> explicitUniformizedMatrix = uniformizedMatrix.toMatrix(odd, odd);
                            conversionWatch.stop();
                            STORM_LOG_INFO("Converting symbolic matrix/vector to explicit representation done in " << conversionWatch.getTimeInMilliseconds() << "ms.");

                            // Compute the transient probabilities.
                            result = storm::modelchecker::helper::SparseMarkovAutomatonCslHelper::computeTransientProbabilities<ValueType>(env, explicitUniformizedMatrix, nullptr, lowerBound, uniformizationRate, result);
                            
                            return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType>(model.getReachableStates(), !relevantStates && model.getReachableStates(), model.getManager().template getAddZero<ValueType>(), relevantStates, odd, result));
                        } else {
                            // In this case, the interval is of the form [t, t'] with t != 0 and t' != inf.
                            
                            if (lowerBound != upperBound) {
                                // In this case, the interval is of the form [t, t'] with t != 0, t' != inf and t != t'.
                                
                                // Find the maximal rate of all 'maybe' states to take it as the uniformization rate.
                                ValueType uniformizationRate =  1.02 * (statesWithProbabilityGreater0NonPsi.template toAdd<ValueType>() * exitRateVector).getMax();
                                STORM_LOG_THROW(uniformizationRate > 0, storm::exceptions::InvalidStateException, "The uniformization rate must be positive.");
                                
                                // Compute the (first) uniformized matrix.
                                storm::dd::Add<DdType, ValueType> uniformizedMatrix = computeUniformizedMatrix(model, rateMatrix, exitRateVector, statesWithProbabilityGreater0NonPsi, uniformizationRate);
                                
                                // Create the one-step vector.
                                storm::dd::Add<DdType, ValueType> b = (statesWithProbabilityGreater0NonPsi.template toAdd<ValueType>() * rateMatrix * psiStates.swapVariables(model.getRowColumnMetaVariablePairs()).template toAdd<ValueType>()).sumAbstract(model.getColumnVariables()) / model.getManager().getConstant(uniformizationRate);
                                
                                // Build an ODD for the relevant states and translate the symbolic parts to their explicit representation.
                                storm::utility::Stopwatch conversionWatch(true);
                                storm::dd::Odd odd = statesWithProbabilityGreater0NonPsi.createOdd();
                                storm::storage::SparseMatrix<ValueType> explicitUniformizedMatrix = uniformizedMatrix.toMatrix(odd, odd);
                                std::vector<ValueType> explicitB = b.toVector(odd);
                                conversionWatch.stop();

                                // Compute the transient probabilities.
                                std::vector<ValueType> values(statesWithProbabilityGreater0NonPsi.getNonZeroCount(), storm::utility::zero<ValueType>());
                                std::vector<ValueType> subResult = storm::modelchecker::helper::SparseMarkovAutomatonCslHelper::computeTransientProbabilities(env, explicitUniformizedMatrix, &explicitB, upperBound - lowerBound, uniformizationRate, values);
                                
                                // Transform the explicit result to a hybrid check result, so we can easily convert it to
                                // a symbolic qualitative format.
                                HybridQuantitativeCheckResult<DdType> hybridResult(model.getReachableStates(), psiStates || (!statesWithProbabilityGreater0 && model.getReachableStates()),
                                                                                   psiStates.template toAdd<ValueType>(), statesWithProbabilityGreater0NonPsi, odd, subResult);
                                
                                // Compute the set of relevant states.
                                storm::dd::Bdd<DdType> relevantStates = statesWithProbabilityGreater0 && phiStates;
                                
                                // Filter the unbounded result such that it only contains values for the relevant states.
                                hybridResult.filter(SymbolicQualitativeCheckResult<DdType>(model.getReachableStates(), relevantStates));
                                                                
                                // Build an ODD for the relevant states.
                                conversionWatch.start();
                                odd = relevantStates.createOdd();
                                
                                std::unique_ptr<CheckResult> explicitResult = hybridResult.toExplicitQuantitativeCheckResult();
                                conversionWatch.stop();
                                std::vector<ValueType> newSubresult = std::move(explicitResult->asExplicitQuantitativeCheckResult<ValueType>().getValueVector());

                                // Then compute the transient probabilities of being in such a state after t time units. For this,
                                // we must re-uniformize the MarkovAutomaton, so we need to compute the second uniformized matrix.
                                uniformizationRate =  1.02 * (relevantStates.template toAdd<ValueType>() * exitRateVector).getMax();
                                STORM_LOG_THROW(uniformizationRate > 0, storm::exceptions::InvalidStateException, "The uniformization rate must be positive.");
                                
                                // If the lower and upper bounds coincide, we have only determined the relevant states at this
                                // point, but we still need to construct the starting vector.
                                if (lowerBound == upperBound) {
                                    odd = relevantStates.createOdd();
                                    newSubresult = psiStates.template toAdd<ValueType>().toVector(odd);
                                }
                                
                                // Finally, we compute the second set of transient probabilities.
                                uniformizedMatrix = computeUniformizedMatrix(model, rateMatrix, exitRateVector, relevantStates, uniformizationRate);
                                conversionWatch.start();
                                explicitUniformizedMatrix = uniformizedMatrix.toMatrix(odd, odd);
                                conversionWatch.stop();
                                STORM_LOG_INFO("Converting symbolic matrix/vector to explicit representation done in " << conversionWatch.getTimeInMilliseconds() << "ms.");

                                newSubresult = storm::modelchecker::helper::SparseMarkovAutomatonCslHelper::computeTransientProbabilities<ValueType>(env, explicitUniformizedMatrix, nullptr, lowerBound, uniformizationRate, newSubresult);
                                
                                return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType>(model.getReachableStates(), !relevantStates && model.getReachableStates(), model.getManager().template getAddZero<ValueType>(), relevantStates, odd, newSubresult));
                            } else {
                                // In this case, the interval is of the form [t, t] with t != 0, t != inf.
                                
                                storm::utility::Stopwatch conversionWatch;
                                
                                // Build an ODD for the relevant states.
                                conversionWatch.start();
                                storm::dd::Odd odd = statesWithProbabilityGreater0.createOdd();

                                std::vector<ValueType> newSubresult = psiStates.template toAdd<ValueType>().toVector(odd);
                                conversionWatch.stop();

                                // Then compute the transient probabilities of being in such a state after t time units. For this,
                                // we must re-uniformize the MarkovAutomaton, so we need to compute the second uniformized matrix.
                                ValueType uniformizationRate =  1.02 * (statesWithProbabilityGreater0.template toAdd<ValueType>() * exitRateVector).getMax();
                                STORM_LOG_THROW(uniformizationRate > 0, storm::exceptions::InvalidStateException, "The uniformization rate must be positive.");
                                
                                // Finally, we compute the second set of transient probabilities.
                                storm::dd::Add<DdType, ValueType> uniformizedMatrix = computeUniformizedMatrix(model, rateMatrix, exitRateVector, statesWithProbabilityGreater0, uniformizationRate);
                                conversionWatch.start();
                                storm::storage::SparseMatrix<ValueType> explicitUniformizedMatrix = uniformizedMatrix.toMatrix(odd, odd);
                                conversionWatch.stop();
                                STORM_LOG_INFO("Converting symbolic matrix/vector to explicit representation done in " << conversionWatch.getTimeInMilliseconds() << "ms.");

                                newSubresult = storm::modelchecker::helper::SparseMarkovAutomatonCslHelper::computeTransientProbabilities<ValueType>(env, explicitUniformizedMatrix, nullptr, lowerBound, uniformizationRate, newSubresult);
                                
                                return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType>(model.getReachableStates(), !statesWithProbabilityGreater0 && model.getReachableStates(), model.getManager().template getAddZero<ValueType>(), statesWithProbabilityGreater0, odd, newSubresult));
                            }
                        }
                    }
                } else {
                    return std::unique_ptr<CheckResult>(new SymbolicQuantitativeCheckResult<DdType>(model.getReachableStates(), psiStates.template toAdd<ValueType>()));
                }
            }
            
            template<storm::dd::DdType DdType, typename ValueType, typename std::enable_if<!storm::NumberTraits<ValueType>::SupportsExponential, int>::type>
            std::unique_ptr<CheckResult> HybridMarkovAutomatonCslHelper::computeBoundedUntilProbabilities(Environment const& env, storm::models::symbolic::MarkovAutomaton<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& rateMatrix, storm::dd::Add<DdType, ValueType> const& exitRateVector, storm::dd::Bdd<DdType> const& phiStates, storm::dd::Bdd<DdType> const& psiStates, bool qualitative, double lowerBound, double upperBound) {
                STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Computing bounded until probabilities is unsupported for this value type.");
            }
            
            template<storm::dd::DdType DdType, class ValueType>
            std::unique_ptr<CheckResult> HybridMarkovAutomatonCslHelper::computeLongRunAverageProbabilities(Environment const& env, storm::models::symbolic::MarkovAutomaton<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& rateMatrix, storm::dd::Add<DdType, ValueType> const& exitRateVector, storm::dd::Bdd<DdType> const& psiStates) {
                
                storm::utility::Stopwatch conversionWatch(true);
                
                // Create ODD for the translation.
                storm::dd::Odd odd = model.getReachableStates().createOdd();
                
                storm::storage::SparseMatrix<ValueType> explicitRateMatrix = rateMatrix.toMatrix(odd, odd);
                std::vector<ValueType> explicitExitRateVector = exitRateVector.toVector(odd);
                conversionWatch.stop();
                STORM_LOG_INFO("Converting symbolic matrix/vector to explicit representation done in " << conversionWatch.getTimeInMilliseconds() << "ms.");

                std::vector<ValueType> result = storm::modelchecker::helper::SparseMarkovAutomatonCslHelper::computeLongRunAverageProbabilities(env, storm::solver::SolveGoal<ValueType>(), explicitRateMatrix, psiStates.toVector(odd), &explicitExitRateVector);

                return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType, ValueType>(model.getReachableStates(), model.getManager().getBddZero(), model.getManager().template getAddZero<ValueType>(), model.getReachableStates(), std::move(odd), std::move(result)));
            }
            
            template<storm::dd::DdType DdType, class ValueType>
            std::unique_ptr<CheckResult> HybridMarkovAutomatonCslHelper::computeLongRunAverageRewards(Environment const& env, storm::models::symbolic::MarkovAutomaton<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& rateMatrix, storm::dd::Add<DdType, ValueType> const& exitRateVector, typename storm::models::symbolic::Model<DdType, ValueType>::RewardModelType const& rewardModel) {
                
                STORM_LOG_THROW(!rewardModel.empty(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
                storm::dd::Add<DdType, ValueType> probabilityMatrix = computeProbabilityMatrix(rateMatrix, exitRateVector);
                
                storm::utility::Stopwatch conversionWatch(true);
                
                // Create ODD for the translation.
                storm::dd::Odd odd = model.getReachableStates().createOdd();
                
                storm::storage::SparseMatrix<ValueType> explicitRateMatrix = rateMatrix.toMatrix(odd, odd);
                std::vector<ValueType> explicitExitRateVector = exitRateVector.toVector(odd);
                conversionWatch.stop();
                STORM_LOG_INFO("Converting symbolic matrix/vector to explicit representation done in " << conversionWatch.getTimeInMilliseconds() << "ms.");

                std::vector<ValueType> result = storm::modelchecker::helper::SparseMarkovAutomatonCslHelper::computeLongRunAverageRewards(env, storm::solver::SolveGoal<ValueType>(), explicitRateMatrix, rewardModel.getTotalRewardVector(probabilityMatrix, model.getColumnVariables(), exitRateVector, true).toVector(odd), &explicitExitRateVector);
                
                return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType, ValueType>(model.getReachableStates(), model.getManager().getBddZero(), model.getManager().template getAddZero<ValueType>(), model.getReachableStates(), std::move(odd), std::move(result)));
            }
            */
 
            // Explicit instantiations.
            
            // Cudd, double.
            template std::unique_ptr<CheckResult> HybridMarkovAutomatonCslHelper::computeReachabilityRewards(Environment const& env, OptimizationDirection dir, storm::models::symbolic::MarkovAutomaton<storm::dd::DdType::CUDD, double> const& model, storm::dd::Add<storm::dd::DdType::CUDD, double> const& transitionMatrix, storm::dd::Bdd<storm::dd::DdType::CUDD> const& markovianStates, storm::dd::Add<storm::dd::DdType::CUDD, double> const& exitRateVector, typename storm::models::symbolic::Model<storm::dd::DdType::CUDD, double>::RewardModelType const& rewardModel, storm::dd::Bdd<storm::dd::DdType::CUDD> const& targetStates, bool qualitative);
            /*
            template std::unique_ptr<CheckResult> HybridMarkovAutomatonCslHelper::computeBoundedUntilProbabilities(Environment const& env, storm::models::symbolic::MarkovAutomaton<storm::dd::DdType::CUDD, double> const& model, storm::dd::Add<storm::dd::DdType::CUDD, double> const& rateMatrix, storm::dd::Add<storm::dd::DdType::CUDD, double> const& exitRateVector, storm::dd::Bdd<storm::dd::DdType::CUDD> const& phiStates, storm::dd::Bdd<storm::dd::DdType::CUDD> const& psiStates, bool qualitative, double lowerBound, double upperBound);
            template std::unique_ptr<CheckResult> HybridMarkovAutomatonCslHelper::computeUntilProbabilities(Environment const& env, storm::models::symbolic::MarkovAutomaton<storm::dd::DdType::CUDD, double> const& model, storm::dd::Add<storm::dd::DdType::CUDD, double> const& rateMatrix, storm::dd::Add<storm::dd::DdType::CUDD, double> const& exitRateVector, storm::dd::Bdd<storm::dd::DdType::CUDD> const& phiStates, storm::dd::Bdd<storm::dd::DdType::CUDD> const& psiStates, bool qualitative);
            template std::unique_ptr<CheckResult> HybridMarkovAutomatonCslHelper::computeLongRunAverageProbabilities(Environment const& env, storm::models::symbolic::MarkovAutomaton<storm::dd::DdType::CUDD, double> const& model, storm::dd::Add<storm::dd::DdType::CUDD, double> const& rateMatrix, storm::dd::Add<storm::dd::DdType::CUDD, double> const& exitRateVector, storm::dd::Bdd<storm::dd::DdType::CUDD> const& psiStates);
            template std::unique_ptr<CheckResult> HybridMarkovAutomatonCslHelper::computeLongRunAverageRewards(Environment const& env, storm::models::symbolic::MarkovAutomaton<storm::dd::DdType::CUDD, double> const& model, storm::dd::Add<storm::dd::DdType::CUDD, double> const& rateMatrix, storm::dd::Add<storm::dd::DdType::CUDD, double> const& exitRateVector, typename storm::models::symbolic::Model<storm::dd::DdType::CUDD, double>::RewardModelType const& rewardModel);
*/
            // Sylvan, double.
            template std::unique_ptr<CheckResult> HybridMarkovAutomatonCslHelper::computeReachabilityRewards(Environment const& env, OptimizationDirection dir, storm::models::symbolic::MarkovAutomaton<storm::dd::DdType::Sylvan, double> const& model, storm::dd::Add<storm::dd::DdType::Sylvan, double> const& transitionMatrix, storm::dd::Bdd<storm::dd::DdType::Sylvan> const& markovianStates, storm::dd::Add<storm::dd::DdType::Sylvan, double> const& exitRateVector, typename storm::models::symbolic::Model<storm::dd::DdType::Sylvan, double>::RewardModelType const& rewardModel, storm::dd::Bdd<storm::dd::DdType::Sylvan> const& targetStates, bool qualitative);
  /*          template std::unique_ptr<CheckResult> HybridMarkovAutomatonCslHelper::computeBoundedUntilProbabilities(Environment const& env, storm::models::symbolic::MarkovAutomaton<storm::dd::DdType::Sylvan, double> const& model, storm::dd::Add<storm::dd::DdType::Sylvan, double> const& rateMatrix, storm::dd::Add<storm::dd::DdType::Sylvan, double> const& exitRateVector, storm::dd::Bdd<storm::dd::DdType::Sylvan> const& phiStates, storm::dd::Bdd<storm::dd::DdType::Sylvan> const& psiStates, bool qualitative, double lowerBound, double upperBound);
            template std::unique_ptr<CheckResult> HybridMarkovAutomatonCslHelper::computeUntilProbabilities(Environment const& env, storm::models::symbolic::MarkovAutomaton<storm::dd::DdType::Sylvan, double> const& model, storm::dd::Add<storm::dd::DdType::Sylvan, double> const& rateMatrix, storm::dd::Add<storm::dd::DdType::Sylvan, double> const& exitRateVector, storm::dd::Bdd<storm::dd::DdType::Sylvan> const& phiStates, storm::dd::Bdd<storm::dd::DdType::Sylvan> const& psiStates, bool qualitative);
            template std::unique_ptr<CheckResult> HybridMarkovAutomatonCslHelper::computeLongRunAverageProbabilities(Environment const& env, storm::models::symbolic::MarkovAutomaton<storm::dd::DdType::Sylvan, double> const& model, storm::dd::Add<storm::dd::DdType::Sylvan, double> const& rateMatrix, storm::dd::Add<storm::dd::DdType::Sylvan, double> const& exitRateVector, storm::dd::Bdd<storm::dd::DdType::Sylvan> const& psiStates);
            template std::unique_ptr<CheckResult> HybridMarkovAutomatonCslHelper::computeLongRunAverageRewards(Environment const& env, storm::models::symbolic::MarkovAutomaton<storm::dd::DdType::Sylvan, double> const& model, storm::dd::Add<storm::dd::DdType::Sylvan, double> const& rateMatrix, storm::dd::Add<storm::dd::DdType::Sylvan, double> const& exitRateVector, typename storm::models::symbolic::Model<storm::dd::DdType::Sylvan, double>::RewardModelType const& rewardModel);
*/
            // Sylvan, rational number.
            template std::unique_ptr<CheckResult> HybridMarkovAutomatonCslHelper::computeReachabilityRewards(Environment const& env, OptimizationDirection dir, storm::models::symbolic::MarkovAutomaton<storm::dd::DdType::Sylvan, storm::RationalNumber> const& model, storm::dd::Add<storm::dd::DdType::Sylvan, storm::RationalNumber> const& transitionMatrix, storm::dd::Bdd<storm::dd::DdType::Sylvan> const& markovianStates, storm::dd::Add<storm::dd::DdType::Sylvan, storm::RationalNumber> const& exitRateVector, typename storm::models::symbolic::Model<storm::dd::DdType::Sylvan, storm::RationalNumber>::RewardModelType const& rewardModel, storm::dd::Bdd<storm::dd::DdType::Sylvan> const& targetStates, bool qualitative);
  /*          template std::unique_ptr<CheckResult> HybridMarkovAutomatonCslHelper::computeBoundedUntilProbabilities(Environment const& env, storm::models::symbolic::MarkovAutomaton<storm::dd::DdType::Sylvan, storm::RationalNumber> const& model, storm::dd::Add<storm::dd::DdType::Sylvan, storm::RationalNumber> const& rateMatrix, storm::dd::Add<storm::dd::DdType::Sylvan, storm::RationalNumber> const& exitRateVector, storm::dd::Bdd<storm::dd::DdType::Sylvan> const& phiStates, storm::dd::Bdd<storm::dd::DdType::Sylvan> const& psiStates, bool qualitative, double lowerBound, double upperBound);
            template std::unique_ptr<CheckResult> HybridMarkovAutomatonCslHelper::computeUntilProbabilities(Environment const& env, storm::models::symbolic::MarkovAutomaton<storm::dd::DdType::Sylvan, storm::RationalNumber> const& model, storm::dd::Add<storm::dd::DdType::Sylvan, storm::RationalNumber> const& rateMatrix, storm::dd::Add<storm::dd::DdType::Sylvan, storm::RationalNumber> const& exitRateVector, storm::dd::Bdd<storm::dd::DdType::Sylvan> const& phiStates, storm::dd::Bdd<storm::dd::DdType::Sylvan> const& psiStates, bool qualitative);
            template std::unique_ptr<CheckResult> HybridMarkovAutomatonCslHelper::computeLongRunAverageProbabilities(Environment const& env, storm::models::symbolic::MarkovAutomaton<storm::dd::DdType::Sylvan, storm::RationalNumber> const& model, storm::dd::Add<storm::dd::DdType::Sylvan, storm::RationalNumber> const& rateMatrix, storm::dd::Add<storm::dd::DdType::Sylvan, storm::RationalNumber> const& exitRateVector, storm::dd::Bdd<storm::dd::DdType::Sylvan> const& psiStates);
            template std::unique_ptr<CheckResult> HybridMarkovAutomatonCslHelper::computeLongRunAverageRewards(Environment const& env, storm::models::symbolic::MarkovAutomaton<storm::dd::DdType::Sylvan, storm::RationalNumber> const& model, storm::dd::Add<storm::dd::DdType::Sylvan, storm::RationalNumber> const& rateMatrix, storm::dd::Add<storm::dd::DdType::Sylvan, storm::RationalNumber> const& exitRateVector, typename storm::models::symbolic::Model<storm::dd::DdType::Sylvan, storm::RationalNumber>::RewardModelType const& rewardModel);
*/

        }
    }
}
