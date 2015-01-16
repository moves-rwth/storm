#include <algorithm>
#include <cmath>

#include "src/storage/expressions/BinaryNumericalFunctionExpression.h"
#include "src/storage/expressions/IntegerLiteralExpression.h"
#include "src/storage/expressions/DoubleLiteralExpression.h"
#include "src/utility/macros.h"
#include "src/exceptions/InvalidTypeException.h"

namespace storm {
    namespace expressions {
        BinaryNumericalFunctionExpression::BinaryNumericalFunctionExpression(ExpressionManager const& manager, Type const& type, std::shared_ptr<BaseExpression const> const& firstOperand, std::shared_ptr<BaseExpression const> const& secondOperand, OperatorType operatorType) : BinaryExpression(manager, type, firstOperand, secondOperand), operatorType(operatorType) {
            // Intentionally left empty.
        }
        
        BinaryNumericalFunctionExpression::OperatorType BinaryNumericalFunctionExpression::getOperatorType() const {
            return this->operatorType;
        }
        
        storm::expressions::OperatorType BinaryNumericalFunctionExpression::getOperator() const {
            switch (this->getOperatorType()) {
                case OperatorType::Plus: return storm::expressions::OperatorType::Plus; break;
                case OperatorType::Minus: return storm::expressions::OperatorType::Minus; break;
                case OperatorType::Times: return storm::expressions::OperatorType::Times; break;
                case OperatorType::Divide: return storm::expressions::OperatorType::Divide; break;
                case OperatorType::Min: return storm::expressions::OperatorType::Min; break;
                case OperatorType::Max: return storm::expressions::OperatorType::Max; break;
                case OperatorType::Power: return storm::expressions::OperatorType::Power; break;
            }
        }
        
        int_fast64_t BinaryNumericalFunctionExpression::evaluateAsInt(Valuation const* valuation) const {
            STORM_LOG_THROW(this->hasIntegerType(), storm::exceptions::InvalidTypeException, "Unable to evaluate expression as integer.");
            
            int_fast64_t firstOperandEvaluation = this->getFirstOperand()->evaluateAsInt(valuation);
            int_fast64_t secondOperandEvaluation = this->getSecondOperand()->evaluateAsInt(valuation);
            switch (this->getOperatorType()) {
                case OperatorType::Plus: return firstOperandEvaluation + secondOperandEvaluation; break;
                case OperatorType::Minus: return firstOperandEvaluation - secondOperandEvaluation; break;
                case OperatorType::Times: return firstOperandEvaluation * secondOperandEvaluation; break;
                case OperatorType::Divide: return firstOperandEvaluation / secondOperandEvaluation; break;
                case OperatorType::Min: return std::min(firstOperandEvaluation, secondOperandEvaluation); break;
                case OperatorType::Max: return std::max(firstOperandEvaluation, secondOperandEvaluation); break;
                case OperatorType::Power: return static_cast<int_fast64_t>(std::pow(firstOperandEvaluation, secondOperandEvaluation)); break;
            }
        }
        
        double BinaryNumericalFunctionExpression::evaluateAsDouble(Valuation const* valuation) const {
            STORM_LOG_THROW(this->hasNumericalType(), storm::exceptions::InvalidTypeException, "Unable to evaluate expression as double.");
            
            double firstOperandEvaluation = this->getFirstOperand()->evaluateAsDouble(valuation);
            double secondOperandEvaluation = this->getSecondOperand()->evaluateAsDouble(valuation);
            switch (this->getOperatorType()) {
                case OperatorType::Plus: return firstOperandEvaluation + secondOperandEvaluation; break;
                case OperatorType::Minus: return firstOperandEvaluation - secondOperandEvaluation; break;
                case OperatorType::Times: return firstOperandEvaluation * secondOperandEvaluation; break;
                case OperatorType::Divide: return firstOperandEvaluation / secondOperandEvaluation; break;
                case OperatorType::Min: return std::min(firstOperandEvaluation, secondOperandEvaluation); break;
                case OperatorType::Max: return std::max(firstOperandEvaluation, secondOperandEvaluation); break;
                case OperatorType::Power: return std::pow(firstOperandEvaluation, secondOperandEvaluation); break;
            }
        }
        
