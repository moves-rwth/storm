/*
 * LtlParser.h
 *
 *  Created on: 22.04.2013
 *      Author: thomas
 */

#ifndef STORM_PARSER_LTLPARSER_H_
#define STORM_PARSER_LTLPARSER_H_

#include "Parser.h"
#include "src/formula/Ltl.h"

namespace storm {
namespace parser {

class LtlParser: public storm::parser::Parser {
public:
public:
	/*!
	 * Reads a LTL formula from its string representation and parses it into a formula tree, consisting of
	 * classes in the namespace storm::formula.
	 *
	 * If the string could not be parsed successfully, it will throw a wrongFormatException.
	 *
	 * @param formulaString The string representation of the formula
	 * @throw wrongFormatException If the input could not be parsed successfully
	 */
	LtlParser(std::string formulaString);

	/*!
	 *	@return a pointer to the parsed formula object
	 */
	storm::formula::ltl::AbstractLtlFormula<double>* getFormula() {
		return this->formula;
	}

	/*!
	 * Destructor
	 *
	 * Does not delete the parsed formula!
	 */
	virtual ~LtlParser() {
		// Intentionally left empty
		// The formula is not deleted with the parser.
	}

private:
	storm::formula::ltl::AbstractLtlFormula<double>* formula;

	/*!
	 * Struct for the Ltl grammar, that Boost::Spirit uses to parse the formulas.
	 */
	template<typename Iterator, typename Skipper>
	struct LtlGrammar;
};

} /* namespace parser */
} /* namespace storm */
#endif /* STORM_PARSER_LTLPARSER_H_ */
