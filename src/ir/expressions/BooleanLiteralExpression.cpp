/*
 * BooleanLiteralExpression.cpp
 *
 *  Created on: 04.01.2013
 *      Author: Christian Dehnert
 */

#include "BooleanLiteralExpression.h"

namespace storm {
    namespace ir {
        namespace expressions {
            
            BooleanLiteralExpression::BooleanLiteralExpression(bool value) : BaseExpression(bool_), value(value) {
                // Nothing to do here.
            }
            
            std::shared_ptr<BaseExpression> BooleanLiteralExpression::clone(std::map<std::string, std::string> const& renaming, parser::prismparser::VariableState const& variableState) const {
                return std::shared_ptr<BaseExpression>(new BooleanLiteralExpression(this->value));
            }
            
            bool BooleanLiteralExpression::getValueAsBool(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
                return value;
            }
            
            void BooleanLiteralExpression::accept(ExpressionVisitor* visitor) {
                visitor->visit(this);
            }
            
            std::string BooleanLiteralExpression::toString() const {
                if (value) {
                    return std::string("true");
                } else {
                    return std::string("false");
                }
            }
            
        } // namespace expressions
    } // namespace ir
} // namespace storm