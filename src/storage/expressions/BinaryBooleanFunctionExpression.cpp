#include "src/storage/expressions/BinaryBooleanFunctionExpression.h"
#include "src/storage/expressions/BooleanLiteralExpression.h"
#include "src/utility/macros.h"
#include "src/exceptions/InvalidTypeException.h"

namespace storm {
    namespace expressions {
        BinaryBooleanFunctionExpression::BinaryBooleanFunctionExpression(ExpressionManager const& manager, ExpressionReturnType returnType, std::shared_ptr<BaseExpression const> const& firstOperand, std::shared_ptr<BaseExpression const> const& secondOperand, OperatorType operatorType) : BinaryExpression(manager, returnType, firstOperand, secondOperand), operatorType(operatorType) {
            // Intentionally left empty.
        }
        
        BinaryBooleanFunctionExpression::OperatorType BinaryBooleanFunctionExpression::getOperatorType() const {
            return this->operatorType;
        }
        
        storm::expressions::OperatorType BinaryBooleanFunctionExpression::getOperator() const {
            switch (this->getOperatorType()) {
                case OperatorType::And: return storm::expressions::OperatorType::And; break;
                case OperatorType::Or: return storm::expressions::OperatorType::Or; break;
                case OperatorType::Xor: return storm::expressions::OperatorType::Xor; break;
                case OperatorType::Implies: return storm::expressions::OperatorType::Implies; break;
                case OperatorType::Iff: return storm::expressions::OperatorType::Iff; break;
            }
        }
                
        bool BinaryBooleanFunctionExpression::evaluateAsBool(Valuation const* valuation) const {
            STORM_LOG_THROW(this->hasBooleanReturnType(), storm::exceptions::InvalidTypeException, "Unable to evaluate expression as boolean.");
            
            bool firstOperandEvaluation = this->getFirstOperand()->evaluateAsBool(valuation);
            bool secondOperandEvaluation = this->getSecondOperand()->evaluateAsBool(valuation);
            
            bool result;
            switch (this->getOperatorType()) {
                case OperatorType::And: result = firstOperandEvaluation && secondOperandEvaluation; break;
                case OperatorType::Or: result = firstOperandEvaluation || secondOperandEvaluation; break;
                case OperatorType::Xor: result = firstOperandEvaluation ^ secondOperandEvaluation; break;
                case OperatorType::Implies: result = !firstOperandEvaluation || secondOperandEvaluation; break;
                case OperatorType::Iff: result = (firstOperandEvaluation && secondOperandEvaluation) || (!firstOperandEvaluation && !secondOperandEvaluation); break;
            }
            
            return result;
        }
                
        std::shared_ptr<BaseExpression const> BinaryBooleanFunctionExpression::simplify() const {
            std::shared_ptr<BaseExpression const> firstOperandSimplified = this->getFirstOperand()->simplify();
            std::shared_ptr<BaseExpression const> secondOperandSimplified = this->getSecondOperand()->simplify();
            
            switch (this->getOperatorType()) {
                case OperatorType::And: if (firstOperandSimplified->isTrue()) {
                    return secondOperandSimplified;
                } else if (firstOperandSimplified->isFalse()) {
                    return firstOperandSimplified;
                } else if (secondOperandSimplified->isTrue()) {
                    return firstOperandSimplified;
                } else if (secondOperandSimplified->isFalse()) {
                    return secondOperandSimplified;
                }
                break;
                case OperatorType::Or: if (firstOperandSimplified->isTrue()) {
                    return firstOperandSimplified;
                } else if (firstOperandSimplified->isFalse()) {
                    return secondOperandSimplified;
                } else if (secondOperandSimplified->isTrue()) {
                    return secondOperandSimplified;
                } else if (secondOperandSimplified->isFalse()) {
                    return firstOperandSimplified;
                }
                break;
                case OperatorType::Xor: break;
                case OperatorType::Implies: if (firstOperandSimplified->isTrue()) {
                    return secondOperandSimplified;
                } else if (firstOperandSimplified->isFalse()) {
                    return std::shared_ptr<BaseExpression>(new BooleanLiteralExpression(true));
                } else if (secondOperandSimplified->isTrue()) {
                    return std::shared_ptr<BaseExpression>(new BooleanLiteralExpression(true));
                }
                break;
                case OperatorType::Iff: if (firstOperandSimplified->isTrue() && secondOperandSimplified->isTrue()) {
                    return std::shared_ptr<BaseExpression>(new BooleanLiteralExpression(true));
                } else if (firstOperandSimplified->isFalse() && secondOperandSimplified->isFalse()) {
                    return std::shared_ptr<BaseExpression>(new BooleanLiteralExpression(true));
                }
                break;
            }
            
            // If the two successors remain unchanged, we can return a shared_ptr to this very object.
            if (firstOperandSimplified.get() == this->getFirstOperand().get() && secondOperandSimplified.get() == this->getSecondOperand().get()) {
                return this->shared_from_this();
            } else {
                return std::shared_ptr<BaseExpression>(new BinaryBooleanFunctionExpression(this->getReturnType(), firstOperandSimplified, secondOperandSimplified, this->getOperatorType()));
            }
        }
        
        boost::any BinaryBooleanFunctionExpression::accept(ExpressionVisitor& visitor) const {
            return visitor.visit(*this);
        }
        
        void BinaryBooleanFunctionExpression::printToStream(std::ostream& stream) const {
            stream << "(" << *this->getFirstOperand();
            switch (this->getOperatorType()) {
                case OperatorType::And: stream << " & "; break;
                case OperatorType::Or: stream << " | "; break;
                case OperatorType::Xor: stream << " != "; break;
                case OperatorType::Implies: stream << " => "; break;
                case OperatorType::Iff: stream << " = "; break;
            }
            stream << *this->getSecondOperand() << ")";
        }
    }
}