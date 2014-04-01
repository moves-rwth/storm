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
            LOG_ASSERT((this->getReturnType() == ExpressionReturnType::int_), "Cannot evaluate expression as integer: return type is not an integer.");
            return evaluation.getIntegerValue(this->getVariableName());
        }
        
        bool VariableExpression::evaluateAsBool(Valuation const& evaluation) const {
            LOG_ASSERT((this->getReturnType() == ExpressionReturnType::bool_), "Cannot evaluate expression as integer: return type is not an integer.");
            return evaluation.getBooleanValue(this->getVariableName());
        }
        
        double VariableExpression::evaluateAsDouble(Valuation const& evaluation) const {
            LOG_ASSERT((this->getReturnType() == ExpressionReturnType::double_), "Cannot evaluate expression as integer: return type is not an integer.");
            return evaluation.getDoubleValue(this->getVariableName());
        }
        
        std::unique_ptr<BaseExpression> VariableExpression::operator+(BaseExpression const& other) const {
            // FIXME
            return nullptr;
        }
        
        std::unique_ptr<BaseExpression> operator-(BaseExpression const& other) const;
        std::unique_ptr<BaseExpression> operator-() const;
        std::unique_ptr<BaseExpression> operator*(BaseExpression const& other) const;
        std::unique_ptr<BaseExpression> operator/(BaseExpression const& other) const;
        std::unique_ptr<BaseExpression> operator&(BaseExpression const& other) const;
        std::unique_ptr<BaseExpression> operator|(BaseExpression const& other) const;
        std::unique_ptr<BaseExpression> operator~() const;
        
        std::unique_ptr<BaseExpression> equals(BaseExpression const& other) const;
        std::unique_ptr<BaseExpression> notEquals(BaseExpression const& other) const;
        std::unique_ptr<BaseExpression> greater(BaseExpression const& other) const;
        std::unique_ptr<BaseExpression> greaterOrEqual(BaseExpression const& other) const;
        std::unique_ptr<BaseExpression> less(BaseExpression const& other) const;
        std::unique_ptr<BaseExpression> lessOrEqual(BaseExpression const& other) const;
        std::unique_ptr<BaseExpression> minimum(BaseExpression const& other) const;
        std::unique_ptr<BaseExpression> maximum(BaseExpression const& other) const;
        std::unique_ptr<BaseExpression> mod(BaseExpression const& other) const;
        std::unique_ptr<BaseExpression> floor() const;
        std::unique_ptr<BaseExpression> ceil() const;
        
        void visit(ExpressionVisitor* visitor) const;
    }
}