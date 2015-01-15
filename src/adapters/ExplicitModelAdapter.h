#ifndef STORM_ADAPTERS_EXPLICITMODELADAPTER_H
#define	STORM_ADAPTERS_EXPLICITMODELADAPTER_H

#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>
#include <queue>
#include <cstdint>
#include <boost/functional/hash.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/algorithm/string.hpp>

#include "src/storage/prism/Program.h"
#include "src/storage/expressions/SimpleValuation.h"
#include "src/storage/expressions/ExprtkExpressionEvaluator.h"
#include "src/storage/BitVectorHashMap.h"
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
#include "src/exceptions/WrongFormatException.h"

namespace storm {
    namespace adapters {
        
        using namespace storm::utility::prism;
                
        template<typename ValueType>
        class ExplicitModelAdapter {
        public:
            typedef storm::storage::BitVector StateType;
            
            // A structure holding information about the reachable state space.
            struct StateInformation {
                StateInformation(uint64_t bitsPerState) : bitsPerState(bitsPerState), reachableStates(), stateToIndexMap(bitsPerState, 1000000) {
                    // Intentionally left empty.
                }
                
                // The number of bits of each state.
                uint64_t bitsPerState;
                
                // A list of reachable states as indices in the stateToIndexMap.
                std::vector<storm::storage::BitVector> reachableStates;
                
                // A list of initial states in terms of their global indices.
                std::vector<uint32_t> initialStateIndices;
                
                // A mapping from reachable states to their indices.
                storm::storage::BitVectorHashMap<uint32_t> stateToIndexMap;
            };
            
            // A structure storing information about the used variables of the program.
            struct VariableInformation {
                struct BooleanVariableInformation {
                    BooleanVariableInformation(storm::expressions::Variable const& variable, bool initialValue, uint_fast64_t bitOffset) : variable(variable), initialValue(initialValue), bitOffset(bitOffset) {
                        // Intentionally left empty.
                    }
                    
                    storm::expressions::Variable variable;
                    bool initialValue;
                    uint_fast64_t bitOffset;
                };

                struct IntegerVariableInformation {
                    IntegerVariableInformation(storm::expressions::Variable const& variable, int_fast64_t initialValue, int_fast64_t lowerBound, int_fast64_t upperBound, uint_fast64_t bitOffset, uint_fast64_t bitWidth) : variable(variable), initialValue(initialValue), lowerBound(lowerBound), upperBound(upperBound), bitOffset(bitOffset), bitWidth(bitWidth) {
                        // Intentionally left empty.
                    }
                    
                    storm::expressions::Variable variable;
                    int_fast64_t initialValue;
                    int_fast64_t lowerBound;
                    int_fast64_t upperBound;
                    uint_fast64_t bitOffset;
                    uint_fast64_t bitWidth;
                };
                
                uint_fast64_t getBitOffset(storm::expressions::Variable const& variable) const {
                    auto const& booleanIndex = booleanVariableToIndexMap.find(variable);
                    if (booleanIndex != booleanVariableToIndexMap.end()) {
                        return booleanVariables[booleanIndex].bitOffset;
                    }
                    auto const& integerIndex = integerVariableToIndexMap.find(variable);
                    if (integerIndex != integerVariableToIndexMap.end()) {
                        return integerVariables[integerIndex].bitOffset;
                    }
                    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Cannot look-up bit index of unknown variable.");
                }
                
                uint_fast64_t getBitWidth(storm::expressions::Variable const& variable) const {
                    auto const& integerIndex = integerVariableToIndexMap.find(variable);
                    if (integerIndex != integerVariableToIndexMap.end()) {
                        return integerVariables[integerIndex].bitWidth;
                    }
                    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Cannot look-up bit width of unknown variable.");
                }
                
                // The list of boolean variables.
                std::map<storm::expressions::Variable, uint_fast64_t> booleanVariableToIndexMap;
                std::vector<BooleanVariableInformation> booleanVariables;

