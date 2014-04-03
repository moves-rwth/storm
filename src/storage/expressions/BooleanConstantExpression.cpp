#include "src/storage/expressions/BooleanConstantExpression.h"

namespace storm {
    namespace expressions {
        BooleanConstantExpression::BooleanConstantExpression(std::string const& constantName) : ConstantExpression(ExpressionReturnType::bool_, constantName) {
            // Intentionally left empty.
        }
        
        BooleanConstantExpression::BooleanConstantExpression(BooleanConstantExpression const& other) : ConstantExpression(other) {
            // Intentionally left empty.
        }
        
        BooleanConstantExpression& BooleanConstantExpression::operator=(BooleanConstantExpression const& other) {
            if (this != &other) {
                ConstantExpression::operator=(other);
            }
            return *this;
        }
        
        bool BooleanConstantExpression::evaluateAsBool(Valuation const& valuation) const {
            return valuation.getBooleanValue(this->getConstantName());
        }
        
        std::unique_ptr<BaseExpression> BooleanConstantExpression::clone() const {
            return std::unique_ptr<BaseExpression>(new BooleanConstantExpression(*this));
        }
        
        void BooleanConstantExpression::accept(ExpressionVisitor* visitor) const {
            visitor->visit(this);
        }
    }
}