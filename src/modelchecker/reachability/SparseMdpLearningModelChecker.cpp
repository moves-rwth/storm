#include "src/modelchecker/reachability/SparseMdpLearningModelChecker.h"

#include "src/storage/SparseMatrix.h"
#include "src/storage/sparse/StateStorage.h"
#include "src/storage/MaximalEndComponentDecomposition.h"

#include "src/generator/PrismNextStateGenerator.h"

#include "src/logic/FragmentSpecification.h"

#include "src/utility/prism.h"

#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "src/models/sparse/StandardRewardModel.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/modules/GeneralSettings.h"

#include "src/utility/macros.h"
#include "src/exceptions/NotSupportedException.h"

namespace storm {
    namespace modelchecker {
        template<typename ValueType>
        SparseMdpLearningModelChecker<ValueType>::SparseMdpLearningModelChecker(storm::prism::Program const& program, boost::optional<std::map<storm::expressions::Variable, storm::expressions::Expression>> const& constantDefinitions) : program(storm::utility::prism::preprocessProgram<ValueType>(program, constantDefinitions)), variableInformation(this->program), //randomGenerator(std::chrono::system_clock::now().time_since_epoch().count()),
        comparator(1e-9) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        bool SparseMdpLearningModelChecker<ValueType>::canHandle(CheckTask<storm::logic::Formula> const& checkTask) const {
            storm::logic::Formula const& formula = checkTask.getFormula();
            storm::logic::FragmentSpecification fragment = storm::logic::propositional().setProbabilityOperatorsAllowed(true).setReachabilityProbabilityFormulasAllowed(true).setNestedOperatorsAllowed(false);
            return formula.isInFragment(fragment) && checkTask.isOnlyInitialStatesRelevantSet();
        }
        
        template<typename ValueType>
        void SparseMdpLearningModelChecker<ValueType>::updateProbabilities(StateType const& sourceStateId, uint32_t action, std::vector<std::vector<storm::storage::MatrixEntry<StateType, ValueType>>> const& transitionMatrix, std::vector<StateType> const& rowGroupIndices, std::vector<StateType> const& stateToRowGroupMapping, std::vector<ValueType>& lowerBoundsPerAction, std::vector<ValueType>& upperBoundsPerAction, std::vector<ValueType>& lowerBoundsPerState, std::vector<ValueType>& upperBoundsPerState, StateType const& unexploredMarker) const {
            // Find out which row of the matrix we have to consider for the given action.
            StateType sourceRow = action;
            
            // Compute the new lower/upper values of the action.
            ValueType newLowerValue = storm::utility::zero<ValueType>();
            ValueType newUpperValue = storm::utility::zero<ValueType>();
            
//            boost::optional<ValueType> loopProbability;
            for (auto const& element : transitionMatrix[sourceRow]) {
                // If the element is a self-loop, we treat the probability by a proper rescaling after the loop.
//                if (element.getColumn() == sourceStateId) {
//                    STORM_LOG_TRACE("Found self-loop with probability " << element.getValue() << ".");
//                    loopProbability = element.getValue();
//                    continue;
//                }
                
                std::cout << "lower += " << element.getValue() << " * state[" << element.getColumn() << "] = " << (stateToRowGroupMapping[element.getColumn()] == unexploredMarker ? storm::utility::zero<ValueType>() : lowerBoundsPerState[stateToRowGroupMapping[element.getColumn()]]) << std::endl;
                if (stateToRowGroupMapping[element.getColumn()] != unexploredMarker) {
                    std::cout << "upper bounds per state @ " << stateToRowGroupMapping[element.getColumn()] << " is " << upperBoundsPerState[stateToRowGroupMapping[element.getColumn()]] << std::endl;
                }
                std::cout << "upper += " << element.getValue() << " * state[" << element.getColumn() << "] = " << (stateToRowGroupMapping[element.getColumn()] == unexploredMarker ? storm::utility::one<ValueType>() : upperBoundsPerState[stateToRowGroupMapping[element.getColumn()]]) << std::endl;
                newLowerValue += element.getValue() * (stateToRowGroupMapping[element.getColumn()] == unexploredMarker ? storm::utility::zero<ValueType>() : lowerBoundsPerState[stateToRowGroupMapping[element.getColumn()]]);
                newUpperValue += element.getValue() * (stateToRowGroupMapping[element.getColumn()] == unexploredMarker ? storm::utility::one<ValueType>() : upperBoundsPerState[stateToRowGroupMapping[element.getColumn()]]);
                std::cout << "after iter " << newLowerValue << " and " << newUpperValue << std::endl;
            }
            
            // If there was a self-loop with probability p, we scale the probabilities by 1/(1-p).
//            if (loopProbability) {
//                STORM_LOG_TRACE("Scaling values " << newLowerValue << " and " << newUpperValue << " with " << (storm::utility::one<ValueType>()/(storm::utility::one<ValueType>() - loopProbability.get())) << ".");
//                newLowerValue = newLowerValue / (storm::utility::one<ValueType>() - loopProbability.get());
//                newUpperValue = newUpperValue / (storm::utility::one<ValueType>() - loopProbability.get());
//            }
            
            // And set them as the current value.
            std::cout << newLowerValue << " vs " << newUpperValue << std::endl;
            STORM_LOG_ASSERT(newLowerValue <= newUpperValue, "Lower bound must always be smaller or equal than upper bound.");
            STORM_LOG_TRACE("Updating lower value of action " << action << " of state " << sourceStateId << " to " << newLowerValue << " (was " << lowerBoundsPerAction[action] << ").");
            lowerBoundsPerAction[action] = newLowerValue;
            STORM_LOG_TRACE("Updating upper value of action " << action << " of state " << sourceStateId << " to " << newUpperValue << " (was " << upperBoundsPerAction[action] << ").");
            upperBoundsPerAction[action] = newUpperValue;
            
            // Check if we need to update the values for the states.
            if (lowerBoundsPerState[stateToRowGroupMapping[sourceStateId]] < newLowerValue) {
                STORM_LOG_TRACE("Got new lower bound for state " << sourceStateId << ": " << newLowerValue << " (was " << lowerBoundsPerState[stateToRowGroupMapping[sourceStateId]] << ").");
                lowerBoundsPerState[stateToRowGroupMapping[sourceStateId]] = newLowerValue;
            }
                
            uint32_t sourceRowGroup = stateToRowGroupMapping[sourceStateId];
            if (newUpperValue < upperBoundsPerState[sourceRowGroup]) {
                if (rowGroupIndices[sourceRowGroup + 1] - rowGroupIndices[sourceRowGroup] > 1) {
                    ValueType max = storm::utility::zero<ValueType>();

                    for (uint32_t currentAction = rowGroupIndices[sourceRowGroup]; currentAction < rowGroupIndices[sourceRowGroup + 1]; ++currentAction) {
                        std::cout << "cur: " << currentAction << std::endl;
                        if (currentAction == action) {
                            continue;
                        }
                        
                        ValueType currentValue = storm::utility::zero<ValueType>();
                        for (auto const& element : transitionMatrix[currentAction]) {
                            currentValue += element.getValue() * (stateToRowGroupMapping[element.getColumn()] == unexploredMarker ? storm::utility::one<ValueType>() : upperBoundsPerState[stateToRowGroupMapping[element.getColumn()]]);
                        }
                        max = std::max(max, currentValue);
                        std::cout << "max is " << max << std::endl;
                    }
                    
                    newUpperValue = std::max(newUpperValue, max);
                }
                
                if (newUpperValue < upperBoundsPerState[sourceRowGroup]) {
                    STORM_LOG_TRACE("Got new upper bound for state " << sourceStateId << ": " << newUpperValue << " (was " << upperBoundsPerState[sourceRowGroup] << ").");
                    std::cout << "writing at index " << sourceRowGroup << std::endl;
                    upperBoundsPerState[sourceRowGroup] = newUpperValue;
                }
            }
        }
        
