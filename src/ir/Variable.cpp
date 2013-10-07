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
        
        Variable::Variable(uint_fast64_t localIndex, uint_fast64_t globalIndex, std::string const& variableName, std::unique_ptr<storm::ir::expressions::BaseExpression>&& initialValue)
        : localIndex(localIndex), globalIndex(globalIndex), variableName(variableName), initialValue(std::move(initialValue)) {
            // Nothing to do here.
        }
        
        Variable::Variable(Variable const& otherVariable) : localIndex(otherVariable.localIndex), globalIndex(otherVariable.globalIndex), variableName(otherVariable.variableName), initialValue() {
            if (otherVariable.initialValue != nullptr) {
                initialValue = otherVariable.initialValue->clone();
            }
        }
        
        Variable::Variable(Variable const& var, std::string const& newName, uint_fast64_t newGlobalIndex, std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState)
        : localIndex(var.getLocalIndex()), globalIndex(newGlobalIndex), variableName(newName), initialValue() {
            if (var.initialValue != nullptr) {
                this->initialValue = var.initialValue->clone(renaming, variableState);
            }
        }
        
        Variable& Variable::operator=(Variable const& otherVariable) {
            if (this != &otherVariable) {
                this->localIndex = otherVariable.localIndex;
                this->globalIndex = otherVariable.globalIndex;
                this->variableName = otherVariable.variableName;
                if (otherVariable.initialValue != nullptr) {
                    this->initialValue = otherVariable.initialValue->clone();
                }
            }
            
            return *this;
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
        
        std::unique_ptr<storm::ir::expressions::BaseExpression> const& Variable::getInitialValue() const {
            return initialValue;
        }
        
        void Variable::setInitialValue(std::unique_ptr<storm::ir::expressions::BaseExpression>&& initialValue) {
            this->initialValue = std::move(initialValue);
        }
        
    } // namespace ir
} // namespace storm
