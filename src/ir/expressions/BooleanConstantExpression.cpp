/*
 * BooleanConstantExpression.cpp
 *
 *  Created on: 10.06.2013
 *      Author: Christian Dehnert
 */

#include <sstream>

#include "BooleanConstantExpression.h"

namespace storm {
    namespace ir {
        namespace expressions {
            
            BooleanConstantExpression::BooleanConstantExpression(std::string const& constantName) : ConstantExpression(bool_, constantName), value(false), defined(false) {
                // Nothing to do here.
            }
            
            std::shared_ptr<BaseExpression> BooleanConstantExpression::clone(std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState) const {
                return std::shared_ptr<BaseExpression>(new BooleanConstantExpression(*this));
            }
            
            bool BooleanConstantExpression::getValueAsBool(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
                if (!defined) {
                    throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
                    << "Boolean constant '" << this->getConstantName() << "' is undefined.";
                } else {
                    return value;
                }
            }
            
            void BooleanConstantExpression::accept(ExpressionVisitor* visitor) {
                visitor->visit(this);
            }
            
            std::string BooleanConstantExpression::toString() const {
                std::stringstream result;
                result << this->getConstantName();
                if (defined) {
                    result << "[" << value << "]";
                }
                return result.str();
            }
            
            bool BooleanConstantExpression::isDefined() const {
                return defined;
            }
            
            bool BooleanConstantExpression::getValue() const {
                return value;
            }
            
            void BooleanConstantExpression::define(bool value) {
                defined = true;
                this->value = value;
            }
            
        } // namespace expressions
    } // namespace ir
} // namespace storm