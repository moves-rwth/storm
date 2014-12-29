#ifndef STORM_ADAPTERS_Z3EXPRESSIONADAPTER_H_
#define STORM_ADAPTERS_Z3EXPRESSIONADAPTER_H_

#include <stack>

// Include the headers of Z3 only if it is available.
#ifdef STORM_HAVE_Z3
#include "z3++.h"
#include "z3.h"
#endif

#include "storm-config.h"
#include "src/storage/expressions/Expressions.h"
#include "src/utility/macros.h"
#include "src/exceptions/ExpressionEvaluationException.h"
#include "src/exceptions/InvalidTypeException.h"
#include "src/exceptions/NotImplementedException.h"

namespace storm {
    namespace adapters {

#ifdef STORM_HAVE_Z3
        class Z3ExpressionAdapter : public storm::expressions::ExpressionVisitor {
        public:
            /*!
             * Creates an expression adapter that can translate expressions to the format of Z3.
			 *
			 * @warning The adapter internally creates helper variables prefixed with `__z3adapter_`. As a consequence,
             * having variables with this prefix in the variableToExpressionMap might lead to unexpected results and is
             * strictly to be avoided.
             *
             * @param context A reference to the Z3 context over which to build the expressions. The lifetime of the
             * context needs to be guaranteed as long as the instance of this adapter is used.
             * @param createVariables If set to true, additional variables will be created for variables that appear in
             * expressions and are not yet known to the adapter.
             * @param variableToExpressionMap A mapping from variable names to their corresponding Z3 expressions (if already existing).
             */
            Z3ExpressionAdapter(z3::context& context, bool createVariables = true, std::map<std::string, z3::expr> const& variableToExpressionMap = std::map<std::string, z3::expr>()) : context(context), additionalAssertions(), additionalVariableCounter(0), variableToExpressionMap(variableToExpressionMap), createVariables(createVariables) {
                // Intentionally left empty.
            }
            
            /*!
             * Translates the given expression to an equivalent expression for Z3.
			 *
			 * @warning The adapter internally creates helper variables prefixed with `__z3adapter_`. As a consequence,
             * having variables with this prefix in the variableToExpressionMap might lead to unexpected results and is
             * strictly to be aboost::anyed.
			 *
             * @param expression The expression to translate.
             * @return An equivalent expression for Z3.
             */
            z3::expr translateExpression(storm::expressions::Expression const& expression) {
                z3::expr result = boost::any_cast<z3::expr>(expression.getBaseExpression().accept(*this));

                for (z3::expr const& assertion : additionalAssertions) {
                    result = result && assertion;
                }
                additionalAssertions.clear();

                return result;
            }
            
			storm::expressions::Expression translateExpression(z3::expr const& expr) {
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
							STORM_LOG_THROW(args != 0, storm::exceptions::ExpressionEvaluationException, "Failed to convert Z3 expression. 0-ary AND is assumed to be an error.");
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
							STORM_LOG_THROW(args != 0, storm::exceptions::ExpressionEvaluationException, "Failed to convert Z3 expression. 0-ary OR is assumed to be an error.");
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
							// Arithmetic numeral.
							if (expr.is_int() && expr.is_const()) {
								long long value;
								if (Z3_get_numeral_int64(expr.ctx(), expr, &value)) {
									return storm::expressions::Expression::createIntegerLiteral(value);
								} else {
									STORM_LOG_THROW(false, storm::exceptions::ExpressionEvaluationException, "Failed to convert Z3 expression. Expression is constant integer and value does not fit into 64-bit integer.");
								}
							} else if (expr.is_real() && expr.is_const()) {
								long long num;
								long long den;
								if (Z3_get_numeral_rational_int64(expr.ctx(), expr, &num, &den)) {
									return storm::expressions::Expression::createDoubleLiteral(static_cast<double>(num) / static_cast<double>(den));
								} else {
									STORM_LOG_THROW(false, storm::exceptions::ExpressionEvaluationException, "Failed to convert Z3 expression. Expression is constant real and value does not fit into a fraction with 64-bit integer numerator and denominator.");
								}
							}
						case Z3_OP_UNINTERPRETED:
							// Currently, we only support uninterpreted constant functions.
							STORM_LOG_THROW(expr.is_const(), storm::exceptions::ExpressionEvaluationException, "Failed to convert Z3 expression. Encountered non-constant uninterpreted function.");
							if (expr.is_bool()) {
								return storm::expressions::Expression::createBooleanVariable(expr.decl().name().str());
							} else if (expr.is_int()) {
								return storm::expressions::Expression::createIntegerVariable(expr.decl().name().str());
							} else if (expr.is_real()) {
								return storm::expressions::Expression::createDoubleVariable(expr.decl().name().str());
							} else {
								STORM_LOG_THROW(false, storm::exceptions::ExpressionEvaluationException, "Failed to convert Z3 expression. Encountered constant uninterpreted function of unknown sort.");
							}
							
						default:
							STORM_LOG_THROW(false, storm::exceptions::ExpressionEvaluationException, "Failed to convert Z3 expression. Encountered unhandled Z3_decl_kind " << expr.decl().kind() <<".");
							break;
					}
				} else {
					STORM_LOG_THROW(false, storm::exceptions::ExpressionEvaluationException, "Failed to convert Z3 expression. Encountered unknown expression type.");
				}
			}

