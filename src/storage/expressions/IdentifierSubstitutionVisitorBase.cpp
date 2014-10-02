#include <string>

#include "src/storage/expressions/IdentifierSubstitutionVisitorBase.h"
#include "src/storage/expressions/Expressions.h"

namespace storm {
    namespace expressions  {
        Expression IdentifierSubstitutionVisitorBase::substitute(Expression const& expression) {
            expression.getBaseExpression().accept(this);
            return Expression(this->expressionStack.top());
        }
        
        void IdentifierSubstitutionVisitorBase::visit(IfThenElseExpression const* expression) {
            expression->getCondition()->accept(this);
            std::shared_ptr<BaseExpression const> conditionExpression = expressionStack.top();
            expressionStack.pop();
            
            expression->getThenExpression()->accept(this);
            std::shared_ptr<BaseExpression const> thenExpression = expressionStack.top();
            expressionStack.pop();
            
            expression->getElseExpression()->accept(this);
            std::shared_ptr<BaseExpression const> elseExpression = expressionStack.top();
            expressionStack.pop();
            
            // If the arguments did not change, we simply push the expression itself.
            if (conditionExpression.get() == expression->getCondition().get() && thenExpression.get() == expression->getThenExpression().get() && elseExpression.get() == expression->getElseExpression().get()) {
                this->expressionStack.push(expression->getSharedPointer());
            } else {
                this->expressionStack.push(std::shared_ptr<BaseExpression>(new IfThenElseExpression(expression->getReturnType(), conditionExpression, thenExpression, elseExpression)));
            }
        }
        
        void IdentifierSubstitutionVisitorBase::visit(BinaryBooleanFunctionExpression const* expression) {
            expression->getFirstOperand()->accept(this);
            std::shared_ptr<BaseExpression const> firstExpression = expressionStack.top();
            expressionStack.pop();
            
            expression->getSecondOperand()->accept(this);
            std::shared_ptr<BaseExpression const> secondExpression = expressionStack.top();
            expressionStack.pop();
            
            // If the arguments did not change, we simply push the expression itself.
            if (firstExpression.get() == expression->getFirstOperand().get() && secondExpression.get() == expression->getSecondOperand().get()) {
                this->expressionStack.push(expression->getSharedPointer());
            } else {
                this->expressionStack.push(std::shared_ptr<BaseExpression>(new BinaryBooleanFunctionExpression(expression->getReturnType(), firstExpression, secondExpression, expression->getOperatorType())));
            }
        }
        
        void IdentifierSubstitutionVisitorBase::visit(BinaryNumericalFunctionExpression const* expression) {
            expression->getFirstOperand()->accept(this);
            std::shared_ptr<BaseExpression const> firstExpression = expressionStack.top();
            expressionStack.pop();
            
            expression->getSecondOperand()->accept(this);
            std::shared_ptr<BaseExpression const> secondExpression = expressionStack.top();
            expressionStack.pop();
            
            // If the arguments did not change, we simply push the expression itself.
            if (firstExpression.get() == expression->getFirstOperand().get() && secondExpression.get() == expression->getSecondOperand().get()) {
                this->expressionStack.push(expression->getSharedPointer());
            } else {
                this->expressionStack.push(std::shared_ptr<BaseExpression>(new BinaryNumericalFunctionExpression(expression->getReturnType(), firstExpression, secondExpression, expression->getOperatorType())));
            }
        }
        
        void IdentifierSubstitutionVisitorBase::visit(BinaryRelationExpression const* expression) {
            expression->getFirstOperand()->accept(this);
            std::shared_ptr<BaseExpression const> firstExpression = expressionStack.top();
            expressionStack.pop();
            
            expression->getSecondOperand()->accept(this);
            std::shared_ptr<BaseExpression const> secondExpression = expressionStack.top();
            expressionStack.pop();
            
            // If the arguments did not change, we simply push the expression itself.
            if (firstExpression.get() == expression->getFirstOperand().get() && secondExpression.get() == expression->getSecondOperand().get()) {
                this->expressionStack.push(expression->getSharedPointer());
            } else {
                this->expressionStack.push(std::shared_ptr<BaseExpression>(new BinaryRelationExpression(expression->getReturnType(), firstExpression, secondExpression, expression->getRelationType())));
            }
        }
        
        void IdentifierSubstitutionVisitorBase::visit(UnaryBooleanFunctionExpression const* expression) {
            expression->getOperand()->accept(this);
            std::shared_ptr<BaseExpression const> operandExpression = expressionStack.top();
            expressionStack.pop();
            
            // If the argument did not change, we simply push the expression itself.
            if (operandExpression.get() == expression->getOperand().get()) {
                expressionStack.push(expression->getSharedPointer());
            } else {
                expressionStack.push(std::shared_ptr<BaseExpression>(new UnaryBooleanFunctionExpression(expression->getReturnType(), operandExpression, expression->getOperatorType())));
            }
        }
        
        void IdentifierSubstitutionVisitorBase::visit(UnaryNumericalFunctionExpression const* expression) {
            expression->getOperand()->accept(this);
            std::shared_ptr<BaseExpression const> operandExpression = expressionStack.top();
            expressionStack.pop();
            
            // If the argument did not change, we simply push the expression itself.
            if (operandExpression.get() == expression->getOperand().get()) {
                expressionStack.push(expression->getSharedPointer());
            } else {
                expressionStack.push(std::shared_ptr<BaseExpression>(new UnaryNumericalFunctionExpression(expression->getReturnType(), operandExpression, expression->getOperatorType())));
            }
        }
        
        void IdentifierSubstitutionVisitorBase::visit(BooleanLiteralExpression const* expression) {
            this->expressionStack.push(expression->getSharedPointer());
        }
        
        void IdentifierSubstitutionVisitorBase::visit(IntegerLiteralExpression const* expression) {
            this->expressionStack.push(expression->getSharedPointer());
        }
        
        void IdentifierSubstitutionVisitorBase::visit(DoubleLiteralExpression const* expression) {
            this->expressionStack.push(expression->getSharedPointer());
        }
    }
}
