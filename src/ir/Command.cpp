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
Command::Command() : actionName(), guardExpression(), updates() {
	// Nothing to do here.
}

// Initializes all members according to the given values.
Command::Command(std::string actionName, std::shared_ptr<storm::ir::expressions::BaseExpression> guardExpression, std::vector<storm::ir::Update> updates)
		: actionName(actionName), guardExpression(guardExpression), updates(updates) {
	// Nothing to do here.
}

// Return the action name.
std::string const& Command::getActionName() const {
	return this->actionName;
}

// Return the expression for the guard.
std::shared_ptr<storm::ir::expressions::BaseExpression> const& Command::getGuard() const {
	return guardExpression;
}

// Return the number of updates.
uint_fast64_t Command::getNumberOfUpdates() const {
	return this->updates.size();
}

// Return the requested update.
storm::ir::Update const& Command::getUpdate(uint_fast64_t index) const {
	return this->updates[index];
}

// Build a string representation of the command.
std::string Command::toString() const {
	std::stringstream result;
	result << "[" << actionName << "] " << guardExpression->toString() << " -> ";
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
