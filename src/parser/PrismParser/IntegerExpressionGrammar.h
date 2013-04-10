/* 
 * File:   IntegerExpressionGrammar.h
 * Author: nafur
 *
 * Created on April 10, 2013, 4:39 PM
 */

#ifndef INTEGEREXPRESSIONGRAMMAR_H
#define	INTEGEREXPRESSIONGRAMMAR_H

#include "src/ir/IR.h"
#include "VariableState.h"
#include "Includes.h"

#include <memory>

namespace storm {
namespace parser {
namespace prism {

class IntegerExpressionGrammar : public qi::grammar<Iterator, std::shared_ptr<BaseExpression>(), Skipper, Skipper> {
public:
	IntegerExpressionGrammar(std::shared_ptr<VariableState>& state);
	void prepareForSecondRun();

private:

	qi::rule<Iterator, std::string(), Skipper> identifierName;

	// The starting point for arbitrary expressions.
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> expression;

	// Rules with integer result type.
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper, Skipper> integerExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), qi::locals<bool>, Skipper> integerPlusExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> integerMultExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> atomicIntegerExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> constantIntegerExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), qi::locals<bool>, Skipper> constantIntegerPlusExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> constantIntegerMultExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> constantAtomicIntegerExpression;

	// Rules for variable recognition.
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> integerVariableExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), qi::locals<std::shared_ptr<BaseExpression>>, Skipper> integerVariableCreatorExpression;

	// Rules for constant recognition.
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> integerConstantExpression;

	// Rules for literal recognition.
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> integerLiteralExpression;

	std::shared_ptr<VariableState> state;
};

}
}
}

#endif	/* INTEGEREXPRESSIONGRAMMAR_H */

