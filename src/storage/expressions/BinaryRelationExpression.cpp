#include "src/storage/expressions/BinaryRelationExpression.h"

namespace storm {
    namespace expressions {
        BinaryRelationExpression::BinaryRelationExpression(ExpressionReturnType returnType, std::unique_ptr<BaseExpression>&& firstOperand, std::unique_ptr<BaseExpression>&& secondOperand, RelationType relationType) : BinaryExpression(returnType, std::move(firstOperand), std::move(secondOperand)), relationType(relationType) {
            // Intentionally left empty.
        }
                
        bool BinaryRelationExpression::evaluateAsBool(Valuation const& valuation) const {
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
        
        std::unique_ptr<BaseExpression> BinaryRelationExpression::simplify() const {
            return std::unique_ptr<BaseExpression>(new BinaryRelationExpression(this->getReturnType(), this->getFirstOperand()->simplify(), this->getSecondOperand()->simplify(), this->getRelationType()));
        }
        
        void BinaryRelationExpression::accept(ExpressionVisitor* visitor) const {
            visitor->visit(this);
        }
        
        std::unique_ptr<BaseExpression> BinaryRelationExpression::clone() const {
            return std::unique_ptr<BaseExpression>(new BinaryRelationExpression(*this));
        }
        
        BinaryRelationExpression::RelationType BinaryRelationExpression::getRelationType() const {
            return this->relationType;
        }
    }
}