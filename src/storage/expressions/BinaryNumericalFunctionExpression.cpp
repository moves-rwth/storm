#include <algorithm>

#include "src/storage/expressions/BinaryNumericalFunctionExpression.h"
#include "src/exceptions/ExceptionMacros.h"

namespace storm {
    namespace expressions {
        BinaryNumericalFunctionExpression::BinaryNumericalFunctionExpression(ExpressionReturnType returnType, std::unique_ptr<BaseExpression>&& firstOperand, std::unique_ptr<BaseExpression>&& secondOperand, OperatorType operatorType) : BinaryExpression(returnType, std::move(firstOperand), std::move(secondOperand)), operatorType(operatorType) {
            // Intentionally left empty.
        }
        
        int_fast64_t BinaryNumericalFunctionExpression::evaluateAsInt(Valuation const& valuation) const {
            LOG_ASSERT(this->getReturnType() == ExpressionReturnType::Int, "Unable to evaluate expression as integer.");
            int_fast64_t firstOperandEvaluation = this->getFirstOperand()->evaluateAsInt(valuation);
            int_fast64_t secondOperandEvaluation = this->getSecondOperand()->evaluateAsInt(valuation);
            switch (this->getOperatorType()) {
                case OperatorType::Plus: return firstOperandEvaluation + secondOperandEvaluation; break;
                case OperatorType::Minus: return firstOperandEvaluation - secondOperandEvaluation; break;
                case OperatorType::Times: return firstOperandEvaluation * secondOperandEvaluation; break;
                case OperatorType::Divide: return firstOperandEvaluation / secondOperandEvaluation; break;
                case OperatorType::Min: return std::min(firstOperandEvaluation, secondOperandEvaluation); break;
                case OperatorType::Max: return std::max(firstOperandEvaluation, secondOperandEvaluation); break;
            }
        }
        
        double BinaryNumericalFunctionExpression::evaluateAsDouble(Valuation const& valuation) const {
            LOG_ASSERT(this->getReturnType() == ExpressionReturnType::Double, "Unable to evaluate expression as double.");
            double firstOperandEvaluation = this->getFirstOperand()->evaluateAsDouble(valuation);
            double secondOperandEvaluation = this->getSecondOperand()->evaluateAsDouble(valuation);
            switch (this->getOperatorType()) {
                case OperatorType::Plus: return static_cast<double>(firstOperandEvaluation + secondOperandEvaluation); break;
                case OperatorType::Minus: return static_cast<double>(firstOperandEvaluation - secondOperandEvaluation); break;
                case OperatorType::Times: return static_cast<double>(firstOperandEvaluation * secondOperandEvaluation); break;
                case OperatorType::Divide: return static_cast<double>(firstOperandEvaluation / secondOperandEvaluation); break;
                case OperatorType::Min: return static_cast<double>(std::min(firstOperandEvaluation, secondOperandEvaluation)); break;
                case OperatorType::Max: return static_cast<double>(std::max(firstOperandEvaluation, secondOperandEvaluation)); break;
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