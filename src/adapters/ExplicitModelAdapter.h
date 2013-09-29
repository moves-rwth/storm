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

#include "src/ir/Program.h"
#include "src/ir/RewardModel.h"
#include "src/ir/StateReward.h"
#include "src/ir/TransitionReward.h"

#include "src/models/AbstractModel.h"
#include "src/models/Dtmc.h"
#include "src/models/Ctmc.h"
#include "src/models/Mdp.h"
#include "src/models/Ctmdp.h"
#include "src/models/AtomicPropositionsLabeling.h"
#include "src/storage/SparseMatrix.h"
#include "src/storage/LabeledValues.h"
#include "src/settings/Settings.h"
#include "src/exceptions/WrongFormatException.h"

#include <boost/algorithm/string.hpp>

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

namespace storm {
    namespace adapters {
        
        /*!
         * A state of the model, i.e. a valuation of all variables.
         */
        typedef std::pair<std::vector<bool>, std::vector<int_fast64_t>> StateType;
        
        /*!
         * A helper class that provides the functionality to compute a hash value for states of the model.
         */
        class StateHash {
        public:
            std::size_t operator()(StateType* state) const {
                size_t seed = 0;
                for (auto it : state->first) {
                    boost::hash_combine<bool>(seed, it);
                }
                for (auto it : state->second) {
                    boost::hash_combine<int_fast64_t>(seed, it);
                }
                return seed;
            }
        };
        
        /*!
         * A helper class that provides the functionality to compare states of the model.
         */
        class StateCompare {
        public:
            bool operator()(StateType* state1, StateType* state2) const {
                return *state1 == *state2;
            }
        };
        
        // A structure holding information about a particular choice.
        template<typename ValueType>
        struct Choice {
            Choice(std::string const& actionLabel) : distribution(), actionLabel(actionLabel), choiceLabels() {
                // Intentionally left empty.
            }
            
            /*!
             * Returns an iterator to the first element of this choice.
             *
             * @return An iterator to the first element of this choice.
             */
            typename std::map<uint_fast64_t, ValueType>::const_iterator begin() const {
                return distribution.cbegin();
            }
            
            /*!
             * Returns an iterator that points past the elements of this choice.
             *
             * @return An iterator that points past the elements of this choice.
             */
            typename std::map<uint_fast64_t, ValueType>::const_iterator end() const {
                return distribution.cend();
            }
            
            /*!
             * Adds the given label to the labels associated with this choice.
             *
             * @param label The label to associate with this choice.
             */
            void addChoiceLabel(uint_fast64_t label) {
                choiceLabels.insert(label);
            }
            
            // The distribution that is associated with the choice.
            std::map<uint_fast64_t, ValueType> distribution;
            
            // The label of the choice.
            std::string actionLabel;
            
            // The labels that are associated with this choice.
            std::set<uint_fast64_t> choiceLabels;
        };
        
        template<typename ValueType>
        void addProbabilityToChoice(Choice<ValueType>& choice, uint_fast64_t state, ValueType probability, std::set<uint_fast64_t> const& labels) {
            auto stateProbabilityPair = choice.distribution.find(state);
            
            if (stateProbabilityPair == choice.distribution.end()) {
                choice.distribution[state] = probability;
            } else {
                choice.distribution[state] += probability;
            }            
        }
        
        template<typename ValueType>
        void addProbabilityToChoice(Choice<storm::storage::LabeledValues<ValueType>>& choice, uint_fast64_t state, ValueType probability, std::set<uint_fast64_t> const& labels) {
            auto stateProbabilityPair = choice.distribution.find(state);
            
            if (stateProbabilityPair == choice.distribution.end()) {
                choice.distribution[state] = storm::storage::LabeledValues<ValueType>();
            }
            choice.distribution[state].addValue(probability);                
        }
        
        template<typename ValueType>
        class ExplicitModelAdapter {
        public:
            // A structure holding information about the reachable state space.
            struct StateInformation {
                StateInformation() : reachableStates(), stateToIndexMap(), numberOfChoices(), numberOfTransitions(0) {
                    // Intentionally left empty.
                }
                
                // A list of reachable states.
                std::vector<StateType*> reachableStates;
                
                // A mapping from states to indices in the list of reachable states.
                std::unordered_map<StateType*, uint_fast64_t, StateHash, StateCompare> stateToIndexMap;
                
                // A vector storing the number of choices for each state.
                std::vector<uint_fast64_t> numberOfChoices;
                
                // The total number of transitions discovered.
                uint_fast64_t numberOfTransitions;
            };
            
            // A structure holding the individual components of a model.
            struct ModelComponents {
                ModelComponents() : transitionMatrix(), stateLabeling(), nondeterministicChoiceIndices(), stateRewards(), transitionRewardMatrix(), choiceLabeling() {
                    // Intentionally left empty.
                }
                
                // The transition matrix.
                storm::storage::SparseMatrix<ValueType> transitionMatrix;
                
                // The state labeling.
                storm::models::AtomicPropositionsLabeling stateLabeling;
                
                // A vector indicating at which row the choices for a particular state begin.
                std::vector<uint_fast64_t> nondeterministicChoiceIndices;
                
                // The state reward vector.
                std::vector<ValueType> stateRewards;
                
                // A matrix storing the reward for particular transitions.
                storm::storage::SparseMatrix<ValueType> transitionRewardMatrix;
                
                // A vector that stores a labeling for each choice.
                std::vector<std::set<uint_fast64_t>> choiceLabeling;
            };
            
            // A structure storing information about the used variables of the program.
            struct VariableInformation {
                VariableInformation() : booleanVariables(), booleanVariableToIndexMap(), integerVariables(), integerVariableToIndexMap() {
                    // Intentinally left empty.
                }
                
                // List of all boolean variables.
                std::vector<storm::ir::BooleanVariable> booleanVariables;
                
                // A mapping of boolean variable names to their indices.
                std::map<std::string, uint_fast64_t> booleanVariableToIndexMap;
                
                // List of all integer variables.
                std::vector<storm::ir::IntegerVariable> integerVariables;
                
