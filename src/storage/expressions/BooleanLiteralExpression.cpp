#include "src/storage/expressions/BooleanLiteralExpression.h"

namespace storm {
    namespace expressions {
        BooleanLiteralExpression::BooleanLiteralExpression(bool value) : BaseExpression(ExpressionReturnType::Bool), value(value) {
            // Intentionally left empty.
        }
        
        bool BooleanLiteralExpression::evaluateAsBool(Valuation const* valuation) const {
            return this->getValue();
        }
        
        bool BooleanLiteralExpression::isLiteral() const {
            return true;
        }
        
        bool BooleanLiteralExpression::hasConstantValue() const {
            return true;
        }
        
        bool BooleanLiteralExpression::isTrue() const {
            return this->getValue() == true;
        }
        
        bool BooleanLiteralExpression::isFalse() const {
            return this->getValue() == false;
        }
        
        std::set<std::string> BooleanLiteralExpression::getVariables() const {
            return std::set<std::string>();
        }
        
        std::set<std::string> BooleanLiteralExpression::getConstants() const {
            return std::set<std::string>();
        }
        
        std::shared_ptr<BaseExpression const> BooleanLiteralExpression::simplify() const {
            return this->shared_from_this();
        }
        
        void BooleanLiteralExpression::accept(ExpressionVisitor* visitor) const {
            visitor->visit(this);
        }
        
        bool BooleanLiteralExpression::getValue() const {
            return this->value;
        }
        
        void BooleanLiteralExpression::printToStream(std::ostream& stream) const {
            stream << (this->getValue() ? "true" : "false");
        }
    }
}