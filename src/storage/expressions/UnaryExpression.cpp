#include "src/storage/expressions/UnaryExpression.h"

namespace storm {
    namespace expressions {
        UnaryExpression::UnaryExpression(ExpressionReturnType returnType, std::shared_ptr<BaseExpression const> const& operand) : BaseExpression(returnType), operand(operand) {
            // Intentionally left empty.
        }

        bool UnaryExpression::isConstant() const {
            return this->getOperand()->isConstant();
        }
        
        std::set<std::string> UnaryExpression::getVariables() const {
            return this->getOperand()->getVariables();
        }
        
        std::set<std::string> UnaryExpression::getConstants() const {
            return this->getOperand()->getVariables();
        }
        
        std::shared_ptr<BaseExpression const> const& UnaryExpression::getOperand() const {
            return this->operand;
        }
    }
}