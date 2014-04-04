#include "src/storage/expressions/VariableExpression.h"
#include "src/exceptions/ExceptionMacros.h"

namespace storm {
    namespace expressions {
        VariableExpression::VariableExpression(ExpressionReturnType returnType, std::string const& variableName) : BaseExpression(returnType), variableName(variableName) {
            // Intentionally left empty.
        }
        
        std::string const& VariableExpression::getVariableName() const {
            return this->variableName;
        }
        
        int_fast64_t VariableExpression::evaluateAsInt(Valuation const& evaluation) const {
            LOG_ASSERT((this->getReturnType() == ExpressionReturnType::Int), "Cannot evaluate expression as integer: return type is not an integer.");
            return evaluation.getIntegerValue(this->getVariableName());
        }
        
        bool VariableExpression::evaluateAsBool(Valuation const& evaluation) const {
            LOG_ASSERT((this->getReturnType() == ExpressionReturnType::Bool), "Cannot evaluate expression as integer: return type is not a boolean.");
            return evaluation.getBooleanValue(this->getVariableName());
        }
        
        double VariableExpression::evaluateAsDouble(Valuation const& evaluation) const {
            LOG_ASSERT((this->getReturnType() == ExpressionReturnType::Double), "Cannot evaluate expression as integer: return type is not a double.");
            return evaluation.getDoubleValue(this->getVariableName());
        }
        
        std::set<std::string> VariableExpression::getVariables() const {
            return {this->getVariableName()};
        }
        
        std::set<std::string> VariableExpression::getConstants() const {
            return std::set<std::string>();
        }
        
        std::unique_ptr<BaseExpression> VariableExpression::simplify() const {
            return this->clone();
        }
        
        void VariableExpression::accept(ExpressionVisitor* visitor) const {
            visitor->visit(this);
        }
        
        std::unique_ptr<BaseExpression> VariableExpression::clone() const {
            return std::unique_ptr<BaseExpression>(new VariableExpression(*this));
        }
    }
}