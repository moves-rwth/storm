#include <map>
#include <unordered_map>
#include <string>

#include "src/storage/expressions/IdentifierSubstitutionVisitor.h"
#include "src/storage/expressions/Expressions.h"

namespace storm {
    namespace expressions  {
		template<typename MapType>
        IdentifierSubstitutionVisitor<MapType>::IdentifierSubstitutionVisitor(MapType const& identifierToIdentifierMap) : identifierToIdentifierMap(identifierToIdentifierMap) {
            // Intentionally left empty.
        }
        
		template<typename MapType>
        Expression IdentifierSubstitutionVisitor<MapType>::substitute(BaseExpression const* expression) {
            expression->accept(this);
            return Expression(this->expressionStack.top());
        }
        
		template<typename MapType>
        void IdentifierSubstitutionVisitor<MapType>::visit(IfThenElseExpression const* expression) {
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
        
        template<typename MapType>
        void IdentifierSubstitutionVisitor<MapType>::visit(BinaryBooleanFunctionExpression const* expression) {
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
        
        template<typename MapType>
        void IdentifierSubstitutionVisitor<MapType>::visit(BinaryNumericalFunctionExpression const* expression) {
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
        
        template<typename MapType>
        void IdentifierSubstitutionVisitor<MapType>::visit(BinaryRelationExpression const* expression) {
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
                
        template<typename MapType>
        void IdentifierSubstitutionVisitor<MapType>::visit(VariableExpression const* expression) {
            // If the variable is in the key set of the substitution, we need to replace it.
            auto const& namePair = this->identifierToIdentifierMap.find(expression->getVariableName());
            if (namePair != this->identifierToIdentifierMap.end()) {
                this->expressionStack.push(std::shared_ptr<BaseExpression>(new VariableExpression(expression->getReturnType(), namePair->second)));
            } else {
                this->expressionStack.push(expression->getSharedPointer());
            }
        }
        
        template<typename MapType>
        void IdentifierSubstitutionVisitor<MapType>::visit(UnaryBooleanFunctionExpression const* expression) {
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
        
        template<typename MapType>
        void IdentifierSubstitutionVisitor<MapType>::visit(UnaryNumericalFunctionExpression const* expression) {
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
        
        template<typename MapType>
        void IdentifierSubstitutionVisitor<MapType>::visit(BooleanLiteralExpression const* expression) {
            this->expressionStack.push(expression->getSharedPointer());
        }
        
        template<typename MapType>
        void IdentifierSubstitutionVisitor<MapType>::visit(IntegerLiteralExpression const* expression) {
            this->expressionStack.push(expression->getSharedPointer());
        }
        
        template<typename MapType>
        void IdentifierSubstitutionVisitor<MapType>::visit(DoubleLiteralExpression const* expression) {
            this->expressionStack.push(expression->getSharedPointer());
        }
        
        // Explicitly instantiate the class with map and unordered_map.
		template class IdentifierSubstitutionVisitor< std::map<std::string, std::string> >;
		template class IdentifierSubstitutionVisitor< std::unordered_map<std::string, std::string> >;
    }
}
