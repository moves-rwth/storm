#include "src/storage/expressions/BinaryExpression.h"

namespace storm {
    namespace expressions {
        BinaryExpression::BinaryExpression(ExpressionReturnType returnType, std::shared_ptr<BaseExpression const> const& firstOperand, std::shared_ptr<BaseExpression const> const& secondOperand) : BaseExpression(returnType), firstOperand(firstOperand), secondOperand(secondOperand) {
            // Intentionally left empty.
        }
                
        bool BinaryExpression::isConstant() const {
            return this->getFirstOperand()->isConstant() && this->getSecondOperand()->isConstant();
        }
        
        std::set<std::string> BinaryExpression::getVariables() const {
            std::set<std::string> firstVariableSet = this->getFirstOperand()->getVariables();
            std::set<std::string> secondVariableSet = this->getSecondOperand()->getVariables();
            firstVariableSet.insert(secondVariableSet.begin(), secondVariableSet.end());
            return firstVariableSet;
        }
        
        std::set<std::string> BinaryExpression::getConstants() const {
            std::set<std::string> firstConstantSet = this->getFirstOperand()->getVariables();
            std::set<std::string> secondConstantSet = this->getSecondOperand()->getVariables();
            firstConstantSet.insert(secondConstantSet.begin(), secondConstantSet.end());
            return firstConstantSet;
        }
        
        std::shared_ptr<BaseExpression const> const& BinaryExpression::getFirstOperand() const {
            return this->firstOperand;
        }
        
        std::shared_ptr<BaseExpression const> const& BinaryExpression::getSecondOperand() const {
            return this->secondOperand;
        }
    }
}