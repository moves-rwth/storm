#include "src/storage/expressions/BinaryExpression.h"

namespace storm {
    namespace expressions {
        BinaryExpression::BinaryExpression(ExpressionReturnType returnType, std::unique_ptr<BaseExpression>&& firstOperand, std::unique_ptr<BaseExpression>&& secondOperand) : BaseExpression(returnType), firstOperand(std::move(firstOperand)), secondOperand(std::move(secondOperand)) {
            // Intentionally left empty.
        }
        
        BinaryExpression::BinaryExpression(BinaryExpression const& other) : BaseExpression(other.getReturnType()), firstOperand(other.getFirstOperand()->clone()), secondOperand(other.getSecondOperand()->clone()) {
            // Intentionally left empty.
        }
        
        BinaryExpression& BinaryExpression::operator=(BinaryExpression const& other) {
            if (this != &other) {
                this->firstOperand = other.getFirstOperand()->clone();
                this->secondOperand = other.getSecondOperand()->clone();
            }
            return *this;
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
        
        std::unique_ptr<BaseExpression> const& BinaryExpression::getFirstOperand() const {
            return this->firstOperand;
        }
        
        std::unique_ptr<BaseExpression> const& BinaryExpression::getSecondOperand() const {
            return this->secondOperand;
        }
    }
}