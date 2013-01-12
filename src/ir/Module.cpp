/*
 * Module.cpp
 *
 *  Created on: 12.01.2013
 *      Author: Christian Dehnert
 */

#include "Module.h"

#include <sstream>

namespace storm {

namespace ir {

// Initializes all members with their default constructors.
Module::Module() : moduleName(), booleanVariables(), integerVariables(), commands() {
	// Nothing to do here.
}

// Initializes all members according to the given values.
Module::Module(std::string moduleName, std::map<std::string, storm::ir::BooleanVariable> booleanVariables, std::map<std::string, storm::ir::IntegerVariable> integerVariables, std::vector<storm::ir::Command> commands)
	: moduleName(moduleName), booleanVariables(booleanVariables), integerVariables(integerVariables), commands(commands) {
	// Nothing to do here.
}

// Build a string representation of the variable.
std::string Module::toString() const {
	std::stringstream result;
	result << "module " << moduleName << std::endl;
	for (auto variable : booleanVariables) {
		result << "\t" << variable.second.toString() << std::endl;
	}
	for (auto variable : integerVariables) {
		result << "\t" << variable.second.toString() << std::endl;
	}
	for (auto command : commands) {
		result << "\t" << command.toString() << std::endl;
	}
	result << "endmodule" << std::endl;
	return result.str();
}

} // namespace ir

} // namespace storm
