#include "src/storage/expressions/UnaryExpression.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidAccessException.h"

namespace storm {
    namespace expressions {
        UnaryExpression::UnaryExpression(ExpressionManager const& manager, ExpressionReturnType returnType, std::shared_ptr<BaseExpression const> const& operand) : BaseExpression(manager, returnType), operand(operand) {
            // Intentionally left empty.
        }

        bool UnaryExpression::isFunctionApplication() const {
            return true;
        }
        
        bool UnaryExpression::containsVariables() const {
            return this->getOperand()->containsVariables();
        }

		std::set<std::string> UnaryExpression::getVariables() const {
			return this->getOperand()->getVariables();
		}
        
        std::shared_ptr<BaseExpression const> const& UnaryExpression::getOperand() const {
            return this->operand;
        }
        
        uint_fast64_t UnaryExpression::getArity() const {
            return 1;
        }
        
        std::shared_ptr<BaseExpression const> UnaryExpression::getOperand(uint_fast64_t operandIndex) const {
            STORM_LOG_THROW(operandIndex == 0, storm::exceptions::InvalidAccessException, "Unable to access operand " << operandIndex << " in expression of arity 2.");
            return this->getOperand();
        }
    }
}