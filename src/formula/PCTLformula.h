/*
 * PCTLformula.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef PCTLFORMULA_H_
#define PCTLFORMULA_H_

#include <string>

namespace mrmc {

namespace formula {


//abstract
class PCTLFormula {
   public:
      virtual std::string toString() = 0;
};

} //namespace formula

} //namespace mrmc

#endif /* PCTLFORMULA_H_ */
