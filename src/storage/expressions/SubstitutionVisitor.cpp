#include <map>
#include <unordered_map>
#include <string>

#include "src/storage/expressions/SubstitutionVisitor.h"
#include "src/storage/expressions/Expressions.h"

namespace storm {
    namespace expressions  {
        template<typename MapType>
        SubstitutionVisitor<MapType>::SubstitutionVisitor(MapType const& identifierToExpressionMap) : identifierToExpressionMap(identifierToExpressionMap) {
            // Intentionally left empty.
        }

		template<typename MapType>
        Expression SubstitutionVisitor<MapType>::substitute(Expression const& expression) {
            return Expression(boost::any_cast<std::shared_ptr<BaseExpression const>>(expression.getBaseExpression().accept(*this)));
        }
        
		template<typename MapType>
        boost::any SubstitutionVisitor<MapType>::visit(IfThenElseExpression const& expression) {
            std::shared_ptr<BaseExpression const> conditionExpression = boost::any_cast<std::shared_ptr<BaseExpression const>>(expression.getCondition()->accept(*this));
            std::shared_ptr<BaseExpression const> thenExpression = boost::any_cast<std::shared_ptr<BaseExpression const>>(expression.getThenExpression()->accept(*this));
            std::shared_ptr<BaseExpression const> elseExpression = boost::any_cast<std::shared_ptr<BaseExpression const>>(expression.getElseExpression()->accept(*this));

            // If the arguments did not change, we simply push the expression itself.
            if (conditionExpression.get() == expression.getCondition().get() && thenExpression.get() == expression.getThenExpression().get() && elseExpression.get() == expression.getElseExpression().get()) {
                return expression.getSharedPointer();
            } else {
                return static_cast<std::shared_ptr<BaseExpression const>>(std::shared_ptr<BaseExpression>(new IfThenElseExpression(expression.getReturnType(), conditionExpression, thenExpression, elseExpression)));
            }
        }
        
		template<typename MapType>
        boost::any SubstitutionVisitor<MapType>::visit(BinaryBooleanFunctionExpression const& expression) {
            std::shared_ptr<BaseExpression const> firstExpression = boost::any_cast<std::shared_ptr<BaseExpression const>>(expression.getFirstOperand()->accept(*this));
            std::shared_ptr<BaseExpression const> secondExpression = boost::any_cast<std::shared_ptr<BaseExpression const>>(expression.getSecondOperand()->accept(*this));

            // If the arguments did not change, we simply push the expression itself.
            if (firstExpression.get() == expression.getFirstOperand().get() && secondExpression.get() == expression.getSecondOperand().get()) {
                return expression.getSharedPointer();
            } else {
                return static_cast<std::shared_ptr<BaseExpression const>>(std::shared_ptr<BaseExpression>(new BinaryBooleanFunctionExpression(expression.getReturnType(), firstExpression, secondExpression, expression.getOperatorType())));
            }
        }
        
		template<typename MapType>
        boost::any SubstitutionVisitor<MapType>::visit(BinaryNumericalFunctionExpression const& expression) {
            std::shared_ptr<BaseExpression const> firstExpression = boost::any_cast<std::shared_ptr<BaseExpression const>>(expression.getFirstOperand()->accept(*this));
            std::shared_ptr<BaseExpression const> secondExpression = boost::any_cast<std::shared_ptr<BaseExpression const>>(expression.getSecondOperand()->accept(*this));
            
            // If the arguments did not change, we simply push the expression itself.
            if (firstExpression.get() == expression.getFirstOperand().get() && secondExpression.get() == expression.getSecondOperand().get()) {
                return expression.getSharedPointer();
            } else {
                return static_cast<std::shared_ptr<BaseExpression const>>(std::shared_ptr<BaseExpression>(new BinaryNumericalFunctionExpression(expression.getReturnType(), firstExpression, secondExpression, expression.getOperatorType())));
            }
        }
        
		template<typename MapType>
        boost::any SubstitutionVisitor<MapType>::visit(BinaryRelationExpression const& expression) {
            std::shared_ptr<BaseExpression const> firstExpression = boost::any_cast<std::shared_ptr<BaseExpression const>>(expression.getFirstOperand()->accept(*this));
            std::shared_ptr<BaseExpression const> secondExpression = boost::any_cast<std::shared_ptr<BaseExpression const>>(expression.getSecondOperand()->accept(*this));
            
            // If the arguments did not change, we simply push the expression itself.
            if (firstExpression.get() == expression.getFirstOperand().get() && secondExpression.get() == expression.getSecondOperand().get()) {
                return expression.getSharedPointer();
            } else {
                return static_cast<std::shared_ptr<BaseExpression const>>(std::shared_ptr<BaseExpression>(new BinaryRelationExpression(expression.getReturnType(), firstExpression, secondExpression, expression.getRelationType())));
            }
        }
        
		template<typename MapType>
        boost::any SubstitutionVisitor<MapType>::visit(VariableExpression const& expression) {
            // If the variable is in the key set of the substitution, we need to replace it.
            auto const& nameExpressionPair = this->identifierToExpressionMap.find(expression.getVariableName());
            if (nameExpressionPair != this->identifierToExpressionMap.end()) {
                return nameExpressionPair->second.getBaseExpressionPointer();
            } else {
                return expression.getSharedPointer();
            }
        }
        
		template<typename MapType>
        boost::any SubstitutionVisitor<MapType>::visit(UnaryBooleanFunctionExpression const& expression) {
            std::shared_ptr<BaseExpression const> operandExpression = boost::any_cast<std::shared_ptr<BaseExpression const>>(expression.getOperand()->accept(*this));
            
            // If the argument did not change, we simply push the expression itself.
            if (operandExpression.get() == expression.getOperand().get()) {
                return expression.getSharedPointer();
            } else {
                return static_cast<std::shared_ptr<BaseExpression const>>(std::shared_ptr<BaseExpression>(new UnaryBooleanFunctionExpression(expression.getReturnType(), operandExpression, expression.getOperatorType())));
            }
        }
        
		template<typename MapType>
        boost::any SubstitutionVisitor<MapType>::visit(UnaryNumericalFunctionExpression const& expression) {
            std::shared_ptr<BaseExpression const> operandExpression = boost::any_cast<std::shared_ptr<BaseExpression const>>(expression.getOperand()->accept(*this));
            
            // If the argument did not change, we simply push the expression itself.
            if (operandExpression.get() == expression.getOperand().get()) {
                return expression.getSharedPointer();
            } else {
                return static_cast<std::shared_ptr<BaseExpression const>>(std::shared_ptr<BaseExpression>(new UnaryNumericalFunctionExpression(expression.getReturnType(), operandExpression, expression.getOperatorType())));
            }
        }
        
		template<typename MapType>
        boost::any SubstitutionVisitor<MapType>::visit(BooleanLiteralExpression const& expression) {
            return expression.getSharedPointer();
        }
        
		template<typename MapType>
        boost::any SubstitutionVisitor<MapType>::visit(IntegerLiteralExpression const& expression) {
            return expression.getSharedPointer();
        }
        
		template<typename MapType>
        boost::any SubstitutionVisitor<MapType>::visit(DoubleLiteralExpression const& expression) {
            return expression.getSharedPointer();
        }
        
        // Explicitly instantiate the class with map and unordered_map.
		template class SubstitutionVisitor<std::map<std::string, Expression>>;
		template class SubstitutionVisitor<std::unordered_map<std::string, Expression>>;
    }
}
