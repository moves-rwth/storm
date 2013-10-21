/*
 * Z3ExpressionAdapter.h
 *
 *  Created on: 04.10.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_ADAPTERS_Z3EXPRESSIONADAPTER_H_
#define STORM_ADAPTERS_Z3EXPRESSIONADAPTER_H_

#include <stack>

#include "src/ir/expressions/ExpressionVisitor.h"
#include "src/ir/expressions/Expressions.h"

namespace storm {
    namespace adapters {
        
        class Z3ExpressionAdapter : public storm::ir::expressions::ExpressionVisitor {
        public:
            /*!
             * Creates a Z3ExpressionAdapter over the given Z3 context.
             *
             * @param context A reference to the Z3 context over which to build the expressions. Be careful to guarantee
             * the lifetime of the context as long as the instance of this adapter is used.
             * @param variableToExpressionMap A mapping from variable names to their corresponding Z3 expressions.
             */
            Z3ExpressionAdapter(z3::context& context, std::map<std::string, z3::expr> const& variableToExpressionMap) : context(context), stack(), variableToExpressionMap(variableToExpressionMap) {
                // Intentionally left empty.
            }
            
            /*!
             * Translates the given expression to an equivalent expression for Z3.
             *
             * @param expression The expression to translate.
             * @return An equivalent expression for Z3.
             */
            z3::expr translateExpression(std::unique_ptr<storm::ir::expressions::BaseExpression> const& expression) {
                expression->accept(this);
                z3::expr result = stack.top();
                stack.pop();
                return result;
            }
            
            virtual void visit(ir::expressions::BinaryBooleanFunctionExpression* expression) {
                expression->getLeft()->accept(this);
                expression->getRight()->accept(this);
                
                z3::expr rightResult = stack.top();
                stack.pop();
                z3::expr leftResult = stack.top();
                stack.pop();
                
                switch(expression->getFunctionType()) {
                    case storm::ir::expressions::BinaryBooleanFunctionExpression::AND:
                        stack.push(leftResult && rightResult);
                        break;
                    case storm::ir::expressions::BinaryBooleanFunctionExpression::OR:
                        stack.push(leftResult || rightResult);
                        break;
                    default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
                        << "Unknown boolean binary operator: '" << expression->getFunctionType() << "' in expression " << expression->toString() << ".";
                }
                
            }
            
            virtual void visit(ir::expressions::BinaryNumericalFunctionExpression* expression) {
                expression->getLeft()->accept(this);
                expression->getRight()->accept(this);
                
                z3::expr rightResult = stack.top();
                stack.pop();
                z3::expr leftResult = stack.top();
                stack.pop();
                
                switch(expression->getFunctionType()) {
                    case storm::ir::expressions::BinaryNumericalFunctionExpression::PLUS:
                        stack.push(leftResult + rightResult);
                        break;
                    case storm::ir::expressions::BinaryNumericalFunctionExpression::MINUS:
                        stack.push(leftResult - rightResult);
                        break;
                    case storm::ir::expressions::BinaryNumericalFunctionExpression::TIMES:
                        stack.push(leftResult * rightResult);
                        break;
                    case storm::ir::expressions::BinaryNumericalFunctionExpression::DIVIDE:
                        stack.push(leftResult / rightResult);
                        break;
                    case storm::ir::expressions::BinaryNumericalFunctionExpression::MIN:
                        stack.push(ite(leftResult <= rightResult, leftResult, rightResult));
                        break;
                    case storm::ir::expressions::BinaryNumericalFunctionExpression::MAX:
                        stack.push(ite(leftResult >= rightResult, leftResult, rightResult));
                        break;
                    default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
                        << "Unknown numerical binary operator: '" << expression->getFunctionType() << "' in expression " << expression->toString() << ".";
                }
            }
            
            virtual void visit(ir::expressions::BinaryRelationExpression* expression) {
                expression->getLeft()->accept(this);
                expression->getRight()->accept(this);
                
                z3::expr rightResult = stack.top();
                stack.pop();
                z3::expr leftResult = stack.top();
                stack.pop();
                
                switch(expression->getRelationType()) {
                    case storm::ir::expressions::BinaryRelationExpression::EQUAL:
                        stack.push(leftResult == rightResult);
                        break;
                    case storm::ir::expressions::BinaryRelationExpression::NOT_EQUAL:
                        stack.push(leftResult != rightResult);
                        break;
                    case storm::ir::expressions::BinaryRelationExpression::LESS:
                        stack.push(leftResult < rightResult);
                        break;
                    case storm::ir::expressions::BinaryRelationExpression::LESS_OR_EQUAL:
                        stack.push(leftResult <= rightResult);
                        break;
                    case storm::ir::expressions::BinaryRelationExpression::GREATER:
                        stack.push(leftResult > rightResult);
                        break;
                    case storm::ir::expressions::BinaryRelationExpression::GREATER_OR_EQUAL:
                        stack.push(leftResult >= rightResult);
                        break;
                    default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
                        << "Unknown boolean binary operator: '" << expression->getRelationType() << "' in expression " << expression->toString() << ".";
                }    
            }
            
            virtual void visit(ir::expressions::BooleanConstantExpression* expression) {
                if (!expression->isDefined()) {
                    throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
					<< ". Boolean constant '" << expression->getConstantName() << "' is undefined.";
                }
                
                stack.push(context.bool_val(expression->getValue()));    
            }
            
            virtual void visit(ir::expressions::BooleanLiteralExpression* expression) {
                stack.push(context.bool_val(expression->getValueAsBool(nullptr)));
            }
            
            virtual void visit(ir::expressions::DoubleConstantExpression* expression) {
                if (!expression->isDefined()) {
                    throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
					<< ". Double constant '" << expression->getConstantName() << "' is undefined.";
                }
                
                std::stringstream fractionStream;
                fractionStream << expression->getValue();
                stack.push(context.real_val(fractionStream.str().c_str()));
            }
            
            virtual void visit(ir::expressions::DoubleLiteralExpression* expression) {
                std::stringstream fractionStream;
                fractionStream << expression->getValueAsDouble(nullptr);
                stack.push(context.real_val(fractionStream.str().c_str()));
            }
            
            virtual void visit(ir::expressions::IntegerConstantExpression* expression) {
                if (!expression->isDefined()) {
                    throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
					<< ". Integer constant '" << expression->getConstantName() << "' is undefined.";
                }
                
                stack.push(context.int_val(expression->getValue()));    
            }
            
            virtual void visit(ir::expressions::IntegerLiteralExpression* expression) {
                stack.push(context.int_val(expression->getValueAsInt(nullptr)));
            }
            
            virtual void visit(ir::expressions::UnaryBooleanFunctionExpression* expression) {
                expression->getChild()->accept(this);
                
                z3::expr childResult = stack.top();
                stack.pop();
                
                switch (expression->getFunctionType()) {
                    case storm::ir::expressions::UnaryBooleanFunctionExpression::NOT:
                        stack.push(!childResult);
                        break;
                    default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
                        << "Unknown boolean binary operator: '" << expression->getFunctionType() << "' in expression " << expression->toString() << ".";
                }    
            }
            
            virtual void visit(ir::expressions::UnaryNumericalFunctionExpression* expression) {
                expression->getChild()->accept(this);
                
                z3::expr childResult = stack.top();
                stack.pop();
                
                switch(expression->getFunctionType()) {
                    case storm::ir::expressions::UnaryNumericalFunctionExpression::MINUS:
                        stack.push(0 - childResult);
                        break;
                    default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
                        << "Unknown numerical unary operator: '" << expression->getFunctionType() << "'.";
                }
            }
            
            virtual void visit(ir::expressions::VariableExpression* expression) {
                stack.push(variableToExpressionMap.at(expression->getVariableName()));
            }
            
        private:
            z3::context& context;
            std::stack<z3::expr> stack;
            std::map<std::string, z3::expr> variableToExpressionMap;
        };
        
    } // namespace adapters
} // namespace storm

#endif /* STORM_ADAPTERS_Z3EXPRESSIONADAPTER_H_ */
