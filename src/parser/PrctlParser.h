#ifndef MRMC_PARSER_PRCTLPARSER_H_
#define MRMC_PARSER_PRCTLPARSER_H_

#include "src/formula/PCTLformula.h"
#include "src/parser/Parser.h"

namespace mrmc {
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
		mrmc::formula::PCTLFormula<double>* getFormula()
		{
			return this->formula;
		}
	
	private:
		mrmc::formula::PCTLFormula<double>* formula;
};

} // namespace parser
} // namespace mrmc

#endif /* MRMC_PARSER_PRCTLPARSER_H_ */
