#include "ConstIntegerExpressionGrammar.h"

namespace storm {
namespace parser {
namespace prism {


ConstIntegerExpressionGrammar::ConstIntegerExpressionGrammar(std::shared_ptr<VariableState>& state)
	: ConstIntegerExpressionGrammar::base_type(constantIntegerExpression), BaseGrammar(state) {

	constantIntegerExpression %= constantIntegerPlusExpression;
	constantIntegerExpression.name("constant integer expression");

	constantIntegerPlusExpression = constantIntegerMultExpression[qi::_val = qi::_1] >> *((qi::lit("+")[qi::_a = true] | qi::lit("-")[qi::_a = false]) >> constantIntegerMultExpression)
			[qi::_val = phoenix::bind(&BaseGrammar::createIntPlus, this, qi::_val, qi::_a, qi::_1)];
	constantIntegerPlusExpression.name("constant integer expression");

	constantIntegerMultExpression %= constantAtomicIntegerExpression[qi::_val = qi::_1] >> *(qi::lit("*") >> constantAtomicIntegerExpression)
			[qi::_val = phoenix::bind(&BaseGrammar::createIntMult, this, qi::_val, qi::_1)];
	constantIntegerMultExpression.name("constant integer expression");

	constantAtomicIntegerExpression %= (qi::lit("(") >> constantIntegerExpression >> qi::lit(")") | integerConstantExpression);
	constantAtomicIntegerExpression.name("constant integer expression");

	integerConstantExpression %= (this->state->integerConstants_ | integerLiteralExpression);
	integerConstantExpression.name("integer constant or literal");
	
	integerLiteralExpression = qi::int_[qi::_val = phoenix::bind(&BaseGrammar::createIntLiteral, this, qi::_1)];
	integerLiteralExpression.name("integer literal");

}

}
}
}