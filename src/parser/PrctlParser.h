#ifndef STORM_PARSER_PRCTLPARSER_H_
#define STORM_PARSER_PRCTLPARSER_H_

#include "src/formula/Prctl.h"
//#include <memory>

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
		 */
		PrctlParser(std::string formulaString);
		 
		/*!
		 *	@return a pointer to the parsed formula object
		 */
		storm::property::prctl::AbstractPrctlFormula<double>* getFormula() {
			return this->formula;
		}

		/*!
		 * Checks whether the line which was parsed was a comment line; also returns true if the line was empty (as the semantics are
		 * the same)
		 *
		 * @return True if the parsed line consisted completely of a (valid) comment, false otherwise.
		 */
		bool parsedComment() {
			return (formula == nullptr);
		}

	private:
		storm::property::prctl::AbstractPrctlFormula<double>* formula;

		/*!
		 * Struct for the Prctl grammar, that Boost::Spirit uses to parse the formulas.
		 */
		template<typename Iterator, typename Skipper>
		struct PrctlGrammar;

};

} // namespace parser
} // namespace storm

#endif /* STORM_PARSER_PRCTLPARSER_H_ */
