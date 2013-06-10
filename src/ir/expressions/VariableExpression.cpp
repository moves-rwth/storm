/*
 * VariableExpression.cpp
 *
 *  Created on: 10.06.2013
 *      Author: Christian Dehnert
 */

#include "VariableExpression.h"
#include "src/parser/prismparser/VariableState.h"

namespace storm {
    namespace ir {
        namespace expressions {
            
            VariableExpression::VariableExpression(ReturnType type, std::string variableName) : BaseExpression(type), localIndex(0), globalIndex(0), variableName(variableName) {
                // Nothing to do here.
            }
            
            VariableExpression::VariableExpression(ReturnType type, uint_fast64_t localIndex, uint_fast64_t globalIndex, std::string variableName)
            : BaseExpression(type), localIndex(localIndex), globalIndex(globalIndex), variableName(variableName) {
                // Nothing to do here.
            }
            
            VariableExpression::VariableExpression(VariableExpression const& oldExpression, std::string const& newName, uint_fast64_t newGlobalIndex)
            : BaseExpression(oldExpression.getType()), localIndex(oldExpression.localIndex), globalIndex(newGlobalIndex), variableName(newName) {
                // Nothing to do here.
            }
            
            virtual VariableExpression::~VariableExpression() {
                // Nothing to do here.
            }
            
            virtual std::shared_ptr<BaseExpression> VariableExpression::clone(std::map<std::string, std::string> const& renaming, VariableState const& variableState) {
                // Perform the proper cloning.
                auto renamingPair = renaming.find(this->variableName);
                if (renamingPair != renaming.end()) {
                    return variableState.getVariableExpression(renamingPair->second);
                } else {
                    return variableState.getVariableExpression(this->variableName);
                }
            }
            
            virtual void VariableExpression::accept(ExpressionVisitor* visitor) {
                std::cout << "Visitor!" << std::endl;
                visitor->visit(this);
            }
            
            virtual std::string VariableExpression::toString() const {
                return this->variableName;
            }
            
            virtual std::string VariableExpression::dump(std::string prefix) const {
                std::stringstream result;
                result << prefix << this->variableName << " " << index << std::endl;
                return result.str();
            }
            
            virtual int_fast64_t VariableExpression::getValueAsInt(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
                if (this->getType() != int_) {
                    BaseExpression::getValueAsInt(variableValues);
                }
                
                if (variableValues != nullptr) {
                    return variableValues->second[globalIndex];
                } else {
                    throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression"
                    << " involving variables without variable values.";
                }
            }
            
            virtual bool VariableExpression::getValueAsBool(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
                if (this->getType() != bool_) {
                    BaseExpression::getValueAsBool(variableValues);
                }
                
                if (variableValues != nullptr) {
                    return variableValues->first[globalIndex];
                } else {
                    throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression"
                    << " involving variables without variable values.";
                }
            }
            
            virtual double VariableExpression::getValueAsDouble(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
                if (this->getType() != double_) {
                    BaseExpression::getValueAsDouble(variableValues);
                }
                
                throw storm::exceptions::NotImplementedException() << "Cannot evaluate expression with "
                << " variable '" << variableName << "' of type double.";
            }
            
            std::string const& VariableExpression::getVariableName() const {
                return variableName;
            }
            
            uint_fast64_t VariableExpression::getLocalVariableIndex() const {
                return this->localIndex;
            }
            
            uint_fast64_t VariableExpression::getGlobalVariableIndex() const {
                return this->globalIndex;
            }
            
        } // namespace expressions
    } // namespace ir
} // namespace storm