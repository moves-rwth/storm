/* 
 * File:   ExplicitModelAdapter.h
 * Author: Christian Dehnert
 *
 * Created on March 15, 2013, 11:42 AM
 */

#ifndef STORM_ADAPTERS_EXPLICITMODELADAPTER_H
#define	STORM_ADAPTERS_EXPLICITMODELADAPTER_H

#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>
#include <queue>
#include <boost/functional/hash.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/algorithm/string.hpp>

#include "src/storage/prism/Program.h"
#include "src/storage/expressions/SimpleValuation.h"
#include "src/utility/PrismUtility.h"
#include "src/models/AbstractModel.h"
#include "src/models/Dtmc.h"
#include "src/models/Ctmc.h"
#include "src/models/Mdp.h"
#include "src/models/Ctmdp.h"
#include "src/models/AtomicPropositionsLabeling.h"
#include "src/storage/SparseMatrix.h"
#include "src/settings/SettingsManager.h"
#include "src/utility/macros.h"
#include "src/utility/ConstantsComparator.h"
#include "src/exceptions/WrongFormatException.h"
#include "src/storage/expressions/ExpressionEvaluation.h"

namespace storm {
    namespace adapters {
		
        using namespace storm::utility::prism;
                
        template<typename ValueType>
        class ExplicitModelAdapter {
        public:
            typedef storm::expressions::SimpleValuation StateType;
            typedef storm::expressions::SimpleValuationPointerHash StateHash;
            typedef storm::expressions::SimpleValuationPointerCompare StateCompare;
            typedef storm::expressions::SimpleValuationPointerLess StateLess;
            
            // A structure holding information about the reachable state space.
            struct StateInformation {
                StateInformation() : reachableStates(), stateToIndexMap() {
                    // Intentionally left empty.
                }
                
                // A list of reachable states.
                std::vector<StateType*> reachableStates;
                
                // A list of initial states.
                std::vector<uint_fast64_t> initialStateIndices;
                
                // A mapping from states to indices in the list of reachable states.
                std::unordered_map<StateType*, uint_fast64_t, StateHash, StateCompare> stateToIndexMap;
            };
            
            // A structure storing information about the used variables of the program.
            struct VariableInformation {
                // A mapping of (integer) variable to their lower/upper bounds.
                std::map<std::string, std::pair<int_fast64_t, int_fast64_t>> variableToBoundsMap;
            };
            
            // A structure holding the individual components of a model.
            struct ModelComponents {
                ModelComponents() : transitionMatrix(), stateLabeling(),  stateRewards(), transitionRewardMatrix(), choiceLabeling() {
                    // Intentionally left empty.
                }
                
                // The transition matrix.
                storm::storage::SparseMatrix<ValueType> transitionMatrix;
                
                // The state labeling.
                storm::models::AtomicPropositionsLabeling stateLabeling;
                
                // The state reward vector.
                std::vector<ValueType> stateRewards;
                
                // A matrix storing the reward for particular transitions.
                storm::storage::SparseMatrix<ValueType> transitionRewardMatrix;
                
                // A vector that stores a labeling for each choice.
                std::vector<boost::container::flat_set<uint_fast64_t>> choiceLabeling;
            };
            
