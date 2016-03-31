#include "src/modelchecker/reachability/SparseMdpLearningModelChecker.h"

#include "src/storage/SparseMatrix.h"
#include "src/storage/sparse/StateStorage.h"
#include "src/storage/MaximalEndComponentDecomposition.h"

#include "src/generator/PrismNextStateGenerator.h"

#include "src/logic/FragmentSpecification.h"

#include "src/utility/prism.h"

#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/modules/GeneralSettings.h"

#include "src/utility/macros.h"
#include "src/exceptions/NotSupportedException.h"

namespace storm {
    namespace modelchecker {
        template<typename ValueType>
        SparseMdpLearningModelChecker<ValueType>::SparseMdpLearningModelChecker(storm::prism::Program const& program) : program(storm::utility::prism::preprocessProgram<ValueType>(program)), variableInformation(this->program), randomGenerator(std::chrono::system_clock::now().time_since_epoch().count()), comparator(1e-9) {
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
            StateType sourceRowGroup = stateToRowGroupMapping[sourceStateId];
            StateType sourceRow = rowGroupIndices[sourceRowGroup] + action;
            
            // Compute the new lower/upper values of the action.
            ValueType newLowerValue = storm::utility::zero<ValueType>();
            ValueType newUpperValue = storm::utility::zero<ValueType>();
            boost::optional<ValueType> loopProbability;
            for (auto const& element : transitionMatrix[sourceRow]) {
                // If the element is a self-loop, we treat the probability by a proper rescaling after the loop.
                if (element.getColumn() == sourceStateId) {
                    STORM_LOG_TRACE("Found self-loop with probability " << element.getValue() << ".");
                    loopProbability = element.getValue();
                    continue;
                }
                
                newLowerValue += element.getValue() * (stateToRowGroupMapping[element.getColumn()] == unexploredMarker ? storm::utility::zero<ValueType>() : lowerBoundsPerState[stateToRowGroupMapping[element.getColumn()]]);
                newUpperValue += element.getValue() * (stateToRowGroupMapping[element.getColumn()] == unexploredMarker ? storm::utility::one<ValueType>() : upperBoundsPerState[stateToRowGroupMapping[element.getColumn()]]);
            }
            
            // If there was a self-loop with probability p, we scale the probabilities by 1/(1-p).
            if (loopProbability) {
                STORM_LOG_TRACE("Scaling values " << newLowerValue << " and " << newUpperValue << " with " << (storm::utility::one<ValueType>()/(storm::utility::one<ValueType>() - loopProbability.get())) << ".");
                newLowerValue = newLowerValue / (storm::utility::one<ValueType>() - loopProbability.get());
                newUpperValue = newUpperValue / (storm::utility::one<ValueType>() - loopProbability.get());
            }
            
            // And set them as the current value.
            lowerBoundsPerAction[rowGroupIndices[stateToRowGroupMapping[sourceStateId]] + action] = newLowerValue;
            STORM_LOG_TRACE("Updating lower value of action " << action << " of state " << sourceStateId << " to " << newLowerValue << ".");
            upperBoundsPerAction[rowGroupIndices[stateToRowGroupMapping[sourceStateId]] + action] = newUpperValue;
            STORM_LOG_TRACE("Updating upper value of action " << action << " of state " << sourceStateId << " to " << newUpperValue << ".");
            
            // Check if we need to update the values for the states.
            if (lowerBoundsPerState[stateToRowGroupMapping[sourceStateId]] < newLowerValue) {
                lowerBoundsPerState[stateToRowGroupMapping[sourceStateId]] = newLowerValue;
                STORM_LOG_TRACE("Got new lower bound for state " << sourceStateId << ": " << newLowerValue << ".");
            }
            if (upperBoundsPerState[stateToRowGroupMapping[sourceStateId]] > newUpperValue) {
                upperBoundsPerState[stateToRowGroupMapping[sourceStateId]] = newUpperValue;
                STORM_LOG_TRACE("Got new upper bound for state " << sourceStateId << ": " << newUpperValue << ".");
            }
        }
        
