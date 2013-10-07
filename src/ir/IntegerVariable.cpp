/*
 * IntegerVariable.cpp
 *
 *  Created on: 12.01.2013
 *      Author: Christian Dehnert
 */

#include <sstream>
#include <iostream>

#include "IntegerVariable.h"
#include "src/parser/prismparser/VariableState.h"

namespace storm {
    namespace ir {
        
        IntegerVariable::IntegerVariable() : lowerBound(), upperBound() {
            // Nothing to do here.
        }
        
        IntegerVariable::IntegerVariable(uint_fast64_t localIndex, uint_fast64_t globalIndex, std::string const& variableName, std::unique_ptr<storm::ir::expressions::BaseExpression>&& lowerBound, std::unique_ptr<storm::ir::expressions::BaseExpression>&& upperBound, std::unique_ptr<storm::ir::expressions::BaseExpression>&& initialValue)
        : Variable(localIndex, globalIndex, variableName, std::move(initialValue)), lowerBound(std::move(lowerBound)), upperBound(std::move(upperBound)) {
            // Nothing to do here.
        }
        
        IntegerVariable::IntegerVariable(IntegerVariable const& oldVariable, std::string const& newName, uint_fast64_t newGlobalIndex, std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState)
        : Variable(oldVariable, newName, newGlobalIndex, renaming, variableState), lowerBound(oldVariable.lowerBound->clone(renaming, variableState)), upperBound(oldVariable.upperBound->clone(renaming, variableState)) {
            // Nothing to do here.
        }
        
        IntegerVariable::IntegerVariable(IntegerVariable const& otherVariable) : Variable(otherVariable.getLocalIndex(), otherVariable.getGlobalIndex(), otherVariable.getName(), nullptr), lowerBound(), upperBound() {
            if (otherVariable.getInitialValue() != nullptr) {
                setInitialValue(otherVariable.getInitialValue()->clone());
            }
            if (otherVariable.lowerBound != nullptr) {
                lowerBound = otherVariable.lowerBound->clone();
            }
            if (otherVariable.upperBound != nullptr) {
                upperBound = otherVariable.upperBound->clone();
            }
        }
        
        IntegerVariable& IntegerVariable::operator=(IntegerVariable const& otherVariable) {
            if (this != &otherVariable) {
                Variable::operator=(otherVariable);
                this->lowerBound = otherVariable.lowerBound->clone();
                this->upperBound = otherVariable.upperBound->clone();
            }
            
            return *this;
        }
        
        std::unique_ptr<storm::ir::expressions::BaseExpression> const& IntegerVariable::getLowerBound() const {
            return this->lowerBound;
        }
        
        std::unique_ptr<storm::ir::expressions::BaseExpression> const& IntegerVariable::getUpperBound() const {
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
