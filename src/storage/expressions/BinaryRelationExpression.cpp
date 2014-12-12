#include "src/storage/expressions/BinaryRelationExpression.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidTypeException.h"

namespace storm {
    namespace expressions {
        BinaryRelationExpression::BinaryRelationExpression(ExpressionReturnType returnType, std::shared_ptr<BaseExpression const> const& firstOperand, std::shared_ptr<BaseExpression const> const& secondOperand, RelationType relationType) : BinaryExpression(returnType, firstOperand, secondOperand), relationType(relationType) {
            // Intentionally left empty.
        }
        
        storm::expressions::OperatorType BinaryRelationExpression::getOperator() const {
            switch (this->getRelationType()) {
                case RelationType::Equal: return storm::expressions::OperatorType::Equal; break;
                case RelationType::NotEqual: return storm::expressions::OperatorType::NotEqual; break;
                case RelationType::Less: return storm::expressions::OperatorType::Less; break;
                case RelationType::LessOrEqual: return storm::expressions::OperatorType::LessOrEqual; break;
                case RelationType::Greater: return storm::expressions::OperatorType::Greater; break;
                case RelationType::GreaterOrEqual: return storm::expressions::OperatorType::GreaterOrEqual; break;
            }
        }
        
        bool BinaryRelationExpression::evaluateAsBool(Valuation const* valuation) const {
            STORM_LOG_THROW(this->hasBooleanReturnType(), storm::exceptions::InvalidTypeException, "Unable to evaluate expression as boolean.");

            double firstOperandEvaluated = this->getFirstOperand()->evaluateAsDouble(valuation);
            double secondOperandEvaluated = this->getSecondOperand()->evaluateAsDouble(valuation);
            switch (this->getRelationType()) {
                case RelationType::Equal: return firstOperandEvaluated == secondOperandEvaluated; break;
                case RelationType::NotEqual: return firstOperandEvaluated != secondOperandEvaluated; break;
                case RelationType::Greater: return firstOperandEvaluated > secondOperandEvaluated; break;
                case RelationType::GreaterOrEqual: return firstOperandEvaluated >= secondOperandEvaluated; break;
                case RelationType::Less: return firstOperandEvaluated < secondOperandEvaluated; break;
                case RelationType::LessOrEqual: return firstOperandEvaluated <= secondOperandEvaluated; break;
            }
        }
        
        std::shared_ptr<BaseExpression const> BinaryRelationExpression::simplify() const {
            std::shared_ptr<BaseExpression const> firstOperandSimplified = this->getFirstOperand()->simplify();
            std::shared_ptr<BaseExpression const> secondOperandSimplified = this->getSecondOperand()->simplify();
            
            if (firstOperandSimplified.get() == this->getFirstOperand().get() && secondOperandSimplified.get() == this->getSecondOperand().get()) {
                return this->shared_from_this();
            } else {
                return std::shared_ptr<BaseExpression>(new BinaryRelationExpression(this->getReturnType(), firstOperandSimplified, secondOperandSimplified, this->getRelationType()));
            }
        }
        
        void BinaryRelationExpression::accept(ExpressionVisitor* visitor) const {
            visitor->visit(this);
        }
        
        BinaryRelationExpression::RelationType BinaryRelationExpression::getRelationType() const {
            return this->relationType;
        }
        
        void BinaryRelationExpression::printToStream(std::ostream& stream) const {
            stream << "(" << *this->getFirstOperand();
            switch (this->getRelationType()) {
                case RelationType::Equal: stream << " = "; break;
                case RelationType::NotEqual: stream << " != "; break;
                case RelationType::Greater: stream << " > "; break;
                case RelationType::GreaterOrEqual: stream << " >= "; break;
                case RelationType::Less: stream << " < "; break;
                case RelationType::LessOrEqual: stream << " <= "; break;
            }
            stream << *this->getSecondOperand() << ")";
        }
    }
}