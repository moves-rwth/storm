#include "IntegerExpressionGrammar.h"

namespace storm {
namespace parser {
namespace prism {

IntegerExpressionGrammar::IntegerExpressionGrammar(std::shared_ptr<VariableState>& state)
	: IntegerExpressionGrammar::base_type(integerExpression), state(state) {

	// This rule defines all identifiers that have not been previously used.
	identifierName %= qi::as_string[qi::raw[qi::lexeme[((qi::alpha | qi::char_('_')) >> *(qi::alnum | qi::char_('_')))]]][ qi::_pass = phoenix::bind(&VariableState::isIdentifier, this->state.get(), qi::_1) ];
	identifierName.name("identifier");

	// This block defines all literal expressions.
	integerLiteralExpression = qi::int_[qi::_val = phoenix::construct<std::shared_ptr<BaseExpression>>(phoenix::new_<IntegerLiteral>(qi::_1))];
	integerLiteralExpression.name("integer literal");

	// This block defines all expressions that are variables.
	std::shared_ptr<BaseExpression> intvarexpr = std::shared_ptr<BaseExpression>(new VariableExpression(BaseExpression::int_, std::numeric_limits<uint_fast64_t>::max(), "int", std::shared_ptr<BaseExpression>(nullptr), std::shared_ptr<BaseExpression>(nullptr)));
	integerVariableExpression = identifierName[qi::_val = intvarexpr];
	integerVariableExpression.name("integer variable");

	// This block defines all atomic expressions that are constant, i.e. literals and constants.
	integerConstantExpression %= (this->state->integerConstants_ | integerLiteralExpression);
	integerConstantExpression.name("integer constant or literal");

	// This block defines all expressions of integral type.
	atomicIntegerExpression %= (integerVariableExpression | qi::lit("(") >> integerExpression >> qi::lit(")") | integerConstantExpression);
	atomicIntegerExpression.name("integer expression");
	integerMultExpression %= atomicIntegerExpression[qi::_val = qi::_1] >> *(qi::lit("*") >> atomicIntegerExpression)[qi::_val = phoenix::construct<std::shared_ptr<BaseExpression>>(phoenix::new_<BinaryNumericalFunctionExpression>(BaseExpression::int_, qi::_val, qi::_1, BinaryNumericalFunctionExpression::TIMES))];
	integerMultExpression.name("integer expression");
	integerPlusExpression = integerMultExpression[qi::_val = qi::_1] >> *((qi::lit("+")[qi::_a = true] | qi::lit("-")[qi::_a = false]) >> integerMultExpression)[phoenix::if_(qi::_a) [qi::_val = phoenix::construct<std::shared_ptr<BaseExpression>>(phoenix::new_<BinaryNumericalFunctionExpression>(BaseExpression::int_, qi::_val, qi::_1, BinaryNumericalFunctionExpression::PLUS)) ] .else_ [qi::_val = phoenix::construct<std::shared_ptr<BaseExpression>>(phoenix::new_<BinaryNumericalFunctionExpression>(BaseExpression::int_, qi::_val, qi::_1, BinaryNumericalFunctionExpression::MINUS))]];
	integerPlusExpression.name("integer expression");
	integerExpression %= integerPlusExpression;
	integerExpression.name("integer expression");
}

void IntegerExpressionGrammar::prepareForSecondRun() {
	// Override variable expressions: only allow declared variables.
	integerVariableExpression %= this->state->integerVariables_;
	integerVariableExpression.name("integer variable");
}

}
}
}