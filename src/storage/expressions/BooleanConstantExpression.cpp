#include "src/storage/expressions/BooleanConstantExpression.h"

namespace storm {
    namespace expressions {
        BooleanConstantExpression::BooleanConstantExpression(std::string const& constantName) : ConstantExpression(ExpressionReturnType::Bool, constantName) {
            // Intentionally left empty.
        }
                
        bool BooleanConstantExpression::evaluateAsBool(Valuation const& valuation) const {
            return valuation.getBooleanValue(this->getConstantName());
        }
        
        void BooleanConstantExpression::accept(ExpressionVisitor* visitor) const {
            visitor->visit(this);
        }
        
        std::shared_ptr<BaseExpression const> BooleanConstantExpression::simplify() const {
            return this->shared_from_this();
        }
    }
}