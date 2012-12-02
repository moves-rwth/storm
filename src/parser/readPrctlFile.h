#ifndef READPRCTLFILE_H_
#define READPRCTLFILE_H_

#include "src/formula/PCTLformula.h"
#include "src/parser/parser.h"

namespace mrmc {
namespace parser {

/*!
 *	@brief Load PRCTL file
 */
class PRCTLParser : Parser
{
	public:
		PRCTLParser(const char * filename);
		 
		/*!
		 *	@brief return formula object parsed from file.
		 */
		mrmc::formula::PCTLFormula* getFormula()
		{
			return this->formula;
		}
	
	private:
		mrmc::formula::PCTLFormula* formula;
};

} // namespace parser
} // namespace mrmc

#endif /* READPRCTLFILE_H_ */
