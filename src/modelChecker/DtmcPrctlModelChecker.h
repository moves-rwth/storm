/*
 * DtmcPrctlModelChecker.h
 *
 *  Created on: 22.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef DTMCPRCTLMODELCHECKER_H_
#define DTMCPRCTLMODELCHECKER_H_

#include "src/formula/And.h"
#include "src/formula/AP.h"
#include "src/formula/BoundedUntil.h"
#include "src/formula/Next.h"
#include "src/formula/Not.h"
#include "src/formula/Or.h"
#include "src/formula/ProbabilisticOperator.h"
#include "src/formula/Until.h"

#include "src/models/dtmc.h"
#include "src/vector/bitvector.h"
#include <vector>

namespace mrmc {

namespace modelChecker {

template<class T>
class DtmcPrctlModelChecker {
   private:
      mrmc::models::Dtmc<T>* dtmc;

   protected:
      mrmc::models::Dtmc<T>* getDtmc() const {
         return this->dtmc;
      }

   public:
      explicit DtmcPrctlModelChecker(mrmc::models::Dtmc<T>* DTMC);
      ~DtmcPrctlModelChecker();

      virtual void makeAbsorbing(mrmc::vector::BitVector*) = 0;
      virtual mrmc::vector::BitVector getStatesSatisying(mrmc::models::SingleAtomicPropositionLabeling*) = 0;
      virtual std::vector<T> multiplyMatrixVector(std::vector<T>*) = 0;

      virtual mrmc::vector::BitVector checkStateFormula(mrmc::formula::PCTLStateFormula* formula) {
         if (formula->type() == AND) {
            return checkAnd(static_cast<mrmc::formula::And*>(formula));
         }
         if (formula->type() == stateFormulaTypes::AP) {
            return checkAP(static_cast<mrmc::formula::AP*>(formula));
         }
         if (formula->type() == NOT) {
            return checkNot(static_cast<mrmc::formula::Not*>(formula));
         }
         if (formula->type() == OR) {
            return checkOr(static_cast<mrmc::formula::Or*>(formula));
         }
         if (formula->type() == PROBABILISTIC) {
            return checkProbabilisticOperator(
                  static_cast<mrmc::formula::ProbabilisticOperator<T>*>(formula));
         }
      }


      virtual mrmc::vector::BitVector checkAnd(mrmc::formula::And*) = 0;
      virtual mrmc::vector::BitVector checkAP(mrmc::formula::AP*) = 0;
      virtual mrmc::vector::BitVector checkNot(mrmc::formula::Not*) = 0;
      virtual mrmc::vector::BitVector checkOr(mrmc::formula::Or*) = 0;
      virtual mrmc::vector::BitVector checkProbabilisticOperator(mrmc::formula::ProbabilisticOperator<T>*) = 0;

      virtual std::vector<T> checkPathFormula(mrmc::formula::PCTLPathFormula* formula) {
         if (formula->type() == NEXT) {
            return checkNext(static_cast<mrmc::formula::Next*>(formula));
         }
         if (formula->type() == UNTIL) {
            return checkUntil(static_cast<mrmc::formula::Until*>(formula));
         }
         if (formula->type() == BOUNDED_UNTIL) {
            return checkBoundedUntil(static_cast<mrmc::formula::BoundedUntil*>(formula));
         }
      }

      virtual std::vector<T> checkBoundedUntil(mrmc::formula::BoundedUntil*) = 0;
      virtual std::vector<T> checkNext(mrmc::formula::Next*) = 0;
      virtual std::vector<T> checkUntil(mrmc::formula::Until*) = 0;
};

} //namespace modelChecker

} //namespace mrmc

#endif /* DTMCPRCTLMODELCHECKER_H_ */