            /*!
             * Convert the program given at construction time to an abstract model. The type of the model is the one
             * specified in the program. The given reward model name selects the rewards that the model will contain.
             *
             * @param program The program to translate.
             * @param constantDefinitionString A string that contains a comma-separated definition of all undefined
             * constants in the model.
             * @param rewardModel The reward model that is to be built.
             * @return The explicit model that was given by the probabilistic program.
             */
            static std::unique_ptr<storm::models::AbstractModel<ValueType>> translateProgram(storm::prism::Program program, std::string const& rewardModelName = "", std::string const& constantDefinitionString = "") {
                // Start by defining the undefined constants in the model.
                // First, we need to parse the constant definition string.
                std::map<std::string, storm::expressions::Expression> constantDefinitions = storm::utility::prism::parseConstantDefinitionString(program, constantDefinitionString);
                
                storm::prism::Program preparedProgram = program.defineUndefinedConstants(constantDefinitions);
#ifdef PARAMETRIC_SYSTEMS
				STORM_LOG_THROW((std::is_same<ValueType, RationalFunction>::value || !preparedProgram.hasUndefinedConstants()), storm::exceptions::InvalidArgumentException, "Program still contains undefined constants.");
#else
				STORM_LOG_THROW(!preparedProgram.hasUndefinedConstants(), storm::exceptions::InvalidArgumentException, "Program still contains undefined constants.");
#endif                
				
                // Now that we have defined all the constants in the program, we need to substitute their appearances in
                // all expressions in the program so we can then evaluate them without having to store the values of the
                // constants in the state (i.e., valuation).
                preparedProgram = preparedProgram.substituteConstants();
                storm::prism::RewardModel const& rewardModel = rewardModelName != "" || preparedProgram.hasRewardModel(rewardModelName) ? preparedProgram.getRewardModel(rewardModelName) : storm::prism::RewardModel();
                
                ModelComponents modelComponents = buildModelComponents(preparedProgram, rewardModel);
                
                std::unique_ptr<storm::models::AbstractModel<ValueType>> result;
                switch (program.getModelType()) {
                    case storm::prism::Program::ModelType::DTMC:
                        result = std::unique_ptr<storm::models::AbstractModel<ValueType>>(new storm::models::Dtmc<ValueType>(std::move(modelComponents.transitionMatrix), std::move(modelComponents.stateLabeling), rewardModel.hasStateRewards() ? std::move(modelComponents.stateRewards) : boost::optional<std::vector<ValueType>>(), rewardModel.hasTransitionRewards() ? std::move(modelComponents.transitionRewardMatrix) : boost::optional<storm::storage::SparseMatrix<ValueType>>(), std::move(modelComponents.choiceLabeling)));
                        break;
                    case storm::prism::Program::ModelType::CTMC:
                        result = std::unique_ptr<storm::models::AbstractModel<ValueType>>(new storm::models::Ctmc<ValueType>(std::move(modelComponents.transitionMatrix), std::move(modelComponents.stateLabeling), rewardModel.hasStateRewards() ? std::move(modelComponents.stateRewards) : boost::optional<std::vector<ValueType>>(), rewardModel.hasTransitionRewards() ? std::move(modelComponents.transitionRewardMatrix) : boost::optional<storm::storage::SparseMatrix<ValueType>>(), std::move(modelComponents.choiceLabeling)));
                        break;
                    case storm::prism::Program::ModelType::MDP:
                        result = std::unique_ptr<storm::models::AbstractModel<ValueType>>(new storm::models::Mdp<ValueType>(std::move(modelComponents.transitionMatrix), std::move(modelComponents.stateLabeling), rewardModel.hasStateRewards() ? std::move(modelComponents.stateRewards) : boost::optional<std::vector<ValueType>>(), rewardModel.hasTransitionRewards() ? std::move(modelComponents.transitionRewardMatrix) : boost::optional<storm::storage::SparseMatrix<ValueType>>(), std::move(modelComponents.choiceLabeling)));
                        break;
                    case storm::prism::Program::ModelType::CTMDP:
                        result = std::unique_ptr<storm::models::AbstractModel<ValueType>>(new storm::models::Ctmdp<ValueType>(std::move(modelComponents.transitionMatrix), std::move(modelComponents.stateLabeling), rewardModel.hasStateRewards() ? std::move(modelComponents.stateRewards) : boost::optional<std::vector<ValueType>>(), rewardModel.hasTransitionRewards() ? std::move(modelComponents.transitionRewardMatrix) : boost::optional<storm::storage::SparseMatrix<ValueType>>(), std::move(modelComponents.choiceLabeling)));
                        break;
                    default:
                        LOG4CPLUS_ERROR(logger, "Error while creating model from probabilistic program: cannot handle this model type.");
                        throw storm::exceptions::WrongFormatException() << "Error while creating model from probabilistic program: cannot handle this model type.";
                        break;
                }
                
                return result;
            }
            
        private:
            /*!
             * Applies an update to the given state and returns the resulting new state object. This methods does not
             * modify the given state but returns a new one.
             *
             * @params state The state to which to apply the update.
             * @params update The update to apply.
             * @return The resulting state.
             */
            static StateType* applyUpdate(VariableInformation const& variableInformation, StateType const* state, storm::prism::Update const& update) {
                return applyUpdate(variableInformation, state, state, update);
            }
            
            /*!
             * Applies an update to the given state and returns the resulting new state object. The update is evaluated
             * over the variable values of the given base state. This methods does not modify the given state but
             * returns a new one.
             *
             * @param state The state to which to apply the update.
             * @param baseState The state used for evaluating the update.
             * @param update The update to apply.
             * @return The resulting state.
             */
            static StateType* applyUpdate(VariableInformation const& variableInformation, StateType const* state, StateType const* baseState, storm::prism::Update const& update) {
                StateType* newState = new StateType(*state);
                
                // This variable needs to be declared prior to the switch, because of C++ rules.
                int_fast64_t newValue = 0;
                for (auto const& assignment : update.getAssignments()) {
                    switch (assignment.getExpression().getReturnType()) {
                        case storm::expressions::ExpressionReturnType::Bool: newState->setBooleanValue(assignment.getVariableName(), assignment.getExpression().evaluateAsBool(baseState)); break;
                        case storm::expressions::ExpressionReturnType::Int:
                        {
                            newValue = assignment.getExpression().evaluateAsInt(baseState);
                            auto const& boundsPair = variableInformation.variableToBoundsMap.find(assignment.getVariableName());
                            STORM_LOG_THROW(boundsPair->second.first <= newValue && newValue <= boundsPair->second.second, storm::exceptions::InvalidArgumentException, "Invalid value " << newValue << " for variable '" << assignment.getVariableName() << "'.");
                            newState->setIntegerValue(assignment.getVariableName(), newValue); break;
                        }
                        default: STORM_LOG_ASSERT(false, "Invalid type of assignment."); break;
                    }
                }
                return newState;
            }
                                    
