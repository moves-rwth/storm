#ifndef STORM_PARSER_PRISMPARSER_TOKENS_H_
#define	STORM_PARSER_PRISMPARSER_TOKENS_H_

#include "src/storage/expressions/Expression.h"

namespace storm {
    namespace parser {
        namespace prism {
            /*!
             * A structure mapping the textual representation of a model type to the model type
             * representation of the intermediate representation.
             */
            struct modelTypeStruct : qi::symbols<char, Program::ModelType> {
                modelTypeStruct() {
                    add
                    ("dtmc", Program::ModelType::DTMC)
                    ("ctmc", Program::ModelType::CTMC)
                    ("mdp", Program::ModelType::MDP)
                    ("ctmdp", Program::ModelType::CTMDP)
                    ("ma", Program::ModelType::MA);
                }
            };
            
            /*!
             * A structure defining the keywords that are not allowed to be chosen as identifiers.
             */
            struct keywordsStruct : qi::symbols<char, unsigned> {
                keywordsStruct() {
                    add
                    ("dtmc", 1)
                    ("ctmc", 2)
                    ("mdp", 3)
                    ("ctmdp", 4)
                    ("ma", 5)
                    ("const", 6)
                    ("int", 7)
                    ("bool", 8)
                    ("module", 9)
                    ("endmodule", 10)
                    ("rewards", 11)
                    ("endrewards", 12)
                    ("true", 13)
                    ("false", 14);
                }
            };
            
            /*!
             * A structure mapping the textual representation of a binary relation.
             */
            struct relationalOperatorStruct : qi::symbols<char, BinaryRelationExpression::RelationType> {
                relationalOperatorStruct() {
                    add
                    ("=", BinaryRelationExpression::RelationType::EQUAL)
                    ("!=", BinaryRelationExpression::RelationType::NOT_EQUAL)
                    ("<", BinaryRelationExpression::RelationType::LESS)
                    ("<=", BinaryRelationExpression::RelationType::LESS_OR_EQUAL)
                    (">", BinaryRelationExpression::RelationType::GREATER)
                    (">=", BinaryRelationExpression::RelationType::GREATER_OR_EQUAL);
                }
            };
        }
    }
}

#endif	/* STORM_PARSER_PRISMPARSER_TOKENS_H_ */

