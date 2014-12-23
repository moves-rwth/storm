#ifndef STORM_ADAPTERS_MATHSATEXPRESSIONADAPTER_H_
#define STORM_ADAPTERS_MATHSATEXPRESSIONADAPTER_H_

#include "storm-config.h"

#include <stack>

#ifdef STORM_HAVE_MSAT
#include "mathsat.h"
#endif

#include "storage/expressions/Expressions.h"
#include "storage/expressions/ExpressionVisitor.h"
#include "src/utility/macros.h"
#include "src/exceptions/ExpressionEvaluationException.h"
#include "src/exceptions/InvalidTypeException.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/NotImplementedException.h"

namespace storm {
	namespace adapters {

#ifdef STORM_HAVE_MSAT
		class MathsatExpressionAdapter : public storm::expressions::ExpressionVisitor {
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
             * @param variableToDeclarationMap A mapping from variable names to their corresponding MathSAT declarations (if already existing).
             */
			MathsatExpressionAdapter(msat_env& env, bool createVariables = true, std::map<std::string, msat_decl> const& variableToDeclarationMap = std::map<std::string, msat_decl>()) : env(env), stack(), variableToDeclarationMap(variableToDeclarationMap), createVariables(createVariables) {
				// Intentionally left empty.
			}

			/*!
			* Translates the given expression to an equivalent term for MathSAT.
			*
			* @param expression The expression to be translated.
			* @return An equivalent term for MathSAT.
			*/
			msat_term translateExpression(storm::expressions::Expression const& expression) {
				expression.getBaseExpression().accept(this);
				msat_term result = stack.top();
				stack.pop();
                STORM_LOG_THROW(!MSAT_ERROR_TERM(result), storm::exceptions::ExpressionEvaluationException, "Could not translate expression to MathSAT's format.");
				return result;
			}

			virtual void visit(expressions::BinaryBooleanFunctionExpression const* expression) override {
				expression->getFirstOperand()->accept(this);
				expression->getSecondOperand()->accept(this);

				msat_term rightResult = stack.top();
				stack.pop();
				msat_term leftResult = stack.top();
				stack.pop();

				switch (expression->getOperatorType()) {
					case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::And:
						stack.push(msat_make_and(env, leftResult, rightResult));
						break;
					case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::Or:
						stack.push(msat_make_or(env, leftResult, rightResult));
						break;
					case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::Iff:
						stack.push(msat_make_iff(env, leftResult, rightResult));
						break;
                    case storm::expressions::BinaryBooleanFunctionExpression::OperatorType::Implies:
                        stack.push(msat_make_or(env, msat_make_not(env, leftResult), rightResult));
                        break;
					default:
                    STORM_LOG_THROW(false, storm::exceptions::ExpressionEvaluationException, "Cannot evaluate expression: unknown boolean binary operator '" << static_cast<uint_fast64_t>(expression->getOperatorType()) << "' in expression " << expression << ".");
				}

			}

			virtual void visit(expressions::BinaryNumericalFunctionExpression const* expression) override {
				expression->getFirstOperand()->accept(this);
				expression->getSecondOperand()->accept(this);

				msat_term rightResult = stack.top();
				stack.pop();
				msat_term leftResult = stack.top();
				stack.pop();

				switch (expression->getOperatorType()) {
					case storm::expressions::BinaryNumericalFunctionExpression::OperatorType::Plus:
						stack.push(msat_make_plus(env, leftResult, rightResult));
						break;
					case storm::expressions::BinaryNumericalFunctionExpression::OperatorType::Minus:
						stack.push(msat_make_plus(env, leftResult, msat_make_times(env, msat_make_number(env, "-1"), rightResult)));
						break;
					case storm::expressions::BinaryNumericalFunctionExpression::OperatorType::Times:
						stack.push(msat_make_times(env, leftResult, rightResult));
						break;
					case storm::expressions::BinaryNumericalFunctionExpression::OperatorType::Divide:
						throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
							<< "Unsupported numerical binary operator: '/' (division) in expression " << expression << ".";
						break;
					case storm::expressions::BinaryNumericalFunctionExpression::OperatorType::Min:
						stack.push(msat_make_term_ite(env, msat_make_leq(env, leftResult, rightResult), leftResult, rightResult));
						break;
					case storm::expressions::BinaryNumericalFunctionExpression::OperatorType::Max:
						stack.push(msat_make_term_ite(env, msat_make_leq(env, leftResult, rightResult), rightResult, leftResult));
						break;
					default:
                        STORM_LOG_THROW(false, storm::exceptions::ExpressionEvaluationException, "Cannot evaluate expression: unknown numerical binary operator '" << static_cast<uint_fast64_t>(expression->getOperatorType()) << "' in expression " << expression << ".");
				}
			}

