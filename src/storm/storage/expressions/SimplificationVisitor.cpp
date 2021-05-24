#include <map>
#include <unordered_map>
#include <string>

#include "storm/storage/expressions/SimplificationVisitor.h"
#include "storm/storage/expressions/Expressions.h"
#include "storm/storage/expressions/PredicateExpression.h"
#include "storm/storage/expressions/ExpressionManager.h"

namespace storm {
    namespace expressions {
        SimplificationVisitor::SimplificationVisitor() {
            // Intentionally left empty.
        }

        Expression SimplificationVisitor::substitute(Expression const &expression) {
            return Expression(boost::any_cast<std::shared_ptr<BaseExpression const>>(
                    expression.getBaseExpression().accept(*this, boost::none)));
        }

        boost::any SimplificationVisitor::visit(IfThenElseExpression const &expression, boost::any const &data) {
            std::shared_ptr<BaseExpression const> conditionExpression = boost::any_cast<std::shared_ptr<BaseExpression const>>(
                    expression.getCondition()->accept(*this, data));
            std::shared_ptr<BaseExpression const> thenExpression = boost::any_cast<std::shared_ptr<BaseExpression const>>(
                    expression.getThenExpression()->accept(*this, data));
            std::shared_ptr<BaseExpression const> elseExpression = boost::any_cast<std::shared_ptr<BaseExpression const>>(
                    expression.getElseExpression()->accept(*this, data));

            // If the arguments did not change, we simply push the expression itself.
            if (conditionExpression.get() == expression.getCondition().get() &&
                thenExpression.get() == expression.getThenExpression().get() &&
                elseExpression.get() == expression.getElseExpression().get()) {
                return expression.getSharedPointer();
            } else {
                return std::const_pointer_cast<BaseExpression const>(std::shared_ptr<BaseExpression>(
                        new IfThenElseExpression(expression.getManager(), expression.getType(), conditionExpression,
                                                 thenExpression, elseExpression)));
            }
        }

        boost::any
        SimplificationVisitor::visit(BinaryBooleanFunctionExpression const &expression, boost::any const &data) {
            std::shared_ptr<BaseExpression const> firstExpression = boost::any_cast<std::shared_ptr<BaseExpression const>>(
                    expression.getFirstOperand()->accept(*this, data));
            std::shared_ptr<BaseExpression const> secondExpression = boost::any_cast<std::shared_ptr<BaseExpression const>>(
                    expression.getSecondOperand()->accept(*this, data));

            // If the arguments did not change, we simply push the expression itself.
            if (firstExpression.get() == expression.getFirstOperand().get() &&
                secondExpression.get() == expression.getSecondOperand().get()) {
                return expression.getSharedPointer();
            } else {
                return std::const_pointer_cast<BaseExpression const>(std::shared_ptr<BaseExpression>(
                        new BinaryBooleanFunctionExpression(expression.getManager(), expression.getType(),
                                                            firstExpression, secondExpression,
                                                            expression.getOperatorType())));
            }
        }

        boost::any
        SimplificationVisitor::visit(BinaryNumericalFunctionExpression const &expression, boost::any const &data) {
            std::shared_ptr<BaseExpression const> firstExpression = boost::any_cast<std::shared_ptr<BaseExpression const>>(
                    expression.getFirstOperand()->accept(*this, data));
            std::shared_ptr<BaseExpression const> secondExpression = boost::any_cast<std::shared_ptr<BaseExpression const>>(
                    expression.getSecondOperand()->accept(*this, data));

            // If the arguments did not change, we simply push the expression itself.
            if (firstExpression.get() == expression.getFirstOperand().get() &&
                secondExpression.get() == expression.getSecondOperand().get()) {
                return expression.getSharedPointer();
            } else {
                return std::const_pointer_cast<BaseExpression const>(std::shared_ptr<BaseExpression>(
                        new BinaryNumericalFunctionExpression(expression.getManager(), expression.getType(),
                                                              firstExpression, secondExpression,
                                                              expression.getOperatorType())));
            }
        }

