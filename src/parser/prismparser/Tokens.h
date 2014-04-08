#ifndef STORM_PARSER_PRISMPARSER_TOKENS_H_
#define	STORM_PARSER_PRISMPARSER_TOKENS_H_

#include "src/storage/prism/Program.h"
#include "src/storage/expressions/Expressions.h"

namespace storm {
    namespace parser {
        namespace prism {
            /*!
             * A structure mapping the textual representation of a model type to the model type
             * representation of the intermediate representation.
             */
            struct modelTypeStruct : qi::symbols<char, storm::prism::Program::ModelType> {
                modelTypeStruct() {
                    add
                    ("dtmc", storm::prism::Program::ModelType::DTMC)
                    ("ctmc", storm::prism::Program::ModelType::CTMC)
                    ("mdp", storm::prism::Program::ModelType::MDP)
                    ("ctmdp", storm::prism::Program::ModelType::CTMDP)
                    ("ma", storm::prism::Program::ModelType::MA);
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
             * A structure mapping the textual representation of binary relations to the corresponding enum values.
             */
            struct BinaryRelationOperatorStruct : qi::symbols<char, storm::expressions::BinaryRelationExpression::RelationType> {
                BinaryRelationOperatorStruct() {
                    add
                    ("=", storm::expressions::BinaryRelationExpression::RelationType::Equal)
                    ("!=", storm::expressions::BinaryRelationExpression::RelationType::NotEqual)
                    ("<", storm::expressions::BinaryRelationExpression::RelationType::Less)
                    ("<=", storm::expressions::BinaryRelationExpression::RelationType::LessOrEqual)
                    (">", storm::expressions::BinaryRelationExpression::RelationType::Greater)
                    (">=", storm::expressions::BinaryRelationExpression::RelationType::GreaterOrEqual);
                }
            };
            
            /*!
             * A structure mapping the textual representation of binary operators to the corresponding enum values.
             */
            struct BinaryBooleanOperatorStruct : qi::symbols<char, storm::expressions::BinaryBooleanFunctionExpression::OperatorType> {
                BinaryBooleanOperatorStruct() {
                    add
                    ("&", storm::expressions::BinaryBooleanFunctionExpression::OperatorType::And)
                    ("|", storm::expressions::BinaryBooleanFunctionExpression::OperatorType::Or)
                    ("=>", storm::expressions::BinaryBooleanFunctionExpression::OperatorType::Implies)
                    ("<=>", storm::expressions::BinaryBooleanFunctionExpression::OperatorType::Iff);
                }
            };

            /*!
             * A structure mapping the textual representation of binary operators to the corresponding enum values.
             */
            struct UnaryBooleanOperatorStruct : qi::symbols<char, storm::expressions::UnaryBooleanFunctionExpression::OperatorType> {
                UnaryBooleanOperatorStruct() {
                    add
                    ("!", storm::expressions::UnaryBooleanFunctionExpression::OperatorType::Not);
                }
            };
            
            /*!
             * A structure mapping the textual representation of binary boolean operators to the corresponding enum values.
             */
            struct BinaryNumericalOperatorStruct : qi::symbols<char, storm::expressions::BinaryNumericalFunctionExpression::OperatorType> {
                BinaryNumericalOperatorStruct() {
                    add
                    ("+", storm::expressions::BinaryNumericalFunctionExpression::OperatorType::Plus)
                    ("-", storm::expressions::BinaryNumericalFunctionExpression::OperatorType::Minus)
                    ("*", storm::expressions::BinaryNumericalFunctionExpression::OperatorType::Times)
                    ("/", storm::expressions::BinaryNumericalFunctionExpression::OperatorType::Divide);
                }
            };
            
            /*!
             * A structure mapping the textual representation of binary operators to the corresponding enum values.
             */
            struct UnaryNumericalOperatorStruct : qi::symbols<char, storm::expressions::UnaryNumericalFunctionExpression::OperatorType> {
                UnaryNumericalOperatorStruct() {
                    add
                    ("!", storm::expressions::UnaryNumericalFunctionExpression::OperatorType::Minus);
                }
            };
        }
    }
}

#endif	/* STORM_PARSER_PRISMPARSER_TOKENS_H_ */

