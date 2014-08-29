#include <cmath>

#include "src/storage/expressions/UnaryNumericalFunctionExpression.h"
#include "src/exceptions/ExceptionMacros.h"
#include "src/exceptions/InvalidTypeException.h"

namespace storm {
    namespace expressions {
        UnaryNumericalFunctionExpression::UnaryNumericalFunctionExpression(ExpressionReturnType returnType, std::shared_ptr<BaseExpression const> const& operand, OperatorType operatorType) : UnaryExpression(returnType, operand), operatorType(operatorType) {
            // Intentionally left empty.
        }
        
        UnaryNumericalFunctionExpression::OperatorType UnaryNumericalFunctionExpression::getOperatorType() const {
            return this->operatorType;
        }
        
        storm::expressions::OperatorType UnaryNumericalFunctionExpression::getOperator() const {
            switch (this->getOperatorType()) {
                case OperatorType::Minus: return storm::expressions::OperatorType::Minus; break;
                case OperatorType::Floor: return storm::expressions::OperatorType::Floor; break;
                case OperatorType::Ceil: return storm::expressions::OperatorType::Ceil; break;
            }
        }
        
        int_fast64_t UnaryNumericalFunctionExpression::evaluateAsInt(Valuation const* valuation) const {
            LOG_THROW(this->hasIntegralReturnType(), storm::exceptions::InvalidTypeException, "Unable to evaluate expression as integer.");

            int_fast64_t operandEvaluated = this->getOperand()->evaluateAsInt(valuation);
            switch (this->getOperatorType()) {
                case OperatorType::Minus: return -operandEvaluated; break;
                case OperatorType::Floor: return std::floor(operandEvaluated); break;
                case OperatorType::Ceil: return std::ceil(operandEvaluated); break;
            }
        }
        
        double UnaryNumericalFunctionExpression::evaluateAsDouble(Valuation const* valuation) const {
            LOG_THROW(this->hasNumericalReturnType(), storm::exceptions::InvalidTypeException, "Unable to evaluate expression as double.");

            double operandEvaluated = this->getOperand()->evaluateAsDouble(valuation);
            switch (this->getOperatorType()) {
                case OperatorType::Minus: return -operandEvaluated; break;
                case OperatorType::Floor: return std::floor(operandEvaluated); break;
                case OperatorType::Ceil: return std::ceil(operandEvaluated); break;
            }
        }
        
        std::shared_ptr<BaseExpression const> UnaryNumericalFunctionExpression::simplify() const {
            std::shared_ptr<BaseExpression const> operandSimplified = this->getOperand()->simplify();
            
            if (operandSimplified.get() == this->getOperand().get()) {
                return this->shared_from_this();
            } else {
                return std::shared_ptr<BaseExpression>(new UnaryNumericalFunctionExpression(this->getReturnType(), operandSimplified, this->getOperatorType()));
            }
        }
        
        void UnaryNumericalFunctionExpression::accept(ExpressionVisitor* visitor) const {
            visitor->visit(this);
        }
        
        void UnaryNumericalFunctionExpression::printToStream(std::ostream& stream) const {
            switch (this->getOperatorType()) {
                case OperatorType::Minus: stream << "-("; break;
                case OperatorType::Floor: stream << "floor("; break;
                case OperatorType::Ceil: stream << "ceil("; break;
            }
            stream << *this->getOperand() << ")";
        }
    }
}