			virtual void visit(expressions::BinaryRelationExpression const* expression) override {
				expression->getFirstOperand()->accept(this);
				expression->getSecondOperand()->accept(this);

				msat_term rightResult = stack.top();
				stack.pop();
				msat_term leftResult = stack.top();
				stack.pop();

				switch (expression->getRelationType()) {
					case storm::expressions::BinaryRelationExpression::RelationType::Equal:
						if (expression->getFirstOperand()->getReturnType() == storm::expressions::ExpressionReturnType::Bool && expression->getSecondOperand()->getReturnType() == storm::expressions::ExpressionReturnType::Bool) {
							stack.push(msat_make_iff(env, leftResult, rightResult));
						} else {
							stack.push(msat_make_equal(env, leftResult, rightResult));
						}
						break;
					case storm::expressions::BinaryRelationExpression::RelationType::NotEqual:
						if (expression->getFirstOperand()->getReturnType() == storm::expressions::ExpressionReturnType::Bool && expression->getSecondOperand()->getReturnType() == storm::expressions::ExpressionReturnType::Bool) {
							stack.push(msat_make_not(env, msat_make_iff(env, leftResult, rightResult)));
						} else {
							stack.push(msat_make_not(env, msat_make_equal(env, leftResult, rightResult)));
						}
						break;
					case storm::expressions::BinaryRelationExpression::RelationType::Less:
						stack.push(msat_make_and(env, msat_make_not(env, msat_make_equal(env, leftResult, rightResult)), msat_make_leq(env, leftResult, rightResult)));
						break;
					case storm::expressions::BinaryRelationExpression::RelationType::LessOrEqual:
						stack.push(msat_make_leq(env, leftResult, rightResult));
						break;
					case storm::expressions::BinaryRelationExpression::RelationType::Greater:
						stack.push(msat_make_not(env, msat_make_leq(env, leftResult, rightResult)));
						break;
					case storm::expressions::BinaryRelationExpression::RelationType::GreaterOrEqual:
						stack.push(msat_make_or(env, msat_make_equal(env, leftResult, rightResult), msat_make_not(env, msat_make_leq(env, leftResult, rightResult))));
						break;
					default:
                        STORM_LOG_THROW(false, storm::exceptions::ExpressionEvaluationException, "Cannot evaluate expression: unknown boolean binary operator '" << static_cast<uint_fast64_t>(expression->getRelationType()) << "' in expression " << expression << ".");
				}
			}

			virtual void visit(storm::expressions::IfThenElseExpression const* expression) override {
				expression->getCondition()->accept(this);
				expression->getThenExpression()->accept(this);
				expression->getElseExpression()->accept(this);

				msat_term conditionResult = stack.top();
				stack.pop();
				msat_term thenResult = stack.top();
				stack.pop();
				msat_term elseResult = stack.top();
				stack.pop();

				stack.push(msat_make_term_ite(env, conditionResult, thenResult, elseResult));
			}

			virtual void visit(expressions::BooleanLiteralExpression const* expression) override {
				stack.push(expression->evaluateAsBool(nullptr) ? msat_make_true(env) : msat_make_false(env));
			}

			virtual void visit(expressions::DoubleLiteralExpression const* expression) override {
				stack.push(msat_make_number(env, std::to_string(expression->evaluateAsDouble(nullptr)).c_str()));
			}

