#include "src/storage/expressions/BooleanLiteralExpression.h"
#include "src/storage/expressions/ExpressionManager.h"

namespace storm {
    namespace expressions {
        BooleanLiteralExpression::BooleanLiteralExpression(ExpressionManager const& manager, bool value) : BaseExpression(manager, manager.getBooleanType()), value(value) {
            // Intentionally left empty.
        }
        
        bool BooleanLiteralExpression::evaluateAsBool(Valuation const* valuation) const {
            return this->getValue();
        }
        
        bool BooleanLiteralExpression::isLiteral() const {
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
        
        std::shared_ptr<BaseExpression const> BooleanLiteralExpression::simplify() const {
            return this->shared_from_this();
        }
        
        boost::any BooleanLiteralExpression::accept(ExpressionVisitor& visitor) const {
            return visitor.visit(*this);
        }
        
        bool BooleanLiteralExpression::getValue() const {
            return this->value;
        }
        
        void BooleanLiteralExpression::printToStream(std::ostream& stream) const {
            stream << (this->getValue() ? "true" : "false");
        }
    }
}