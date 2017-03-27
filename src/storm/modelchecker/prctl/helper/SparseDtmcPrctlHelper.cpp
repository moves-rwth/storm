#include "storm/modelchecker/prctl/helper/SparseDtmcPrctlHelper.h"

#include "storm/modelchecker/csl/helper/SparseCtmcCslHelper.h"

#include "storm/utility/macros.h"
#include "storm/utility/vector.h"
#include "storm/utility/graph.h"

#include "storm/solver/LinearEquationSolver.h"

#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            template<typename ValueType, typename RewardModelType>
            std::vector<ValueType> SparseDtmcPrctlHelper<ValueType, RewardModelType>::computeBoundedUntilProbabilities(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, uint_fast64_t stepBound, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                std::vector<ValueType> result(transitionMatrix.getRowCount(), storm::utility::zero<ValueType>());
                
                // If we identify the states that have probability 0 of reaching the target states, we can exclude them in the further analysis.
                storm::storage::BitVector maybeStates = storm::utility::graph::performProbGreater0(backwardTransitions, phiStates, psiStates, true, stepBound);
                maybeStates &= ~psiStates;
                STORM_LOG_INFO("Found " << maybeStates.getNumberOfSetBits() << " 'maybe' states.");
                
                if (!maybeStates.empty()) {
                    // We can eliminate the rows and columns from the original transition probability matrix that have probability 0.
                    storm::storage::SparseMatrix<ValueType> submatrix = transitionMatrix.getSubmatrix(true, maybeStates, maybeStates, true);
                    
                    // Create the vector of one-step probabilities to go to target states.
                    std::vector<ValueType> b = transitionMatrix.getConstrainedRowSumVector(maybeStates, psiStates);
                    
                    // Create the vector with which to multiply.
                    std::vector<ValueType> subresult(maybeStates.getNumberOfSetBits());
                    
                    // Perform the matrix vector multiplication as often as required by the formula bound.
                    std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = linearEquationSolverFactory.create(std::move(submatrix));
                    solver->repeatedMultiply(subresult, &b, stepBound);
                    
                    // Set the values of the resulting vector accordingly.
                    storm::utility::vector::setVectorValues(result, maybeStates, subresult);
                }
                storm::utility::vector::setVectorValues<ValueType>(result, psiStates, storm::utility::one<ValueType>());
                
                return result;
            }
            
            template<typename ValueType, typename RewardModelType>

            std::vector<ValueType> SparseDtmcPrctlHelper<ValueType, RewardModelType>::computeUntilProbabilities(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory, boost::optional<std::vector<ValueType>> resultHint) {
                // We need to identify the states which have to be taken out of the matrix, i.e.
                // all states that have probability 0 and 1 of satisfying the until-formula.
                std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01 = storm::utility::graph::performProb01(backwardTransitions, phiStates, psiStates);
                storm::storage::BitVector statesWithProbability0 = std::move(statesWithProbability01.first);
                storm::storage::BitVector statesWithProbability1 = std::move(statesWithProbability01.second);
                                
                // Perform some logging.
                storm::storage::BitVector maybeStates = ~(statesWithProbability0 | statesWithProbability1);
                STORM_LOG_INFO("Found " << statesWithProbability0.getNumberOfSetBits() << " 'no' states.");
                STORM_LOG_INFO("Found " << statesWithProbability1.getNumberOfSetBits() << " 'yes' states.");
                STORM_LOG_INFO("Found " << maybeStates.getNumberOfSetBits() << " 'maybe' states.");
                
                // Create resulting vector.
                std::vector<ValueType> result(transitionMatrix.getRowCount());
                
                // Check whether we need to compute exact probabilities for some states.
                if (qualitative) {
                    // Set the values for all maybe-states to 0.5 to indicate that their probability values are neither 0 nor 1.
                    storm::utility::vector::setVectorValues<ValueType>(result, maybeStates, storm::utility::convertNumber<ValueType>(0.5));
                } else {
                    if (!maybeStates.empty()) {
                        // In this case we have have to compute the probabilities.
                        
                        // We can eliminate the rows and columns from the original transition probability matrix.
                        storm::storage::SparseMatrix<ValueType> submatrix = transitionMatrix.getSubmatrix(true, maybeStates, maybeStates, true);
                        
                        // Converting the matrix from the fixpoint notation to the form needed for the equation
                        // system. That is, we go from x = A*x + b to (I-A)x = b.
                        submatrix.convertToEquationSystem();
                        
                        // Initialize the x vector with the hint (if available) or with 0.5 for each element.
                        // This is the initial guess for the iterative solvers. It should be safe as for all
                        // 'maybe' states we know that the probability is strictly larger than 0.
                        std::vector<ValueType> x(maybeStates.getNumberOfSetBits(), storm::utility::convertNumber<ValueType>(0.5));
                        if(resultHint){
                            storm::utility::vector::selectVectorValues(x, maybeStates, resultHint.get());
                        }

                        // Prepare the right-hand side of the equation system. For entry i this corresponds to
                        // the accumulated probability of going from state i to some 'yes' state.
                        std::vector<ValueType> b = transitionMatrix.getConstrainedRowSumVector(maybeStates, statesWithProbability1);
                        
                        // Now solve the created system of linear equations.
                        std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = linearEquationSolverFactory.create(std::move(submatrix));
                        solver->setBounds(storm::utility::zero<ValueType>(), storm::utility::one<ValueType>());
                        solver->solveEquations(x, b);
                        
                        // Set values of resulting vector according to result.
                        storm::utility::vector::setVectorValues<ValueType>(result, maybeStates, x);
                    }
                }
                
                // Set values of resulting vector that are known exactly.
                storm::utility::vector::setVectorValues<ValueType>(result, statesWithProbability0, storm::utility::zero<ValueType>());
                storm::utility::vector::setVectorValues<ValueType>(result, statesWithProbability1, storm::utility::one<ValueType>());
                
                return result;
            }
            
            template<typename ValueType, typename RewardModelType>
            std::vector<ValueType> SparseDtmcPrctlHelper<ValueType, RewardModelType>::computeGloballyProbabilities(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& psiStates, bool qualitative, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                std::vector<ValueType> result = computeUntilProbabilities(transitionMatrix, backwardTransitions, storm::storage::BitVector(transitionMatrix.getRowCount(), true), ~psiStates, qualitative, linearEquationSolverFactory);
                for (auto& entry : result) {
                    entry = storm::utility::one<ValueType>() - entry;
                }
                return result;
            }
            
            template<typename ValueType, typename RewardModelType>
            std::vector<ValueType> SparseDtmcPrctlHelper<ValueType, RewardModelType>::computeNextProbabilities(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& nextStates, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                // Create the vector with which to multiply and initialize it correctly.
                std::vector<ValueType> result(transitionMatrix.getRowCount());
                storm::utility::vector::setVectorValues(result, nextStates, storm::utility::one<ValueType>());
                
                // Perform one single matrix-vector multiplication.
                std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = linearEquationSolverFactory.create(transitionMatrix);
                solver->repeatedMultiply(result, nullptr, 1);
                return result;
            }
            
            template<typename ValueType, typename RewardModelType>
            std::vector<ValueType> SparseDtmcPrctlHelper<ValueType, RewardModelType>::computeCumulativeRewards(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, RewardModelType const& rewardModel, uint_fast64_t stepBound, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                // Initialize result to the null vector.
                std::vector<ValueType> result(transitionMatrix.getRowCount());
                
                // Compute the reward vector to add in each step based on the available reward models.
                std::vector<ValueType> totalRewardVector = rewardModel.getTotalRewardVector(transitionMatrix);
                
                // Perform the matrix vector multiplication as often as required by the formula bound.
                std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = linearEquationSolverFactory.create(transitionMatrix);
                solver->repeatedMultiply(result, &totalRewardVector, stepBound);
                
                return result;
            }
            
            template<typename ValueType, typename RewardModelType>
            std::vector<ValueType> SparseDtmcPrctlHelper<ValueType, RewardModelType>::computeInstantaneousRewards(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, RewardModelType const& rewardModel, uint_fast64_t stepCount, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                // Only compute the result if the model has a state-based reward this->getModel().
                STORM_LOG_THROW(rewardModel.hasStateRewards(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
                
                // Initialize result to state rewards of the model.
                std::vector<ValueType> result = rewardModel.getStateRewardVector();
                
                // Perform the matrix vector multiplication as often as required by the formula bound.
                std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = linearEquationSolverFactory.create(transitionMatrix);
                solver->repeatedMultiply(result, nullptr, stepCount);
                
                return result;
            }
            
            template<typename ValueType, typename RewardModelType>
            std::vector<ValueType> SparseDtmcPrctlHelper<ValueType, RewardModelType>::computeReachabilityRewards(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, RewardModelType const& rewardModel, storm::storage::BitVector const& targetStates, bool qualitative, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory, boost::optional<std::vector<ValueType>> resultHint) {
                return computeReachabilityRewards(transitionMatrix, backwardTransitions, [&] (uint_fast64_t numberOfRows, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& maybeStates) { return rewardModel.getTotalRewardVector(numberOfRows, transitionMatrix, maybeStates); }, targetStates, qualitative, linearEquationSolverFactory, resultHint);
            }

            template<typename ValueType, typename RewardModelType>
            std::vector<ValueType> SparseDtmcPrctlHelper<ValueType, RewardModelType>::computeReachabilityRewards(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& totalStateRewardVector, storm::storage::BitVector const& targetStates, bool qualitative, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory, boost::optional<std::vector<ValueType>> resultHint) {

                return computeReachabilityRewards(transitionMatrix, backwardTransitions,
                                                  [&] (uint_fast64_t numberOfRows, storm::storage::SparseMatrix<ValueType> const&, storm::storage::BitVector const& maybeStates) {
                                                      std::vector<ValueType> result(numberOfRows);
                                                      storm::utility::vector::selectVectorValues(result, maybeStates, totalStateRewardVector);
                                                      return result;
                                                  },
                                                  targetStates, qualitative, linearEquationSolverFactory, resultHint);
            }
            
            template<typename ValueType, typename RewardModelType>
            std::vector<ValueType> SparseDtmcPrctlHelper<ValueType, RewardModelType>::computeReachabilityRewards(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::function<std::vector<ValueType>(uint_fast64_t, storm::storage::SparseMatrix<ValueType> const&, storm::storage::BitVector const&)> const& totalStateRewardVectorGetter, storm::storage::BitVector const& targetStates, bool qualitative, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory, boost::optional<std::vector<ValueType>> resultHint) {
                // Determine which states have a reward of infinity by definition.
                storm::storage::BitVector trueStates(transitionMatrix.getRowCount(), true);
                storm::storage::BitVector infinityStates = storm::utility::graph::performProb1(backwardTransitions, trueStates, targetStates);
                infinityStates.complement();
                storm::storage::BitVector maybeStates = ~targetStates & ~infinityStates;
                STORM_LOG_INFO("Found " << infinityStates.getNumberOfSetBits() << " 'infinity' states.");
                STORM_LOG_INFO("Found " << targetStates.getNumberOfSetBits() << " 'target' states.");
                STORM_LOG_INFO("Found " << maybeStates.getNumberOfSetBits() << " 'maybe' states.");
                
                // Create resulting vector.
                std::vector<ValueType> result(transitionMatrix.getRowCount(), storm::utility::zero<ValueType>());
                
                // Check whether we need to compute exact rewards for some states.
                if (qualitative) {
                    // Set the values for all maybe-states to 1 to indicate that their reward values
                    // are neither 0 nor infinity.
                    storm::utility::vector::setVectorValues<ValueType>(result, maybeStates, storm::utility::one<ValueType>());
                } else {
                    if (!maybeStates.empty()) {
                        // In this case we have to compute the reward values for the remaining states.
                        // We can eliminate the rows and columns from the original transition probability matrix.
                        storm::storage::SparseMatrix<ValueType> submatrix = transitionMatrix.getSubmatrix(true, maybeStates, maybeStates, true);
                        
                        // Converting the matrix from the fixpoint notation to the form needed for the equation
                        // system. That is, we go from x = A*x + b to (I-A)x = b.
                        submatrix.convertToEquationSystem();
                        
                        // Initialize the x vector with the hint (if available) or with 1 for each element.
                        // This is the initial guess for the iterative solvers.
                        std::vector<ValueType> x(submatrix.getColumnCount(), storm::utility::one<ValueType>());
                        if(resultHint){
                            storm::utility::vector::selectVectorValues(x, maybeStates, resultHint.get());
                        }
                        
                        // Prepare the right-hand side of the equation system.
                        std::vector<ValueType> b = totalStateRewardVectorGetter(submatrix.getRowCount(), transitionMatrix, maybeStates);
                        
                        // Now solve the resulting equation system.
                        std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = linearEquationSolverFactory.create(std::move(submatrix));
                        solver->setLowerBound(storm::utility::zero<ValueType>());
                        solver->solveEquations(x, b);
                        
                        // Set values of resulting vector according to result.
                        storm::utility::vector::setVectorValues<ValueType>(result, maybeStates, x);
                    }
                }
                
                // Set values of resulting vector that are known exactly.
                storm::utility::vector::setVectorValues(result, infinityStates, storm::utility::infinity<ValueType>());
                
                return result;
            }
            
            template<typename ValueType, typename RewardModelType>
            std::vector<ValueType> SparseDtmcPrctlHelper<ValueType, RewardModelType>::computeLongRunAverageProbabilities(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& psiStates, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                return SparseCtmcCslHelper::computeLongRunAverageProbabilities<ValueType>(transitionMatrix, psiStates, nullptr, linearEquationSolverFactory);
            }
            
            template<typename ValueType, typename RewardModelType>
            std::vector<ValueType> SparseDtmcPrctlHelper<ValueType, RewardModelType>::computeLongRunAverageRewards(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, RewardModelType const& rewardModel, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                return SparseCtmcCslHelper::computeLongRunAverageRewards<ValueType, RewardModelType>(transitionMatrix, rewardModel, nullptr, linearEquationSolverFactory);
            }
            
            template<typename ValueType, typename RewardModelType>
            std::vector<ValueType> SparseDtmcPrctlHelper<ValueType, RewardModelType>::computeLongRunAverageRewards(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& stateRewards, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                return SparseCtmcCslHelper::computeLongRunAverageRewards<ValueType>(transitionMatrix, stateRewards, nullptr, linearEquationSolverFactory);
            }
            
            template<typename ValueType, typename RewardModelType>
            typename SparseDtmcPrctlHelper<ValueType, RewardModelType>::BaierTransformedModel SparseDtmcPrctlHelper<ValueType, RewardModelType>::computeBaierTransformation(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& targetStates, storm::storage::BitVector const& conditionStates, boost::optional<std::vector<ValueType>> const& stateRewards, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {

                BaierTransformedModel result;
                
                // Start by computing all 'before' states, i.e. the states for which the conditional probability is defined.
                std::vector<ValueType> probabilitiesToReachConditionStates = computeUntilProbabilities(transitionMatrix, backwardTransitions, storm::storage::BitVector(transitionMatrix.getRowCount(), true), conditionStates, false, linearEquationSolverFactory);
                
                result.beforeStates = storm::storage::BitVector(targetStates.size(), true);
                uint_fast64_t state = 0;
                uint_fast64_t beforeStateIndex = 0;
                for (auto const& value : probabilitiesToReachConditionStates) {
                    if (value == storm::utility::zero<ValueType>()) {
                        result.beforeStates.set(state, false);
                    } else {
                        probabilitiesToReachConditionStates[beforeStateIndex] = value;
                        ++beforeStateIndex;
                    }
                    ++state;
                }
                probabilitiesToReachConditionStates.resize(beforeStateIndex);
                
                if (targetStates.empty()) {
                    result.noTargetStates = true;
                    return result;
                } else if (!result.beforeStates.empty()) {
                    // If there are some states for which the conditional probability is defined and there are some
                    // states that can reach the target states without visiting condition states first, we need to
                    // do more work.
                    
                    // First, compute the relevant states and some offsets.
                    storm::storage::BitVector allStates(targetStates.size(), true);
                    std::vector<uint_fast64_t> numberOfBeforeStatesUpToState = result.beforeStates.getNumberOfSetBitsBeforeIndices();
                    storm::storage::BitVector statesWithProbabilityGreater0 = storm::utility::graph::performProbGreater0(backwardTransitions, allStates, targetStates);
                    statesWithProbabilityGreater0 &= storm::utility::graph::getReachableStates(transitionMatrix, conditionStates, allStates, targetStates);
                    uint_fast64_t normalStatesOffset = result.beforeStates.getNumberOfSetBits();
                    std::vector<uint_fast64_t> numberOfNormalStatesUpToState = statesWithProbabilityGreater0.getNumberOfSetBitsBeforeIndices();
                    
                    // All transitions going to states with probability zero, need to be redirected to a deadlock state.
                    bool addDeadlockState = false;
                    uint_fast64_t deadlockState = normalStatesOffset + statesWithProbabilityGreater0.getNumberOfSetBits();
                    
                    // Now, we create the matrix of 'before' and 'normal' states.
                    storm::storage::SparseMatrixBuilder<ValueType> builder;
                    
                    // Start by creating the transitions of the 'before' states.
                    uint_fast64_t currentRow = 0;
                    for (auto beforeState : result.beforeStates) {
                        if (conditionStates.get(beforeState)) {
                            // For condition states, we move to the 'normal' states.
                            ValueType zeroProbability = storm::utility::zero<ValueType>();
                            for (auto const& successorEntry : transitionMatrix.getRow(beforeState)) {
                                if (statesWithProbabilityGreater0.get(successorEntry.getColumn())) {
                                    builder.addNextValue(currentRow, normalStatesOffset + numberOfNormalStatesUpToState[successorEntry.getColumn()], successorEntry.getValue());
                                } else {
                                    zeroProbability += successorEntry.getValue();
                                }
                            }
                            if (!storm::utility::isZero(zeroProbability)) {
                                builder.addNextValue(currentRow, deadlockState, zeroProbability);
                            }
                        } else {
                            // For non-condition states, we scale the probabilities going to other before states.
                            for (auto const& successorEntry : transitionMatrix.getRow(beforeState)) {
                                if (result.beforeStates.get(successorEntry.getColumn())) {
                                    builder.addNextValue(currentRow, numberOfBeforeStatesUpToState[successorEntry.getColumn()], successorEntry.getValue() * probabilitiesToReachConditionStates[numberOfBeforeStatesUpToState[successorEntry.getColumn()]] / probabilitiesToReachConditionStates[currentRow]);
                                }
                            }
                        }
                        ++currentRow;
                    }
                    
                    // Then, create the transitions of the 'normal' states.
                    for (auto state : statesWithProbabilityGreater0) {
                        ValueType zeroProbability = storm::utility::zero<ValueType>();
                        for (auto const& successorEntry : transitionMatrix.getRow(state)) {
                            if (statesWithProbabilityGreater0.get(successorEntry.getColumn())) {
                                builder.addNextValue(currentRow, normalStatesOffset + numberOfNormalStatesUpToState[successorEntry.getColumn()], successorEntry.getValue());
                            } else {
                                zeroProbability += successorEntry.getValue();
                            }
                        }
                        if (!storm::utility::isZero(zeroProbability)) {
                            addDeadlockState = true;
                            builder.addNextValue(currentRow, deadlockState, zeroProbability);
                        }
                        ++currentRow;
                    }
                    if (addDeadlockState) {
                        builder.addNextValue(deadlockState, deadlockState, storm::utility::one<ValueType>());
                    }
                    
                    // Build the new transition matrix and the new targets.
                    result.transitionMatrix = builder.build();
                    storm::storage::BitVector newTargetStates = targetStates % result.beforeStates;
                    newTargetStates.resize(result.transitionMatrix.get().getRowCount());
                    for (auto state : targetStates % statesWithProbabilityGreater0) {
                        newTargetStates.set(normalStatesOffset + state, true);
                    }
                    result.targetStates = std::move(newTargetStates);

                    // If a reward model was given, we need to compute the rewards for the transformed model.
                    if (stateRewards) {
                        std::vector<ValueType> newStateRewards(result.beforeStates.getNumberOfSetBits());
                        storm::utility::vector::selectVectorValues(newStateRewards, result.beforeStates, stateRewards.get());
                        
                        newStateRewards.reserve(newStateRewards.size() + statesWithProbabilityGreater0.getNumberOfSetBits() + 1);
                        for (auto state : statesWithProbabilityGreater0) {
                            newStateRewards.push_back(stateRewards.get()[state]);
                        }
                        // Add a zero reward to the deadlock state.
                        newStateRewards.push_back(storm::utility::zero<ValueType>());
                        result.stateRewards = std::move(newStateRewards);
                    }
                    
                }
                
                return result;
            }
            
            template<typename ValueType, typename RewardModelType>
            std::vector<ValueType> SparseDtmcPrctlHelper<ValueType, RewardModelType>::computeConditionalProbabilities(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& targetStates, storm::storage::BitVector const& conditionStates, bool qualitative, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                
                // Prepare result vector.
                std::vector<ValueType> result(transitionMatrix.getRowCount(), storm::utility::infinity<ValueType>());
                
                if (!conditionStates.empty()) {
                    BaierTransformedModel transformedModel = computeBaierTransformation(transitionMatrix, backwardTransitions, targetStates, conditionStates, boost::none, linearEquationSolverFactory);
                    
                    if (transformedModel.noTargetStates) {
                        storm::utility::vector::setVectorValues(result, transformedModel.beforeStates, storm::utility::zero<ValueType>());
                    } else {
                        // At this point, we do not need to check whether there are 'before' states, since the condition
                        // states were non-empty so there is at least one state with a positive probability of satisfying
                        // the condition.
                        
                        // Now compute reachability probabilities in the transformed model.
                        storm::storage::SparseMatrix<ValueType> const& newTransitionMatrix = transformedModel.transitionMatrix.get();
                        std::vector<ValueType> conditionalProbabilities = computeUntilProbabilities(newTransitionMatrix, newTransitionMatrix.transpose(), storm::storage::BitVector(newTransitionMatrix.getRowCount(), true), transformedModel.targetStates.get(), qualitative, linearEquationSolverFactory);
                        
                        storm::utility::vector::setVectorValues(result, transformedModel.beforeStates, conditionalProbabilities);
                    }
                }
                
                return result;
            }
            
            template<typename ValueType, typename RewardModelType>
            std::vector<ValueType> SparseDtmcPrctlHelper<ValueType, RewardModelType>::computeConditionalRewards(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, RewardModelType const& rewardModel, storm::storage::BitVector const& targetStates, storm::storage::BitVector const& conditionStates, bool qualitative, storm::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
                // Prepare result vector.
                std::vector<ValueType> result(transitionMatrix.getRowCount(), storm::utility::infinity<ValueType>());
                
                if (!conditionStates.empty()) {
                    BaierTransformedModel transformedModel = computeBaierTransformation(transitionMatrix, backwardTransitions, targetStates, conditionStates, rewardModel.getTotalRewardVector(transitionMatrix), linearEquationSolverFactory);
                    
                    if (transformedModel.noTargetStates) {
                        storm::utility::vector::setVectorValues(result, transformedModel.beforeStates, storm::utility::zero<ValueType>());
                    } else {
                        // At this point, we do not need to check whether there are 'before' states, since the condition
                        // states were non-empty so there is at least one state with a positive probability of satisfying
                        // the condition.
                        
                        // Now compute reachability probabilities in the transformed model.
                        storm::storage::SparseMatrix<ValueType> const& newTransitionMatrix = transformedModel.transitionMatrix.get();
                        std::vector<ValueType> conditionalRewards = computeReachabilityRewards(newTransitionMatrix, newTransitionMatrix.transpose(), transformedModel.stateRewards.get(), transformedModel.targetStates.get(), qualitative, linearEquationSolverFactory);
                        storm::utility::vector::setVectorValues(result, transformedModel.beforeStates, conditionalRewards);
                    }
                }
                
                return result;
            }
            
            template class SparseDtmcPrctlHelper<double>;

#ifdef STORM_HAVE_CARL
            template class SparseDtmcPrctlHelper<storm::RationalNumber>;
            template class SparseDtmcPrctlHelper<storm::RationalFunction>;
#endif
        }
    }
}
