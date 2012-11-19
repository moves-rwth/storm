/*
 * Until.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef UNTIL_H_
#define UNTIL_H_

#include "PCTLPathFormula.h"
#include "PCTLStateFormula.h"

namespace mrmc {

namespace formula {

class Until : public PCTLPathFormula {
      PCTLStateFormula* left;
      PCTLStateFormula* right;
   public:
      Until() {
         this->left = NULL;
         this->right = NULL;
      }

      Until(PCTLStateFormula* left, PCTLStateFormula* right) {
         this->left = left;
         this->right = right;
      }
      virtual ~Until();

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
         result += " U ";
         result += right->toString();
         result += ")";
         return result;
      }

      virtual enum pathFormulaTypes type() {
            return UNTIL;
      }
};

} //namespace formula

} //namespace mrmc

#endif /* UNTIL_H_ */
