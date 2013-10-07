/*
 * UnaryFunctionExpression.h
 *
 *  Created on: 03.01.2013
 *      Author: Christian Dehnert
 */

#include "UnaryNumericalFunctionExpression.h"

namespace storm {
    namespace ir {
        namespace expressions {
            
            UnaryNumericalFunctionExpression::UnaryNumericalFunctionExpression(ReturnType type, std::unique_ptr<BaseExpression>&& child, FunctionType functionType) : UnaryExpression(type, std::move(child)), functionType(functionType) {
                // Nothing to do here.
            }
            
            UnaryNumericalFunctionExpression::UnaryNumericalFunctionExpression(UnaryNumericalFunctionExpression const& unaryNumericalFunctionExpression) : UnaryExpression(unaryNumericalFunctionExpression), functionType(unaryNumericalFunctionExpression.functionType) {
                // Nothing to do here.
            }
            
            std::unique_ptr<BaseExpression> UnaryNumericalFunctionExpression::clone() const {
                return std::unique_ptr<BaseExpression>(new UnaryNumericalFunctionExpression(this->getType(), this->getChild()->clone(), functionType));
            }
            
            std::unique_ptr<BaseExpression> UnaryNumericalFunctionExpression::clone(std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState) const {
                return std::unique_ptr<BaseExpression>(new UnaryNumericalFunctionExpression(this->getType(), this->getChild()->clone(renaming, variableState), this->functionType));
            }
            
            int_fast64_t UnaryNumericalFunctionExpression::getValueAsInt(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
                if (this->getType() != int_) {
                    BaseExpression::getValueAsInt(variableValues);
                }
                
                int_fast64_t resultChild = this->getChild()->getValueAsInt(variableValues);
                switch(functionType) {
                    case MINUS: return -resultChild; break;
                    default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
                        << "Unknown numerical unary operator: '" << functionType << "'.";
                }
            }
            
            double UnaryNumericalFunctionExpression::getValueAsDouble(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
                if (this->getType() != double_ && this->getType() != int_) {
                    BaseExpression::getValueAsDouble(variableValues);
                }
                
                double resultChild = this->getChild()->getValueAsDouble(variableValues);
                switch(functionType) {
                    case MINUS: return -resultChild; break;
                    default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
                        << "Unknown numerical unary operator: '" << functionType << "'.";
                }
            }
            
            UnaryNumericalFunctionExpression::FunctionType UnaryNumericalFunctionExpression::getFunctionType() const {
                return functionType;
            }
            
            void UnaryNumericalFunctionExpression::accept(ExpressionVisitor* visitor) {
                visitor->visit(this);
            }
            
            std::string UnaryNumericalFunctionExpression::toString() const {
                std::string result = "";
                switch (functionType) {
                    case MINUS: result += "-"; break;
                }
                result += this->getChild()->toString();
                
                return result;
            }
            
        } // namespace expressions
    } // namespace ir
} // namespace storm

