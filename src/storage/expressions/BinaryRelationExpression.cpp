#include "src/storage/expressions/BinaryRelationExpression.h"

namespace storm {
    namespace expressions {
        BinaryRelationExpression::BinaryRelationExpression(ExpressionReturnType returnType, std::unique_ptr<BaseExpression>&& firstOperand, std::unique_ptr<BaseExpression>&& secondOperand, RelationType relationType) : BinaryExpression(returnType, std::move(firstOperand), std::move(secondOperand)), relationType(relationType) {
            // Intentionally left empty.
        }
        
        BinaryRelationExpression::BinaryRelationExpression(BinaryRelationExpression const& other) : BinaryExpression(other), relationType(other.getRelationType()) {
            // Intentionally left empty.
        }
        
        BinaryRelationExpression& BinaryRelationExpression::operator=(BinaryRelationExpression const& other) {
            if (this != &other) {
                BinaryExpression::operator=(other);
                this->relationType = other.getRelationType();
            }
            return *this;
        }
        
        bool BinaryRelationExpression::evaluateAsBool(Valuation const& valuation) const {
            double firstOperandEvaluated = this->getFirstOperand()->evaluateAsDouble(valuation);
            double secondOperandEvaluated = this->getSecondOperand()->evaluateAsDouble(valuation);
            switch (this->getRelationType()) {
                case EQUAL: return firstOperandEvaluated == secondOperandEvaluated; break;
                case NOT_EQUAL: return firstOperandEvaluated != secondOperandEvaluated; break;
                case GREATER: return firstOperandEvaluated > secondOperandEvaluated; break;
                case GREATER_OR_EQUAL: return firstOperandEvaluated >= secondOperandEvaluated; break;
                case LESS: return firstOperandEvaluated < secondOperandEvaluated; break;
                case LESS_OR_EQUAL: return firstOperandEvaluated <= secondOperandEvaluated; break;
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