        template<typename ValueType>
        void SparseMdpLearningModelChecker<ValueType>::updateProbabilitiesUsingStack(std::vector<std::pair<StateType, uint32_t>>& stateActionStack, std::vector<std::vector<storm::storage::MatrixEntry<StateType, ValueType>>> const& transitionMatrix, std::vector<StateType> const& rowGroupIndices, std::vector<StateType> const& stateToRowGroupMapping, std::vector<ValueType>& lowerBoundsPerAction, std::vector<ValueType>& upperBoundsPerAction, std::vector<ValueType>& lowerBoundsPerState, std::vector<ValueType>& upperBoundsPerState, StateType const& unexploredMarker) const {
            
            std::cout << "stack is:" << std::endl;
            for (auto const& el : stateActionStack) {
                std::cout << el.first << "-[" << el.second << "]-> ";
            }
            std::cout << std::endl;
            
            stateActionStack.pop_back();
            while (!stateActionStack.empty()) {
                updateProbabilities(stateActionStack.back().first, stateActionStack.back().second, transitionMatrix, rowGroupIndices, stateToRowGroupMapping, lowerBoundsPerAction, upperBoundsPerAction, lowerBoundsPerState, upperBoundsPerState, unexploredMarker);
                stateActionStack.pop_back();
            }
        }
        
        template<typename ValueType>
        std::pair<ValueType, ValueType> SparseMdpLearningModelChecker<ValueType>::computeValuesOfChoice(uint32_t action, std::vector<std::vector<storm::storage::MatrixEntry<StateType, ValueType>>> const& transitionMatrix, std::vector<StateType> const& stateToRowGroupMapping, std::vector<ValueType> const& lowerBoundsPerState, std::vector<ValueType> const& upperBoundsPerState, StateType const& unexploredMarker) {
            std::pair<ValueType, ValueType> result = std::make_pair(storm::utility::zero<ValueType>(), storm::utility::zero<ValueType>());
            for (auto const& element : transitionMatrix[action]) {
                result.first += element.getValue() * (stateToRowGroupMapping[element.getColumn()] == unexploredMarker ? storm::utility::zero<ValueType>() : lowerBoundsPerState[stateToRowGroupMapping[element.getColumn()]]);
                result.second += element.getValue() * (stateToRowGroupMapping[element.getColumn()] == unexploredMarker ? storm::utility::one<ValueType>() : upperBoundsPerState[stateToRowGroupMapping[element.getColumn()]]);
            }
            return result;
        }
        
