/*
 * IntegerConstantExpression.cpp
 *
 *  Created on: 10.06.2013
 *      Author: Christian Dehnert
 */

#include "IntegerConstantExpression.h"

namespace storm {
    namespace ir {
        namespace expressions {
            
            IntegerConstantExpression::IntegerConstantExpression(std::string const& constantName) : ConstantExpression(int_, constantName) {
                // Nothing to do here.
            }
            
            IntegerConstantExpression::IntegerConstantExpression(IntegerConstantExpression const& integerConstantExpression) : ConstantExpression(integerConstantExpression) {
                // Nothing to do here.
            }
            
            std::unique_ptr<BaseExpression> IntegerConstantExpression::clone() const {
                return std::unique_ptr<BaseExpression>(new IntegerConstantExpression(*this));
            }
            
            std::unique_ptr<BaseExpression> IntegerConstantExpression::clone(std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState) const {
                return std::unique_ptr<BaseExpression>(new IntegerConstantExpression(*this));
            }
            
            double IntegerConstantExpression::getValueAsDouble(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
                return static_cast<double>(getValueAsInt(variableValues));
            }
            
            int_fast64_t IntegerConstantExpression::getValueAsInt(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
                if (!this->isDefined()) {
                    throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
                    << "Integer constant '" << this->getConstantName() << "' is undefined.";
                } else {
                    return this->getValue();
                }
            }
            
            void IntegerConstantExpression::accept(ExpressionVisitor* visitor) {
                visitor->visit(this);
            }
        } // namespace expressions
    } // namespace ir
} // namespace storm