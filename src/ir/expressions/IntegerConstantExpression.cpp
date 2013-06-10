/*
 * IntegerConstantExpression.cpp
 *
 *  Created on: 10.06.2013
 *      Author: Christian Dehnert
 */

#include <sstream>

#include "IntegerConstantExpression.h"

namespace storm {
    namespace ir {
        namespace expressions {
            
            IntegerConstantExpression::IntegerConstantExpression(std::string const& constantName) : ConstantExpression(int_, constantName), value(0), defined(false) {
                // Nothing to do here.
            }
            
            IntegerConstantExpression::IntegerConstantExpression(IntegerConstantExpression const& integerConstantExpression)
            : ConstantExpression(integerConstantExpression), value(integerConstantExpression.value), defined(integerConstantExpression.defined) {
                // Nothing to do here.
            }
            
            std::shared_ptr<BaseExpression> IntegerConstantExpression::clone(std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState) const {
                return std::shared_ptr<BaseExpression>(new IntegerConstantExpression(*this));
            }
            
            int_fast64_t IntegerConstantExpression::getValueAsInt(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
                if (!defined) {
                    throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
                    << "Integer constant '" << this->getConstantName() << "' is undefined.";
                } else {
                    return value;
                }
            }
            
            void IntegerConstantExpression::accept(ExpressionVisitor* visitor) {
                visitor->visit(this);
            }
            
            std::string IntegerConstantExpression::toString() const {
                std::stringstream result;
                result << this->getConstantName();
                if (defined) {
                    result << "[" << value << "]";
                }
                return result.str();
            }
            
            bool IntegerConstantExpression::isDefined() const {
                return defined;
            }
            
            int_fast64_t IntegerConstantExpression::getValue() const {
                return value;
            }
            
            void IntegerConstantExpression::define(int_fast64_t value) {
                defined = true;
                this->value = value;
            }
            
        } // namespace expressions
    } // namespace ir
} // namespace storm