            /*!
             * Retrieves the state id of the given state. If the state has not been encountered yet, it will be added to
             * the lists of all states with a new id. If the state was already known, the object that is pointed to by
             * the given state pointer is deleted and the old state id is returned. Note that the pointer should not be
             * used after invoking this method.
             *
             * @param state A pointer to a state for which to retrieve the index. This must not be used after the call.
             * @param stateInformation The information about the already explored part of the reachable state space.
             * @return A pair indicating whether the state was already discovered before and the state id of the state.
             */
            static std::pair<bool, uint_fast64_t> getOrAddStateIndex(StateType* state, StateInformation& stateInformation) {
                // Check, if the state was already registered.
                auto indexIt = stateInformation.stateToIndexMap.find(state);
                
                if (indexIt == stateInformation.stateToIndexMap.end()) {
                    // The state has not been seen, yet, so add it to the list of all reachable states.
                    stateInformation.stateToIndexMap[state] = stateInformation.reachableStates.size();
                    stateInformation.reachableStates.push_back(state);
                    return std::make_pair(false, stateInformation.stateToIndexMap[state]);
                } else {
                    // The state was already encountered. Delete the copy of the old state and return its index.
                    delete state;
                    return std::make_pair(true, indexIt->second);
                }
            }
    
            /*!
             * Retrieves all commands that are labeled with the given label and enabled in the given state, grouped by
             * modules.
             *
             * This function will iterate over all modules and retrieve all commands that are labeled with the given
             * action and active (i.e. enabled) in the current state. The result is a list of lists of commands in which
             * the inner lists contain all commands of exactly one module. If a module does not have *any* (including
             * disabled) commands, there will not be a list of commands of that module in the result. If, however, the
             * module has a command with a relevant label, but no enabled one, nothing is returned to indicate that there
             * is no legal transition possible.
             *
             * @param The program in which to search for active commands.
             * @param state The current state.
             * @param action The action label to select.
             * @return A list of lists of active commands or nothing.
             */
            static boost::optional<std::vector<std::list<storm::prism::Command>>> getActiveCommandsByAction(storm::prism::Program const& program, StateType const* state, std::string const& action) {
                boost::optional<std::vector<std::list<storm::prism::Command>>> result((std::vector<std::list<storm::prism::Command>>()));
                
                // Iterate over all modules.
                for (uint_fast64_t i = 0; i < program.getNumberOfModules(); ++i) {
                    storm::prism::Module const& module = program.getModule(i);
                    
                    // If the module has no command labeled with the given action, we can skip this module.
                    if (!module.hasAction(action)) {
                        continue;
                    }
                    
                    std::set<uint_fast64_t> const& commandIndices = module.getCommandIndicesByAction(action);
                    
                    // If the module contains the action, but there is no command in the module that is labeled with
                    // this action, we don't have any feasible command combinations.
                    if (commandIndices.empty()) {
                        return boost::optional<std::vector<std::list<storm::prism::Command>>>();
                    }
                    
                    std::list<storm::prism::Command> commands;
                    
                    // Look up commands by their indices and add them if the guard evaluates to true in the given state.
                    for (uint_fast64_t commandIndex : commandIndices) {
                        storm::prism::Command const& command = module.getCommand(commandIndex);
                        if (command.getGuardExpression().evaluateAsBool(state)) {
                            commands.push_back(command);
                        }
                    }
                    
                    // If there was no enabled command although the module has some command with the required action label,
                    // we must not return anything.
                    if (commands.size() == 0) {
                        return boost::optional<std::vector<std::list<storm::prism::Command>>>();
                    }
                    
                    result.get().push_back(std::move(commands));
                }
                return result;
            }
                        
