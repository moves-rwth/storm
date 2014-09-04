#include "src/storage/expressions/IntegerLiteralExpression.h"

namespace storm {
    namespace expressions {
        IntegerLiteralExpression::IntegerLiteralExpression(int_fast64_t value) : BaseExpression(ExpressionReturnType::Int), value(value) {
            // Intentionally left empty.
        }
        
        int_fast64_t IntegerLiteralExpression::evaluateAsInt(Valuation const* valuation) const {
            return this->getValue();
        }
        
        double IntegerLiteralExpression::evaluateAsDouble(Valuation const* valuation) const {
            return static_cast<double>(this->evaluateAsInt(valuation));
        }
        
        bool IntegerLiteralExpression::isLiteral() const {
            return true;
		}

		std::set<std::string> IntegerLiteralExpression::getVariables() const {
			return std::set<std::string>();
		}

		std::map<std::string,ExpressionReturnType> IntegerLiteralExpression::getVariablesAndTypes() const {
			return std::map<std::string, ExpressionReturnType>();
		}
        
        std::shared_ptr<BaseExpression const> IntegerLiteralExpression::simplify() const {
            return this->shared_from_this();
        }
        
        void IntegerLiteralExpression::accept(ExpressionVisitor* visitor) const {
            visitor->visit(this);
        }
        
        int_fast64_t IntegerLiteralExpression::getValue() const {
            return this->value;
        }
        
        void IntegerLiteralExpression::printToStream(std::ostream& stream) const {
            stream << this->getValue();
        }
    }
}