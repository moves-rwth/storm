/*
 * Module.h
 *
 *  Created on: 04.01.2013
 *      Author: chris
 */

#ifndef MODULE_H_
#define MODULE_H_

#include "IR.h"

namespace storm {

namespace ir {

class Module {
public:
	Module() {

	}

	std::string toString() {
		std::string result = "module " + moduleName + "\n";
		for (auto variable : booleanVariables) {
			result += "\t" + variable.toString() + "\n";
		}
		result += "\n";
		for (auto variable : integerVariables) {
			result += "\t" + variable.toString() + "\n";
		}
		for (auto command : commands) {
			result += "\t" + command.toString() + "\n";
		}
		result += "endmodule\n";
		return result;
	}

	void addBooleanVariable(storm::ir::BooleanVariable variable) {
		booleanVariables.push_back(variable);
	}

	void addIntegerVariable(storm::ir::IntegerVariable variable) {
		integerVariables.push_back(variable);
	}

	std::string moduleName;

	std::vector<storm::ir::BooleanVariable> booleanVariables;
	std::vector<storm::ir::IntegerVariable> integerVariables;
	std::vector<storm::ir::Command> commands;

};

}

}

BOOST_FUSION_ADAPT_STRUCT(
    storm::ir::Module,
    (std::string, moduleName)
    (std::vector<storm::ir::Command>, commands)
)

#endif /* MODULE_H_ */
