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
#include "IdentifierGrammars.h"

#include <memory>

namespace storm {
    namespace parser {
        namespace prism {
            
            /*!
             * This grammar parses a (non constant) integer expressions as used in prism models.
             */
            class IntegerExpressionGrammar : public qi::grammar<Iterator, std::shared_ptr<BaseExpression>(), Skipper, Unused>, public BaseGrammar<IntegerExpressionGrammar> {
            public:
                IntegerExpressionGrammar(std::shared_ptr<VariableState> const& state);
                
                /*!
                 * Switch to second run.
                 * Variable names may be any valid identifier in the first run, but only defined variables in the second run.
                 */
                virtual void prepareSecondRun();
                
            private:
                qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper, Unused> integerExpression;
                qi::rule<Iterator, std::shared_ptr<BaseExpression>(), qi::locals<bool>, Skipper> integerPlusExpression;
                qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> integerMultExpression;
                qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> atomicIntegerExpression;
                qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> integerVariableExpression;
            };
            
        }
    }
}

#endif	/* INTEGEREXPRESSIONGRAMMAR_H */

