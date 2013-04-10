#include "ConstBooleanExpressionGrammar.h"

namespace storm {
namespace parser {
namespace prism {


	ConstBooleanExpressionGrammar::ConstBooleanExpressionGrammar(std::shared_ptr<VariableState>& state)
		: ConstBooleanExpressionGrammar::base_type(constantBooleanExpression), state(state) {

		ConstIntegerExpressionGrammar constIntExpr(this->state);

		booleanLiteralExpression = qi::bool_[qi::_val = phoenix::construct<std::shared_ptr<BaseExpression>>(phoenix::new_<BooleanLiteral>(qi::_1))];
		booleanLiteralExpression.name("boolean literal");
		booleanConstantExpression %= (this->state->booleanConstants_ | booleanLiteralExpression);
		booleanConstantExpression.name("boolean constant or literal");
		constantRelativeExpression = (constIntExpr >> relations_ >> constIntExpr)[qi::_val = phoenix::construct<std::shared_ptr<BaseExpression>>(phoenix::new_<BinaryRelationExpression>(qi::_1, qi::_3, qi::_2))];
		constantRelativeExpression.name("constant boolean expression");
		constantAtomicBooleanExpression %= (constantRelativeExpression | qi::lit("(") >> constantBooleanExpression >> qi::lit(")") | booleanLiteralExpression | booleanConstantExpression);
		constantAtomicBooleanExpression.name("constant boolean expression");
		constantNotExpression = constantAtomicBooleanExpression[qi::_val = qi::_1] | (qi::lit("!") >> constantAtomicBooleanExpression)[qi::_val = phoenix::construct<std::shared_ptr<UnaryBooleanFunctionExpression>>(phoenix::new_<UnaryBooleanFunctionExpression>(qi::_1, UnaryBooleanFunctionExpression::NOT))];
		constantNotExpression.name("constant boolean expression");
		constantAndExpression = constantNotExpression[qi::_val = qi::_1] >> *(qi::lit("&") >> constantNotExpression)[qi::_val = phoenix::construct<std::shared_ptr<BaseExpression>>(phoenix::new_<BinaryBooleanFunctionExpression>(qi::_val, qi::_1, BinaryBooleanFunctionExpression::AND))];
		constantAndExpression.name("constant boolean expression");
		constantOrExpression = constantAndExpression[qi::_val = qi::_1] >> *(qi::lit("|") >> constantAndExpression)[qi::_val = phoenix::construct<std::shared_ptr<BaseExpression>>(phoenix::new_<BinaryBooleanFunctionExpression>(qi::_val, qi::_1, BinaryBooleanFunctionExpression::OR))];
		constantOrExpression.name("constant boolean expression");
		constantBooleanExpression %= constantOrExpression;
		constantBooleanExpression.name("constant boolean expression");
	}
}
}
}