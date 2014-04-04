#include <cmath>

#include "src/storage/expressions/UnaryNumericalFunctionExpression.h"

namespace storm {
    namespace expressions {
        UnaryNumericalFunctionExpression::UnaryNumericalFunctionExpression(ExpressionReturnType returnType, std::unique_ptr<BaseExpression>&& operand, OperatorType operatorType) : UnaryExpression(returnType, std::move(operand)), operatorType(operatorType) {
            // Intentionally left empty.
        }
        
        int_fast64_t UnaryNumericalFunctionExpression::evaluateAsInt(Valuation const& valuation) const {
            int_fast64_t operandEvaluated = this->getOperand()->evaluateAsInt(valuation);
            switch (this->getOperatorType()) {
                case OperatorType::Minus: return -operandEvaluated; break;
                case OperatorType::Floor: return std::floor(operandEvaluated); break;
                case OperatorType::Ceil: return std::ceil(operandEvaluated); break;
            }
        }
        
        double UnaryNumericalFunctionExpression::evaluateAsDouble(Valuation const& valuation) const {
            double operandEvaluated = this->getOperand()->evaluateAsDouble(valuation);
            switch (this->getOperatorType()) {
                case OperatorType::Minus: return -operandEvaluated; break;
                case OperatorType::Floor: return std::floor(operandEvaluated); break;
                case OperatorType::Ceil: return std::ceil(operandEvaluated); break;
            }
        }
        
        std::unique_ptr<BaseExpression> UnaryNumericalFunctionExpression::simplify() const {
            return std::unique_ptr<BaseExpression>(new UnaryNumericalFunctionExpression(this->getReturnType(), this->getOperand()->simplify(), this->getOperatorType()));
        }
        
        void UnaryNumericalFunctionExpression::accept(ExpressionVisitor* visitor) const {
            visitor->visit(this);
        }
        
        std::unique_ptr<BaseExpression> UnaryNumericalFunctionExpression::clone() const {
            return std::unique_ptr<BaseExpression>(new UnaryNumericalFunctionExpression(*this));
        }
    }
}