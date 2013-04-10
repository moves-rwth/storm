/* 
 * File:   BooleanExpressionGrammar.h
 * Author: nafur
 *
 * Created on April 10, 2013, 6:27 PM
 */

#ifndef BOOLEANEXPRESSIONGRAMMAR_H
#define	BOOLEANEXPRESSIONGRAMMAR_H

#include "Includes.h"
#include "VariableState.h"
#include "IntegerExpressionGrammar.h"

namespace storm {
namespace parser {
namespace prism {

class BooleanExpressionGrammar : public qi::grammar<Iterator, std::shared_ptr<BaseExpression>(), Skipper, Skipper> {
public:
	BooleanExpressionGrammar(std::shared_ptr<VariableState>& state);

private:
	// Rules for variable/command names.
	qi::rule<Iterator, std::string(), Skipper> integerVariableName;
	qi::rule<Iterator, std::string(), Skipper> booleanVariableName;
	qi::rule<Iterator, std::string(), Skipper> unassignedLocalBooleanVariableName;
	qi::rule<Iterator, std::string(), Skipper> unassignedLocalIntegerVariableName;

	qi::rule<Iterator, std::string(), Skipper> freeIdentifierName;
	qi::rule<Iterator, std::string(), Skipper> identifierName;

	// The starting point for arbitrary expressions.
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> expression;

	// Rules with boolean result type.
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper, Skipper> booleanExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> orExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> andExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> notExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> atomicBooleanExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> relativeExpression;

	// Rules for variable recognition.
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> booleanVariableExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> booleanVariableCreatorExpression;

	// Rules for constant recognition.
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> booleanConstantExpression;

	// Rules for literal recognition.
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> booleanLiteralExpression;

	// A structure mapping the textual representation of a binary relation to the representation
	// of the intermediate representation.
	storm::parser::prism::relationalOperatorStruct relations_;

	std::shared_ptr<storm::parser::prism::VariableState> state;
};


}
}
}

#endif	/* BOOLEANEXPRESSIONGRAMMAR_H */
