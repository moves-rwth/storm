#include "src/generator/PrismNextStateGenerator.h"

#include "src/utility/constants.h"
#include "src/utility/macros.h"
#include "src/exceptions/WrongFormatException.h"

namespace storm {
    namespace generator {
        
        template<typename ValueType, typename StateType>
        PrismNextStateGenerator<ValueType, StateType>::PrismNextStateGenerator(storm::prism::Program const& program, VariableInformation const& variableInformation, bool buildChoiceLabeling) : program(program), selectedRewardModels(), buildChoiceLabeling(buildChoiceLabeling), variableInformation(variableInformation), evaluator(), comparator() {
            // Intentionally left empty.
        }
        
        template<typename ValueType, typename StateType>
        void PrismNextStateGenerator<ValueType, StateType>::addRewardModel(storm::prism::RewardModel const& rewardModel) {
            selectedRewardModels.push_back(rewardModel);
        }
        
        template<typename ValueType, typename StateType>
        StateBehavior<ValueType, StateType> PrismNextStateGenerator<ValueType, StateType>::expand(CompressedState const& state, typename NextStateGenerator<ValueType, StateType>::StateToIdCallback stateToIdCallback) {
            // TODO
        }
        
        template<typename ValueType, typename StateType>
        void PrismNextStateGenerator<ValueType, StateType>::unpackStateIntoEvaluator(storm::storage::BitVector const& state) {
            for (auto const& booleanVariable : variableInformation.booleanVariables) {
                evaluator.setBooleanValue(booleanVariable.variable, state.get(booleanVariable.bitOffset));
            }
            for (auto const& integerVariable : variableInformation.integerVariables) {
                evaluator.setIntegerValue(integerVariable.variable, state.getAsInt(integerVariable.bitOffset, integerVariable.bitWidth) + integerVariable.lowerBound);
            }
        }

        template<typename ValueType, typename StateType>
        CompressedState PrismNextStateGenerator<ValueType, StateType>::applyUpdate(CompressedState const& state, storm::prism::Update const& update) {
            CompressedState newState(state);
            
            auto assignmentIt = update.getAssignments().begin();
            auto assignmentIte = update.getAssignments().end();
            
            // Iterate over all boolean assignments and carry them out.
            auto boolIt = variableInformation.booleanVariables.begin();
            for (; assignmentIt != assignmentIte && assignmentIt->getExpression().hasBooleanType(); ++assignmentIt) {
                while (assignmentIt->getVariable() != boolIt->variable) {
                    ++boolIt;
                }
                newState.set(boolIt->bitOffset, evaluator.asBool(assignmentIt->getExpression()));
            }
            
            // Iterate over all integer assignments and carry them out.
            auto integerIt = variableInformation.integerVariables.begin();
            for (; assignmentIt != assignmentIte && assignmentIt->getExpression().hasIntegerType(); ++assignmentIt) {
                while (assignmentIt->getVariable() != integerIt->variable) {
                    ++integerIt;
                }
                int_fast64_t assignedValue = evaluator.asInt(assignmentIt->getExpression());
                STORM_LOG_THROW(assignedValue <= integerIt->upperBound, storm::exceptions::WrongFormatException, "The update " << update << " leads to an out-of-bounds value (" << assignedValue << ") for the variable '" << assignmentIt->getVariableName() << "'.");
                newState.setFromInt(integerIt->bitOffset, integerIt->bitWidth, assignedValue - integerIt->lowerBound);
                STORM_LOG_ASSERT(static_cast<int_fast64_t>(newState.getAsInt(integerIt->bitOffset, integerIt->bitWidth)) + integerIt->lowerBound == assignedValue, "Writing to the bit vector bucket failed (read " << newState.getAsInt(integerIt->bitOffset, integerIt->bitWidth) << " but wrote " << assignedValue << ").");
            }
            
            // Check that we processed all assignments.
            STORM_LOG_ASSERT(assignmentIt == assignmentIte, "Not all assignments were consumed.");
            
            return newState;
        }
        
