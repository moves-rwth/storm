#include "src/storage/expressions/DoubleLiteralExpression.h"

namespace storm {
    namespace expressions {
        DoubleLiteralExpression::DoubleLiteralExpression(double value) : value(value) {
            // Intentionally left empty.
        }
        
        double DoubleLiteralExpression::evaluateAsDouble(Valuation const& valuation) const {
            return this->getValue();
        }
        
        bool DoubleLiteralExpression::isConstant() const {
            return true;
        }
        
        std::set<std::string> DoubleLiteralExpression::getVariables() const {
            return {};
        }
        
        std::set<std::string> DoubleLiteralExpression::getConstants() const {
            return {};
        }
        
        std::unique_ptr<BaseExpression> DoubleLiteralExpression::simplify() const {
            return this->clone();
        }
        
        void DoubleLiteralExpression::accept(ExpressionVisitor* visitor) const {
            visitor->visit(this);
        }
        
        std::unique_ptr<BaseExpression> DoubleLiteralExpression::clone() const {
            return std::unique_ptr<BaseExpression>(new DoubleLiteralExpression(*this));
        }
        
        double DoubleLiteralExpression::getValue() const {
            return this->value;
        }
    }
}