                // A mapping of integer variable names to their indices.
                std::map<std::string, uint_fast64_t> integerVariableToIndexMap;
            };
            
            /*!
             * Convert the program given at construction time to an abstract model. The type of the model is the one
             * specified in the program. The given reward model name selects the rewards that the model will contain.
             *
             * @param program The program to translate.
             * @param constantDefinitionString A string that contains a comma-separated definition of all undefined
             * constants in the model.
             * @param rewardModelName The name of reward model to be added to the model. This must be either a reward
             * model of the program or the empty string. In the latter case, the constructed model will contain no 
             * rewards.
             * @return The explicit model that was given by the probabilistic program.
             */
            static std::shared_ptr<storm::models::AbstractModel<ValueType>> translateProgram(storm::ir::Program program, std::string const& constantDefinitionString = "", std::string const& rewardModelName = "") {
                // Start by defining the undefined constants in the model.
                defineUndefinedConstants(program, constantDefinitionString);
                
                ModelComponents modelComponents = buildModelComponents(program, rewardModelName);
                
                std::shared_ptr<storm::models::AbstractModel<ValueType>> result;
                switch (program.getModelType()) {
                    case storm::ir::Program::DTMC:
                        result = std::shared_ptr<storm::models::AbstractModel<ValueType>>(new storm::models::Dtmc<ValueType>(std::move(modelComponents.transitionMatrix), std::move(modelComponents.stateLabeling), std::move(modelComponents.stateRewards), std::move(modelComponents.transitionRewardMatrix), std::move(modelComponents.choiceLabeling)));
                        break;
                    case storm::ir::Program::CTMC:
                        result = std::shared_ptr<storm::models::AbstractModel<ValueType>>(new storm::models::Ctmc<ValueType>(std::move(modelComponents.transitionMatrix), std::move(modelComponents.stateLabeling), std::move(modelComponents.stateRewards), std::move(modelComponents.transitionRewardMatrix), std::move(modelComponents.choiceLabeling)));
                        break;
                    case storm::ir::Program::MDP:
                        result = std::shared_ptr<storm::models::AbstractModel<ValueType>>(new storm::models::Mdp<ValueType>(std::move(modelComponents.transitionMatrix), std::move(modelComponents.stateLabeling), std::move(modelComponents.nondeterministicChoiceIndices), std::move(modelComponents.stateRewards), std::move(modelComponents.transitionRewardMatrix), std::move(modelComponents.choiceLabeling)));
                        break;
                    case storm::ir::Program::CTMDP:
                        result = std::shared_ptr<storm::models::AbstractModel<ValueType>>(new storm::models::Ctmdp<ValueType>(std::move(modelComponents.transitionMatrix), std::move(modelComponents.stateLabeling), std::move(modelComponents.nondeterministicChoiceIndices), std::move(modelComponents.stateRewards), std::move(modelComponents.transitionRewardMatrix), std::move(modelComponents.choiceLabeling)));
                        break;
                    default:
                        LOG4CPLUS_ERROR(logger, "Error while creating model from probabilistic program: cannot handle this model type.");
                        throw storm::exceptions::WrongFormatException() << "Error while creating model from probabilistic program: cannot handle this model type.";
                        break;
                }
                
                // Undefine the constants so that the program can be used again somewhere else.
                undefineUndefinedConstants(program);
                
                return result;
            }
            
        private:
            /*!
             * Sets some boolean variable in the given state object.
             *
             * @param state The state to modify.
             * @param index The index of the boolean variable to modify.
             * @param value The new value of the variable.
             */
            static void setValue(StateType* state, uint_fast64_t index, bool value) {
                std::get<0>(*state)[index] = value;
            }
            
            /*!
             * Set some integer variable in the given state object.
             *
             * @param state The state to modify.
             * @param index index of the integer variable to modify.
             * @param value The new value of the variable.
             */
            static void setValue(StateType* state, uint_fast64_t index, int_fast64_t value) {
                std::get<1>(*state)[index] = value;
            }
            
            /*!
             * Transforms a state into a somewhat readable string.
             *
             * @param state The state to transform into a string.
             * @return A string representation of the state.
             */
            static std::string toString(StateType const* state) {
                std::stringstream ss;
                for (unsigned int i = 0; i < state->first.size(); i++) ss << state->first[i] << "\t";
                for (unsigned int i = 0; i < state->second.size(); i++) ss << state->second[i] << "\t";
                return ss.str();
            }
            
            /*!
             * Applies an update to the given state and returns the resulting new state object. This methods does not
             * modify the given state but returns a new one.
             *
             * @param variableInformation A structure with information about the variables in the program.
             * @params state The state to which to apply the update.
             * @params update The update to apply.
             * @return The resulting state.
             */
            static StateType* applyUpdate(VariableInformation const& variableInformation, StateType const* state, storm::ir::Update const& update) {
                return applyUpdate(variableInformation, state, state, update);
            }
            
            /*!
             * Applies an update to the given state and returns the resulting new state object. The update is evaluated
             * over the variable values of the given base state. This methods does not modify the given state but
             * returns a new one.
             *
             * @param variableInformation A structure with information about the variables in the program.
             * @param state The state to which to apply the update.
             * @param baseState The state used for evaluating the update.
             * @param update The update to apply.
             * @return The resulting state.
             */
            static StateType* applyUpdate(VariableInformation const& variableInformation, StateType const* state, StateType const* baseState, storm::ir::Update const& update) {
                StateType* newState = new StateType(*state);
                for (auto variableAssignmentPair : update.getBooleanAssignments()) {
                    setValue(newState, variableInformation.booleanVariableToIndexMap.at(variableAssignmentPair.first), variableAssignmentPair.second.getExpression()->getValueAsBool(baseState));
                }
                for (auto variableAssignmentPair : update.getIntegerAssignments()) {
                    setValue(newState, variableInformation.integerVariableToIndexMap.at(variableAssignmentPair.first), variableAssignmentPair.second.getExpression()->getValueAsInt(baseState));
                }
                return newState;
            }
            