        template<typename ValueType, typename StateType>
        boost::optional<std::vector<std::vector<std::reference_wrapper<storm::prism::Command const>>>> PrismNextStateGenerator<ValueType, StateType>::getActiveCommandsByActionIndex(uint_fast64_t const& actionIndex) {
            boost::optional<std::vector<std::vector<std::reference_wrapper<storm::prism::Command const>>>> result((std::vector<std::vector<std::reference_wrapper<storm::prism::Command const>>>()));
            
            // Iterate over all modules.
            for (uint_fast64_t i = 0; i < program.getNumberOfModules(); ++i) {
                storm::prism::Module const& module = program.getModule(i);
                
                // If the module has no command labeled with the given action, we can skip this module.
                if (!module.hasActionIndex(actionIndex)) {
                    continue;
                }
                
                std::set<uint_fast64_t> const& commandIndices = module.getCommandIndicesByActionIndex(actionIndex);
                
                // If the module contains the action, but there is no command in the module that is labeled with
                // this action, we don't have any feasible command combinations.
                if (commandIndices.empty()) {
                    return boost::optional<std::vector<std::vector<std::reference_wrapper<storm::prism::Command const>>>>();
                }
                
                std::vector<std::reference_wrapper<storm::prism::Command const>> commands;
                
                // Look up commands by their indices and add them if the guard evaluates to true in the given state.
                for (uint_fast64_t commandIndex : commandIndices) {
                    storm::prism::Command const& command = module.getCommand(commandIndex);
                    if (evaluator.asBool(command.getGuardExpression())) {
                        commands.push_back(command);
                    }
                }
                
                // If there was no enabled command although the module has some command with the required action label,
                // we must not return anything.
                if (commands.size() == 0) {
                    return boost::optional<std::vector<std::vector<std::reference_wrapper<storm::prism::Command const>>>>();
                }
                
                result.get().push_back(std::move(commands));
            }
            
            return result;
        }
        
        template<typename ValueType, typename StateType>
        std::vector<Choice<ValueType>> PrismNextStateGenerator<ValueType, StateType>::getUnlabeledTransitions(CompressedState const& state, StateToIdCallback stateToIdCallback) {
            std::vector<Choice<ValueType>> result;
            
            // Iterate over all modules.
            for (uint_fast64_t i = 0; i < program.getNumberOfModules(); ++i) {
                storm::prism::Module const& module = program.getModule(i);
                
                // Iterate over all commands.
                for (uint_fast64_t j = 0; j < module.getNumberOfCommands(); ++j) {
                    storm::prism::Command const& command = module.getCommand(j);
                    
                    // Only consider unlabeled commands.
                    if (command.isLabeled()) continue;
                    
                    // Skip the command, if it is not enabled.
                    if (!evaluator.asBool(command.getGuardExpression())) {
                        continue;
                    }
                    
                    result.push_back(Choice<ValueType>(0, buildChoiceLabeling));
                    Choice<ValueType>& choice = result.back();
                    
                    // Remember the command labels only if we were asked to.
                    if (buildChoiceLabeling) {
                        choice.addChoiceLabel(command.getGlobalIndex());
                    }
                    
                    // Iterate over all updates of the current command.
                    ValueType probabilitySum = storm::utility::zero<ValueType>();
                    for (uint_fast64_t k = 0; k < command.getNumberOfUpdates(); ++k) {
                        storm::prism::Update const& update = command.getUpdate(k);
                        
                        // Obtain target state index and add it to the list of known states. If it has not yet been
                        // seen, we also add it to the set of states that have yet to be explored.
                        StateType stateIndex = stateToIdCallback(applyUpdate(state, update));
                        
                        // Update the choice by adding the probability/target state to it.
                        ValueType probability = evaluator.asRational(update.getLikelihoodExpression());
                        choice.addProbability(stateIndex, probability);
                        probabilitySum += probability;
                    }
                    
                    // Check that the resulting distribution is in fact a distribution.
                    STORM_LOG_THROW(!program.isDiscreteTimeModel() || comparator.isOne(probabilitySum), storm::exceptions::WrongFormatException, "Probabilities do not sum to one for command '" << command << "' (actually sum to " << probabilitySum << ").");
                }
            }
            
            return result;
        }
        
