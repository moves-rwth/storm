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

#include "storm-config.h"
#include "src/storage/expressions/Expressions.h"
#include "src/exceptions/ExceptionMacros.h"
#include "src/exceptions/ExpressionEvaluationException.h"
#include "src/exceptions/InvalidTypeException.h"
#include "src/exceptions/NotImplementedException.h"

namespace storm {
    namespace adapters {

#ifdef STORM_HAVE_Z3
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
						LOG_THROW(false, storm::exceptions::InvalidTypeException, "Encountered variable with ambigious type while trying to autocreate solver variables: " << e);
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
									LOG_THROW(false, storm::exceptions::InvalidTypeException, "Encountered variable with unknown type while trying to autocreate solver variables: " << variableAndType.first);
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
            
			storm::expressions::Expression translateExpression(z3::expr const& expr) {
				//std::cout << std::boolalpha << expr.is_var() << std::endl;
				//std::cout << std::boolalpha << expr.is_app() << std::endl;
				//std::cout << expr.decl().decl_kind() << std::endl;

				/*
				if (expr.is_bool() && expr.is_const()) {
					switch (Z3_get_bool_value(expr.ctx(), expr)) {
						case Z3_L_FALSE:
							return storm::expressions::Expression::createFalse();
						case Z3_L_TRUE:
							return storm::expressions::Expression::createTrue();
							break;
						default:
							LOG_THROW(false, storm::exceptions::ExpressionEvaluationException, "Failed to convert Z3 expression. Expression is constant boolean, but value is undefined.");
							break;
					}
				} else if (expr.is_int() && expr.is_const()) {
					int_fast64_t value;
					if (Z3_get_numeral_int64(expr.ctx(), expr, &value)) {
						return storm::expressions::Expression::createIntegerLiteral(value);
					} else {
						LOG_THROW(false, storm::exceptions::ExpressionEvaluationException, "Failed to convert Z3 expression. Expression is constant integer and value does not fit into 64-bit integer.");
					}
				} else if (expr.is_real() && expr.is_const()) {
					int_fast64_t num;
					int_fast64_t den;
					if (Z3_get_numeral_rational_int64(expr.ctx(), expr, &num, &den)) {
						return storm::expressions::Expression::createDoubleLiteral(static_cast<double>(num) / static_cast<double>(den));
					} else {
						LOG_THROW(false, storm::exceptions::ExpressionEvaluationException, "Failed to convert Z3 expression. Expression is constant real and value does not fit into a fraction with 64-bit integer numerator and denominator.");
					}
					} else */
				if (expr.is_app()) {
					switch (expr.decl().decl_kind()) {
						case Z3_OP_TRUE:
							return storm::expressions::Expression::createTrue();
						case Z3_OP_FALSE:
							return storm::expressions::Expression::createFalse();
						case Z3_OP_EQ:
							return this->translateExpression(expr.arg(0)) == this->translateExpression(expr.arg(1));
						case Z3_OP_ITE:
							return this->translateExpression(expr.arg(0)).ite(this->translateExpression(expr.arg(1)), this->translateExpression(expr.arg(2)));
						case Z3_OP_AND: {
							unsigned args = expr.num_args();
							LOG_THROW(args != 0, storm::exceptions::ExpressionEvaluationException, "Failed to convert Z3 expression. 0-ary AND is assumed to be an error.");
							if (args == 1) {
								return this->translateExpression(expr.arg(0));
							} else {
								storm::expressions::Expression retVal = this->translateExpression(expr.arg(0));
								for (unsigned i = 1; i < args; i++) {
									retVal = retVal && this->translateExpression(expr.arg(i));
								}
								return retVal;
							}
						}
						case Z3_OP_OR: {
							unsigned args = expr.num_args();
							LOG_THROW(args != 0, storm::exceptions::ExpressionEvaluationException, "Failed to convert Z3 expression. 0-ary OR is assumed to be an error.");
							if (args == 1) {
								return this->translateExpression(expr.arg(0));
							} else {
								storm::expressions::Expression retVal = this->translateExpression(expr.arg(0));
								for (unsigned i = 1; i < args; i++) {
									retVal = retVal || this->translateExpression(expr.arg(i));
								}
								return retVal;
							}
						}
						case Z3_OP_IFF:
							return this->translateExpression(expr.arg(0)).iff(this->translateExpression(expr.arg(1)));
						case Z3_OP_XOR:
							return this->translateExpression(expr.arg(0)) ^ this->translateExpression(expr.arg(1));
						case Z3_OP_NOT:
							return !this->translateExpression(expr.arg(0));
						case Z3_OP_IMPLIES:
							return this->translateExpression(expr.arg(0)).implies(this->translateExpression(expr.arg(1)));
						case Z3_OP_LE:
							return this->translateExpression(expr.arg(0)) <= this->translateExpression(expr.arg(1));
						case Z3_OP_GE:
							return this->translateExpression(expr.arg(0)) >= this->translateExpression(expr.arg(1));
						case Z3_OP_LT:
							return this->translateExpression(expr.arg(0)) < this->translateExpression(expr.arg(1));
						case Z3_OP_GT:
							return this->translateExpression(expr.arg(0)) > this->translateExpression(expr.arg(1));
						case Z3_OP_ADD:
							return this->translateExpression(expr.arg(0)) + this->translateExpression(expr.arg(1));
						case Z3_OP_SUB:
							return this->translateExpression(expr.arg(0)) - this->translateExpression(expr.arg(1));
						case Z3_OP_UMINUS:
							return -this->translateExpression(expr.arg(0));
						case Z3_OP_MUL:
							return this->translateExpression(expr.arg(0)) * this->translateExpression(expr.arg(1));
						case Z3_OP_DIV:
							return this->translateExpression(expr.arg(0)) / this->translateExpression(expr.arg(1));
						case Z3_OP_IDIV:
							return this->translateExpression(expr.arg(0)) / this->translateExpression(expr.arg(1));
						case Z3_OP_ANUM:
							//Arithmetic numeral
							if (expr.is_int() && expr.is_const()) {
								long long value;
								if (Z3_get_numeral_int64(expr.ctx(), expr, &value)) {
									return storm::expressions::Expression::createIntegerLiteral(value);
								} else {
									LOG_THROW(false, storm::exceptions::ExpressionEvaluationException, "Failed to convert Z3 expression. Expression is constant integer and value does not fit into 64-bit integer.");
								}
							} else if (expr.is_real() && expr.is_const()) {
								long long num;
								long long den;
								if (Z3_get_numeral_rational_int64(expr.ctx(), expr, &num, &den)) {
									return storm::expressions::Expression::createDoubleLiteral(static_cast<double>(num) / static_cast<double>(den));
								} else {
									LOG_THROW(false, storm::exceptions::ExpressionEvaluationException, "Failed to convert Z3 expression. Expression is constant real and value does not fit into a fraction with 64-bit integer numerator and denominator.");
								}
							}
						case Z3_OP_UNINTERPRETED:
							//storm only supports uninterpreted constant functions
							LOG_THROW(expr.is_const(), storm::exceptions::ExpressionEvaluationException, "Failed to convert Z3 expression. Encountered non constant uninterpreted function.");
							if (expr.is_bool()) {
								return storm::expressions::Expression::createBooleanVariable(expr.decl().name().str());
							} else if (expr.is_int()) {
								return storm::expressions::Expression::createIntegerVariable(expr.decl().name().str());
							} else if (expr.is_real()) {
								return storm::expressions::Expression::createDoubleVariable(expr.decl().name().str());
							} else {
								LOG_THROW(false, storm::exceptions::ExpressionEvaluationException, "Failed to convert Z3 expression. Encountered constant uninterpreted function of unknown sort.");
							}
							
						default:
							LOG_THROW(false, storm::exceptions::ExpressionEvaluationException, "Failed to convert Z3 expression. Encountered unhandled Z3_decl_kind " << expr.decl().kind() <<".");
							break;
					}
				} else {
					LOG_THROW(false, storm::exceptions::ExpressionEvaluationException, "Failed to convert Z3 expression. Encountered unknown expression type.");
				}
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
						<< "Unknown boolean binary operator: '" << static_cast<int>(expression->getOperatorType()) << "' in expression " << expression << ".";
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
						<< "Unknown numerical binary operator: '" << static_cast<int>(expression->getOperatorType()) << "' in expression " << expression << ".";
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
                        << "Unknown boolean binary operator: '" << static_cast<int>(expression->getRelationType()) << "' in expression " << expression << ".";
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
						<< "Unknown boolean binary operator: '" << static_cast<int>(expression->getOperatorType()) << "' in expression " << expression << ".";
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
						additionalAssertions.push(z3::expr(context, Z3_mk_int2real(context, floorVariable)) <= childResult && childResult < (z3::expr(context, Z3_mk_int2real(context, floorVariable)) + 1));
						stack.push(floorVariable);
						//throw storm::exceptions::NotImplementedException() << "Unary numerical function 'floor' is not supported by Z3ExpressionAdapter.";
						break;
					}
					case storm::expressions::UnaryNumericalFunctionExpression::OperatorType::Ceil:{
						z3::expr ceilVariable = context.int_const(("__z3adapter_ceil_" + std::to_string(additionalVariableCounter++)).c_str());
						additionalAssertions.push(z3::expr(context, Z3_mk_int2real(context, ceilVariable)) - 1 <= childResult && childResult < z3::expr(context, Z3_mk_int2real(context, ceilVariable)));
						stack.push(ceilVariable);
						//throw storm::exceptions::NotImplementedException() << "Unary numerical function 'floor' is not supported by Z3ExpressionAdapter.";
						break;
					}
                    default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
                        << "Unknown numerical unary operator: '" << static_cast<int>(expression->getOperatorType()) << "'.";
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
#endif
    } // namespace adapters
} // namespace storm

#endif /* STORM_ADAPTERS_Z3EXPRESSIONADAPTER_H_ */