			virtual void visit(expressions::IntegerLiteralExpression const* expression) override {
				stack.push(msat_make_number(env, std::to_string(static_cast<int>(expression->evaluateAsInt(nullptr))).c_str()));
			}

			virtual void visit(expressions::UnaryBooleanFunctionExpression const* expression) override {
				expression->getOperand()->accept(this);

				msat_term childResult = stack.top();
				stack.pop();

				switch (expression->getOperatorType()) {
					case storm::expressions::UnaryBooleanFunctionExpression::OperatorType::Not:
						stack.push(msat_make_not(env, childResult));
						break;
					default:
                    STORM_LOG_THROW(false, storm::exceptions::ExpressionEvaluationException, "Cannot evaluate expression: unknown boolean unary operator: '" << static_cast<uint_fast64_t>(expression->getOperatorType()) << "' in expression " << expression << ".");
				}
			}

			virtual void visit(expressions::UnaryNumericalFunctionExpression const* expression) override {
				expression->getOperand()->accept(this);

				msat_term childResult = stack.top();
				stack.pop();

				switch (expression->getOperatorType()) {
					case storm::expressions::UnaryNumericalFunctionExpression::OperatorType::Minus:
						stack.push(msat_make_times(env, msat_make_number(env, "-1"), childResult));
						break;
                    case storm::expressions::UnaryNumericalFunctionExpression::OperatorType::Floor:
                        stack.push(msat_make_floor(env, childResult));
                        break;
                    case storm::expressions::UnaryNumericalFunctionExpression::OperatorType::Ceil:
                        stack.push(msat_make_plus(env, msat_make_floor(env, childResult), msat_make_number(env, "1")));
                        break;
					default:
                        STORM_LOG_THROW(false, storm::exceptions::ExpressionEvaluationException, "Cannot evaluate expression: unknown numerical unary operator: '" << static_cast<uint_fast64_t>(expression->getOperatorType()) << "' in expression " << expression << ".");
				}
			}

			virtual void visit(expressions::VariableExpression const* expression) override {
                std::map<std::string, msat_decl>::iterator stringVariablePair = variableToDeclarationMap.find(expression->getVariableName());
                msat_decl result;
                
                if (stringVariablePair == variableToDeclarationMap.end() && createVariables) {
                    std::pair<std::map<std::string, msat_decl>::iterator, bool> iteratorAndFlag;
                    switch (expression->getReturnType()) {
                        case storm::expressions::ExpressionReturnType::Bool:
                            iteratorAndFlag = this->variableToDeclarationMap.insert(std::make_pair(expression->getVariableName(), msat_declare_function(env, expression->getVariableName().c_str(), msat_get_bool_type(env))));
                            result = iteratorAndFlag.first->second;
                            break;
                        case storm::expressions::ExpressionReturnType::Int:
                            iteratorAndFlag = this->variableToDeclarationMap.insert(std::make_pair(expression->getVariableName(), msat_declare_function(env, expression->getVariableName().c_str(), msat_get_integer_type(env))));
                            result = iteratorAndFlag.first->second;
                            break;
                        case storm::expressions::ExpressionReturnType::Double:
                            iteratorAndFlag = this->variableToDeclarationMap.insert(std::make_pair(expression->getVariableName(), msat_declare_function(env, expression->getVariableName().c_str(), msat_get_rational_type(env))));
                            result = iteratorAndFlag.first->second;
                            break;
                        default:
                            STORM_LOG_THROW(false, storm::exceptions::InvalidTypeException, "Encountered variable '" << expression->getVariableName() << "' with unknown type while trying to create solver variables.");
                    }
                } else {
                    STORM_LOG_THROW(stringVariablePair != variableToDeclarationMap.end(), storm::exceptions::InvalidArgumentException, "Expression refers to unknown variable '" << expression->getVariableName() << "'.");
                    result = stringVariablePair->second;
                }

                STORM_LOG_THROW(!MSAT_ERROR_DECL(result), storm::exceptions::ExpressionEvaluationException, "Unable to translate expression to MathSAT format, because a variable could not be translated.");
				stack.push(msat_make_constant(env, result));
			}

