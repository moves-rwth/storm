/* 
 * File:   ConstBooleanExpressionGrammar.h
 * Author: nafur
 *
 * Created on April 10, 2013, 6:34 PM
 */

#ifndef CONSTBOOLEANEXPRESSIONGRAMMAR_H
#define	CONSTBOOLEANEXPRESSIONGRAMMAR_H

#include "Includes.h"
#include "ConstIntegerExpressionGrammar.h"
#include "VariableState.h"
#include "UtilityGrammars.h"

namespace storm {
namespace parser {
namespace prism {

class ConstBooleanExpressionGrammar : public qi::grammar<Iterator, std::shared_ptr<BaseExpression>(), Skipper, Skipper> {
public:
	ConstBooleanExpressionGrammar(std::shared_ptr<VariableState>& state);

private:
	
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> booleanLiteralExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> booleanConstantExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> relativeExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper, Skipper> constantBooleanExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> constantOrExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> constantAndExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> constantNotExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> constantAtomicBooleanExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> constantRelativeExpression;


	storm::parser::prism::relationalOperatorStruct relations_;

	std::shared_ptr<storm::parser::prism::VariableState> state;

};


}
}
}

#endif	/* CONSTBOOLEANEXPRESSIONGRAMMAR_H */