        boost::any SimplificationVisitor::visit(BinaryRelationExpression const &expression, boost::any const &data) {
            std::shared_ptr<BaseExpression const> firstExpression = boost::any_cast<std::shared_ptr<BaseExpression const>>(
                    expression.getFirstOperand()->accept(*this, data));
            std::shared_ptr<BaseExpression const> secondExpression = boost::any_cast<std::shared_ptr<BaseExpression const>>(
                    expression.getSecondOperand()->accept(*this, data));

            // If the arguments did not change, we simply push the expression itself.
            if (firstExpression.get() == expression.getFirstOperand().get() &&
                secondExpression.get() == expression.getSecondOperand().get()) {
                return expression.getSharedPointer();
            } else {
                return std::const_pointer_cast<BaseExpression const>(std::shared_ptr<BaseExpression>(
                        new BinaryRelationExpression(expression.getManager(), expression.getType(), firstExpression,
                                                     secondExpression, expression.getRelationType())));
            }
        }

        boost::any SimplificationVisitor::visit(VariableExpression const &expression, boost::any const &) {

            return expression.getSharedPointer();

        }

        boost::any
        SimplificationVisitor::visit(UnaryBooleanFunctionExpression const &expression, boost::any const &data) {
            std::shared_ptr<BaseExpression const> operandExpression = boost::any_cast<std::shared_ptr<BaseExpression const>>(
                    expression.getOperand()->accept(*this, data));

            // If the argument did not change, we simply push the expression itself.
            if (operandExpression.get() == expression.getOperand().get()) {
                return expression.getSharedPointer();
            } else {
                return std::const_pointer_cast<BaseExpression const>(std::shared_ptr<BaseExpression>(
                        new UnaryBooleanFunctionExpression(expression.getManager(), expression.getType(),
                                                           operandExpression, expression.getOperatorType())));
            }
        }

        boost::any
        SimplificationVisitor::visit(UnaryNumericalFunctionExpression const &expression, boost::any const &data) {
            std::shared_ptr<BaseExpression const> operandExpression = boost::any_cast<std::shared_ptr<BaseExpression const>>(
                    expression.getOperand()->accept(*this, data));

            // If the argument did not change, we simply push the expression itself.
            if (operandExpression.get() == expression.getOperand().get()) {
                return expression.getSharedPointer();
            } else {
                return std::const_pointer_cast<BaseExpression const>(std::shared_ptr<BaseExpression>(
                        new UnaryNumericalFunctionExpression(expression.getManager(), expression.getType(),
                                                             operandExpression, expression.getOperatorType())));
            }
        }

        boost::any SimplificationVisitor::visit(PredicateExpression const &expression, boost::any const &data) {
            std::vector<Expression> newExpressions;
            for (uint64_t i = 0; i < expression.getArity(); ++i) {
                newExpressions.emplace_back(boost::any_cast<std::shared_ptr<BaseExpression const>>(
                        expression.getOperand(i)->accept(*this, data)));
            }
            std::vector<Expression> newSumExpressions;
            for (auto const &expr : newExpressions) {
                newSumExpressions.push_back(
                        ite(expr, expression.getManager().integer(1), expression.getManager().integer(0)));
            }

            storm::expressions::Expression finalexpr;
            if (expression.getPredicateType() == PredicateExpression::PredicateType::AtLeastOneOf) {
                finalexpr = storm::expressions::sum(newSumExpressions) > expression.getManager().integer(0);
            } else if (expression.getPredicateType() == PredicateExpression::PredicateType::AtMostOneOf) {
                finalexpr = storm::expressions::sum(newSumExpressions) <= expression.getManager().integer(1);
            } else if (expression.getPredicateType() == PredicateExpression::PredicateType::ExactlyOneOf) {
                finalexpr = storm::expressions::sum(newSumExpressions) == expression.getManager().integer(1);
            } else {
                STORM_LOG_ASSERT(false, "Unknown predicate type.");
            }
            return std::const_pointer_cast<BaseExpression const>(finalexpr.getBaseExpressionPointer());
        }


        boost::any SimplificationVisitor::visit(BooleanLiteralExpression const &expression, boost::any const &) {
            return expression.getSharedPointer();
        }

        boost::any SimplificationVisitor::visit(IntegerLiteralExpression const &expression, boost::any const &) {
            return expression.getSharedPointer();
        }

        boost::any SimplificationVisitor::visit(RationalLiteralExpression const &expression, boost::any const &) {
            return expression.getSharedPointer();
        }

    }
}