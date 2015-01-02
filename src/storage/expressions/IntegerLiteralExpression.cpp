#include "src/storage/expressions/IntegerLiteralExpression.h"
#include "src/storage/expressions/ExpressionManager.h"

namespace storm {
    namespace expressions {
        IntegerLiteralExpression::IntegerLiteralExpression(ExpressionManager const& manager, int_fast64_t value) : BaseExpression(manager, manager.getIntegerType()), value(value) {
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

        std::shared_ptr<BaseExpression const> IntegerLiteralExpression::simplify() const {
            return this->shared_from_this();
        }
        
        boost::any IntegerLiteralExpression::accept(ExpressionVisitor& visitor) const {
            return visitor.visit(*this);
        }
        
        int_fast64_t IntegerLiteralExpression::getValue() const {
            return this->value;
        }
        
        void IntegerLiteralExpression::printToStream(std::ostream& stream) const {
            stream << this->getValue();
        }
    }
}