            static std::list<Choice<ValueType>> getUnlabeledTransitions(storm::prism::Program const& program, StateInformation& stateInformation, VariableInformation const& variableInformation, uint_fast64_t stateIndex, std::queue<uint_fast64_t>& stateQueue, storm::utility::ConstantsComparator<ValueType> const& comparator, expressions::ExpressionEvaluation<ValueType>& eval) {
                std::list<Choice<ValueType>> result;
                
                StateType const* currentState = stateInformation.reachableStates[stateIndex];

                // Iterate over all modules.
                for (uint_fast64_t i = 0; i < program.getNumberOfModules(); ++i) {
                    storm::prism::Module const& module = program.getModule(i);
                    
                    // Iterate over all commands.
                    for (uint_fast64_t j = 0; j < module.getNumberOfCommands(); ++j) {
                        storm::prism::Command const& command = module.getCommand(j);
                        
                        // Only consider unlabeled commands.
                        if (command.getActionName() != "") continue;

                        // Skip the command, if it is not enabled.
                        if (!command.getGuardExpression().evaluateAsBool(currentState)) {
                            continue;
                        }
                        
                        result.push_back(Choice<ValueType>(""));
                        Choice<ValueType>& choice = result.back();
                        choice.addChoiceLabel(command.getGlobalIndex());
                        
                        ValueType probabilitySum = utility::constantZero<ValueType>();
                        // Iterate over all updates of the current command.
                        for (uint_fast64_t k = 0; k < command.getNumberOfUpdates(); ++k) {
                            storm::prism::Update const& update = command.getUpdate(k);
                            
                            // Obtain target state index.
                            std::pair<bool, uint_fast64_t> flagTargetStateIndexPair = getOrAddStateIndex(applyUpdate(variableInformation, currentState, update), stateInformation);
                            
                            // If the state has not been discovered yet, add it to the queue of states to be explored.
                            if (!flagTargetStateIndexPair.first) {
                                stateQueue.push(flagTargetStateIndexPair.second);
                            }
                            
                            // Update the choice by adding the probability/target state to it.
                            ValueType probabilityToAdd = eval.evaluate(update.getLikelihoodExpression(), currentState);
                            probabilitySum += probabilityToAdd;
                            boost::container::flat_set<uint_fast64_t> labels;
							labels.insert(update.getGlobalIndex());
							addProbabilityToChoice(choice, flagTargetStateIndexPair.second, probabilityToAdd, labels);
                        }
                        
                        // Check that the resulting distribution is in fact a distribution.
                        STORM_LOG_THROW(!comparator.isConstant(probabilitySum) || comparator.isOne(probabilitySum), storm::exceptions::WrongFormatException, "Probabilities do not sum to one for command '" << command << "' (sum = " << probabilitySum << ").");
                    }
                }
                
                return result;
            }
            
