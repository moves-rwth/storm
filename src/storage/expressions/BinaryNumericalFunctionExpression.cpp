#include <algorithm>

#include "src/storage/expressions/BinaryNumericalFunctionExpression.h"
#include "src/exceptions/ExceptionMacros.h"

namespace storm {
    namespace expressions {
        BinaryNumericalFunctionExpression::BinaryNumericalFunctionExpression(ExpressionReturnType returnType, std::unique_ptr<BaseExpression>&& firstOperand, std::unique_ptr<BaseExpression>&& secondOperand, OperatorType operatorType) : BinaryExpression(returnType, std::move(firstOperand), std::move(secondOperand)), operatorType(operatorType) {
            // Intentionally left empty.
        }
        
        BinaryNumericalFunctionExpression::BinaryNumericalFunctionExpression(BinaryNumericalFunctionExpression const& other) : BinaryExpression(other), operatorType(this->getOperatorType()) {
            // Intentionally left empty.
        }
        
        BinaryNumericalFunctionExpression& BinaryNumericalFunctionExpression::operator=(BinaryNumericalFunctionExpression const& other) {
            if (this != &other) {
                BinaryExpression::operator=(other);
                this->operatorType = other.getOperatorType();
            }
            return *this;
        }
        
        int_fast64_t BinaryNumericalFunctionExpression::evaluateAsInt(Valuation const& valuation) const {
            LOG_ASSERT(this->getReturnType() == ExpressionReturnType::int_, "Unable to evaluate expression as integer.");
            int_fast64_t firstOperandEvaluation = this->getFirstOperand()->evaluateAsInt(valuation);
            int_fast64_t secondOperandEvaluation = this->getSecondOperand()->evaluateAsInt(valuation);
            switch (this->getOperatorType()) {
                case PLUS: return firstOperandEvaluation + secondOperandEvaluation; break;
                case MINUS: return firstOperandEvaluation - secondOperandEvaluation; break;
                case TIMES: return firstOperandEvaluation * secondOperandEvaluation; break;
                case DIVIDE: return firstOperandEvaluation / secondOperandEvaluation; break;
                case MIN: return std::min(firstOperandEvaluation, secondOperandEvaluation); break;
                case MAX: return std::max(firstOperandEvaluation, secondOperandEvaluation); break;
            }
        }
        
        double BinaryNumericalFunctionExpression::evaluateAsDouble(Valuation const& valuation) const {
            LOG_ASSERT(this->getReturnType() == ExpressionReturnType::int_, "Unable to evaluate expression as integer.");
            double firstOperandEvaluation = this->getFirstOperand()->evaluateAsInt(valuation);
            double secondOperandEvaluation = this->getSecondOperand()->evaluateAsInt(valuation);
            switch (this->getOperatorType()) {
                case PLUS: return firstOperandEvaluation + secondOperandEvaluation; break;
                case MINUS: return firstOperandEvaluation - secondOperandEvaluation; break;
                case TIMES: return firstOperandEvaluation * secondOperandEvaluation; break;
                case DIVIDE: return firstOperandEvaluation / secondOperandEvaluation; break;
                case MIN: return std::min(firstOperandEvaluation, secondOperandEvaluation); break;
                case MAX: return std::max(firstOperandEvaluation, secondOperandEvaluation); break;
            }
        }
        
        std::unique_ptr<BaseExpression> BinaryNumericalFunctionExpression::simplify() const {
            return std::unique_ptr<BaseExpression>(new BinaryNumericalFunctionExpression(this->getReturnType(), this->getFirstOperand()->simplify(), this->getSecondOperand()->simplify(), this->getOperatorType()));
        }
        
        void BinaryNumericalFunctionExpression::accept(ExpressionVisitor* visitor) const {
            visitor->visit(this);
        }
        
        std::unique_ptr<BaseExpression> BinaryNumericalFunctionExpression::clone() const {
            return std::unique_ptr<BaseExpression>(new BinaryNumericalFunctionExpression(*this));
        }
    }
}