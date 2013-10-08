/*
 * BaseExpression.cpp
 *
 *  Created on: 10.06.2013
 *      Author: Christian Dehnert
 */

#include "BaseExpression.h"

namespace storm {
    namespace ir {
        namespace expressions {
            BaseExpression::BaseExpression() : type(undefined) {
                // Nothing to do here.
            }
            
            BaseExpression::BaseExpression(ReturnType type) : type(type) {
                // Nothing to do here.
            }
            
            BaseExpression::BaseExpression(BaseExpression const& baseExpression) : type(baseExpression.type) {
                // Nothing to do here.
            }
            
            BaseExpression::~BaseExpression() {
                // Nothing to do here.
            }
            
            std::unique_ptr<BaseExpression> BaseExpression::substitute(std::unique_ptr<BaseExpression>&& expression, std::map<std::string, std::reference_wrapper<BaseExpression>> const& substitution) {
                BaseExpression* result = expression->performSubstitution(substitution);
                
                if (result != expression.get()) {
                    return std::unique_ptr<BaseExpression>(result);
                } else {
                    return std::move(expression);
                }
            }
            
            int_fast64_t BaseExpression::getValueAsInt(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
                if (type != int_) {
                    throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression of type '"
                    << this->getTypeName() << "' as 'int'.";
                }
                throw storm::exceptions::NotImplementedException() << "Cannot evaluate expression of type '"
                << this->getTypeName() << " because evaluation implementation is missing.";
            }
            
            bool BaseExpression::getValueAsBool(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
                if (type != bool_) {
                    throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression of type '"
                    << this->getTypeName() << "' as 'bool'.";
                }
                throw storm::exceptions::NotImplementedException() << "Cannot evaluate expression of type '"
                << this->getTypeName() << " because evaluation implementation is missing.";
            }
            
            double BaseExpression::getValueAsDouble(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
                if (type != double_ && type != int_) {
                    throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression of type '"
                    << this->getTypeName() << "' as 'double'.";
                }
                throw storm::exceptions::NotImplementedException() << "Cannot evaluate expression of type '"
                << this->getTypeName() << " because evaluation implementation is missing.";
            }
            
            std::string BaseExpression::getTypeName() const {
                switch(type) {
                    case bool_: return std::string("bool");
                    case int_: return std::string("int");
                    case double_: return std::string("double");
                    default: return std::string("undefined");
                }
            }
            
            BaseExpression::ReturnType BaseExpression::getType() const {
                return type;
            }
            
            BaseExpression* BaseExpression::performSubstitution(std::map<std::string, std::reference_wrapper<BaseExpression>> const& substitution) {
                return this;
            }
            
        } // namespace expressions
    } // namespace ir
} // namespace storm
