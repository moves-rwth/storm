/*
 * LtlParser.h
 *
 *  Created on: 22.04.2013
 *      Author: thomas
 */

#ifndef STORM_PARSER_LTLPARSER_H_
#define STORM_PARSER_LTLPARSER_H_

#include "src/properties/Ltl.h"
#include "src/properties/ltl/LtlFilter.h"

namespace storm {
namespace parser {

/*!
 * Reads a LTL formula from a string and return the formula tree.
 *
 * If you want to read the formula from a file, use the LtlFileParser class instead.
 */
class LtlParser {
public:

	/*!
	* Reads a LTL formula from its string representation and parses it into a formula tree, consisting of
	* classes in the namespace storm::properties.
	*
	* If the string could not be parsed successfully, it will throw a wrongFormatException.
	*
	* @param formulaString The string representation of the formula.
	* @throw wrongFormatException If the input could not be parsed successfully.
	* @return A LtlFilter maintaining the parsed formula.
	*/
	static std::shared_ptr<storm::properties::ltl::LtlFilter<double>> parseLtlFormula(std::string formulaString);

private:

	/*!
	 * Struct for the Ltl grammar, that Boost::Spirit uses to parse the formulas.
	 */
	template<typename Iterator, typename Skipper>
	struct LtlGrammar;

};

} /* namespace parser */
} /* namespace storm */
#endif /* STORM_PARSER_LTLPARSER_H_ */
