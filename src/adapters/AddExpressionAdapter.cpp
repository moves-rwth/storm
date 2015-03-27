#include "src/adapters/AddExpressionAdapter.h"

#include "src/utility/macros.h"
#include "src/exceptions/ExpressionEvaluationException.h"

#include "src/storage/dd/CuddDdManager.h"

namespace storm {
    namespace adapters {
        
        template<storm::dd::DdType Type>
        AddExpressionAdapter<Type>::AddExpressionAdapter(std::shared_ptr<storm::dd::DdManager<Type>> ddManager, std::map<storm::expressions::Variable, storm::expressions::Variable> const& variableMapping) : ddManager(ddManager), variableMapping(variableMapping) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType Type>
        storm::dd::Add<Type> AddExpressionAdapter<Type>::translateExpression(storm::expressions::Expression const& expression) {
            return boost::any_cast<storm::dd::Add<Type>>(expression.accept(*this));
        }
        
        template<storm::dd::DdType Type>
        boost::any AddExpressionAdapter<Type>::visit(storm::expressions::IfThenElseExpression const& expression) {
            storm::dd::Add<Type> elseDd = boost::any_cast<storm::dd::Add<Type>>(expression.getElseExpression()->accept(*this));
            storm::dd::Add<Type> thenDd = boost::any_cast<storm::dd::Add<Type>>(expression.getThenExpression()->accept(*this));
            storm::dd::Add<Type> conditionDd = boost::any_cast<storm::dd::Add<Type>>(expression.getCondition()->accept(*this));
            return conditionDd.ite(thenDd, elseDd);
        }
        
        template<storm::dd::DdType Type>
        boost::any AddExpressionAdapter<Type>::visit(storm::expressions::BinaryBooleanFunctionExpression const& expression) {
            storm::dd::Bdd<Type> leftResult = boost::any_cast<storm::dd::Add<Type>>(expression.getFirstOperand()->accept(*this)).toBdd();
            storm::dd::Bdd<Type> rightResult = boost::any_cast<storm::dd::Add<Type>>(expression.getSecondOperand()->accept(*this)).toBdd();
            
            storm::dd::Add<Type> result;
            switch (expression.getOperatorType()) {
                case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::And:
                    result = (leftResult && rightResult).toAdd();
                    break;
                case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::Or:
                    result = (leftResult || rightResult).toAdd();
                    break;
                case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::Iff:
                    result = (leftResult.iff(rightResult)).toAdd();
                    break;
                case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::Implies:
                    result = (!leftResult || rightResult).toAdd();
                    break;
                case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::Xor:
                    result = (leftResult.exclusiveOr(rightResult)).toAdd();
                    break;
            }
            
            return result;
        }
        
        template<storm::dd::DdType Type>
        boost::any AddExpressionAdapter<Type>::visit(storm::expressions::BinaryNumericalFunctionExpression const& expression) {
            storm::dd::Add<Type> leftResult = boost::any_cast<storm::dd::Add<Type>>(expression.getFirstOperand()->accept(*this));
            storm::dd::Add<Type> rightResult = boost::any_cast<storm::dd::Add<Type>>(expression.getSecondOperand()->accept(*this));
            
            storm::dd::Add<Type> result;
            switch (expression.getOperatorType()) {
                case storm::expressions::BinaryNumericalFunctionExpression::OperatorType::Plus:
                    result = leftResult + rightResult;
                    break;
                case storm::expressions::BinaryNumericalFunctionExpression::OperatorType::Minus:
                    result = leftResult - rightResult;
                    break;
                case storm::expressions::BinaryNumericalFunctionExpression::OperatorType::Times:
                    result = leftResult * rightResult;
                    break;
                case storm::expressions::BinaryNumericalFunctionExpression::OperatorType::Divide:
                    result = leftResult / rightResult;
                    break;
                case storm::expressions::BinaryNumericalFunctionExpression::OperatorType::Max:
                    result = leftResult.maximum(rightResult);
                    break;
                case storm::expressions::BinaryNumericalFunctionExpression::OperatorType::Min:
                    result = leftResult.minimum(rightResult);
                    break;
                default:
                    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Cannot translate expression containing power operator.");
            }
            
            return result;
        }
        
