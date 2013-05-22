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

	std::shared_ptr<BaseExpression> createLiteral(double value);
	std::shared_ptr<BaseExpression> createPlus(const std::shared_ptr<BaseExpression> left, bool addition, const std::shared_ptr<BaseExpression> right);
	std::shared_ptr<BaseExpression> createMult(const std::shared_ptr<BaseExpression> left, bool multiplication, const std::shared_ptr<BaseExpression> right);
};


}
}
}

#endif	/* CONSTDOUBLEEXPRESSIONGRAMMAR_H */