        template<typename ValueType>
        std::pair<ValueType, ValueType> SparseMdpLearningModelChecker<ValueType>::computeValuesOfState(StateType currentStateId, std::vector<std::vector<storm::storage::MatrixEntry<StateType, ValueType>>> const& transitionMatrix, std::vector<StateType> const& rowGroupIndices, std::vector<StateType> const& stateToRowGroupMapping, std::vector<ValueType> const& lowerBounds, std::vector<ValueType> const& upperBounds, std::vector<ValueType> const& lowerBoundsPerState, std::vector<ValueType> const& upperBoundsPerState, StateType const& unexploredMarker) {
            StateType sourceRowGroup = stateToRowGroupMapping[currentStateId];
            std::pair<ValueType, ValueType> result = std::make_pair(storm::utility::zero<ValueType>(), storm::utility::zero<ValueType>());
            for (uint32_t choice = rowGroupIndices[sourceRowGroup]; choice < rowGroupIndices[sourceRowGroup + 1]; ++choice) {
                std::pair<ValueType, ValueType> choiceValues = computeValuesOfChoice(choice, transitionMatrix, stateToRowGroupMapping, lowerBoundsPerState, upperBoundsPerState, unexploredMarker);
                result.first = std::max(choiceValues.first, result.first);
                result.second = std::max(choiceValues.second, result.second);
            }
            return result;
        }
        
        template<typename ValueType>
        uint32_t SparseMdpLearningModelChecker<ValueType>::sampleFromMaxActions(StateType currentStateId, std::vector<std::vector<storm::storage::MatrixEntry<StateType, ValueType>>> const& transitionMatrix, std::vector<StateType> const& rowGroupIndices, std::vector<StateType> const& stateToRowGroupMapping, std::vector<ValueType> const& upperBoundsPerState, std::unordered_map<StateType, ChoiceSetPointer> const& stateToLeavingChoicesOfEndComponent, StateType const& unexploredMarker) {
            
            StateType rowGroup = stateToRowGroupMapping[currentStateId];
            
            // First, determine all maximizing actions.
            std::vector<uint32_t> allMaxActions;
            
            for (StateType state = 0; state < stateToRowGroupMapping.size(); ++state) {
                if (stateToRowGroupMapping[state] != unexploredMarker) {
                    std::cout << "state " << state << " (grp " << stateToRowGroupMapping[state] << ") has bounds [x, " << upperBoundsPerState[stateToRowGroupMapping[state]] << "]" << std::endl;
                } else {
                    std::cout << "state " << state << " is unexplored" << std::endl;
                }
            }
            
            // Determine the maximal value of any action.
            ValueType max = 0;
            auto choicesInEcIt = stateToLeavingChoicesOfEndComponent.find(currentStateId);
            if (choicesInEcIt != stateToLeavingChoicesOfEndComponent.end()) {
                STORM_LOG_TRACE("Sampling from actions leaving the previously detected EC.");
                for (auto const& row : *choicesInEcIt->second) {
                    ValueType current = 0;
                    for (auto const& element : transitionMatrix[row]) {
                        current += element.getValue() * (stateToRowGroupMapping[element.getColumn()] == unexploredMarker ? storm::utility::one<ValueType>() : upperBoundsPerState[stateToRowGroupMapping[element.getColumn()]]);
                    }
                    
                    max = std::max(max, current);
                }
            } else {
                STORM_LOG_TRACE("Sampling from actions leaving the state.");
                for (uint32_t row = rowGroupIndices[rowGroup]; row < rowGroupIndices[rowGroup + 1]; ++row) {
                    ValueType current = 0;
                    for (auto const& element : transitionMatrix[row]) {
                        current += element.getValue() * (stateToRowGroupMapping[element.getColumn()] == unexploredMarker ? storm::utility::one<ValueType>() : upperBoundsPerState[stateToRowGroupMapping[element.getColumn()]]);
                    }
                    max = std::max(max, current);
                }
            }

            STORM_LOG_TRACE("Looking for action with value " << max << ".");

            for (StateType state = 0; state < stateToRowGroupMapping.size(); ++state) {
                if (stateToRowGroupMapping[state] != unexploredMarker) {
                    std::cout << "state " << state << " (grp " << stateToRowGroupMapping[state] << ") has bounds [x, " << upperBoundsPerState[stateToRowGroupMapping[state]] << "]" << std::endl;
                } else {
                    std::cout << "state " << state << " is unexplored" << std::endl;
                }
            }
            
            if (choicesInEcIt != stateToLeavingChoicesOfEndComponent.end()) {
                for (auto const& row : *choicesInEcIt->second) {
                    ValueType current = 0;
                    for (auto const& element : transitionMatrix[row]) {
                        current += element.getValue() * (stateToRowGroupMapping[element.getColumn()] == unexploredMarker ? storm::utility::one<ValueType>() : upperBoundsPerState[stateToRowGroupMapping[element.getColumn()]]);
                    }
                    STORM_LOG_TRACE("Computed (upper) bound " << current << " for row " << row << ".");
                    
                    // If the action is one of the maximizing ones, insert it into our list.
                    if (comparator.isEqual(current, max)) {
                        allMaxActions.push_back(row);
                    }
                }
            } else {
                for (uint32_t row = rowGroupIndices[rowGroup]; row < rowGroupIndices[rowGroup + 1]; ++row) {
                    ValueType current = 0;
                    for (auto const& element : transitionMatrix[row]) {
                        if (stateToRowGroupMapping[element.getColumn()] != unexploredMarker) {
                            std::cout << "upper bounds per state @ " << stateToRowGroupMapping[element.getColumn()] << " is " << upperBoundsPerState[stateToRowGroupMapping[element.getColumn()]] << std::endl;
                        }
                        std::cout << "+= " << element.getValue() << " * " << "state[" << element.getColumn() << "] = " << (stateToRowGroupMapping[element.getColumn()] == unexploredMarker ? storm::utility::one<ValueType>() : upperBoundsPerState[stateToRowGroupMapping[element.getColumn()]]) << std::endl;
                        current += element.getValue() * (stateToRowGroupMapping[element.getColumn()] == unexploredMarker ? storm::utility::one<ValueType>() : upperBoundsPerState[stateToRowGroupMapping[element.getColumn()]]);
                    }
                    STORM_LOG_TRACE("Computed (upper) bound " << current << " for row " << row << ".");
                    
                    // If the action is one of the maximizing ones, insert it into our list.
                    if (comparator.isEqual(current, max)) {
                        allMaxActions.push_back(row);
                    }
                }
            }
            
            STORM_LOG_ASSERT(!allMaxActions.empty(), "Must have at least one maximizing action.");
            
            // Now sample from all maximizing actions.
            std::uniform_int_distribution<uint32_t> distribution(0, allMaxActions.size() - 1);
            return allMaxActions[distribution(randomGenerator)];
        }
        
