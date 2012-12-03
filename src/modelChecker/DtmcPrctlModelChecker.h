/*
 * DtmcPrctlModelChecker.h
 *
 *  Created on: 22.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef DTMCPRCTLMODELCHECKER_H_
#define DTMCPRCTLMODELCHECKER_H_

namespace mrmc {

namespace modelChecker {

/* The formula classes need to reference a model checker for the check function,
 * which is used to infer the correct type of formula,
 * so the model checker class is declared here already.
 *
 */
template <class T>
class DtmcPrctlModelChecker;
}

}

#include "src/formula/PCTLPathFormula.h"
#include "src/formula/PCTLStateFormula.h"

#include "src/formula/And.h"
#include "src/formula/AP.h"
#include "src/formula/BoundedUntil.h"
#include "src/formula/Next.h"
#include "src/formula/Not.h"
#include "src/formula/Or.h"
#include "src/formula/ProbabilisticOperator.h"
#include "src/formula/Until.h"

#include "src/models/Dtmc.h"
#include "src/storage/BitVector.h"
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
      virtual ~DtmcPrctlModelChecker();

      virtual void makeAbsorbing(mrmc::storage::BitVector*) = 0;
      virtual mrmc::storage::BitVector& getStatesSatisying(std::string) = 0;
      virtual std::vector<T> multiplyMatrixVector(std::vector<T>*) = 0;

      virtual mrmc::storage::BitVector checkStateFormula(mrmc::formula::PCTLStateFormula<T>* formula) {
         return formula->check(this);
      }


      virtual mrmc::storage::BitVector checkAnd(mrmc::formula::And<T>*) = 0;
      virtual mrmc::storage::BitVector checkAP(mrmc::formula::AP<T>*) = 0;
      virtual mrmc::storage::BitVector checkNot(mrmc::formula::Not<T>*) = 0;
      virtual mrmc::storage::BitVector checkOr(mrmc::formula::Or<T>*) = 0;
      virtual mrmc::storage::BitVector checkProbabilisticOperator(mrmc::formula::ProbabilisticOperator<T>*) = 0;

      virtual std::vector<T> checkPathFormula(mrmc::formula::PCTLPathFormula<T>* formula) {
         return formula->check(this);
      }

      virtual std::vector<T> checkBoundedUntil(mrmc::formula::BoundedUntil<T>*) = 0;
      virtual std::vector<T> checkNext(mrmc::formula::Next<T>*) = 0;
      virtual std::vector<T> checkUntil(mrmc::formula::Until<T>*) = 0;
};

} //namespace modelChecker

} //namespace mrmc

#endif /* DTMCPRCTLMODELCHECKER_H_ */