            /*!
             * Defines the undefined constants of the given program using the given string.
             *
             * @param program The program in which to define the constants.
             * @param constantDefinitionString A comma-separated list of constant definitions.
             */
            static void defineUndefinedConstants(storm::ir::Program& program, std::string const& constantDefinitionString) {
                if (!constantDefinitionString.empty()) {
                    // Parse the string that defines the undefined constants of the model and make sure that it contains exactly
                    // one value for each undefined constant of the model.
                    std::vector<std::string> definitions;
                    boost::split(definitions, constantDefinitionString, boost::is_any_of(","));
                    for (auto& definition : definitions) {
                        boost::trim(definition);
                        
                        // Check whether the token could be a legal constant definition.
                        uint_fast64_t positionOfAssignmentOperator = definition.find('=');
                        if (positionOfAssignmentOperator == std::string::npos) {
                            throw storm::exceptions::InvalidArgumentException() << "Illegal constant definition string: syntax error.";
                        }
                        
                        // Now extract the variable name and the value from the string.
                        std::string constantName = definition.substr(0, positionOfAssignmentOperator);
                        boost::trim(constantName);
                        std::string value = definition.substr(positionOfAssignmentOperator + 1);
                        boost::trim(value);
                        
                        // Check whether the constant is a legal undefined constant of the program and if so, of what type it is.
                        if (program.hasUndefinedBooleanConstant(constantName)) {
                            if (value == "true") {
                                program.getUndefinedBooleanConstantExpression(constantName)->define(true);
                            } else if (value == "false") {
                                program.getUndefinedBooleanConstantExpression(constantName)->define(false);
                            } else {
                                throw storm::exceptions::InvalidArgumentException() << "Illegal value for boolean constant: " << value << ".";
                            }
                        } else if (program.hasUndefinedIntegerConstant(constantName)) {
                            try {
                                int_fast64_t integerValue = std::stoi(value);
                                program.getUndefinedIntegerConstantExpression(constantName)->define(integerValue);
                            } catch (std::invalid_argument const&) {
                                throw storm::exceptions::InvalidArgumentException() << "Illegal value of integer constant: " << value << ".";
                            } catch (std::out_of_range const&) {
                                throw storm::exceptions::InvalidArgumentException() << "Illegal value of integer constant: " << value << " (value too big).";
                            }
                        } else if (program.hasUndefinedDoubleConstant(constantName)) {
                            try {
                                double doubleValue = std::stod(value);
                                program.getUndefinedDoubleConstantExpression(constantName)->define(doubleValue);
                            } catch (std::invalid_argument const&) {
                                throw storm::exceptions::InvalidArgumentException() << "Illegal value of double constant: " << value << ".";
                            } catch (std::out_of_range const&) {
                                throw storm::exceptions::InvalidArgumentException() << "Illegal value of double constant: " << value << " (value too big).";
                            }
                            
                        } else {
                            throw storm::exceptions::InvalidArgumentException() << "Illegal constant definition string: unknown undefined constant " << constantName << ".";
                        }
                    }
                }
            }
            
            /*!
             * Undefines all previously defined constants in the given program.
             *
             * @param program The program in which to undefine the constants.
             */
            static void undefineUndefinedConstants(storm::ir::Program& program) {
                for (auto nameExpressionPair : program.getBooleanUndefinedConstantExpressionsMap()) {
                    nameExpressionPair.second->undefine();
                }
                for (auto nameExpressionPair : program.getIntegerUndefinedConstantExpressionsMap()) {
                    nameExpressionPair.second->undefine();
                }
                for (auto nameExpressionPair : program.getDoubleUndefinedConstantExpressionsMap()) {
                    nameExpressionPair.second->undefine();
                }
            }
            
            /*!
             * Generates the initial state of the given program.
             *
             * @param program The program for which to construct the initial state.
             * @param variableInformation A structure with information about the variables in the program.
             * @return The initial state.
             */
            static StateType* getInitialState(storm::ir::Program const& program, VariableInformation const& variableInformation) {
                StateType* initialState = new StateType();
                initialState->first.resize(variableInformation.booleanVariables.size());
                initialState->second.resize(variableInformation.integerVariables.size());
                
                // Start with boolean variables.
                for (uint_fast64_t i = 0; i < variableInformation.booleanVariables.size(); ++i) {
                    // Check if an initial value is given
                    if (variableInformation.booleanVariables[i].getInitialValue().get() == nullptr) {
                        // If no initial value was given, we assume that the variable is initially false.
                        std::get<0>(*initialState)[i] = false;
                    } else {
                        // Initial value was given.
                        bool initialValue = variableInformation.booleanVariables[i].getInitialValue()->getValueAsBool(nullptr);
                        std::get<0>(*initialState)[i] = initialValue;
                    }
                }
                
                // Now process integer variables.
                for (uint_fast64_t i = 0; i < variableInformation.integerVariables.size(); ++i) {
                    // Check if an initial value was given.
                    if (variableInformation.integerVariables[i].getInitialValue().get() == nullptr) {
                        // No initial value was given, so we assume that the variable initially has the least value it can take.
                        std::get<1>(*initialState)[i] = variableInformation.integerVariables[i].getLowerBound()->getValueAsInt(nullptr);
                    } else {
                        // Initial value was given.
                        int_fast64_t initialValue = variableInformation.integerVariables[i].getInitialValue()->getValueAsInt(nullptr);
                        std::get<1>(*initialState)[i] = initialValue;
                    }
                }
                
                LOG4CPLUS_DEBUG(logger, "Generated initial state.");
                return initialState;
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
                    stateInformation.numberOfChoices.push_back(0);
                    stateInformation.reachableStates.push_back(state);
                    return std::make_pair(false, stateInformation.stateToIndexMap[state]);
                } else {
                    // The state was already encountered. Delete the copy of the old state and return its index.
                    delete state;
                    return std::make_pair(true, indexIt->second);
                }
            }
            
