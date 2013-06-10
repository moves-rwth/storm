/*
 * DoubleConstantExpression.cpp
 *
 *  Created on: 10.06.2013
 *      Author: Christian Dehnert
 */

#include <sstream>

#include "DoubleConstantExpression.h"

namespace storm {
    namespace ir {
        namespace expressions {
            
            DoubleConstantExpression::DoubleConstantExpression(std::string const& constantName) : ConstantExpression(double_, constantName), value(0), defined(false) {
                // Nothing to do here.
            }
            
            DoubleConstantExpression::DoubleConstantExpression(DoubleConstantExpression const& doubleConstantExpression)
            : ConstantExpression(doubleConstantExpression), value(doubleConstantExpression.value), defined(doubleConstantExpression.defined) {
                // Nothing to do here.
            }
            
            std::shared_ptr<BaseExpression> DoubleConstantExpression::clone(std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState) const {
                return std::shared_ptr<BaseExpression>(new DoubleConstantExpression(*this));
            }
            
            double DoubleConstantExpression::getValueAsDouble(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
                if (!defined) {
                    throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
                    << "Double constant '" << this->getConstantName() << "' is undefined.";
                } else {
                    return value;
                }
            }
            
            void DoubleConstantExpression::accept(ExpressionVisitor* visitor) {
                visitor->visit(this);
            }
            
            std::string DoubleConstantExpression::toString() const {
                std::stringstream result;
                result << this->getConstantName();
                if (defined) {
                    result << "[" << value << "]";
                }
                return result.str();
            }
            
            bool DoubleConstantExpression::isDefined() const {
                return defined;
            }
            
            double DoubleConstantExpression::getValue() const {
                return value;
            }
            
            void DoubleConstantExpression::define(double value) {
                defined = true;
                this->value = value;
            }
            
        } // namespace expressions
    } // namespace ir
} // namespace storm