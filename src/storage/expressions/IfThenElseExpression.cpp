#include "src/storage/expressions/IfThenElseExpression.h"

namespace storm {
    namespace expressions {
        IfThenElseExpression::IfThenElseExpression(ExpressionReturnType returnType, std::shared_ptr<BaseExpression const> const& condition, std::shared_ptr<BaseExpression const> const& thenExpression, std::shared_ptr<BaseExpression const> const& elseExpression) : BaseExpression(returnType), condition(condition), thenExpression(thenExpression), elseExpression(elseExpression) {
            // Intentionally left empty.
        }
        
        bool IfThenElseExpression::evaluateAsBool(Valuation const* valuation) const {
            bool conditionValue = this->condition->evaluateAsBool(valuation);
            if (conditionValue) {
                return this->thenExpression->evaluateAsBool(valuation);
            } else {
                return this->elseExpression->evaluateAsBool(valuation);
            }
        }
        
        int_fast64_t IfThenElseExpression::evaluateAsInt(Valuation const* valuation) const {
            bool conditionValue = this->condition->evaluateAsBool(valuation);
            if (conditionValue) {
                return this->thenExpression->evaluateAsInt(valuation);
            } else {
                return this->elseExpression->evaluateAsInt(valuation);
            }
        }
        
        double IfThenElseExpression::evaluateAsDouble(Valuation const* valuation) const {
            bool conditionValue = this->condition->evaluateAsBool(valuation);
            if (conditionValue) {
                return this->thenExpression->evaluateAsDouble(valuation);
            } else {
                return this->elseExpression->evaluateAsDouble(valuation);
            }
		}

		std::set<std::string> IfThenElseExpression::getVariables() const {
			std::set<std::string> result = this->condition->getVariables();
			std::set<std::string> tmp = this->thenExpression->getVariables();
			result.insert(tmp.begin(), tmp.end());
			tmp = this->elseExpression->getVariables();
			result.insert(tmp.begin(), tmp.end());
			return result;
		}

		std::map<std::string, ExpressionReturnType> IfThenElseExpression::getVariablesAndTypes() const {
			std::map<std::string, ExpressionReturnType>  result = this->condition->getVariablesAndTypes();
			std::map<std::string, ExpressionReturnType>  tmp = this->thenExpression->getVariablesAndTypes();
			result.insert(tmp.begin(), tmp.end());
			tmp = this->elseExpression->getVariablesAndTypes();
			result.insert(tmp.begin(), tmp.end());
			return result;
		}
        
        std::shared_ptr<BaseExpression const> IfThenElseExpression::simplify() const {
            std::shared_ptr<BaseExpression const> conditionSimplified;
            if (conditionSimplified->isTrue()) {
                return this->thenExpression->simplify();
            } else if (conditionSimplified->isFalse()) {
                return this->elseExpression->simplify();
            } else {
                std::shared_ptr<BaseExpression const> thenExpressionSimplified = this->thenExpression->simplify();
                std::shared_ptr<BaseExpression const> elseExpressionSimplified = this->elseExpression->simplify();
                
                if (conditionSimplified.get() == this->condition.get() && thenExpressionSimplified.get() == this->thenExpression.get() && elseExpressionSimplified.get() == this->elseExpression.get()) {
                    return this->shared_from_this();
                } else {
                    return std::shared_ptr<BaseExpression>(new IfThenElseExpression(this->getReturnType(), conditionSimplified, thenExpressionSimplified, elseExpressionSimplified));
                }
            }
        }
        
        void IfThenElseExpression::accept(ExpressionVisitor* visitor) const {
            visitor->visit(this);
        }
        
        std::shared_ptr<BaseExpression const> IfThenElseExpression::getCondition() const {
            return this->condition;
        }
        
        std::shared_ptr<BaseExpression const> IfThenElseExpression::getThenExpression() const {
            return this->thenExpression;
        }
        
        std::shared_ptr<BaseExpression const> IfThenElseExpression::getElseExpression() const {
            return this->elseExpression;
        }
        
        void IfThenElseExpression::printToStream(std::ostream& stream) const {
            stream << "(" << *this->condition << " ? " << *this->thenExpression << " : " << *this->elseExpression << ")";
        }
    }
}