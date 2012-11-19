/*
 * ProbabilisticOperator.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef PROBABILISTICOPERATOR_H_
#define PROBABILISTICOPERATOR_H_

#include "PCTLStateFormula.h"

namespace mrmc {

namespace formula {

template<class T>
class ProbabilisticOperator : public PCTLStateFormula {
      T lower;
      T upper;
      PCTLPathFormula* pathFormula;
   public:
      ProbabilisticOperator(T lowerBound, T upperBound, PCTLPathFormula* pathFormula=NULL) {
         this->lower = lowerBound;
         this->upper = upperBound;
         this->pathFormula = pathFormula;
      }

      virtual ~ProbabilisticOperator() {
         delete pathFormula;
      }

      PCTLPathFormula* getPathFormula () {
         return pathFormula;
      }

      T getLowerBound() {
         return lower;
      }

      T getUpperBound() {
         return upper;
      }

      void setPathFormula(PCTLPathFormula* pathFormula) {
         this->pathFormula = pathFormula;
      }

      void setInterval(T lowerBound, T upperBound) {
         this->lower = lowerBound;
         this->upper = upperBound;
      }

      std::string toString() {
         std::string result = "(";
         result += " P[";
         result += lower;
         result += ";";
         result += upper;
         result += "] ";
         result += pathFormula->toString();
         result += ")";
         return result;
      }

      virtual enum stateFormulaTypes type() {
            return PROBABILISTIC;
      }
};

} //namespace formula

} //namespace mrmc

#endif /* PROBABILISTICOPERATOR_H_ */
