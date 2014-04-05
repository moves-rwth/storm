#include "src/storage/expressions/DoubleConstantExpression.h"

namespace storm {
    namespace expressions {
        DoubleConstantExpression::DoubleConstantExpression(std::string const& constantName) : ConstantExpression(ExpressionReturnType::Double, constantName) {
            // Intentionally left empty.
        }
        
        double DoubleConstantExpression::evaluateAsDouble(Valuation const& valuation) const {
            return valuation.getDoubleValue(this->getConstantName());
        }
        
        std::shared_ptr<BaseExpression const> DoubleConstantExpression::simplify() const {
            return this->shared_from_this();
        }
        
        void DoubleConstantExpression::accept(ExpressionVisitor* visitor) const {
            visitor->visit(this);
        }
    }
}