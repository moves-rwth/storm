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
            
            virtual std::shared_ptr<BaseExpression> DoubleLiteralExpression::clone(std::map<std::string, std::string> const& renaming, parser::prismparser::VariableState const& variableState) const {
                return std::shared_ptr<BaseExpression>(new DoubleLiteral(this->value));
            }
            
            virtual double DoubleLiteralExpression::getValueAsDouble(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
                return value;
            }
            
            virtual void DoubleLiteralExpression::accept(ExpressionVisitor* visitor) {
                visitor->visit(this);
            }
            
            virtual std::string DoubleLiteralExpression::toString() const {
                std::stringstream result;
                result << value;
                return result.str();
            }
            
        } // namespace expressions
    } // namespace ir
} // namespace storm