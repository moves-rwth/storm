/*
 * Z3ExpressionAdapter.h
 *
 *  Created on: 04.10.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_ADAPTERS_Z3EXPRESSIONADAPTER_H_
#define STORM_ADAPTERS_Z3EXPRESSIONADAPTER_H_

#include <stack>

#include "z3++.h"
#include "z3.h"

#include "src/storage/expressions/Expressions.h"
#include "src/exceptions/ExceptionMacros.h"
#include "src/exceptions/ExpressionEvaluationException.h"
#include "src/exceptions/InvalidTypeException.h"
#include "src/exceptions/NotImplementedException.h"

namespace storm {
    namespace adapters {
        
        class Z3ExpressionAdapter : public storm::expressions::ExpressionVisitor {
        public:
            /*!
             * Creates a Z3ExpressionAdapter over the given Z3 context.
			 *
			 * @remark The adapter internally creates helper variables prefixed with `__z3adapter_`. Avoid having variables with
			 *         this prefix in the variableToExpressionMap, as this might lead to unexpected results.
             *
             * @param context A reference to the Z3 context over which to build the expressions. Be careful to guarantee
             * the lifetime of the context as long as the instance of this adapter is used.
             * @param variableToExpressionMap A mapping from variable names to their corresponding Z3 expressions.
             */
            Z3ExpressionAdapter(z3::context& context, std::map<std::string, z3::expr> const& variableToExpressionMap)
				: context(context)
				, stack()
				, variableToExpressionMap(variableToExpressionMap)
				, additionalVariableCounter(0)
				, additionalAssertions() {
                // Intentionally left empty.
            }
            
            /*!
             * Translates the given expression to an equivalent expression for Z3.
			 *
			 * @remark The adapter internally creates helper variables prefixed with `__z3adapter_`. Avoid having variables with
			 *         this prefix in the expression, as this might lead to unexpected results.
			 *
             * @param expression The expression to translate.
			 * @param createZ3Variables If set to true a solver variable is created for each variable in expression that is not
			 *                          yet known to the adapter. (i.e. values from the variableToExpressionMap passed to the constructor
			 *                          are not overwritten)
             * @return An equivalent expression for Z3.
             */
            z3::expr translateExpression(storm::expressions::Expression const& expression, bool createZ3Variables = false) {
				if (createZ3Variables) {
					std::map<std::string, storm::expressions::ExpressionReturnType> variables;

					try	{
						variables = expression.getVariablesAndTypes();
					}
					catch (storm::exceptions::InvalidTypeException* e) {
						LOG_THROW(true, storm::exceptions::InvalidTypeException, "Encountered variable with ambigious type while trying to autocreate solver variables: " << e);
					}

					for (auto variableAndType : variables) {
						if (this->variableToExpressionMap.find(variableAndType.first) == this->variableToExpressionMap.end()) {
							switch (variableAndType.second)
							{
								case storm::expressions::ExpressionReturnType::Bool:
									this->variableToExpressionMap.insert(std::make_pair(variableAndType.first, context.bool_const(variableAndType.first.c_str())));
									break;
								case storm::expressions::ExpressionReturnType::Int:
									this->variableToExpressionMap.insert(std::make_pair(variableAndType.first, context.int_const(variableAndType.first.c_str())));
									break;
								case storm::expressions::ExpressionReturnType::Double:
									this->variableToExpressionMap.insert(std::make_pair(variableAndType.first, context.real_const(variableAndType.first.c_str())));
									break;
								default:
									LOG_THROW(true, storm::exceptions::InvalidTypeException, "Encountered variable with unknown type while trying to autocreate solver variables: " << variableAndType.first);
									break;
							}
						}
					}
				}

                expression.getBaseExpression().accept(this);
                z3::expr result = stack.top();
                stack.pop();

				while (!additionalAssertions.empty()) {
					result = result && additionalAssertions.top();
					additionalAssertions.pop();
				}

                return result;
            }
            
            virtual void visit(storm::expressions::BinaryBooleanFunctionExpression const* expression) override {
                expression->getFirstOperand()->accept(this);
                expression->getSecondOperand()->accept(this);
                
                const z3::expr rightResult = stack.top();
                stack.pop();
				const z3::expr leftResult = stack.top();
                stack.pop();
                
                switch(expression->getOperatorType()) {
					case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::And:
                        stack.push(leftResult && rightResult);
                        break;
					case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::Or:
                        stack.push(leftResult || rightResult);
						break;
					case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::Xor:
						stack.push(z3::expr(context, Z3_mk_xor(context, leftResult, rightResult)));
						break;
					case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::Implies:
						stack.push(z3::expr(context, Z3_mk_implies(context, leftResult, rightResult)));
						break;
					case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::Iff:
						stack.push(z3::expr(context, Z3_mk_iff(context, leftResult, rightResult)));
						break;
                    default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
						<< "Unknown boolean binary operator: '" << expression->getOperatorType() << "' in expression " << expression << ".";
                }
                
            }
            
			virtual void visit(storm::expressions::BinaryNumericalFunctionExpression const* expression) override {
				expression->getFirstOperand()->accept(this);
				expression->getSecondOperand()->accept(this);
                
                z3::expr rightResult = stack.top();
                stack.pop();
                z3::expr leftResult = stack.top();
                stack.pop();
                
                switch(expression->getOperatorType()) {
					case storm::expressions::BinaryNumericalFunctionExpression::OperatorType::Plus:
                        stack.push(leftResult + rightResult);
                        break;
					case storm::expressions::BinaryNumericalFunctionExpression::OperatorType::Minus:
                        stack.push(leftResult - rightResult);
                        break;
					case storm::expressions::BinaryNumericalFunctionExpression::OperatorType::Times:
                        stack.push(leftResult * rightResult);
                        break;
					case storm::expressions::BinaryNumericalFunctionExpression::OperatorType::Divide:
                        stack.push(leftResult / rightResult);
                        break;
					case storm::expressions::BinaryNumericalFunctionExpression::OperatorType::Min:
                        stack.push(ite(leftResult <= rightResult, leftResult, rightResult));
                        break;
					case storm::expressions::BinaryNumericalFunctionExpression::OperatorType::Max:
                        stack.push(ite(leftResult >= rightResult, leftResult, rightResult));
                        break;
                    default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
						<< "Unknown numerical binary operator: '" << expression->getOperatorType() << "' in expression " << expression << ".";
                }
            }
            
			virtual void visit(storm::expressions::BinaryRelationExpression const* expression) override {
				expression->getFirstOperand()->accept(this);
                expression->getSecondOperand()->accept(this);
                
                z3::expr rightResult = stack.top();
                stack.pop();
                z3::expr leftResult = stack.top();
                stack.pop();
                
                switch(expression->getRelationType()) {
					case storm::expressions::BinaryRelationExpression::RelationType::Equal:
                        stack.push(leftResult == rightResult);
                        break;
					case storm::expressions::BinaryRelationExpression::RelationType::NotEqual:
                        stack.push(leftResult != rightResult);
                        break;
					case storm::expressions::BinaryRelationExpression::RelationType::Less:
                        stack.push(leftResult < rightResult);
                        break;
					case storm::expressions::BinaryRelationExpression::RelationType::LessOrEqual:
                        stack.push(leftResult <= rightResult);
                        break;
					case storm::expressions::BinaryRelationExpression::RelationType::Greater:
                        stack.push(leftResult > rightResult);
                        break;
					case storm::expressions::BinaryRelationExpression::RelationType::GreaterOrEqual:
                        stack.push(leftResult >= rightResult);
                        break;
                    default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
                        << "Unknown boolean binary operator: '" << expression->getRelationType() << "' in expression " << expression << ".";
                }    
            }
            
			virtual void visit(storm::expressions::BooleanLiteralExpression const* expression) override {
                stack.push(context.bool_val(expression->evaluateAsBool()));
            }
            
			virtual void visit(storm::expressions::DoubleLiteralExpression const* expression) override {
                std::stringstream fractionStream;
                fractionStream << expression->evaluateAsDouble();
                stack.push(context.real_val(fractionStream.str().c_str()));
            }
            
			virtual void visit(storm::expressions::IntegerLiteralExpression const* expression) override {
                stack.push(context.int_val(static_cast<int>(expression->evaluateAsInt())));
            }
            
			virtual void visit(storm::expressions::UnaryBooleanFunctionExpression const* expression) override {
                expression->getOperand()->accept(this);
                
                z3::expr childResult = stack.top();
                stack.pop();
                
                switch (expression->getOperatorType()) {
					case storm::expressions::UnaryBooleanFunctionExpression::OperatorType::Not:
                        stack.push(!childResult);
                        break;
                    default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
						<< "Unknown boolean binary operator: '" << expression->getOperatorType() << "' in expression " << expression << ".";
                }    
            }
            
			virtual void visit(storm::expressions::UnaryNumericalFunctionExpression const* expression) override {
                expression->getOperand()->accept(this);
                
                z3::expr childResult = stack.top();
                stack.pop();
                
                switch(expression->getOperatorType()) {
					case storm::expressions::UnaryNumericalFunctionExpression::OperatorType::Minus:
                        stack.push(0 - childResult);
						break;
					case storm::expressions::UnaryNumericalFunctionExpression::OperatorType::Floor: {
						z3::expr floorVariable = context.int_const(("__z3adapter_floor_" + std::to_string(additionalVariableCounter++)).c_str());
						additionalAssertions.push(z3::expr(context, Z3_mk_int2real(context, floorVariable)) <= childResult < (z3::expr(context, Z3_mk_int2real(context, floorVariable)) + 1));
						throw storm::exceptions::NotImplementedException() << "Unary numerical function 'floor' is not supported by Z3ExpressionAdapter.";
						break;
					}
					case storm::expressions::UnaryNumericalFunctionExpression::OperatorType::Ceil:{
						z3::expr ceilVariable = context.int_const(("__z3adapter_ceil_" + std::to_string(additionalVariableCounter++)).c_str());
						additionalAssertions.push(z3::expr(context, Z3_mk_int2real(context, ceilVariable)) - 1 <= childResult < z3::expr(context, Z3_mk_int2real(context, ceilVariable)));
						throw storm::exceptions::NotImplementedException() << "Unary numerical function 'floor' is not supported by Z3ExpressionAdapter.";
						break;
					}
                    default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
                        << "Unknown numerical unary operator: '" << expression->getOperatorType() << "'.";
                }
            }

			virtual void visit(storm::expressions::IfThenElseExpression const* expression) override {
				expression->getCondition()->accept(this);
				expression->getThenExpression()->accept(this);
				expression->getElseExpression()->accept(this);

				z3::expr conditionResult = stack.top();
				stack.pop();
				z3::expr thenResult = stack.top();
				stack.pop();
				z3::expr elseResult = stack.top();
				stack.pop();

				stack.push(z3::expr(context, Z3_mk_ite(context, conditionResult, thenResult, elseResult)));
			}
            
			virtual void visit(storm::expressions::VariableExpression const* expression) override {
                stack.push(variableToExpressionMap.at(expression->getVariableName()));
            }

        private:
            z3::context& context;
            std::stack<z3::expr> stack;
			std::stack<z3::expr> additionalAssertions;
			uint_fast64_t additionalVariableCounter;

            std::map<std::string, z3::expr> variableToExpressionMap;
        };
        
    } // namespace adapters
} // namespace storm

#endif /* STORM_ADAPTERS_Z3EXPRESSIONADAPTER_H_ */
