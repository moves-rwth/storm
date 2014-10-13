/*
* MathSatExpressionAdapter.h
*
*      Author: David Korzeniewski
*/

#ifndef STORM_ADAPTERS_MATHSATEXPRESSIONADAPTER_H_
#define STORM_ADAPTERS_MATHSATEXPRESSIONADAPTER_H_

#include "storm-config.h"

#include <stack>

#include "mathsat.h"

#include "storage/expressions/Expressions.h"
#include "storage/expressions/ExpressionVisitor.h"
#include "src/exceptions/ExceptionMacros.h"
#include "src/exceptions/ExpressionEvaluationException.h"
#include "src/exceptions/InvalidTypeException.h"
#include "src/exceptions/NotImplementedException.h"

namespace storm {
	namespace adapters {

		class MathSatExpressionAdapter : public storm::expressions::ExpressionVisitor {
		public:
			/*!
			* Creates a MathSatExpressionAdapter over the given MathSAT enviroment.
			*
			* @param context A reference to the MathSAT enviroment over which to build the expressions. Be careful to guarantee
			* the lifetime of the context as long as the instance of this adapter is used.
			* @param variableToDeclMap A mapping from variable names to their corresponding MathSAT Declarations.
			*/
			MathSatExpressionAdapter(msat_env& env, std::map<std::string, msat_decl> const& variableToDeclMap) : env(env), stack(), variableToDeclMap(variableToDeclMap) {
				// Intentionally left empty.
			}

			/*!
			* Translates the given expression to an equivalent term for MathSAT.
			*
			* @param expression The expression to translate.
			* @param createMathSatVariables If set to true a solver variable is created for each variable in expression that is not
			*                          yet known to the adapter. (i.e. values from the variableToExpressionMap passed to the constructor
			*                          are not overwritten)
			* @return An equivalent term for MathSAT.
			*/
			msat_term translateExpression(storm::expressions::Expression const& expression, bool createMathSatVariables = false) {
				//LOG4CPLUS_TRACE(logger, "Translating expression:\n" << expression->toString());
				expression.accept(this);
				msat_term result = stack.top();
				stack.pop();
				if (MSAT_ERROR_TERM(result)) {
					//LOG4CPLUS_WARN(logger, "Translating term to MathSAT returned an error!");
				}

				char* repr = msat_term_repr(result);
				//LOG4CPLUS_TRACE(logger, "Result is:\n" << repr);
				msat_free(repr);
				return result;
			}

			virtual void visit(expressions::BinaryBooleanFunctionExpression const* expression) override {
				expression->getFirstOperand()->accept(this);
				expression->getSecondOperand()->accept(this);

				msat_term rightResult = stack.top();
				stack.pop();
				msat_term leftResult = stack.top();
				stack.pop();

				//char* repr = msat_term_repr(leftResult);
				//LOG4CPLUS_TRACE(logger, "LHS: "<<repr);
				//msat_free(repr);
				//repr = msat_term_repr(rightResult);
				//LOG4CPLUS_TRACE(logger, "RHS: "<<repr);
				//msat_free(repr);

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
					default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
						<< "Unknown boolean binary operator: '" << expression->getOperatorType() << "' in expression " << expression << ".";
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
					default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
						<< "Unknown numerical binary operator: '" << expression->getOperatorType() << "' in expression " << expression << ".";
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
					default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
						<< "Unknown boolean binary operator: '" << expression->getRelationType() << "' in expression " << expression << ".";
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
					default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
						<< "Unknown boolean binary operator: '" << expression->getOperatorType() << "' in expression " << expression << ".";
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
					default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
						<< "Unknown numerical unary operator: '" << expression->getOperatorType() << "'.";
				}
			}

			virtual void visit(expressions::VariableExpression const* expression) override {
				if (variableToDeclMap.count(expression->getVariableName()) == 0) {
					LOG4CPLUS_ERROR(logger, "Variable " << expression->getVariableName() << " is unknown!");
				}
				//LOG4CPLUS_TRACE(logger, "Variable "<<expression->getVariableName());
				//char* repr = msat_decl_repr(variableToDeclMap.at(expression->getVariableName()));
				//LOG4CPLUS_TRACE(logger, "Decl: "<<repr);
				//msat_free(repr);
				if (MSAT_ERROR_DECL(variableToDeclMap.at(expression->getVariableName()))) {
					LOG4CPLUS_WARN(logger, "Encountered an invalid MathSAT declaration");
				}
				stack.push(msat_make_constant(env, variableToDeclMap.at(expression->getVariableName())));
			}

			storm::expressions::Expression translateTerm(msat_term term) {
				this->processTerm(term);

				storm::expressions::Expression result = std::move(expression_stack.top());
				expression_stack.pop();
				return result;
			}

