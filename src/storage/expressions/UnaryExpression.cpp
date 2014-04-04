#include "src/storage/expressions/UnaryExpression.h"

namespace storm {
    namespace expressions {
        UnaryExpression::UnaryExpression(ExpressionReturnType returnType, std::unique_ptr<BaseExpression>&& operand) : BaseExpression(returnType), operand(std::move(operand)) {
            // Intentionally left empty.
        }
        
        UnaryExpression::UnaryExpression(UnaryExpression const& other) : BaseExpression(other), operand(other.getOperand()->clone()) {
            // Intentionally left empty.
        }
        
        UnaryExpression& UnaryExpression::operator=(UnaryExpression const& other) {
            if (this != &other) {
                BaseExpression::operator=(other);
                this->operand = other.getOperand()->clone();
            }
            return *this;
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
    }
}