/*
 * BooleanVariable.cpp
 *
 *  Created on: 12.01.2013
 *      Author: Christian Dehnert
 */

#include <sstream>

#include "BooleanVariable.h"
#include "src/parser/prismparser/VariableState.h"

namespace storm {
    namespace ir {
        
        BooleanVariable::BooleanVariable() : Variable() {
            // Nothing to do here.
        }
        
        BooleanVariable::BooleanVariable(uint_fast64_t localIndex, uint_fast64_t globalIndex, std::string const& variableName, std::shared_ptr<storm::ir::expressions::BaseExpression> const& initialValue)
		: Variable(localIndex, globalIndex, variableName,  initialValue) {
            // Nothing to do here.
        }
        
        BooleanVariable::BooleanVariable(BooleanVariable const& oldVariable, std::string const& newName, uint_fast64_t newGlobalIndex, std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState)
        : Variable(oldVariable, newName, newGlobalIndex, renaming, variableState) {
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
