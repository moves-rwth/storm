/*
 * Command.h
 *
 *  Created on: 06.01.2013
 *      Author: chris
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#include "expressions/BaseExpression.h"
#include "Update.h"

namespace storm {

namespace ir {

class Command {
public:

	Command() : commandName(""), guardExpression(nullptr), updates() {

	}

	Command(std::string commandName, std::shared_ptr<storm::ir::expressions::BaseExpression> guardExpression, std::vector<storm::ir::Update> updates)
		: commandName(commandName), guardExpression(guardExpression), updates(updates) {

	}

	std::string toString() {
		std::string result = "[" + commandName + "] " + guardExpression->toString() + " -> ";
		for (uint_fast64_t i = 0; i < updates.size(); ++i) {
			result += updates[i].toString();
			if (i < updates.size() - 1) {
				result += " + ";
			}
		}
		result += ";";
		return result;
	}

private:
	std::string commandName;
	std::shared_ptr<storm::ir::expressions::BaseExpression> guardExpression;
	std::vector<storm::ir::Update> updates;

};

}

}

#endif /* COMMAND_H_ */
