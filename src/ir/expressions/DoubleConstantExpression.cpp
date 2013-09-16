/*
 * DoubleConstantExpression.cpp
 *
 *  Created on: 10.06.2013
 *      Author: Christian Dehnert
 */

#include "DoubleConstantExpression.h"

namespace storm {
    namespace ir {
        namespace expressions {
            
            DoubleConstantExpression::DoubleConstantExpression(std::string const& constantName) : ConstantExpression<double>(double_, constantName) {
                // Nothing to do here.
            }
            
            DoubleConstantExpression::DoubleConstantExpression(DoubleConstantExpression const& doubleConstantExpression) : ConstantExpression(doubleConstantExpression) {
                // Nothing to do here.
            }
            
            std::shared_ptr<BaseExpression> DoubleConstantExpression::clone(std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState) const {
                return std::shared_ptr<BaseExpression>(new DoubleConstantExpression(*this));
            }
            
            double DoubleConstantExpression::getValueAsDouble(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
                if (!this->isDefined()) {
                    throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
                    << "Double constant '" << this->getConstantName() << "' is undefined.";
                } else {
                    return this->getValue();
                }
            }
            
            void DoubleConstantExpression::accept(ExpressionVisitor* visitor) {
                visitor->visit(this);
            }
            
        } // namespace expressions
    } // namespace ir
} // namespace storm