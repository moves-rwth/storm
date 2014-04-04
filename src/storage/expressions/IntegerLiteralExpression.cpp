#include "src/storage/expressions/IntegerLiteralExpression.h"

namespace storm {
    namespace expressions {
        IntegerLiteralExpression::IntegerLiteralExpression(int_fast64_t value) : value(value) {
            // Intentionally left empty.
        }
        
        int_fast64_t IntegerLiteralExpression::evaluateAsInt(Valuation const& valuation) const {
            return this->getValue();
        }
        
        double IntegerLiteralExpression::evaluateAsDouble(Valuation const& valuation) const {
            return static_cast<double>(this->getValue());
        }
        
        bool IntegerLiteralExpression::isConstant() const {
            return true;
        }
        
        std::set<std::string> IntegerLiteralExpression::getVariables() const {
            return {};
        }
        
        std::set<std::string> IntegerLiteralExpression::getConstants() const {
            return {};
        }
        
        std::unique_ptr<BaseExpression> IntegerLiteralExpression::simplify() const {
            return this->clone();
        }
        
        void IntegerLiteralExpression::accept(ExpressionVisitor* visitor) const {
            visitor->visit(this);
        }
        
        std::unique_ptr<BaseExpression> IntegerLiteralExpression::clone() const {
            return std::unique_ptr<BaseExpression>(new IntegerLiteralExpression(*this));
        }
        
        int_fast64_t IntegerLiteralExpression::getValue() const {
            return this->value;
        }
    }
}