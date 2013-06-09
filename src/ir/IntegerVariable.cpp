/*
 * IntegerVariable.cpp
 *
 *  Created on: 12.01.2013
 *      Author: Christian Dehnert
 */

#include <sstream>
#include <iostream>

#include "IntegerVariable.h"

namespace storm {

namespace ir {

IntegerVariable::IntegerVariable() : lowerBound(), upperBound() {
	// Nothing to do here.
}

IntegerVariable::IntegerVariable(uint_fast64_t globalIndex, uint_fast64_t localIndex, std::string const& variableName, std::shared_ptr<storm::ir::expressions::BaseExpression> lowerBound, std::shared_ptr<storm::ir::expressions::BaseExpression> upperBound, std::shared_ptr<storm::ir::expressions::BaseExpression> initialValue)
	: Variable(globalIndex, localIndex, variableName, initialValue), lowerBound(lowerBound), upperBound(upperBound) {
	if (this->getInitialValue() == nullptr) {
		this->setInitialValue(lowerBound);
	}
}

IntegerVariable::IntegerVariable(IntegerVariable const& oldVariable, std::string const& newName, uint_fast64_t newGlobalIndex, std::map<std::string, std::string> const& renaming, std::map<std::string, uint_fast64_t> const& booleanVariableToIndexMap, std::map<std::string, uint_fast64_t> const& integerVariableToIndexMap)
	: Variable(oldVariable, newName, newGlobalIndex, renaming, booleanVariableToIndexMap, integerVariableToIndexMap), lowerBound(oldVariable.lowerBound->clone(renaming, booleanVariableToIndexMap, integerVariableToIndexMap)), upperBound(oldVariable.upperBound->clone(renaming, booleanVariableToIndexMap, integerVariableToIndexMap)) {
}

std::shared_ptr<storm::ir::expressions::BaseExpression> IntegerVariable::getLowerBound() const {
	return this->lowerBound;
}

std::shared_ptr<storm::ir::expressions::BaseExpression> IntegerVariable::getUpperBound() const {
	return this->upperBound;
}

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
