/*
 * LtlParser.h
 *
 *  Created on: 22.04.2013
 *      Author: thomas
 */

#ifndef STORM_PARSER_LTLPARSER_H_
#define STORM_PARSER_LTLPARSER_H_

#include "src/formula/Ltl.h"
#include "src/formula/Ltl/LtlFilter.h"

namespace storm {
namespace parser {

/*!
	* Reads a LTL formula from its string representation and parses it into a formula tree, consisting of
	* classes in the namespace storm::property.
	*
	* If the string could not be parsed successfully, it will throw a wrongFormatException.
	*
	* @param formulaString The string representation of the formula
	* @throw wrongFormatException If the input could not be parsed successfully
	*/
storm::property::ltl::LtlFilter<double>* LtlParser(std::string formulaString);

/*!
 * Struct for the Ltl grammar, that Boost::Spirit uses to parse the formulas.
 */
template<typename Iterator, typename Skipper>
struct LtlGrammar;

} /* namespace parser */
} /* namespace storm */
#endif /* STORM_PARSER_LTLPARSER_H_ */