        template<typename ValueType>
        void SparseMdpLearningModelChecker<ValueType>::updateProbabilitiesUsingStack(std::vector<std::pair<StateType, uint32_t>>& stateActionStack, std::vector<std::vector<storm::storage::MatrixEntry<StateType, ValueType>>> const& transitionMatrix, std::vector<StateType> const& rowGroupIndices, std::vector<StateType> const& stateToRowGroupMapping, std::vector<ValueType>& lowerBoundsPerAction, std::vector<ValueType>& upperBoundsPerAction, std::vector<ValueType>& lowerBoundsPerState, std::vector<ValueType>& upperBoundsPerState, StateType const& unexploredMarker) const {
            
            while (stateActionStack.size() > 1) {
                stateActionStack.pop_back();

                updateProbabilities(stateActionStack.back().first, stateActionStack.back().second, transitionMatrix, rowGroupIndices, stateToRowGroupMapping, lowerBoundsPerAction, upperBoundsPerAction, lowerBoundsPerState, upperBoundsPerState, unexploredMarker);
            }
        }
        
        template<typename ValueType>
        uint32_t SparseMdpLearningModelChecker<ValueType>::sampleFromMaxActions(StateType currentStateId, std::vector<std::vector<storm::storage::MatrixEntry<StateType, ValueType>>> const& transitionMatrix, std::vector<StateType> const& rowGroupIndices, std::vector<StateType> const& stateToRowGroupMapping, std::vector<ValueType>& upperBoundsPerState, StateType const& unexploredMarker) {
            
            StateType rowGroup = stateToRowGroupMapping[currentStateId];
            STORM_LOG_TRACE("Row group for action sampling is " << rowGroup << ".");
            
            // First, determine all maximizing actions.
            std::vector<uint32_t> allMaxActions;
            
            // Determine the maximal value of any action.
            ValueType max = 0;
            for (uint32_t row = rowGroupIndices[rowGroup]; row < rowGroupIndices[rowGroup + 1]; ++row) {
                ValueType current = 0;
                for (auto const& element : transitionMatrix[row]) {
                    current += element.getValue() * (stateToRowGroupMapping[element.getColumn()] == unexploredMarker ? storm::utility::one<ValueType>() : upperBoundsPerState[stateToRowGroupMapping[element.getColumn()]]);
                }

                max = std::max(max, current);
            }

            STORM_LOG_TRACE("Looking for action with value " << max << ".");

            for (uint32_t row = rowGroupIndices[rowGroup]; row < rowGroupIndices[rowGroup + 1]; ++row) {
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
            
            STORM_LOG_ASSERT(!allMaxActions.empty(), "Must have at least one maximizing action.");
            
            // Now sample from all maximizing actions.
            std::uniform_int_distribution<uint32_t> distribution(0, allMaxActions.size() - 1);
            return allMaxActions[distribution(randomGenerator)] - rowGroupIndices[rowGroup];
        }
        
        template<typename ValueType>
        typename SparseMdpLearningModelChecker<ValueType>::StateType SparseMdpLearningModelChecker<ValueType>::sampleSuccessorFromAction(StateType currentStateId, std::vector<std::vector<storm::storage::MatrixEntry<StateType, ValueType>>> const& transitionMatrix, std::vector<StateType> const& rowGroupIndices, std::vector<StateType> const& stateToRowGroupMapping) {
            uint32_t row = rowGroupIndices[stateToRowGroupMapping[currentStateId]];
            
            // TODO: precompute this?
            std::vector<ValueType> probabilities(transitionMatrix[row].size());
            std::transform(transitionMatrix[row].begin(), transitionMatrix[row].end(), probabilities.begin(), [] (storm::storage::MatrixEntry<StateType, ValueType> const& entry) { return entry.getValue(); } );
            
            // Now sample according to the probabilities.
            std::discrete_distribution<StateType> distribution(probabilities.begin(), probabilities.end());
            StateType offset = distribution(randomGenerator);
            STORM_LOG_TRACE("Sampled " << offset << " from " << probabilities.size() << " elements.");
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
        void SparseMdpLearningModelChecker<ValueType>::detectEndComponents(std::vector<std::pair<StateType, uint32_t>> const& stateActionStack, boost::container::flat_set<StateType> const& targetStates, std::vector<std::vector<storm::storage::MatrixEntry<StateType, ValueType>>> const& transitionMatrix, std::vector<StateType> const& rowGroupIndices, std::vector<StateType> const& stateToRowGroupMapping, std::vector<ValueType>& lowerBoundsPerAction, std::vector<ValueType>& upperBoundsPerAction, std::vector<ValueType>& lowerBoundsPerState, std::vector<ValueType>& upperBoundsPerState, std::unordered_map<StateType, storm::generator::CompressedState>& unexploredStates, StateType const& unexploredMarker) const {
            
            // Outline:
            // 1. construct a sparse transition matrix of the relevant part of the state space.
            // 2. use this matrix to compute an MEC decomposition.
            // 3. if non-empty analyze the decomposition for accepting/rejecting MECs.
            // 4. modify matrix to account for the findings of 3.
            
            // Start with 1.
            storm::storage::SparseMatrixBuilder<ValueType> builder(0, 0, 0, false, true, 0);
            std::vector<StateType> relevantStates;
            for (StateType state = 0; state < stateToRowGroupMapping.size(); ++state) {
                if (stateToRowGroupMapping[state] != unexploredMarker) {
                    relevantStates.push_back(state);
                }
            }
            StateType unexploredState = relevantStates.size();
            
            std::unordered_map<StateType, StateType> relevantStateToNewRowGroupMapping;
            for (StateType index = 0; index < relevantStates.size(); ++index) {
                relevantStateToNewRowGroupMapping.emplace(relevantStates[index], index);
            }
            
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
                    builder.addNextValue(currentRow, unexploredState, unexpandedProbability);
                    ++currentRow;
                }
            }
            builder.newRowGroup(currentRow);
            
            
            // Go on to step 2.
            storm::storage::MaximalEndComponentDecomposition<ValueType> mecDecomposition;
            
            // 3. Analyze the MEC decomposition.
            
            // 4. Finally modify the system
        }
        
