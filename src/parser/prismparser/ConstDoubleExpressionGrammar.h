/* 
 * File:   ConstDoubleExpressionGrammar.h
 * Author: nafur
 *
 * Created on April 10, 2013, 7:04 PM
 */

#ifndef CONSTDOUBLEEXPRESSIONGRAMMAR_H
#define	CONSTDOUBLEEXPRESSIONGRAMMAR_H

#include "Includes.h"
#include "VariableState.h"
#include "IdentifierGrammars.h"

namespace storm {
namespace parser {
namespace prism {

/*!
 * This grammar parses constant double expressions as used in prism models.
 */
class ConstDoubleExpressionGrammar : public qi::grammar<Iterator, std::shared_ptr<BaseExpression>(), Skipper, Unused>, public BaseGrammar<ConstDoubleExpressionGrammar> {
public:
	ConstDoubleExpressionGrammar(std::shared_ptr<VariableState>& state);

private:
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper, Unused> constantDoubleExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), qi::locals<bool>, Skipper> constantDoublePlusExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), qi::locals<bool>, Skipper> constantDoubleMultExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> constantAtomicDoubleExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> doubleConstantExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> doubleLiteralExpression;
};


}
}
}

#endif	/* CONSTDOUBLEEXPRESSIONGRAMMAR_H */

