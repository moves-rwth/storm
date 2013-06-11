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
#include "src/parser/prismparser/VariableState.h"

namespace storm {
    namespace ir {
        
        Variable::Variable() : localIndex(0), globalIndex(0), variableName(), initialValue() {
            // Nothing to do here.
        }
        
        Variable::Variable(uint_fast64_t localIndex, uint_fast64_t globalIndex, std::string const& variableName, std::shared_ptr<storm::ir::expressions::BaseExpression> const& initialValue)
        : localIndex(localIndex), globalIndex(globalIndex), variableName(variableName), initialValue(initialValue) {
            // Nothing to do here.
        }
        
        Variable::Variable(Variable const& var, std::string const& newName, uint_fast64_t newGlobalIndex, std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState)
        : localIndex(var.getLocalIndex()), globalIndex(newGlobalIndex), variableName(newName) {
            if (var.initialValue != nullptr) {
                this->initialValue = var.initialValue->clone(renaming, variableState);
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
