/*
 * Module.h
 *
 *  Created on: 04.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_MODULE_H_
#define STORM_IR_MODULE_H_

#include "BooleanVariable.h"
#include "IntegerVariable.h"
#include "expressions/VariableExpression.h"
#include "Command.h"

#include <map>
#include <set>
#include <string>
#include <vector>
#include <memory>

namespace storm {

namespace ir {

	struct VariableAdder {
		virtual uint_fast64_t addIntegerVariable(const std::string& name, const std::shared_ptr<storm::ir::expressions::BaseExpression> lower, const std::shared_ptr<storm::ir::expressions::BaseExpression> upper, const std::shared_ptr<storm::ir::expressions::BaseExpression> init) = 0;
		virtual uint_fast64_t addBooleanVariable(const std::string& name, const std::shared_ptr<storm::ir::expressions::BaseExpression> init) = 0;
		virtual std::shared_ptr<expressions::VariableExpression> getVariable(const std::string& name) = 0;
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
	 * @param moduleName the name of the module.
	 * @param booleanVariables a map of boolean variables.
	 * @param integerVariables a map of integer variables.
	 * @param commands the vector of commands.
	 */
	Module(std::string moduleName, std::vector<storm::ir::BooleanVariable> booleanVariables,
			std::vector<storm::ir::IntegerVariable> integerVariables,
			std::map<std::string, uint_fast64_t> booleanVariableToIndexMap,
			std::map<std::string, uint_fast64_t> integerVariableToIndexMap,
			std::vector<storm::ir::Command> commands);

	typedef uint_fast64_t (*addIntegerVariablePtr)(const std::string& name, const std::shared_ptr<storm::ir::expressions::BaseExpression> lower, const std::shared_ptr<storm::ir::expressions::BaseExpression> upper, const std::shared_ptr<storm::ir::expressions::BaseExpression> init);
	typedef uint_fast64_t (*addBooleanVariablePtr)(const std::string& name, const std::shared_ptr<storm::ir::expressions::BaseExpression> init);
	
	/*!
	 * Special copy constructor, implementing the module renaming functionality.
	 * This will create a new module having all identifier renamed according to the given map.
	 * @param module Module to be copied.
	 * @param moduleName Name of the new module.
	 * @param renaming Renaming map.
	 */
	Module(const Module& module, const std::string& moduleName, const std::map<std::string, std::string>& renaming, std::shared_ptr<VariableAdder> adder);

	/*!
	 * Retrieves the number of boolean variables in the module.
	 * @returns the number of boolean variables in the module.
	 */
	uint_fast64_t getNumberOfBooleanVariables() const;

	/*!
	 * Retrieves a reference to the boolean variable with the given index.
	 * @returns a reference to the boolean variable with the given index.
	 */
	storm::ir::BooleanVariable const& getBooleanVariable(uint_fast64_t index) const;

	/*!
	 * Retrieves the number of integer variables in the module.
	 * @returns the number of integer variables in the module.
	 */
	uint_fast64_t getNumberOfIntegerVariables() const;

	/*!
	 * Retrieves a reference to the integer variable with the given index.
	 * @returns a reference to the integer variable with the given index.
	 */
	storm::ir::IntegerVariable const& getIntegerVariable(uint_fast64_t index) const;

	/*!
	 * Retrieves the number of commands of this module.
	 * @returns the number of commands of this module.
	 */
	uint_fast64_t getNumberOfCommands() const;

	/*!
	 * Retrieves the index of the boolean variable with the given name.
	 * @param variableName the name of the variable whose index to retrieve.
	 * @returns the index of the boolean variable with the given name.
	 */
	uint_fast64_t getBooleanVariableIndex(std::string variableName) const;

	/*!
	 * Retrieves the index of the integer variable with the given name.
	 * @param variableName the name of the variable whose index to retrieve.
	 * @returns the index of the integer variable with the given name.
	 */
	uint_fast64_t getIntegerVariableIndex(std::string variableName) const;

	/*!
	 * Retrieves a reference to the command with the given index.
	 * @returns a reference to the command with the given index.
	 */
	storm::ir::Command const getCommand(uint_fast64_t index) const;

	/*!
	 * Retrieves a string representation of this variable.
	 * @returns a string representation of this variable.
	 */
	std::string toString() const;
	
	/*!
	 * Retrieves the set of actions present in this module.
	 * @returns the set of actions present in this module.
	 */
	std::set<std::string> const& getActions() const;
	
	/*!
	 * Retrieves the indices of all commands within this module that are labelled
	 * by the given action.
	 * @param action Name of the action.
	 * @returns Indices of all matching commands.
	 */
	std::shared_ptr<std::set<uint_fast64_t>> const getCommandsByAction(std::string const& action) const;

private:

	void collectActions();

	// The name of the module.
	std::string moduleName;

	// A list of boolean variables.
	std::vector<storm::ir::BooleanVariable> booleanVariables;

	// A list of integer variables.
	std::vector<storm::ir::IntegerVariable> integerVariables;

	// A map of boolean variable names to their index.
	std::map<std::string, uint_fast64_t> booleanVariablesToIndexMap;

	// A map of integer variable names to their details.
	std::map<std::string, uint_fast64_t> integerVariablesToIndexMap;

	// The commands associated with the module.
	std::vector<storm::ir::Command> commands;
	
	// The set of actions present in this module.
	std::set<std::string> actions;
	
	// A map of actions to the set of commands labeled with this action.
	std::map<std::string, std::shared_ptr<std::set<uint_fast64_t>>> actionsToCommandIndexMap;
};

} // namespace ir

} // namespace storm

#endif /* STORM_IR_MODULE_H_ */
