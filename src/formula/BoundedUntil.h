/*
 * BoundedUntil.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef BOUNDEDUNTIL_H_
#define BOUNDEDUNTIL_H_

#include "PCTLPathFormula.h"

namespace mrmc {

namespace formula {

class BoundedUntil : public PCTLPathFormula {
      PCTLStateFormula* left;
      PCTLStateFormula* right;
      uint_fast64_t bound;
   public:
      BoundedUntil() {
         this->left = NULL;
         this->right = NULL;
         bound = 0;
      }

      BoundedUntil(PCTLStateFormula* left, PCTLStateFormula* right,
                   uint_fast64_t bound) {
         this->left = left;
         this->right = right;
         this->bound = bound;
      }

      virtual ~BoundedUntil();

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

      uint_fast64_t getBound() {
         return bound;
      }

      void setBound(uint_fast64_t bound) {
         this->bound = bound;
      }

      std::string toString() {
         std::string result = "(";
         result += left->toString();
         result += " U<=";
         result += bound;
         result += " ";
         result += right->toString();
         result += ")";
         return result;
      }

      virtual enum pathFormulaTypes type() {
            return BOUNDED_UNTIL;
      }
};

} //namespace formula

} //namespace mrmc

#endif /* BOUNDEDUNTIL_H_ */
