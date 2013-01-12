/*
 * Variable.cpp
 *
 *  Created on: 12.01.2013
 *      Author: Christian Dehnert
 */

#include "Variable.h"

#include <sstream>

namespace storm {

namespace ir {

// Initializes all members with their default constructors.
Variable::Variable() : variableName(), initialValue() {
	// Nothing to do here.
}

// Initializes all members according to the given values.
Variable::Variable(std::string variableName, std::shared_ptr<storm::ir::expressions::BaseExpression> initialValue) : variableName(variableName), initialValue(initialValue) {
	// Nothing to do here.
}

// Return the name of the variable.
std::string const& Variable::getVariableName() const {
	return variableName;
}

// Return the expression for the initial value of the variable.
std::shared_ptr<storm::ir::expressions::BaseExpression> const& Variable::getInitialValue() const {
	return initialValue;
}

} // namespace ir

} // namespace storm
