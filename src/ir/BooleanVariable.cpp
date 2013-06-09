/*
 * BooleanVariable.cpp
 *
 *  Created on: 12.01.2013
 *      Author: Christian Dehnert
 */

#include <sstream>

#include "BooleanVariable.h"

namespace storm {

namespace ir {

BooleanVariable::BooleanVariable() : Variable() {
	// Nothing to do here.
}

BooleanVariable::BooleanVariable(uint_fast64_t globalIndex, uint_fast64_t localIndex, std::string const& variableName, std::shared_ptr<storm::ir::expressions::BaseExpression> const& initialValue)
		: Variable(globalIndex, localIndex, variableName,  initialValue) {
	// Nothing to do here.
}

BooleanVariable::BooleanVariable(BooleanVariable const& oldVariable, std::string const& newName, uint_fast64_t newGlobalIndex, std::map<std::string, std::string> const& renaming, std::map<std::string, uint_fast64_t> const& booleanVariableToIndexMap, std::map<std::string,uint_fast64_t> const& integerVariableToIndexMap)
	: Variable(oldVariable, newName, newGlobalIndex, renaming, booleanVariableToIndexMap, integerVariableToIndexMap) {
    // Nothing to do here.
}

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
