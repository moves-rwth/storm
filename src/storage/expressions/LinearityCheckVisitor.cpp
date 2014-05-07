#include "src/storage/expressions/LinearityCheckVisitor.h"
#include "src/storage/expressions/Expressions.h"

#include "src/exceptions/ExceptionMacros.h"
#include "src/exceptions/InvalidTypeException.h"

namespace storm {
    namespace expressions {
        bool LinearityCheckVisitor::check(BaseExpression const* expression) {
            expression->accept(this);
            return resultStack.top();
        }
        
        void LinearityCheckVisitor::visit(IfThenElseExpression const* expression) {
            // An if-then-else expression is never linear.
            resultStack.push(false);
        }
        
        void LinearityCheckVisitor::visit(BinaryBooleanFunctionExpression const* expression) {
            // Boolean function applications are not allowed in linear expressions.
            resultStack.push(false);
        }
        
        void LinearityCheckVisitor::visit(BinaryNumericalFunctionExpression const* expression) {
            bool leftResult = true;
            bool rightResult = true;
            switch (expression->getOperatorType()) {
                case BinaryNumericalFunctionExpression::OperatorType::Plus:
                case BinaryNumericalFunctionExpression::OperatorType::Minus:
                    expression->getFirstOperand()->accept(this);
                    leftResult = resultStack.top();
                    
                    if (!leftResult) {
                        
                    } else {
                        resultStack.pop();

                        expression->getSecondOperand()->accept(this);
                    }

                case BinaryNumericalFunctionExpression::OperatorType::Times:
                case BinaryNumericalFunctionExpression::OperatorType::Divide:
                case BinaryNumericalFunctionExpression::OperatorType::Min: resultStack.push(false); break;
                case BinaryNumericalFunctionExpression::OperatorType::Max: resultStack.push(false); break;
            }
        }
        
        void LinearityCheckVisitor::visit(BinaryRelationExpression const* expression) {
            resultStack.push(false);
        }
        
        void LinearityCheckVisitor::visit(VariableExpression const* expression) {
            resultStack.push(true);
        }
        
        void LinearityCheckVisitor::visit(UnaryBooleanFunctionExpression const* expression) {
            // Boolean function applications are not allowed in linear expressions.
            resultStack.push(false);
        }
        
        void LinearityCheckVisitor::visit(UnaryNumericalFunctionExpression const* expression) {
            // Intentionally left empty (just pass subresult one level further).
        }
        
        void LinearityCheckVisitor::visit(BooleanLiteralExpression const* expression) {
            // Boolean function applications are not allowed in linear expressions.
            resultStack.push(false);
        }
        
        void LinearityCheckVisitor::visit(IntegerLiteralExpression const* expression) {
            resultStack.push(true);
        }
        
        void LinearityCheckVisitor::visit(DoubleLiteralExpression const* expression) {
            resultStack.push(true);
        }
    }
}