            storm::expressions::Expression translateExpression(msat_term const& term) {
				if (msat_term_is_and(env, term)) {
					return translateExpression(msat_term_get_arg(term, 0)) && translateExpression(msat_term_get_arg(term, 1));
				} else if (msat_term_is_or(env, term)) {
					return translateExpression(msat_term_get_arg(term, 0)) || translateExpression(msat_term_get_arg(term, 1));
				} else if (msat_term_is_iff(env, term)) {
					return translateExpression(msat_term_get_arg(term, 0)).iff(translateExpression(msat_term_get_arg(term, 1)));
				} else if (msat_term_is_not(env, term)) {
                    return !translateExpression(msat_term_get_arg(term, 0));
				} else if (msat_term_is_plus(env, term)) {
                    return translateExpression(msat_term_get_arg(term, 0)) + translateExpression(msat_term_get_arg(term, 1));
				} else if (msat_term_is_times(env, term)) {
                    return translateExpression(msat_term_get_arg(term, 0)) * translateExpression(msat_term_get_arg(term, 1));
				} else if (msat_term_is_equal(env, term)) {
                    return translateExpression(msat_term_get_arg(term, 0)) == translateExpression(msat_term_get_arg(term, 1));
				} else if (msat_term_is_leq(env, term)) {
                    return translateExpression(msat_term_get_arg(term, 0)) <= translateExpression(msat_term_get_arg(term, 1));
				} else if (msat_term_is_true(env, term)) {
                    return storm::expressions::Expression::createTrue();
				} else if (msat_term_is_false(env, term)) {
                    return storm::expressions::Expression::createFalse();
				} else if (msat_term_is_boolean_constant(env, term)) {
					char* name = msat_decl_get_name(msat_term_get_decl(term));
					std::string name_str(name);
                    storm::expressions::Expression result = expressions::Expression::createBooleanVariable(name_str.substr(0, name_str.find('/')));
					msat_free(name);
                    return result;
				} else if (msat_term_is_constant(env, term)) {
                    
					char* name = msat_decl_get_name(msat_term_get_decl(term));
					std::string name_str(name);
                    storm::expressions::Expression result;
					if (msat_is_integer_type(env, msat_term_get_type(term))) {
                        result = expressions::Expression::createIntegerVariable(name_str.substr(0, name_str.find('/')));
					} else if (msat_is_rational_type(env, msat_term_get_type(term))) {
                        result = expressions::Expression::createDoubleVariable(name_str.substr(0, name_str.find('/')));
					}
					msat_free(name);
                    return result;
				} else if (msat_term_is_number(env, term)) {
					if (msat_is_integer_type(env, msat_term_get_type(term))) {
						return expressions::Expression::createIntegerLiteral(std::stoll(msat_term_repr(term)));
					} else if (msat_is_rational_type(env, msat_term_get_type(term))) {
                        return expressions::Expression::createDoubleLiteral(std::stod(msat_term_repr(term)));
					}
				}
                
                // If all other cases did not apply, we cannot represent the term in our expression framework.
                char* termAsCString = msat_term_repr(term);
                std::string termString(termAsCString);
                msat_free(termAsCString);
                STORM_LOG_THROW(false, storm::exceptions::ExpressionEvaluationException, "Cannot translate expression: unknown term: '" << termString << "'.");
			}

		private:
            // The MathSAT environment used.
			msat_env& env;
                        
            // A stack used for communicating results between different functions.
			std::stack<msat_term> stack;
            
            // A mapping of variable names to their declaration in the MathSAT environment.
            std::map<std::string, msat_decl> variableToDeclarationMap;
                    
            // A flag indicating whether variables are supposed to be created if they are not already known to the adapter.
            bool createVariables;
		};
#endif
	} // namespace adapters
} // namespace storm

#endif /* STORM_ADAPTERS_MATHSATEXPRESSIONADAPTER_H_ */
