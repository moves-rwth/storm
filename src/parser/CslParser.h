/*
 * CslParser.h
 *
 *  Created on: 08.04.2013
 *      Author: Thomas Heinemann
 */

#ifndef STORM_PARSER_CSLPARSER_H_
#define STORM_PARSER_CSLPARSER_H_

#include "src/formula/Csl.h"
#include "src/formula/csl/CslFilter.h"
#include <functional>

namespace storm {
namespace parser {

class CslParser {
public:

	/*!
	 * Reads a CSL formula from its string representation and parses it into a formula tree, consisting of
	 * classes in the namespace storm::property.
	 *
	 * If the string could not be parsed successfully, it will throw a wrongFormatException.
	 *
	 * @param formulaString The string representation of the formula
	 * @throw wrongFormatException If the input could not be parsed successfully
	 */
	static storm::property::csl::CslFilter<double>* parseCslFormula(std::string formulaString);

private:

	/*!
	 * Struct for the CSL grammar, that Boost::Spirit uses to parse the formulas.
	 */
	template<typename Iterator, typename Skipper>
	struct CslGrammar;

};

} /* namespace parser */
} /* namespace storm */
#endif /* STORM_PARSER_CSLPARSER_H_ */
