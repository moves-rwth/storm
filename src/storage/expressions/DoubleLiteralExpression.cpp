#include "src/storage/expressions/DoubleLiteralExpression.h"

namespace storm {
    namespace expressions {
        DoubleLiteralExpression::DoubleLiteralExpression(double value) : BaseExpression(ExpressionReturnType::Double), value(value) {
            // Intentionally left empty.
        }
        
        double DoubleLiteralExpression::evaluateAsDouble(Valuation const* valuation) const {
            return this->getValue();
        }
        
        bool DoubleLiteralExpression::isLiteral() const {
            return true;
        }
        
        std::set<std::string> DoubleLiteralExpression::getVariables() const {
            return std::set<std::string>();
		}

		std::map<std::string, ExpressionReturnType> DoubleLiteralExpression::getVariablesAndTypes() const {
			return std::map<std::string, ExpressionReturnType>();
		}
        
        std::shared_ptr<BaseExpression const> DoubleLiteralExpression::simplify() const {
            return this->shared_from_this();
        }
        
        boost::any DoubleLiteralExpression::accept(ExpressionVisitor& visitor) const {
            return visitor.visit(*this);
        }
        
        double DoubleLiteralExpression::getValue() const {
            return this->value;
        }
        
        void DoubleLiteralExpression::printToStream(std::ostream& stream) const {
            stream << this->getValue();
        }
    }
}