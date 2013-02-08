#ifndef STORM_PARSER_PRCTLPARSER_H_
#define STORM_PARSER_PRCTLPARSER_H_

#include "src/parser/Parser.h"

#include "src/formula/Formulas.h"
#include <memory>

namespace storm {
namespace parser {

/*!
 *	@brief Load PRCTL file
 */
class PrctlParser : Parser
{
	public:
		PrctlParser() { }
		PrctlParser(std::string formulaString);
		 
		/*!
		 *	@brief return formula object parsed from file.
		 */
	storm::formula::AbstractFormula<double>* getFormula()
		{
			return this->formula;
		}
	
	protected:
		/*!
		 * Parses a formula and stores the result in the field "formula"
		 * @param formula The string representation of the formula to parse
		 */
		void parse(std::string formula);

	private:
		storm::formula::AbstractFormula<double>* formula;

		template<typename Iterator, typename Skipper>
		struct PrctlGrammar;

};

} // namespace parser
} // namespace storm

#endif /* STORM_PARSER_PRCTLPARSER_H_ */
