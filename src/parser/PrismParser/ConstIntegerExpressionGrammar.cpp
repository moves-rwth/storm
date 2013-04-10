#include "ConstIntegerExpressionGrammar.h"


namespace storm {
namespace parser {
namespace prism {

ConstIntegerExpressionGrammar::ConstIntegerExpressionGrammar(std::shared_ptr<VariableState>& state)
	: ConstIntegerExpressionGrammar::base_type(constantIntegerExpression), state(state) {

	integerLiteralExpression = qi::int_[qi::_val = phoenix::construct<std::shared_ptr<BaseExpression>>(phoenix::new_<IntegerLiteral>(qi::_1))];
	integerLiteralExpression.name("integer literal");
	integerConstantExpression %= (this->state->integerConstants_ | integerLiteralExpression);
	integerConstantExpression.name("integer constant or literal");

	constantAtomicIntegerExpression %= (qi::lit("(") >> constantIntegerExpression >> qi::lit(")") | integerConstantExpression);
	constantAtomicIntegerExpression.name("constant integer expression");
	constantIntegerMultExpression %= constantAtomicIntegerExpression[qi::_val = qi::_1] >> *(qi::lit("*") >> constantAtomicIntegerExpression)[qi::_val = phoenix::construct<std::shared_ptr<BaseExpression>>(phoenix::new_<BinaryNumericalFunctionExpression>(BaseExpression::int_, qi::_val, qi::_1, BinaryNumericalFunctionExpression::TIMES))];
	constantIntegerMultExpression.name("constant integer expression");
	constantIntegerPlusExpression = constantIntegerMultExpression[qi::_val = qi::_1] >> *((qi::lit("+")[qi::_a = true] | qi::lit("-")[qi::_a = false]) >> constantIntegerMultExpression)[phoenix::if_(qi::_a) [qi::_val = phoenix::construct<std::shared_ptr<BaseExpression>>(phoenix::new_<BinaryNumericalFunctionExpression>(BaseExpression::int_, qi::_val, qi::_1, BinaryNumericalFunctionExpression::PLUS)) ] .else_ [qi::_val = phoenix::construct<std::shared_ptr<BaseExpression>>(phoenix::new_<BinaryNumericalFunctionExpression>(BaseExpression::int_, qi::_val, qi::_1, BinaryNumericalFunctionExpression::MINUS))]];
	constantIntegerPlusExpression.name("constant integer expression");
	constantIntegerExpression %= constantIntegerPlusExpression;
	constantIntegerExpression.name("constant integer expression");


}

}
}
}