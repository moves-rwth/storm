#ifndef STORM_PARSER_PRCTLPARSER_H_
#define STORM_PARSER_PRCTLPARSER_H_

// #include "src/formula/PctlFormula.h"
#include "src/parser/Parser.h"

namespace storm {
namespace parser {

/*!
 *	@brief Load PRCTL file
 */
class PrctlParser : Parser
{
	public:
		PrctlParser(const char * filename);
		 
		/*!
		 *	@brief return formula object parsed from file.
		 */
/*		storm::formula::PctlFormula<double>* getFormula()
		{
			return this->formula;
		}
	
	private:
		storm::formula::PctlFormula<double>* formula;
*/
};

} // namespace parser
} // namespace storm

#endif /* STORM_PARSER_PRCTLPARSER_H_ */
