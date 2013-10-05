/*
 * DoubleLiteralExpression.cpp
 *
 *  Created on: 10.06.2013
 *      Author: Christian Dehnert
 */

#include <sstream>

#include "DoubleLiteralExpression.h"

namespace storm {
    namespace ir {
        namespace expressions {
            
            DoubleLiteralExpression::DoubleLiteralExpression(double value) : BaseExpression(double_), value(value) {
                // Nothing to do here.
            }
            
            DoubleLiteralExpression::DoubleLiteralExpression(DoubleLiteralExpression const& doubleLiteralExpression)
            : BaseExpression(doubleLiteralExpression), value(doubleLiteralExpression.value) {
                // Nothing to do here.
            }
            
            std::shared_ptr<BaseExpression> DoubleLiteralExpression::clone() const {
                return std::shared_ptr<BaseExpression>(new DoubleLiteralExpression(*this));
            }

            std::shared_ptr<BaseExpression> DoubleLiteralExpression::clone(std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState) const {
                return std::shared_ptr<BaseExpression>(new DoubleLiteralExpression(this->value));
            }
            
            double DoubleLiteralExpression::getValueAsDouble(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
                return value;
            }
            
            void DoubleLiteralExpression::accept(ExpressionVisitor* visitor) {
                visitor->visit(this);
            }
            
            std::string DoubleLiteralExpression::toString() const {
                std::stringstream result;
                result << value;
                return result.str();
            }
            
        } // namespace expressions
    } // namespace ir
} // namespace storm