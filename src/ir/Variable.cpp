/*
 * Variable.cpp
 *
 *  Created on: 12.01.2013
 *      Author: Christian Dehnert
 */

#include "Variable.h"

#include <sstream>
#include <map>
#include <iostream>

namespace storm {

namespace ir {

// Initializes all members with their default constructors.
Variable::Variable() : index(0), variableName(), initialValue() {
	// Nothing to do here.
}

// Initializes all members according to the given values.
Variable::Variable(uint_fast64_t index, std::string variableName, std::shared_ptr<storm::ir::expressions::BaseExpression> initialValue) : index(index), variableName(variableName), initialValue(initialValue) {
	// Nothing to do here.
}

Variable::Variable(const Variable& var, const std::string& newName, const std::map<std::string, std::string>& renaming, const std::map<std::string,uint_fast64_t>& bools, const std::map<std::string,uint_fast64_t>& ints)
	: Variable(var.index, newName, var.initialValue->clone(renaming, bools, ints)) {
	std::cout << "Cloning Variable " << var.variableName << " to " << newName << std::endl;
}

// Return the name of the variable.
std::string const& Variable::getName() const {
	return variableName;
}

uint_fast64_t Variable::getIndex() const {
	return index;
}

// Return the expression for the initial value of the variable.
std::shared_ptr<storm::ir::expressions::BaseExpression> const& Variable::getInitialValue() const {
	return initialValue;
}

// Set the initial value expression to the one provided.
void Variable::setInitialValue(std::shared_ptr<storm::ir::expressions::BaseExpression> const& initialValue) {
	this->initialValue = initialValue;
}


} // namespace ir

} // namespace storm
