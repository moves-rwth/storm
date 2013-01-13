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
#include "Command.h"

#include <map>
#include <string>
#include <vector>

namespace storm {

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
	 * Retrieves a reference to the command with the given index.
	 * @returns a reference to the command with the given index.
	 */
	storm::ir::Command const& getCommand(uint_fast64_t index) const;

	/*!
	 * Retrieves a string representation of this variable.
	 * @returns a string representation of this variable.
	 */
	std::string toString() const;

private:
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
};

} // namespace ir

} // namespace storm

#endif /* STORM_IR_MODULE_H_ */
