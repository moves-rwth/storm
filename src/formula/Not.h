/*
 * Not.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef NOT_H_
#define NOT_H_

namespace mrmc {

namespace formula {

#include "PCTLStateFormula.h"

class Not : public PCTLStateFormula {
   private:
      PCTLStateFormula* child;
   public:
      Not() {
         this->child = NULL;
      }

      Not(PCTLStateFormula* child) {
         this->child = child;
      }

      virtual ~Not() {

      }

      PCTLStateFormula* getChild() {
         return child;
      }

      void setChild(PCTLStateFormula* child) {
         this->child = child;
      }

      std::string toString() {
         std::string result = "!";
         result += child->toString();
         return result;
      }

      virtual enum stateFormulaTypes type() {
            return NOT;
      }
};

} //namespace formula

} //namespace MRMC

#endif /* NOT_H_ */
