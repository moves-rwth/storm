/* 
 * File:   Keywords.h
 * Author: nafur
 *
 * Created on April 10, 2013, 6:03 PM
 */

#ifndef KEYWORDS_H
#define	KEYWORDS_H

#include "Includes.h"

namespace storm {
namespace parser {
namespace prism {

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

	// A structure mapping the textual representation of a binary relation to the representation
	// of the intermediate representation.
	struct relationalOperatorStruct : qi::symbols<char, BinaryRelationExpression::RelationType> {
		relationalOperatorStruct() {
			add
				("=", BinaryRelationExpression::EQUAL)
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
#endif	/* KEYWORDS_H */

