/* 
 * File:   Tokens.h
 * Author: nafur
 *
 * Created on April 19, 2013, 11:17 PM
 */

#ifndef TOKENS_H
#define	TOKENS_H

namespace storm {
namespace parser {
namespace prism {


	// A structure mapping the textual representation of a model type to the model type
	// representation of the intermediate representation.
	struct modelTypeStruct : qi::symbols<char, Program::ModelType> {
		modelTypeStruct() {
			add
				("dtmc", Program::ModelType::DTMC)
				("ctmc", Program::ModelType::CTMC)
				("mdp", Program::ModelType::MDP)
				("ctmdp", Program::ModelType::CTMDP)
			;
		}
	};


	// A structure defining the keywords that are not allowed to be chosen as identifiers.
	struct keywordsStruct : qi::symbols<char, unsigned> {
		keywordsStruct() {
			add
				("dtmc", 1)
				("ctmc", 2)
				("mdp", 3)
				("ctmdp", 4)
				("const", 5)
				("int", 6)
				("bool", 7)
				("module", 8)
				("endmodule", 9)
				("rewards", 10)
				("endrewards", 11)
				("true", 12)
				("false", 13)
			;
		}
	};
	
	// A structure mapping the textual representation of a binary relation to the representation
	// of the intermediate representation.
	struct relationalOperatorStruct : qi::symbols<char, BinaryRelationExpression::RelationType> {
		relationalOperatorStruct() {
			add
				("=", BinaryRelationExpression::EQUAL)
				("!=", BinaryRelationExpression::NOT_EQUAL)
				("<", BinaryRelationExpression::LESS)
				("<=", BinaryRelationExpression::LESS_OR_EQUAL)
				(">", BinaryRelationExpression::GREATER)
				(">=", BinaryRelationExpression::GREATER_OR_EQUAL)
			;
		}
	};
}
}
}

#endif	/* TOKENS_H */

