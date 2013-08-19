/*
 * IntegerLiteralExpression.cpp
 *
 *  Created on: 03.01.2013
 *      Author: Christian Dehnert
 */

#include <sstream>

#include "IntegerLiteralExpression.h"

namespace storm {
    namespace ir {
        namespace expressions {
            
            IntegerLiteralExpression::IntegerLiteralExpression(int_fast64_t value) : BaseExpression(int_), value(value) {
                // Nothing to do here.
            }
            
            IntegerLiteralExpression::IntegerLiteralExpression(IntegerLiteralExpression const& integerLiteralExpression)
            : BaseExpression(integerLiteralExpression), value(integerLiteralExpression.value) {
                // Nothing to do here.
            }
            
            std::shared_ptr<BaseExpression> IntegerLiteralExpression::clone(std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState) const {
                return std::shared_ptr<BaseExpression>(new IntegerLiteralExpression(this->value));
            }
            
            double IntegerLiteralExpression::getValueAsDouble(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
                return value;
            }
            
            int_fast64_t IntegerLiteralExpression::getValueAsInt(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
                return value;
            }
            
            void IntegerLiteralExpression::accept(ExpressionVisitor* visitor) {
                visitor->visit(this);
            }
            
            std::string IntegerLiteralExpression::toString() const {
                std::stringstream result;
                result << value;
                return result.str();
            }
            
        } // namespace expressions
    } // namespace ir
} // namespace storm