        template<typename ValueType>
        typename SparseMdpLearningModelChecker<ValueType>::StateType SparseMdpLearningModelChecker<ValueType>::sampleSuccessorFromAction(StateType chosenAction, std::vector<std::vector<storm::storage::MatrixEntry<StateType, ValueType>>> const& transitionMatrix, std::vector<StateType> const& rowGroupIndices, std::vector<StateType> const& stateToRowGroupMapping) {
            uint32_t row = chosenAction;
            
            // TODO: precompute this?
            std::vector<ValueType> probabilities(transitionMatrix[row].size());
            std::transform(transitionMatrix[row].begin(), transitionMatrix[row].end(), probabilities.begin(), [] (storm::storage::MatrixEntry<StateType, ValueType> const& entry) { return entry.getValue(); } );
            
            // Now sample according to the probabilities.
            std::discrete_distribution<StateType> distribution(probabilities.begin(), probabilities.end());
            StateType offset = distribution(randomGenerator);
            return transitionMatrix[row][offset].getColumn();
        }
        
        template<typename ValueType>
        storm::expressions::Expression SparseMdpLearningModelChecker<ValueType>::getTargetStateExpression(storm::logic::Formula const& subformula) {
            storm::expressions::Expression result;
            if (subformula.isAtomicExpressionFormula()) {
                result = subformula.asAtomicExpressionFormula().getExpression();
            } else {
                result = program.getLabelExpression(subformula.asAtomicLabelFormula().getLabel());
            }
            return result;
        }
        
