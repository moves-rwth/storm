#ifndef READPRCTLFILE_H_
#define READPRCTLFILE_H_

#include "src/formula/PCTLformula.h"

namespace mrmc {
namespace parser {

/*!
 *	@brief Load PRCTL file
 */
mrmc::formula::PCTLFormula* readPrctlFile(const char * filename);

} // namespace parser
} // namespace mrmc

#endif /* READPRCTLFILE_H_ */