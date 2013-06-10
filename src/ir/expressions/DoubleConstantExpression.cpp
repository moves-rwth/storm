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
            
            DoubleConstantExpression::DoubleConstantExpression(std::string const& constantName) : ConstantExpression(double_, constantName), defined(false), value(0) {
                // Nothing to do here.
            }
            
            std::shared_ptr<BaseExpression> DoubleConstantExpression::clone(std::map<std::string, std::string> const& renaming, parser::prismparser::VariableState const& variableState) const {
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
            
            virtual void DoubleConstantExpression::accept(ExpressionVisitor* visitor) {
                visitor->visit(this);
            }
            
            virtual std::string DoubleConstantExpression::toString() const {
                std::stringstream result;
                result << this->constantName;
                if (defined) {
                    result << "[" << value << "]";
                }
                return result.str();
            }
            
            bool DoubleConstantExpression::isDefined() {
                return defined;
            }
            
            double DoubleConstantExpression::getValue() {
                return value;
            }
            
            void DoubleConstantExpression::define(double value) {
                defined = true;
                this->value = value;
            }
            
        } // namespace expressions
    } // namespace ir
} // namespace storm