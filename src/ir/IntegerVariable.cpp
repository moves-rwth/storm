/*
 * IntegerVariable.cpp
 *
 *  Created on: 12.01.2013
 *      Author: Christian Dehnert
 */

#include "IntegerVariable.h"

#include <sstream>

namespace storm {

namespace ir {

// Initializes all members with their default constructors.
IntegerVariable::IntegerVariable() : lowerBound(), upperBound() {
	// Nothing to do here.
}

// Initializes all members according to the given values.
IntegerVariable::IntegerVariable(std::string variableName, std::shared_ptr<storm::ir::expressions::BaseExpression> lowerBound, std::shared_ptr<storm::ir::expressions::BaseExpression> upperBound, std::shared_ptr<storm::ir::expressions::BaseExpression> initialValue) : Variable(variableName, initialValue), lowerBound(lowerBound), upperBound(upperBound) {
	// Nothing to do here.
}

// Build a string representation of the variable.
std::string IntegerVariable::toString() const {
	std::stringstream result;
	result << this->getVariableName() << ": [" << lowerBound->toString() << ".." << upperBound->toString() << "]";
	if (this->getInitialValue() != nullptr) {
		result << " init " + this->getInitialValue()->toString();
	}
	result << ";";
	return result.str();
}

} // namespace ir

} // namespace storm