            /*!
             * Expands all unlabeled (i.e. independent) transitions of the given state and adds newly discovered states
             * to the given queue.
             *
             * @param program The program that defines the transitions.
             * @param stateInformation Provides information about the discovered state space.
             * @param variableInformation A structure with information about the variables in the program.
             * @param stateIndex The index of the state to expand.
             * @param stateQueue The queue of states into which newly discovered states are inserted.
             */
            static void exploreUnlabeledTransitions(storm::ir::Program const& program, StateInformation& stateInformation, VariableInformation const& variableInformation, uint_fast64_t stateIndex, std::queue<uint_fast64_t>& stateQueue) {
                StateType const* currentState = stateInformation.reachableStates[stateIndex];
                
                // Iterate over all modules.
                for (uint_fast64_t i = 0; i < program.getNumberOfModules(); ++i) {
                    storm::ir::Module const& module = program.getModule(i);

                    // Iterate over all commands.
                    for (uint_fast64_t j = 0; j < module.getNumberOfCommands(); ++j) {
                        storm::ir::Command const& command = module.getCommand(j);
                        
                        // Only consider unlabeled commands.
                        if (command.getActionName() != "") continue;
                        // Skip the command, if it is not enabled.
                        if (!command.getGuard()->getValueAsBool(currentState)) continue;
                        
                        ++stateInformation.numberOfChoices[stateIndex];
                        
                        std::set<uint_fast64_t> targetStates;
                        
                        // Iterate over all updates of the current command.
                        for (uint_fast64_t k = 0; k < command.getNumberOfUpdates(); ++k) {
                            storm::ir::Update const& update = command.getUpdate(k);
                            
                            // Obtain target state index.
                            std::pair<bool, uint_fast64_t> flagAndTargetStateIndexPair = getOrAddStateIndex(applyUpdate(variableInformation, currentState, update), stateInformation);
                            
                            // If the state has not yet been discovered, add it to the queue.
                            if (!flagAndTargetStateIndexPair.first) {
                                stateQueue.push(flagAndTargetStateIndexPair.second);
                            }
                            
                            // In any case, record that there is a transition to the newly found state to count the number
                            // of transitions correctly.
                            targetStates.insert(flagAndTargetStateIndexPair.second);
                        }
                        
                        stateInformation.numberOfTransitions += targetStates.size();
                    }
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
            static boost::optional<std::vector<std::list<storm::ir::Command>>> getActiveCommandsByAction(storm::ir::Program const& program, StateType const* state, std::string const& action) {
                boost::optional<std::vector<std::list<storm::ir::Command>>> result((std::vector<std::list<storm::ir::Command>>()));
                
                // Iterate over all modules.
                for (uint_fast64_t i = 0; i < program.getNumberOfModules(); ++i) {
                    storm::ir::Module const& module = program.getModule(i);
                    
                    // If the module has no command labeled with the given action, we can skip this module.
                    if (!module.hasAction(action)) {
                        continue;
                    }
                    
                    std::set<uint_fast64_t> const& commandIndices = module.getCommandsByAction(action);
                    std::list<storm::ir::Command> commands;
                    
                    // Look up commands by their indices and add them if the guard evaluates to true in the given state.
                    for (uint_fast64_t commandIndex : commandIndices) {
                        storm::ir::Command const& command = module.getCommand(commandIndex);
                        if (command.getGuard()->getValueAsBool(state)) {
                            commands.push_back(command);
                        }
                    }
                    
                    // If there was no enabled command although the module has some command with the required action label,
                    // we must not return anything.
                    if (commands.size() == 0) {
                        return boost::optional<std::vector<std::list<storm::ir::Command>>>();
                    }
                    
                    result.get().push_back(std::move(commands));
                }
                return result;
            }
            
            /*!
             * Expands all labeled (i.e. synchronizing) transitions of the given state and adds newly discovered states
             * to the given queue.
             *
             * @param program The program that defines the transitions.
             * @param stateInformation Provides information about the discovered state space.
             * @param variableInformation A structure with information about the variables in the program.
             * @param stateIndex The index of the state to expand.
             * @param stateQueue The queue of states into which newly discovered states are inserted.
             */
            static void exploreLabeledTransitions(storm::ir::Program const& program, StateInformation& stateInformation, VariableInformation const& variableInformation, uint_fast64_t stateIndex, std::queue<uint_fast64_t>& stateQueue) {
                for (std::string const& action : program.getActions()) {
                    StateType const* currentState = stateInformation.reachableStates[stateIndex];
                    boost::optional<std::vector<std::list<storm::ir::Command>>> optionalActiveCommandLists = getActiveCommandsByAction(program, currentState, action);
                    
                    // Only process this action label, if there is at least one feasible solution.
                    if (optionalActiveCommandLists) {
                        std::vector<std::list<storm::ir::Command>> const& activeCommandList = optionalActiveCommandLists.get();
                        std::vector<std::list<storm::ir::Command>::const_iterator> iteratorList(activeCommandList.size());
                        
                        // Initialize the list of iterators.
                        for (size_t i = 0; i < activeCommandList.size(); ++i) {
                            iteratorList[i] = activeCommandList[i].cbegin();
                        }
                        
                        // As long as there is one feasible combination of commands, keep on expanding it.
                        bool done = false;
                        while (!done) {
                            ++stateInformation.numberOfChoices[stateIndex];
                            
                            std::unordered_set<StateType*, StateHash, StateCompare>* currentTargetStates = new std::unordered_set<StateType*, StateHash, StateCompare>();
                            std::unordered_set<StateType*, StateHash, StateCompare>* newTargetStates = new std::unordered_set<StateType*, StateHash, StateCompare>();
                            currentTargetStates->insert(new StateType(*currentState));
                            
                            // FIXME: This does not check whether a global variable is written multiple times. While the
                            // behaviour for this is undefined anyway, a warning should be issued in that case.
                            for (uint_fast64_t i = 0; i < iteratorList.size(); ++i) {
                                storm::ir::Command const& command = *iteratorList[i];
                                
                                for (uint_fast64_t j = 0; j < command.getNumberOfUpdates(); ++j) {
                                    storm::ir::Update const& update = command.getUpdate(j);
                                    
                                    for (auto const& targetState : *currentTargetStates) {
                                        StateType* newTargetState = applyUpdate(variableInformation, targetState, currentState, update);
                                        
                                        auto existingState = newTargetStates->find(newTargetState);
                                        if (existingState == newTargetStates->end()) {
                                            newTargetStates->insert(newTargetState);
                                        } else {
                                            // If the state was already seen in one of the other updates, we need to delete this copy.
                                            delete newTargetState;
                                        }
                                    }
                                }
                                
                                // If there is one more command to come, shift the target states one time step back.
                                if (i < iteratorList.size() - 1) {
                                    for (auto const& state : *currentTargetStates) {
                                        delete state;
                                    }
                                    delete currentTargetStates;
                                    currentTargetStates = newTargetStates;
                                    newTargetStates = new std::unordered_set<StateType*, StateHash, StateCompare>();
                                }
                            }
                            
                            // If there are states that have not yet been discovered, add them to the queue.
                            for (auto const& state : *newTargetStates) {
                                std::pair<bool, uint_fast64_t> flagAndNewStateIndexPair = getOrAddStateIndex(state, stateInformation);
                                
                                if (!flagAndNewStateIndexPair.first) {
                                    stateQueue.push(flagAndNewStateIndexPair.second);
                                }
                            }
                            
                            stateInformation.numberOfTransitions += newTargetStates->size();
                            
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
            }
            
            /*!
             * Explores the reachable state space given by the program and returns a list of all states as well as a
             * mapping from states to their indices.
             *
             * @param program The program whose state space to explore.
             * @param variableInformation A structure with information about the variables in the program.
             * @return A structure containing all reachable states and a mapping from states to their indices.
             */
            static StateInformation exploreReachableStateSpace(storm::ir::Program const& program, VariableInformation const& variableInformation) {
                StateInformation stateInformation;
                
                // Initialize a queue and insert the initial state.
                std::queue<uint_fast64_t> stateQueue;
                StateType* initialState = getInitialState(program, variableInformation);
                getOrAddStateIndex(initialState, stateInformation);
                stateQueue.push(stateInformation.stateToIndexMap[initialState]);
                
                // Now explore the current state until there is no more reachable state.
                while (!stateQueue.empty()) {
                    uint_fast64_t currentStateIndex = stateQueue.front();
                    
                    exploreUnlabeledTransitions(program, stateInformation, variableInformation, currentStateIndex, stateQueue);
                    exploreLabeledTransitions(program, stateInformation, variableInformation, currentStateIndex, stateQueue);
                    
                    // If the state does not have any outgoing transitions, we fix this by adding a self-loop if the
                    // option was set.
                    if (stateInformation.numberOfChoices[currentStateIndex] == 0) {
                        if (storm::settings::Settings::getInstance()->isSet("fixDeadlocks")) {
                            ++stateInformation.numberOfTransitions;
                            ++stateInformation.numberOfChoices[currentStateIndex] = 1;
                        } else {
                            LOG4CPLUS_ERROR(logger, "Error while creating sparse matrix from probabilistic program: found deadlock state. For fixing these, please provide the appropriate option.");
                            throw storm::exceptions::WrongFormatException() << "Error while creating sparse matrix from probabilistic program: found deadlock state. For fixing these, please provide the appropriate option.";
                        }
                    }

                    
                    // After exploring a state, remove it from the queue.
                    stateQueue.pop();
                }
                
                return stateInformation;
            }
            
            /*!
             * Aggregates information about the variables in the program.
             *
             * @param program The program whose variables to aggregate.
             */
            static VariableInformation createVariableInformation(storm::ir::Program const& program) {
                VariableInformation result;
                
                uint_fast64_t numberOfIntegerVariables = 0;
                uint_fast64_t numberOfBooleanVariables = 0;
                
                // Count number of variables.
                numberOfBooleanVariables += program.getNumberOfGlobalBooleanVariables();
                numberOfIntegerVariables += program.getNumberOfGlobalIntegerVariables();
                for (uint_fast64_t i = 0; i < program.getNumberOfModules(); ++i) {
                    numberOfBooleanVariables += program.getModule(i).getNumberOfBooleanVariables();
                    numberOfIntegerVariables += program.getModule(i).getNumberOfIntegerVariables();
                }
                
                // Resize the variable vectors appropriately.
                result.booleanVariables.resize(numberOfBooleanVariables);
                result.integerVariables.resize(numberOfIntegerVariables);
                
                // Create variables.
                for (uint_fast64_t i = 0; i < program.getNumberOfGlobalBooleanVariables(); ++i) {
                    storm::ir::BooleanVariable const& var = program.getGlobalBooleanVariable(i);
                    result.booleanVariables[var.getGlobalIndex()] = var;
                    result.booleanVariableToIndexMap[var.getName()] = var.getGlobalIndex();
                }
                for (uint_fast64_t i = 0; i < program.getNumberOfGlobalIntegerVariables(); ++i) {
                    storm::ir::IntegerVariable const& var = program.getGlobalIntegerVariable(i);
                    result.integerVariables[var.getGlobalIndex()] = var;
                    result.integerVariableToIndexMap[var.getName()] = var.getGlobalIndex();
                }
                for (uint_fast64_t i = 0; i < program.getNumberOfModules(); ++i) {
                    storm::ir::Module const& module = program.getModule(i);
                    
                    for (uint_fast64_t j = 0; j < module.getNumberOfBooleanVariables(); ++j) {
                        storm::ir::BooleanVariable const& var = module.getBooleanVariable(j);
                        result.booleanVariables[var.getGlobalIndex()] = var;
                        result.booleanVariableToIndexMap[var.getName()] = var.getGlobalIndex();
                    }
                    for (uint_fast64_t j = 0; j < module.getNumberOfIntegerVariables(); ++j) {
                        storm::ir::IntegerVariable const& var = module.getIntegerVariable(j);
                        result.integerVariables[var.getGlobalIndex()] = var;
                        result.integerVariableToIndexMap[var.getName()] = var.getGlobalIndex();
                    }
                }
                
                return result;
            }
            
            static std::list<Choice<ValueType>> getUnlabeledTransitions(storm::ir::Program const& program, StateInformation const& stateInformation, VariableInformation const& variableInformation, uint_fast64_t stateIndex) {
                std::list<Choice<ValueType>> result;
                
                StateType const* currentState = stateInformation.reachableStates[stateIndex];
                
                // Iterate over all modules.
                for (uint_fast64_t i = 0; i < program.getNumberOfModules(); ++i) {
                    storm::ir::Module const& module = program.getModule(i);
                    
                    // Iterate over all commands.
                    for (uint_fast64_t j = 0; j < module.getNumberOfCommands(); ++j) {
                        storm::ir::Command const& command = module.getCommand(j);
                        
                        // Only consider unlabeled commands.
                        if (command.getActionName() != "") continue;
                        // Skip the command, if it is not enabled.
                        if (!command.getGuard()->getValueAsBool(currentState)) continue;
                        
                        result.push_back(Choice<ValueType>(""));
                        Choice<ValueType>& choice = result.back();
                        choice.addChoiceLabel(command.getGlobalIndex());
                        
                        double probabilitySum = 0;
                        // Iterate over all updates of the current command.
                        for (uint_fast64_t k = 0; k < command.getNumberOfUpdates(); ++k) {
                            storm::ir::Update const& update = command.getUpdate(k);
                            
                            // Obtain target state index.
                            uint_fast64_t targetStateIndex = stateInformation.stateToIndexMap.at(applyUpdate(variableInformation, currentState, update));
                            
                            // Check, if we already saw this state in another update and, if so, add up probabilities.
                            double probabilityToAdd = update.getLikelihoodExpression()->getValueAsDouble(currentState);
                            probabilitySum += probabilityToAdd;
                            std::set<uint_fast64_t> labels;
                            // FIXME: We have to retrieve the index of the update here, which is currently not possible.
                            // labels.insert(update.getGlobalIndex());
                            addProbabilityToChoice(choice, targetStateIndex, probabilityToAdd, labels);
                        }
                        
                        // Check that the resulting distribution is in fact a distribution.
                        if (std::abs(1 - probabilitySum) > storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble()) {
                            LOG4CPLUS_ERROR(logger, "Sum of update probabilities should be one for command:\n\t"  << command.toString());
                            throw storm::exceptions::WrongFormatException() << "Sum of update probabilities should be one for command:\n\t"  << command.toString();
                        }
                    }
                }
                
                return result;
            }
            
            static std::list<Choice<ValueType>> getLabeledTransitions(storm::ir::Program const& program, StateInformation const& stateInformation, VariableInformation const& variableInformation, uint_fast64_t stateIndex) {
                std::list<Choice<ValueType>> result;
                
                for (std::string const& action : program.getActions()) {
                    StateType const* currentState = stateInformation.reachableStates[stateIndex];
                    boost::optional<std::vector<std::list<storm::ir::Command>>> optionalActiveCommandLists = getActiveCommandsByAction(program, currentState, action);
                    
                    // Only process this action label, if there is at least one feasible solution.
                    if (optionalActiveCommandLists) {
                        std::vector<std::list<storm::ir::Command>> const& activeCommandList = optionalActiveCommandLists.get();
                        std::vector<std::list<storm::ir::Command>::const_iterator> iteratorList(activeCommandList.size());
                        
                        // Initialize the list of iterators.
                        for (size_t i = 0; i < activeCommandList.size(); ++i) {
                            iteratorList[i] = activeCommandList[i].cbegin();
                        }
                        
                        // As long as there is one feasible combination of commands, keep on expanding it.
                        bool done = false;
                        while (!done) {
                            std::unordered_map<StateType*, double, StateHash, StateCompare>* currentTargetStates = new std::unordered_map<StateType*, double, StateHash, StateCompare>();
                            std::unordered_map<StateType*, double, StateHash, StateCompare>* newTargetStates = new std::unordered_map<StateType*, double, StateHash, StateCompare>();
                            (*currentTargetStates)[new StateType(*currentState)] = 1.0;
                            
                            // FIXME: This does not check whether a global variable is written multiple times. While the
                            // behaviour for this is undefined anyway, a warning should be issued in that case.
                            for (uint_fast64_t i = 0; i < iteratorList.size(); ++i) {
                                storm::ir::Command const& command = *iteratorList[i];
                                
                                for (uint_fast64_t j = 0; j < command.getNumberOfUpdates(); ++j) {
                                    storm::ir::Update const& update = command.getUpdate(j);
                                    
                                    for (auto const& stateProbabilityPair : *currentTargetStates) {
                                        StateType* newTargetState = applyUpdate(variableInformation, stateProbabilityPair.first, currentState, update);
                                        
                                        auto existingStateProbabilityPair = newTargetStates->find(newTargetState);
                                        if (existingStateProbabilityPair == newTargetStates->end()) {
                                            (*newTargetStates)[newTargetState] = stateProbabilityPair.second * update.getLikelihoodExpression()->getValueAsDouble(currentState);
                                        } else {
                                            existingStateProbabilityPair->second += stateProbabilityPair.second * update.getLikelihoodExpression()->getValueAsDouble(currentState);
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
                                    newTargetStates = new std::unordered_map<StateType*, double, StateHash, StateCompare>();
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
                            
                            double probabilitySum = 0;
                            std::set<uint_fast64_t> labels;
                            for (auto const& stateProbabilityPair : *newTargetStates) {
                                uint_fast64_t newStateIndex = stateInformation.stateToIndexMap.at(stateProbabilityPair.first);
                                addProbabilityToChoice(choice, newStateIndex, stateProbabilityPair.second, labels);
                                probabilitySum += stateProbabilityPair.second;
                            }
                            
                            // Check that the resulting distribution is in fact a distribution.
                            if (std::abs(1 - probabilitySum) > storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble()) {
                                LOG4CPLUS_ERROR(logger, "Sum of update probabilities do not some to one for some command.");
                                throw storm::exceptions::WrongFormatException() << "Sum of update probabilities do not some to one for some command.";
                            }
                            
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
             *
             */
            static std::pair<std::vector<uint_fast64_t>, std::vector<std::set<uint_fast64_t>>> buildMatrices(storm::ir::Program const& program, VariableInformation const& variableInformation, std::vector<storm::ir::TransitionReward> const& transitionRewards, StateInformation const& stateInformation, bool deterministicModel, storm::storage::SparseMatrix<ValueType>& transitionMatrix, storm::storage::SparseMatrix<ValueType>& transitionRewardMatrix) {
                std::vector<uint_fast64_t> nondeterministicChoiceIndices(stateInformation.reachableStates.size() + 1);
                std::vector<std::set<uint_fast64_t>> choiceLabels;
                
                // Add transitions and rewards for all states.
                uint_fast64_t currentRow = 0;
                for (uint_fast64_t currentState = 0; currentState < stateInformation.reachableStates.size(); ++currentState) {
                    // Retrieve all choices for the current state.
                    std::list<Choice<ValueType>> allUnlabeledChoices = getUnlabeledTransitions(program, stateInformation, variableInformation, currentState);
                    std::list<Choice<ValueType>> allLabeledChoices = getLabeledTransitions(program, stateInformation, variableInformation, currentState);
                    
                    uint_fast64_t totalNumberOfChoices = allUnlabeledChoices.size() + allLabeledChoices.size();
                    
                    // If the current state does not have a single choice, we equip it with a self-loop if that was
                    // requested and issue an error otherwise.
                    if (totalNumberOfChoices == 0) {
                        if (storm::settings::Settings::getInstance()->isSet("fixDeadlocks")) {
                            transitionMatrix.insertNextValue(currentRow, currentState, storm::utility::constGetOne<ValueType>());
                            ++currentRow;
                        } else {
                            LOG4CPLUS_ERROR(logger, "Error while creating sparse matrix from probabilistic program: found deadlock state. For fixing these, please provide the appropriate option.");
                            throw storm::exceptions::WrongFormatException() << "Error while creating sparse matrix from probabilistic program: found deadlock state. For fixing these, please provide the appropriate option.";
                        }
                    } else {
                        // Then, based on whether the model is deterministic or not, either add the choices individually
                        // or compose them to one choice.
                        if (deterministicModel) {
                            std::map<uint_fast64_t, ValueType> globalDistribution;
                            std::set<uint_fast64_t> allChoiceLabels;
                            
                            // Combine all the choices and scale them with the total number of choices of the current state.
                            for (auto const& choice : allUnlabeledChoices) {
                                allChoiceLabels.insert(choice.choiceLabels.begin(), choice.choiceLabels.end());
                                for (auto const& stateProbabilityPair : choice) {
                                    auto existingStateProbabilityPair = globalDistribution.find(stateProbabilityPair.first);
                                    if (existingStateProbabilityPair == globalDistribution.end()) {
                                        globalDistribution[stateProbabilityPair.first] += stateProbabilityPair.second / totalNumberOfChoices;
                                    } else {
                                        globalDistribution[stateProbabilityPair.first] = stateProbabilityPair.second / totalNumberOfChoices;
                                    }
                                }
                            }
                            for (auto const& choice : allLabeledChoices) {
                                for (auto const& stateProbabilityPair : choice) {
                                    auto existingStateProbabilityPair = globalDistribution.find(stateProbabilityPair.first);
                                    if (existingStateProbabilityPair == globalDistribution.end()) {
                                        globalDistribution[stateProbabilityPair.first] += stateProbabilityPair.second / totalNumberOfChoices;
                                    } else {
                                        globalDistribution[stateProbabilityPair.first] = stateProbabilityPair.second / totalNumberOfChoices;
                                    }
                                }
                            }

                            
                            // Now add the resulting distribution as the only choice of the current state.
                            nondeterministicChoiceIndices[currentState] = currentRow;
                            choiceLabels.push_back(allChoiceLabels);
                            
                            for (auto const& stateProbabilityPair : globalDistribution) {
                                transitionMatrix.insertNextValue(currentRow, stateProbabilityPair.first, stateProbabilityPair.second);
                            }
                            
                            ++currentRow;
                        } else {
                            // If the model is nondeterministic, we add all choices individually.
                            nondeterministicChoiceIndices[currentState] = currentRow;
                            
                            // First, process all unlabeled choices.
                            for (auto const& choice : allUnlabeledChoices) {
                                std::map<uint_fast64_t, ValueType> stateToRewardMap;
                                choiceLabels.emplace_back(std::move(choice.choiceLabels));
                                
                                for (auto const& stateProbabilityPair : choice) {
                                    transitionMatrix.insertNextValue(currentRow, stateProbabilityPair.first, stateProbabilityPair.second);
                                    
                                    // Now add all rewards that match this choice.
                                    for (auto const& transitionReward : transitionRewards) {
                                        if (transitionReward.getActionName() == "" && transitionReward.getStatePredicate()->getValueAsBool(stateInformation.reachableStates.at(currentState))) {
                                            stateToRewardMap[stateProbabilityPair.first] += transitionReward.getRewardValue()->getValueAsDouble(stateInformation.reachableStates.at(currentState));
                                        }
                                    }

                                }
                                
                                // Add all transition rewards to the matrix.
                                for (auto const& stateRewardPair : stateToRewardMap) {
                                    transitionRewardMatrix.insertNextValue(currentRow, stateRewardPair.first, stateRewardPair.second);
                                }
                                
                                ++currentRow;
                            }
                            
                            // Then, process all labeled choices.
                            for (auto const& choice : allLabeledChoices) {
                                std::map<uint_fast64_t, ValueType> stateToRewardMap;
                                choiceLabels.emplace_back(std::move(choice.choiceLabels));

                                for (auto const& stateProbabilityPair : choice) {
                                    transitionMatrix.insertNextValue(currentRow, stateProbabilityPair.first, stateProbabilityPair.second);
                                    
                                    // Now add all rewards that match this choice.
                                    for (auto const& transitionReward : transitionRewards) {
                                        if (transitionReward.getActionName() == "" && transitionReward.getStatePredicate()->getValueAsBool(stateInformation.reachableStates.at(currentState))) {
                                            stateToRewardMap[stateProbabilityPair.first] += transitionReward.getRewardValue()->getValueAsDouble(stateInformation.reachableStates.at(currentState));
                                        }
                                    }

                                }
                                
                                // Add all transition rewards to the matrix.
                                for (auto const& stateRewardPair : stateToRewardMap) {
                                    transitionRewardMatrix.insertNextValue(currentRow, stateRewardPair.first, stateRewardPair.second);
                                }

                                ++currentRow;
                            }
                        }
                    }
                }
                
                nondeterministicChoiceIndices[stateInformation.reachableStates.size()] = currentRow;
                
                return std::make_pair(nondeterministicChoiceIndices, choiceLabels);
            }
            
            /*!
             * Explores the state space of the given program and returns the components of the model as a result.
             *
             * @param program The program whose state space to explore.
             * @param rewardModelName The name of the reward model that is to be considered. If empty, no reward model
             * is considered.
             * @return A structure containing the components of the resulting model.
             */
            static ModelComponents buildModelComponents(storm::ir::Program const& program, std::string const& rewardModelName) {
                ModelComponents modelComponents;
                
                VariableInformation variableInformation = createVariableInformation(program);
                
                // Start by exploring the state space.
                StateInformation stateInformation = exploreReachableStateSpace(program, variableInformation);
                
                // Then build the transition and reward matrices for the reachable states.
                bool deterministicModel = program.getModelType() == storm::ir::Program::DTMC || program.getModelType() == storm::ir::Program::CTMC;
                if (deterministicModel) {
                    modelComponents.transitionMatrix = storm::storage::SparseMatrix<ValueType>(stateInformation.reachableStates.size());
                    modelComponents.transitionMatrix.initialize();
                    modelComponents.transitionRewardMatrix = storm::storage::SparseMatrix<ValueType>(stateInformation.reachableStates.size());
                    modelComponents.transitionRewardMatrix.initialize();
                } else {
                    uint_fast64_t totalNumberOfChoices = 0;
                    for (auto numberOfChoices : stateInformation.numberOfChoices) {
                        totalNumberOfChoices += numberOfChoices;
                    }
                    modelComponents.transitionMatrix = storm::storage::SparseMatrix<ValueType>(totalNumberOfChoices, stateInformation.reachableStates.size());
                    modelComponents.transitionMatrix.initialize();
                    modelComponents.transitionRewardMatrix = storm::storage::SparseMatrix<ValueType>(totalNumberOfChoices);
                    modelComponents.transitionRewardMatrix.initialize();
                }
                
                // Get the selected reward model or create an empty one if none is selected.
                storm::ir::RewardModel const& rewardModel = rewardModelName != "" ? program.getRewardModel(rewardModelName) : storm::ir::RewardModel();
                
                // Build the transition and reward matrices.
                std::pair<std::vector<uint_fast64_t>, std::vector<std::set<uint_fast64_t>>> nondeterministicChoiceIndicesAndChoiceLabelsPair = buildMatrices(program, variableInformation, rewardModel.getTransitionRewards(), stateInformation, deterministicModel, modelComponents.transitionMatrix, modelComponents.transitionRewardMatrix);
                modelComponents.nondeterministicChoiceIndices = std::move(nondeterministicChoiceIndicesAndChoiceLabelsPair.first);
                modelComponents.choiceLabeling = std::move(nondeterministicChoiceIndicesAndChoiceLabelsPair.second);
                
                // Finalize the resulting matrices.
                modelComponents.transitionMatrix.finalize();
                modelComponents.transitionRewardMatrix.finalize();
                
                // Now build the state labeling.
                modelComponents.stateLabeling = buildStateLabeling(program, variableInformation, stateInformation);
                
                // Finally, construct the state rewards.
                modelComponents.stateRewards = buildStateRewards(rewardModel.getStateRewards(), stateInformation);
                
                // After everything has been created, we can delete the states.
                for (auto state : stateInformation.reachableStates) {
                    delete state;
                }
                
                return modelComponents;
            }
            
            static storm::models::AtomicPropositionsLabeling buildStateLabeling(storm::ir::Program const& program, VariableInformation const& variableInformation, StateInformation const& stateInformation) {
                std::map<std::string, std::shared_ptr<storm::ir::expressions::BaseExpression>> const& labels = program.getLabels();
                
                storm::models::AtomicPropositionsLabeling result(stateInformation.reachableStates.size(), labels.size() + 1);
                
                // Initialize labeling.
                for (auto const& labelExpressionPair : labels) {
                    result.addAtomicProposition(labelExpressionPair.first);
                }
                for (uint_fast64_t index = 0; index < stateInformation.reachableStates.size(); index++) {
                    for (auto const& labelExpressionPair: labels) {
                        // Add label to state, if the corresponding expression is true.
                        if (labelExpressionPair.second->getValueAsBool(stateInformation.reachableStates[index])) {
                            result.addAtomicPropositionToState(labelExpressionPair.first, index);
                        }
                    }
                }
                
                // Also label the initial state with the special label "init".
                result.addAtomicProposition("init");
                StateType* initialState = getInitialState(program, variableInformation);
                uint_fast64_t initialIndex = stateInformation.stateToIndexMap.at(initialState);
                result.addAtomicPropositionToState("init", initialIndex);
                delete initialState;
                
                return result;
            }

            static std::vector<ValueType> buildStateRewards(std::vector<storm::ir::StateReward> const& rewards, StateInformation const& stateInformation) {
                std::vector<ValueType> result(stateInformation.reachableStates.size());
                for (uint_fast64_t index = 0; index < stateInformation.reachableStates.size(); index++) {
                    result[index] = 0;
                    for (auto const& reward : rewards) {
                        // Add this reward to the state if the state is included in the state reward.
                        if (reward.getStatePredicate()->getValueAsBool(stateInformation.reachableStates[index])) {
                            result[index] += reward.getRewardValue()->getValueAsDouble(stateInformation.reachableStates[index]);
                        }
                    }
                }
                return result;
            }
        };
        
    } // namespace adapters
} // namespace storm

#endif	/* STORM_ADAPTERS_EXPLICITMODELADAPTER_H */
