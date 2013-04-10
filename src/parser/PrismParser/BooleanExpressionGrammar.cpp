#include "BooleanExpressionGrammar.h"

namespace storm {
namespace parser {
namespace prism {

BooleanExpressionGrammar::BooleanExpressionGrammar(std::shared_ptr<VariableState>& state)
			: BooleanExpressionGrammar::base_type(booleanExpression), state(state) {

		IntegerExpressionGrammar intExpr(this->state);

		// This rule defines all identifiers that have not been previously used.
		identifierName %= qi::as_string[qi::raw[qi::lexeme[((qi::alpha | qi::char_('_')) >> *(qi::alnum | qi::char_('_')))]]][ qi::_pass = phoenix::bind(&storm::parser::prism::VariableState::isIdentifier, this->state.get(), qi::_1) ];
		identifierName.name("identifier");
		freeIdentifierName %= qi::as_string[qi::raw[qi::lexeme[((qi::alpha | qi::char_('_')) >> *(qi::alnum | qi::char_('_')))]]][ qi::_pass = phoenix::bind(&storm::parser::prism::VariableState::isFreeIdentifier, this->state.get(), qi::_1) ];
		freeIdentifierName.name("unused identifier");

		// This block defines all literal expressions.
		booleanLiteralExpression = qi::bool_[qi::_val = phoenix::construct<std::shared_ptr<BaseExpression>>(phoenix::new_<BooleanLiteral>(qi::_1))];
		booleanLiteralExpression.name("boolean literal");


		// This block defines all expressions that are variables.
		std::shared_ptr<BaseExpression> boolvarexpr = std::shared_ptr<BaseExpression>(new VariableExpression(BaseExpression::bool_, std::numeric_limits<uint_fast64_t>::max(), "bool", std::shared_ptr<BaseExpression>(nullptr), std::shared_ptr<BaseExpression>(nullptr)));
		booleanVariableExpression = identifierName[qi::_val = boolvarexpr];
		booleanVariableExpression.name("boolean variable");

		// This block defines all atomic expressions that are constant, i.e. literals and constants.
		booleanConstantExpression %= (this->state->booleanConstants_ | booleanLiteralExpression);
		booleanConstantExpression.name("boolean constant or literal");

		// This block defines all expressions of type boolean.
		relativeExpression = (intExpr >> relations_ >> intExpr)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BinaryRelationExpression>(qi::_1, qi::_3, qi::_2))];
		relativeExpression.name("relative expression");
		atomicBooleanExpression %= (relativeExpression | booleanVariableExpression | qi::lit("(") >> booleanExpression >> qi::lit(")") | booleanConstantExpression);
		atomicBooleanExpression.name("boolean expression");
		notExpression = atomicBooleanExpression[qi::_val = qi::_1] | (qi::lit("!") >> atomicBooleanExpression)[qi::_val = phoenix::construct<std::shared_ptr<UnaryBooleanFunctionExpression>>(phoenix::new_<UnaryBooleanFunctionExpression>(qi::_1, UnaryBooleanFunctionExpression::NOT))];
		notExpression.name("boolean expression");
		andExpression = notExpression[qi::_val = qi::_1] >> *(qi::lit("&") >> notExpression)[qi::_val = phoenix::construct<std::shared_ptr<BaseExpression>>(phoenix::new_<BinaryBooleanFunctionExpression>(qi::_val, qi::_1, BinaryBooleanFunctionExpression::AND))];
		andExpression.name("boolean expression");
		orExpression = andExpression[qi::_val = qi::_1] >> *(qi::lit("|") >> andExpression)[qi::_val = phoenix::construct<std::shared_ptr<BaseExpression>>(phoenix::new_<BinaryBooleanFunctionExpression>(qi::_val, qi::_1, BinaryBooleanFunctionExpression::OR))];
		orExpression.name("boolean expression");
		booleanExpression %= orExpression;
		booleanExpression.name("boolean expression");

		// This block defines auxiliary entities that are used to check whether a certain variable exist.
		booleanVariableName %= this->state->booleanVariableNames_;
		booleanVariableName.name("boolean variable");
		unassignedLocalBooleanVariableName %= this->state->localBooleanVariables_ - this->state->assignedLocalBooleanVariables_;
		unassignedLocalBooleanVariableName.name("unassigned local boolean variable");
	}

}
}
}