        template<typename ValueType>
        void SparseMdpLearningModelChecker<ValueType>::detectEndComponents(std::vector<std::pair<StateType, uint32_t>> const& stateActionStack, boost::container::flat_set<StateType>& terminalStates, std::vector<std::vector<storm::storage::MatrixEntry<StateType, ValueType>>>& transitionMatrix, std::vector<StateType> const& rowGroupIndices, std::vector<StateType> const& stateToRowGroupMapping, std::vector<ValueType>& lowerBoundsPerAction, std::vector<ValueType>& upperBoundsPerAction, std::vector<ValueType>& lowerBoundsPerState, std::vector<ValueType>& upperBoundsPerState, std::unordered_map<StateType, ChoiceSetPointer>& stateToLeavingChoicesOfEndComponent, StateType const& unexploredMarker) const {
            
            STORM_LOG_TRACE("Starting EC detection.");
            
            // Outline:
            // 1. construct a sparse transition matrix of the relevant part of the state space.
            // 2. use this matrix to compute an MEC decomposition.
            // 3. if non-empty analyze the decomposition for accepting/rejecting MECs.
            
            // Start with 1.
            storm::storage::SparseMatrixBuilder<ValueType> builder(0, 0, 0, false, true, 0);
            
            // Determine the set of states that was expanded.
            std::vector<StateType> relevantStates;
            for (StateType state = 0; state < stateToRowGroupMapping.size(); ++state) {
                if (stateToRowGroupMapping[state] != unexploredMarker) {
                    relevantStates.push_back(state);
                }
            }
            // Sort according to the actual row groups so we can insert the elements in order later.
            std::sort(relevantStates.begin(), relevantStates.end(), [&stateToRowGroupMapping] (StateType const& a, StateType const& b) { return stateToRowGroupMapping[a] < stateToRowGroupMapping[b]; });
            StateType unexploredState = relevantStates.size();
            
            // Create a mapping for faster look-up during the translation of flexible matrix to the real sparse matrix.
            std::unordered_map<StateType, StateType> relevantStateToNewRowGroupMapping;
            for (StateType index = 0; index < relevantStates.size(); ++index) {
                std::cout << "relevant: " << relevantStates[index] << std::endl;
                relevantStateToNewRowGroupMapping.emplace(relevantStates[index], index);
            }
            
            // Do the actual translation.
            StateType currentRow = 0;
            for (auto const& state : relevantStates) {
                builder.newRowGroup(currentRow);
                StateType rowGroup = stateToRowGroupMapping[state];
                for (auto row = rowGroupIndices[rowGroup]; row < rowGroupIndices[rowGroup + 1]; ++row) {
                    ValueType unexpandedProbability = storm::utility::zero<ValueType>();
                    for (auto const& entry : transitionMatrix[row]) {
                        auto it = relevantStateToNewRowGroupMapping.find(entry.getColumn());
                        if (it != relevantStateToNewRowGroupMapping.end()) {
                            // If the entry is a relevant state, we copy it over (and compensate for the offset change).
                            builder.addNextValue(currentRow, it->second, entry.getValue());
                        } else {
                            // If the entry is an unexpanded state, we gather the probability to later redirect it to an unexpanded sink.
                            unexpandedProbability += entry.getValue();
                        }
                    }
                    if (unexpandedProbability != storm::utility::zero<ValueType>()) {
                        builder.addNextValue(currentRow, unexploredState, unexpandedProbability);
                    }
                    ++currentRow;
                }
            }
            // Then, make the unexpanded state absorbing.
            builder.newRowGroup(currentRow);
            builder.addNextValue(currentRow, unexploredState, storm::utility::one<ValueType>());
            STORM_LOG_TRACE("Successfully built matrix for MEC decomposition.");
            
            // Go on to step 2.
            storm::storage::SparseMatrix<ValueType> relevantStatesMatrix = builder.build();
            storm::storage::MaximalEndComponentDecomposition<ValueType> mecDecomposition(relevantStatesMatrix, relevantStatesMatrix.transpose(true));
            STORM_LOG_TRACE("Successfully computed MEC decomposition. Found " << (mecDecomposition.size() > 1 ? (mecDecomposition.size() - 1) : 0) << " MEC(s).");
            
            // 3. Analyze the MEC decomposition.
            for (auto const& mec : mecDecomposition) {
                // Ignore the (expected) MEC of the unexplored state.
                if (mec.containsState(unexploredState)) {
                    continue;
                }
                
                bool containsTargetState = false;
                
                // Now we record all choices leaving the EC.
                ChoiceSetPointer leavingChoices = std::make_shared<ChoiceSet>();
                for (auto const& stateAndChoices : mec) {
                    // Compute the state of the original model that corresponds to the current state.
                    std::cout << "local state: " << stateAndChoices.first << std::endl;
                    StateType originalState = relevantStates[stateAndChoices.first];
                    std::cout << "original state: " << originalState << std::endl;
                    uint32_t originalRowGroup = stateToRowGroupMapping[originalState];
                    std::cout << "original row group: " << originalRowGroup << std::endl;
                    
                    // TODO: This check for a target state is a bit hackish and only works for max probabilities.
                    if (!containsTargetState && lowerBoundsPerState[originalRowGroup] == storm::utility::one<ValueType>()) {
                        containsTargetState = true;
                    }
                    
                    auto includedChoicesIt = stateAndChoices.second.begin();
                    auto includedChoicesIte = stateAndChoices.second.end();
                    
                    for (auto choice = rowGroupIndices[originalRowGroup]; choice < rowGroupIndices[originalRowGroup + 1]; ++choice) {
                        if (includedChoicesIt != includedChoicesIte) {
                            STORM_LOG_TRACE("Next (local) choice contained in MEC is " << (*includedChoicesIt - relevantStatesMatrix.getRowGroupIndices()[stateAndChoices.first]));
                            STORM_LOG_TRACE("Current (local) choice iterated is " << (choice - rowGroupIndices[originalRowGroup]));
                            if (choice - rowGroupIndices[originalRowGroup] != *includedChoicesIt - relevantStatesMatrix.getRowGroupIndices()[stateAndChoices.first]) {
                                STORM_LOG_TRACE("Choice leaves the EC.");
                                leavingChoices->insert(choice);
                            } else {
                                STORM_LOG_TRACE("Choice stays in the EC.");
                                ++includedChoicesIt;
                            }
                        } else {
                            STORM_LOG_TRACE("Choice leaves the EC, because there is no more choice staying in the EC.");
                            leavingChoices->insert(choice);
                        }
                    }
                    
                    stateToLeavingChoicesOfEndComponent[originalState] = leavingChoices;
                }
                
                // If one of the states of the EC is a target state, all states in the EC have probability 1.
                if (containsTargetState) {
                    STORM_LOG_TRACE("MEC contains a target state.");
                    for (auto const& stateAndChoices : mec) {
                        // Compute the state of the original model that corresponds to the current state.
                        StateType originalState = relevantStates[stateAndChoices.first];

                        STORM_LOG_TRACE("Setting lower bound of state in row group " << stateToRowGroupMapping[originalState] << " to 1.");
                        lowerBoundsPerState[stateToRowGroupMapping[originalState]] = storm::utility::one<ValueType>();
                        terminalStates.insert(originalState);
                    }
                } else if (leavingChoices->empty()) {
                    STORM_LOG_TRACE("MEC's leaving choices are empty.");
                    // If there is no choice leaving the EC, but it contains no target state, all states have probability 0.
                    for (auto const& stateAndChoices : mec) {
                        // Compute the state of the original model that corresponds to the current state.
                        StateType originalState = relevantStates[stateAndChoices.first];
                        
                        STORM_LOG_TRACE("Setting upper bound of state in row group " << stateToRowGroupMapping[originalState] << " to 0.");
                        upperBoundsPerState[stateToRowGroupMapping[originalState]] = storm::utility::zero<ValueType>();
                        terminalStates.insert(originalState);
                    }
                }
            }
        }
        
