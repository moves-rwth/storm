/*
 * PCTLPathFormula.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef PCTLPATHFORMULA_H_
#define PCTLPATHFORMULA_H_

#include "PCTLformula.h"
#include "formulaTypes.h"

namespace mrmc {

namespace formula {

//abstract
class PCTLPathFormula : public PCTLFormula {
   public:
      virtual enum pathFormulaTypes type() = 0;
};

} //namespace formula

} //namespace mrmc

#endif /* PCTLPATHFORMULA_H_ */
