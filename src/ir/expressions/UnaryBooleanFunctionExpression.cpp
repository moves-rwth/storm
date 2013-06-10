/*
 * UnaryBooleanFunctionExpression.cpp
 *
 *  Created on: 10.06.2013
 *      Author: Christian Dehnert
 */

#include <sstream>

#include "UnaryBooleanFunctionExpression.h"

namespace storm {
    namespace ir {
        namespace expressions {
            
            UnaryBooleanFunctionExpression::UnaryBooleanFunctionExpression(std::shared_ptr<BaseExpression> child, FunctionType functionType) : UnaryExpression(bool_, child), functionType(functionType) {
                // Nothing to do here.
            }
            
            UnaryBooleanFunctionExpression::UnaryBooleanFunctionExpression(UnaryBooleanFunctionExpression const& unaryBooleanFunctionExpression) : UnaryExpression(unaryBooleanFunctionExpression), functionType(unaryBooleanFunctionExpression.functionType) {
                // Nothing to do here.
            }
            
            std::shared_ptr<BaseExpression> UnaryBooleanFunctionExpression::clone(std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState) const {
                return std::shared_ptr<BaseExpression>(new UnaryBooleanFunctionExpression(this->getChild()->clone(renaming, variableState), this->functionType));
            }
            
            UnaryBooleanFunctionExpression::FunctionType UnaryBooleanFunctionExpression::getFunctionType() const {
                return functionType;
            }
            
            bool UnaryBooleanFunctionExpression::getValueAsBool(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
                bool resultChild = this->getChild()->getValueAsBool(variableValues);
                switch(functionType) {
                    case NOT: return !resultChild; break;
                    default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
                        << "Unknown boolean unary operator: '" << functionType << "'.";
                }
            }
            
            void UnaryBooleanFunctionExpression::accept(ExpressionVisitor* visitor) {
                visitor->visit(this);
            }
            
            std::string UnaryBooleanFunctionExpression::toString() const {
                std::stringstream result;
                switch (functionType) {
                    case NOT: result << "!"; break;
                }
                result << "(" << this->getChild()->toString() << ")";
                
                return result.str();
            }

        } // namespace expressions
    } // namespace ir
} // namespace storm