            virtual boost::any visit(storm::expressions::BinaryBooleanFunctionExpression const& expression) override {
                z3::expr leftResult = boost::any_cast<z3::expr>(expression.getFirstOperand()->accept(*this));
                z3::expr rightResult = boost::any_cast<z3::expr>(expression.getSecondOperand()->accept(*this));
                
                switch(expression.getOperatorType()) {
					case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::And:
                        return leftResult && rightResult;
					case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::Or:
                        return leftResult || rightResult;
					case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::Xor:
						return z3::expr(context, Z3_mk_xor(context, leftResult, rightResult));
					case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::Implies:
						return z3::expr(context, Z3_mk_implies(context, leftResult, rightResult));
					case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::Iff:
						return z3::expr(context, Z3_mk_iff(context, leftResult, rightResult));
                    default:
                        STORM_LOG_THROW(false, storm::exceptions::ExpressionEvaluationException, "Cannot evaluate expression: unknown boolean binary operator '" << static_cast<int>(expression.getOperatorType()) << "' in expression " << expression << ".");
                }
                
            }
            
			virtual boost::any visit(storm::expressions::BinaryNumericalFunctionExpression const& expression) override {
                z3::expr leftResult = boost::any_cast<z3::expr>(expression.getFirstOperand()->accept(*this));
                z3::expr rightResult = boost::any_cast<z3::expr>(expression.getSecondOperand()->accept(*this));
        
                switch(expression.getOperatorType()) {
					case storm::expressions::BinaryNumericalFunctionExpression::OperatorType::Plus:
                        return leftResult + rightResult;
					case storm::expressions::BinaryNumericalFunctionExpression::OperatorType::Minus:
                        return leftResult - rightResult;
					case storm::expressions::BinaryNumericalFunctionExpression::OperatorType::Times:
                        return leftResult * rightResult;
					case storm::expressions::BinaryNumericalFunctionExpression::OperatorType::Divide:
                        return leftResult / rightResult;
					case storm::expressions::BinaryNumericalFunctionExpression::OperatorType::Min:
                        return ite(leftResult <= rightResult, leftResult, rightResult);
					case storm::expressions::BinaryNumericalFunctionExpression::OperatorType::Max:
                        return ite(leftResult >= rightResult, leftResult, rightResult);
                    default:
                        STORM_LOG_THROW(false, storm::exceptions::ExpressionEvaluationException, "Cannot evaluate expression: unknown numerical binary operator '" << static_cast<int>(expression.getOperatorType()) << "' in expression " << expression << ".");
                }
            }
            
			virtual boost::any visit(storm::expressions::BinaryRelationExpression const& expression) override {
                z3::expr leftResult = boost::any_cast<z3::expr>(expression.getFirstOperand()->accept(*this));
                z3::expr rightResult = boost::any_cast<z3::expr>(expression.getSecondOperand()->accept(*this));
    
                switch(expression.getRelationType()) {
					case storm::expressions::BinaryRelationExpression::RelationType::Equal:
                        return leftResult == rightResult;
					case storm::expressions::BinaryRelationExpression::RelationType::NotEqual:
                        return leftResult != rightResult;
					case storm::expressions::BinaryRelationExpression::RelationType::Less:
                        return leftResult < rightResult;
					case storm::expressions::BinaryRelationExpression::RelationType::LessOrEqual:
                        return leftResult <= rightResult;
					case storm::expressions::BinaryRelationExpression::RelationType::Greater:
                        return leftResult > rightResult;
					case storm::expressions::BinaryRelationExpression::RelationType::GreaterOrEqual:
                        return leftResult >= rightResult;
                    default:
                        STORM_LOG_THROW(false, storm::exceptions::ExpressionEvaluationException, "Cannot evaluate expression: unknown boolean binary operator '" << static_cast<int>(expression.getRelationType()) << "' in expression " << expression << ".");
                }    
            }
            
			virtual boost::any visit(storm::expressions::BooleanLiteralExpression const& expression) override {
                return context.bool_val(expression.getValue());
            }
            
			virtual boost::any visit(storm::expressions::DoubleLiteralExpression const& expression) override {
                std::stringstream fractionStream;
                fractionStream << expression.getValue();
                return context.real_val(fractionStream.str().c_str());
            }
            
			virtual boost::any visit(storm::expressions::IntegerLiteralExpression const& expression) override {
                return context.int_val(static_cast<int>(expression.getValue()));
            }
            