            static std::list<Choice<ValueType>> getLabeledTransitions(storm::prism::Program const& program, StateInformation& stateInformation, VariableInformation const& variableInformation, uint_fast64_t stateIndex, std::queue<uint_fast64_t>& stateQueue, storm::utility::ConstantsComparator<ValueType> const& comparator, expressions::ExpressionEvaluation<ValueType>& eval) {
                std::list<Choice<ValueType>> result;
                
                for (std::string const& action : program.getActions()) {
                    StateType const* currentState = stateInformation.reachableStates[stateIndex];
                    boost::optional<std::vector<std::list<storm::prism::Command>>> optionalActiveCommandLists = getActiveCommandsByAction(program, currentState, action);
                    
                    // Only process this action label, if there is at least one feasible solution.
                    if (optionalActiveCommandLists) {
                        std::vector<std::list<storm::prism::Command>> const& activeCommandList = optionalActiveCommandLists.get();
                        std::vector<std::list<storm::prism::Command>::const_iterator> iteratorList(activeCommandList.size());
                        
                        // Initialize the list of iterators.
                        for (size_t i = 0; i < activeCommandList.size(); ++i) {
                            iteratorList[i] = activeCommandList[i].cbegin();
                        }
                        
                        // As long as there is one feasible combination of commands, keep on expanding it.
                        bool done = false;
                        while (!done) {
                            std::unordered_map<StateType*, storm::storage::LabeledValues<ValueType>, StateHash, StateCompare>* currentTargetStates = new std::unordered_map<StateType*, storm::storage::LabeledValues<ValueType>, StateHash, StateCompare>();
                            std::unordered_map<StateType*, storm::storage::LabeledValues<ValueType>, StateHash, StateCompare>* newTargetStates = new std::unordered_map<StateType*, storm::storage::LabeledValues<ValueType>, StateHash, StateCompare>();
                            (*currentTargetStates)[new StateType(*currentState)] = storm::storage::LabeledValues<ValueType>(ValueType(1));
                            
                            // FIXME: This does not check whether a global variable is written multiple times. While the
                            // behaviour for this is undefined anyway, a warning should be issued in that case.
                            for (uint_fast64_t i = 0; i < iteratorList.size(); ++i) {
                                storm::prism::Command const& command = *iteratorList[i];
                                
                                for (uint_fast64_t j = 0; j < command.getNumberOfUpdates(); ++j) {
                                    storm::prism::Update const& update = command.getUpdate(j);
                                    
                                    for (auto const& stateProbabilityPair : *currentTargetStates) {
                                        StateType* newTargetState = applyUpdate(variableInformation, stateProbabilityPair.first, currentState, update);

                                        storm::storage::LabeledValues<ValueType> newProbability;
                                        
                                        ValueType updateProbability = eval.evaluate(update.getLikelihoodExpression(),currentState);
                                        for (auto const& valueLabelSetPair : stateProbabilityPair.second) {
                                            // Copy the label set, so we can modify it.
                                            boost::container::flat_set<uint_fast64_t> newLabelSet = valueLabelSetPair.second;
                                            newLabelSet.insert(update.getGlobalIndex());
                                            
                                            newProbability.addValue(valueLabelSetPair.first * updateProbability, newLabelSet);
                                        }
                                        
                                        auto existingStateProbabilityPair = newTargetStates->find(newTargetState);
                                        if (existingStateProbabilityPair == newTargetStates->end()) {
                                            (*newTargetStates)[newTargetState] = newProbability;
                                        } else {                                            
                                            existingStateProbabilityPair->second += newProbability;
                                            
                                            // If the state was already seen in one of the other updates, we need to delete this copy.
                                            delete newTargetState;
                                        }
                                    }
                                }
                                
                                // If there is one more command to come, shift the target states one time step back.
                                if (i < iteratorList.size() - 1) {
                                    for (auto const& stateProbabilityPair : *currentTargetStates) {
                                        delete stateProbabilityPair.first;
                                    }
                                    
                                    delete currentTargetStates;
                                    currentTargetStates = newTargetStates;
                                    newTargetStates = new std::unordered_map<StateType*, storm::storage::LabeledValues<ValueType>, StateHash, StateCompare>();
                                }
                            }
                            
                            // At this point, we applied all commands of the current command combination and newTargetStates
                            // contains all target states and their respective probabilities. That means we are now ready to
                            // add the choice to the list of transitions.
                            result.push_back(Choice<ValueType>(action));
                            
                            // Now create the actual distribution.
                            Choice<ValueType>& choice = result.back();
                            
                            // Add the labels of all commands to this choice.
                            for (uint_fast64_t i = 0; i < iteratorList.size(); ++i) {
                                choice.addChoiceLabel(iteratorList[i]->getGlobalIndex());
                            }
                            
                            ValueType probabilitySum = utility::constantZero<ValueType>();
                            for (auto const& stateProbabilityPair : *newTargetStates) {
                                std::pair<bool, uint_fast64_t> flagTargetStateIndexPair = getOrAddStateIndex(stateProbabilityPair.first, stateInformation);

                                // If the state has not been discovered yet, add it to the queue of states to be explored.
                                if (!flagTargetStateIndexPair.first) {
                                    stateQueue.push(flagTargetStateIndexPair.second);
                                }
                                
                                for (auto const& probabilityLabelPair : stateProbabilityPair.second) {
                                    addProbabilityToChoice(choice, flagTargetStateIndexPair.second, probabilityLabelPair.first, probabilityLabelPair.second);
                                    probabilitySum += probabilityLabelPair.first;
                                }
                            }
                            
                            // Check that the resulting distribution is in fact a distribution.
                            STORM_LOG_THROW(!comparator.isConstant(probabilitySum) || comparator.isOne(probabilitySum), storm::exceptions::WrongFormatException, "Sum of update probabilities do sum to " << probabilitySum << " to one for some command.");
                            
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
            
            /*!
             * Builds the transition matrix and the transition reward matrix based for the given program.
             *
             * @param program The program for which to build the matrices.
             * @param variableInformation A structure containing information about the variables in the program.
             * @param transitionRewards A list of transition rewards that are to be considered in the transition reward
             * matrix.
             * @param stateInformation A structure containing information about the states of the program.
             * @param deterministicModel A flag indicating whether the model is supposed to be deterministic or not.
             * @param transitionMatrix A reference to an initialized matrix which is filled with all transitions by this
             * function.
             * @param transitionRewardMatrix A reference to an initialized matrix which is filled with all transition
             * rewards by this function.
             * @return A tuple containing a vector with all rows at which the nondeterministic choices of each state begin
             * and a vector containing the labels associated with each choice.
             */
            static std::vector<boost::container::flat_set<uint_fast64_t>> buildMatrices(storm::prism::Program const& program, VariableInformation const& variableInformation, std::vector<storm::prism::TransitionReward> const& transitionRewards, StateInformation& stateInformation, bool deterministicModel, storm::storage::SparseMatrixBuilder<ValueType>& transitionMatrixBuilder, storm::storage::SparseMatrixBuilder<ValueType>& transitionRewardMatrixBuilder, storm::utility::ConstantsComparator<ValueType> const& comparator, expressions::ExpressionEvaluation<ValueType>& eval) {
                std::vector<boost::container::flat_set<uint_fast64_t>> choiceLabels;
                
                // Initialize a queue and insert the initial state.
                std::queue<uint_fast64_t> stateQueue;
                StateType* initialState = new StateType;
                for (auto const& booleanVariable : program.getGlobalBooleanVariables()) {
                    initialState->addBooleanIdentifier(booleanVariable.getName(), booleanVariable.getInitialValueExpression().evaluateAsBool());
                }
                for (auto const& integerVariable : program.getGlobalIntegerVariables()) {
                    initialState->addIntegerIdentifier(integerVariable.getName(), integerVariable.getInitialValueExpression().evaluateAsInt());
                }
                for (auto const& module : program.getModules()) {
                    for (auto const& booleanVariable : module.getBooleanVariables()) {
			            initialState->addBooleanIdentifier(booleanVariable.getName(), booleanVariable.getInitialValueExpression().evaluateAsBool());
                    }
                    for (auto const& integerVariable : module.getIntegerVariables()) {
                        initialState->addIntegerIdentifier(integerVariable.getName(), integerVariable.getInitialValueExpression().evaluateAsInt());
                    }
                }
			
                std::pair<bool, uint_fast64_t> addIndexPair = getOrAddStateIndex(initialState, stateInformation);
                stateInformation.initialStateIndices.push_back(addIndexPair.second);
                stateQueue.push(stateInformation.stateToIndexMap[initialState]);
                
                // Now explore the current state until there is no more reachable state.
                uint_fast64_t currentRow = 0;
                while (!stateQueue.empty()) {
                    uint_fast64_t currentState = stateQueue.front();
            
                    // Retrieve all choices for the current state.
                    std::list<Choice<ValueType>> allUnlabeledChoices = getUnlabeledTransitions(program, stateInformation, variableInformation, currentState, stateQueue, comparator, eval);
                    std::list<Choice<ValueType>> allLabeledChoices = getLabeledTransitions(program, stateInformation, variableInformation, currentState, stateQueue, comparator, eval);
                    
                    uint_fast64_t totalNumberOfChoices = allUnlabeledChoices.size() + allLabeledChoices.size();
                    
                    // If the current state does not have a single choice, we equip it with a self-loop if that was
                    // requested and issue an error otherwise.
                    if (totalNumberOfChoices == 0) {
                        if (!storm::settings::generalSettings().isDontFixDeadlocksSet()) {
                            // Insert empty choice labeling for added self-loop transitions.
                            choiceLabels.push_back(boost::container::flat_set<uint_fast64_t>());
                            transitionMatrixBuilder.addNextValue(currentRow, currentState, storm::utility::constantOne<ValueType>());
                            ++currentRow;
                        } else {
                            LOG4CPLUS_ERROR(logger, "Error while creating sparse matrix from probabilistic program: found deadlock state. For fixing these, please provide the appropriate option.");
                            throw storm::exceptions::WrongFormatException() << "Error while creating sparse matrix from probabilistic program: found deadlock state. For fixing these, please provide the appropriate option.";
                        }
                    } else {
                        // Then, based on whether the model is deterministic or not, either add the choices individually
                        // or compose them to one choice.
                        if (deterministicModel) {
                            Choice<ValueType> globalChoice("");
                            std::map<uint_fast64_t, ValueType> stateToRewardMap;
                            boost::container::flat_set<uint_fast64_t> allChoiceLabels;
                            
                            // Combine all the choices and scale them with the total number of choices of the current state.
                            for (auto const& choice : allUnlabeledChoices) {
                                globalChoice.addChoiceLabels(choice.getChoiceLabels());
                                for (auto const& stateProbabilityPair : choice) {
                                    globalChoice.getOrAddEntry(stateProbabilityPair.first) += stateProbabilityPair.second / totalNumberOfChoices;
                                    
                                    // Now add all rewards that match this choice.
                                    for (auto const& transitionReward : transitionRewards) {
                                        std::cout << transitionReward.getStatePredicateExpression() << std::endl;
                                        if (transitionReward.getActionName() == "" && transitionReward.getStatePredicateExpression().evaluateAsBool(stateInformation.reachableStates.at(currentState))) {
                                            stateToRewardMap[stateProbabilityPair.first] += eval.evaluate(transitionReward.getRewardValueExpression(),stateInformation.reachableStates.at(currentState));
                                        }
                                    }
                                }
                            }
                            for (auto const& choice : allLabeledChoices) {
                                globalChoice.addChoiceLabels(choice.getChoiceLabels());
                                for (auto const& stateProbabilityPair : choice) {
                                    globalChoice.getOrAddEntry(stateProbabilityPair.first) += stateProbabilityPair.second / totalNumberOfChoices;
                                
                                    // Now add all rewards that match this choice.
                                    for (auto const& transitionReward : transitionRewards) {
                                        if (transitionReward.getActionName() == choice.getActionLabel() && transitionReward.getStatePredicateExpression().evaluateAsBool(stateInformation.reachableStates.at(currentState))) {
                                            stateToRewardMap[stateProbabilityPair.first] += eval.evaluate(transitionReward.getRewardValueExpression(),stateInformation.reachableStates.at(currentState));
                                        }
                                    }
                                }
                            }

                            
                            // Now add the resulting distribution as the only choice of the current state.
                            choiceLabels.push_back(globalChoice.getChoiceLabels());
                            
                            for (auto const& stateProbabilityPair : globalChoice) {
                                transitionMatrixBuilder.addNextValue(currentRow, stateProbabilityPair.first, stateProbabilityPair.second);
                            }
                            
                            // Add all transition rewards to the matrix and add dummy entry if there is none.
                            if (stateToRewardMap.size() > 0) {
                                for (auto const& stateRewardPair : stateToRewardMap) {
                                    transitionRewardMatrixBuilder.addNextValue(currentRow, stateRewardPair.first, stateRewardPair.second);
                                }
                            }
                            
                            ++currentRow;
                        } else {
                            // If the model is nondeterministic, we add all choices individually.
                            transitionMatrixBuilder.newRowGroup(currentRow);
                            transitionRewardMatrixBuilder.newRowGroup(currentRow);
                            
                            // First, process all unlabeled choices.
                            for (auto const& choice : allUnlabeledChoices) {
                                std::map<uint_fast64_t, ValueType> stateToRewardMap;
                                choiceLabels.emplace_back(std::move(choice.getChoiceLabels()));
                                
                                for (auto const& stateProbabilityPair : choice) {
                                    transitionMatrixBuilder.addNextValue(currentRow, stateProbabilityPair.first, stateProbabilityPair.second);
                                    
                                    // Now add all rewards that match this choice.
                                    for (auto const& transitionReward : transitionRewards) {
                                        if (transitionReward.getActionName() == "" && transitionReward.getStatePredicateExpression().evaluateAsBool(stateInformation.reachableStates.at(currentState))) {
                                            stateToRewardMap[stateProbabilityPair.first] += eval.evaluate(transitionReward.getRewardValueExpression(),stateInformation.reachableStates.at(currentState));
                                        }
                                    }

                                }
                                
                                // Add all transition rewards to the matrix and add dummy entry if there is none.
                                if (stateToRewardMap.size() > 0) {
                                    for (auto const& stateRewardPair : stateToRewardMap) {
                                        transitionRewardMatrixBuilder.addNextValue(currentRow, stateRewardPair.first, stateRewardPair.second);
                                    }
                                }
                                
                                ++currentRow;
                            }
                            
                            // Then, process all labeled choices.
                            for (auto const& choice : allLabeledChoices) {
                                std::map<uint_fast64_t, ValueType> stateToRewardMap;
                                choiceLabels.emplace_back(std::move(choice.getChoiceLabels()));

                                for (auto const& stateProbabilityPair : choice) {
                                    transitionMatrixBuilder.addNextValue(currentRow, stateProbabilityPair.first, stateProbabilityPair.second);
                                    
                                    // Now add all rewards that match this choice.
                                    for (auto const& transitionReward : transitionRewards) {
                                        if (transitionReward.getActionName() == choice.getActionLabel() && transitionReward.getStatePredicateExpression().evaluateAsBool(stateInformation.reachableStates.at(currentState))) {
                                            stateToRewardMap[stateProbabilityPair.first] += eval.evaluate(transitionReward.getRewardValueExpression(),stateInformation.reachableStates.at(currentState));
                                        }
                                    }

                                }
                                
                                // Add all transition rewards to the matrix and add dummy entry if there is none.
                                if (stateToRewardMap.size() > 0) {
                                    for (auto const& stateRewardPair : stateToRewardMap) {
                                        transitionRewardMatrixBuilder.addNextValue(currentRow, stateRewardPair.first, stateRewardPair.second);
                                    }
                                }

                                ++currentRow;
                            }
                        }
                    }
                    
                    stateQueue.pop();
                }
                
                return choiceLabels;
            }
            
            /*!
             * Explores the state space of the given program and returns the components of the model as a result.
             *
             * @param program The program whose state space to explore.
             * @param rewardModel The reward model that is to be considered.
             * @return A structure containing the components of the resulting model.
             */
            static ModelComponents buildModelComponents(storm::prism::Program const& program, storm::prism::RewardModel const& rewardModel) {
                ModelComponents modelComponents;
				expressions::ExpressionEvaluation<ValueType> eval;
				
                VariableInformation variableInformation;
                for (auto const& integerVariable : program.getGlobalIntegerVariables()) {
                    variableInformation.variableToBoundsMap[integerVariable.getName()] = std::make_pair(integerVariable.getLowerBoundExpression().evaluateAsInt(), integerVariable.getUpperBoundExpression().evaluateAsInt());
                }
                for (auto const& module : program.getModules()) {
                    for (auto const& integerVariable : module.getIntegerVariables()) {
                        variableInformation.variableToBoundsMap[integerVariable.getName()] = std::make_pair(integerVariable.getLowerBoundExpression().evaluateAsInt(), integerVariable.getUpperBoundExpression().evaluateAsInt());
                    }
                }
                
                // Create the structure for storing the reachable state space.
                StateInformation stateInformation;
                
                // Determine whether we have to combine different choices to one or whether this model can have more than
                // one choice per state.
                bool deterministicModel = program.getModelType() == storm::prism::Program::ModelType::DTMC || program.getModelType() == storm::prism::Program::ModelType::CTMC;

                // Build the transition and reward matrices.
                storm::storage::SparseMatrixBuilder<ValueType> transitionMatrixBuilder(0, 0, 0, false, !deterministicModel, 0);
                storm::storage::SparseMatrixBuilder<ValueType> transitionRewardMatrixBuilder(0, 0, 0, false, !deterministicModel, 0);
                storm::utility::ConstantsComparator<ValueType> comparator;
                modelComponents.choiceLabeling = buildMatrices(program, variableInformation, rewardModel.getTransitionRewards(), stateInformation, deterministicModel, transitionMatrixBuilder, transitionRewardMatrixBuilder, comparator, eval);
                
                // Finalize the resulting matrices.
                modelComponents.transitionMatrix = transitionMatrixBuilder.build();
                modelComponents.transitionRewardMatrix = transitionRewardMatrixBuilder.build(modelComponents.transitionMatrix.getRowCount(), modelComponents.transitionMatrix.getColumnCount(), modelComponents.transitionMatrix.getRowGroupCount());
                
                // Now build the state labeling.
                modelComponents.stateLabeling = buildStateLabeling(program, variableInformation, stateInformation);
                
                // Finally, construct the state rewards.
                modelComponents.stateRewards = buildStateRewards(rewardModel.getStateRewards(), stateInformation, eval);
                
                // After everything has been created, we can delete the states.
                for (auto state : stateInformation.reachableStates) {
                    delete state;
                }
                
                return modelComponents;
            }
            
            /*!
             * Builds the state labeling for the given program.
             *
             * @param program The program for which to build the state labeling.
             * @param variableInformation Information about the variables in the program.
             * @param stateInformation Information about the state space of the program.
             * @return The state labeling of the given program.
             */
            static storm::models::AtomicPropositionsLabeling buildStateLabeling(storm::prism::Program const& program, VariableInformation const& variableInformation, StateInformation const& stateInformation) {
                std::vector<storm::prism::Label> const& labels = program.getLabels();
                
                storm::models::AtomicPropositionsLabeling result(stateInformation.reachableStates.size(), labels.size() + 1);
                
                // Initialize labeling.
                for (auto const& label : labels) {
                    result.addAtomicProposition(label.getName());
                }
                for (uint_fast64_t index = 0; index < stateInformation.reachableStates.size(); index++) {
                    for (auto const& label : labels) {
                        // Add label to state, if the corresponding expression is true.
                        if (label.getStatePredicateExpression().evaluateAsBool(stateInformation.reachableStates[index])) {
                            result.addAtomicPropositionToState(label.getName(), index);
                        }
                    }
                }
                
                // Also label the initial state with the special label "init".
                result.addAtomicProposition("init");
                for (auto const& index : stateInformation.initialStateIndices) {
                    result.addAtomicPropositionToState("init", index);
                }
                
                return result;
            }

            /*!
             * Builds the state rewards for the given state space.
             *
             * @param rewards A vector of state rewards to consider.
             * @param stateInformation Information about the state space.
             * @return A vector containing the state rewards for the state space.
             */
            static std::vector<ValueType> buildStateRewards(std::vector<storm::prism::StateReward> const& rewards, StateInformation const& stateInformation, expressions::ExpressionEvaluation<ValueType>& eval) {
                std::vector<ValueType> result(stateInformation.reachableStates.size());
                for (uint_fast64_t index = 0; index < stateInformation.reachableStates.size(); index++) {
                    result[index] = ValueType(0);
                    for (auto const& reward : rewards) {
                        // Add this reward to the state if the state is included in the state reward.
                        if (reward.getStatePredicateExpression().evaluateAsBool(stateInformation.reachableStates[index])) {
                            result[index] += eval.evaluate(reward.getRewardValueExpression(),stateInformation.reachableStates[index]);
                        }
                    }
                }
                return result;
            }
        };
		
		
        
    } // namespace adapters
} // namespace storm

#endif	/* STORM_ADAPTERS_EXPLICITMODELADAPTER_H */