			void processTerm(msat_term term) {
				if (msat_term_is_and(env, term)) {
					this->processTerm(msat_term_get_arg(term, 0));
					this->processTerm(msat_term_get_arg(term, 1));

					storm::expressions::Expression rightResult = std::move(expression_stack.top());
					expression_stack.pop();
					storm::expressions::Expression leftResult = std::move(expression_stack.top());
					expression_stack.pop();

					expression_stack.push(leftResult &&rightResult);
				} else if (msat_term_is_or(env, term)) {
					this->processTerm(msat_term_get_arg(term, 0));
					this->processTerm(msat_term_get_arg(term, 1));

					storm::expressions::Expression rightResult = std::move(expression_stack.top());
					expression_stack.pop();
					storm::expressions::Expression leftResult = std::move(expression_stack.top());
					expression_stack.pop();

					expression_stack.push(leftResult && rightResult);
				} else if (msat_term_is_iff(env, term)) {
					this->processTerm(msat_term_get_arg(term, 0));
					this->processTerm(msat_term_get_arg(term, 1));

					storm::expressions::Expression rightResult = std::move(expression_stack.top());
					expression_stack.pop();
					storm::expressions::Expression leftResult = std::move(expression_stack.top());
					expression_stack.pop();

					expression_stack.push(leftResult.iff(rightResult));
				} else if (msat_term_is_not(env, term)) {
					this->processTerm(msat_term_get_arg(term, 0));

					storm::expressions::Expression childResult = std::move(expression_stack.top());
					expression_stack.pop();

					expression_stack.push(!childResult);
				} else if (msat_term_is_plus(env, term)) {
					this->processTerm(msat_term_get_arg(term, 0));
					this->processTerm(msat_term_get_arg(term, 1));

					storm::expressions::Expression rightResult = std::move(expression_stack.top());
					expression_stack.pop();
					storm::expressions::Expression leftResult = std::move(expression_stack.top());
					expression_stack.pop();

					expression_stack.push(leftResult+rightResult);
				} else if (msat_term_is_times(env, term)) {
					this->processTerm(msat_term_get_arg(term, 0));
					this->processTerm(msat_term_get_arg(term, 1));

					storm::expressions::Expression rightResult = std::move(expression_stack.top());
					expression_stack.pop();
					storm::expressions::Expression leftResult = std::move(expression_stack.top());
					expression_stack.pop();

					expression_stack.push(leftResult * rightResult);
				} else if (msat_term_is_equal(env, term)) {
					this->processTerm(msat_term_get_arg(term, 0));
					this->processTerm(msat_term_get_arg(term, 1));

					storm::expressions::Expression rightResult = std::move(expression_stack.top());
					expression_stack.pop();
					storm::expressions::Expression leftResult = std::move(expression_stack.top());
					expression_stack.pop();

					expression_stack.push(leftResult == rightResult);
				} else if (msat_term_is_leq(env, term)) {
					this->processTerm(msat_term_get_arg(term, 0));
					this->processTerm(msat_term_get_arg(term, 1));

					storm::expressions::Expression rightResult = std::move(expression_stack.top());
					expression_stack.pop();
					storm::expressions::Expression leftResult = std::move(expression_stack.top());
					expression_stack.pop();

					expression_stack.push(leftResult <= rightResult);
				} else if (msat_term_is_true(env, term)) {
					expression_stack.push(expressions::Expression::createTrue());
				} else if (msat_term_is_false(env, term)) {
					expression_stack.push(expressions::Expression::createFalse());
				} else if (msat_term_is_boolean_constant(env, term)) {
					char* name = msat_decl_get_name(msat_term_get_decl(term));
					std::string name_str(name);
					expression_stack.push(expressions::Expression::createBooleanVariable(name_str.substr(0, name_str.find('/'))));
					msat_free(name);
				} else if (msat_term_is_constant(env, term)) {
					char* name = msat_decl_get_name(msat_term_get_decl(term));
					std::string name_str(name);
					if (msat_is_integer_type(env, msat_term_get_type(term))) {
						expression_stack.push(expressions::Expression::createIntegerVariable(name_str.substr(0, name_str.find('/'))));
					} else if (msat_is_rational_type(env, msat_term_get_type(term))) {
						expression_stack.push(expressions::Expression::createDoubleVariable(name_str.substr(0, name_str.find('/'))));
					}
					msat_free(name);
				} else if (msat_term_is_number(env, term)) {
					if (msat_is_integer_type(env, msat_term_get_type(term))) {
						expression_stack.push(expressions::Expression::createIntegerLiteral(std::stoll(msat_term_repr(term))));
					} else if (msat_is_rational_type(env, msat_term_get_type(term))) {
						expression_stack.push(expressions::Expression::createDoubleLiteral(std::stod(msat_term_repr(term))));
					}
				} else {
					char* term_cstr = msat_term_repr(term);
					std::string term_str(term_cstr);
					msat_free(term_cstr);
					throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
						<< "Unknown term: '" << term_str << "'.";
				}
			}

		private:
			msat_env& env;
			std::stack<msat_term> stack;
			std::stack<expressions::Expression> expression_stack;
			std::map<std::string, msat_decl> variableToDeclMap;
		};

	} // namespace adapters
} // namespace storm

#endif /* STORM_ADAPTERS_MATHSATEXPRESSIONADAPTER_H_ */
