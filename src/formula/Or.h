/*
 * Or.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef OR_H_
#define OR_H_

#include "PCTLStateFormula.h"

namespace mrmc {

namespace formula {

class Or : public PCTLStateFormula {
   private:
      PCTLStateFormula* left;
      PCTLStateFormula* right;
   public:
      Or() {
         left = NULL;
         right = NULL;
      }
      Or(PCTLStateFormula* left, PCTLStateFormula* right) {
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
         result += " || ";
         result += right->toString();
         result += ")";
         return result;
      }

      virtual enum stateFormulaTypes type() {
            return OR;
      }
};

} //namespace formula

} //namespace mrmc

#endif /* OR_H_ */
