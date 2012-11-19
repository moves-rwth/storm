/*
 * PCTLStateFormula.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef PCTLSTATEFORMULA_H_
#define PCTLSTATEFORMULA_H_

#include "PCTLformula.h"
#include "formulaTypes.h"

namespace mrmc {

namespace formula {

//abstract
class PCTLStateFormula : public PCTLFormula {
   public:
      virtual enum stateFormulaTypes type() = 0;

};

} //namespace formula

} //namespace mrmc


#endif /* PCTLSTATEFORMULA_H_ */
