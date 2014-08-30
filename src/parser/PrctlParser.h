#ifndef STORM_PARSER_PRCTLPARSER_H_
#define STORM_PARSER_PRCTLPARSER_H_

#include "src/properties/Prctl.h"
#include "src/properties/prctl/PrctlFilter.h"

namespace storm {
namespace parser {

/*!
 * Reads a PRCTL formula from a string and return the formula tree.
 *
 * If you want to read the formula from a file, use the PrctlFileParser class instead.
 */
class PrctlParser {
public:

	/*!
	 * Reads a Prctl formula from its string representation and parses it into a formula tree, consisting of
	 * classes in the namespace storm::properties.
	 *
	 * If the string could not be parsed successfully, it will throw a wrongFormatException.
	 *
	 * @param formulaString The string representation of the formula
	 * @throw wrongFormatException If the input could not be parsed successfully
	 * @return A PrctlFilter maintaining the parsed formula. If the line just contained a comment a nullptr will be returned instead.
	 */
	static std::shared_ptr<storm::properties::prctl::PrctlFilter<double>> parsePrctlFormula(std::string formulaString);

private:

	/*!
	 * Struct for the Prctl grammar, that Boost::Spirit uses to parse the formulas.
	 */
	template<typename Iterator, typename Skipper>
	struct PrctlGrammar;

};

} // namespace parser
} // namespace storm

#endif /* STORM_PARSER_PRCTLPARSER_H_ */
