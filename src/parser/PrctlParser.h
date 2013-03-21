#ifndef STORM_PARSER_PRCTLPARSER_H_
#define STORM_PARSER_PRCTLPARSER_H_

#include "src/parser/Parser.h"

#include "src/formula/Formulas.h"
#include <memory>

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
class PrctlParser : Parser
{
	public:
		/*!
		 * Reads a PRCTL formula from its string representation and parses it into a formula tree, consisting of
		 * classes in the namespace storm::formula.
		 *
		 * If the string could not be parsed successfully, it will throw a wrongFormatException.
		 *
		 * @param formulaString The string representation of the formula
		 * @throw wrongFormatException If the input could not be parsed successfully
		 */
		PrctlParser(std::string formulaString);
		 
		/*!
		 *	@return a pointer to the parsed formula object
		 */
	storm::formula::AbstractFormula<double>* getFormula()
		{
			return this->formula;
		}

	private:
		storm::formula::AbstractFormula<double>* formula;

		/*!
		 * Struct for the Prctl grammar, that Boost::Spirit uses to parse the formulas.
		 */
		template<typename Iterator, typename Skipper>
		struct PrctlGrammar;

};

} // namespace parser
} // namespace storm

#endif /* STORM_PARSER_PRCTLPARSER_H_ */
