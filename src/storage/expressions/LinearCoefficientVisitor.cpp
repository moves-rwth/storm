#include "src/storage/expressions/LinearCoefficientVisitor.h"

#include "src/storage/expressions/Expressions.h"
#include "src/utility/macros.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace expressions {
        std::pair<SimpleValuation, double> LinearCoefficientVisitor::getLinearCoefficients(Expression const& expression) {
            expression.getBaseExpression().accept(this);
            return resultStack.top();
        }
        
        void LinearCoefficientVisitor::visit(IfThenElseExpression const* expression) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression is non-linear.");
        }
        
        void LinearCoefficientVisitor::visit(BinaryBooleanFunctionExpression const* expression) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression is non-linear.");
        }
        
        void LinearCoefficientVisitor::visit(BinaryNumericalFunctionExpression const* expression) {
            if (expression->getOperatorType() == BinaryNumericalFunctionExpression::OperatorType::Plus) {
                expression->getFirstOperand()->accept(this);
                std::pair<SimpleValuation, double> leftResult = resultStack.top();
                resultStack.pop();
                expression->getSecondOperand()->accept(this);
                std::pair<SimpleValuation, double>& rightResult = resultStack.top();
                
                // Now add the left result to the right result.
                for (auto const& identifier : leftResult.first.getDoubleIdentifiers()) {
                    if (rightResult.first.containsDoubleIdentifier(identifier)) {
                        rightResult.first.setDoubleValue(identifier, leftResult.first.getDoubleValue(identifier) + rightResult.first.getDoubleValue(identifier));
                    } else {
                        rightResult.first.setDoubleValue(identifier, leftResult.first.getDoubleValue(identifier));
                    }
                }
                rightResult.second += leftResult.second;
                return;
            } else if (expression->getOperatorType() == BinaryNumericalFunctionExpression::OperatorType::Minus) {
                expression->getFirstOperand()->accept(this);
                std::pair<SimpleValuation, double> leftResult = resultStack.top();
                resultStack.pop();
                expression->getSecondOperand()->accept(this);
                std::pair<SimpleValuation, double>& rightResult = resultStack.top();
                
                // Now subtract the right result from the left result.
                for (auto const& identifier : leftResult.first.getDoubleIdentifiers()) {
                    if (rightResult.first.containsDoubleIdentifier(identifier)) {
                        rightResult.first.setDoubleValue(identifier, leftResult.first.getDoubleValue(identifier) - rightResult.first.getDoubleValue(identifier));
                    } else {
                        rightResult.first.setDoubleValue(identifier, leftResult.first.getDoubleValue(identifier));
                    }
                }
                for (auto const& identifier : rightResult.first.getDoubleIdentifiers()) {
                    if (!leftResult.first.containsDoubleIdentifier(identifier)) {
                        rightResult.first.setDoubleValue(identifier, -rightResult.first.getDoubleValue(identifier));
                    }
                }
                rightResult.second = leftResult.second - rightResult.second;
                return;
            } else if (expression->getOperatorType() == BinaryNumericalFunctionExpression::OperatorType::Times) {
                expression->getFirstOperand()->accept(this);
                std::pair<SimpleValuation, double> leftResult = resultStack.top();
                resultStack.pop();
                expression->getSecondOperand()->accept(this);
                std::pair<SimpleValuation, double>& rightResult = resultStack.top();
                
                // If the expression is linear, either the left or the right side must not contain variables.
                STORM_LOG_THROW(leftResult.first.getNumberOfIdentifiers() == 0 || rightResult.first.getNumberOfIdentifiers() == 0, storm::exceptions::InvalidArgumentException, "Expression is non-linear.");
                if (leftResult.first.getNumberOfIdentifiers() == 0) {
                    for (auto const& identifier : rightResult.first.getDoubleIdentifiers()) {
                        rightResult.first.setDoubleValue(identifier, leftResult.second * rightResult.first.getDoubleValue(identifier));
                    }
                } else {
                    for (auto const& identifier : leftResult.first.getDoubleIdentifiers()) {
                        rightResult.first.addDoubleIdentifier(identifier, rightResult.second * leftResult.first.getDoubleValue(identifier));
                    }
                }
                rightResult.second *= leftResult.second;
                return;
            } else if (expression->getOperatorType() == BinaryNumericalFunctionExpression::OperatorType::Divide) {
                expression->getFirstOperand()->accept(this);
                std::pair<SimpleValuation, double> leftResult = resultStack.top();
                resultStack.pop();
                expression->getSecondOperand()->accept(this);
                std::pair<SimpleValuation, double>& rightResult = resultStack.top();
                
                // If the expression is linear, either the left or the right side must not contain variables.
                STORM_LOG_THROW(leftResult.first.getNumberOfIdentifiers() == 0 || rightResult.first.getNumberOfIdentifiers() == 0, storm::exceptions::InvalidArgumentException, "Expression is non-linear.");
                if (leftResult.first.getNumberOfIdentifiers() == 0) {
                    for (auto const& identifier : rightResult.first.getDoubleIdentifiers()) {
                        rightResult.first.setDoubleValue(identifier, leftResult.second / rightResult.first.getDoubleValue(identifier));
                    }
                } else {
                    for (auto const& identifier : leftResult.first.getDoubleIdentifiers()) {
                        rightResult.first.addDoubleIdentifier(identifier, leftResult.first.getDoubleValue(identifier) / rightResult.second);
                    }
                }
                rightResult.second = leftResult.second / leftResult.second;
                return;
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression is non-linear.");
            }
        }
        
        void LinearCoefficientVisitor::visit(BinaryRelationExpression const* expression) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression is non-linear.");
        }
        
        void LinearCoefficientVisitor::visit(VariableExpression const* expression) {
            SimpleValuation valuation;
            switch (expression->getReturnType()) {
                case ExpressionReturnType::Bool: STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression is non-linear."); break;
                case ExpressionReturnType::Int:
                case ExpressionReturnType::Double: valuation.addDoubleIdentifier(expression->getVariableName(), 1); break;
                case ExpressionReturnType::Undefined: STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Illegal expression return type."); break;
            }
            
            resultStack.push(std::make_pair(valuation, 0));
        }
        
        void LinearCoefficientVisitor::visit(UnaryBooleanFunctionExpression const* expression) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression is non-linear.");
        }
        
        void LinearCoefficientVisitor::visit(UnaryNumericalFunctionExpression const* expression) {
            if (expression->getOperatorType() == UnaryNumericalFunctionExpression::OperatorType::Minus) {
                // Here, we need to negate all double identifiers.
                std::pair<SimpleValuation, double>& valuationConstantPair = resultStack.top();
                for (auto const& identifier : valuationConstantPair.first.getDoubleIdentifiers()) {
                    valuationConstantPair.first.setDoubleValue(identifier, -valuationConstantPair.first.getDoubleValue(identifier));
                }
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression is non-linear.");
            }
        }
        
        void LinearCoefficientVisitor::visit(BooleanLiteralExpression const* expression) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression is non-linear.");
        }
        
        void LinearCoefficientVisitor::visit(IntegerLiteralExpression const* expression) {
            resultStack.push(std::make_pair(SimpleValuation(), static_cast<double>(expression->getValue())));
        }
        
        void LinearCoefficientVisitor::visit(DoubleLiteralExpression const* expression) {
            resultStack.push(std::make_pair(SimpleValuation(), expression->getValue()));
        }
    }
}