/*
 * Variable.cpp
 *
 *  Created on: 12.01.2013
 *      Author: Christian Dehnert
 */

#include <sstream>
#include <map>
#include <iostream>

#include "Variable.h"

namespace storm {

namespace ir {

Variable::Variable() : globalIndex(0), localIndex(0), variableName(), initialValue() {
	// Nothing to do here.
}

Variable::Variable(uint_fast64_t globalIndex, uint_fast64_t localIndex, std::string variableName, std::shared_ptr<storm::ir::expressions::BaseExpression> initialValue)
    : globalIndex(globalIndex), localIndex(localIndex), variableName(variableName), initialValue(initialValue) {
	// Nothing to do here.
}

Variable::Variable(Variable const& var, std::string const& newName, uint_fast64_t const newGlobalIndex, std::map<std::string, std::string> const& renaming, std::map<std::string, uint_fast64_t> const& booleanVariableToIndexMap, std::map<std::string, uint_fast64_t> const& integerVariableToIndexMap)
    : globalIndex(newGlobalIndex), localIndex(var.getLocalIndex()), variableName(var.getName()) {
	if (var.initialValue != nullptr) {
		this->initialValue = var.initialValue->clone(renaming, booleanVariableToIndexMap, integerVariableToIndexMap);
	}
}

std::string const& Variable::getName() const {
	return variableName;
}

uint_fast64_t Variable::getGlobalIndex() const {
	return globalIndex;
}
    
uint_fast64_t Variable::getLocalIndex() const {
    return localIndex;
}

std::shared_ptr<storm::ir::expressions::BaseExpression> const& Variable::getInitialValue() const {
	return initialValue;
}

void Variable::setInitialValue(std::shared_ptr<storm::ir::expressions::BaseExpression> const& initialValue) {
	this->initialValue = initialValue;
}

} // namespace ir

} // namespace storm
