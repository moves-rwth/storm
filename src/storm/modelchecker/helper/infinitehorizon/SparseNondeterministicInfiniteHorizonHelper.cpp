#include "SparseNondeterministicInfiniteHorizonHelper.h"

#include "storm/solver/MinMaxLinearEquationSolver.h"
#include "storm/solver/Multiplier.h"
#include "storm/solver/LpSolver.h"

#include "storm/utility/graph.h"
#include "storm/utility/SignalHandler.h"
#include "storm/utility/solver.h"
#include "storm/utility/vector.h"

#include "storm/environment/solver/LongRunAverageSolverEnvironment.h"
#include "storm/environment/solver/MinMaxSolverEnvironment.h"

#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/UnmetRequirementException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
        
            template <typename ValueType>
            SparseNondeterministicInfiniteHorizonHelper<ValueType>::SparseNondeterministicInfiniteHorizonHelper(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions) : _transitionMatrix(transitionMatrix), _backwardTransitions(backwardTransitions), _markovianStates(nullptr), _exitRates(nullptr), _produceScheduler(false) {
                // Intentionally left empty.
            }
            
            template <typename ValueType>
            SparseNondeterministicInfiniteHorizonHelper<ValueType>::SparseNondeterministicInfiniteHorizonHelper(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& markovianStates, std::vector<ValueType> const& exitRates) : _transitionMatrix(transitionMatrix), _backwardTransitions(backwardTransitions), _markovianStates(&markovianStates), _exitRates(&exitRates) {
                // Intentionally left empty.
            }
            
            template <typename ValueType>
            std::vector<ValueType> SparseNondeterministicInfiniteHorizonHelper<ValueType>::computeLongRunAverageProbabilities(Environment const& env, storm::storage::BitVector const& psiStates) {
                return computeLongRunAverageValues(env, [&psiStates] (uint64_t stateIndex, uint64_t) { return psiStates.get(stateIndex) ? storm::utility::one<ValueType>() : storm::utility::zero<ValueType>();});
            }
            
            
            template <typename ValueType>
            std::vector<ValueType> SparseNondeterministicInfiniteHorizonHelper<ValueType>::computeLongRunAverageRewards(Environment const& env, storm::models::sparse::StandardRewardModel<ValueType> const& rewardModel) {
                if (_markovianStates) {
                    return computeLongRunAverageValues(env, [&] (uint64_t stateIndex, uint64_t globalChoiceIndex) {
                        if (rewardModel.hasStateRewards() && _markovianStates->get(stateIndex)) {
                            return rewardModel.getTotalStateActionReward(stateIndex, globalChoiceIndex, _transitionMatrix, (ValueType) (storm::utility::one<ValueType>() / (*_exitRates)[stateIndex]));
                        } else {
                            return rewardModel.getTotalStateActionReward(stateIndex, globalChoiceIndex, _transitionMatrix, storm::utility::zero<ValueType>());
                        }
                    });
                } else {
                    return computeLongRunAverageValues(env, [&] (uint64_t stateIndex, uint64_t globalChoiceIndex) {
                        return rewardModel.getTotalStateActionReward(stateIndex, globalChoiceIndex, _transitionMatrix);
                    });
                }
            }
            
            template <typename ValueType>
            std::vector<ValueType> SparseNondeterministicInfiniteHorizonHelper<ValueType>::computeLongRunAverageValues(Environment const& env, std::vector<ValueType> const& combinedStateActionRewards) {
                return computeLongRunAverageValues(env, [&combinedStateActionRewards] (uint64_t, uint64_t globalChoiceIndex) {
                    return combinedStateActionRewards[globalChoiceIndex];
                });
            }
            
            template <typename ValueType>
            std::vector<ValueType> SparseNondeterministicInfiniteHorizonHelper<ValueType>::computeLongRunAverageValues(Environment const& env, std::function<ValueType(uint64_t stateIndex, uint64_t globalChoiceIndex)> const& combinedStateActionRewardsGetter) {
                
                 // Prepare an environment for the underlying solvers
                auto underlyingSolverEnvironment = env;
                if (env.solver().isForceSoundness()) {
                    // For sound computations, the error in the MECS plus the error in the remaining system should not exceed the user defined precsion.
                    underlyingSolverEnvironment.solver().minMax().setPrecision(env.solver().lra().getPrecision() / storm::utility::convertNumber<storm::RationalNumber>(2));
                    underlyingSolverEnvironment.solver().minMax().setRelativeTerminationCriterion(env.solver().lra().getRelativeTerminationCriterion());
                    underlyingSolverEnvironment.solver().lra().setPrecision(env.solver().lra().getPrecision() / storm::utility::convertNumber<storm::RationalNumber>(2));
                }
                
                // If requested, allocate memory for the choices made
                if (isProduceSchedulerSet()) {
                    if (!_producedOptimalChoices.is_initialized()) {
                        _producedOptimalChoices.emplace();
                    }
                    _producedOptimalChoices->resize(_transitionMatrix.getRowGroupCount());
                }
                
                // Start by decomposing the Model into its MECs.
                storm::storage::MaximalEndComponentDecomposition<ValueType> mecDecomposition(_transitionMatrix, _backwardTransitions);

                // Compute the long-run average for all end components in isolation.
                std::vector<ValueType> mecLraValues;
                mecLraValues.reserve(mecDecomposition.size());
                for (auto const& mec : mecDecomposition) {
                    mecLraValues.push_back(computeLraForMec(underlyingSolverEnvironment, combinedStateActionRewardsGetter, mec));
                }
                
                // Solve the resulting SSP where end components are collapsed into single auxiliary states
                return buildAndSolveSsp(underlyingSolverEnvironment, mecDecomposition, mecLraValues);
            }
            
            
            template <typename ValueType>
            void SparseNondeterministicInfiniteHorizonHelper<ValueType>::setProduceScheduler(bool value) {
                _produceScheduler = value;
            }
            
            template <typename ValueType>
            bool SparseNondeterministicInfiniteHorizonHelper<ValueType>::isProduceSchedulerSet() const {
                return _produceScheduler;
            }
            
            template <typename ValueType>
            std::vector<uint64_t> const& SparseNondeterministicInfiniteHorizonHelper<ValueType>::getProducedOptimalChoices() const {
                STORM_LOG_ASSERT(isProduceSchedulerSet(), "Trying to get the produced optimal choices although no scheduler was requested.");
                STORM_LOG_ASSERT(_producedOptimalChoices.is_initialized(), "Trying to get the produced optimal choices but none were available. Was there a computation call before?");
                return _producedOptimalChoices.get();
            }
            
            template <typename ValueType>
            std::vector<uint64_t>& SparseNondeterministicInfiniteHorizonHelper<ValueType>::getProducedOptimalChoices() {
                STORM_LOG_ASSERT(isProduceSchedulerSet(), "Trying to get the produced optimal choices although no scheduler was requested.");
                STORM_LOG_ASSERT(_producedOptimalChoices.is_initialized(), "Trying to get the produced optimal choices but none were available. Was there a computation call before?");
                return _producedOptimalChoices.get();
            }
            
            template <typename ValueType>
            storm::storage::Scheduler<ValueType> SparseNondeterministicInfiniteHorizonHelper<ValueType>::extractScheduler() const {
                auto const& optimalChoices = getProducedOptimalChoices();
                storm::storage::Scheduler<ValueType> scheduler(optimalChoices.size());
                for (uint64_t state = 0; state < optimalChoices.size(); ++state) {
                        scheduler.setChoice(optimalChoices[state], state);
                }
                return scheduler;
            }
            
            template <typename ValueType>
            ValueType SparseNondeterministicInfiniteHorizonHelper<ValueType>::computeLraForMec(Environment const& env, std::function<ValueType(uint64_t stateIndex, uint64_t globalChoiceIndex)> const& combinedStateActionRewardsGetter, storm::storage::MaximalEndComponent const& mec) {
                
                // FIXME: MA
                // If the mec only consists of a single state, we compute the LRA value directly
                if (++mec.begin() == mec.end()) {
                    uint64_t state = mec.begin()->first;
                    auto choiceIt = mec.begin()->second.begin();
                    ValueType result = combinedStateActionRewardsGetter(state, *choiceIt);
                    uint64_t bestChoice = *choiceIt;
                    for (++choiceIt; choiceIt != mec.begin()->second.end(); ++choiceIt) {
                        ValueType choiceValue = combinedStateActionRewardsGetter(state, *choiceIt);
                        if (this->minimize()) {
                            if (result > choiceValue) {
                                result = std::move(choiceValue);
                                bestChoice = *choiceIt;
                            }
                        } else {
                             if (result < choiceValue) {
                                    result = std::move(choiceValue);
                                    bestChoice = *choiceIt;
                             }
                        }
                    }
                    if (isProduceSchedulerSet()) {
                        _producedOptimalChoices.get()[state] = bestChoice - _transitionMatrix.getRowGroupIndices()[state];
                    }
                    return result;
                }
                
                // Solve MEC with the method specified in the settings
                storm::solver::LraMethod method = env.solver().lra().getNondetLraMethod();
                if ((storm::NumberTraits<ValueType>::IsExact || env.solver().isForceExact()) && env.solver().lra().isNondetLraMethodSetFromDefault() && method != storm::solver::LraMethod::LinearProgramming) {
                    STORM_LOG_INFO("Selecting 'LP' as the solution technique for long-run properties to guarantee exact results. If you want to override this, please explicitly specify a different LRA method.");
                    method = storm::solver::LraMethod::LinearProgramming;
                } else if (env.solver().isForceSoundness() && env.solver().lra().isNondetLraMethodSetFromDefault() && method != storm::solver::LraMethod::ValueIteration) {
                    STORM_LOG_INFO("Selecting 'VI' as the solution technique for long-run properties to guarantee sound results. If you want to override this, please explicitly specify a different LRA method.");
                    method = storm::solver::LraMethod::ValueIteration;
                }
                STORM_LOG_ERROR_COND(!isProduceSchedulerSet() || method == storm::solver::LraMethod::ValueIteration, "Scheduler generation not supported for the chosen LRA method. Try value-iteration.");
                if (method == storm::solver::LraMethod::LinearProgramming) {
                    return computeLraForMecLp(env, combinedStateActionRewardsGetter, mec);
                } else if (method == storm::solver::LraMethod::ValueIteration) {
                    return computeLraForMecVi(env, combinedStateActionRewardsGetter, mec);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Unsupported technique.");
                }
            }
            
            template <typename ValueType>
            ValueType SparseNondeterministicInfiniteHorizonHelper<ValueType>::computeLraForMecVi(Environment const& env, std::function<ValueType(uint64_t stateIndex, uint64_t globalChoiceIndex)> const& combinedStateActionRewardsGetter, storm::storage::MaximalEndComponent const& mec) {
                // Initialize data about the mec
                storm::storage::BitVector mecStates(_transitionMatrix.getRowGroupCount(), false);
                storm::storage::BitVector mecChoices(_transitionMatrix.getRowCount(), false);
                for (auto const& stateChoicesPair : mec) {
                    mecStates.set(stateChoicesPair.first);
                    for (auto const& choice : stateChoicesPair.second) {
                        mecChoices.set(choice);
                    }
                }
                
                boost::container::flat_map<uint64_t, uint64_t> toSubModelStateMapping;
                uint64_t currState = 0;
                toSubModelStateMapping.reserve(mecStates.getNumberOfSetBits());
                for (auto const& mecState : mecStates) {
                    toSubModelStateMapping.insert(std::pair<uint64_t, uint64_t>(mecState, currState));
                    ++currState;
                }
                
                // Get a transition matrix that only considers the states and choices within the MEC
                storm::storage::SparseMatrixBuilder<ValueType> mecTransitionBuilder(mecChoices.getNumberOfSetBits(), mecStates.getNumberOfSetBits(), 0, true, true, mecStates.getNumberOfSetBits());
                std::vector<ValueType> choiceValues;
                choiceValues.reserve(mecChoices.getNumberOfSetBits());
                uint64_t currRow = 0;
                ValueType selfLoopProb = storm::utility::convertNumber<ValueType>(env.solver().lra().getAperiodicFactor());
                ValueType scalingFactor = storm::utility::one<ValueType>() - selfLoopProb;
                for (auto const& mecState : mecStates) {
                    mecTransitionBuilder.newRowGroup(currRow);
                    uint64_t groupStart = _transitionMatrix.getRowGroupIndices()[mecState];
                    uint64_t groupEnd = _transitionMatrix.getRowGroupIndices()[mecState + 1];
                    for (uint64_t choice = mecChoices.getNextSetIndex(groupStart); choice < groupEnd; choice = mecChoices.getNextSetIndex(choice + 1)) {
                        bool insertedDiagElement = false;
                        for (auto const& entry : _transitionMatrix.getRow(choice)) {
                            uint64_t column = toSubModelStateMapping[entry.getColumn()];
                            if (!insertedDiagElement && entry.getColumn() > mecState) {
                                mecTransitionBuilder.addNextValue(currRow, toSubModelStateMapping[mecState], selfLoopProb);
                                insertedDiagElement = true;
                            }
                            if (!insertedDiagElement && entry.getColumn() == mecState) {
                                mecTransitionBuilder.addNextValue(currRow, column, selfLoopProb + scalingFactor * entry.getValue());
                                insertedDiagElement = true;
                            } else {
                                mecTransitionBuilder.addNextValue(currRow, column,  scalingFactor * entry.getValue());
                            }
                        }
                        if (!insertedDiagElement) {
                            mecTransitionBuilder.addNextValue(currRow, toSubModelStateMapping[mecState], selfLoopProb);
                        }
                        
                        // Compute the rewards obtained for this choice
                        choiceValues.push_back(scalingFactor * combinedStateActionRewardsGetter(mecState, choice));
                        
                        ++currRow;
                    }
                }
                auto mecTransitions = mecTransitionBuilder.build();
                STORM_LOG_ASSERT(mecTransitions.isProbabilistic(), "The MEC-Matrix is not probabilistic.");
                
                // start the iterations
                ValueType precision = storm::utility::convertNumber<ValueType>(env.solver().lra().getPrecision()) / scalingFactor;
                bool relative = env.solver().lra().getRelativeTerminationCriterion();
                std::vector<ValueType> x(mecTransitions.getRowGroupCount(), storm::utility::zero<ValueType>());
                std::vector<ValueType> xPrime = x;
                auto dir = this->getOptimizationDirection();
                
                auto multiplier = storm::solver::MultiplierFactory<ValueType>().create(env, mecTransitions);
                ValueType maxDiff, minDiff;
                
                uint64_t iter = 0;
                boost::optional<uint64_t> maxIter;
                if (env.solver().lra().isMaximalIterationCountSet()) {
                    maxIter = env.solver().lra().getMaximalIterationCount();
                }
                while (!maxIter.is_initialized() || iter < maxIter.get()) {
                    ++iter;
                    // Compute the obtained values for the next step
                    multiplier->multiplyAndReduce(env, dir, x, &choiceValues, x);
                    
                    // update xPrime and check for convergence
                    // to avoid large (and numerically unstable) x-values, we substract a reference value.
                    auto xIt = x.begin();
                    auto xPrimeIt = xPrime.begin();
                    ValueType refVal = *xIt;
                    maxDiff = *xIt - *xPrimeIt;
                    minDiff = maxDiff;
                    *xIt -= refVal;
                    *xPrimeIt = *xIt;
                    for (++xIt, ++xPrimeIt; xIt != x.end(); ++xIt, ++xPrimeIt) {
                        ValueType diff = *xIt - *xPrimeIt;
                        maxDiff = std::max(maxDiff, diff);
                        minDiff = std::min(minDiff, diff);
                        *xIt -= refVal;
                        *xPrimeIt = *xIt;
                    }

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
                
                if (isProduceSchedulerSet()) {
                    std::vector<uint_fast64_t> localMecChoices(mecTransitions.getRowGroupCount(), 0);
                    multiplier->multiplyAndReduce(env, dir, x, &choiceValues, x, &localMecChoices);
                    auto localMecChoiceIt = localMecChoices.begin();
                    for (auto const& mecState : mecStates) {
                        // Get the choice index of the selected mec choice with respect to the global transition matrix.
                        uint_fast64_t globalChoice = mecChoices.getNextSetIndex(_transitionMatrix.getRowGroupIndices()[mecState]);
                        for (uint_fast64_t i = 0; i < *localMecChoiceIt; ++i) {
                            globalChoice = mecChoices.getNextSetIndex(globalChoice + 1);
                        }
                        STORM_LOG_ASSERT(globalChoice < _transitionMatrix.getRowGroupIndices()[mecState + 1], "Invalid global choice for mec state.");
                        _producedOptimalChoices.get()[mecState] = globalChoice - _transitionMatrix.getRowGroupIndices()[mecState];
                        ++localMecChoiceIt;
                    }
                }
                return (maxDiff + minDiff) / (storm::utility::convertNumber<ValueType>(2.0) * scalingFactor);

            }
            
            template <typename ValueType>
            ValueType SparseNondeterministicInfiniteHorizonHelper<ValueType>::computeLraForMecLp(Environment const& env, std::function<ValueType(uint64_t stateIndex, uint64_t globalChoiceIndex)> const& combinedStateActionRewardsGetter, storm::storage::MaximalEndComponent const& mec) {
                std::shared_ptr<storm::solver::LpSolver<ValueType>> solver = storm::utility::solver::getLpSolver<ValueType>("LRA for MEC");
                solver->setOptimizationDirection(invert(this->getOptimizationDirection()));
                
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
                        
                        for (auto element : _transitionMatrix.getRow(choice)) {
                            constraint = constraint + stateToVariableMap.at(element.getColumn()) * solver->getConstant(element.getValue());
                        }
                        constraint = solver->getConstant(combinedStateActionRewardsGetter(state, choice)) + constraint;
                        
                        if (this->minimize()) {
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
            
            /*!
             * Auxiliary function that adds the entries of the Ssp Matrix for a single choice (i.e., row)
             * Transitions that lead to a MEC state will be redirected to a new auxiliary state (there is one aux. state for each MEC).
             * Transitions that don't lead to a MEC state are copied (taking a state index mapping into account).
             */
            template <typename ValueType>
            void addSspMatrixChoice(uint64_t const& inputMatrixChoice, storm::storage::SparseMatrix<ValueType> const& inputTransitionMatrix, std::vector<uint64_t> const& inputToSspStateMap, uint64_t const& numberOfStatesNotInMecs, uint64_t const& currentSspChoice, storm::storage::SparseMatrixBuilder<ValueType>& sspMatrixBuilder) {
            
                // As there could be multiple transitions to the same MEC, we accumulate them in this map before adding them to the matrix builder.
                std::map<uint64_t, ValueType> auxiliaryStateToProbabilityMap;
                
                for (auto transition : inputTransitionMatrix.getRow(inputMatrixChoice)) {
                    if (!storm::utility::isZero(transition.getValue())) {
                        auto const& sspTransitionTarget = inputToSspStateMap[transition.getColumn()];
                        // Since the auxiliary MEC states are appended at the end of the matrix, we can use this check to
                        // decide whether the transition leads to a MEC state or not
                        if (sspTransitionTarget < numberOfStatesNotInMecs) {
                            // If the target state is not contained in a MEC, we can copy over the entry.
                            sspMatrixBuilder.addNextValue(currentSspChoice, sspTransitionTarget, transition.getValue());
                        } else {
                            // If the target state is contained in MEC i, we need to add the probability to the corresponding field in the vector
                            // so that we are able to write the cumulative probability to the MEC into the matrix.
                            auto insertionRes = auxiliaryStateToProbabilityMap.emplace(sspTransitionTarget, transition.getValue());
                            if (!insertionRes.second) {
                                // sspTransitionTarget already existed in the map, i.e., there already was a transition to that MEC.
                                // Hence, we add up the probabilities.
                                insertionRes.first->second += transition.getValue();
                            }
                        }
                    }
                }
                
                // Now insert all (cumulative) probability values that target a MEC.
                for (auto const& mecToProbEntry : auxiliaryStateToProbabilityMap) {
                    sspMatrixBuilder.addNextValue(currentSspChoice, mecToProbEntry.first, mecToProbEntry.second);
                }
            }
            
            template <typename ValueType>
            std::vector<ValueType> SparseNondeterministicInfiniteHorizonHelper<ValueType>::buildAndSolveSsp(Environment const& env, storm::storage::MaximalEndComponentDecomposition<ValueType> const& mecDecomposition, std::vector<ValueType> const& mecLraValues) {
                
                // Let's improve readability a bit
                uint64_t numberOfStates = _transitionMatrix.getRowGroupCount();
                auto const& nondeterministicChoiceIndices = _transitionMatrix.getRowGroupIndices();
                
                // For fast transition rewriting, we build a mapping from the input state indices to the state indices of a new transition matrix
                // which redirects all transitions leading to a former MEC state to a new auxiliary state.
                // There will be one auxiliary state for each MEC. These states will be appended to the end of the matrix.
                
                // First gather the states that are part of a MEC
                // and create a mapping from states that lie in a MEC to the corresponding MEC index.
                storm::storage::BitVector statesInMecs(numberOfStates);
                std::vector<uint64_t> inputToSspStateMap(numberOfStates, std::numeric_limits<uint64_t>::max());
                for (uint64_t currentMecIndex = 0; currentMecIndex < mecDecomposition.size(); ++currentMecIndex) {
                    for (auto const& stateChoicesPair : mecDecomposition[currentMecIndex]) {
                        statesInMecs.set(stateChoicesPair.first);
                        inputToSspStateMap[stateChoicesPair.first] = currentMecIndex;
                    }
                }
                // Now take care of the non-mec states. Note that the order of these states will be preserved.
                uint64_t numberOfStatesNotInMecs = 0;
                storm::storage::BitVector statesNotContainedInAnyMec = ~statesInMecs;
                for (auto const& nonMecState : statesNotContainedInAnyMec) {
                    inputToSspStateMap[nonMecState] = numberOfStatesNotInMecs;
                    ++numberOfStatesNotInMecs;
                }
                // Finalize the mapping for the mec states which now still assigns mec states to to their Mec index.
                // To make sure that they point to the auxiliary states (located at the end of the SspMatrix), we need to shift them by the
                // number of states that are not in a mec.
                for (auto const& mecState : statesInMecs) {
                    inputToSspStateMap[mecState] += numberOfStatesNotInMecs;
                }
                
                // For scheduler extraction, we will need to create a mapping between choices at the auxiliary states and the
                // corresponding choices in the original model.
                std::vector<std::pair<uint_fast64_t, uint_fast64_t>> sspMecExitChoicesToOriginalMap;
                
                // The next step is to create the SSP matrix and the right-hand side of the SSP.
                std::vector<ValueType> rhs;
                uint64_t numberOfSspStates = numberOfStatesNotInMecs + mecDecomposition.size();
                typename storm::storage::SparseMatrixBuilder<ValueType> sspMatrixBuilder(0, numberOfSspStates , 0, false, true, numberOfSspStates);
                // If the source state of a transition is not contained in any MEC, we copy its choices (and perform the necessary modifications).
                uint64_t currentSspChoice = 0;
                for (auto const& nonMecState : statesNotContainedInAnyMec) {
                    sspMatrixBuilder.newRowGroup(currentSspChoice);
                    
                    for (uint64_t choice = nondeterministicChoiceIndices[nonMecState]; choice < nondeterministicChoiceIndices[nonMecState + 1]; ++choice, ++currentSspChoice) {
                        rhs.push_back(storm::utility::zero<ValueType>());
                        addSspMatrixChoice(choice, _transitionMatrix, inputToSspStateMap, numberOfStatesNotInMecs, currentSspChoice, sspMatrixBuilder);
                    }
                }
                // Now we construct the choices for the auxiliary states which reflect former MEC states.
                for (uint64_t mecIndex = 0; mecIndex < mecDecomposition.size(); ++mecIndex) {
                    storm::storage::MaximalEndComponent const& mec = mecDecomposition[mecIndex];
                    sspMatrixBuilder.newRowGroup(currentSspChoice);
                    for (auto const& stateChoicesPair : mec) {
                        uint64_t const& mecState = stateChoicesPair.first;
                        auto const& choicesInMec = stateChoicesPair.second;
                        for (uint64_t choice = nondeterministicChoiceIndices[mecState]; choice < nondeterministicChoiceIndices[mecState + 1]; ++choice) {
                            // If the choice is not contained in the MEC itself, we have to add a similar distribution to the auxiliary state.
                            if (choicesInMec.find(choice) == choicesInMec.end()) {
                                rhs.push_back(storm::utility::zero<ValueType>());
                                addSspMatrixChoice(choice, _transitionMatrix, inputToSspStateMap, numberOfStatesNotInMecs, currentSspChoice, sspMatrixBuilder);
                                if (isProduceSchedulerSet()) {
                                    // Later we need to be able to map this choice back to the original input model
                                    sspMecExitChoicesToOriginalMap.emplace_back(mecState, choice - nondeterministicChoiceIndices[mecState]);
                                }
                                ++currentSspChoice;
                            }
                        }
                    }
                    // For each auxiliary state, there is the option to achieve the reward value of the LRA associated with the MEC.
                    rhs.push_back(mecLraValues[mecIndex]);
                    if (isProduceSchedulerSet()) {
                        // Insert some invalid values so we can later detect that this choice is not an exit choice
                        sspMecExitChoicesToOriginalMap.emplace_back(std::numeric_limits<uint_fast64_t>::max(), std::numeric_limits<uint_fast64_t>::max());
                    }
                    ++currentSspChoice;
                }
                storm::storage::SparseMatrix<ValueType> sspMatrix = sspMatrixBuilder.build(currentSspChoice, numberOfSspStates, numberOfSspStates);
                
                // Set-up a solver
                storm::solver::GeneralMinMaxLinearEquationSolverFactory<ValueType> minMaxLinearEquationSolverFactory;
                storm::solver::MinMaxLinearEquationSolverRequirements requirements = minMaxLinearEquationSolverFactory.getRequirements(env, true, true, this->getOptimizationDirection(), false, this->isProduceSchedulerSet());
                requirements.clearBounds();
                STORM_LOG_THROW(!requirements.hasEnabledCriticalRequirement(), storm::exceptions::UnmetRequirementException, "Solver requirements " + requirements.getEnabledRequirementsAsString() + " not checked.");
                std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> solver = minMaxLinearEquationSolverFactory.create(env, sspMatrix);
                solver->setHasUniqueSolution();
                solver->setHasNoEndComponents();
                solver->setTrackScheduler(isProduceSchedulerSet());
                auto lowerUpperBounds = std::minmax_element(mecLraValues.begin(), mecLraValues.end());
                solver->setLowerBound(*lowerUpperBounds.first);
                solver->setUpperBound(*lowerUpperBounds.second);
                solver->setRequirementsChecked();
                
                // Solve the equation system
                std::vector<ValueType> x(numberOfSspStates);
                solver->solveEquations(env, this->getOptimizationDirection(), x, rhs);

                // Prepare scheduler (if requested)
                if (isProduceSchedulerSet() && solver->hasScheduler()) {
                    // Translate result for ssp matrix to original model
                    auto const& sspChoices = solver->getSchedulerChoices();
                    // We first take care of non-mec states
                    storm::utility::vector::setVectorValues(_producedOptimalChoices.get(), statesNotContainedInAnyMec, sspChoices);
                    // Secondly, we consider MEC states. There are 3 cases for each MEC state:
                    // 1. The SSP choices encode that we want to stay in the MEC
                    // 2. The SSP choices encode that we want to leave the MEC and
                    //      a) we take an exit (non-MEC) choice at the given state
                    //      b) we have to take a MEC choice at the given state in a way that eventually an exit state of the MEC is reached
                    uint64_t exitChoiceOffset = sspMatrix.getRowGroupIndices()[numberOfStatesNotInMecs];
                    for (auto const& mec : mecDecomposition) {
                        // Get the sspState of this MEC (using one representative mec state)
                        auto const& sspState = inputToSspStateMap[mec.begin()->first];
                        uint64_t sspChoiceIndex = sspMatrix.getRowGroupIndices()[sspState] + sspChoices[sspState];
                        // Obtain the state and choice of the original model to which the selected choice corresponds.
                        auto const& originalStateChoice = sspMecExitChoicesToOriginalMap[sspChoiceIndex - exitChoiceOffset];
                        // Check if we are in Case 1 or 2
                        if (originalStateChoice.first == std::numeric_limits<uint_fast64_t>::max()) {
                            // The optimal choice is to stay in this mec (Case 1)
                            // In this case, no further operations are necessary. The scheduler has already been set to the optimal choices during the call of computeLraForMec.
                            STORM_LOG_ASSERT(sspMatrix.getRow(sspState, sspChoices[sspState]).getNumberOfEntries() == 0, "Expected empty row at choice that stays in MEC.");
                        } else {
                            // The best choice is to leave this MEC via the selected state and choice. (Case 2)
                            // Set the exit choice (Case 2.a)
                            _producedOptimalChoices.get()[originalStateChoice.first] = originalStateChoice.second;
                            // The remaining states in this MEC need to reach the state with the exit choice with probability 1. (Case 2.b)
                            // Perform a backwards search from the exit state, only using MEC choices
                            // We start by setting an invalid choice to all remaining mec states (so that we can easily detect them as unprocessed)
                            for (auto const& stateActions : mec) {
                                if (stateActions.first != originalStateChoice.first) {
                                    _producedOptimalChoices.get()[stateActions.first] = std::numeric_limits<uint64_t>::max();
                                }
                            }
                            // Now start a backwards DFS
                            std::vector<uint64_t> stack = {originalStateChoice.first};
                            while (!stack.empty()) {
                                uint64_t currentState = stack.back();
                                stack.pop_back();
                                for (auto const& backwardsTransition : _backwardTransitions.getRowGroup(currentState)) {
                                    uint64_t predecessorState = backwardsTransition.getColumn();
                                    if (mec.containsState(predecessorState)) {
                                        auto& selectedPredChoice = _producedOptimalChoices.get()[predecessorState];
                                        if (selectedPredChoice == std::numeric_limits<uint64_t>::max()) {
                                            // We don't already have a choice for this predecessor.
                                            // We now need to check whether there is a *MEC* choice leading to currentState
                                            for (auto const& predChoice : mec.getChoicesForState(predecessorState)) {
                                                for (auto const& forwardTransition : _transitionMatrix.getRow(predChoice)) {
                                                    if (forwardTransition.getColumn() == currentState && !storm::utility::isZero(forwardTransition.getValue())) {
                                                        // Playing this choice (infinitely often) will lead to current state (infinitely often)!
                                                        selectedPredChoice = predChoice - nondeterministicChoiceIndices[predecessorState];
                                                        stack.push_back(predecessorState);
                                                        break;
                                                    }
                                                }
                                                if (selectedPredChoice != std::numeric_limits<uint64_t>::max()) {
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                } else {
                    STORM_LOG_ERROR_COND(!isProduceSchedulerSet(), "Requested to produce a scheduler, but no scheduler was generated.");
                }
                
                // Prepare result vector.
                // For efficiency reasons, we re-use the memory of our rhs for this!
                std::vector<ValueType> result = std::move(rhs);
                result.resize(numberOfStates);
                result.shrink_to_fit();
                storm::utility::vector::selectVectorValues(result, inputToSspStateMap, x);
                return result;
            }
            
            template class SparseNondeterministicInfiniteHorizonHelper<double>;
            template class SparseNondeterministicInfiniteHorizonHelper<storm::RationalNumber>;
        }
    }
}