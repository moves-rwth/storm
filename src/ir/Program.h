/*
 * Program.h
 *
 *  Created on: 04.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_PROGRAM_H_
#define STORM_IR_PROGRAM_H_

#include <map>
#include <vector>
#include <memory>
#include <set>

#include "expressions/BaseExpression.h"
#include "expressions/BooleanConstantExpression.h"
#include "expressions/IntegerConstantExpression.h"
#include "expressions/DoubleConstantExpression.h"
#include "Module.h"
#include "RewardModel.h"

namespace storm {
    namespace ir {
        
        /*!
         * A class representing a program.
         */
        class Program {
        public:
            
            /*!
             * An enum for the different model types.
             */
            enum ModelType {UNDEFINED, DTMC, CTMC, MDP, CTMDP};
            
            /*!
             * Default constructor. Creates an empty program.
             */
            Program();
            
            /*!
             * Creates a program with the given model type, undefined constants, modules, rewards and labels.
             *
             * @param modelType The type of the model that this program gives rise to.
             * @param booleanUndefinedConstantExpressions A map of undefined boolean constants to their
             * expression nodes.
             * @param integerUndefinedConstantExpressions A map of undefined integer constants to their
             * expression nodes.
             * @param doubleUndefinedConstantExpressions A map of undefined double constants to their
             * expression nodes.
             * @param globalBooleanVariables A list of global boolean variables.
             * @param globalIntegerVariables A list of global integer variables.
             * @param globalBooleanVariableToIndexMap A mapping from global boolean variable names to the index in the
             * list of global boolean variables.
             * @param globalIntegerVariableToIndexMap A mapping from global integer variable names to the index in the
             * list of global integer variables.
             * @param modules The modules of the program.
             * @param rewards The reward models of the program.
             * @param labels The labels defined for this model.
             */
            Program(ModelType modelType,
                    std::map<std::string, std::shared_ptr<storm::ir::expressions::BooleanConstantExpression>> const& booleanUndefinedConstantExpressions,
                    std::map<std::string, std::shared_ptr<storm::ir::expressions::IntegerConstantExpression>> const& integerUndefinedConstantExpressions,
                    std::map<std::string, std::shared_ptr<storm::ir::expressions::DoubleConstantExpression>> const& doubleUndefinedConstantExpressions,
                    std::vector<BooleanVariable> const& globalBooleanVariables,
                    std::vector<IntegerVariable> const& globalIntegerVariables,
                    std::map<std::string, uint_fast64_t> const& globalBooleanVariableToIndexMap,
                    std::map<std::string, uint_fast64_t> const& globalIntegerVariableToIndexMap,
                    std::vector<storm::ir::Module> const& modules,
                    std::map<std::string, storm::ir::RewardModel> const& rewards,
                    std::map<std::string, std::shared_ptr<storm::ir::expressions::BaseExpression>> const& labels);
            
            /*!
             * Retrieves the number of modules in the program.
             *
             * @return The number of modules in the program.
             */
            uint_fast64_t getNumberOfModules() const;
            
            /*!
             * Retrieves a reference to the module with the given index.
             *
             * @param index The index of the module to retrieve.
             * @return The module with the given index.
             */
            storm::ir::Module const& getModule(uint_fast64_t index) const;
            
            /*!
             * Retrieves the model type of the model.
             *
             * @return The type of the model.
             */
            ModelType getModelType() const;
            
            /*!
             * Retrieves a string representation of this program.
             *
             * @return A string representation of this program.
             */
            std::string toString() const;
            
            /*!
             * Retrieves a reference to the global boolean variable with the given index.
             *
             * @return A reference to the global boolean variable with the given index.
             */
            storm::ir::BooleanVariable const& getGlobalBooleanVariable(uint_fast64_t index) const;
            
            /*!
             * Retrieves a reference to the global integer variable with the given index.
             *
             * @return A reference to the global integer variable with the given index.
             */
            storm::ir::IntegerVariable const& getGlobalIntegerVariable(uint_fast64_t index) const;
            
            /*!
             * Retrieves the set of actions present in this module.
             *
             * @return The set of actions present in this module.
             */
            std::set<std::string> const& getActions() const;
            
            /*!
             * Retrieves the indices of all modules within this program that contain commands that are labelled with the given
             * action.
             *
             * @param action The name of the action the modules are supposed to possess.
             * @return A set of indices of all matching modules.
             */
            std::set<uint_fast64_t> const& getModulesByAction(std::string const& action) const;
            
            /*!
             * Retrieves the index of the module in which the given variable name was declared.
             *
             * @param variableName The name of the variable to search.
             * @return The index of the module in which the given variable name was declared.
             */
            uint_fast64_t getModuleIndexForVariable(std::string const& variableName) const;
            
            /*!
             * Retrieves the number of global boolean variables of the program.
             *
             * @return The number of global boolean variables of the program.
             */
            uint_fast64_t getNumberOfGlobalBooleanVariables() const;
            
            /*!
             * Retrieves the number of global integer variables of the program.
             *
             * @return The number of global integer variables of the program.
             */
            uint_fast64_t getNumberOfGlobalIntegerVariables() const;
            
            /*!
             * Retrieves the reward model with the given name.
             *
             * @param name The name of the reward model to return.
             * @return The reward model with the given name.
             */
            storm::ir::RewardModel const& getRewardModel(std::string const& name) const;
            
            /*!
             * Retrieves all labels that are defined by the probabilitic program.
             *
             * @return A set of labels that are defined in the program.
             */
            std::map<std::string, std::shared_ptr<storm::ir::expressions::BaseExpression>> const& getLabels() const;
            
            /*!
             * Retrieves whether the given constant name is an undefined boolean constant of the program.
             *
             * @return True if the given constant name is an undefined boolean constant of the program.
             */
            bool hasUndefinedBooleanConstant(std::string const& constantName) const;
            
            /*!
             * Retrieves the expression associated with the given undefined boolean constant.
             *
             * @param constantName The name of the undefined boolean constant for which to retrieve the expression.
             * @return The expression associated with the given undefined boolean constant.
             */
            std::shared_ptr<storm::ir::expressions::BooleanConstantExpression> getUndefinedBooleanConstantExpression(std::string const& constantName) const;
            
            /*!
             * Retrieves whether the given constant name is an undefined integer constant of the program.
             *
             * @return True if the given constant name is an undefined integer constant of the program.
             */
            bool hasUndefinedIntegerConstant(std::string const& constantName) const;
            
            /*!
             * Retrieves the expression associated with the given undefined integer constant.
             *
             * @param constantName The name of the undefined integer constant for which to retrieve the expression.
             * @return The expression associated with the given undefined integer constant.
             */
            std::shared_ptr<storm::ir::expressions::IntegerConstantExpression> getUndefinedIntegerConstantExpression(std::string const& constantName) const;

            /*!
             * Retrieves whether the given constant name is an undefined double constant of the program.
             *
             * @return True if the given constant name is an undefined double constant of the program.
             */
            bool hasUndefinedDoubleConstant(std::string const& constantName) const;
            
            /*!
             * Retrieves the expression associated with the given undefined double constant.
             *
             * @param constantName The name of the undefined double constant for which to retrieve the expression.
             * @return The expression associated with the given undefined double constant.
             */
            std::shared_ptr<storm::ir::expressions::DoubleConstantExpression> getUndefinedDoubleConstantExpression(std::string const& constantName) const;
            
            /*!
             * Retrieves the mapping of undefined boolean constant names to their expression objects.
             *
             * @return The mapping of undefined boolean constant names to their expression objects.
             */
            std::map<std::string, std::shared_ptr<storm::ir::expressions::BooleanConstantExpression>> const& getBooleanUndefinedConstantExpressionsMap() const;

            /*!
             * Retrieves the mapping of undefined integer constant names to their expression objects.
             *
             * @return The mapping of undefined integer constant names to their expression objects.
             */
            std::map<std::string, std::shared_ptr<storm::ir::expressions::IntegerConstantExpression>> const& getIntegerUndefinedConstantExpressionsMap() const;

            /*!
             * Retrieves the mapping of undefined double constant names to their expression objects.
             *
             * @return The mapping of undefined double constant names to their expression objects.
             */
            std::map<std::string, std::shared_ptr<storm::ir::expressions::DoubleConstantExpression>> const& getDoubleUndefinedConstantExpressionsMap() const;
            
        private:
            // The type of the model.
            ModelType modelType;
            
            // A map of undefined boolean constants to their expression nodes.
            std::map<std::string, std::shared_ptr<storm::ir::expressions::BooleanConstantExpression>> booleanUndefinedConstantExpressions;
            
            // A map of undefined integer constants to their expressions nodes.
            std::map<std::string, std::shared_ptr<storm::ir::expressions::IntegerConstantExpression>> integerUndefinedConstantExpressions;
            
            // A map of undefined double constants to their expressions nodes.
            std::map<std::string, std::shared_ptr<storm::ir::expressions::DoubleConstantExpression>> doubleUndefinedConstantExpressions;
            
            // A list of global boolean variables.
            std::vector<BooleanVariable> globalBooleanVariables;
            
            // A list of global integer variables.
            std::vector<IntegerVariable> globalIntegerVariables;
            
            // A mapping from global boolean variable names to their indices.
            std::map<std::string, uint_fast64_t> globalBooleanVariableToIndexMap;
            
            // A mapping from global integer variable names to their indices.
            std::map<std::string, uint_fast64_t> globalIntegerVariableToIndexMap;
            
            // The modules associated with the program.
            std::vector<storm::ir::Module> modules;
            
            // The reward models associated with the program.
            std::map<std::string, storm::ir::RewardModel> rewards;
            
            // The labels that are defined for this model.
            std::map<std::string, std::shared_ptr<storm::ir::expressions::BaseExpression>> labels;
            
            // The set of actions present in this program.
            std::set<std::string> actions;
            
            // A map of actions to the set of modules containing commands labelled with this action.
            std::map<std::string, std::set<uint_fast64_t>> actionsToModuleIndexMap;
            
            // A mapping from variable names to the modules in which they were declared.
            std::map<std::string, uint_fast64_t> variableToModuleIndexMap;
        };
        
    } // namespace ir
} // namespace storm

#endif /* STORM_IR_PROGRAM_H_ */
