#include "ConstDoubleExpressionGrammar.h"

namespace storm {
namespace parser {
namespace prism {


	std::shared_ptr<BaseExpression> ConstDoubleExpressionGrammar::createLiteral(double value) {
		return std::shared_ptr<DoubleLiteral>(new DoubleLiteral(value));
	}
	std::shared_ptr<BaseExpression> ConstDoubleExpressionGrammar::createPlus(const std::shared_ptr<BaseExpression> left, bool addition, const std::shared_ptr<BaseExpression> right) {
		if (addition) {
			return std::shared_ptr<BinaryNumericalFunctionExpression>(new BinaryNumericalFunctionExpression(BaseExpression::double_, left, right, BinaryNumericalFunctionExpression::PLUS));
		} else {
			return std::shared_ptr<BinaryNumericalFunctionExpression>(new BinaryNumericalFunctionExpression(BaseExpression::double_, left, right, BinaryNumericalFunctionExpression::MINUS));
		}
	}
	std::shared_ptr<BaseExpression> ConstDoubleExpressionGrammar::createMult(const std::shared_ptr<BaseExpression> left, bool multiplication, const std::shared_ptr<BaseExpression> right) {
		if (multiplication) {
			return std::shared_ptr<BinaryNumericalFunctionExpression>(new BinaryNumericalFunctionExpression(BaseExpression::double_, left, right, BinaryNumericalFunctionExpression::TIMES));
		} else {
			return std::shared_ptr<BinaryNumericalFunctionExpression>(new BinaryNumericalFunctionExpression(BaseExpression::double_, left, right, BinaryNumericalFunctionExpression::DIVIDE));
		}
	}

ConstDoubleExpressionGrammar::ConstDoubleExpressionGrammar(std::shared_ptr<VariableState>& state)
	: ConstDoubleExpressionGrammar::base_type(constantDoubleExpression), BaseGrammar(state) {

	constantDoubleExpression %= constantDoublePlusExpression;
	constantDoubleExpression.name("constant double expression");

	constantDoublePlusExpression %= constantDoubleMultExpression[qi::_val = qi::_1] >> *((qi::lit("+")[qi::_a = true] | qi::lit("-")[qi::_a = false]) >> constantDoubleMultExpression)
			[qi::_val = phoenix::bind(&ConstDoubleExpressionGrammar::createPlus, this, qi::_val, qi::_a, qi::_1)];
	constantDoublePlusExpression.name("constant double expression");

	constantDoubleMultExpression %= constantAtomicDoubleExpression[qi::_val = qi::_1] >> *((qi::lit("*")[qi::_a = true] | qi::lit("/")[qi::_a = false]) >> constantAtomicDoubleExpression)
			[qi::_val = phoenix::bind(&ConstDoubleExpressionGrammar::createMult, this, qi::_val, qi::_a, qi::_1)];
	constantDoubleMultExpression.name("constant double expression");

	constantAtomicDoubleExpression %= (qi::lit("(") >> constantDoubleExpression >> qi::lit(")") | doubleConstantExpression);
	constantAtomicDoubleExpression.name("constant double expression");

	doubleConstantExpression %= (this->state->doubleConstants_ | doubleLiteralExpression);
	doubleConstantExpression.name("double constant or literal");

	doubleLiteralExpression = qi::double_[qi::_val = phoenix::bind(&ConstDoubleExpressionGrammar::createLiteral, this, qi::_1)];
	doubleLiteralExpression.name("double literal");
}


}
}
}