#include "SparseNondeterministicInfiniteHorizonHelper.h"

#include "storm/modelchecker/helper/infinitehorizon/internal/ComponentUtility.h"
#include "storm/modelchecker/helper/infinitehorizon/internal/LraViHelper.h"

#include "storm/models/sparse/StandardRewardModel.h"

#include "storm/storage/SparseMatrix.h"
#include "storm/storage/MaximalEndComponentDecomposition.h"
#include "storm/storage/StronglyConnectedComponentDecomposition.h"

#include "storm/solver/MinMaxLinearEquationSolver.h"
#include "storm/solver/LinearEquationSolver.h"
#include "storm/solver/Multiplier.h"
#include "storm/solver/LpSolver.h"

#include "storm/utility/SignalHandler.h"
#include "storm/utility/solver.h"
#include "storm/utility/vector.h"

#include "storm/environment/solver/LongRunAverageSolverEnvironment.h"
#include "storm/environment/solver/MinMaxSolverEnvironment.h"

#include "storm/exceptions/UnmetRequirementException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
        
            template <typename ValueType, bool Nondeterministic>
            SparseNondeterministicInfiniteHorizonHelper<ValueType, Nondeterministic>::SparseNondeterministicInfiniteHorizonHelper(storm::storage::SparseMatrix<ValueType> const& transitionMatrix) : _transitionMatrix(transitionMatrix), _backwardTransitions(nullptr), _longRunComponentDecomposition(nullptr), _markovianStates(nullptr), _exitRates(nullptr) {
                // Intentionally left empty.
            }
            
            template <typename ValueType, bool Nondeterministic>
            SparseNondeterministicInfiniteHorizonHelper<ValueType, Nondeterministic>::SparseNondeterministicInfiniteHorizonHelper(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& markovianStates, std::vector<ValueType> const& exitRates) : _transitionMatrix(transitionMatrix), _backwardTransitions(nullptr), _longRunComponentDecomposition(nullptr), _markovianStates(&markovianStates), _exitRates(&exitRates) {
                // Intentionally left empty.
            }
            
            template <typename ValueType, bool Nondeterministic>
            void SparseNondeterministicInfiniteHorizonHelper<ValueType, Nondeterministic>::provideBackwardTransitions(storm::storage::SparseMatrix<ValueType> const& backwardTransitions) {
                STORM_LOG_WARN_COND(_backwardTransitions == nullptr, "Backwards transitions were provided but they were already computed or provided before.");
                _backwardTransitions = &backwardTransitions;
            }
            
            template <typename ValueType, bool Nondeterministic>
            void SparseNondeterministicInfiniteHorizonHelper<ValueType, Nondeterministic>::provideLongRunComponentDecomposition(storm::storage::Decomposition<LongRunComponent> const& decomposition) {
                STORM_LOG_WARN_COND(_longRunComponentDecomposition == nullptr, "Long Run Component Decomposition was provided but it was already computed or provided before.");
                _longRunComponentDecomposition = &decomposition;
            }
            
            template <typename ValueType, bool Nondeterministic>
            std::vector<ValueType> SparseNondeterministicInfiniteHorizonHelper<ValueType, Nondeterministic>::computeLongRunAverageProbabilities(Environment const& env, storm::storage::BitVector const& psiStates) {
                return computeLongRunAverageValues(env,
                            [&psiStates] (uint64_t stateIndex) { return psiStates.get(stateIndex) ? storm::utility::one<ValueType>() : storm::utility::zero<ValueType>(); },
                            [] (uint64_t) { return storm::utility::zero<ValueType>(); }
                    );
            }
            
            template <typename ValueType, bool Nondeterministic>
            std::vector<ValueType> SparseNondeterministicInfiniteHorizonHelper<ValueType, Nondeterministic>::computeLongRunAverageRewards(Environment const& env, storm::models::sparse::StandardRewardModel<ValueType> const& rewardModel) {
                std::function<ValueType(uint64_t stateIndex)> stateRewardsGetter;
                if (rewardModel.hasStateRewards()) {
                    stateRewardsGetter = [&rewardModel] (uint64_t stateIndex) { return rewardModel.getStateReward(stateIndex); };
                } else {
                    stateRewardsGetter = [] (uint64_t) { return storm::utility::zero<ValueType>(); };
                }
                std::function<ValueType(uint64_t globalChoiceIndex)> actionRewardsGetter;
                if (rewardModel.hasStateActionRewards() || rewardModel.hasTransitionRewards()) {
                    if (rewardModel.hasTransitionRewards()) {
                        actionRewardsGetter = [&] (uint64_t globalChoiceIndex) { return rewardModel.getStateActionAndTransitionReward(globalChoiceIndex, this->_transitionMatrix); };
                    } else {
                        actionRewardsGetter = [&] (uint64_t globalChoiceIndex) { return rewardModel.getStateActionReward(globalChoiceIndex); };
                    }
                } else {
                    stateRewardsGetter = [] (uint64_t) { return storm::utility::zero<ValueType>(); };
                }
                
                return computeLongRunAverageValues(env, stateRewardsGetter, actionRewardsGetter);
            }
            
            template <typename ValueType, bool Nondeterministic>
            std::vector<ValueType> SparseNondeterministicInfiniteHorizonHelper<ValueType, Nondeterministic>::computeLongRunAverageValues(Environment const& env, std::vector<ValueType> const* stateValues, std::vector<ValueType> const* actionValues) {
                std::function<ValueType(uint64_t stateIndex)> stateValuesGetter;
                if (stateValues) {
                    stateValuesGetter = [&stateValues] (uint64_t stateIndex) { return (*stateValues)[stateIndex]; };
                } else {
                    stateValuesGetter = [] (uint64_t) { return storm::utility::zero<ValueType>(); };
                }
                std::function<ValueType(uint64_t actionIndex)> actionValuesGetter;
                if (actionValues) {
                    actionValuesGetter = [&actionValues] (uint64_t globalChoiceIndex) { return (*actionValues)[globalChoiceIndex]; };
                } else {
                    actionValuesGetter = [] (uint64_t) { return storm::utility::zero<ValueType>(); };
                }
                
                return computeLongRunAverageValues(env, stateValuesGetter, actionValuesGetter);

            }
            
            template <typename ValueType, bool Nondeterministic>
            std::vector<ValueType> SparseNondeterministicInfiniteHorizonHelper<ValueType, Nondeterministic>::computeLongRunAverageValues(Environment const& env, std::function<ValueType(uint64_t stateIndex)> const& stateRewardsGetter,  std::function<ValueType(uint64_t globalChoiceIndex)> const& actionRewardsGetter) {
                // We will compute the long run average value for each MEC individually and then set-up a MinMax Equation system to compute the value also at non-mec states.
                // For a description of this approach see, e.g., Guck et al.: Modelling and Analysis of Markov Reward Automata (ATVA'14), https://doi.org/10.1007/978-3-319-11936-6_13
                
                 // Prepare an environment for the underlying solvers
                auto underlyingSolverEnvironment = env;
                if (env.solver().isForceSoundness()) {
                    // For sound computations, the error in the MECS plus the error in the remaining system should not exceed the user defined precsion.
                    storm::RationalNumber newPrecision = env.solver().lra().getPrecision() / storm::utility::convertNumber<storm::RationalNumber>(2);
                    underlyingSolverEnvironment.solver().minMax().setPrecision(newPrecision);
                    underlyingSolverEnvironment.solver().minMax().setRelativeTerminationCriterion(env.solver().lra().getRelativeTerminationCriterion());
                    underlyingSolverEnvironment.solver().setLinearEquationSolverPrecision(newPrecision, env.solver().lra().getRelativeTerminationCriterion());
                    underlyingSolverEnvironment.solver().lra().setPrecision(newPrecision);
                }
                
                // If requested, allocate memory for the choices made
                if (Nondeterministic && this->isProduceSchedulerSet()) {
                    if (!_producedOptimalChoices.is_initialized()) {
                        _producedOptimalChoices.emplace();
                    }
                    _producedOptimalChoices->resize(_transitionMatrix.getRowGroupCount());
                }
                STORM_LOG_ASSERT(Nondeterministic || !this->isProduceSchedulerSet(), "Scheduler production enabled for deterministic model.");
                
                // Start by decomposing the Model into its MECs.
                if (_longRunComponentDecomposition == nullptr) {
                    // The decomposition has not been provided or computed, yet.
                    if (Nondeterministic) {
                        if (_backwardTransitions == nullptr) {
                            _computedBackwardTransitions = std::make_unique<storm::storage::SparseMatrix>(_transitionMatrix.transpose(true));
                            _backwardTransitions = _computedBackwardTransitions.get();
                        }
                        _computedLongRunComponentDecomposition = std::make_unique<storm::storage::MaximalEndComponentDecomposition<ValueType>(_transitionMatrix, *_backwardTransitions);
                    } else {
                        _computedLongRunComponentDecomposition = std::make_unique<storm::storage::StronglyConnectedComponentDecomposition<ValueType>(_transitionMatrix, storm::storage::StronglyConnectedComponentDecompositionOptions().onlyBottomSccs());
                    }
                    _longRunComponentDecomposition = _computedLongRunComponentDecomposition.get();
                }

                // Compute the long-run average for all components in isolation.
                std::vector<ValueType> componentLraValues;
                mecLraValues.reserve(_longRunComponentDecomposition->size());
                for (auto const& c : *_longRunComponentDecomposition) {
                    componentLraValues.push_back(computeLraForComponent(underlyingSolverEnvironment, stateRewardsGetter, actionRewardsGetter, c));
                }
                
                // Solve the resulting SSP where end components are collapsed into single auxiliary states
                return buildAndSolveSsp(underlyingSolverEnvironment, componentLraValues);
            }
            
            template <typename ValueType, bool Nondeterministic>
            std::vector<uint64_t> const& SparseNondeterministicInfiniteHorizonHelper<ValueType, Nondeterministic>::getProducedOptimalChoices() const {
                STORM_LOG_WARN_COND(Nondeterministic, "Getting optimal choices for deterministic model.");
                STORM_LOG_ASSERT(this->isProduceSchedulerSet(), "Trying to get the produced optimal choices although no scheduler was requested.");
                STORM_LOG_ASSERT(_producedOptimalChoices.is_initialized(), "Trying to get the produced optimal choices but none were available. Was there a computation call before?");
                return _producedOptimalChoices.get();
            }
            
            template <typename ValueType, bool Nondeterministic>
            std::vector<uint64_t>& SparseNondeterministicInfiniteHorizonHelper<ValueType, Nondeterministic>::getProducedOptimalChoices() {
                STORM_LOG_WARN_COND(Nondeterministic, "Getting optimal choices for deterministic model.");
                STORM_LOG_ASSERT(this->isProduceSchedulerSet(), "Trying to get the produced optimal choices although no scheduler was requested.");
                STORM_LOG_ASSERT(_producedOptimalChoices.is_initialized(), "Trying to get the produced optimal choices but none were available. Was there a computation call before?");
                return _producedOptimalChoices.get();
            }
            
            template <typename ValueType, bool Nondeterministic>
            storm::storage::Scheduler<ValueType> SparseNondeterministicInfiniteHorizonHelper<ValueType, Nondeterministic>::extractScheduler() const {
                auto const& optimalChoices = getProducedOptimalChoices();
                storm::storage::Scheduler<ValueType> scheduler(optimalChoices.size());
                for (uint64_t state = 0; state < optimalChoices.size(); ++state) {
                        scheduler.setChoice(optimalChoices[state], state);
                }
                return scheduler;
            }
            
            template <typename ValueType, bool Nondeterministic>
            bool SparseNondeterministicInfiniteHorizonHelper<ValueType, Nondeterministic>::isContinuousTime() const {
                STORM_LOG_ASSERT((_markovianStates == nullptr) || (_exitRates != nullptr), "Inconsistent information given: Have Markovian states but no exit rates." );
                return _exitRates != nullptr;
            }
    
            template <typename ValueType, bool Nondeterministic>
            template < typename = typename std::enable_if< !Nondeterministic >::type >
            ValueType SparseNondeterministicInfiniteHorizonHelper<ValueType, Nondeterministic>::computeLraForComponent(Environment const& env, std::function<ValueType(uint64_t stateIndex)> const& stateRewardsGetter,  std::function<ValueType(uint64_t globalChoiceIndex)> const& actionRewardsGetter, LongRunComponentType const& component) {
                // For deterministic models, we compute the LRA for a BSCC
                
                STORM_LOG_ASSERT(!this->isProduceSchedulerSet(), "Scheduler production enabled for deterministic model.");
                
                auto trivialResult = computeLraForTrivialComponent(env, stateReardsGetter, actionRewardsGetter, component);
                if (trivialResult.first) {
                    return trivialResult.second;
                }
                
                // Solve nontrivial BSCC with the method specified  in the settings
                
                // TODO
                
            }
            
            template <typename ValueType, bool Nondeterministic>
            template < typename = typename std::enable_if< Nondeterministic >::type >
            ValueType SparseNondeterministicInfiniteHorizonHelper<ValueType, Nondeterministic>::computeLraForComponent(Environment const& env, std::function<ValueType(uint64_t stateIndex)> const& stateRewardsGetter,  std::function<ValueType(uint64_t globalChoiceIndex)> const& actionRewardsGetter, LongRunComponentType const& component) {
                // For models with potential nondeterminisim, we compute the LRA for a maximal end component (MEC)
                
                // Allocate memory for the nondeterministic choices.
                if (this->isProduceSchedulerSet()) {
                    if (!_producedOptimalChoices.is_initialized()) {
                        _producedOptimalChoices.emplace();
                    }
                    _producedOptimalChoices->resize(_transitionMatrix.getRowGroupCount());
                }
                
                auto trivialResult = computeLraForTrivialComponent(env, stateReardsGetter, actionRewardsGetter, component);
                if (trivialResult.first) {
                    return trivialResult.second;
                }
                
                // Solve nontrivial MEC with the method specified in the settings
                storm::solver::LraMethod method = env.solver().lra().getNondetLraMethod();
                if ((storm::NumberTraits<ValueType>::IsExact || env.solver().isForceExact()) && env.solver().lra().isNondetLraMethodSetFromDefault() && method != storm::solver::LraMethod::LinearProgramming) {
                    STORM_LOG_INFO("Selecting 'LP' as the solution technique for long-run properties to guarantee exact results. If you want to override this, please explicitly specify a different LRA method.");
                    method = storm::solver::LraMethod::LinearProgramming;
                } else if (env.solver().isForceSoundness() && env.solver().lra().isNondetLraMethodSetFromDefault() && method != storm::solver::LraMethod::ValueIteration) {
                    STORM_LOG_INFO("Selecting 'VI' as the solution technique for long-run properties to guarantee sound results. If you want to override this, please explicitly specify a different LRA method.");
                    method = storm::solver::LraMethod::ValueIteration;
                }
                STORM_LOG_ERROR_COND(!this->isProduceSchedulerSet() || method == storm::solver::LraMethod::ValueIteration, "Scheduler generation not supported for the chosen LRA method. Try value-iteration.");
                if (method == storm::solver::LraMethod::LinearProgramming) {
                    return computeLraForMecLp(env, stateRewardsGetter, actionRewardsGetter, mec);
                } else if (method == storm::solver::LraMethod::ValueIteration) {
                    return computeLraForMecVi(env, stateRewardsGetter, actionRewardsGetter, mec);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Unsupported technique.");
                }
            }
    
            template <typename ValueType, bool Nondeterministic>
            std::pair<bool, ValueType> SparseNondeterministicInfiniteHorizonHelper<ValueType, Nondeterministic>::computeLraForTrivialComponent(Environment const& env, std::function<ValueType(uint64_t stateIndex)> const& stateRewardsGetter,  std::function<ValueType(uint64_t globalChoiceIndex)> const& actionRewardsGetter, LongRunComponentType const& component) {
                
                // If the component only consists of a single state, we compute the LRA value directly
                if (component.size() == 1) {
                    auto element const& = *component.begin();
                    uint64_t state = internal::getComponentElementState(element);
                    auto choiceIt = internal::getComponentChoicesBegin(element);
                    if (Nondeterministic && !isContinuousTime()) {
                        // This is an MDP.
                        // Find the choice with the highest/lowest reward
                        ValueType bestValue = actionRewardsGetter(*choiceIt);
                        uint64_t bestChoice = *choiceIt;
                        for (++choiceIt; choiceIt != getComponentChoicesEnd(element); ++choiceIt) {
                            ValueType currentValue = actionRewardsGetter(*choiceIt);
                            if ((this->minimize() &&  currentValue < bestValue) || (this->maximize() && currentValue > bestValue)) {
                                bestValue = std::move(currentValue);
                                bestChoice = *choiceIt;
                            }
                        }
                        if (this->isProduceSchedulerSet()) {
                            _producedOptimalChoices.get()[state] = bestChoice - _transitionMatrix.getRowGroupIndices()[state];
                        }
                        bestValue += stateRewardsGetter(state);
                        return {true, bestValue};
                    } else {
                        // In a Markov Automaton, singleton components have to consist of a Markovian state because of the non-Zenoness assumption. Then, there is just one possible choice.
                        STORM_LOG_THROW(!Nondeterministic || (_markovianStates != nullptr && _markovianStates->get(state)), storm::exceptions::InvalidOperationException, "Markov Automaton has Zeno behavior. Computation of Long Run Average values not supported.");
                        STORM_LOG_ASSERT(internal::getComponentElementChoiceCount(element) == 1, "Markovian state has Nondeterministic behavior.");
                        if (Nondeterministic && this->isProduceSchedulerSet()) {
                            _producedOptimalChoices.get()[state] = 0;
                        }
                        ValueType result = stateRewardsGetter(state) + (isContinuousTime() ? (*_exitRates)[state] * actionRewardsGetter(*choiceIt) : actionRewardsGetter(*choiceIt));
                        return {true, result};
                    }
                } else if (!Nondeterministic) {
                    // For deterministic models, we can also easily catch the case where all values are the same
                    bool first = true;
                    ValueType val = storm::utility::zero<ValueType>();
                    for (auto const& element : component) {
                        auto state = getComponentElementState(element);
                        STORM_LOG_ASSERT(state == *getComponentChoicesBegin(element), "Unexpected choice index at state " << state << " of deterministic model.");
                        ValueType curr = stateRewardsGetter(state) + (isContinuousTime() ? (*_exitRates)[state] * actionRewardsGetter(state) : actionRewardsGetter(state));
                        if (first) {
                            first = false;
                        } else if (val != curr) {
                            return {false, storm::utility::zero<ValueType>()};
                        }
                    }
                    // All values are the same
                    return {true, val};
                } else {
                    return {false, storm::utility::zero<ValueType>()};
                }
            }
            
            template <typename ValueType, bool Nondeterministic>
            ValueType SparseNondeterministicInfiniteHorizonHelper<ValueType, Nondeterministic>::computeLraForMecVi(Environment const& env, std::function<ValueType(uint64_t stateIndex)> const& stateRewardsGetter,  std::function<ValueType(uint64_t globalChoiceIndex)> const& actionRewardsGetter, storm::storage::MaximalEndComponent const& mec) {

                // Collect some parameters of the computation
                ValueType aperiodicFactor = storm::utility::convertNumber<ValueType>(env.solver().lra().getAperiodicFactor());
                std::vector<uint64_t>* optimalChoices = nullptr;
                if (this->isProduceSchedulerSet()) {
                    optimalChoices = &_producedOptimalChoices.get();
                }
                
                // Now create a helper and perform the algorithm
                if (isContinuousTime()) {
                    // We assume a Markov Automaton (with deterministic timed states and nondeterministic instant states)
                    storm::modelchecker::helper::internal::LraViHelper<ValueType, storm::storage::MaximalEndComponent, storm::modelchecker::helper::internal::LraViTransitionsType::DetTsNondetIs> viHelper(mec, _transitionMatrix, aperiodicFactor, _markovianStates, _exitRates);
                    return viHelper.performValueIteration(env, stateRewardsGetter, actionRewardsGetter, _exitRates, &this->getOptimizationDirection(), optimalChoices);
                } else {
                    // We assume an MDP (with nondeterministic timed states and no instant states)
                    storm::modelchecker::helper::internal::LraViHelper<ValueType, storm::storage::MaximalEndComponent, storm::modelchecker::helper::internal::LraViTransitionsType::NondetTsNoIs> viHelper(mec, _transitionMatrix, aperiodicFactor);
                    return viHelper.performValueIteration(env, stateRewardsGetter, actionRewardsGetter, nullptr, &this->getOptimizationDirection(), optimalChoices);
                }
            }
            
            template <typename ValueType, bool Nondeterministic>
            ValueType SparseNondeterministicInfiniteHorizonHelper<ValueType, Nondeterministic>::computeLraForMecLp(Environment const& env, std::function<ValueType(uint64_t stateIndex)> const& stateRewardsGetter,  std::function<ValueType(uint64_t globalChoiceIndex)> const& actionRewardsGetter, storm::storage::MaximalEndComponent const& mec) {
                // Create an LP solver
                auto solver = storm::utility::solver::LpSolverFactory<ValueType>().create("LRA for MEC");
                
                // Now build the LP formulation as described in:
                // Guck et al.: Modelling and Analysis of Markov Reward Automata (ATVA'14), https://doi.org/10.1007/978-3-319-11936-6_13
                solver->setOptimizationDirection(invert(this->getOptimizationDirection()));
                
                // Create variables
                // TODO: Investigate whether we can easily make the variables bounded
                std::map<uint_fast64_t, storm::expressions::Variable> stateToVariableMap;
                for (auto const& stateChoicesPair : mec) {
                    std::string variableName = "x" + std::to_string(stateChoicesPair.first);
                    stateToVariableMap[stateChoicesPair.first] = solver->addUnboundedContinuousVariable(variableName);
                }
                storm::expressions::Variable k = solver->addUnboundedContinuousVariable("k", storm::utility::one<ValueType>());
                solver->update();
                
                // Add constraints.
                for (auto const& stateChoicesPair : mec) {
                    uint_fast64_t state = stateChoicesPair.first;
                    bool stateIsMarkovian = _markovianStates && _markovianStates->get(state);
                    
                    // Now create a suitable constraint for each choice
                    // x_s  {≤, ≥}  -k/rate(s) + sum_s' P(s,act,s') * x_s' + (value(s)/rate(s) + value(s,act))
                    for (auto choice : stateChoicesPair.second) {
                        std::vector<storm::expressions::Expression> summands;
                        auto matrixRow = _transitionMatrix.getRow(choice);
                        summands.reserve(matrixRow.getNumberOfEntries() + 2);
                        // add -k/rate(s) (only if s is either a Markovian state or we have an MDP)
                        if (stateIsMarkovian) {
                            summands.push_back(-(k / solver->getManager().rational((*_exitRates)[state])));
                        } else if (!isContinuousTime()) {
                            summands.push_back(-k);
                        }
                        // add sum_s' P(s,act,s') * x_s'
                        for (auto const& element : matrixRow) {
                            summands.push_back(stateToVariableMap.at(element.getColumn()) * solver->getConstant(element.getValue()));
                        }
                        // add value for state and choice
                        ValueType value;
                        if (stateIsMarkovian) {
                            // divide state reward with exit rate
                            value = stateRewardsGetter(state) / (*_exitRates)[state] + actionRewardsGetter(choice);
                        } else if (!isContinuousTime()) {
                            // in discrete time models no scaling is needed
                            value = stateRewardsGetter(state) + actionRewardsGetter(choice);
                        } else {
                            // state is a probabilistic state of a Markov automaton. The state reward is not collected
                            value = actionRewardsGetter(choice);
                        }
                        summands.push_back(solver->getConstant(value));
                        storm::expressions::Expression constraint;
                        if (this->minimize()) {
                            constraint = stateToVariableMap.at(state) <= storm::expressions::sum(summands);
                        } else {
                            constraint = stateToVariableMap.at(state) >= storm::expressions::sum(summands);
                        }
                        solver->addConstraint("s" + std::to_string(state) + "," + std::to_string(choice), constraint);
                    }
                }
                
                solver->optimize();
                STORM_LOG_THROW(!this->isProduceSchedulerSet(), storm::exceptions::NotImplementedException, "Scheduler extraction is not yet implemented for LP based LRA method.");
                return solver->getContinuousValue(k);
            }
            
            /*!
             * Auxiliary function that adds the entries of the Ssp Matrix for a single choice (i.e., row)
             * Transitions that lead to a Component state will be redirected to a new auxiliary state (there is one aux. state for each component).
             * Transitions that don't lead to a Component state are copied (taking a state index mapping into account).
             */
            template <typename ValueType>
            void addSspMatrixChoice(uint64_t const& inputMatrixChoice, storm::storage::SparseMatrix<ValueType> const& inputTransitionMatrix, std::vector<uint64_t> const& inputToSspStateMap, uint64_t const& numberOfStatesNotInComponents, uint64_t const& currentSspChoice, storm::storage::SparseMatrixBuilder<ValueType>& sspMatrixBuilder) {
            
                // As there could be multiple transitions to the same MEC, we accumulate them in this map before adding them to the matrix builder.
                std::map<uint64_t, ValueType> auxiliaryStateToProbabilityMap;
                
                for (auto const& transition : inputTransitionMatrix.getRow(inputMatrixChoice)) {
                    if (!storm::utility::isZero(transition.getValue())) {
                        auto const& sspTransitionTarget = inputToSspStateMap[transition.getColumn()];
                        // Since the auxiliary Component states are appended at the end of the matrix, we can use this check to
                        // decide whether the transition leads to a component state or not
                        if (sspTransitionTarget < numberOfStatesNotInMecs) {
                            // If the target state is not contained in a component, we can copy over the entry.
                            sspMatrixBuilder.addNextValue(currentSspChoice, sspTransitionTarget, transition.getValue());
                        } else {
                            // If the target state is contained in component i, we need to add the probability to the corresponding field in the vector
                            // so that we are able to write the cumulative probability to the component into the matrix later.
                            auto insertionRes = auxiliaryStateToProbabilityMap.emplace(sspTransitionTarget, transition.getValue());
                            if (!insertionRes.second) {
                                // sspTransitionTarget already existed in the map, i.e., there already was a transition to that component.
                                // Hence, we add up the probabilities.
                                insertionRes.first->second += transition.getValue();
                            }
                        }
                    }
                }
                
                // Now insert all (cumulative) probability values that target a component.
                for (auto const& componentToProbEntry : auxiliaryStateToProbabilityMap) {
                    sspMatrixBuilder.addNextValue(currentSspChoice, componentToProbEntry.first, componentToProbEntry.second);
                }
            }
            
            template <typename ValueType, bool Nondeterministic>
            std::vector<ValueType> SparseNondeterministicInfiniteHorizonHelper<ValueType, Nondeterministic>::buildAndSolveSsp(Environment const& env, std::vector<ValueType> const& mecLraValues) {
                STORM_LOG_ASSERT(_longRunComponentDecomposition != nullptr, "Decomposition not computed, yet.");
                
                uint64_t numberOfStates = _transitionMatrix.getRowGroupCount();
                
                // For fast transition rewriting, we build a mapping from the input state indices to the state indices of a new transition matrix
                // which redirects all transitions leading to a former component state to a new auxiliary state.
                // There will be one auxiliary state for each component. These states will be appended to the end of the matrix.
                
                // First gather the states that are part of a component
                // and create a mapping from states that lie in a component to the corresponding component index.
                storm::storage::BitVector statesInMecs(numberOfStates);
                std::vector<uint64_t> inputToSspStateMap(numberOfStates, std::numeric_limits<uint64_t>::max());
                for (uint64_t currentMecIndex = 0; currentMecIndex < _longRunComponentDecomposition->size(); ++currentMecIndex) {
                    for (auto const& stateChoicesPair : (*_longRunComponentDecomposition)[currentMecIndex]) {
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
                uint64_t numberOfSspStates = numberOfStatesNotInMecs + _longRunComponentDecomposition->size();
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
                for (uint64_t mecIndex = 0; mecIndex < _longRunComponentDecomposition->size(); ++mecIndex) {
                    storm::storage::MaximalEndComponent const& mec = (*_longRunComponentDecomposition)[mecIndex];
                    sspMatrixBuilder.newRowGroup(currentSspChoice);
                    for (auto const& stateChoicesPair : mec) {
                        uint64_t const& mecState = stateChoicesPair.first;
                        auto const& choicesInMec = stateChoicesPair.second;
                        for (uint64_t choice = nondeterministicChoiceIndices[mecState]; choice < nondeterministicChoiceIndices[mecState + 1]; ++choice) {
                            // If the choice is not contained in the MEC itself, we have to add a similar distribution to the auxiliary state.
                            if (choicesInMec.find(choice) == choicesInMec.end()) {
                                rhs.push_back(storm::utility::zero<ValueType>());
                                addSspMatrixChoice(choice, _transitionMatrix, inputToSspStateMap, numberOfStatesNotInMecs, currentSspChoice, sspMatrixBuilder);
                                if (this->isProduceSchedulerSet()) {
                                    // Later we need to be able to map this choice back to the original input model
                                    sspMecExitChoicesToOriginalMap.emplace_back(mecState, choice - nondeterministicChoiceIndices[mecState]);
                                }
                                ++currentSspChoice;
                            }
                        }
                    }
                    // For each auxiliary state, there is the option to achieve the reward value of the LRA associated with the MEC.
                    rhs.push_back(mecLraValues[mecIndex]);
                    if (this->isProduceSchedulerSet()) {
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
                solver->setTrackScheduler(this->isProduceSchedulerSet());
                auto lowerUpperBounds = std::minmax_element(mecLraValues.begin(), mecLraValues.end());
                solver->setLowerBound(*lowerUpperBounds.first);
                solver->setUpperBound(*lowerUpperBounds.second);
                solver->setRequirementsChecked();
                
                // Solve the equation system
                std::vector<ValueType> x(numberOfSspStates);
                solver->solveEquations(env, this->getOptimizationDirection(), x, rhs);

                // Prepare scheduler (if requested)
                if (this->isProduceSchedulerSet() && solver->hasScheduler()) {
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
                    for (auto const& mec : *_longRunComponentDecomposition) {
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
                            // Ensure that backwards transitions are available
                            if (_backwardTransitions == nullptr) {
                                _computedBackwardTransitions = _transitionMatrix.transpose(true);
                                _backwardTransitions = &_computedBackwardTransitions;
                            }
                            // Now start a backwards DFS
                            std::vector<uint64_t> stack = {originalStateChoice.first};
                            while (!stack.empty()) {
                                uint64_t currentState = stack.back();
                                stack.pop_back();
                                for (auto const& backwardsTransition : _backwardTransitions->getRowGroup(currentState)) {
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
                    STORM_LOG_ERROR_COND(!this->isProduceSchedulerSet(), "Requested to produce a scheduler, but no scheduler was generated.");
                }
                
                // Prepare result vector.
                // For efficiency reasons, we re-use the memory of our rhs for this!
                std::vector<ValueType> result = std::move(rhs);
                result.resize(numberOfStates);
                result.shrink_to_fit();
                storm::utility::vector::selectVectorValues(result, inputToSspStateMap, x);
                return result;
            }
            
            template class SparseNondeterministicInfiniteHorizonHelper<double, false>;
            template class SparseNondeterministicInfiniteHorizonHelper<storm::RationalNumber, false>;
            
            //template class SparseNondeterministicInfiniteHorizonHelper<double, true>;
            //template class SparseNondeterministicInfiniteHorizonHelper<storm::RationalNumber, true>;
        }
    }
}