        template<storm::dd::DdType Type>
        boost::any AddExpressionAdapter<Type>::visit(storm::expressions::BinaryRelationExpression const& expression) {
            storm::dd::Add<Type> leftResult = boost::any_cast<storm::dd::Add<Type>>(expression.getFirstOperand()->accept(*this));
            storm::dd::Add<Type> rightResult = boost::any_cast<storm::dd::Add<Type>>(expression.getSecondOperand()->accept(*this));

            storm::dd::Add<Type> result;
            switch (expression.getRelationType()) {
                case storm::expressions::BinaryRelationExpression::RelationType::Equal:
                    result = leftResult.equals(rightResult);
                    break;
                case storm::expressions::BinaryRelationExpression::RelationType::NotEqual:
                    result = leftResult.notEquals(rightResult);
                    break;
                case storm::expressions::BinaryRelationExpression::RelationType::Less:
                    result = leftResult.less(rightResult);
                    break;
                case storm::expressions::BinaryRelationExpression::RelationType::LessOrEqual:
                    result = leftResult.lessOrEqual(rightResult);
                    break;
                case storm::expressions::BinaryRelationExpression::RelationType::Greater:
                    result = leftResult.greater(rightResult);
                    break;
                case storm::expressions::BinaryRelationExpression::RelationType::GreaterOrEqual:
                    result = leftResult.greaterOrEqual(rightResult);
                    break;
            }
            
            return result;
        }
        
        template<storm::dd::DdType Type>
        boost::any AddExpressionAdapter<Type>::visit(storm::expressions::VariableExpression const& expression) {
            auto const& variablePair = variableMapping.find(expression.getVariable());
            STORM_LOG_THROW(variablePair != variableMapping.end(), storm::exceptions::InvalidArgumentException, "Cannot translate the given expression, because it contains the variable '" << expression.getVariableName() << "' for which no DD counterpart is known.");
            return ddManager->getIdentity(variablePair->second);
        }
        
        template<storm::dd::DdType Type>
        boost::any AddExpressionAdapter<Type>::visit(storm::expressions::UnaryBooleanFunctionExpression const& expression) {
            storm::dd::Bdd<Type> result = boost::any_cast<storm::dd::Add<Type>>(expression.getOperand()->accept(*this)).toBdd();
            
            switch (expression.getOperatorType()) {
                case storm::expressions::UnaryBooleanFunctionExpression::OperatorType::Not:
                    result = !result;
                    break;
            }
            
            return result.toAdd();
        }
        
        template<storm::dd::DdType Type>
        boost::any AddExpressionAdapter<Type>::visit(storm::expressions::UnaryNumericalFunctionExpression const& expression) {
            storm::dd::Add<Type> result = boost::any_cast<storm::dd::Add<Type>>(expression.getOperand()->accept(*this));
            
            switch (expression.getOperatorType()) {
                case storm::expressions::UnaryNumericalFunctionExpression::OperatorType::Minus:
                    result = -result;
                    break;
                default:
                    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Cannot translate expression containing floor/ceil operator.");
            }
            
            return result;
        }
        
        template<storm::dd::DdType Type>
        boost::any AddExpressionAdapter<Type>::visit(storm::expressions::BooleanLiteralExpression const& expression) {
            return ddManager->getConstant(expression.getValue());
        }
        
        template<storm::dd::DdType Type>
        boost::any AddExpressionAdapter<Type>::visit(storm::expressions::IntegerLiteralExpression const& expression) {
            return ddManager->getConstant(expression.getValue());
        }
        
        template<storm::dd::DdType Type>
        boost::any AddExpressionAdapter<Type>::visit(storm::expressions::DoubleLiteralExpression const& expression) {
            return ddManager->getConstant(expression.getValue());
        }
        
        // Explicitly instantiate the symbolic expression adapter
        template class AddExpressionAdapter<storm::dd::DdType::CUDD>;
        
    } // namespace adapters
} // namespace storm
