/* 
 * File:   ConstIntegerExpressionGrammar.h
 * Author: nafur
 *
 * Created on April 10, 2013, 6:02 PM
 */

#ifndef CONSTINTEGEREXPRESSIONGRAMMAR_H
#define	CONSTINTEGEREXPRESSIONGRAMMAR_H

#include "Includes.h"
#include "VariableState.h"

namespace storm {
namespace parser {
namespace prism {

class ConstIntegerExpressionGrammar : public qi::grammar<Iterator, std::shared_ptr<BaseExpression>(), Skipper, Skipper> {
public:
	ConstIntegerExpressionGrammar(std::shared_ptr<VariableState>& state);
	
private:
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> integerLiteralExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> integerConstantExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper, Skipper> constantIntegerExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), qi::locals<bool>, Skipper> constantIntegerPlusExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> constantIntegerMultExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> constantAtomicIntegerExpression;

	std::shared_ptr<VariableState> state;
};


}
}
}

#endif	/* CONSTINTEGEREXPRESSIONGRAMMAR_H */