        template<typename ValueType, typename StateType>
        std::vector<Choice<ValueType>> PrismNextStateGenerator<ValueType, StateType>::getLabeledTransitions(CompressedState const& state, StateToIdCallback stateToIdCallback) {
            std::vector<Choice<ValueType>> result;
            
            for (uint_fast64_t actionIndex : program.getSynchronizingActionIndices()) {
                boost::optional<std::vector<std::vector<std::reference_wrapper<storm::prism::Command const>>>> optionalActiveCommandLists = getActiveCommandsByActionIndex(program, evaluator, actionIndex);
                
                // Only process this action label, if there is at least one feasible solution.
                if (optionalActiveCommandLists) {
                    std::vector<std::vector<std::reference_wrapper<storm::prism::Command const>>> const& activeCommandList = optionalActiveCommandLists.get();
                    std::vector<std::vector<std::reference_wrapper<storm::prism::Command const>>::const_iterator> iteratorList(activeCommandList.size());
                    
                    // Initialize the list of iterators.
                    for (size_t i = 0; i < activeCommandList.size(); ++i) {
                        iteratorList[i] = activeCommandList[i].cbegin();
                    }
                    
                    // As long as there is one feasible combination of commands, keep on expanding it.
                    bool done = false;
                    while (!done) {
                        std::unordered_map<CompressedState, ValueType>* currentTargetStates = new std::unordered_map<CompressedState, ValueType>();
                        std::unordered_map<CompressedState, ValueType>* newTargetStates = new std::unordered_map<CompressedState, ValueType>();
                        currentTargetStates->emplace(state, storm::utility::one<ValueType>());
                        
                        for (uint_fast64_t i = 0; i < iteratorList.size(); ++i) {
                            storm::prism::Command const& command = *iteratorList[i];
                            
                            for (uint_fast64_t j = 0; j < command.getNumberOfUpdates(); ++j) {
                                storm::prism::Update const& update = command.getUpdate(j);
                                
                                for (auto const& stateProbabilityPair : *currentTargetStates) {
                                    // Compute the new state under the current update and add it to the set of new target states.
                                    CompressedState newTargetState = applyUpdate(stateProbabilityPair.first, state, update);
                                    newTargetStates->emplace(newTargetState, stateProbabilityPair.second * evaluator.asRational(update.getLikelihoodExpression()));
                                }
                            }
                            
                            // If there is one more command to come, shift the target states one time step back.
                            if (i < iteratorList.size() - 1) {
                                delete currentTargetStates;
                                currentTargetStates = newTargetStates;
                                newTargetStates = new std::unordered_map<CompressedState, ValueType>();
                            }
                        }
                        
                        // At this point, we applied all commands of the current command combination and newTargetStates
                        // contains all target states and their respective probabilities. That means we are now ready to
                        // add the choice to the list of transitions.
                        result.push_back(Choice<ValueType>(actionIndex, buildChoiceLabeling));
                        
                        // Now create the actual distribution.
                        Choice<ValueType>& choice = result.back();
                        
                        // Remember the command labels only if we were asked to.
                        if (buildChoiceLabeling) {
                            // Add the labels of all commands to this choice.
                            for (uint_fast64_t i = 0; i < iteratorList.size(); ++i) {
                                choice.addChoiceLabel(iteratorList[i]->get().getGlobalIndex());
                            }
                        }
                        
                        ValueType probabilitySum = storm::utility::zero<ValueType>();
                        for (auto const& stateProbabilityPair : *newTargetStates) {
                            StateType actualIndex = stateToIdCallback(stateProbabilityPair.first);
                            choice.addProbability(actualIndex, stateProbabilityPair.second);
                            probabilitySum += stateProbabilityPair.second;
                        }
                        
                        // Check that the resulting distribution is in fact a distribution.
                        STORM_LOG_THROW(!program.isDiscreteTimeModel() || !comparator.isConstant(probabilitySum) || comparator.isOne(probabilitySum), storm::exceptions::WrongFormatException, "Sum of update probabilities do not some to one for some command (actually sum to " << probabilitySum << ").");
                        
                        // Dispose of the temporary maps.
                        delete currentTargetStates;
                        delete newTargetStates;
                        
                        // Now, check whether there is one more command combination to consider.
                        bool movedIterator = false;
                        for (int_fast64_t j = iteratorList.size() - 1; j >= 0; --j) {
                            ++iteratorList[j];
                            if (iteratorList[j] != activeCommandList[j].end()) {
                                movedIterator = true;
                            } else {
                                // Reset the iterator to the beginning of the list.
                                iteratorList[j] = activeCommandList[j].begin();
                            }
                        }
                        
                        done = !movedIterator;
                    }
                }
            }
            
            return result;
        }
    }
}