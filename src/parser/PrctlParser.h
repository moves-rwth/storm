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
		 * Reads a PRCTL formula from its string representation and parses it into the formula tree
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
	
	protected:
		/*!
		 * Empty constructor.
		 *
		 * Some subclasses do not get a formula string as input (E.g. PrctlFileFormat), hence they should not
		 * call the usual constructor of this class.
		 *
		 * However, this constructor should never be called directly (only as constructor of the super class),
		 * as it will not parse anything (and formula will point to nullptr then), so it is protected.
		 */
		PrctlParser() {
			formula = nullptr;
		}

		/*!
		 * Parses a formula and stores the result in the field "formula"
		 * @param formula The string representation of the formula to parse
		 */
		void parse(std::string formula);

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