        template<typename ValueType>
        std::tuple<typename SparseMdpLearningModelChecker<ValueType>::StateType, ValueType, ValueType> SparseMdpLearningModelChecker<ValueType>::performLearningProcedure(storm::expressions::Expression const& targetStateExpression, storm::storage::sparse::StateStorage<StateType>& stateStorage, storm::generator::PrismNextStateGenerator<ValueType, StateType>& generator, std::function<StateType (storm::generator::CompressedState const&)> const& stateToIdCallback, std::vector<std::vector<storm::storage::MatrixEntry<StateType, ValueType>>>& matrix, std::vector<StateType>& rowGroupIndices, std::vector<StateType>& stateToRowGroupMapping, std::unordered_map<StateType, storm::generator::CompressedState>& unexploredStates, StateType const& unexploredMarker) {
            
            // Generate the initial state so we know where to start the simulation.
            stateStorage.initialStateIndices = generator.getInitialStates(stateToIdCallback);
            STORM_LOG_THROW(stateStorage.initialStateIndices.size() == 1, storm::exceptions::NotSupportedException, "Currently only models with one initial state are supported by the learning engine.");
            StateType initialStateIndex = stateStorage.initialStateIndices.front();
            
            // A set storing all target states.
            boost::container::flat_set<StateType> targetStates;

            // Vectors to store the lower/upper bounds for each action (in each state).
            std::vector<ValueType> lowerBoundsPerAction;
            std::vector<ValueType> upperBoundsPerAction;
            std::vector<ValueType> lowerBoundsPerState;
            std::vector<ValueType> upperBoundsPerState;
            
            // Now perform the actual sampling.
            std::vector<std::pair<StateType, uint32_t>> stateActionStack;
            
            std::size_t iterations = 0;
            std::size_t maxPathLength = 0;
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
                            STORM_LOG_TRACE("State does not need to be expanded, because it is a target state.");
                            targetStates.insert(currentStateId);
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
                                uint32_t currentAction = 0;
                                for (auto const& choice : behavior) {
                                    for (auto const& entry : choice) {
                                        matrix[startRow + currentAction].emplace_back(entry.first, entry.second);
                                    }
                                    
                                    lowerBoundsPerAction.push_back(storm::utility::zero<ValueType>());
                                    upperBoundsPerAction.push_back(storm::utility::one<ValueType>());
                                    
                                    // Update the bounds for the explored states to initially let them have the correct value.
                                    updateProbabilities(currentStateId, currentAction, matrix, rowGroupIndices, stateToRowGroupMapping, lowerBoundsPerAction, upperBoundsPerAction, lowerBoundsPerState, upperBoundsPerState, unexploredMarker);
                                    
                                    ++currentAction;
                                }
                            }
                        }
                        
