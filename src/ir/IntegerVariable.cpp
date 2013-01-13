/*
 * IntegerVariable.cpp
 *
 *  Created on: 12.01.2013
 *      Author: Christian Dehnert
 */

#include "IntegerVariable.h"

#include <sstream>

#include <iostream>

namespace storm {

namespace ir {

// Initializes all members with their default constructors.
IntegerVariable::IntegerVariable() : lowerBound(), upperBound() {
	// Nothing to do here.
}

// Initializes all members according to the given values.
IntegerVariable::IntegerVariable(uint_fast64_t index, std::string variableName, std::shared_ptr<storm::ir::expressions::BaseExpression> lowerBound, std::shared_ptr<storm::ir::expressions::BaseExpression> upperBound, std::shared_ptr<storm::ir::expressions::BaseExpression> initialValue) : Variable(index, variableName, initialValue), lowerBound(lowerBound), upperBound(upperBound) {
	if (this->getInitialValue() == nullptr) {
		this->setInitialValue(lowerBound);
	}
}

// Build a string representation of the variable.
std::string IntegerVariable::toString() const {
	std::stringstream result;
	result << this->getName() << ": [" << lowerBound->toString() << ".." << upperBound->toString() << "]";
	if (this->getInitialValue() != nullptr) {
		result << " init " + this->getInitialValue()->toString();
	}
	result << ";";
	return result.str();
}

} // namespace ir

} // namespace storm
