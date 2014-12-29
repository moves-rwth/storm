#include "src/storage/expressions/LinearCoefficientVisitor.h"

#include "src/storage/expressions/Expressions.h"
#include "src/utility/macros.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace expressions {
        std::pair<SimpleValuation, double> LinearCoefficientVisitor::getLinearCoefficients(Expression const& expression) {
            return boost::any_cast<std::pair<SimpleValuation, double>>(expression.getBaseExpression().accept(*this));
        }
        
        boost::any LinearCoefficientVisitor::visit(IfThenElseExpression const& expression) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression is non-linear.");
        }
        
        boost::any LinearCoefficientVisitor::visit(BinaryBooleanFunctionExpression const& expression) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression is non-linear.");
        }
        
        boost::any LinearCoefficientVisitor::visit(BinaryNumericalFunctionExpression const& expression) {
            std::pair<SimpleValuation, double> leftResult = boost::any_cast<std::pair<SimpleValuation, double>>(expression.getFirstOperand()->accept(*this));
            std::pair<SimpleValuation, double> rightResult = boost::any_cast<std::pair<SimpleValuation, double>>(expression.getSecondOperand()->accept(*this));

            if (expression.getOperatorType() == BinaryNumericalFunctionExpression::OperatorType::Plus) {
                // Now add the left result to the right result.
                for (auto const& identifier : leftResult.first.getDoubleIdentifiers()) {
                    if (rightResult.first.containsDoubleIdentifier(identifier)) {
                        rightResult.first.setDoubleValue(identifier, leftResult.first.getDoubleValue(identifier) + rightResult.first.getDoubleValue(identifier));
                    } else {
                        rightResult.first.setDoubleValue(identifier, leftResult.first.getDoubleValue(identifier));
                    }
                }
                rightResult.second += leftResult.second;
            } else if (expression.getOperatorType() == BinaryNumericalFunctionExpression::OperatorType::Minus) {
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
            } else if (expression.getOperatorType() == BinaryNumericalFunctionExpression::OperatorType::Times) {
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
            } else if (expression.getOperatorType() == BinaryNumericalFunctionExpression::OperatorType::Divide) {
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
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression is non-linear.");
            }
            return rightResult;
        }
        
        boost::any LinearCoefficientVisitor::visit(BinaryRelationExpression const& expression) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression is non-linear.");
        }
        
        boost::any LinearCoefficientVisitor::visit(VariableExpression const& expression) {
            SimpleValuation valuation;
            switch (expression.getReturnType()) {
                case ExpressionReturnType::Bool: STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression is non-linear."); break;
                case ExpressionReturnType::Int:
                case ExpressionReturnType::Double: valuation.addDoubleIdentifier(expression.getVariableName(), 1); break;
                case ExpressionReturnType::Undefined: STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Illegal expression return type."); break;
            }
            
            return std::make_pair(valuation, static_cast<double>(0));
        }
        
        boost::any LinearCoefficientVisitor::visit(UnaryBooleanFunctionExpression const& expression) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression is non-linear.");
        }
        
        boost::any LinearCoefficientVisitor::visit(UnaryNumericalFunctionExpression const& expression) {
            std::pair<SimpleValuation, double> childResult = boost::any_cast<std::pair<SimpleValuation, double>>(expression.getOperand()->accept(*this));
            
            if (expression.getOperatorType() == UnaryNumericalFunctionExpression::OperatorType::Minus) {
                // Here, we need to negate all double identifiers.
                for (auto const& identifier : childResult.first.getDoubleIdentifiers()) {
                    childResult.first.setDoubleValue(identifier, -childResult.first.getDoubleValue(identifier));
                }
                return childResult;
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression is non-linear.");
            }
        }
        
        boost::any LinearCoefficientVisitor::visit(BooleanLiteralExpression const& expression) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Expression is non-linear.");
        }
        
        boost::any LinearCoefficientVisitor::visit(IntegerLiteralExpression const& expression) {
            return std::make_pair(SimpleValuation(), static_cast<double>(expression.getValue()));
        }
        
        boost::any LinearCoefficientVisitor::visit(DoubleLiteralExpression const& expression) {
            return std::make_pair(SimpleValuation(), expression.getValue());
        }
    }
}