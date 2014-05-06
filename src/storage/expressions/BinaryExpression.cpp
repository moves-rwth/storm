#include "src/storage/expressions/BinaryExpression.h"

#include "src/exceptions/ExceptionMacros.h"
#include "src/exceptions/InvalidAccessException.h"

namespace storm {
    namespace expressions {
        BinaryExpression::BinaryExpression(ExpressionReturnType returnType, std::shared_ptr<BaseExpression const> const& firstOperand, std::shared_ptr<BaseExpression const> const& secondOperand) : BaseExpression(returnType), firstOperand(firstOperand), secondOperand(secondOperand) {
            // Intentionally left empty.
        }
        
        bool BinaryExpression::containsVariables() const {
            return this->getFirstOperand()->containsVariables() || this->getSecondOperand()->containsVariables();
		}

		std::set<std::string> BinaryExpression::getVariables() const {
			std::set<std::string> firstVariableSet = this->getFirstOperand()->getVariables();
			std::set<std::string> secondVariableSet = this->getSecondOperand()->getVariables();
			firstVariableSet.insert(secondVariableSet.begin(), secondVariableSet.end());
			return firstVariableSet;
		}

		std::map<std::string, ExpressionReturnType> BinaryExpression::getVariablesAndTypes() const {
			std::map<std::string, ExpressionReturnType> firstVariableSet = this->getFirstOperand()->getVariablesAndTypes();
			std::map<std::string, ExpressionReturnType> secondVariableSet = this->getSecondOperand()->getVariablesAndTypes();
			firstVariableSet.insert(secondVariableSet.begin(), secondVariableSet.end());
			return firstVariableSet;
		}
        
        std::shared_ptr<BaseExpression const> const& BinaryExpression::getFirstOperand() const {
            return this->firstOperand;
        }
        
        std::shared_ptr<BaseExpression const> const& BinaryExpression::getSecondOperand() const {
            return this->secondOperand;
        }
        
        uint_fast64_t BinaryExpression::getArity() const {
            return 2;
        }
        
        std::shared_ptr<BaseExpression const> BinaryExpression::getOperand(uint_fast64_t operandIndex) const {
            LOG_THROW(operandIndex < 2, storm::exceptions::InvalidAccessException, "Unable to access operand " << operandIndex << " in expression of arity 2.");
            if (operandIndex == 1) {
                return this->getFirstOperand();
            } else {
                return this->getSecondOperand();
            }
        }
    }
}