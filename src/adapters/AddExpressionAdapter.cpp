#include "src/adapters/AddExpressionAdapter.h"

#include "src/utility/macros.h"
#include "src/exceptions/ExpressionEvaluationException.h"
#include "src/exceptions/InvalidArgumentException.h"

#include "src/storage/dd/DdManager.h"
#include "src/storage/dd/Add.h"
#include "src/storage/dd/Bdd.h"

namespace storm {
    namespace adapters {
        
        template<storm::dd::DdType Type, typename ValueType>
        AddExpressionAdapter<Type, ValueType>::AddExpressionAdapter(std::shared_ptr<storm::dd::DdManager<Type>> ddManager, std::map<storm::expressions::Variable, storm::expressions::Variable> const& variableMapping) : ddManager(ddManager), variableMapping(variableMapping) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        storm::dd::Add<Type> AddExpressionAdapter<Type, ValueType>::translateExpression(storm::expressions::Expression const& expression) {
            if (expression.hasBooleanType()) {
                return boost::any_cast<storm::dd::Bdd<Type>>(expression.accept(*this)).template toAdd<ValueType>();
            } else {
                return boost::any_cast<storm::dd::Add<Type>>(expression.accept(*this));
            }
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        boost::any AddExpressionAdapter<Type, ValueType>::visit(storm::expressions::IfThenElseExpression const& expression) {
            storm::dd::Add<Type> elseDd = boost::any_cast<storm::dd::Add<Type>>(expression.getElseExpression()->accept(*this));
            storm::dd::Add<Type> thenDd = boost::any_cast<storm::dd::Add<Type>>(expression.getThenExpression()->accept(*this));
            storm::dd::Add<Type> conditionDd = boost::any_cast<storm::dd::Add<Type>>(expression.getCondition()->accept(*this));
            return conditionDd.ite(thenDd, elseDd);
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        boost::any AddExpressionAdapter<Type, ValueType>::visit(storm::expressions::BinaryBooleanFunctionExpression const& expression) {
            storm::dd::Bdd<Type> leftResult = boost::any_cast<storm::dd::Bdd<Type>>(expression.getFirstOperand()->accept(*this));
            storm::dd::Bdd<Type> rightResult = boost::any_cast<storm::dd::Bdd<Type>>(expression.getSecondOperand()->accept(*this));
            
            storm::dd::Bdd<Type> result;
            switch (expression.getOperatorType()) {
                case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::And:
                    result = (leftResult && rightResult);
                    break;
                case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::Or:
                    result = (leftResult || rightResult);
                    break;
                case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::Iff:
                    result = (leftResult.iff(rightResult));
                    break;
                case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::Implies:
                    result = (!leftResult || rightResult);
                    break;
                case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::Xor:
                    result = (leftResult.exclusiveOr(rightResult));
                    break;
            }
            
            return result;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        boost::any AddExpressionAdapter<Type, ValueType>::visit(storm::expressions::BinaryNumericalFunctionExpression const& expression) {
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
                case storm::expressions::BinaryNumericalFunctionExpression::OperatorType::Power:
                    result = leftResult.pow(rightResult);
                    break;
                default:
                    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Cannot translate expression containing power operator.");
            }
            
            return result;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        boost::any AddExpressionAdapter<Type, ValueType>::visit(storm::expressions::BinaryRelationExpression const& expression) {
            storm::dd::Add<Type> leftResult = boost::any_cast<storm::dd::Add<Type>>(expression.getFirstOperand()->accept(*this));
            storm::dd::Add<Type> rightResult = boost::any_cast<storm::dd::Add<Type>>(expression.getSecondOperand()->accept(*this));

            storm::dd::Bdd<Type> result;
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
        
        template<storm::dd::DdType Type, typename ValueType>
        boost::any AddExpressionAdapter<Type, ValueType>::visit(storm::expressions::VariableExpression const& expression) {
            auto const& variablePair = variableMapping.find(expression.getVariable());
            STORM_LOG_THROW(variablePair != variableMapping.end(), storm::exceptions::InvalidArgumentException, "Cannot translate the given expression, because it contains the variable '" << expression.getVariableName() << "' for which no DD counterpart is known.");
            if (expression.hasBooleanType()) {
                return ddManager->template getIdentity<ValueType>(variablePair->second).toBdd();
            } else {
                return ddManager->template getIdentity<ValueType>(variablePair->second);
            }
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        boost::any AddExpressionAdapter<Type, ValueType>::visit(storm::expressions::UnaryBooleanFunctionExpression const& expression) {
            storm::dd::Bdd<Type> result = boost::any_cast<storm::dd::Bdd<Type>>(expression.getOperand()->accept(*this));
            
            switch (expression.getOperatorType()) {
                case storm::expressions::UnaryBooleanFunctionExpression::OperatorType::Not:
                    result = !result;
                    break;
            }
            
            return result;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        boost::any AddExpressionAdapter<Type, ValueType>::visit(storm::expressions::UnaryNumericalFunctionExpression const& expression) {
            storm::dd::Add<Type> result = boost::any_cast<storm::dd::Add<Type>>(expression.getOperand()->accept(*this));
            
            switch (expression.getOperatorType()) {
                case storm::expressions::UnaryNumericalFunctionExpression::OperatorType::Minus:
                    result = -result;
                    break;
                case storm::expressions::UnaryNumericalFunctionExpression::OperatorType::Floor:
                    result = result.floor();
                    break;
                case storm::expressions::UnaryNumericalFunctionExpression::OperatorType::Ceil:
                    result = result.ceil();
                    break;
                default:
                    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Cannot translate expression containing floor/ceil operator.");
            }
            
            return result;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        boost::any AddExpressionAdapter<Type, ValueType>::visit(storm::expressions::BooleanLiteralExpression const& expression) {
            return expression.getValue() ? ddManager->getBddOne() : ddManager->getBddZero();
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        boost::any AddExpressionAdapter<Type, ValueType>::visit(storm::expressions::IntegerLiteralExpression const& expression) {
            return ddManager->getConstant(static_cast<ValueType>(expression.getValue()));
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        boost::any AddExpressionAdapter<Type, ValueType>::visit(storm::expressions::DoubleLiteralExpression const& expression) {
            return ddManager->getConstant(static_cast<ValueType>(expression.getValue()));
        }
        
        // Explicitly instantiate the symbolic expression adapter
        template class AddExpressionAdapter<storm::dd::DdType::CUDD, double>;
        
    } // namespace adapters
} // namespace storm