                        if (foundTerminalState) {
                            STORM_LOG_TRACE("State does not need to be explored, because it is " << (foundTargetState ? "a target state" : "a rejecting terminal state") << ".");
                            
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
                            
                            STORM_LOG_TRACE("Updating probabilities along states in stack.");
                            updateProbabilitiesUsingStack(stateActionStack, matrix, rowGroupIndices, stateToRowGroupMapping, lowerBoundsPerAction, upperBoundsPerAction, lowerBoundsPerState, upperBoundsPerState, unexploredMarker);
                        }
                        
                        // Now that we have explored the state, we can dispose of it.
                        unexploredStates.erase(unexploredIt);
                        
                        // Terminate the row group.
                        rowGroupIndices.push_back(matrix.size());
                    } else {
                        // If the state was explored before, but determined to be a terminal state of the exploration,
                        // we need to determine this now.
                        if (matrix[rowGroupIndices[stateToRowGroupMapping[currentStateId]]].empty()) {
                            foundTerminalState = true;
                            
                            // Update the bounds along the path to the terminal state.
                            updateProbabilitiesUsingStack(stateActionStack, matrix, rowGroupIndices, stateToRowGroupMapping, lowerBoundsPerAction, upperBoundsPerAction, lowerBoundsPerState, upperBoundsPerState, unexploredMarker);
                        }
                    }
                    
                    if (!foundTerminalState) {
                        // At this point, we can be sure that the state was expanded and that we can sample according to the
                        // probabilities in the matrix.
                        uint32_t chosenAction = sampleFromMaxActions(currentStateId, matrix, rowGroupIndices, stateToRowGroupMapping, upperBoundsPerAction, unexploredMarker);
                        stateActionStack.back().second = chosenAction;
                        STORM_LOG_TRACE("Sampled action " << chosenAction << " in state " << currentStateId << ".");
                        
                        StateType successor = sampleSuccessorFromAction(currentStateId, matrix, rowGroupIndices, stateToRowGroupMapping);
                        STORM_LOG_TRACE("Sampled successor " << successor << " according to action " << chosenAction << " of state " << currentStateId << ".");
                        
                        // Put the successor state and a dummy action on top of the stack.
                        stateActionStack.emplace_back(successor, 0);
                        maxPathLength = std::max(maxPathLength, stateActionStack.size());
                        
                        // If the current path length exceeds the threshold, we perform an EC detection.
                        if (stateActionStack.size() > pathLengthUntilEndComponentDetection) {
                            detectEndComponents(stateActionStack, targetStates, matrix, rowGroupIndices, stateToRowGroupMapping, lowerBoundsPerAction, upperBoundsPerAction, lowerBoundsPerState, upperBoundsPerState, unexploredStates, unexploredMarker);
                        }
                    }
                }
                
                STORM_LOG_DEBUG("Discovered states: " << stateStorage.numberOfStates << " (" << unexploredStates.size() << " unexplored).");
                STORM_LOG_DEBUG("Lower bound is " << lowerBoundsPerState[initialStateIndex] << ".");
                STORM_LOG_DEBUG("Upper bound is " << upperBoundsPerState[initialStateIndex] << ".");
                ValueType difference = upperBoundsPerState[initialStateIndex] - lowerBoundsPerState[initialStateIndex];
                STORM_LOG_DEBUG("Difference after iteration " << iterations << " is " << difference << ".");
                convergenceCriterionMet = difference < 1e-6;
                
                ++iterations;
            }
            
            if (storm::settings::generalSettings().isShowStatisticsSet()) {
                std::cout << std::endl << "Learning summary -------------------------" << std::endl;
                std::cout << "Discovered states: " << stateStorage.numberOfStates << " (" << unexploredStates.size() << " unexplored)" << std::endl;
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