#include "src/modelchecker/csl/helper/HybridCtmcCslHelper.h"

#include "src/modelchecker/csl/helper/SparseCtmcCslHelper.h"
#include "src/modelchecker/prctl/helper/HybridDtmcPrctlHelper.h"

#include "src/storage/dd/CuddDdManager.h"
#include "src/storage/dd/CuddAdd.h"
#include "src/storage/dd/CuddBdd.h"
#include "src/storage/dd/CuddOdd.h"

#include "src/utility/macros.h"
#include "src/utility/graph.h"

#include "src/models/symbolic/StandardRewardModel.h"

#include "src/modelchecker/results/SymbolicQualitativeCheckResult.h"
#include "src/modelchecker/results/SymbolicQuantitativeCheckResult.h"
#include "src/modelchecker/results/HybridQuantitativeCheckResult.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template<storm::dd::DdType DdType, class ValueType>
            std::unique_ptr<CheckResult> HybridCtmcCslHelper<DdType, ValueType>::computeReachabilityRewards(storm::models::symbolic::Model<DdType> const& model, storm::dd::Add<DdType> const& rateMatrix, storm::dd::Add<DdType> const& exitRateVector, RewardModelType const& rewardModel, storm::dd::Bdd<DdType> const& targetStates, bool qualitative, storm::utility::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                
                return HybridDtmcPrctlHelper<DdType, ValueType>::computeReachabilityRewards(model, computeProbabilityMatrix(model, rateMatrix, exitRateVector), rewardModel.divideStateRewardVector(exitRateVector), targetStates, qualitative, linearEquationSolverFactory);
            }
            
            template<storm::dd::DdType DdType, class ValueType>
            std::unique_ptr<CheckResult> HybridCtmcCslHelper<DdType, ValueType>::computeNextProbabilities(storm::models::symbolic::Model<DdType> const& model, storm::dd::Add<DdType> const& rateMatrix, storm::dd::Add<DdType> const& exitRateVector, storm::dd::Bdd<DdType> const& nextStates) {
                return HybridDtmcPrctlHelper<DdType, ValueType>::computeNextProbabilities(model, computeProbabilityMatrix(model, rateMatrix, exitRateVector), nextStates);
            }
            
            template<storm::dd::DdType DdType, class ValueType>
            std::unique_ptr<CheckResult> HybridCtmcCslHelper<DdType, ValueType>::computeUntilProbabilities(storm::models::symbolic::Model<DdType> const& model, storm::dd::Add<DdType> const& rateMatrix, storm::dd::Add<DdType> const& exitRateVector, storm::dd::Bdd<DdType> const& phiStates, storm::dd::Bdd<DdType> const& psiStates, bool qualitative, storm::utility::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                return HybridDtmcPrctlHelper<DdType, ValueType>::computeUntilProbabilities(model, computeProbabilityMatrix(model, rateMatrix, exitRateVector), phiStates, psiStates, qualitative, linearEquationSolverFactory);
            }
            
            template<storm::dd::DdType DdType, class ValueType>
            std::unique_ptr<CheckResult> HybridCtmcCslHelper<DdType, ValueType>::computeBoundedUntilProbabilities(storm::models::symbolic::Model<DdType> const& model, storm::dd::Add<DdType> const& rateMatrix, storm::dd::Add<DdType> const& exitRateVector, storm::dd::Bdd<DdType> const& phiStates, storm::dd::Bdd<DdType> const& psiStates, bool qualitative, double lowerBound, double upperBound, storm::utility::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                
                // If the time bounds are [0, inf], we rather call untimed reachability.
                if (lowerBound == storm::utility::zero<ValueType>() && upperBound == storm::utility::infinity<ValueType>()) {
                    return computeUntilProbabilities(model, rateMatrix, exitRateVector, phiStates, psiStates, qualitative, linearEquationSolverFactory);
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
                    if (upperBound == storm::utility::zero<ValueType>()) {
                        // In this case, the interval is of the form [0, 0].
                        return std::unique_ptr<CheckResult>(new SymbolicQuantitativeCheckResult<DdType>(model.getReachableStates(), psiStates.toAdd()));
                    } else {
                        if (lowerBound == storm::utility::zero<ValueType>()) {
                            // In this case, the interval is of the form [0, t].
                            // Note that this excludes [0, inf] since this is untimed reachability and we considered this case earlier.
                            
                            // Find the maximal rate of all 'maybe' states to take it as the uniformization rate.
                            ValueType uniformizationRate =  1.02 * (statesWithProbabilityGreater0NonPsi.toAdd() * exitRateVector).getMax();
                            STORM_LOG_THROW(uniformizationRate > 0, storm::exceptions::InvalidStateException, "The uniformization rate must be positive.");
                            
                            // Compute the uniformized matrix.
                            storm::dd::Add<DdType> uniformizedMatrix = computeUniformizedMatrix(model, rateMatrix, exitRateVector, statesWithProbabilityGreater0NonPsi, uniformizationRate);
                            
                            // Compute the vector that is to be added as a compensation for removing the absorbing states.
                            storm::dd::Add<DdType> b = (statesWithProbabilityGreater0NonPsi.toAdd() * rateMatrix * psiStates.swapVariables(model.getRowColumnMetaVariablePairs()).toAdd()).sumAbstract(model.getColumnVariables()) / model.getManager().getConstant(uniformizationRate);
                            
                            // Create an ODD for the translation to an explicit representation.
                            storm::dd::Odd<DdType> odd(statesWithProbabilityGreater0NonPsi);
                            
                            // Convert the symbolic parts to their explicit representation.
                            storm::storage::SparseMatrix<ValueType> explicitUniformizedMatrix = uniformizedMatrix.toMatrix(odd, odd);
                            std::vector<ValueType> explicitB = b.template toVector<ValueType>(odd);
                            
                            // Finally compute the transient probabilities.
                            std::vector<ValueType> values(statesWithProbabilityGreater0NonPsi.getNonZeroCount(), storm::utility::zero<ValueType>());
                            std::vector<ValueType> subresult = storm::modelchecker::helper::SparseCtmcCslHelper<ValueType>::computeTransientProbabilities(explicitUniformizedMatrix, &explicitB, upperBound, uniformizationRate, values, linearEquationSolverFactory);
                            
                            return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType>(model.getReachableStates(),
                                                                                                          (psiStates || !statesWithProbabilityGreater0) && model.getReachableStates(),
                                                                                                          psiStates.toAdd(), statesWithProbabilityGreater0NonPsi, odd, subresult));
                        } else if (upperBound == storm::utility::infinity<ValueType>()) {
                            // In this case, the interval is of the form [t, inf] with t != 0.
                            
                            // Start by computing the (unbounded) reachability probabilities of reaching psi states while
                            // staying in phi states.
                            std::unique_ptr<CheckResult> unboundedResult = computeUntilProbabilities(model, rateMatrix, exitRateVector, phiStates, psiStates, qualitative, linearEquationSolverFactory);
                            
                            // Compute the set of relevant states.
                            storm::dd::Bdd<DdType> relevantStates = statesWithProbabilityGreater0 && phiStates;
                            
                            // Filter the unbounded result such that it only contains values for the relevant states.
                            unboundedResult->filter(SymbolicQualitativeCheckResult<DdType>(model.getReachableStates(), relevantStates));
                            
                            // Build an ODD for the relevant states.
                            storm::dd::Odd<DdType> odd(relevantStates);
                            
                            std::vector<ValueType> result;
                            if (unboundedResult->isHybridQuantitativeCheckResult()) {
                                std::unique_ptr<CheckResult> explicitUnboundedResult = unboundedResult->asHybridQuantitativeCheckResult<DdType>().toExplicitQuantitativeCheckResult();
                                result = std::move(explicitUnboundedResult->asExplicitQuantitativeCheckResult<ValueType>().getValueVector());
                            } else {
                                STORM_LOG_THROW(unboundedResult->isSymbolicQuantitativeCheckResult(), storm::exceptions::InvalidStateException, "Expected check result of different type.");
                                result = unboundedResult->asSymbolicQuantitativeCheckResult<DdType>().getValueVector().template toVector<ValueType>(odd);
                            }
                            
                            // Determine the uniformization rate for the transient probability computation.
                            ValueType uniformizationRate = 1.02 * (relevantStates.toAdd() * exitRateVector).getMax();
                            
                            // Compute the uniformized matrix.
                            storm::dd::Add<DdType> uniformizedMatrix = computeUniformizedMatrix(model, rateMatrix, exitRateVector, relevantStates, uniformizationRate);
                            storm::storage::SparseMatrix<ValueType> explicitUniformizedMatrix = uniformizedMatrix.toMatrix(odd, odd);
                            
                            // Compute the transient probabilities.
                            result = storm::modelchecker::helper::SparseCtmcCslHelper<ValueType>::computeTransientProbabilities(explicitUniformizedMatrix, nullptr, lowerBound, uniformizationRate, result, linearEquationSolverFactory);
                            
                            return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType>(model.getReachableStates(), !relevantStates && model.getReachableStates(), model.getManager().getAddZero(), relevantStates, odd, result));
                        } else {
                            // In this case, the interval is of the form [t, t'] with t != 0 and t' != inf.
                            
                            if (lowerBound != upperBound) {
                                // In this case, the interval is of the form [t, t'] with t != 0, t' != inf and t != t'.
                                
                                // Find the maximal rate of all 'maybe' states to take it as the uniformization rate.
                                ValueType uniformizationRate =  1.02 * (statesWithProbabilityGreater0NonPsi.toAdd() * exitRateVector).getMax();
                                STORM_LOG_THROW(uniformizationRate > 0, storm::exceptions::InvalidStateException, "The uniformization rate must be positive.");
                                
                                // Compute the (first) uniformized matrix.
                                storm::dd::Add<DdType> uniformizedMatrix = computeUniformizedMatrix(model, rateMatrix, exitRateVector, statesWithProbabilityGreater0NonPsi, uniformizationRate);
                                
                                // Create the one-step vector.
                                storm::dd::Add<DdType> b = (statesWithProbabilityGreater0NonPsi.toAdd() * rateMatrix * psiStates.swapVariables(model.getRowColumnMetaVariablePairs()).toAdd()).sumAbstract(model.getColumnVariables()) / model.getManager().getConstant(uniformizationRate);
                                
                                // Build an ODD for the relevant states and translate the symbolic parts to their explicit representation.
                                storm::dd::Odd<DdType> odd = storm::dd::Odd<DdType>(statesWithProbabilityGreater0NonPsi);
                                storm::storage::SparseMatrix<ValueType> explicitUniformizedMatrix = uniformizedMatrix.toMatrix(odd, odd);
                                std::vector<ValueType> explicitB = b.template toVector<ValueType>(odd);
                                
                                // Compute the transient probabilities.
                                std::vector<ValueType> values(statesWithProbabilityGreater0NonPsi.getNonZeroCount(), storm::utility::zero<ValueType>());
                                std::vector<ValueType> subResult = storm::modelchecker::helper::SparseCtmcCslHelper<ValueType>::computeTransientProbabilities(explicitUniformizedMatrix, &explicitB, upperBound - lowerBound, uniformizationRate, values, linearEquationSolverFactory);
                                
                                // Transform the explicit result to a hybrid check result, so we can easily convert it to
                                // a symbolic qualitative format.
                                HybridQuantitativeCheckResult<DdType> hybridResult(model.getReachableStates(), psiStates || (!statesWithProbabilityGreater0 && model.getReachableStates()),
                                                                                   psiStates.toAdd(), statesWithProbabilityGreater0NonPsi, odd, subResult);
                                
                                // Compute the set of relevant states.
                                storm::dd::Bdd<DdType> relevantStates = statesWithProbabilityGreater0 && phiStates;
                                
                                // Filter the unbounded result such that it only contains values for the relevant states.
                                hybridResult.filter(SymbolicQualitativeCheckResult<DdType>(model.getReachableStates(), relevantStates));
                                
                                // Build an ODD for the relevant states.
                                odd = storm::dd::Odd<DdType>(relevantStates);
                                
                                std::unique_ptr<CheckResult> explicitResult = hybridResult.toExplicitQuantitativeCheckResult();
                                std::vector<ValueType> newSubresult = std::move(explicitResult->asExplicitQuantitativeCheckResult<ValueType>().getValueVector());
                                
                                // Then compute the transient probabilities of being in such a state after t time units. For this,
                                // we must re-uniformize the CTMC, so we need to compute the second uniformized matrix.
                                uniformizationRate =  1.02 * (relevantStates.toAdd() * exitRateVector).getMax();
                                STORM_LOG_THROW(uniformizationRate > 0, storm::exceptions::InvalidStateException, "The uniformization rate must be positive.");
                                
                                // If the lower and upper bounds coincide, we have only determined the relevant states at this
                                // point, but we still need to construct the starting vector.
                                if (lowerBound == upperBound) {
                                    odd = storm::dd::Odd<DdType>(relevantStates);
                                    newSubresult = psiStates.toAdd().template toVector<ValueType>(odd);
                                }
                                
                                // Finally, we compute the second set of transient probabilities.
                                uniformizedMatrix = computeUniformizedMatrix(model, rateMatrix, exitRateVector, relevantStates, uniformizationRate);
                                explicitUniformizedMatrix = uniformizedMatrix.toMatrix(odd, odd);
                                
                                newSubresult = storm::modelchecker::helper::SparseCtmcCslHelper<ValueType>::computeTransientProbabilities(explicitUniformizedMatrix, nullptr, lowerBound, uniformizationRate, newSubresult, linearEquationSolverFactory);
                                
                                return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType>(model.getReachableStates(), !relevantStates && model.getReachableStates(), model.getManager().getAddZero(), relevantStates, odd, newSubresult));
                            } else {
                                // In this case, the interval is of the form [t, t] with t != 0, t != inf.
                                
                                // Build an ODD for the relevant states.
                                storm::dd::Odd<DdType> odd = storm::dd::Odd<DdType>(statesWithProbabilityGreater0);
                                
                                std::vector<ValueType> newSubresult = psiStates.toAdd().template toVector<ValueType>(odd);
                                
                                // Then compute the transient probabilities of being in such a state after t time units. For this,
                                // we must re-uniformize the CTMC, so we need to compute the second uniformized matrix.
                                ValueType uniformizationRate =  1.02 * (statesWithProbabilityGreater0.toAdd() * exitRateVector).getMax();
                                STORM_LOG_THROW(uniformizationRate > 0, storm::exceptions::InvalidStateException, "The uniformization rate must be positive.");
                                
                                // Finally, we compute the second set of transient probabilities.
                                storm::dd::Add<DdType> uniformizedMatrix = computeUniformizedMatrix(model, rateMatrix, exitRateVector, statesWithProbabilityGreater0, uniformizationRate);
                                storm::storage::SparseMatrix<ValueType> explicitUniformizedMatrix = uniformizedMatrix.toMatrix(odd, odd);
                                
                                newSubresult = storm::modelchecker::helper::SparseCtmcCslHelper<ValueType>::computeTransientProbabilities(explicitUniformizedMatrix, nullptr, lowerBound, uniformizationRate, newSubresult, linearEquationSolverFactory);
                                
                                return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType>(model.getReachableStates(), !statesWithProbabilityGreater0 && model.getReachableStates(), model.getManager().getAddZero(), statesWithProbabilityGreater0, odd, newSubresult));
                            }
                        }
                    }
                } else {
                    return std::unique_ptr<CheckResult>(new SymbolicQuantitativeCheckResult<DdType>(model.getReachableStates(), psiStates.toAdd()));
                }
            }
            
            template<storm::dd::DdType DdType, class ValueType>
            std::unique_ptr<CheckResult> HybridCtmcCslHelper<DdType, ValueType>::computeInstantaneousRewards(storm::models::symbolic::Model<DdType> const& model, storm::dd::Add<DdType> const& rateMatrix, storm::dd::Add<DdType> const& exitRateVector, RewardModelType const& rewardModel, double timeBound, storm::utility::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                
                // Only compute the result if the model has a state-based reward model.
                STORM_LOG_THROW(rewardModel.hasStateRewards(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
                
                // Create ODD for the translation.
                storm::dd::Odd<DdType> odd(model.getReachableStates());
                
                // Initialize result to state rewards of the model.
                std::vector<ValueType> result = rewardModel.getStateRewardVector().template toVector<ValueType>(odd);
                
                // If the time-bound is not zero, we need to perform a transient analysis.
                if (timeBound > 0) {
                    ValueType uniformizationRate = 1.02 * exitRateVector.getMax();
                    STORM_LOG_THROW(uniformizationRate > 0, storm::exceptions::InvalidStateException, "The uniformization rate must be positive.");
                    
                    storm::dd::Add<DdType> uniformizedMatrix = computeUniformizedMatrix(model, rateMatrix, exitRateVector, model.getReachableStates(), uniformizationRate);
                    
                    storm::storage::SparseMatrix<ValueType> explicitUniformizedMatrix = uniformizedMatrix.toMatrix(odd, odd);
                    result = storm::modelchecker::helper::SparseCtmcCslHelper<ValueType>::computeTransientProbabilities(explicitUniformizedMatrix, nullptr, timeBound, uniformizationRate, result, linearEquationSolverFactory);
                }
                
                return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType>(model.getReachableStates(), model.getManager().getBddZero(), model.getManager().getAddZero(), model.getReachableStates(), odd, result));
            }
            
            template<storm::dd::DdType DdType, class ValueType>
            std::unique_ptr<CheckResult> HybridCtmcCslHelper<DdType, ValueType>::computeCumulativeRewards(storm::models::symbolic::Model<DdType> const& model, storm::dd::Add<DdType> const& rateMatrix, storm::dd::Add<DdType> const& exitRateVector, RewardModelType const& rewardModel, double timeBound, storm::utility::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                // Only compute the result if the model has a state-based reward model.
                STORM_LOG_THROW(!rewardModel.empty(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
                
                // If the time bound is zero, the result is the constant zero vector.
                if (timeBound == 0) {
                    return std::unique_ptr<CheckResult>(new SymbolicQuantitativeCheckResult<DdType>(model.getReachableStates(), model.getManager().getAddZero()));
                }
                
                // Otherwise, we need to perform some computations.
                
                // Start with the uniformization.
                ValueType uniformizationRate = 1.02 * exitRateVector.getMax();
                STORM_LOG_THROW(uniformizationRate > 0, storm::exceptions::InvalidStateException, "The uniformization rate must be positive.");
                
                // Create ODD for the translation.
                storm::dd::Odd<DdType> odd(model.getReachableStates());
                
                // Compute the uniformized matrix.
                storm::dd::Add<DdType> uniformizedMatrix = computeUniformizedMatrix(model, rateMatrix, exitRateVector,  model.getReachableStates(), uniformizationRate);
                storm::storage::SparseMatrix<ValueType> explicitUniformizedMatrix = uniformizedMatrix.toMatrix(odd, odd);
                
                // Then compute the state reward vector to use in the computation.
                storm::dd::Add<DdType> totalRewardVector = rewardModel.getTotalRewardVector(rateMatrix, model.getColumnVariables(), exitRateVector);
                std::vector<ValueType> explicitTotalRewardVector = totalRewardVector.template toVector<ValueType>(odd);
                
                // Finally, compute the transient probabilities.
                std::vector<ValueType> result = storm::modelchecker::helper::SparseCtmcCslHelper<ValueType>::template computeTransientProbabilities<true>(explicitUniformizedMatrix, nullptr, timeBound, uniformizationRate, explicitTotalRewardVector, linearEquationSolverFactory);
                return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType>(model.getReachableStates(), model.getManager().getBddZero(), model.getManager().getAddZero(), model.getReachableStates(), std::move(odd), std::move(result)));
            }
            
            template<storm::dd::DdType DdType, class ValueType>
            std::unique_ptr<CheckResult> HybridCtmcCslHelper<DdType, ValueType>::computeLongRunAverage(storm::models::symbolic::Model<DdType> const& model, storm::dd::Add<DdType> const& rateMatrix, storm::dd::Add<DdType> const& exitRateVector, storm::dd::Bdd<DdType> const& psiStates, bool qualitative, storm::utility::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                storm::dd::Add<DdType> probabilityMatrix = computeProbabilityMatrix(model, rateMatrix, exitRateVector);
                
                // Create ODD for the translation.
                storm::dd::Odd<DdType> odd(model.getReachableStates());
                
                storm::storage::SparseMatrix<ValueType> explicitProbabilityMatrix = probabilityMatrix.toMatrix(odd, odd);
                std::vector<ValueType> explicitExitRateVector = exitRateVector.template toVector<ValueType>(odd);
                
                std::vector<ValueType> result = storm::modelchecker::helper::SparseCtmcCslHelper<ValueType>::computeLongRunAverage(explicitProbabilityMatrix, psiStates.toVector(odd), &explicitExitRateVector, qualitative, linearEquationSolverFactory);
                return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType>(model.getReachableStates(), model.getManager().getBddZero(), model.getManager().getAddZero(), model.getReachableStates(), std::move(odd), std::move(result)));
            }
            
            template<storm::dd::DdType DdType, class ValueType>
            storm::dd::Add<DdType> HybridCtmcCslHelper<DdType, ValueType>::computeUniformizedMatrix(storm::models::symbolic::Model<DdType> const& model, storm::dd::Add<DdType> const& transitionMatrix, storm::dd::Add<DdType> const& exitRateVector, storm::dd::Bdd<DdType> const& maybeStates, ValueType uniformizationRate) {
                STORM_LOG_DEBUG("Computing uniformized matrix using uniformization rate " << uniformizationRate << ".");
                STORM_LOG_DEBUG("Keeping " << maybeStates.getNonZeroCount() << " rows.");
                
                // Cut all non-maybe rows/columns from the transition matrix.
                storm::dd::Add<DdType> uniformizedMatrix = transitionMatrix * maybeStates.toAdd() * maybeStates.swapVariables(model.getRowColumnMetaVariablePairs()).toAdd();
                
                // Now perform the uniformization.
                uniformizedMatrix = uniformizedMatrix / model.getManager().getConstant(uniformizationRate);
                storm::dd::Add<DdType> diagonal = model.getRowColumnIdentity() * maybeStates.toAdd();
                storm::dd::Add<DdType> diagonalOffset = diagonal;
                diagonalOffset -= diagonal * (exitRateVector / model.getManager().getConstant(uniformizationRate));
                uniformizedMatrix += diagonalOffset;
                
                return uniformizedMatrix;
            }
            
            template<storm::dd::DdType DdType, class ValueType>
            storm::dd::Add<DdType> HybridCtmcCslHelper<DdType, ValueType>::computeProbabilityMatrix(storm::models::symbolic::Model<DdType> const& model, storm::dd::Add<DdType> const& rateMatrix, storm::dd::Add<DdType> const& exitRateVector) {
                return rateMatrix / exitRateVector;
            }

            template class HybridCtmcCslHelper<storm::dd::DdType::CUDD, double>;
            
        }
    }
}