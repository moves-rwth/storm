/*
 * Next.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef NEXT_H_
#define NEXT_H_

#include "PCTLPathFormula.h"
#include "PCTLStateFormula.h"

namespace mrmc {

namespace formula {


class Next : public PCTLPathFormula {
   private:
      PCTLStateFormula* child;
   public:
      Next() {
         this->child = NULL;
      }

      Next(PCTLStateFormula* child) {
         this->child = child;
      }

      PCTLStateFormula* getChild() {
         return child;
      }

      void setChild(PCTLStateFormula* child) {
         this->child = child;
      }

      std::string toString() {
         std::string result = "(";
         result += " X ";
         result += child->toString();
         result += ")";
         return result;
      }

      virtual enum pathFormulaTypes type() {
            return NEXT;
      }
};

} //namespace formula

} //namespace mrmc

#endif /* NEXT_H_ */
