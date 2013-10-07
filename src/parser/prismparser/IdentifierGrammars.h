/* 
 * File:   Keywords.h
 * Author: nafur
 *
 * Created on April 10, 2013, 6:03 PM
 */

#ifndef IDENTIFIERGRAMMARS_H
#define	IDENTIFIERGRAMMARS_H

#include "Includes.h"
#include "BaseGrammar.h"
#include "VariableState.h"

namespace storm {
    namespace parser {
        namespace prism {
            
            /*!
             * This grammar parses a (possibly used) identifier as used in a prism models.
             */
            class IdentifierGrammar : public qi::grammar<Iterator, std::string(), Skipper, Unused>, public BaseGrammar<IdentifierGrammar> {
            public:
                IdentifierGrammar(std::shared_ptr<VariableState> const& state);
            private:
                qi::rule<Iterator, std::string(), Skipper> identifierName;
            };
            
            /*!
             * This grammar parses an used identifier as used in a prism models.
             */
            class FreeIdentifierGrammar : public qi::grammar<Iterator, std::string(), Skipper, Unused>, public BaseGrammar<IdentifierGrammar>  {
            public:
                FreeIdentifierGrammar(std::shared_ptr<VariableState> const& state);
            private:
                qi::rule<Iterator, std::string(), Skipper> freeIdentifierName;
            };
        }
    }
}
#endif	/* IDENTIFIERGRAMMARS_H */

