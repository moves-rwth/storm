/*
 * Module.h
 *
 *  Created on: 04.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_MODULE_H_
#define STORM_IR_MODULE_H_

#include <map>
#include <set>
#include <string>
#include <vector>
#include <memory>

#include "BooleanVariable.h"
#include "IntegerVariable.h"
#include "expressions/VariableExpression.h"
#include "Command.h"

namespace storm {

namespace ir {

	struct VariableAdder {
        /*!
         * Adds an integer variable with the given name, lower and upper bound.
         *
         * @param name The name of the boolean variable to add.
         */
		uint_fast64_t addBooleanVariable(std::string const& name) = 0;

        /*!
         * Adds an integer variable with the given name, lower and upper bound.
         *
         * @param name The name of the integer variable to add.
         * @param lower The lower bound of the integer variable.
         * @param upper The upper bound of the integer variable.
         */
		uint_fast64_t addIntegerVariable(std::string const& name) = 0;
        
        /*!
         * Retrieves the next free (global) index for a boolean variable.
         */
        uint_fast64_t getNextGlobalBooleanVariableIndex() const = 0;
        
        /*!
         * Retrieves the next free (global) index for a integer variable.
         */
        uint_fast64_t getNextGlobalIntegerVariableIndex() const = 0;
	};

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
     * @param booleanVariableToIndexMap A mapping of boolean variables to global (i.e. program-wide) indices.
     * @param integerVariableToIndexMap A mapping of integer variables to global (i.e. program-wide) indices.
	 * @param commands The commands of the module.
	 */
	Module(std::string const& moduleName, std::vector<storm::ir::BooleanVariable> const& booleanVariables,
			std::vector<storm::ir::IntegerVariable> const& integerVariables,
			std::map<std::string, uint_fast64_t> const& booleanVariableToLocalIndexMap,
			std::map<std::string, uint_fast64_t> const& integerVariableToLocalIndexMap,
			std::vector<storm::ir::Command> const& commands);

	typedef uint_fast64_t (*addIntegerVariablePtr)(std::string const& name, std::shared_ptr<storm::ir::expressions::BaseExpression> const& lower, std::shared_ptr<storm::ir::expressions::BaseExpression> const upper, std::shared_ptr<storm::ir::expressions::BaseExpression> const& init);
	typedef uint_fast64_t (*addBooleanVariablePtr)(std::string const& name, std::shared_ptr<storm::ir::expressions::BaseExpression> const& init);
	
	/*!
	 * Special copy constructor, implementing the module renaming functionality.
	 * This will create a new module having all identifiers renamed according to the given map.
     *
	 * @param oldModule The module to be copied.
	 * @param newModuleName The name of the new module.
	 * @param renaming A mapping of identifiers to the new identifiers they are to be replaced with.
     * @param booleanVariableToIndexMap A mapping from boolean variable names to their global indices.
     * @param integerVariableToIndexMap A mapping from integer variable names to their global indices.
     * @param adder An instance of the VariableAdder interface that keeps track of all the variables in the probabilistic
     * program.
	 */
	Module(Module const& oldModule, std::string const& newModuleName, std::map<std::string, std::string> const& renaming, std::map<std::string, uint_fast64_t> const& booleanVariableToIndexMap, std::map<std::string, uint_fast64_t> const& integerVariableToIndexMap, std::shared_ptr<VariableAdder> const& adder);

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
	 * Retrieves the indices of all commands within this module that are labelled by the given action.
     *
	 * @param action The action with which the commands have to be labelled.
	 * @return A set of indices of commands that are labelled with the given action.
	 */
	std::set<uint_fast64_t> const& getCommandsByAction(std::string const& action) const;

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
	std::map<std::string, std::set<uint_fast64_t>> actionsToCommandIndexMap;
};

} // namespace ir

} // namespace storm

#endif /* STORM_IR_MODULE_H_ */
