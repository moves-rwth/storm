#include <map>
#include <unordered_map>
#include <string>

#include "src/storage/expressions/SubstitutionVisitor.h"

#include "src/storage/expressions/IfThenElseExpression.h"
#include "src/storage/expressions/BinaryBooleanFunctionExpression.h"
#include "src/storage/expressions/BinaryNumericalFunctionExpression.h"
#include "src/storage/expressions/BinaryRelationExpression.h"
#include "src/storage/expressions/BooleanConstantExpression.h"
#include "src/storage/expressions/IntegerConstantExpression.h"
#include "src/storage/expressions/DoubleConstantExpression.h"
#include "src/storage/expressions/BooleanLiteralExpression.h"
#include "src/storage/expressions/IntegerLiteralExpression.h"
#include "src/storage/expressions/DoubleLiteralExpression.h"
#include "src/storage/expressions/VariableExpression.h"
#include "src/storage/expressions/UnaryBooleanFunctionExpression.h"
#include "src/storage/expressions/UnaryNumericalFunctionExpression.h"

namespace storm {
    namespace expressions  {
        template<typename MapType>
        SubstitutionVisitor<MapType>::SubstitutionVisitor(MapType const& identifierToExpressionMap) : identifierToExpressionMap(identifierToExpressionMap) {
            // Intentionally left empty.
        }

		template<typename MapType>
        Expression SubstitutionVisitor<MapType>::substitute(BaseExpression const* expression) {
            expression->accept(this);
            return Expression(this->expressionStack.top());
        }
        
		template<typename MapType>
        void SubstitutionVisitor<MapType>::visit(IfThenElseExpression const* expression) {
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
        void SubstitutionVisitor<MapType>::visit(BinaryBooleanFunctionExpression const* expression) {
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
        void SubstitutionVisitor<MapType>::visit(BinaryNumericalFunctionExpression const* expression) {
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
        void SubstitutionVisitor<MapType>::visit(BinaryRelationExpression const* expression) {
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
        void SubstitutionVisitor<MapType>::visit(BooleanConstantExpression const* expression) {
            // If the boolean constant is in the key set of the substitution, we need to replace it.
            auto const& nameExpressionPair = this->identifierToExpressionMap.find(expression->getConstantName());
            if (nameExpressionPair != this->identifierToExpressionMap.end()) {
                this->expressionStack.push(nameExpressionPair->second.getBaseExpressionPointer());
            } else {
                this->expressionStack.push(expression->getSharedPointer());
            }
        }
        
		template<typename MapType>
        void SubstitutionVisitor<MapType>::visit(DoubleConstantExpression const* expression) {
            // If the double constant is in the key set of the substitution, we need to replace it.
            auto const& nameExpressionPair = this->identifierToExpressionMap.find(expression->getConstantName());
            if (nameExpressionPair != this->identifierToExpressionMap.end()) {
                this->expressionStack.push(nameExpressionPair->second.getBaseExpressionPointer());
            } else {
                this->expressionStack.push(expression->getSharedPointer());
            }
        }
        
		template<typename MapType>
        void SubstitutionVisitor<MapType>::visit(IntegerConstantExpression const* expression) {
            // If the integer constant is in the key set of the substitution, we need to replace it.
            auto const& nameExpressionPair = this->identifierToExpressionMap.find(expression->getConstantName());
            if (nameExpressionPair != this->identifierToExpressionMap.end()) {
                this->expressionStack.push(nameExpressionPair->second.getBaseExpressionPointer());
            } else {
                this->expressionStack.push(expression->getSharedPointer());
            }
        }
        
		template<typename MapType>
        void SubstitutionVisitor<MapType>::visit(VariableExpression const* expression) {
            // If the variable is in the key set of the substitution, we need to replace it.
            auto const& nameExpressionPair = this->identifierToExpressionMap.find(expression->getVariableName());
            if (nameExpressionPair != this->identifierToExpressionMap.end()) {
                this->expressionStack.push(nameExpressionPair->second.getBaseExpressionPointer());
            } else {
                this->expressionStack.push(expression->getSharedPointer());
            }
        }
        
		template<typename MapType>
        void SubstitutionVisitor<MapType>::visit(UnaryBooleanFunctionExpression const* expression) {
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
        void SubstitutionVisitor<MapType>::visit(UnaryNumericalFunctionExpression const* expression) {
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
        void SubstitutionVisitor<MapType>::visit(BooleanLiteralExpression const* expression) {
            this->expressionStack.push(expression->getSharedPointer());
        }
        
		template<typename MapType>
        void SubstitutionVisitor<MapType>::visit(IntegerLiteralExpression const* expression) {
            this->expressionStack.push(expression->getSharedPointer());
        }
        
		template<typename MapType>
        void SubstitutionVisitor<MapType>::visit(DoubleLiteralExpression const* expression) {
            this->expressionStack.push(expression->getSharedPointer());
        }
        
        // Explicitly instantiate the class with map and unordered_map.
		template class SubstitutionVisitor< std::map<std::string, Expression> >;
		template class SubstitutionVisitor< std::unordered_map<std::string, Expression> >;
    }
}
