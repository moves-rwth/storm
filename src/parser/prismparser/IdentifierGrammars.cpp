#include "IdentifierGrammars.h"

namespace storm {
    namespace parser {
        namespace prism {
            
            IdentifierGrammar::IdentifierGrammar(std::shared_ptr<VariableState> const& state)
            : IdentifierGrammar::base_type(identifierName), BaseGrammar(state) {
                
                identifierName %= qi::as_string[qi::raw[qi::lexeme[((qi::alpha | qi::char_('_')) >> *(qi::alnum | qi::char_('_')))]]][ qi::_pass = phoenix::bind(&VariableState::isIdentifier, this->state.get(), qi::_1) ];
                identifierName.name("identifier");
            }
            
            FreeIdentifierGrammar::FreeIdentifierGrammar(std::shared_ptr<VariableState> const& state)
            : FreeIdentifierGrammar::base_type(freeIdentifierName), BaseGrammar(state) {
                
                freeIdentifierName %= qi::as_string[qi::raw[qi::lexeme[((qi::alpha | qi::char_('_')) >> *(qi::alnum | qi::char_('_')))]]][ qi::_pass = phoenix::bind(&VariableState::isFreeIdentifier, this->state.get(), qi::_1) ];
                freeIdentifierName.name("identifier");
            }
            
        }
    }
}