/*
 * BinaryBooleanFunctionExpression.cpp
 *
 *  Created on: 10.06.2013
 *      Author: Christian Dehnert
 */

#include <sstream>

#include "BinaryBooleanFunctionExpression.h"

namespace storm {
    namespace ir {
        namespace expressions {
            
            BinaryBooleanFunctionExpression::BinaryBooleanFunctionExpression(std::unique_ptr<BaseExpression>&& left, std::unique_ptr<BaseExpression>&& right, FunctionType functionType)
            : BinaryExpression(bool_, std::move(left), std::move(right)), functionType(functionType) {
                // Nothing to do here.
            }
            
            BinaryBooleanFunctionExpression::BinaryBooleanFunctionExpression(BinaryBooleanFunctionExpression const& binaryBooleanFunctionExpression)
            : BinaryExpression(binaryBooleanFunctionExpression), functionType(binaryBooleanFunctionExpression.functionType) {
                // Nothing to do here.
            }

            std::unique_ptr<BaseExpression> BinaryBooleanFunctionExpression::clone() const {
                return std::unique_ptr<BaseExpression>(new BinaryBooleanFunctionExpression(this->getLeft()->clone(), this->getRight()->clone(), functionType));
            }
            
            std::unique_ptr<BaseExpression> BinaryBooleanFunctionExpression::clone(std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState) const {
                return std::unique_ptr<BaseExpression>(new BinaryBooleanFunctionExpression(this->getLeft()->clone(renaming, variableState), this->getRight()->clone(renaming, variableState), this->functionType));
            }
            
            bool BinaryBooleanFunctionExpression::getValueAsBool(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
                bool resultLeft = this->getLeft()->getValueAsBool(variableValues);
                bool resultRight = this->getRight()->getValueAsBool(variableValues);
                switch(functionType) {
                    case AND: return resultLeft & resultRight; break;
                    case OR: return resultLeft | resultRight; break;
                    default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
                        << "Unknown boolean binary operator: '" << functionType << "'.";
                }
            }
            
            BinaryBooleanFunctionExpression::FunctionType BinaryBooleanFunctionExpression::getFunctionType() const {
                return functionType;
            }
            
            void BinaryBooleanFunctionExpression::accept(ExpressionVisitor* visitor) {
                visitor->visit(this);
            }
            
            std::string BinaryBooleanFunctionExpression::toString() const {
                std::stringstream result;
                result << "(" << this->getLeft()->toString();
                switch (functionType) {
                    case AND: result << " & "; break;
                    case OR: result << " | "; break;
                }
                result << this->getRight()->toString() << ")";
                
                return result.str();
            }
            
        } // namespace expressions
    } // namespace ir
} // namespace storm
