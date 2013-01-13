/*
 * BooleanVariable.cpp
 *
 *  Created on: 12.01.2013
 *      Author: Christian Dehnert
 */

#include "BooleanVariable.h"

#include <sstream>

namespace storm {

namespace ir {

// Initializes all members with their default constructors.
BooleanVariable::BooleanVariable() : Variable() {
	// Nothing to do here.
}

// Initializes all members according to the given values.
BooleanVariable::BooleanVariable(uint_fast64_t index, std::string variableName,
		std::shared_ptr<storm::ir::expressions::BaseExpression> initialValue)
		: Variable(index, variableName,  initialValue) {
	// Nothing to do here.
}

// Build a string representation of the variable.
std::string BooleanVariable::toString() const {
	std::stringstream result;
	result << this->getName() << ": bool";
	if (this->getInitialValue() != nullptr) {
		result << " init " << this->getInitialValue()->toString();
	}
	result << ";";
	return result.str();
}

} // namespace ir

} // namespace storm
