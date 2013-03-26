/*
 * Assignment.cpp
 *
 *  Created on: 12.01.2013
 *      Author: Christian Dehnert
 */

#include "Assignment.h"

#include <sstream>

namespace storm {

namespace ir {

// Initializes all members with their default constructors.
Assignment::Assignment() : variableName(), expression() {
	// Nothing to do here.
}

// Initializes all members according to the given values.
Assignment::Assignment(std::string variableName, std::shared_ptr<storm::ir::expressions::BaseExpression> expression)
	: variableName(variableName), expression(expression) {
	// Nothing to do here.
}

Assignment::Assignment(const Assignment& assignment, const std::map<std::string, std::string>& renaming, const std::map<std::string,uint_fast64_t>& bools, const std::map<std::string,uint_fast64_t>& ints)
	: variableName(assignment.variableName), expression(assignment.expression->clone(renaming, bools, ints)) {
	if (renaming.count(assignment.variableName) > 0) {
		this->variableName = renaming.at(assignment.variableName);
	}
}

// Returns the name of the variable associated with this assignment.
std::string const& Assignment::getVariableName() const {
	return variableName;
}

// Returns the expression associated with this assignment.
std::shared_ptr<storm::ir::expressions::BaseExpression> const& Assignment::getExpression() const {
	return expression;
}

// Build a string representation of the assignment.
std::string Assignment::toString() const {
	std::stringstream result;
	result << "(" << variableName << "' = " << expression->toString() << ")";
	return result.str();
}

} // namespace ir

} // namespace storm
