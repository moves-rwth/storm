#include "src/adapters/DdExpressionAdapter.h"

#include "src/utility/macros.h"
#include "src/exceptions/ExpressionEvaluationException.h"

#include "src/storage/dd/CuddDdManager.h"

namespace storm {
    namespace adapters {
        
        template<storm::dd::DdType Type>
        SymbolicExpressionAdapter<Type>::SymbolicExpressionAdapter(storm::dd::DdManager<Type> const& ddManager, std::map<storm::expressions::Variable, storm::expressions::Variable> const& variableMapping) : ddManager(ddManager), variableMapping(variableMapping) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType Type>
        storm::dd::Dd<Type> SymbolicExpressionAdapter<Type>::translateExpression(storm::expressions::Expression const& expression) {
            return boost::any_cast<storm::dd::Dd<Type>>(expression.accept(*this));
        }
        
        template<storm::dd::DdType Type>
        boost::any SymbolicExpressionAdapter<Type>::visit(storm::expressions::IfThenElseExpression const& expression) {
            storm::dd::Dd<Type> elseDd = boost::any_cast<storm::dd::Dd<Type>>(expression.getElseExpression()->accept(*this));
            storm::dd::Dd<Type> thenDd = boost::any_cast<storm::dd::Dd<Type>>(expression.getThenExpression()->accept(*this));
            storm::dd::Dd<Type> conditionDd = boost::any_cast<storm::dd::Dd<Type>>(expression.getCondition()->accept(*this));
            return conditionDd.ite(thenDd, elseDd);
        }
        
        template<storm::dd::DdType Type>
        boost::any SymbolicExpressionAdapter<Type>::visit(storm::expressions::BinaryBooleanFunctionExpression const& expression) {
            storm::dd::Dd<Type> leftResult = boost::any_cast<storm::dd::Dd<Type>>(expression.getFirstOperand()->accept(*this));
            storm::dd::Dd<Type> rightResult = boost::any_cast<storm::dd::Dd<Type>>(expression.getSecondOperand()->accept(*this));
            
            storm::dd::Dd<Type> result;
            switch (expression.getOperatorType()) {
                case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::And:
                    result = leftResult && rightResult;
                    break;
                case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::Or:
                    result = leftResult || rightResult;
                    break;
                case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::Iff:
                    result = leftResult.equals(rightResult);
                    break;
                case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::Implies:
                    result = !leftResult || rightResult;
                    break;
                case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::Xor:
                    result = (leftResult && !rightResult) || (!leftResult && rightResult);
                    break;
            }
            
            return result;
        }
        
        template<storm::dd::DdType Type>
        boost::any SymbolicExpressionAdapter<Type>::visit(storm::expressions::BinaryNumericalFunctionExpression const& expression) {
            storm::dd::Dd<Type> leftResult = boost::any_cast<storm::dd::Dd<Type>>(expression.getFirstOperand()->accept(*this));
            storm::dd::Dd<Type> rightResult = boost::any_cast<storm::dd::Dd<Type>>(expression.getSecondOperand()->accept(*this));
            
            storm::dd::Dd<Type> result;
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
        boost::any SymbolicExpressionAdapter<Type>::visit(storm::expressions::BinaryRelationExpression const& expression) {
            storm::dd::Dd<Type> leftResult = boost::any_cast<storm::dd::Dd<Type>>(expression.getFirstOperand()->accept(*this));
            storm::dd::Dd<Type> rightResult = boost::any_cast<storm::dd::Dd<Type>>(expression.getSecondOperand()->accept(*this));
            
            storm::dd::Dd<Type> result;
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
        boost::any SymbolicExpressionAdapter<Type>::visit(storm::expressions::VariableExpression const& expression) {
            auto const& variablePair = variableMapping.find(expression.getVariable());
            STORM_LOG_THROW(variablePair != variableMapping.end(), storm::exceptions::InvalidArgumentException, "Cannot translate the given expression, because it contains th variable '" << expression.getVariableName() << "' for which no DD counterpart is known.");
            return ddManager.getIdentity(variablePair->second);
        }
        
        template<storm::dd::DdType Type>
        boost::any SymbolicExpressionAdapter<Type>::visit(storm::expressions::UnaryBooleanFunctionExpression const& expression) {
            storm::dd::Dd<Type> result = boost::any_cast<storm::dd::Dd<Type>>(expression.getOperand()->accept(*this));
            
            switch (expression.getOperatorType()) {
                case storm::expressions::UnaryBooleanFunctionExpression::OperatorType::Not:
                    result = !result;
                    break;
            }
            
            return result;
        }
        
        template<storm::dd::DdType Type>
        boost::any SymbolicExpressionAdapter<Type>::visit(storm::expressions::UnaryNumericalFunctionExpression const& expression) {
            storm::dd::Dd<Type> result = boost::any_cast<storm::dd::Dd<Type>>(expression.getOperand()->accept(*this));
            
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
        boost::any SymbolicExpressionAdapter<Type>::visit(storm::expressions::BooleanLiteralExpression const& expression) {
            return ddManager.getConstant(expression.getValue());
        }
        
        template<storm::dd::DdType Type>
        boost::any SymbolicExpressionAdapter<Type>::visit(storm::expressions::IntegerLiteralExpression const& expression) {
            return ddManager.getConstant(expression.getValue());
        }
        
        template<storm::dd::DdType Type>
        boost::any SymbolicExpressionAdapter<Type>::visit(storm::expressions::DoubleLiteralExpression const& expression) {
            return ddManager.getConstant(expression.getValue());
        }
        
        // Explicitly instantiate the symbolic expression adapter
        template class SymbolicExpressionAdapter<storm::dd::DdType::CUDD>;
        
    } // namespace adapters
} // namespace storm
