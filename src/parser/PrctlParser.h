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
	
	protected:
		/*!
		 * Empty constructor.
		 *
		 * Some subclasses do not get a formula string as input (E.g. PrctlFileParser), hence they should not
		 * call the usual constructor of this class.
		 *
		 * However, this constructor should never be called directly (only during construction of an object of
		 * a subclass), as it will not parse anything (and formula will point to nullptr then); hence, it is
		 * protected.
		 */
		PrctlParser() {
			formula = nullptr;
		}

		/*!
		 * Does the actual parsing.
		 *
		 * Is to be called once in a constructor, and never from any other location.
		 * The function is not included in the constructor, as sub classes may use constructors
		 * that calculate the string representation of the formula (e.g. read it from a file), hence they
		 * cannot hand it over to a constructor of this class.
		 *
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
