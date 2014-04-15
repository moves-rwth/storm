#include "src/storage/expressions/UnaryBooleanFunctionExpression.h"
#include "src/storage/expressions/BooleanLiteralExpression.h"
#include "src/exceptions/ExceptionMacros.h"
#include "src/exceptions/InvalidTypeException.h"

namespace storm {
    namespace expressions {
        UnaryBooleanFunctionExpression::UnaryBooleanFunctionExpression(ExpressionReturnType returnType, std::shared_ptr<BaseExpression const> const& operand, OperatorType operatorType) : UnaryExpression(returnType, operand), operatorType(operatorType) {
            // Intentionally left empty.
        }
        
        UnaryBooleanFunctionExpression::OperatorType UnaryBooleanFunctionExpression::getOperatorType() const {
            return this->operatorType;
        }
        
        bool UnaryBooleanFunctionExpression::evaluateAsBool(Valuation const* valuation) const {
            LOG_THROW(this->hasBooleanReturnType(), storm::exceptions::InvalidTypeException, "Unable to evaluate expression as boolean.");

            bool operandEvaluated = this->getOperand()->evaluateAsBool(valuation);
            switch (this->getOperatorType()) {
                case OperatorType::Not: return !operandEvaluated; break;
            }
        }
        
        std::shared_ptr<BaseExpression const> UnaryBooleanFunctionExpression::simplify() const {
            std::shared_ptr<BaseExpression const> operandSimplified = this->getOperand()->simplify();
            switch (this->getOperatorType()) {
                case OperatorType::Not: if (operandSimplified->isTrue()) {
                    return std::shared_ptr<BaseExpression>(new BooleanLiteralExpression(false));
                } else {
                    return std::shared_ptr<BaseExpression>(new BooleanLiteralExpression(true));
                }
            }
            
            if (operandSimplified.get() == this->getOperand().get()) {
                return this->shared_from_this();
            } else {
                return std::shared_ptr<BaseExpression>(new UnaryBooleanFunctionExpression(this->getReturnType(), operandSimplified, this->getOperatorType()));
            }
        }
        
        void UnaryBooleanFunctionExpression::accept(ExpressionVisitor* visitor) const {
            visitor->visit(this);
        }
        
        void UnaryBooleanFunctionExpression::printToStream(std::ostream& stream) const {
            stream << "!(" << *this->getOperand() << ")";
        }
    }
}