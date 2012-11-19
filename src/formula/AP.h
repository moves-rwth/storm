/*
 * AP.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef AP_H_
#define AP_H_

#include "PCTLStateFormula.h"

namespace mrmc {

namespace formula {

class AP : public PCTLStateFormula {
   private:
   std::string ap;
   public:
      AP(std::string ap) {
         this->ap = ap;
      }

      AP(char* ap) {
         this->ap = ap;
      }

      std::string getAP() {
         return ap;
      }

      std::string toString() {
         return getAP();
      }

      virtual enum stateFormulaTypes type() {
            return stateFormulaTypes::AP;
      }
};

} //namespace formula

} //namespace mrmc

#endif /* AP_H_ */
