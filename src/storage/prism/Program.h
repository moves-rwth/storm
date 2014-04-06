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
#include <boost/container/flat_set.hpp>

#include "src/storage/expressions/Expression.h"
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
                    std::set<std::string> const& booleanUndefinedConstantExpressions,
                    std::set<std::string> const& integerUndefinedConstantExpressions,
                    std::set<std::string> const& doubleUndefinedConstantExpressions,
                    std::map<std::string, BooleanVariable> const& globalBooleanVariables,
                    std::map<std::string, IntegerVariable> const& globalIntegerVariables,
                    std::vector<storm::ir::Module> const& modules,
                    std::map<std::string, storm::ir::RewardModel> const& rewards,
                    std::map<std::string, std::unique_ptr<storm::ir::expressions::BaseExpression>> const& labels);
            
            // Provide default implementations for constructors and assignments.
            Program() = default;
            Program(Program const& otherProgram) = default;
            Program& operator=(Program const& otherProgram) = default;
            Program(Program&& otherProgram) = default;
            Program& operator=(Program&& otherProgram) = default;
            
            /*!
             * Retrieves the model type of the model.
             *
             * @return The type of the model.
             */
            ModelType getModelType() const;

            bool hasUndefinedConstants() const;
            
            bool hasUndefinedBooleanConstants() const;
            bool hasUndefinedIntegerConstants() const;
            bool hasUndefinedDoubleConstants() const;
            
            std::set<std::string> const& getUndefinedBooleanConstants() const;
            std::set<std::string> const& getUndefinedIntegerConstants() const;
            std::set<std::string> const& getUndefinedDoubleConstants() const;
            
            std::map<std::string, storm::ir::BooleanVariable> const& getGlobalBooleanVariables() const;
            
            /*!
             * Retrieves a reference to the global boolean variable with the given index.
             *
             * @return A reference to the global boolean variable with the given index.
             */
            storm::ir::BooleanVariable const& getGlobalBooleanVariable(std::string const& variableName) const;
            
            std::map<std::string, storm::ir::IntegerVariable> const& getGlobalIntegerVariables() const;

            /*!
             * Retrieves a reference to the global integer variable with the given index.
             *
             * @return A reference to the global integer variable with the given index.
             */
            storm::ir::IntegerVariable const& getGlobalIntegerVariable(std::string const& variableName) const;

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
             * Retrieves the set of actions present in the program.
             *
             * @return The set of actions present in the program.
             */
            std::set<std::string> const& getActions() const;
            
            /*!
             * Retrieves the indices of all modules within this program that contain commands that are labelled with the given
             * action.
             *
             * @param action The name of the action the modules are supposed to possess.
             * @return A set of indices of all matching modules.
             */
            std::set<uint_fast64_t> const& getModuleIndicesByAction(std::string const& action) const;
            
            /*!
             * Retrieves the index of the module in which the given variable name was declared.
             *
             * @param variableName The name of the variable to search.
             * @return The index of the module in which the given variable name was declared.
             */
            uint_fast64_t getModuleIndexByVariable(std::string const& variableName) const;
            
            std::map<std::string, storm::ir::RewardModel> const& getRewardModels() const;
            
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
            std::map<std::string, Expression> const& getLabels() const;
            
            /*!
             * Creates a new program that drops all commands whose indices are not in the given set.
             *
             * @param indexSet The set of indices for which to keep the commands.
             */
            Program restrictCommands(boost::container::flat_set<uint_fast64_t> const& indexSet);
            
            friend std::ostream& operator<<(std::ostream& stream, Program const& program);
            
        private:
            // The type of the model.
            ModelType modelType;
            
            // A list of undefined boolean constants of the model.
            std::set<std::string> undefinedBooleanConstants;
            
            // A list of undefined integer constants of the model.
            std::set<std::string> undefinedIntegerConstants;

            // A list of undefined double constants of the model.
            std::set<std::string> undefinedDoubleConstants;
            
            // A list of global boolean variables.
            std::map<std::string, BooleanVariable> globalBooleanVariables;
            
            // A list of global integer variables.
            std::std::string, IntegerVariable> globalIntegerVariables;
            
            // The modules associated with the program.
            std::vector<storm::ir::Module> modules;
            
            // The reward models associated with the program.
            std::map<std::string, storm::ir::RewardModel> rewardModels;
            
            // The labels that are defined for this model.
            std::map<std::string, Expression> labels;
            
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
