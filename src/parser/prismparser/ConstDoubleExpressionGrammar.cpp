#include "ConstDoubleExpressionGrammar.h"

namespace storm {
namespace parser {
namespace prism {

ConstDoubleExpressionGrammar::ConstDoubleExpressionGrammar(std::shared_ptr<VariableState>& state)
	: ConstDoubleExpressionGrammar::base_type(constantDoubleExpression), BaseGrammar(state) {

	constantDoubleExpression %= constantDoublePlusExpression;
	constantDoubleExpression.name("constant double expression");

	constantDoublePlusExpression %= constantDoubleMultExpression[qi::_val = qi::_1] >> *((qi::lit("+")[qi::_a = true] | qi::lit("-")[qi::_a = false]) >> constantDoubleMultExpression)
			[qi::_val = phoenix::bind(&BaseGrammar::createDoublePlus, this, qi::_val, qi::_a, qi::_1)];
	constantDoublePlusExpression.name("constant double expression");

	constantDoubleMultExpression %= constantAtomicDoubleExpression[qi::_val = qi::_1] >> *((qi::lit("*")[qi::_a = true] | qi::lit("/")[qi::_a = false]) >> constantAtomicDoubleExpression)
			[qi::_val = phoenix::bind(&BaseGrammar::createDoubleMult, this, qi::_val, qi::_a, qi::_1)];
	constantDoubleMultExpression.name("constant double expression");

	constantAtomicDoubleExpression %= (qi::lit("(") >> constantDoubleExpression >> qi::lit(")") | doubleConstantExpression);
	constantAtomicDoubleExpression.name("constant double expression");

	doubleConstantExpression %= (this->state->doubleConstants_ | this->state->integerConstants_ | doubleLiteralExpression);
	doubleConstantExpression.name("double constant or literal");

	doubleLiteralExpression = qi::double_[qi::_val = phoenix::bind(&BaseGrammar::createDoubleLiteral, this, qi::_1)];
	doubleLiteralExpression.name("double literal");
}


}
}
}