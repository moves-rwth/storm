#include "storm/storage/expressions/VariableSetAbstractor.h"

#include "storm/storage/expressions/Expressions.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace expressions {
        
        VariableSetAbstractor::VariableSetAbstractor(std::set<storm::expressions::Variable> const& variablesToAbstract) : variablesToAbstract(variablesToAbstract) {
            // Intentionally left empty.
        }
        
        storm::expressions::Expression VariableSetAbstractor::abstract(storm::expressions::Expression const& expression) {
            std::set<storm::expressions::Variable> containedVariables = expression.getVariables();
            bool onlyAbstractedVariables = std::includes(variablesToAbstract.begin(), variablesToAbstract.end(), containedVariables.begin(), containedVariables.end());
            
            if (onlyAbstractedVariables) {
                return storm::expressions::Expression();
            }
            
            std::set<storm::expressions::Variable> tmp;
            std::set_intersection(containedVariables.begin(), containedVariables.end(), variablesToAbstract.begin(), variablesToAbstract.end(), std::inserter(tmp, tmp.begin()));
            bool hasAbstractedVariables = !tmp.empty();
            
            if (hasAbstractedVariables) {
                return boost::any_cast<storm::expressions::Expression>(expression.accept(*this, boost::none));
            } else {
                return expression;
            }
        }
        
        boost::any VariableSetAbstractor::visit(IfThenElseExpression const& expression, boost::any const& data) {
            std::set<storm::expressions::Variable> conditionVariables;
            expression.getCondition()->gatherVariables(conditionVariables);
            bool conditionOnlyAbstractedVariables = std::includes(variablesToAbstract.begin(), variablesToAbstract.end(), conditionVariables.begin(), conditionVariables.end());
            
            std::set<storm::expressions::Variable> tmp;
            std::set_intersection(conditionVariables.begin(), conditionVariables.end(), variablesToAbstract.begin(), variablesToAbstract.end(), std::inserter(tmp, tmp.begin()));
            bool conditionHasAbstractedVariables = !tmp.empty();
            
            std::set<storm::expressions::Variable> thenVariables;
            expression.getThenExpression()->gatherVariables(thenVariables);
            bool thenOnlyAbstractedVariables = std::includes(variablesToAbstract.begin(), variablesToAbstract.end(), thenVariables.begin(), thenVariables.end());

            tmp.clear();
            std::set_intersection(thenVariables.begin(), thenVariables.end(), variablesToAbstract.begin(), variablesToAbstract.end(), std::inserter(tmp, tmp.begin()));
            bool thenHasAbstractedVariables = !tmp.empty();

            std::set<storm::expressions::Variable> elseVariables;
            expression.getElseExpression()->gatherVariables(elseVariables);
            bool elseOnlyAbstractedVariables = std::includes(variablesToAbstract.begin(), variablesToAbstract.end(), elseVariables.begin(), elseVariables.end());

            tmp.clear();
            std::set_intersection(elseVariables.begin(), elseVariables.end(), variablesToAbstract.begin(), variablesToAbstract.end(), std::inserter(tmp, tmp.begin()));
            bool elseHasAbstractedVariables = !tmp.empty();

            if (conditionHasAbstractedVariables || thenHasAbstractedVariables || elseHasAbstractedVariables) {
                if (conditionOnlyAbstractedVariables && thenOnlyAbstractedVariables && elseOnlyAbstractedVariables) {
                    return boost::any();
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Cannot abstract from variable set in expression as it mixes variables of different types.");
                }
            } else {
                return expression.toExpression();
            }
        }
        
        boost::any VariableSetAbstractor::visit(BinaryBooleanFunctionExpression const& expression, boost::any const& data) {
            std::set<storm::expressions::Variable> leftContainedVariables;
            expression.getFirstOperand()->gatherVariables(leftContainedVariables);
            bool leftOnlyAbstractedVariables = std::includes(variablesToAbstract.begin(), variablesToAbstract.end(), leftContainedVariables.begin(), leftContainedVariables.end());
            
            std::set<storm::expressions::Variable> tmp;
            std::set_intersection(leftContainedVariables.begin(), leftContainedVariables.end(), variablesToAbstract.begin(), variablesToAbstract.end(), std::inserter(tmp, tmp.begin()));
            bool leftHasAbstractedVariables = !tmp.empty();
            
            std::set<storm::expressions::Variable> rightContainedVariables;
            expression.getSecondOperand()->gatherVariables(rightContainedVariables);
            bool rightOnlyAbstractedVariables = std::includes(variablesToAbstract.begin(), variablesToAbstract.end(), rightContainedVariables.begin(), rightContainedVariables.end());
            
            tmp.clear();
            std::set_intersection(rightContainedVariables.begin(), rightContainedVariables.end(), variablesToAbstract.begin(), variablesToAbstract.end(), std::inserter(tmp, tmp.begin()));
            bool rightHasAbstractedVariables = !tmp.empty();
            
            if (leftOnlyAbstractedVariables && rightOnlyAbstractedVariables) {
                return boost::any();
            } else if (!leftHasAbstractedVariables && !rightHasAbstractedVariables) {
                return expression;
            } else {
                if (leftHasAbstractedVariables && !rightHasAbstractedVariables) {
                    return expression.getFirstOperand()->toExpression();
                } else  if (rightHasAbstractedVariables && !leftHasAbstractedVariables) {
                    return expression.getSecondOperand()->toExpression();
                } else {
                    storm::expressions::Expression leftResult = boost::any_cast<storm::expressions::Expression>(expression.getFirstOperand()->accept(*this, data));
                    storm::expressions::Expression rightResult = boost::any_cast<storm::expressions::Expression>(expression.getFirstOperand()->accept(*this, data));
                    
                    switch (expression.getOperatorType()) {
                        case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::And: return leftResult && rightResult;
                        case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::Or: return leftResult || rightResult;
                        case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::Xor: return leftResult ^ rightResult;
                        case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::Implies: return storm::expressions::implies(leftResult, rightResult);
                        case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::Iff: return storm::expressions::iff(leftResult, rightResult);
                    }
                }
            }
        }
        
        boost::any VariableSetAbstractor::visit(BinaryNumericalFunctionExpression const& expression, boost::any const& data) {
            std::set<storm::expressions::Variable> leftContainedVariables;
            expression.getFirstOperand()->gatherVariables(leftContainedVariables);
            bool leftOnlyAbstractedVariables = std::includes(variablesToAbstract.begin(), variablesToAbstract.end(), leftContainedVariables.begin(), leftContainedVariables.end());
            
            std::set<storm::expressions::Variable> tmp;
            std::set_intersection(leftContainedVariables.begin(), leftContainedVariables.end(), variablesToAbstract.begin(), variablesToAbstract.end(), std::inserter(tmp, tmp.begin()));
            bool leftHasAbstractedVariables = !tmp.empty();
            
            std::set<storm::expressions::Variable> rightContainedVariables;
            expression.getSecondOperand()->gatherVariables(rightContainedVariables);
            bool rightOnlyAbstractedVariables = std::includes(variablesToAbstract.begin(), variablesToAbstract.end(), rightContainedVariables.begin(), rightContainedVariables.end());
            
            tmp.clear();
            std::set_intersection(rightContainedVariables.begin(), rightContainedVariables.end(), variablesToAbstract.begin(), variablesToAbstract.end(), std::inserter(tmp, tmp.begin()));
            bool rightHasAbstractedVariables = !tmp.empty();
            
            if (leftOnlyAbstractedVariables && rightOnlyAbstractedVariables) {
                return boost::any();
            } else if (!leftHasAbstractedVariables && !rightHasAbstractedVariables) {
                return expression;
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Cannot abstract from variable set in expression as it mixes variables of different types.");
            }
        }
        
        boost::any VariableSetAbstractor::visit(BinaryRelationExpression const& expression, boost::any const& data) {
            std::set<storm::expressions::Variable> leftContainedVariables;
            expression.getFirstOperand()->gatherVariables(leftContainedVariables);
            bool leftOnlyAbstractedVariables = std::includes(variablesToAbstract.begin(), variablesToAbstract.end(), leftContainedVariables.begin(), leftContainedVariables.end());
            
            std::set<storm::expressions::Variable> tmp;
            std::set_intersection(leftContainedVariables.begin(), leftContainedVariables.end(), variablesToAbstract.begin(), variablesToAbstract.end(), std::inserter(tmp, tmp.begin()));
            bool leftHasAbstractedVariables = !tmp.empty();

            std::set<storm::expressions::Variable> rightContainedVariables;
            expression.getSecondOperand()->gatherVariables(rightContainedVariables);
            bool rightOnlyAbstractedVariables = std::includes(variablesToAbstract.begin(), variablesToAbstract.end(), rightContainedVariables.begin(), rightContainedVariables.end());
            
            tmp.clear();
            std::set_intersection(rightContainedVariables.begin(), rightContainedVariables.end(), variablesToAbstract.begin(), variablesToAbstract.end(), std::inserter(tmp, tmp.begin()));
            bool rightHasAbstractedVariables = !tmp.empty();

            if (leftOnlyAbstractedVariables && rightOnlyAbstractedVariables) {
                return boost::any();
            } else if (!leftHasAbstractedVariables && !rightHasAbstractedVariables) {
                return expression;
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Cannot abstract from variable set in expression as it mixes variables of different types.");
            }
        }
        
        boost::any VariableSetAbstractor::visit(VariableExpression const& expression, boost::any const& data) {
            if (variablesToAbstract.find(expression.getVariable()) != variablesToAbstract.end()) {
                return boost::any();
            } else {
                return expression.toExpression();
            }
        }
        
        boost::any VariableSetAbstractor::visit(UnaryBooleanFunctionExpression const& expression, boost::any const& data) {
            std::set<storm::expressions::Variable> containedVariables;
            expression.gatherVariables(containedVariables);
            bool onlyAbstractedVariables = std::includes(variablesToAbstract.begin(), variablesToAbstract.end(), containedVariables.begin(), containedVariables.end());
            
            if (onlyAbstractedVariables) {
                return boost::any();
            }
            
            std::set<storm::expressions::Variable> tmp;
            std::set_intersection(containedVariables.begin(), containedVariables.end(), variablesToAbstract.begin(), variablesToAbstract.end(), std::inserter(tmp, tmp.begin()));
            bool hasAbstractedVariables = !tmp.empty();
            if (hasAbstractedVariables) {
                storm::expressions::Expression subexpression = boost::any_cast<storm::expressions::Expression>(expression.getOperand()->accept(*this, data));
                switch (expression.getOperatorType()) {
                    case storm::expressions::UnaryBooleanFunctionExpression::OperatorType::Not: return !subexpression;
                }
            } else {
                return expression.toExpression();
            }
        }
        
        boost::any VariableSetAbstractor::visit(UnaryNumericalFunctionExpression const& expression, boost::any const& data) {
            std::set<storm::expressions::Variable> containedVariables;
            expression.gatherVariables(containedVariables);
            bool onlyAbstractedVariables = std::includes(variablesToAbstract.begin(), variablesToAbstract.end(), containedVariables.begin(), containedVariables.end());
            
            if (onlyAbstractedVariables) {
                return boost::any();
            }
            
            std::set<storm::expressions::Variable> tmp;
            std::set_intersection(containedVariables.begin(), containedVariables.end(), variablesToAbstract.begin(), variablesToAbstract.end(), std::inserter(tmp, tmp.begin()));
            bool hasAbstractedVariables = !tmp.empty();
            if (hasAbstractedVariables) {
                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Cannot abstract from variable set in expression as it mixes variables of different types.");
            } else {
                return expression.toExpression();
            }
        }
        
        boost::any VariableSetAbstractor::visit(BooleanLiteralExpression const& expression, boost::any const& data) {
            return expression.toExpression();
        }
        
        boost::any VariableSetAbstractor::visit(IntegerLiteralExpression const& expression, boost::any const& data) {
            return expression.toExpression();
        }
        
        boost::any VariableSetAbstractor::visit(RationalLiteralExpression const& expression, boost::any const& data) {
            return expression.toExpression();
        }
        
    }
}
