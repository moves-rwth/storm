/*
 * Module.h
 *
 *  Created on: 04.01.2013
 *      Author: chris
 */

#ifndef MODULE_H_
#define MODULE_H_

#include "BooleanVariable.h"
#include "IntegerVariable.h"
#include "Command.h"

namespace storm {

namespace ir {

class Module {
public:
	Module() : moduleName(""), booleanVariables(), integerVariables(), commands() {

	}

	Module(std::string moduleName, std::vector<storm::ir::BooleanVariable> booleanVariables, std::vector<storm::ir::IntegerVariable> integerVariables, std::vector<storm::ir::Command> commands)
		: moduleName(moduleName), booleanVariables(booleanVariables), integerVariables(integerVariables), commands(commands) {

	}

	Module(std::string moduleName, std::vector<storm::ir::Command> commands) : moduleName(moduleName), booleanVariables(), integerVariables(), commands(commands) {

	}

	std::string toString() {
		std::string result = "module " + moduleName + "\n";
		for (auto variable : booleanVariables) {
			result += "\t" + variable.toString() + "\n";
		}
		for (auto variable : integerVariables) {
			result += "\t" + variable.toString() + "\n";
		}
		for (auto command : commands) {
			result += "\t" + command.toString() + "\n";
		}
		result += "endmodule\n";
		return result;
	}

private:
	std::string moduleName;

	std::vector<storm::ir::BooleanVariable> booleanVariables;
	std::vector<storm::ir::IntegerVariable> integerVariables;
	std::vector<storm::ir::Command> commands;

};

}

}

#endif /* MODULE_H_ */