                // The list of integer variables.
                std::map<storm::expressions::Variable, uint_fast64_t> integerVariableToIndexMap;
                std::vector<IntegerVariableInformation> integerVariables;
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
            static std::unique_ptr<storm::models::AbstractModel<ValueType>> translateProgram(storm::prism::Program program, bool rewards = true, std::string const& rewardModelName = "", std::string const& constantDefinitionString = "") {
                // Start by defining the undefined constants in the model.
                // First, we need to parse the constant definition string.
                std::map<storm::expressions::Variable, storm::expressions::Expression> constantDefinitions = storm::utility::prism::parseConstantDefinitionString(program, constantDefinitionString);
                
                storm::prism::Program preparedProgram = program.defineUndefinedConstants(constantDefinitions);
                STORM_LOG_THROW(!preparedProgram.hasUndefinedConstants(), storm::exceptions::InvalidArgumentException, "Program still contains undefined constants.");
                
                // Now that we have defined all the constants in the program, we need to substitute their appearances in
                // all expressions in the program so we can then evaluate them without having to store the values of the
                // constants in the state (i.e., valuation).
                preparedProgram = preparedProgram.substituteConstants();
                storm::prism::RewardModel rewardModel = storm::prism::RewardModel();
                
                // Select the appropriate reward model.
                if (rewards) {
                    // If a specific reward model was selected or one with the empty name exists, select it.
                    if (rewardModelName != "" || preparedProgram.hasRewardModel(rewardModelName)) {
                        rewardModel = preparedProgram.getRewardModel(rewardModelName);
                    } else if (preparedProgram.hasRewardModel()) {
                        // Otherwise, we select the first one.
                        rewardModel = preparedProgram.getRewardModel(0);
                    }
                }

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
            static void unpackStateIntoEvaluator(storm::storage::BitVector const& currentState, VariableInformation const& variableInformation, storm::expressions::ExprtkExpressionEvaluator& evaluator) {
                for (auto const& booleanVariable : variableInformation.booleanVariables) {
                    evaluator.setBooleanValue(booleanVariable.variable, currentState.get(booleanVariable.bitOffset));
                }
                for (auto const& integerVariable : variableInformation.integerVariables) {
                    evaluator.setIntegerValue(integerVariable.variable, currentState.getAsInt(integerVariable.bitOffset, integerVariable.bitWidth) + integerVariable.lowerBound);
                }
            }
            
            /*!
             * Applies an update to the given state and returns the resulting new state object. This methods does not
             * modify the given state but returns a new one.
             *
             * @params state The state to which to apply the update.
             * @params update The update to apply.
             * @return The resulting state.
             */
            static StateType applyUpdate(VariableInformation const& variableInformation, StateType const& state, storm::prism::Update const& update, storm::expressions::ExprtkExpressionEvaluator const& evaluator) {
                return applyUpdate(variableInformation, state, state, update, evaluator);
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
            static StateType applyUpdate(VariableInformation const& variableInformation, StateType const& state, StateType const& baseState, storm::prism::Update const& update, storm::expressions::ExprtkExpressionEvaluator const& evaluator) {
                StateType newState(state);

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
                    newState.setFromInt(integerIt->bitOffset, integerIt->bitWidth, evaluator.asInt(assignmentIt->getExpression()) - integerIt->lowerBound);
                }
                
                // Check that we processed all assignments.
                STORM_LOG_ASSERT(assignmentIt == assignmentIte, "Not all assignments were consumed.");
                
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
            static uint32_t getOrAddStateIndex(StateType const& state, StateInformation& stateInformation, std::queue<storm::storage::BitVector>& stateQueue) {
                uint32_t newIndex = stateInformation.reachableStates.size();
                
                // Check, if the state was already registered.
                std::pair<uint32_t, std::size_t> actualIndexBucketPair = stateInformation.stateToIndexMap.findOrAddAndGetBucket(state, newIndex);
                
                if (actualIndexBucketPair.first == newIndex) {
                    stateQueue.push(state);
                    stateInformation.reachableStates.push_back(state);
                }
                
                return actualIndexBucketPair.first;
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
             * @param actionIndex The index of the action label to select.
             * @return A list of lists of active commands or nothing.
             */
            static boost::optional<std::vector<std::vector<std::reference_wrapper<storm::prism::Command const>>>> getActiveCommandsByActionIndex(storm::prism::Program const& program,storm::expressions::ExprtkExpressionEvaluator const& evaluator, uint_fast64_t const& actionIndex) {
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
                        
            static std::vector<Choice<ValueType>> getUnlabeledTransitions(storm::prism::Program const& program, StateInformation& stateInformation, VariableInformation const& variableInformation, storm::storage::BitVector const& currentState, storm::expressions::ExprtkExpressionEvaluator const& evaluator, std::queue<storm::storage::BitVector>& stateQueue) {
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
                        
                        result.push_back(Choice<ValueType>());
                        Choice<ValueType>& choice = result.back();
                        choice.addChoiceLabel(command.getGlobalIndex());
                        
                        // Iterate over all updates of the current command.
                        double probabilitySum = 0;
                        for (uint_fast64_t k = 0; k < command.getNumberOfUpdates(); ++k) {
                            storm::prism::Update const& update = command.getUpdate(k);
                            
                            // Obtain target state index and add it to the list of known states. If it has not yet been
                            // seen, we also add it to the set of states that have yet to be explored.
                            uint32_t stateIndex = getOrAddStateIndex(applyUpdate(variableInformation, currentState, update, evaluator), stateInformation, stateQueue);
                            
                            // Update the choice by adding the probability/target state to it.
                            ValueType probability = evaluator.asDouble(update.getLikelihoodExpression());
							choice.addProbability(stateIndex, probability);
                            probabilitySum += probability;
                        }
                        
                        // Check that the resulting distribution is in fact a distribution.
                        STORM_LOG_THROW(std::abs(1 - probabilitySum) < storm::settings::generalSettings().getPrecision(), storm::exceptions::WrongFormatException, "Probabilities do not sum to one for command '" << command << "' (actually sum to " << probabilitySum << ").");
                    }
                }
                
                return result;
            }
            
            static std::vector<Choice<ValueType>> getLabeledTransitions(storm::prism::Program const& program, StateInformation& stateInformation, VariableInformation const& variableInformation, storm::storage::BitVector const& currentState, storm::expressions::ExprtkExpressionEvaluator const& evaluator, std::queue<storm::storage::BitVector>& stateQueue) {
                std::vector<Choice<ValueType>> result;
                
                for (uint_fast64_t actionIndex : program.getActionIndices()) {
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
                            std::unordered_map<StateType, ValueType>* currentTargetStates = new std::unordered_map<StateType, ValueType>();
                            std::unordered_map<StateType, ValueType>* newTargetStates = new std::unordered_map<StateType, ValueType>();
                            currentTargetStates->emplace(currentState, storm::utility::one<ValueType>());
                            
                            // FIXME: This does not check whether a global variable is written multiple times. While the
                            // behaviour for this is undefined anyway, a warning should be issued in that case.
                            for (uint_fast64_t i = 0; i < iteratorList.size(); ++i) {
                                storm::prism::Command const& command = *iteratorList[i];
                                
                                for (uint_fast64_t j = 0; j < command.getNumberOfUpdates(); ++j) {
                                    storm::prism::Update const& update = command.getUpdate(j);
                                    
                                    for (auto const& stateProbabilityPair : *currentTargetStates) {
                                        // Compute the new state under the current update and add it to the set of new target states.
                                        StateType newTargetState = applyUpdate(variableInformation, stateProbabilityPair.first, currentState, update, evaluator);
                                        newTargetStates->emplace(newTargetState, stateProbabilityPair.second * evaluator.asDouble(update.getLikelihoodExpression()));
                                    }
                                }
                                
                                // If there is one more command to come, shift the target states one time step back.
                                if (i < iteratorList.size() - 1) {
                                    delete currentTargetStates;
                                    currentTargetStates = newTargetStates;
                                    newTargetStates = new std::unordered_map<StateType, ValueType>();
                                }
                            }
                            
                            // At this point, we applied all commands of the current command combination and newTargetStates
                            // contains all target states and their respective probabilities. That means we are now ready to
                            // add the choice to the list of transitions.
                            result.push_back(Choice<ValueType>(actionIndex));
                            
                            // Now create the actual distribution.
                            Choice<ValueType>& choice = result.back();
                            
                            // Add the labels of all commands to this choice.
                            for (uint_fast64_t i = 0; i < iteratorList.size(); ++i) {
                                choice.addChoiceLabel(iteratorList[i]->get().getGlobalIndex());
                            }
                            
                            double probabilitySum = 0;
                            for (auto const& stateProbabilityPair : *newTargetStates) {
                                uint32_t actualIndex = getOrAddStateIndex(stateProbabilityPair.first, stateInformation, stateQueue);
                                choice.addProbability(actualIndex, stateProbabilityPair.second);
                                probabilitySum += stateProbabilityPair.second;
                            }
                            
                            // Check that the resulting distribution is in fact a distribution.
                            STORM_LOG_THROW(std::abs(1 - probabilitySum) <= storm::settings::generalSettings().getPrecision(), storm::exceptions::WrongFormatException, "Sum of update probabilities do not some to one for some command (actually sum to " << probabilitySum << ").");
                            
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
            static std::vector<boost::container::flat_set<uint_fast64_t>> buildMatrices(storm::prism::Program const& program, VariableInformation const& variableInformation, std::vector<storm::prism::TransitionReward> const& transitionRewards, StateInformation& stateInformation, bool deterministicModel, storm::storage::SparseMatrixBuilder<ValueType>& transitionMatrixBuilder, storm::storage::SparseMatrixBuilder<ValueType>& transitionRewardMatrixBuilder) {
                std::vector<boost::container::flat_set<uint_fast64_t>> choiceLabels;
                
                // Initialize a queue and insert the initial state.
                std::queue<storm::storage::BitVector> stateQueue;
                StateType initialState(stateInformation.bitsPerState);
                
                // We need to initialize the values of the variables to their initial value.
                for (auto const& booleanVariable : variableInformation.booleanVariables) {
                    initialState.set(booleanVariable.bitOffset, booleanVariable.initialValue);
                }
                for (auto const& integerVariable : variableInformation.integerVariables) {
                    initialState.setFromInt(integerVariable.bitOffset, integerVariable.bitWidth, static_cast<uint_fast64_t>(integerVariable.initialValue - integerVariable.lowerBound));
                }
        
                // Insert the initial state in the global state to index mapping and state queue.
                uint32_t stateIndex = getOrAddStateIndex(initialState, stateInformation, stateQueue);
                stateInformation.initialStateIndices.push_back(stateIndex);
                
                // Now explore the current state until there is no more reachable state.
                uint_fast64_t currentRow = 0;
                storm::expressions::ExprtkExpressionEvaluator evaluator(program.getManager());
                while (!stateQueue.empty()) {
                    // Get the current state and unpack it.
                    storm::storage::BitVector currentState = stateQueue.front();
                    stateQueue.pop();
                    ValueType stateIndex = stateInformation.stateToIndexMap.getValue(currentState);
                    unpackStateIntoEvaluator(currentState, variableInformation, evaluator);
            
                    // Retrieve all choices for the current state.
                    std::vector<Choice<ValueType>> allUnlabeledChoices = getUnlabeledTransitions(program, stateInformation, variableInformation, currentState, evaluator, stateQueue);
                    std::vector<Choice<ValueType>> allLabeledChoices = getLabeledTransitions(program, stateInformation, variableInformation, currentState, evaluator, stateQueue);
                    
                    uint_fast64_t totalNumberOfChoices = allUnlabeledChoices.size() + allLabeledChoices.size();
                    
                    // If the current state does not have a single choice, we equip it with a self-loop if that was
                    // requested and issue an error otherwise.
                    if (totalNumberOfChoices == 0) {
                        if (!storm::settings::generalSettings().isDontFixDeadlocksSet()) {
                            // Insert empty choice labeling for added self-loop transitions.
                            choiceLabels.push_back(boost::container::flat_set<uint_fast64_t>());
                            transitionMatrixBuilder.addNextValue(currentRow, stateIndex, storm::utility::one<ValueType>());
                            ++currentRow;
                        } else {
                            LOG4CPLUS_ERROR(logger, "Error while creating sparse matrix from probabilistic program: found deadlock state. For fixing these, please provide the appropriate option.");
                            throw storm::exceptions::WrongFormatException() << "Error while creating sparse matrix from probabilistic program: found deadlock state. For fixing these, please provide the appropriate option.";
                        }
                    } else {
                        // Then, based on whether the model is deterministic or not, either add the choices individually
                        // or compose them to one choice.
                        if (deterministicModel) {
                            Choice<ValueType> globalChoice;
                            std::map<uint32_t, ValueType> stateToRewardMap;
                            
                            // Combine all the choices and scale them with the total number of choices of the current state.
                            for (auto const& choice : allUnlabeledChoices) {
                                globalChoice.addChoiceLabels(choice.getChoiceLabels());
                                for (auto const& stateProbabilityPair : choice) {
                                    globalChoice.getOrAddEntry(stateProbabilityPair.first) += stateProbabilityPair.second / totalNumberOfChoices;
                                    
                                    // Now add all rewards that match this choice.
                                    for (auto const& transitionReward : transitionRewards) {
                                        if (!transitionReward.isLabeled() && evaluator.asBool(transitionReward.getStatePredicateExpression())) {
                                            stateToRewardMap[stateProbabilityPair.first] += ValueType(evaluator.asDouble(transitionReward.getRewardValueExpression()));
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
                                        if (transitionReward.getActionIndex() == choice.getActionIndex() && evaluator.asBool(transitionReward.getStatePredicateExpression())) {
                                            stateToRewardMap[stateProbabilityPair.first] += ValueType(evaluator.asDouble(transitionReward.getRewardValueExpression()));
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
                                        if (!transitionReward.isLabeled() && evaluator.asBool(transitionReward.getStatePredicateExpression())) {
                                            stateToRewardMap[stateProbabilityPair.first] += ValueType(evaluator.asDouble(transitionReward.getRewardValueExpression()));
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
                                        if (transitionReward.getActionIndex() == choice.getActionIndex() && evaluator.asBool(transitionReward.getStatePredicateExpression())) {
                                            stateToRewardMap[stateProbabilityPair.first] += ValueType(evaluator.asDouble(transitionReward.getRewardValueExpression()));
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
                
                uint_fast64_t bitOffset = 0;
                VariableInformation variableInformation;
                for (auto const& booleanVariable : program.getGlobalBooleanVariables()) {
                    variableInformation.booleanVariables.emplace_back(booleanVariable.getExpressionVariable(), booleanVariable.getInitialValueExpression().evaluateAsBool(), bitOffset);
                    ++bitOffset;
                    variableInformation.booleanVariableToIndexMap[booleanVariable.getExpressionVariable()] = variableInformation.booleanVariables.size() - 1;
                }
                for (auto const& integerVariable : program.getGlobalIntegerVariables()) {
                    int_fast64_t lowerBound = integerVariable.getLowerBoundExpression().evaluateAsInt();
                    int_fast64_t upperBound = integerVariable.getUpperBoundExpression().evaluateAsInt();
                    uint_fast64_t bitwidth = static_cast<uint_fast64_t>(std::ceil(std::log2(upperBound - lowerBound + 1)));
                    variableInformation.integerVariables.emplace_back(integerVariable.getExpressionVariable(), integerVariable.getInitialValueExpression().evaluateAsInt(), lowerBound, upperBound, bitOffset, bitwidth);
                    bitOffset += bitwidth;
                    variableInformation.integerVariableToIndexMap[integerVariable.getExpressionVariable()] = variableInformation.integerVariables.size() - 1;
                }
                for (auto const& module : program.getModules()) {
                    for (auto const& booleanVariable : module.getBooleanVariables()) {
                        variableInformation.booleanVariables.emplace_back(booleanVariable.getExpressionVariable(), booleanVariable.getInitialValueExpression().evaluateAsBool(), bitOffset);
                        ++bitOffset;
                        variableInformation.booleanVariableToIndexMap[booleanVariable.getExpressionVariable()] = variableInformation.booleanVariables.size() - 1;
                    }
                    for (auto const& integerVariable : module.getIntegerVariables()) {
                        int_fast64_t lowerBound = integerVariable.getLowerBoundExpression().evaluateAsInt();
                        int_fast64_t upperBound = integerVariable.getUpperBoundExpression().evaluateAsInt();
                        uint_fast64_t bitwidth = static_cast<uint_fast64_t>(std::ceil(std::log2(upperBound - lowerBound + 1)));
                        variableInformation.integerVariables.emplace_back(integerVariable.getExpressionVariable(), integerVariable.getInitialValueExpression().evaluateAsInt(), lowerBound, upperBound, bitOffset, bitwidth);
                        bitOffset += bitwidth;
                        variableInformation.integerVariableToIndexMap[integerVariable.getExpressionVariable()] = variableInformation.integerVariables.size() - 1;
                    }
                }
                
                // Create the structure for storing the reachable state space.
                uint64_t bitsPerState = ((bitOffset / 64) + 1) * 64;
                StateInformation stateInformation(bitsPerState);
                
                // Determine whether we have to combine different choices to one or whether this model can have more than
                // one choice per state.
                bool deterministicModel = program.getModelType() == storm::prism::Program::ModelType::DTMC || program.getModelType() == storm::prism::Program::ModelType::CTMC;

                // Build the transition and reward matrices.
                storm::storage::SparseMatrixBuilder<ValueType> transitionMatrixBuilder(0, 0, 0, false, !deterministicModel, 0);
                storm::storage::SparseMatrixBuilder<ValueType> transitionRewardMatrixBuilder(0, 0, 0, false, !deterministicModel, 0);
                modelComponents.choiceLabeling = buildMatrices(program, variableInformation, rewardModel.getTransitionRewards(), stateInformation, deterministicModel, transitionMatrixBuilder, transitionRewardMatrixBuilder);
                
                // Finalize the resulting matrices.
                modelComponents.transitionMatrix = transitionMatrixBuilder.build();
                modelComponents.transitionRewardMatrix = transitionRewardMatrixBuilder.build(modelComponents.transitionMatrix.getRowCount(), modelComponents.transitionMatrix.getColumnCount(), modelComponents.transitionMatrix.getRowGroupCount());
                
                // Now build the state labeling.
                modelComponents.stateLabeling = buildStateLabeling(program, variableInformation, stateInformation);
                
                // Finally, construct the state rewards.
                modelComponents.stateRewards = buildStateRewards(program, variableInformation, rewardModel.getStateRewards(), stateInformation);
                
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
                storm::expressions::ExprtkExpressionEvaluator evaluator(program.getManager());
                
                std::vector<storm::prism::Label> const& labels = program.getLabels();
                
                storm::models::AtomicPropositionsLabeling result(stateInformation.reachableStates.size(), labels.size() + 1);
                
                // Initialize labeling.
                for (auto const& label : labels) {
                    result.addAtomicProposition(label.getName());
                }
                for (uint_fast64_t index = 0; index < stateInformation.reachableStates.size(); index++) {
                    unpackStateIntoEvaluator(stateInformation.reachableStates[index], variableInformation, evaluator);
                    for (auto const& label : labels) {
                        // Add label to state, if the corresponding expression is true.
                        if (evaluator.asBool(label.getStatePredicateExpression())) {
                            result.addAtomicPropositionToState(label.getName(), index);
                        }
                    }
                }
                
                // Also label the initial state with the special label "init".
                result.addAtomicProposition("init");
                for (auto index : stateInformation.initialStateIndices) {
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
            static std::vector<ValueType> buildStateRewards(storm::prism::Program const& program, VariableInformation const& variableInformation, std::vector<storm::prism::StateReward> const& rewards, StateInformation const& stateInformation) {
                storm::expressions::ExprtkExpressionEvaluator evaluator(program.getManager());

                std::vector<ValueType> result(stateInformation.reachableStates.size());
                for (uint_fast64_t index = 0; index < stateInformation.reachableStates.size(); index++) {
                    result[index] = storm::utility::zero<ValueType>();
                    unpackStateIntoEvaluator(stateInformation.reachableStates[index], variableInformation, evaluator);
                    for (auto const& reward : rewards) {
                        
                        // Add this reward to the state if the state is included in the state reward.
                        if (evaluator.asBool(reward.getStatePredicateExpression())) {
                            result[index] += ValueType(evaluator.asDouble(reward.getRewardValueExpression()));
                        }
                    }
                }
                return result;
            }
        };
        
    } // namespace adapters
} // namespace storm

#endif	/* STORM_ADAPTERS_EXPLICITMODELADAPTER_H */
