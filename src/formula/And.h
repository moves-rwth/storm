/*
 * And.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef AND_H_
#define AND_H_

#include "PCTLStateFormula.h"
#include <string>

namespace mrmc {

namespace formula {

class And : public PCTLStateFormula {
   private:
      PCTLStateFormula* left;
      PCTLStateFormula* right;
   public:
      And() {
         left = NULL;
         right = NULL;
      }
      And(PCTLStateFormula* left, PCTLStateFormula* right) {
         this->left = left;
         this->right = right;
      }

      void setLeft(PCTLStateFormula* newLeft) {
         left = newLeft;
      }

      void setRight(PCTLStateFormula* newRight) {
         right = newRight;
      }

      PCTLStateFormula* getLeft() {
         return left;
      }

      PCTLStateFormula* getRight() {
         return right;
      }

      std::string toString() {
         std::string result = "(";
         result += left->toString();
         result += " && ";
         result += right->toString();
         result += ")";
         return result;
      }

      virtual enum stateFormulaTypes type() {
            return AND;
      }
};

} //namespace formula

} //namespace mrmc

#endif /* AND_H_ */
