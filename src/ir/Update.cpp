/*
 * Update.cpp
 *
 *  Created on: 12.01.2013
 *      Author: Christian Dehnert
 */

#include "Update.h"

#include <sstream>

namespace storm {

namespace ir {

// Initializes all members with their default constructors.
Update::Update() : likelihoodExpression(), assignments() {
	// Nothing to do here.
}

// Initializes all members according to the given values.
Update::Update(std::shared_ptr<storm::ir::expressions::BaseExpression> likelihoodExpression, std::map<std::string, storm::ir::Assignment> assignments)
	: likelihoodExpression(likelihoodExpression), assignments(assignments) {
	// Nothing to do here.
}

// Build a string representation of the update.
std::string Update::toString() const {
	std::stringstream result;
	result << likelihoodExpression->toString() << " : ";
	uint_fast64_t i = 0;
	for (auto assignment : assignments) {
		result << assignment.second.toString();
		++i;
		if (i < assignments.size() - 1) {
			result << " & ";
		}

	}
	return result.str();
}

} // namespace ir

} // namespace storm
