/*
 * Command.cpp
 *
 *  Created on: 12.01.2013
 *      Author: Christian Dehnert
 */

#include "Command.h"

#include <sstream>

namespace storm {

namespace ir {

// Initializes all members with their default constructors.
Command::Command() : commandName(), guardExpression(), updates() {
	// Nothing to do here.
}

// Initializes all members according to the given values.
Command::Command(std::string commandName, std::shared_ptr<storm::ir::expressions::BaseExpression> guardExpression, std::vector<storm::ir::Update> updates)
		: commandName(commandName), guardExpression(guardExpression), updates(updates) {
	// Nothing to do here.
}

// Build a string representation of the command.
std::string Command::toString() const {
	std::stringstream result;
	result << "[" << commandName << "] " << guardExpression->toString() << " -> ";
	for (uint_fast64_t i = 0; i < updates.size(); ++i) {
		result << updates[i].toString();
		if (i < updates.size() - 1) {
			result << " + ";
		}
	}
	result << ";";
	return result.str();
}

} // namespace ir

} // namespace storm
