#ifndef STORM_PARSER_PRCTLPARSER_H_
#define STORM_PARSER_PRCTLPARSER_H_

#include "src/formula/Prctl.h"
#include "src/formula/Prctl/PrctlFilter.h"

namespace storm {
namespace parser {

/*!
 * Reads a PRCTL formula from a string and return the formula tree.
 *
 * If you want to read the formula from a file, use the PrctlFileParser class instead.
 *
 * @note
 * This class creates a PctlFormula object which can be accessed through the getFormula() method (of base
 * class PrctlParser). However, it will not delete this object.
 */
class PrctlParser {
public:

	/*!
	 * Reads a PRCTL formula from its string representation and parses it into a formula tree, consisting of
	 * classes in the namespace storm::property.
	 *
	 * If the string could not be parsed successfully, it will throw a wrongFormatException.
	 *
	 * @param formulaString The string representation of the formula
	 * @throw wrongFormatException If the input could not be parsed successfully
	 * @return A pointer to the parsed Prctl formula. If the line just contained a comment a nullptr will be returned instead.
	 */
	static storm::property::prctl::PrctlFilter<double>* parsePrctlFormula(std::string formulaString);

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