        std::shared_ptr<BaseExpression const> BinaryNumericalFunctionExpression::simplify() const {
            std::shared_ptr<BaseExpression const> firstOperandSimplified = this->getFirstOperand()->simplify();
            std::shared_ptr<BaseExpression const> secondOperandSimplified = this->getSecondOperand()->simplify();
            
            if (firstOperandSimplified->isLiteral() && secondOperandSimplified->isLiteral()) {
                if (this->hasIntegerType()) {
                    int_fast64_t firstOperandEvaluation = firstOperandSimplified->evaluateAsInt();
                    int_fast64_t secondOperandEvaluation = secondOperandSimplified->evaluateAsInt();
                    int_fast64_t newValue = 0;
                    switch (this->getOperatorType()) {
                        case OperatorType::Plus: newValue = firstOperandEvaluation + secondOperandEvaluation; break;
                        case OperatorType::Minus: newValue = firstOperandEvaluation - secondOperandEvaluation; break;
                        case OperatorType::Times: newValue = firstOperandEvaluation * secondOperandEvaluation; break;
                        case OperatorType::Divide: newValue = firstOperandEvaluation / secondOperandEvaluation; break;
                        case OperatorType::Min: newValue = std::min(firstOperandEvaluation, secondOperandEvaluation); break;
                        case OperatorType::Max: newValue = std::max(firstOperandEvaluation, secondOperandEvaluation); break;
                        case OperatorType::Power: newValue = static_cast<int_fast64_t>(std::pow(firstOperandEvaluation, secondOperandEvaluation)); break;
                    }
                    return std::shared_ptr<BaseExpression>(new IntegerLiteralExpression(this->getManager(), newValue));
                } else if (this->hasRationalType()) {
                    double firstOperandEvaluation = firstOperandSimplified->evaluateAsDouble();
                    double secondOperandEvaluation = secondOperandSimplified->evaluateAsDouble();
                    double newValue = 0;
                    switch (this->getOperatorType()) {
                        case OperatorType::Plus: newValue = firstOperandEvaluation + secondOperandEvaluation; break;
                        case OperatorType::Minus: newValue = firstOperandEvaluation - secondOperandEvaluation; break;
                        case OperatorType::Times: newValue = firstOperandEvaluation * secondOperandEvaluation; break;
                        case OperatorType::Divide: newValue = firstOperandEvaluation / secondOperandEvaluation; break;
                        case OperatorType::Min: newValue = std::min(firstOperandEvaluation, secondOperandEvaluation); break;
                        case OperatorType::Max: newValue = std::max(firstOperandEvaluation, secondOperandEvaluation); break;
                        case OperatorType::Power: newValue = static_cast<int_fast64_t>(std::pow(firstOperandEvaluation, secondOperandEvaluation)); break;
                    }
                    return std::shared_ptr<BaseExpression>(new DoubleLiteralExpression(this->getManager(), newValue));
                }
            }
            
            if (firstOperandSimplified.get() == this->getFirstOperand().get() && secondOperandSimplified.get() == this->getSecondOperand().get()) {
                return this->shared_from_this();
            } else {
                return std::shared_ptr<BaseExpression>(new BinaryNumericalFunctionExpression(this->getManager(), this->getType(), firstOperandSimplified, secondOperandSimplified, this->getOperatorType()));
            }
        }
        
        boost::any BinaryNumericalFunctionExpression::accept(ExpressionVisitor& visitor) const {
            return visitor.visit(*this);
        }
        
        void BinaryNumericalFunctionExpression::printToStream(std::ostream& stream) const {
            stream << "(";
            switch (this->getOperatorType()) {
                case OperatorType::Plus: stream << *this->getFirstOperand() << " + " << *this->getSecondOperand(); break;
                case OperatorType::Minus: stream << *this->getFirstOperand() << " - " << *this->getSecondOperand(); break;
                case OperatorType::Times: stream << *this->getFirstOperand() << " * " << *this->getSecondOperand(); break;
                case OperatorType::Divide: stream << *this->getFirstOperand() << " / " << *this->getSecondOperand(); break;
                case OperatorType::Min: stream << "min(" << *this->getFirstOperand() << ", " << *this->getSecondOperand() << ")"; break;
                case OperatorType::Max: stream << "max(" << *this->getFirstOperand() << ", " << *this->getSecondOperand() << ")"; break;
                case OperatorType::Power: stream << *this->getFirstOperand() << " ^ " << *this->getSecondOperand(); break;
            }
            stream << ")";
        }
    }
}