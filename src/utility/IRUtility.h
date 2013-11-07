/*
 * IRUtility.h
 *
 *  Created on: 05.10.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_UTILITY_IRUTILITY_H_
#define STORM_UTILITY_IRUTILITY_H_

#include <boost/algorithm/string.hpp>

#include "src/storage/LabeledValues.h"
#include "src/ir/IR.h"
#include "src/exceptions/InvalidArgumentException.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

extern log4cplus::Logger logger;

namespace storm {
    namespace utility {
        namespace ir {

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
             * A helper class that provides the functionality to compare states of the model wrt. equality.
             */
            class StateCompare {
            public:
                bool operator()(StateType* state1, StateType* state2) const {
                    return *state1 == *state2;
                }
            };
            
            /*!
             * A helper class that provides the functionality to compare states of the model wrt. the relation "<".
             */
            class StateLess {
            public:
                bool operator()(StateType* state1, StateType* state2) const {
                    // Compare boolean variables.
                    for (uint_fast64_t i = 0; i < state1->first.size(); ++i) {
                        if (!state1->first.at(i) && state2->first.at(i)) {
                            return true;
                        }
                    }
                    // Then compare integer variables.
                    for (uint_fast64_t i = 0; i < state1->second.size(); ++i) {
                        if (!state1->second.at(i) && state2->second.at(i)) {
                            return true;
                        }
                    }
                    return false;
                }
            };
            
            // A structure holding information about a particular choice.
            template<typename ValueType, typename KeyType=uint_fast64_t, typename Compare=std::less<uint_fast64_t>>
            struct Choice {
            public:
                Choice(std::string const& actionLabel) : distribution(), actionLabel(actionLabel), choiceLabels() {
                    // Intentionally left empty.
                }
                
                /*!
                 * Returns an iterator to the first element of this choice.
                 *
                 * @return An iterator to the first element of this choice.
                 */
                typename std::map<KeyType, ValueType>::iterator begin() {
                    return distribution.begin();
                }
                
                /*!
                 * Returns an iterator to the first element of this choice.
                 *
                 * @return An iterator to the first element of this choice.
                 */
                typename std::map<KeyType, ValueType>::const_iterator begin() const {
                    return distribution.cbegin();
                }
                
                /*!
                 * Returns an iterator that points past the elements of this choice.
                 *
                 * @return An iterator that points past the elements of this choice.
                 */
                typename std::map<KeyType, ValueType>::iterator end() {
                    return distribution.end();
                }
                
                /*!
                 * Returns an iterator that points past the elements of this choice.
                 *
                 * @return An iterator that points past the elements of this choice.
                 */
                typename std::map<KeyType, ValueType>::const_iterator end() const {
                    return distribution.cend();
                }
                
                /*!
                 * Returns an iterator to the element with the given key, if there is one. Otherwise, the iterator points to
                 * distribution.end().
                 *
                 * @param value The value to find.
                 * @return An iterator to the element with the given key, if there is one.
                 */
                typename std::map<KeyType, ValueType>::iterator find(uint_fast64_t value) {
                    return distribution.find(value);
                }
                
                /*!
                 * Inserts the contents of this object to the given output stream.
                 *
                 * @param out The stream in which to insert the contents.
                 */
                friend std::ostream& operator<<(std::ostream& out, Choice<ValueType> const& choice) {
                    out << "<";
                    for (auto const& stateProbabilityPair : choice.distribution) {
                        out << stateProbabilityPair.first << " : " << stateProbabilityPair.second << ", ";
                    }
                    out << ">";
                    return out;
                }
                
                /*!
                 * Adds the given label to the labels associated with this choice.
                 *
                 * @param label The label to associate with this choice.
                 */
                void addChoiceLabel(uint_fast64_t label) {
                    choiceLabels.insert(label);
                }
                
                /*!
                 * Adds the given label set to the labels associated with this choice.
                 *
                 * @param labelSet The label set to associate with this choice.
                 */
                void addChoiceLabels(storm::storage::VectorSet<uint_fast64_t> const& labelSet) {
                    for (uint_fast64_t label : labelSet) {
                        addChoiceLabel(label);
                    }
                }
                
                /*!
                 * Retrieves the set of labels associated with this choice.
                 *
                 * @return The set of labels associated with this choice.
                 */
                storm::storage::VectorSet<uint_fast64_t> const& getChoiceLabels() const {
                    return choiceLabels;
                }
                
                /*!
                 * Retrieves the action label of this choice.
                 *
                 * @return The action label of this choice.
                 */
                std::string const& getActionLabel() const {
                    return actionLabel;
                }
                
                /*!
                 * Retrieves the entry in the choice that is associated with the given state and creates one if none exists,
                 * yet.
                 *
                 * @param state The state for which to add the entry.
                 * @return A reference to the entry that is associated with the given state.
                 */
                ValueType& getOrAddEntry(uint_fast64_t state) {
                    auto stateProbabilityPair = distribution.find(state);
                    
                    if (stateProbabilityPair == distribution.end()) {
                        distribution[state] = ValueType();
                    }
                    return distribution.at(state);
                }
                
                /*!
                 * Retrieves the entry in the choice that is associated with the given state and creates one if none exists,
                 * yet.
                 *
                 * @param state The state for which to add the entry.
                 * @return A reference to the entry that is associated with the given state.
                 */
                ValueType const& getOrAddEntry(uint_fast64_t state) const {
                    auto stateProbabilityPair = distribution.find(state);
                    
                    if (stateProbabilityPair == distribution.end()) {
                        distribution[state] = ValueType();
                    }
                    return distribution.at(state);
                }
                
            private:
                // The distribution that is associated with the choice.
                std::map<KeyType, ValueType, Compare> distribution;
                
                // The label of the choice.
                std::string actionLabel;
                
                // The labels that are associated with this choice.
                storm::storage::VectorSet<uint_fast64_t> choiceLabels;
            };
            
            /*!
             * Adds the target state and probability to the given choice and ignores the labels. This function overloads with
             * other functions to ensure the proper treatment of labels.
             *
             * @param choice The choice to which to add the target state and probability.
             * @param state The target state of the probability.
             * @param probability The probability to reach the target state in one step.
             * @param labels A set of labels that is supposed to be associated with this state and probability. NOTE: this
             * is ignored by this particular function but not by the overloaded functions.
             */
            template<typename ValueType>
            void addProbabilityToChoice(Choice<ValueType>& choice, uint_fast64_t state, ValueType probability, storm::storage::VectorSet<uint_fast64_t> const& labels) {
                choice.getOrAddEntry(state) += probability;
            }
            
            /*!
             * Adds the target state and probability to the given choice and labels it accordingly. This function overloads
             * with other functions to ensure the proper treatment of labels.
             *
             * @param choice The choice to which to add the target state and probability.
             * @param state The target state of the probability.
             * @param probability The probability to reach the target state in one step.
             * @param labels A set of labels that is supposed to be associated with this state and probability.
             */
            template<typename ValueType>
            void addProbabilityToChoice(Choice<storm::storage::LabeledValues<ValueType>>& choice, uint_fast64_t state, ValueType probability, storm::storage::VectorSet<uint_fast64_t> const& labels) {
                auto& labeledEntry = choice.getOrAddEntry(state);
                labeledEntry.addValue(probability, labels);
            }
            
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
                
                // List of all lower bounds for integer variables.
                std::vector<int_fast64_t> lowerBounds;
                
                // List of all upper bounds for integer variables.
                std::vector<int_fast64_t> upperBounds;
                
                // A mapping of integer variable names to their indices.
                std::map<std::string, uint_fast64_t> integerVariableToIndexMap;
            };

            /*!
             * Aggregates information about the variables in the program.
             *
             * @param program The program whose variables to aggregate.
             * @return A structure containing information about the variables in the program.
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
                result.lowerBounds.resize(numberOfIntegerVariables);
                result.upperBounds.resize(numberOfIntegerVariables);
                
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
                    result.lowerBounds[var.getGlobalIndex()] = var.getLowerBound()->getValueAsInt(nullptr);
                    result.upperBounds[var.getGlobalIndex()] = var.getUpperBound()->getValueAsInt(nullptr);
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
                        result.lowerBounds[var.getGlobalIndex()] = var.getLowerBound()->getValueAsInt(nullptr);
                        result.upperBounds[var.getGlobalIndex()] = var.getUpperBound()->getValueAsInt(nullptr);
                    }
                }
                
                return result;
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
                for (auto const& nameExpressionPair : program.getBooleanUndefinedConstantExpressionsMap()) {
                    nameExpressionPair.second->undefine();
                }
                for (auto const& nameExpressionPair : program.getIntegerUndefinedConstantExpressionsMap()) {
                    nameExpressionPair.second->undefine();
                }
                for (auto const& nameExpressionPair : program.getDoubleUndefinedConstantExpressionsMap()) {
                    nameExpressionPair.second->undefine();
                }
            }

            /*!
             * Computes the weakest precondition of the given boolean expression wrt. the given updates. The weakest
             * precondition is the most liberal expression that must hold in order to satisfy the given boolean
             * expression after performing the updates. The updates must be disjoint in the sense that they must not
             * assign an expression to a variable twice or more.
             *
             * @param expression The expression for which to build the weakest precondition.
             * @param update The update with respect to which to compute the weakest precondition.
             */
            std::unique_ptr<storm::ir::expressions::BaseExpression> getWeakestPrecondition(std::unique_ptr<storm::ir::expressions::BaseExpression> const& booleanExpression, std::vector<std::reference_wrapper<storm::ir::Update const>> const& updates) {
                std::map<std::string, std::reference_wrapper<storm::ir::expressions::BaseExpression>> variableToExpressionMap;
                
                // Construct the full substitution we need to perform later.
                for (auto const& update : updates) {
                    for (auto const& variableAssignmentPair : update.get().getBooleanAssignments()) {
                        variableToExpressionMap.emplace(variableAssignmentPair.first, *variableAssignmentPair.second.getExpression());
                    }
                    for (auto const& variableAssignmentPair : update.get().getIntegerAssignments()) {
                        variableToExpressionMap.emplace(variableAssignmentPair.first, *variableAssignmentPair.second.getExpression());
                    }
                }
                
                // Copy the given expression and apply the substitution.
                return storm::ir::expressions::BaseExpression::substitute(booleanExpression->clone(), variableToExpressionMap);
            }
            
        } // namespace ir
    } // namespace utility
} // namespace storm

#endif /* STORM_UTILITY_IRUTILITY_H_ */
