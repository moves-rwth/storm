/*
 * VariableExpression.cpp
 *
 *  Created on: 10.06.2013
 *      Author: Christian Dehnert
 */

#include "VariableExpression.h"
#include "src/parser/prismparser/VariableState.h"
#include "src/exceptions/ExpressionEvaluationException.h"

namespace storm {
    namespace ir {
        namespace expressions {
            
            VariableExpression::VariableExpression(ReturnType type, std::string const& variableName) : BaseExpression(type), globalIndex(0), variableName(variableName) {
                // Nothing to do here.
            }
            
            VariableExpression::VariableExpression(ReturnType type, uint_fast64_t globalIndex, std::string const& variableName)
            : BaseExpression(type), globalIndex(globalIndex), variableName(variableName) {
                // Nothing to do here.
            }
            
            VariableExpression::VariableExpression(VariableExpression const& variableExpression) : BaseExpression(variableExpression), globalIndex(variableExpression.globalIndex), variableName(variableExpression.variableName) {
                // Nothing to do here.
            }
            
            std::unique_ptr<BaseExpression> VariableExpression::clone() const {
                return std::unique_ptr<BaseExpression>(new VariableExpression(*this));
            }
            
            std::unique_ptr<BaseExpression> VariableExpression::clone(std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState) const {
                // Perform the proper cloning.
                auto renamingPair = renaming.find(this->variableName);
                if (renamingPair != renaming.end()) {
                    if (this->getType() == int_) {
                        return variableState.getIntegerVariableExpression(renamingPair->second)->clone();
                    } else {
                        return variableState.getBooleanVariableExpression(renamingPair->second)->clone();
                    }
                } else {
                    if (this->getType() == int_) {
                        return variableState.getIntegerVariableExpression(this->variableName)->clone();
                    } else {
                        return variableState.getBooleanVariableExpression(this->variableName)->clone();
                    }
                }
            }
            
            BaseExpression* VariableExpression::performSubstitution(std::map<std::string, std::reference_wrapper<BaseExpression>> const& substitution) {
                // If the name of the variable is a key of the map, we need to replace it.
                auto substitutionIterator = substitution.find(variableName);
                
                if (substitutionIterator != substitution.end()) {
                    std::unique_ptr<BaseExpression> expressionClone = substitutionIterator->second.get().clone();
                    BaseExpression* rawPointer = expressionClone.release();
                    return rawPointer;
                } else {
                    // Otherwise, we don't need to replace anything.
                    return this;
                }
            }
            
            void VariableExpression::accept(ExpressionVisitor* visitor) {
                visitor->visit(this);
            }
            
            std::string VariableExpression::toString() const {
                return this->variableName;
            }
            
            int_fast64_t VariableExpression::getValueAsInt(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
                if (this->getType() != int_) {
                    BaseExpression::getValueAsInt(variableValues);
                }
                
                if (variableValues != nullptr) {
                    return variableValues->second[globalIndex];
                } else {
                    throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression involving variables without variable values.";
                }
            }
            
            bool VariableExpression::getValueAsBool(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
                if (this->getType() != bool_) {
                    BaseExpression::getValueAsBool(variableValues);
                }
                
                if (variableValues != nullptr) {
                    return variableValues->first[globalIndex];
                } else {
                    throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression involving variables without variable values.";
                }
            }
            
            double VariableExpression::getValueAsDouble(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
                if (this->getType() != double_ && this->getType() != int_) {
                    BaseExpression::getValueAsDouble(variableValues);
                }
                
                // Because only int variables can deliver a double value, we only need to check them.
                if (variableValues != nullptr) {
                    return static_cast<double>(variableValues->second[globalIndex]);
                } else {
                    throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression with variable '" << variableName << "' of type double.";
                }
            }
            
            std::string const& VariableExpression::getVariableName() const {
                return variableName;
            }
            
            uint_fast64_t VariableExpression::getGlobalVariableIndex() const {
                return this->globalIndex;
            }
            
        } // namespace expressions
    } // namespace ir
} // namespace storm
