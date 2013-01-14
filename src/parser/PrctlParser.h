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
		PrctlParser(std::string filename);
		 
		/*!
		 *	@brief return formula object parsed from file.
		 */
		std::shared_ptr<storm::formula::PctlFormula<double>> getFormula()
		{
			return this->formula;
		}
	
	private:
		std::shared_ptr<storm::formula::PctlFormula<double>> formula;
		template<typename Iterator, typename Skipper>
		struct PrctlGrammar;
};

} // namespace parser
} // namespace storm

#endif /* STORM_PARSER_PRCTLPARSER_H_ */
