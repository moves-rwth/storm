/* 
 * File:   ConstBooleanExpressionGrammar.h
 * Author: nafur
 *
 * Created on April 10, 2013, 6:34 PM
 */

#ifndef CONSTBOOLEANEXPRESSIONGRAMMAR_H
#define	CONSTBOOLEANEXPRESSIONGRAMMAR_H

#include "Includes.h"
#include "VariableState.h"
#include "IdentifierGrammars.h"
#include "Tokens.h"

namespace storm {
namespace parser {
namespace prism {

/*!
 * This grammar parses constant boolean expression as used in prism models.
 */
class ConstBooleanExpressionGrammar : public qi::grammar<Iterator, std::shared_ptr<BaseExpression>(), Skipper, Unused>, public BaseGrammar<ConstBooleanExpressionGrammar> {
public:
	ConstBooleanExpressionGrammar(std::shared_ptr<VariableState> const& state);


private:
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper, Unused> constantBooleanExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> constantOrExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> constantAndExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> constantNotExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> constantAtomicBooleanExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> constantRelativeExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> booleanConstantExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> booleanLiteralExpression;

	storm::parser::prism::relationalOperatorStruct relations_;
};


}
}
}

#endif	/* CONSTBOOLEANEXPRESSIONGRAMMAR_H */

