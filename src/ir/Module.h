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
	Module(std::string moduleName, std::map<std::string, storm::ir::BooleanVariable> booleanVariables, std::map<std::string, storm::ir::IntegerVariable> integerVariables, std::vector<storm::ir::Command> commands);

	/*!
	 * Retrieves a string representation of this variable.
	 * @returns a string representation of this variable.
	 */
	std::string toString() const;

private:
	// The name of the module.
	std::string moduleName;

	// A map of boolean variable names to their details.
	std::map<std::string, storm::ir::BooleanVariable> booleanVariables;

	// A map of integer variable names to their details.
	std::map<std::string, storm::ir::IntegerVariable> integerVariables;

	// The commands associated with the module.
	std::vector<storm::ir::Command> commands;
};

} // namespace ir

} // namespace storm

#endif /* STORM_IR_MODULE_H_ */
