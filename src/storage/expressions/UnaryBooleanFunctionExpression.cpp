#include "src/storage/expressions/UnaryBooleanFunctionExpression.h"
#include "src/storage/expressions/BooleanLiteralExpression.h"

namespace storm {
    namespace expressions {
        UnaryBooleanFunctionExpression::UnaryBooleanFunctionExpression(ExpressionReturnType returnType, std::unique_ptr<BaseExpression>&& operand, OperatorType operatorType) : UnaryExpression(returnType, std::move(operand)), operatorType(operatorType) {
            // Intentionally left empty.
        }
        
        OperatorType UnaryBooleanFunctionExpression::getOperatorType() const {
            return this->operatorType;
        }
        
        bool UnaryBooleanFunctionExpression::evaluateAsBool(Valuation const& valuation) const {
            bool operandEvaluated = this->getOperand()->evaluateAsBool(valuation);
            switch (this->getOperatorType()) {
                case OperatorType::Not: return !operandEvaluated; break;
            }
        }
        
        std::unique_ptr<BaseExpression> UnaryBooleanFunctionExpression::simplify() const {
            std::unique_ptr<BaseExpression> operandSimplified = this->getOperand()->simplify();
            switch (this->getOperatorType()) {
                case OperatorType::Not: if (operandSimplified->isTrue()) {
                    return std::unique_ptr<BaseExpression>(new BooleanLiteralExpression(false));
                } else {
                    return std::unique_ptr<BaseExpression>(new BooleanLiteralExpression(true));
                }
            }
            
            return UnaryBooleanFunctionExpression(this->getReturnType(), std::move(operandSimplified), this->getOperatorType());
        }
        
        void UnaryBooleanFunctionExpression::accept(ExpressionVisitor* visitor) const {
            visitor->visit(this);
        }
        
        std::unique_ptr<BaseExpression> UnaryBooleanFunctionExpression::clone() const {
            return std::unique_ptr<BaseExpression>(new UnaryBooleanFunctionExpression(*this));
        }
    }
}