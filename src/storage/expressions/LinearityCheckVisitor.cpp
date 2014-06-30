#include "src/storage/expressions/LinearityCheckVisitor.h"
#include "src/storage/expressions/Expressions.h"

#include "src/exceptions/ExceptionMacros.h"
#include "src/exceptions/InvalidTypeException.h"

namespace storm {
    namespace expressions {
        LinearityCheckVisitor::LinearityCheckVisitor() : resultStack() {
            // Intentionally left empty.
        }
        
        bool LinearityCheckVisitor::check(Expression const& expression) {
            expression.getBaseExpression().accept(this);
            return resultStack.top() == LinearityStatus::LinearWithoutVariables || resultStack.top() == LinearityStatus::LinearContainsVariables;
        }
        
        void LinearityCheckVisitor::visit(IfThenElseExpression const* expression) {
            // An if-then-else expression is never linear.
            resultStack.push(LinearityStatus::NonLinear);
        }
        
        void LinearityCheckVisitor::visit(BinaryBooleanFunctionExpression const* expression) {
            // Boolean function applications are not allowed in linear expressions.
            resultStack.push(LinearityStatus::NonLinear);
        }
        
        void LinearityCheckVisitor::visit(BinaryNumericalFunctionExpression const* expression) {
            LinearityStatus leftResult;
            LinearityStatus rightResult;
            switch (expression->getOperatorType()) {
                case BinaryNumericalFunctionExpression::OperatorType::Plus:
                case BinaryNumericalFunctionExpression::OperatorType::Minus:
                    expression->getFirstOperand()->accept(this);
                    leftResult = resultStack.top();
                    
                    if (leftResult == LinearityStatus::NonLinear) {
                        return;
                    } else {
                        resultStack.pop();
                        expression->getSecondOperand()->accept(this);
                        rightResult = resultStack.top();
                        if (rightResult == LinearityStatus::NonLinear) {
                            return;
                        }
                        resultStack.pop();
                    }
                                        
                    resultStack.push(leftResult == LinearityStatus::LinearContainsVariables || rightResult == LinearityStatus::LinearContainsVariables ? LinearityStatus::LinearContainsVariables : LinearityStatus::LinearWithoutVariables);
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Times:
                case BinaryNumericalFunctionExpression::OperatorType::Divide:
                    expression->getFirstOperand()->accept(this);
                    leftResult = resultStack.top();
                    
                    if (leftResult == LinearityStatus::NonLinear) {
                        return;
                    } else {
                        resultStack.pop();
                        expression->getSecondOperand()->accept(this);
                        rightResult = resultStack.top();
                        if (rightResult == LinearityStatus::NonLinear) {
                            return;
                        }
                        resultStack.pop();
                    }
                    
                    if (leftResult == LinearityStatus::LinearContainsVariables && rightResult == LinearityStatus::LinearContainsVariables) {
                        resultStack.push(LinearityStatus::NonLinear);
                    }
                    
                    resultStack.push(leftResult == LinearityStatus::LinearContainsVariables || rightResult == LinearityStatus::LinearContainsVariables ? LinearityStatus::LinearContainsVariables : LinearityStatus::LinearWithoutVariables);
                    break;
                case BinaryNumericalFunctionExpression::OperatorType::Min: resultStack.push(LinearityStatus::NonLinear); break;
                case BinaryNumericalFunctionExpression::OperatorType::Max: resultStack.push(LinearityStatus::NonLinear); break;
                case BinaryNumericalFunctionExpression::OperatorType::Power: resultStack.push(LinearityStatus::NonLinear); break;
            }
        }
        
        void LinearityCheckVisitor::visit(BinaryRelationExpression const* expression) {
            resultStack.push(LinearityStatus::NonLinear);
        }
        
        void LinearityCheckVisitor::visit(VariableExpression const* expression) {
            resultStack.push(LinearityStatus::LinearContainsVariables);
        }
        
        void LinearityCheckVisitor::visit(UnaryBooleanFunctionExpression const* expression) {
            // Boolean function applications are not allowed in linear expressions.
            resultStack.push(LinearityStatus::NonLinear);
        }
        
        void LinearityCheckVisitor::visit(UnaryNumericalFunctionExpression const* expression) {
            switch (expression->getOperatorType()) {
                case UnaryNumericalFunctionExpression::OperatorType::Minus: break;
                case UnaryNumericalFunctionExpression::OperatorType::Floor:
                case UnaryNumericalFunctionExpression::OperatorType::Ceil: resultStack.pop(); resultStack.push(LinearityStatus::NonLinear); break;
            }
        }
        
        void LinearityCheckVisitor::visit(BooleanLiteralExpression const* expression) {
            resultStack.push(LinearityStatus::NonLinear);
        }
        
        void LinearityCheckVisitor::visit(IntegerLiteralExpression const* expression) {
            resultStack.push(LinearityStatus::LinearWithoutVariables);
        }
        
        void LinearityCheckVisitor::visit(DoubleLiteralExpression const* expression) {
            resultStack.push(LinearityStatus::LinearWithoutVariables);
        }
    }
}