			virtual boost::any visit(storm::expressions::UnaryBooleanFunctionExpression const& expression) override {
                z3::expr childResult = boost::any_cast<z3::expr>(expression.getOperand()->accept(*this));
                
                switch (expression.getOperatorType()) {
					case storm::expressions::UnaryBooleanFunctionExpression::OperatorType::Not:
                        return !childResult;
                    default:
                        STORM_LOG_THROW(false, storm::exceptions::ExpressionEvaluationException, "Cannot evaluate expression: unknown boolean binary operator '" << static_cast<int>(expression.getOperatorType()) << "' in expression " << expression << ".");
                }    
            }
            
			virtual boost::any visit(storm::expressions::UnaryNumericalFunctionExpression const& expression) override {
                z3::expr childResult = boost::any_cast<z3::expr>(expression.getOperand()->accept(*this));
                
                switch(expression.getOperatorType()) {
					case storm::expressions::UnaryNumericalFunctionExpression::OperatorType::Minus:
                        return 0 - childResult;
					case storm::expressions::UnaryNumericalFunctionExpression::OperatorType::Floor: {
						z3::expr floorVariable = context.int_const(("__z3adapter_floor_" + std::to_string(additionalVariableCounter++)).c_str());
						additionalAssertions.push_back(z3::expr(context, Z3_mk_int2real(context, floorVariable)) <= childResult && childResult < (z3::expr(context, Z3_mk_int2real(context, floorVariable)) + 1));
						return floorVariable;
					}
					case storm::expressions::UnaryNumericalFunctionExpression::OperatorType::Ceil:{
						z3::expr ceilVariable = context.int_const(("__z3adapter_ceil_" + std::to_string(additionalVariableCounter++)).c_str());
						additionalAssertions.push_back(z3::expr(context, Z3_mk_int2real(context, ceilVariable)) - 1 <= childResult && childResult < z3::expr(context, Z3_mk_int2real(context, ceilVariable)));
						return ceilVariable;
					}
                    default: STORM_LOG_THROW(false, storm::exceptions::ExpressionEvaluationException, "Cannot evaluate expression: unknown numerical unary operator '" << static_cast<int>(expression.getOperatorType()) << "'.");
                }
            }

			virtual boost::any visit(storm::expressions::IfThenElseExpression const& expression) override {
                z3::expr conditionResult = boost::any_cast<z3::expr>(expression.getCondition()->accept(*this));
                z3::expr thenResult = boost::any_cast<z3::expr>(expression.getThenExpression()->accept(*this));
                z3::expr elseResult = boost::any_cast<z3::expr>(expression.getElseExpression()->accept(*this));
				return z3::expr(context, Z3_mk_ite(context, conditionResult, thenResult, elseResult));
			}
            
			virtual boost::any visit(storm::expressions::VariableExpression const& expression) override {
                std::map<std::string, z3::expr>::iterator stringVariablePair = variableToExpressionMap.find(expression.getVariableName());
                z3::expr result(context);
                
				if (stringVariablePair == variableToExpressionMap.end() && createVariables) {
                    std::pair<std::map<std::string, z3::expr>::iterator, bool> iteratorAndFlag;
                    switch (expression.getReturnType()) {
                        case storm::expressions::ExpressionReturnType::Bool:
                            iteratorAndFlag = this->variableToExpressionMap.insert(std::make_pair(expression.getVariableName(), context.bool_const(expression.getVariableName().c_str())));
                            result = iteratorAndFlag.first->second;
                            break;
                        case storm::expressions::ExpressionReturnType::Int:
                            iteratorAndFlag = this->variableToExpressionMap.insert(std::make_pair(expression.getVariableName(), context.int_const(expression.getVariableName().c_str())));
                            result = iteratorAndFlag.first->second;
                            break;
                        case storm::expressions::ExpressionReturnType::Double:
                            iteratorAndFlag = this->variableToExpressionMap.insert(std::make_pair(expression.getVariableName(), context.real_const(expression.getVariableName().c_str())));
                            result = iteratorAndFlag.first->second;
                            break;
                        default:
                            STORM_LOG_THROW(false, storm::exceptions::InvalidTypeException, "Encountered variable '" << expression.getVariableName() << "' with unknown type while trying to create solver variables.");
                    }
                } else {
                    STORM_LOG_THROW(stringVariablePair != variableToExpressionMap.end(), storm::exceptions::InvalidArgumentException, "Expression refers to unknown variable '" << expression.getVariableName() << "'.");
                    result = stringVariablePair->second;
                }
                
                return result;
            }

        private:
            // The context that is used to translate the expressions.
            z3::context& context;

            // A stack of assertions that need to be kept separate, because they were only impliclty part of an assertion that was added.
			std::vector<z3::expr> additionalAssertions;

            // A counter for the variables that were created to identify the additional assertions.
			uint_fast64_t additionalVariableCounter;

            // A mapping from variable names to their Z3 equivalent.
            std::map<std::string, z3::expr> variableToExpressionMap;

            // A flag that indicates whether new variables are to be created when an unkown variable is encountered.
            bool createVariables;
        };
#endif
    } // namespace adapters
} // namespace storm

#endif /* STORM_ADAPTERS_Z3EXPRESSIONADAPTER_H_ */
