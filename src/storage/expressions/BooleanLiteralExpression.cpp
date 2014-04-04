#include "src/storage/expressions/BooleanLiteralExpression.h"

namespace storm {
    namespace expressions {
        BooleanLiteralExpression::BooleanLiteralExpression(bool value) : value(value) {
            // Intentionally left empty.
        }
        
        bool BooleanLiteralExpression::evaluateAsBool(Valuation const& valuation) const {
            return this->getValue();
        }
        
        bool BooleanLiteralExpression::isConstant() const {
            return true;
        }
        
        bool BooleanLiteralExpression::isTrue() const {
            return this->getValue() == true;
        }
        
        bool BooleanLiteralExpression::isFalse() const {
            return this->getValue() == false;
        }
        
        std::set<std::string> BooleanLiteralExpression::getVariables() const {
            return {};
        }
        
        std::set<std::string> BooleanLiteralExpression::getConstants() const {
            return {};
        }
        
        std::unique_ptr<BaseExpression> BooleanLiteralExpression::simplify() const {
            return this->clone();
        }
        
        void BooleanLiteralExpression::accept(ExpressionVisitor* visitor) const {
            visitor->visit(this);
        }
        
        std::unique_ptr<BaseExpression> BooleanLiteralExpression::clone() const {
            return std::unique_ptr<BaseExpression>(new BooleanLiteralExpression(*this));
        }
        
        bool BooleanLiteralExpression::getValue() const {
            return this->value;
        }
    }
}