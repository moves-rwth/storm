#include "src/modelchecker/prctl/helper/SparseMdpPrctlHelper.h"

#include "src/models/sparse/StandardRewardModel.h"

#include "src/storage/MaximalEndComponentDecomposition.h"

#include "src/utility/macros.h"
#include "src/utility/vector.h"
#include "src/utility/graph.h"

#include "src/storage/expressions/Variable.h"
#include "src/storage/expressions/Expression.h"

#include "src/solver/MinMaxLinearEquationSolver.h"
#include "src/solver/LpSolver.h"

#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {

            template<typename ValueType>
            std::vector<ValueType> SparseMdpPrctlHelper<ValueType>::computeBoundedUntilProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, uint_fast64_t stepBound, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory) {
                std::vector<ValueType> result(transitionMatrix.getRowCount(), storm::utility::zero<ValueType>());
                
                // Determine the states that have 0 probability of reaching the target states.
                storm::storage::BitVector maybeStates;
                if (dir == OptimizationDirection::Minimize) {
                    maybeStates = storm::utility::graph::performProbGreater0A(transitionMatrix, transitionMatrix.getRowGroupIndices(), backwardTransitions, phiStates, psiStates, true, stepBound);
                } else {
                    maybeStates = storm::utility::graph::performProbGreater0E(transitionMatrix, transitionMatrix.getRowGroupIndices(), backwardTransitions, phiStates, psiStates, true, stepBound);
                }
                maybeStates &= ~psiStates;
                STORM_LOG_INFO("Found " << maybeStates.getNumberOfSetBits() << " 'maybe' states.");
                
                
                if (!maybeStates.empty()) {
                    // We can eliminate the rows and columns from the original transition probability matrix that have probability 0.
                    storm::storage::SparseMatrix<ValueType> submatrix = transitionMatrix.getSubmatrix(true, maybeStates, maybeStates, false);
                    std::vector<ValueType> b = transitionMatrix.getConstrainedRowGroupSumVector(maybeStates, psiStates);
                    
                    // Create the vector with which to multiply.
                    std::vector<ValueType> subresult(maybeStates.getNumberOfSetBits());
                    
                    std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> solver = minMaxLinearEquationSolverFactory.create(submatrix);
                    solver->performMatrixVectorMultiplication(dir, subresult, &b, stepBound);
                    
                    // Set the values of the resulting vector accordingly.
                    storm::utility::vector::setVectorValues(result, maybeStates, subresult);
                }
                storm::utility::vector::setVectorValues(result, psiStates, storm::utility::one<ValueType>());
                
                return result;
            }
            

            template<typename ValueType>
            std::vector<ValueType> SparseMdpPrctlHelper<ValueType>::computeNextProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& nextStates, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory) {

                // Create the vector with which to multiply and initialize it correctly.
                std::vector<ValueType> result(transitionMatrix.getRowCount());
                storm::utility::vector::setVectorValues(result, nextStates, storm::utility::one<ValueType>());
                
                std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> solver = minMaxLinearEquationSolverFactory.create(transitionMatrix);
                solver->performMatrixVectorMultiplication(dir, result);
                
                return result;
            }
       
            
            template<typename ValueType>
            MDPSparseModelCheckingHelperReturnType<ValueType> SparseMdpPrctlHelper<ValueType>::computeUntilProbabilities(storm::solver::SolveGoal const& goal, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative, bool getPolicy, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory) {
                     
                uint_fast64_t numberOfStates = transitionMatrix.getRowCount();
                
                // We need to identify the states which have to be taken out of the matrix, i.e.
                // all states that have probability 0 and 1 of satisfying the until-formula.
                std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01;
                if (goal.minimize()) {
                    statesWithProbability01 = storm::utility::graph::performProb01Min(transitionMatrix, transitionMatrix.getRowGroupIndices(), backwardTransitions, phiStates, psiStates);
                } else {
                    statesWithProbability01 = storm::utility::graph::performProb01Max(transitionMatrix, transitionMatrix.getRowGroupIndices(), backwardTransitions, phiStates, psiStates);
                }
                storm::storage::BitVector statesWithProbability0 = std::move(statesWithProbability01.first);
                storm::storage::BitVector statesWithProbability1 = std::move(statesWithProbability01.second);
                storm::storage::BitVector maybeStates = ~(statesWithProbability0 | statesWithProbability1);
                LOG4CPLUS_INFO(logger, "Found " << statesWithProbability0.getNumberOfSetBits() << " 'no' states.");
                LOG4CPLUS_INFO(logger, "Found " << statesWithProbability1.getNumberOfSetBits() << " 'yes' states.");
                LOG4CPLUS_INFO(logger, "Found " << maybeStates.getNumberOfSetBits() << " 'maybe' states.");
                
                // Create resulting vector.
                std::vector<ValueType> result(numberOfStates);
                
                // Set values of resulting vector that are known exactly.
                storm::utility::vector::setVectorValues<ValueType>(result, statesWithProbability0, storm::utility::zero<ValueType>());
                storm::utility::vector::setVectorValues<ValueType>(result, statesWithProbability1, storm::utility::one<ValueType>());
                
                
                // Check whether we need to compute exact probabilities for some states.
                if (qualitative) {
                    // Set the values for all maybe-states to 0.5 to indicate that their probability values are neither 0 nor 1.
                    storm::utility::vector::setVectorValues<ValueType>(result, maybeStates, ValueType(0.5));
                } else {
                    if (!maybeStates.empty()) {
                        // In this case we have have to compute the probabilities.
                        
                        // First, we can eliminate the rows and columns from the original transition probability matrix for states
                        // whose probabilities are already known.
                        storm::storage::SparseMatrix<ValueType> submatrix = transitionMatrix.getSubmatrix(true, maybeStates, maybeStates, false);
                        
                        // Prepare the right-hand side of the equation system. For entry i this corresponds to
                        // the accumulated probability of going from state i to some 'yes' state.
                        std::vector<ValueType> b = transitionMatrix.getConstrainedRowGroupSumVector(maybeStates, statesWithProbability1);
                        
                        // Create vector for results for maybe states.
                        std::vector<ValueType> x(maybeStates.getNumberOfSetBits());
                        
                        // Solve the corresponding system of equations.
                        std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> solver = storm::solver::configureMinMaxLinearEquationSolver(goal, minMaxLinearEquationSolverFactory, submatrix);
                        solver->solveEquationSystem(x, b);
                        
                        // Set values of resulting vector according to result.
                        storm::utility::vector::setVectorValues<ValueType>(result, maybeStates, x);
                    }
                }
                
                return MDPSparseModelCheckingHelperReturnType<ValueType>(std::move(result));
            }
            

            template<typename ValueType>
            MDPSparseModelCheckingHelperReturnType<ValueType> SparseMdpPrctlHelper<ValueType>::computeUntilProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative, bool getPolicy, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory) {
                storm::solver::SolveGoal goal(dir);
                return std::move(computeUntilProbabilities(goal, transitionMatrix, backwardTransitions, phiStates, psiStates, qualitative, getPolicy, minMaxLinearEquationSolverFactory));
            }
           
            
            template<typename ValueType>
            template<typename RewardModelType>
            std::vector<ValueType> SparseMdpPrctlHelper<ValueType>::computeInstantaneousRewards(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, RewardModelType const& rewardModel, uint_fast64_t stepCount, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory) {

                // Only compute the result if the model has a state-based reward this->getModel().
                STORM_LOG_THROW(rewardModel.hasStateRewards(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
                
                // Initialize result to state rewards of the this->getModel().
                std::vector<ValueType> result(rewardModel.getStateRewardVector());
                
                std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> solver = minMaxLinearEquationSolverFactory.create(transitionMatrix);
                solver->performMatrixVectorMultiplication(dir, result, nullptr, stepCount);
                
                return result;
            }
            
            template<typename ValueType>
            template<typename RewardModelType>
            std::vector<ValueType> SparseMdpPrctlHelper<ValueType>::computeCumulativeRewards(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, RewardModelType const& rewardModel, uint_fast64_t stepBound, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory) {

                // Only compute the result if the model has at least one reward this->getModel().
                STORM_LOG_THROW(!rewardModel.empty(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
                
                // Compute the reward vector to add in each step based on the available reward models.
                std::vector<ValueType> totalRewardVector = rewardModel.getTotalRewardVector(transitionMatrix);
                
                // Initialize result to either the state rewards of the model or the null vector.
                std::vector<ValueType> result;
                if (rewardModel.hasStateRewards()) {
                    result = rewardModel.getStateRewardVector();
                } else {
                    result.resize(transitionMatrix.getRowCount());
                }
                
                std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> solver = minMaxLinearEquationSolverFactory.create(transitionMatrix);
                solver->performMatrixVectorMultiplication(dir, result, &totalRewardVector, stepBound);
                
                return result;
            }
            

            template<typename ValueType>
            template<typename RewardModelType>
            std::vector<ValueType> SparseMdpPrctlHelper<ValueType>::computeReachabilityRewards(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, RewardModelType const& rewardModel, storm::storage::BitVector const& targetStates, bool qualitative, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory) {
                // Only compute the result if the model has at least one reward this->getModel().
                STORM_LOG_THROW(!rewardModel.empty(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
                return computeReachabilityRewardsHelper(dir, transitionMatrix, backwardTransitions,
                                                        [&rewardModel] (uint_fast64_t rowCount, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& maybeStates) {
                                                            return rewardModel.getTotalRewardVector(rowCount, transitionMatrix, maybeStates);
                                                        },
                                                        targetStates, qualitative, minMaxLinearEquationSolverFactory);
            }
            
#ifdef STORM_HAVE_CARL
            template<typename ValueType>
            std::vector<ValueType> SparseMdpPrctlHelper<ValueType>::computeReachabilityRewards(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::models::sparse::StandardRewardModel<storm::Interval> const& intervalRewardModel, storm::storage::BitVector const& targetStates, bool qualitative, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory) {
                // Only compute the result if the reward model is not empty.
                STORM_LOG_THROW(!intervalRewardModel.empty(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
                return computeReachabilityRewardsHelper(dir, transitionMatrix, backwardTransitions, \
                                                        [&] (uint_fast64_t rowCount, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& maybeStates) {
                                                            std::vector<ValueType> result;
                                                            result.reserve(rowCount);
                                                            std::vector<storm::Interval> subIntervalVector = intervalRewardModel.getTotalRewardVector(rowCount, transitionMatrix, maybeStates);
                                                            for (auto const& interval : subIntervalVector) {
                                                                result.push_back(dir == OptimizationDirection::Minimize ? interval.lower() : interval.upper());
                                                            }
                                                            return result;
                                                        }, \
                                                        targetStates, qualitative, minMaxLinearEquationSolverFactory);
            }
#endif
            
            template<typename ValueType>
            std::vector<ValueType> SparseMdpPrctlHelper<ValueType>::computeReachabilityRewardsHelper(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::function<std::vector<ValueType>(uint_fast64_t, storm::storage::SparseMatrix<ValueType> const&, storm::storage::BitVector const&)> const& totalStateRewardVectorGetter, storm::storage::BitVector const& targetStates, bool qualitative, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory) {
                // Determine which states have a reward of infinity by definition.
                storm::storage::BitVector infinityStates;
                storm::storage::BitVector trueStates(transitionMatrix.getRowCount(), true);
                if (dir == OptimizationDirection::Minimize) {
                    infinityStates = storm::utility::graph::performProb1E(transitionMatrix, transitionMatrix.getRowGroupIndices(), backwardTransitions, trueStates, targetStates);
                } else {
                    infinityStates = storm::utility::graph::performProb1A(transitionMatrix, transitionMatrix.getRowGroupIndices(), backwardTransitions, trueStates, targetStates);
                }
                infinityStates.complement();
                storm::storage::BitVector maybeStates = ~targetStates & ~infinityStates;
                LOG4CPLUS_INFO(logger, "Found " << infinityStates.getNumberOfSetBits() << " 'infinity' states.");
                LOG4CPLUS_INFO(logger, "Found " << targetStates.getNumberOfSetBits() << " 'target' states.");
                LOG4CPLUS_INFO(logger, "Found " << maybeStates.getNumberOfSetBits() << " 'maybe' states.");
                
                // Create resulting vector.
                std::vector<ValueType> result(transitionMatrix.getRowCount(), storm::utility::zero<ValueType>());
                
                // Check whether we need to compute exact rewards for some states.
                if (qualitative) {
                    LOG4CPLUS_INFO(logger, "The rewards for the initial states were determined in a preprocessing step. No exact rewards were computed.");
                    // Set the values for all maybe-states to 1 to indicate that their reward values
                    // are neither 0 nor infinity.
                    storm::utility::vector::setVectorValues<ValueType>(result, maybeStates, storm::utility::one<ValueType>());
                } else {
                    if (!maybeStates.empty()) {
                        // In this case we have to compute the reward values for the remaining states.
                        
                        // We can eliminate the rows and columns from the original transition probability matrix for states
                        // whose reward values are already known.
                        storm::storage::SparseMatrix<ValueType> submatrix = transitionMatrix.getSubmatrix(true, maybeStates, maybeStates, false);
                        
                        // Prepare the right-hand side of the equation system.
                        std::vector<ValueType> b = totalStateRewardVectorGetter(submatrix.getRowCount(), transitionMatrix, maybeStates);
                        
                        // Create vector for results for maybe states.
                        std::vector<ValueType> x(maybeStates.getNumberOfSetBits());
                        
                        // Solve the corresponding system of equations.
                        std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> solver = minMaxLinearEquationSolverFactory.create(submatrix);
                        solver->solveEquationSystem(dir, x, b);
                        
                        // Set values of resulting vector according to result.
                        storm::utility::vector::setVectorValues<ValueType>(result, maybeStates, x);
                    }
                }
                
                // Set values of resulting vector that are known exactly.
                storm::utility::vector::setVectorValues(result, infinityStates, storm::utility::infinity<ValueType>());
                
                return result;
            }
            
            template<typename ValueType>
            std::vector<ValueType> SparseMdpPrctlHelper<ValueType>::computeLongRunAverage(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& psiStates, bool qualitative, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory) {
                // If there are no goal states, we avoid the computation and directly return zero.
                uint_fast64_t numberOfStates = transitionMatrix.getRowGroupCount();
                if (psiStates.empty()) {
                    return std::vector<ValueType>(numberOfStates, storm::utility::zero<ValueType>());
                }
                
                // Likewise, if all bits are set, we can avoid the computation and set.
                if ((~psiStates).empty()) {
                    return std::vector<ValueType>(numberOfStates, storm::utility::one<ValueType>());
                }
                
                // Start by decomposing the MDP into its MECs.
                storm::storage::MaximalEndComponentDecomposition<double> mecDecomposition(transitionMatrix, backwardTransitions);
                
                // Get some data members for convenience.
                std::vector<uint_fast64_t> const& nondeterministicChoiceIndices = transitionMatrix.getRowGroupIndices();
                ValueType zero = storm::utility::zero<ValueType>();
                
                //first calculate LRA for the Maximal End Components.
                storm::storage::BitVector statesInMecs(numberOfStates);
                std::vector<uint_fast64_t> stateToMecIndexMap(transitionMatrix.getColumnCount());
                std::vector<ValueType> lraValuesForEndComponents(mecDecomposition.size(), zero);
                
                for (uint_fast64_t currentMecIndex = 0; currentMecIndex < mecDecomposition.size(); ++currentMecIndex) {
                    storm::storage::MaximalEndComponent const& mec = mecDecomposition[currentMecIndex];
                    
                    lraValuesForEndComponents[currentMecIndex] = computeLraForMaximalEndComponent(dir, transitionMatrix, psiStates, mec);
                    
                    // Gather information for later use.
                    for (auto const& stateChoicesPair : mec) {
                        statesInMecs.set(stateChoicesPair.first);
                        stateToMecIndexMap[stateChoicesPair.first] = currentMecIndex;
                    }
                }
                
                // For fast transition rewriting, we build some auxiliary data structures.
                storm::storage::BitVector statesNotContainedInAnyMec = ~statesInMecs;
                uint_fast64_t firstAuxiliaryStateIndex = statesNotContainedInAnyMec.getNumberOfSetBits();
                uint_fast64_t lastStateNotInMecs = 0;
                uint_fast64_t numberOfStatesNotInMecs = 0;
                std::vector<uint_fast64_t> statesNotInMecsBeforeIndex;
                statesNotInMecsBeforeIndex.reserve(numberOfStates);
                for (auto state : statesNotContainedInAnyMec) {
                    while (lastStateNotInMecs <= state) {
                        statesNotInMecsBeforeIndex.push_back(numberOfStatesNotInMecs);
                        ++lastStateNotInMecs;
                    }
                    ++numberOfStatesNotInMecs;
                }
                
                // Finally, we are ready to create the SSP matrix and right-hand side of the SSP.
                std::vector<ValueType> b;
                typename storm::storage::SparseMatrixBuilder<ValueType> sspMatrixBuilder(0, 0, 0, false, true, numberOfStatesNotInMecs + mecDecomposition.size());
                
                // If the source state is not contained in any MEC, we copy its choices (and perform the necessary modifications).
                uint_fast64_t currentChoice = 0;
                for (auto state : statesNotContainedInAnyMec) {
                    sspMatrixBuilder.newRowGroup(currentChoice);
                    
                    for (uint_fast64_t choice = nondeterministicChoiceIndices[state]; choice < nondeterministicChoiceIndices[state + 1]; ++choice, ++currentChoice) {
                        std::vector<ValueType> auxiliaryStateToProbabilityMap(mecDecomposition.size());
                        b.push_back(storm::utility::zero<ValueType>());
                        
                        for (auto element : transitionMatrix.getRow(choice)) {
                            if (statesNotContainedInAnyMec.get(element.getColumn())) {
                                // If the target state is not contained in an MEC, we can copy over the entry.
                                sspMatrixBuilder.addNextValue(currentChoice, statesNotInMecsBeforeIndex[element.getColumn()], element.getValue());
                            } else {
                                // If the target state is contained in MEC i, we need to add the probability to the corresponding field in the vector
                                // so that we are able to write the cumulative probability to the MEC into the matrix.
                                auxiliaryStateToProbabilityMap[stateToMecIndexMap[element.getColumn()]] += element.getValue();
                            }
                        }
                        
                        // Now insert all (cumulative) probability values that target an MEC.
                        for (uint_fast64_t mecIndex = 0; mecIndex < auxiliaryStateToProbabilityMap.size(); ++mecIndex) {
                            if (auxiliaryStateToProbabilityMap[mecIndex] != 0) {
                                sspMatrixBuilder.addNextValue(currentChoice, firstAuxiliaryStateIndex + mecIndex, auxiliaryStateToProbabilityMap[mecIndex]);
                            }
                        }
                    }
                }
                
                // Now we are ready to construct the choices for the auxiliary states.
                for (uint_fast64_t mecIndex = 0; mecIndex < mecDecomposition.size(); ++mecIndex) {
                    storm::storage::MaximalEndComponent const& mec = mecDecomposition[mecIndex];
                    sspMatrixBuilder.newRowGroup(currentChoice);
                    
                    for (auto const& stateChoicesPair : mec) {
                        uint_fast64_t state = stateChoicesPair.first;
                        boost::container::flat_set<uint_fast64_t> const& choicesInMec = stateChoicesPair.second;
                        
                        for (uint_fast64_t choice = nondeterministicChoiceIndices[state]; choice < nondeterministicChoiceIndices[state + 1]; ++choice) {
                            std::vector<ValueType> auxiliaryStateToProbabilityMap(mecDecomposition.size());
                            
                            // If the choice is not contained in the MEC itself, we have to add a similar distribution to the auxiliary state.
                            if (choicesInMec.find(choice) == choicesInMec.end()) {
                                b.push_back(storm::utility::zero<ValueType>());
                                
                                for (auto element : transitionMatrix.getRow(choice)) {
                                    if (statesNotContainedInAnyMec.get(element.getColumn())) {
                                        // If the target state is not contained in an MEC, we can copy over the entry.
                                        sspMatrixBuilder.addNextValue(currentChoice, statesNotInMecsBeforeIndex[element.getColumn()], element.getValue());
                                    } else {
                                        // If the target state is contained in MEC i, we need to add the probability to the corresponding field in the vector
                                        // so that we are able to write the cumulative probability to the MEC into the matrix.
                                        auxiliaryStateToProbabilityMap[stateToMecIndexMap[element.getColumn()]] += element.getValue();
                                    }
                                }
                                
                                // Now insert all (cumulative) probability values that target an MEC.
                                for (uint_fast64_t targetMecIndex = 0; targetMecIndex < auxiliaryStateToProbabilityMap.size(); ++targetMecIndex) {
                                    if (auxiliaryStateToProbabilityMap[targetMecIndex] != 0) {
                                        sspMatrixBuilder.addNextValue(currentChoice, firstAuxiliaryStateIndex + targetMecIndex, auxiliaryStateToProbabilityMap[targetMecIndex]);
                                    }
                                }
                                
                                ++currentChoice;
                            }
                        }
                    }
                    
                    // For each auxiliary state, there is the option to achieve the reward value of the LRA associated with the MEC.
                    ++currentChoice;
                    b.push_back(lraValuesForEndComponents[mecIndex]);
                }
                
                // Finalize the matrix and solve the corresponding system of equations.
                storm::storage::SparseMatrix<ValueType> sspMatrix = sspMatrixBuilder.build(currentChoice);
                
                std::vector<ValueType> sspResult(numberOfStatesNotInMecs + mecDecomposition.size());
                std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> solver = minMaxLinearEquationSolverFactory.create(sspMatrix);
                solver->solveEquationSystem(dir, sspResult, b);
                
                // Prepare result vector.
                std::vector<ValueType> result(numberOfStates, zero);
                
                // Set the values for states not contained in MECs.
                storm::utility::vector::setVectorValues(result, statesNotContainedInAnyMec, sspResult);
                
                // Set the values for all states in MECs.
                for (auto state : statesInMecs) {
                    result[state] = sspResult[firstAuxiliaryStateIndex + stateToMecIndexMap[state]];
                }
                
                return result;
            }
            
            template<typename ValueType>
            ValueType SparseMdpPrctlHelper<ValueType>::computeLraForMaximalEndComponent(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& psiStates, storm::storage::MaximalEndComponent const& mec) {
                std::shared_ptr<storm::solver::LpSolver> solver = storm::utility::solver::getLpSolver("LRA for MEC");
                solver->setOptimizationDirection(invert(dir));
                
                // First, we need to create the variables for the problem.
                std::map<uint_fast64_t, storm::expressions::Variable> stateToVariableMap;
                for (auto const& stateChoicesPair : mec) {
                    std::string variableName = "h" + std::to_string(stateChoicesPair.first);
                    stateToVariableMap[stateChoicesPair.first] = solver->addUnboundedContinuousVariable(variableName);
                }
                storm::expressions::Variable lambda = solver->addUnboundedContinuousVariable("L", 1);
                solver->update();
                
                // Now we encode the problem as constraints.
                for (auto const& stateChoicesPair : mec) {
                    uint_fast64_t state = stateChoicesPair.first;
                    
                    // Now, based on the type of the state, create a suitable constraint.
                    for (auto choice : stateChoicesPair.second) {
                        storm::expressions::Expression constraint = -lambda;
                        ValueType r = 0;
                        
                        for (auto element : transitionMatrix.getRow(choice)) {
                            constraint = constraint + stateToVariableMap.at(element.getColumn()) * solver->getConstant(element.getValue());
                            if (psiStates.get(element.getColumn())) {
                                r += element.getValue();
                            }
                        }
                        constraint = solver->getConstant(r) + constraint;
                        
                        if (dir == OptimizationDirection::Minimize) {
                            constraint = stateToVariableMap.at(state) <= constraint;
                        } else {
                            constraint = stateToVariableMap.at(state) >= constraint;
                        }
                        solver->addConstraint("state" + std::to_string(state) + "," + std::to_string(choice), constraint);
                    }
                }
                
                solver->optimize();
                return solver->getContinuousValue(lambda);
            }
            
            template class SparseMdpPrctlHelper<double>;
            template std::vector<double> SparseMdpPrctlHelper<double>::computeInstantaneousRewards(OptimizationDirection dir, storm::storage::SparseMatrix<double> const& transitionMatrix, storm::models::sparse::StandardRewardModel<double> const& rewardModel, uint_fast64_t stepCount, storm::utility::solver::MinMaxLinearEquationSolverFactory<double> const& minMaxLinearEquationSolverFactory);
            template std::vector<double> SparseMdpPrctlHelper<double>::computeCumulativeRewards(OptimizationDirection dir, storm::storage::SparseMatrix<double> const& transitionMatrix, storm::models::sparse::StandardRewardModel<double> const& rewardModel, uint_fast64_t stepBound, storm::utility::solver::MinMaxLinearEquationSolverFactory<double> const& minMaxLinearEquationSolverFactory);
            template std::vector<double> SparseMdpPrctlHelper<double>::computeReachabilityRewards(OptimizationDirection dir, storm::storage::SparseMatrix<double> const& transitionMatrix, storm::storage::SparseMatrix<double> const& backwardTransitions, storm::models::sparse::StandardRewardModel<double> const& rewardModel, storm::storage::BitVector const& targetStates, bool qualitative, storm::utility::solver::MinMaxLinearEquationSolverFactory<double> const& minMaxLinearEquationSolverFactory);
            
        }
    }
}
