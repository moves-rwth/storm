#include "src/modelchecker/csl/helper/HybridCtmcCslHelper.h"

#include "src/modelchecker/csl/helper/SparseCtmcCslHelper.h"
#include "src/modelchecker/prctl/helper/HybridDtmcPrctlHelper.h"

#include "src/storage/dd/DdManager.h"
#include "src/storage/dd/Add.h"
#include "src/storage/dd/Bdd.h"

#include "src/utility/macros.h"
#include "src/utility/graph.h"
#include "src/utility/constants.h"

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
            std::unique_ptr<CheckResult> HybridCtmcCslHelper<DdType, ValueType>::computeReachabilityRewards(storm::models::symbolic::Ctmc<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& rateMatrix, storm::dd::Add<DdType, ValueType> const& exitRateVector, RewardModelType const& rewardModel, storm::dd::Bdd<DdType> const& targetStates, bool qualitative, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                
                return HybridDtmcPrctlHelper<DdType, ValueType>::computeReachabilityRewards(model, computeProbabilityMatrix(model, rateMatrix, exitRateVector), rewardModel.divideStateRewardVector(exitRateVector), targetStates, qualitative, linearEquationSolverFactory);
            }
            
            template<storm::dd::DdType DdType, class ValueType>
            std::unique_ptr<CheckResult> HybridCtmcCslHelper<DdType, ValueType>::computeNextProbabilities(storm::models::symbolic::Ctmc<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& rateMatrix, storm::dd::Add<DdType, ValueType> const& exitRateVector, storm::dd::Bdd<DdType> const& nextStates) {
                return HybridDtmcPrctlHelper<DdType, ValueType>::computeNextProbabilities(model, computeProbabilityMatrix(model, rateMatrix, exitRateVector), nextStates);
            }
            
            template<storm::dd::DdType DdType, class ValueType>
            std::unique_ptr<CheckResult> HybridCtmcCslHelper<DdType, ValueType>::computeUntilProbabilities(storm::models::symbolic::Ctmc<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& rateMatrix, storm::dd::Add<DdType, ValueType> const& exitRateVector, storm::dd::Bdd<DdType> const& phiStates, storm::dd::Bdd<DdType> const& psiStates, bool qualitative, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                return HybridDtmcPrctlHelper<DdType, ValueType>::computeUntilProbabilities(model, computeProbabilityMatrix(model, rateMatrix, exitRateVector), phiStates, psiStates, qualitative, linearEquationSolverFactory);
            }
            
            template<storm::dd::DdType DdType, class ValueType>
            std::unique_ptr<CheckResult> HybridCtmcCslHelper<DdType, ValueType>::computeBoundedUntilProbabilities(storm::models::symbolic::Ctmc<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& rateMatrix, storm::dd::Add<DdType, ValueType> const& exitRateVector, storm::dd::Bdd<DdType> const& phiStates, storm::dd::Bdd<DdType> const& psiStates, bool qualitative, double lowerBound, double upperBound, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                
                // If the time bounds are [0, inf], we rather call untimed reachability.
                if (storm::utility::isZero(lowerBound) && upperBound == storm::utility::infinity<ValueType>()) {
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
                            
                            // Create an ODD for the translation to an explicit representation.
                            storm::dd::Odd odd = statesWithProbabilityGreater0NonPsi.createOdd();
                            
                            // Convert the symbolic parts to their explicit representation.
                            storm::storage::SparseMatrix<ValueType> explicitUniformizedMatrix = uniformizedMatrix.toMatrix(odd, odd);
                            std::vector<ValueType> explicitB = b.toVector(odd);
                            
                            // Finally compute the transient probabilities.
                            std::vector<ValueType> values(statesWithProbabilityGreater0NonPsi.getNonZeroCount(), storm::utility::zero<ValueType>());
                            std::vector<ValueType> subresult = storm::modelchecker::helper::SparseCtmcCslHelper::computeTransientProbabilities(explicitUniformizedMatrix, &explicitB, upperBound, uniformizationRate, values, linearEquationSolverFactory);
                            
                            return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType>(model.getReachableStates(),
                                                                                                          (psiStates || !statesWithProbabilityGreater0) && model.getReachableStates(),
                                                                                                          psiStates.template toAdd<ValueType>(), statesWithProbabilityGreater0NonPsi, odd, subresult));
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
                            storm::dd::Odd odd = relevantStates.createOdd();
                            
                            std::vector<ValueType> result;
                            if (unboundedResult->isHybridQuantitativeCheckResult()) {
                                std::unique_ptr<CheckResult> explicitUnboundedResult = unboundedResult->asHybridQuantitativeCheckResult<DdType, ValueType>().toExplicitQuantitativeCheckResult();
                                result = std::move(explicitUnboundedResult->asExplicitQuantitativeCheckResult<ValueType>().getValueVector());
                            } else {
                                STORM_LOG_THROW(unboundedResult->isSymbolicQuantitativeCheckResult(), storm::exceptions::InvalidStateException, "Expected check result of different type.");
                                result = unboundedResult->asSymbolicQuantitativeCheckResult<DdType, ValueType>().getValueVector().toVector(odd);
                            }
                            
                            // Determine the uniformization rate for the transient probability computation.
                            ValueType uniformizationRate = 1.02 * (relevantStates.template toAdd<ValueType>() * exitRateVector).getMax();
                            
                            // Compute the uniformized matrix.
                            storm::dd::Add<DdType, ValueType> uniformizedMatrix = computeUniformizedMatrix(model, rateMatrix, exitRateVector, relevantStates, uniformizationRate);
                            storm::storage::SparseMatrix<ValueType> explicitUniformizedMatrix = uniformizedMatrix.toMatrix(odd, odd);
                            
                            // Compute the transient probabilities.
                            result = storm::modelchecker::helper::SparseCtmcCslHelper::computeTransientProbabilities<ValueType>(explicitUniformizedMatrix, nullptr, lowerBound, uniformizationRate, result, linearEquationSolverFactory);
                            
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
                                storm::dd::Odd odd = statesWithProbabilityGreater0NonPsi.createOdd();
                                storm::storage::SparseMatrix<ValueType> explicitUniformizedMatrix = uniformizedMatrix.toMatrix(odd, odd);
                                std::vector<ValueType> explicitB = b.toVector(odd);
                                
                                // Compute the transient probabilities.
                                std::vector<ValueType> values(statesWithProbabilityGreater0NonPsi.getNonZeroCount(), storm::utility::zero<ValueType>());
                                std::vector<ValueType> subResult = storm::modelchecker::helper::SparseCtmcCslHelper::computeTransientProbabilities(explicitUniformizedMatrix, &explicitB, upperBound - lowerBound, uniformizationRate, values, linearEquationSolverFactory);
                                
                                // Transform the explicit result to a hybrid check result, so we can easily convert it to
                                // a symbolic qualitative format.
                                HybridQuantitativeCheckResult<DdType> hybridResult(model.getReachableStates(), psiStates || (!statesWithProbabilityGreater0 && model.getReachableStates()),
                                                                                   psiStates.template toAdd<ValueType>(), statesWithProbabilityGreater0NonPsi, odd, subResult);
                                
                                // Compute the set of relevant states.
                                storm::dd::Bdd<DdType> relevantStates = statesWithProbabilityGreater0 && phiStates;
                                
                                // Filter the unbounded result such that it only contains values for the relevant states.
                                hybridResult.filter(SymbolicQualitativeCheckResult<DdType>(model.getReachableStates(), relevantStates));
                                
                                // Build an ODD for the relevant states.
                                odd = relevantStates.createOdd();
                                
                                std::unique_ptr<CheckResult> explicitResult = hybridResult.toExplicitQuantitativeCheckResult();
                                std::vector<ValueType> newSubresult = std::move(explicitResult->asExplicitQuantitativeCheckResult<ValueType>().getValueVector());
                                
                                // Then compute the transient probabilities of being in such a state after t time units. For this,
                                // we must re-uniformize the CTMC, so we need to compute the second uniformized matrix.
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
                                explicitUniformizedMatrix = uniformizedMatrix.toMatrix(odd, odd);
                                
                                newSubresult = storm::modelchecker::helper::SparseCtmcCslHelper::computeTransientProbabilities<ValueType>(explicitUniformizedMatrix, nullptr, lowerBound, uniformizationRate, newSubresult, linearEquationSolverFactory);
                                
                                return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType>(model.getReachableStates(), !relevantStates && model.getReachableStates(), model.getManager().template getAddZero<ValueType>(), relevantStates, odd, newSubresult));
                            } else {
                                // In this case, the interval is of the form [t, t] with t != 0, t != inf.
                                
                                // Build an ODD for the relevant states.
                                storm::dd::Odd odd = statesWithProbabilityGreater0.createOdd();
                                
                                std::vector<ValueType> newSubresult = psiStates.template toAdd<ValueType>().toVector(odd);
                                
                                // Then compute the transient probabilities of being in such a state after t time units. For this,
                                // we must re-uniformize the CTMC, so we need to compute the second uniformized matrix.
                                ValueType uniformizationRate =  1.02 * (statesWithProbabilityGreater0.template toAdd<ValueType>() * exitRateVector).getMax();
                                STORM_LOG_THROW(uniformizationRate > 0, storm::exceptions::InvalidStateException, "The uniformization rate must be positive.");
                                
                                // Finally, we compute the second set of transient probabilities.
                                storm::dd::Add<DdType, ValueType> uniformizedMatrix = computeUniformizedMatrix(model, rateMatrix, exitRateVector, statesWithProbabilityGreater0, uniformizationRate);
                                storm::storage::SparseMatrix<ValueType> explicitUniformizedMatrix = uniformizedMatrix.toMatrix(odd, odd);
                                
                                newSubresult = storm::modelchecker::helper::SparseCtmcCslHelper::computeTransientProbabilities<ValueType>(explicitUniformizedMatrix, nullptr, lowerBound, uniformizationRate, newSubresult, linearEquationSolverFactory);
                                
                                return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType>(model.getReachableStates(), !statesWithProbabilityGreater0 && model.getReachableStates(), model.getManager().template getAddZero<ValueType>(), statesWithProbabilityGreater0, odd, newSubresult));
                            }
                        }
                    }
                } else {
                    return std::unique_ptr<CheckResult>(new SymbolicQuantitativeCheckResult<DdType>(model.getReachableStates(), psiStates.template toAdd<ValueType>()));
                }
            }
            
            template<storm::dd::DdType DdType, class ValueType>
            std::unique_ptr<CheckResult> HybridCtmcCslHelper<DdType, ValueType>::computeInstantaneousRewards(storm::models::symbolic::Ctmc<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& rateMatrix, storm::dd::Add<DdType, ValueType> const& exitRateVector, RewardModelType const& rewardModel, double timeBound, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                
                // Only compute the result if the model has a state-based reward model.
                STORM_LOG_THROW(rewardModel.hasStateRewards(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
                
                // Create ODD for the translation.
                storm::dd::Odd odd =model.getReachableStates().createOdd();
                
                // Initialize result to state rewards of the model.
                std::vector<ValueType> result = rewardModel.getStateRewardVector().toVector(odd);
                
                // If the time-bound is not zero, we need to perform a transient analysis.
                if (timeBound > 0) {
                    ValueType uniformizationRate = 1.02 * exitRateVector.getMax();
                    STORM_LOG_THROW(uniformizationRate > 0, storm::exceptions::InvalidStateException, "The uniformization rate must be positive.");
                    
                    storm::dd::Add<DdType, ValueType> uniformizedMatrix = computeUniformizedMatrix(model, rateMatrix, exitRateVector, model.getReachableStates(), uniformizationRate);
                    
                    storm::storage::SparseMatrix<ValueType> explicitUniformizedMatrix = uniformizedMatrix.toMatrix(odd, odd);
                    result = storm::modelchecker::helper::SparseCtmcCslHelper::computeTransientProbabilities<ValueType>(explicitUniformizedMatrix, nullptr, timeBound, uniformizationRate, result, linearEquationSolverFactory);
                }
                
                return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType>(model.getReachableStates(), model.getManager().getBddZero(), model.getManager().template getAddZero<ValueType>(), model.getReachableStates(), odd, result));
            }
            
            template<storm::dd::DdType DdType, class ValueType>
            std::unique_ptr<CheckResult> HybridCtmcCslHelper<DdType, ValueType>::computeCumulativeRewards(storm::models::symbolic::Ctmc<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& rateMatrix, storm::dd::Add<DdType, ValueType> const& exitRateVector, RewardModelType const& rewardModel, double timeBound, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                // Only compute the result if the model has a state-based reward model.
                STORM_LOG_THROW(!rewardModel.empty(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
                
                // If the time bound is zero, the result is the constant zero vector.
                if (timeBound == 0) {
                    return std::unique_ptr<CheckResult>(new SymbolicQuantitativeCheckResult<DdType>(model.getReachableStates(), model.getManager().template getAddZero<ValueType>()));
                }
                
                // Otherwise, we need to perform some computations.
                
                // Start with the uniformization.
                ValueType uniformizationRate = 1.02 * exitRateVector.getMax();
                STORM_LOG_THROW(uniformizationRate > 0, storm::exceptions::InvalidStateException, "The uniformization rate must be positive.");
                
                // Create ODD for the translation.
                storm::dd::Odd odd = model.getReachableStates().createOdd();
                
                // Compute the uniformized matrix.
                storm::dd::Add<DdType, ValueType> uniformizedMatrix = computeUniformizedMatrix(model, rateMatrix, exitRateVector,  model.getReachableStates(), uniformizationRate);
                storm::storage::SparseMatrix<ValueType> explicitUniformizedMatrix = uniformizedMatrix.toMatrix(odd, odd);
                
                // Then compute the state reward vector to use in the computation.
                storm::dd::Add<DdType, ValueType> totalRewardVector = rewardModel.getTotalRewardVector(rateMatrix, model.getColumnVariables(), exitRateVector);
                std::vector<ValueType> explicitTotalRewardVector = totalRewardVector.toVector(odd);
                
                // Finally, compute the transient probabilities.
                std::vector<ValueType> result = storm::modelchecker::helper::SparseCtmcCslHelper::computeTransientProbabilities<ValueType, true>(explicitUniformizedMatrix, nullptr, timeBound, uniformizationRate, explicitTotalRewardVector, linearEquationSolverFactory);
                return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType>(model.getReachableStates(), model.getManager().getBddZero(), model.getManager().template getAddZero<ValueType>(), model.getReachableStates(), std::move(odd), std::move(result)));
            }
            
            template<storm::dd::DdType DdType, class ValueType>
            std::unique_ptr<CheckResult> HybridCtmcCslHelper<DdType, ValueType>::computeLongRunAverageProbabilities(storm::models::symbolic::Ctmc<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& rateMatrix, storm::dd::Add<DdType, ValueType> const& exitRateVector, storm::dd::Bdd<DdType> const& psiStates, bool qualitative, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                storm::dd::Add<DdType, ValueType> probabilityMatrix = computeProbabilityMatrix(model, rateMatrix, exitRateVector);
                
                // Create ODD for the translation.
                storm::dd::Odd odd = model.getReachableStates().createOdd();
                
                storm::storage::SparseMatrix<ValueType> explicitProbabilityMatrix = probabilityMatrix.toMatrix(odd, odd);
                std::vector<ValueType> explicitExitRateVector = exitRateVector.toVector(odd);
                
                std::vector<ValueType> result = storm::modelchecker::helper::SparseCtmcCslHelper::computeLongRunAverageProbabilities(explicitProbabilityMatrix, psiStates.toVector(odd), &explicitExitRateVector, qualitative, linearEquationSolverFactory);

                return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType>(model.getReachableStates(), model.getManager().getBddZero(), model.getManager().template getAddZero<ValueType>(), model.getReachableStates(), std::move(odd), std::move(result)));
            }
            
            template<storm::dd::DdType DdType, class ValueType>
            storm::dd::Add<DdType, ValueType> HybridCtmcCslHelper<DdType, ValueType>::computeUniformizedMatrix(storm::models::symbolic::Ctmc<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, storm::dd::Add<DdType, ValueType> const& exitRateVector, storm::dd::Bdd<DdType> const& maybeStates, ValueType uniformizationRate) {
                STORM_LOG_DEBUG("Computing uniformized matrix using uniformization rate " << uniformizationRate << ".");
                STORM_LOG_DEBUG("Keeping " << maybeStates.getNonZeroCount() << " rows.");
                
                // Cut all non-maybe rows/columns from the transition matrix.
                storm::dd::Add<DdType, ValueType> uniformizedMatrix = transitionMatrix * maybeStates.template toAdd<ValueType>() * maybeStates.swapVariables(model.getRowColumnMetaVariablePairs()).template toAdd<ValueType>();
                
                // Now perform the uniformization.
                uniformizedMatrix = uniformizedMatrix / model.getManager().getConstant(uniformizationRate);
                storm::dd::Add<DdType, ValueType> diagonal = model.getRowColumnIdentity() * maybeStates.template toAdd<ValueType>();
                storm::dd::Add<DdType, ValueType> diagonalOffset = diagonal;
                diagonalOffset -= diagonal * (exitRateVector / model.getManager().getConstant(uniformizationRate));
                uniformizedMatrix += diagonalOffset;
                
                return uniformizedMatrix;
            }
            
            template<storm::dd::DdType DdType, class ValueType>
            storm::dd::Add<DdType, ValueType> HybridCtmcCslHelper<DdType, ValueType>::computeProbabilityMatrix(storm::models::symbolic::Ctmc<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& rateMatrix, storm::dd::Add<DdType, ValueType> const& exitRateVector) {
                return rateMatrix / exitRateVector;
            }

            template class HybridCtmcCslHelper<storm::dd::DdType::CUDD, double>;
            template class HybridCtmcCslHelper<storm::dd::DdType::Sylvan, double>;
            
        }
    }
}