        template<typename ValueType>
        std::tuple<typename SparseMdpLearningModelChecker<ValueType>::StateType, ValueType, ValueType> SparseMdpLearningModelChecker<ValueType>::performLearningProcedure(storm::expressions::Expression const& targetStateExpression, storm::storage::sparse::StateStorage<StateType>& stateStorage, storm::generator::PrismNextStateGenerator<ValueType, StateType>& generator, std::function<StateType (storm::generator::CompressedState const&)> const& stateToIdCallback, std::vector<std::vector<storm::storage::MatrixEntry<StateType, ValueType>>>& matrix, std::vector<StateType>& rowGroupIndices, std::vector<StateType>& stateToRowGroupMapping, std::unordered_map<StateType, storm::generator::CompressedState>& unexploredStates, StateType const& unexploredMarker) {
            
            // Generate the initial state so we know where to start the simulation.
            stateStorage.initialStateIndices = generator.getInitialStates(stateToIdCallback);
            STORM_LOG_THROW(stateStorage.initialStateIndices.size() == 1, storm::exceptions::NotSupportedException, "Currently only models with one initial state are supported by the learning engine.");
            StateType initialStateIndex = stateStorage.initialStateIndices.front();
            
            // A set storing all states in which to terminate the search.
            boost::container::flat_set<StateType> terminalStates;

            // Vectors to store the lower/upper bounds for each action (in each state).
            std::vector<ValueType> lowerBoundsPerAction;
            std::vector<ValueType> upperBoundsPerAction;
            std::vector<ValueType> lowerBoundsPerState;
            std::vector<ValueType> upperBoundsPerState;
            
            // Since we might run into end-components, we track a mapping from states in ECs to all leaving choices of
            // that EC.
            std::unordered_map<StateType, ChoiceSetPointer> stateToLeavingChoicesOfEndComponent;
            
            // Now perform the actual sampling.
            std::vector<std::pair<StateType, uint32_t>> stateActionStack;
            
            std::size_t iterations = 0;
            std::size_t maxPathLength = 0;
            std::size_t numberOfTargetStates = 0;
            std::size_t pathLengthUntilEndComponentDetection = 27;
            bool convergenceCriterionMet = false;
            while (!convergenceCriterionMet) {
                // Start the search from the initial state.
                stateActionStack.push_back(std::make_pair(initialStateIndex, 0));
                
                bool foundTerminalState = false;
                bool foundTargetState = false;
                while (!foundTerminalState) {
                    StateType const& currentStateId = stateActionStack.back().first;
                    STORM_LOG_TRACE("State on top of stack is: " << currentStateId << ".");
                    
                    // If the state is not yet expanded, we need to retrieve its behaviors.
                    auto unexploredIt = unexploredStates.find(currentStateId);
                    if (unexploredIt != unexploredStates.end()) {
                        STORM_LOG_TRACE("State was not yet explored.");
                        
                        // Map the unexplored state to a row group.
                        stateToRowGroupMapping[currentStateId] = rowGroupIndices.size() - 1;
                        STORM_LOG_TRACE("Assigning row group " << stateToRowGroupMapping[currentStateId] << " to state " << currentStateId << ".");
                        lowerBoundsPerState.push_back(storm::utility::zero<ValueType>());
                        upperBoundsPerState.push_back(storm::utility::one<ValueType>());
                        
                        // We need to get the compressed state back from the id to explore it.
                        STORM_LOG_ASSERT(unexploredIt != unexploredStates.end(), "Unable to find unexplored state " << currentStateId << ".");
                        storm::storage::BitVector const& currentState = unexploredIt->second;
                        
                        // Before generating the behavior of the state, we need to determine whether it's a target state that
                        // does not need to be expanded.
                        generator.load(currentState);
                        if (generator.satisfies(targetStateExpression)) {
                            STORM_LOG_TRACE("State does not need to be expanded, because it is a target state. +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
                            ++numberOfTargetStates;
                            foundTargetState = true;
                            foundTerminalState = true;
                        } else {
                            STORM_LOG_TRACE("Exploring state.");
                            
                            // If it needs to be expanded, we use the generator to retrieve the behavior of the new state.
                            storm::generator::StateBehavior<ValueType, StateType> behavior = generator.expand(stateToIdCallback);
                            STORM_LOG_TRACE("State has " << behavior.getNumberOfChoices() << " choices.");
                            
                            // Clumsily check whether we have found a state that forms a trivial BMEC.
                            if (behavior.getNumberOfChoices() == 0) {
                                foundTerminalState = true;
                            } else if (behavior.getNumberOfChoices() == 1) {
                                auto const& onlyChoice = *behavior.begin();
                                if (onlyChoice.size() == 1) {
                                    auto const& onlyEntry = *onlyChoice.begin();
                                    if (onlyEntry.first == currentStateId) {
                                        foundTerminalState = true;
                                    }
                                }
                            }
                            
                            // If the state was neither a trivial (non-accepting) terminal state nor a target state, we
                            // need to store its behavior.
                            if (!foundTerminalState) {
                                // Next, we insert the behavior into our matrix structure.
                                StateType startRow = matrix.size();
                                matrix.resize(startRow + behavior.getNumberOfChoices());
                                
                                // Terminate the row group.
                                rowGroupIndices.push_back(matrix.size());
                                
                                uint32_t currentAction = 0;
                                for (auto const& choice : behavior) {
                                    for (auto const& entry : choice) {
                                        std::cout << "adding " << currentStateId << " -> " << entry.first << " with prob " << entry.second << std::endl;
                                        matrix[startRow + currentAction].emplace_back(entry.first, entry.second);
                                    }
                                    
                                    lowerBoundsPerAction.push_back(storm::utility::zero<ValueType>());
                                    upperBoundsPerAction.push_back(storm::utility::one<ValueType>());
                                    
                                    std::pair<ValueType, ValueType> values = computeValuesOfChoice(startRow + currentAction, matrix, stateToRowGroupMapping, lowerBoundsPerState, upperBoundsPerState, unexploredMarker);
                                    lowerBoundsPerAction.back() = values.first;
                                    upperBoundsPerAction.back() = values.second;
                                    
                                    STORM_LOG_TRACE("Initializing bounds of action " << (startRow + currentAction) << " to " << lowerBoundsPerAction.back() << " and " << upperBoundsPerAction.back() << ".");
                                    
                                    ++currentAction;
                                }
                                
                                std::pair<ValueType, ValueType> values = computeValuesOfState(currentStateId, matrix, rowGroupIndices, stateToRowGroupMapping, lowerBoundsPerAction, upperBoundsPerAction, lowerBoundsPerState, upperBoundsPerState, unexploredMarker);
                                lowerBoundsPerState.back() = values.first;
                                upperBoundsPerState.back() = values.second;
                                
                                STORM_LOG_TRACE("Initializing bounds of state " << currentStateId << " to " << lowerBoundsPerState.back() << " and " << upperBoundsPerState.back() << ".");
                            }
                        }
                        
                        if (foundTerminalState) {
                            STORM_LOG_TRACE("State does not need to be explored, because it is " << (foundTargetState ? "a target state" : "a rejecting terminal state") << ".");
                            terminalStates.insert(currentStateId);
                            
                            if (foundTargetState) {
                                lowerBoundsPerState.back() = storm::utility::one<ValueType>();
                                lowerBoundsPerAction.push_back(storm::utility::one<ValueType>());
                                upperBoundsPerAction.push_back(storm::utility::one<ValueType>());
                            } else {
                                upperBoundsPerState.back() = storm::utility::zero<ValueType>();
                                lowerBoundsPerAction.push_back(storm::utility::zero<ValueType>());
                                upperBoundsPerAction.push_back(storm::utility::zero<ValueType>());
                            }
                            
                            // Increase the size of the matrix, but leave the row empty.
                            matrix.resize(matrix.size() + 1);
                            
                            // Terminate the row group.
                            rowGroupIndices.push_back(matrix.size());
                        }
                        
                        // Now that we have explored the state, we can dispose of it.
                        unexploredStates.erase(unexploredIt);
                    } else {
                        if (terminalStates.find(currentStateId) != terminalStates.end()) {
                            STORM_LOG_TRACE("Found already explored terminal state: " << currentStateId << ".");
                            foundTerminalState = true;
                        }
                    }
                    
                    if (foundTerminalState) {
                        // Update the bounds along the path to the terminal state.
                        STORM_LOG_TRACE("Found terminal state, updating probabilities along path.");
                        updateProbabilitiesUsingStack(stateActionStack, matrix, rowGroupIndices, stateToRowGroupMapping, lowerBoundsPerAction, upperBoundsPerAction, lowerBoundsPerState, upperBoundsPerState, unexploredMarker);
                    } else {
                        std::cout << "(2) stack is:" << std::endl;
                        for (auto const& el : stateActionStack) {
                            std::cout << el.first << "-[" << el.second << "]-> ";
                        }
                        std::cout << std::endl;
                        
                        for (StateType state = 0; state < stateToRowGroupMapping.size(); ++state) {
                            if (stateToRowGroupMapping[state] != unexploredMarker) {
                                std::cout << "state " << state << " (grp " << stateToRowGroupMapping[state] << ") has bounds [" << lowerBoundsPerState[stateToRowGroupMapping[state]] << ", " << upperBoundsPerState[stateToRowGroupMapping[state]] << "], actions: ";
                                for (auto choice = rowGroupIndices[stateToRowGroupMapping[state]]; choice < rowGroupIndices[stateToRowGroupMapping[state] + 1]; ++choice) {
                                    std::cout << choice << " = [" << lowerBoundsPerAction[choice] << ", " << upperBoundsPerAction[choice] << "], ";
                                }
                                std::cout << std::endl;
                            } else {
                                std::cout << "state " << state << " is unexplored" << std::endl;
                            }
                        }
                        
                        // At this point, we can be sure that the state was expanded and that we can sample according to the
                        // probabilities in the matrix.
                        uint32_t chosenAction = sampleFromMaxActions(currentStateId, matrix, rowGroupIndices, stateToRowGroupMapping, upperBoundsPerState, stateToLeavingChoicesOfEndComponent, unexploredMarker);
                        stateActionStack.back().second = chosenAction;
                        STORM_LOG_TRACE("Sampled action " << chosenAction << " in state " << currentStateId << ".");
                        
                        StateType successor = sampleSuccessorFromAction(chosenAction, matrix, rowGroupIndices, stateToRowGroupMapping);
                        STORM_LOG_TRACE("Sampled successor " << successor << " according to action " << chosenAction << " of state " << currentStateId << ".");
                        
                        // Put the successor state and a dummy action on top of the stack.
                        stateActionStack.emplace_back(successor, 0);
                        maxPathLength = std::max(maxPathLength, stateActionStack.size());
                        
                        // If the current path length exceeds the threshold and the model is a nondeterministic one, we
                        // perform an EC detection.
                        if (stateActionStack.size() > pathLengthUntilEndComponentDetection && !program.isDeterministicModel()) {
                            detectEndComponents(stateActionStack, terminalStates, matrix, rowGroupIndices, stateToRowGroupMapping, lowerBoundsPerAction, upperBoundsPerAction, lowerBoundsPerState, upperBoundsPerState, stateToLeavingChoicesOfEndComponent, unexploredMarker);
                            
                            // Abort the current search.
                            STORM_LOG_TRACE("Aborting the search after EC detection.");
                            stateActionStack.clear();
                            break;
                        }
                    }
                }
                
                // Sanity check of results.
                for (StateType state = 0; state < stateToRowGroupMapping.size(); ++state) {
                    if (stateToRowGroupMapping[state] != unexploredMarker) {
                        STORM_LOG_ASSERT(lowerBoundsPerState[stateToRowGroupMapping[state]] <= upperBoundsPerState[stateToRowGroupMapping[state]], "The bounds for state " << state << " are not in a sane relation: " << lowerBoundsPerState[stateToRowGroupMapping[state]] << " > " << upperBoundsPerState[stateToRowGroupMapping[state]] << ".");
                    }
                }
                
                for (StateType state = 0; state < stateToRowGroupMapping.size(); ++state) {
                    if (stateToRowGroupMapping[state] != unexploredMarker) {
                        std::cout << "state " << state << " (grp " << stateToRowGroupMapping[state] << ") has bounds [" << lowerBoundsPerState[stateToRowGroupMapping[state]] << ", " << upperBoundsPerState[stateToRowGroupMapping[state]] << "], actions: ";
                        for (auto choice = rowGroupIndices[stateToRowGroupMapping[state]]; choice < rowGroupIndices[stateToRowGroupMapping[state] + 1]; ++choice) {
                            std::cout << choice << " = [" << lowerBoundsPerAction[choice] << ", " << upperBoundsPerAction[choice] << "], ";
                        }
                        std::cout << std::endl;
                    } else {
                        std::cout << "state " << state << " is unexplored" << std::endl;
                    }
                }
                
                
                STORM_LOG_DEBUG("Discovered states: " << stateStorage.numberOfStates << " (" << unexploredStates.size() << " unexplored).");
                STORM_LOG_DEBUG("Value of initial state is in [" << lowerBoundsPerState[initialStateIndex] << ", " << upperBoundsPerState[initialStateIndex] << "].");
                ValueType difference = upperBoundsPerState[initialStateIndex] - lowerBoundsPerState[initialStateIndex];
                STORM_LOG_DEBUG("Difference after iteration " << iterations << " is " << difference << ".");
                convergenceCriterionMet = difference < 1e-6;
                
                ++iterations;
            }
            
            if (storm::settings::generalSettings().isShowStatisticsSet()) {
                std::cout << std::endl << "Learning summary -------------------------" << std::endl;
                std::cout << "Discovered states: " << stateStorage.numberOfStates << " (" << unexploredStates.size() << " unexplored, " << numberOfTargetStates << " target states)" << std::endl;
                std::cout << "Sampling iterations: " << iterations << std::endl;
                std::cout << "Maximal path length: " << maxPathLength << std::endl;
            }
            
            return std::make_tuple(initialStateIndex, lowerBoundsPerState[initialStateIndex], upperBoundsPerState[initialStateIndex]);
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> SparseMdpLearningModelChecker<ValueType>::computeReachabilityProbabilities(CheckTask<storm::logic::EventuallyFormula> const& checkTask) {
            storm::logic::EventuallyFormula const& eventuallyFormula = checkTask.getFormula();
            storm::logic::Formula const& subformula = eventuallyFormula.getSubformula();
            STORM_LOG_THROW(subformula.isAtomicExpressionFormula() || subformula.isAtomicLabelFormula(), storm::exceptions::NotSupportedException, "Learning engine can only deal with formulas of the form 'F \"label\"' or 'F expression'.");
            
            // Derive the expression for the target states, so we know when to abort the learning run.
            storm::expressions::Expression targetStateExpression = getTargetStateExpression(subformula);
            
            // A container for the encountered states.
            storm::storage::sparse::StateStorage<StateType> stateStorage(variableInformation.getTotalBitOffset(true));
            
            // A generator used to explore the model.
            storm::generator::PrismNextStateGenerator<ValueType, StateType> generator(program, variableInformation, false);
            
            // A container that stores the transitions found so far.
            std::vector<std::vector<storm::storage::MatrixEntry<StateType, ValueType>>> matrix;
            
            // A vector storing where the row group of each state starts.
            std::vector<StateType> rowGroupIndices;
            rowGroupIndices.push_back(0);
            
            // A vector storing the mapping from state ids to row groups.
            std::vector<StateType> stateToRowGroupMapping;
            StateType unexploredMarker = std::numeric_limits<StateType>::max();
            
            // A mapping of unexplored IDs to their actual compressed states.
            std::unordered_map<StateType, storm::generator::CompressedState> unexploredStates;
            
            // Create a callback for the next-state generator to enable it to request the index of states.
            std::function<StateType (storm::generator::CompressedState const&)> stateToIdCallback = [&stateStorage, &stateToRowGroupMapping, &unexploredStates, &unexploredMarker] (storm::generator::CompressedState const& state) -> StateType {
                StateType newIndex = stateStorage.numberOfStates;
                
                // Check, if the state was already registered.
                std::pair<uint32_t, std::size_t> actualIndexBucketPair = stateStorage.stateToId.findOrAddAndGetBucket(state, newIndex);
                
                if (actualIndexBucketPair.first == newIndex) {
                    ++stateStorage.numberOfStates;
                    stateToRowGroupMapping.push_back(unexploredMarker);
                    unexploredStates[newIndex] = state;
                }
                
                return actualIndexBucketPair.first;
            };
            
            // Compute and return result.
            std::tuple<StateType, ValueType, ValueType> boundsForInitialState = performLearningProcedure(targetStateExpression, stateStorage, generator, stateToIdCallback, matrix, rowGroupIndices, stateToRowGroupMapping, unexploredStates, unexploredMarker);
            return std::make_unique<ExplicitQuantitativeCheckResult<ValueType>>(std::get<0>(boundsForInitialState), std::get<1>(boundsForInitialState));
        }
        
        template class SparseMdpLearningModelChecker<double>;
    }
}