/*
 * Module.h
 *
 *  Created on: 04.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_MODULE_H_
#define STORM_IR_MODULE_H_

#include "utility/OsDetection.h"

#ifdef LINUX
#include <boost/container/map.hpp>
#endif
#include <map>

#include <set>
#include <string>
#include <vector>
#include <memory>

#include "BooleanVariable.h"
#include "IntegerVariable.h"
#include "Command.h"
#include "expressions/VariableExpression.h"

namespace storm {
    
    namespace parser {
        namespace prism {
            class VariableState;
        } // namespace prismparser
    } // namespace parser
    
    namespace ir {
        
        /*!
         * A class representing a module.
         */
        class Module {
        public:
            /*!
             * Default constructor. Creates an empty module.
             */
            Module();
            
            /*!
             * Creates a module with the given name, variables and commands.
             *
             * @param moduleName The name of the module.
             * @param booleanVariables The boolean variables defined by the module.
             * @param integerVariables The integer variables defined by the module.
             * @param booleanVariableToLocalIndexMap A mapping of boolean variables to local (i.e. module-local) indices.
             * @param integerVariableToLocalIndexMap A mapping of integer variables to local (i.e. module-local) indices.
             * @param commands The commands of the module.
             */
            Module(std::string const& moduleName, std::vector<storm::ir::BooleanVariable> const& booleanVariables,
                   std::vector<storm::ir::IntegerVariable> const& integerVariables,
                   std::map<std::string, uint_fast64_t> const& booleanVariableToLocalIndexMap,
                   std::map<std::string, uint_fast64_t> const& integerVariableToLocalIndexMap,
                   std::vector<storm::ir::Command> const& commands);
            
            /*!
             * Special copy constructor, implementing the module renaming functionality.
             * This will create a new module having all identifiers renamed according to the given map.
             *
             * @param oldModule The module to be copied.
             * @param newModuleName The name of the new module.
             * @param renaming A mapping of identifiers to the new identifiers they are to be replaced with.
             * @param variableState An object knowing about the variables in the system.
             */
            Module(Module const& oldModule, std::string const& newModuleName, std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState& variableState);
            
            /*!
             * Retrieves the number of boolean variables in the module.
             *
             * @return the number of boolean variables in the module.
             */
            uint_fast64_t getNumberOfBooleanVariables() const;
            
            /*!
             * Retrieves a reference to the boolean variable with the given index.
             *
             * @return A reference to the boolean variable with the given index.
             */
            storm::ir::BooleanVariable const& getBooleanVariable(uint_fast64_t index) const;
            
            /*!
             * Retrieves a reference to the boolean variable with the given name.
             *
             * @return A reference to the boolean variable with the given name.
             */
            storm::ir::BooleanVariable const& getBooleanVariable(std::string const& variableName) const;
            
            /*!
             * Retrieves the number of integer variables in the module.
             *
             * @return The number of integer variables in the module.
             */
            uint_fast64_t getNumberOfIntegerVariables() const;
            
            /*!
             * Retrieves a reference to the integer variable with the given index.
             *
             * @return A reference to the integer variable with the given index.
             */
            storm::ir::IntegerVariable const& getIntegerVariable(uint_fast64_t index) const;
            
            /*!
             * Retrieves a reference to the boolean variable with the given name.
             *
             * @return A reference to the boolean variable with the given name.
             */
            storm::ir::IntegerVariable const& getIntegerVariable(std::string const& variableName) const;
            
            /*!
             * Retrieves the number of commands of this module.
             *
             * @return the number of commands of this module.
             */
            uint_fast64_t getNumberOfCommands() const;
            
            /*!
             * Retrieves the index of the boolean variable with the given name.
             *
             * @param variableName The name of the boolean variable whose index to retrieve.
             * @return The index of the boolean variable with the given name.
             */
            uint_fast64_t getBooleanVariableIndex(std::string const& variableName) const;
            
            /*!
             * Retrieves the index of the integer variable with the given name.
             *
             * @param variableName The name of the integer variable whose index to retrieve.
             * @return The index of the integer variable with the given name.
             */
            uint_fast64_t getIntegerVariableIndex(std::string const& variableName) const;
            
            /*!
             * Retrieves a reference to the command with the given index.
             *
             * @return A reference to the command with the given index.
             */
            storm::ir::Command const& getCommand(uint_fast64_t index) const;
            
            /*!
             * Retrieves the name of the module.
             *
             * @return The name of the module.
             */
            std::string const& getName() const;
            
            /*!
             * Retrieves a string representation of this module.
             *
             * @return a string representation of this module.
             */
            std::string toString() const;
            
            /*!
             * Retrieves the set of actions present in this module.
             *
             * @return the set of actions present in this module.
             */
            std::set<std::string> const& getActions() const;
            
            /*!
             * Retrieves whether or not this module contains a command labeled with the given action.
             *
             * @param action The action name to look for in this module.
             * @return True if the module has at least one command labeled with the given action.
             */
            bool hasAction(std::string const& action) const;
            
            /*!
             * Retrieves the indices of all commands within this module that are labelled by the given action.
             *
             * @param action The action with which the commands have to be labelled.
             * @return A set of indices of commands that are labelled with the given action.
             */
            std::set<uint_fast64_t> const& getCommandsByAction(std::string const& action) const;
            
            /*!
             * Deletes all commands with indices not in the given set from the module.
             *
             * @param indexSet The set of indices for which to keep the commands.
             */
            void restrictCommands(std::set<uint_fast64_t> const& indexSet);
        private:
            
            /*!
             * Computes the locally maintained mappings for fast data retrieval.
             */
            void collectActions();
            
            // The name of the module.
            std::string moduleName;
            
            // A list of boolean variables.
            std::vector<storm::ir::BooleanVariable> booleanVariables;
            
            // A list of integer variables.
            std::vector<storm::ir::IntegerVariable> integerVariables;
            
            // A map of boolean variable names to their index.
            std::map<std::string, uint_fast64_t> booleanVariableToLocalIndexMap;
            
            // A map of integer variable names to their index.
            std::map<std::string, uint_fast64_t> integerVariableToLocalIndexMap;
            
            // The commands associated with the module.
            std::vector<storm::ir::Command> commands;
            
            // The set of actions present in this module.
            std::set<std::string> actions;
            
            // A map of actions to the set of commands labeled with this action.
#ifdef LINUX
            boost::container::map<std::string, std::set<uint_fast64_t>> actionsToCommandIndexMap;
#else
            std::map<std::string, std::set<uint_fast64_t>> actionsToCommandIndexMap;
#endif
        };
        
    } // namespace ir
} // namespace storm

#endif /* STORM_IR_MODULE_H_ */
