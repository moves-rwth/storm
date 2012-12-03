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

/*!
 * @brief
 * Interface for model checker classes.
 *
 * This class provides basic functions that are the same for all subclasses, but mainly only declares
 * abstract methods that are to be implemented in concrete instances.
 *
 * @attention This class is abstract.
 */
template<class T>
class DtmcPrctlModelChecker {
   private:
      mrmc::models::Dtmc<T>* dtmc;

   protected:
      /*!
       * @returns A reference to the dtmc of the model checker.
       */
      mrmc::models::Dtmc<T>* getDtmc() const {
         return this->dtmc;
      }

   public:
      /*!
       * Constructor
       */
      explicit DtmcPrctlModelChecker(mrmc::models::Dtmc<T>* DTMC);

      /*!
       * Destructor
       */
      virtual ~DtmcPrctlModelChecker();

      /*!
       * Makes all states in a given set absorbing (i.e. adds self loops with probability 1 and removes all
       * other edges from these states)
       *
       * @param states A bit vector representing a set of states that should become absorbing.
       */
      virtual void makeAbsorbing(mrmc::storage::BitVector* states) = 0;

      /*!
       * Returns all states that are labeled with a given atomic proposition.
       *
       * @param ap A string representing an atomic proposition.
       * @returns The set of states labeled with the atomic proposition ap.
       */
      virtual mrmc::storage::BitVector& getStatesLabeledWith(std::string ap) = 0;

      /*!
       * Multiplies the matrix with a given vector; the result again is a vector.
       *
       * @param vector The vector to multiply the matrix with.
       * @returns The result of multiplying the transition probability matrix with vector.
       */
      virtual std::vector<T> multiplyMatrixVector(std::vector<T>* vector) = 0;

      /*!
       * The check method for a state formula; Will infer the actual type of formula and delegate it
       * to the specialized method
       *
       * @param formula The state formula to check
       * @returns The set of states satisfying the formula, represented by a bit vector
       */
      virtual mrmc::storage::BitVector checkStateFormula(mrmc::formula::PCTLStateFormula<T>* formula) {
         return formula->check(this);
      }

      /*!
       * The check method for a state formula with an And node as root in its formula tree
       *
       * @param formula The And formula to check
       * @returns The set of states satisfying the formula, represented by a bit vector
       */
      virtual mrmc::storage::BitVector checkAnd(mrmc::formula::And<T>* formula) = 0;

      /*!
       * The check method for a formula with an AP node as root in its formula tree
       *
       * @param formula The AP state formula to check
       * @returns The set of states satisfying the formula, represented by a bit vector
       */
      virtual mrmc::storage::BitVector checkAP(mrmc::formula::AP<T>*) = 0;

      /*!
       * The check method for a formula with a Not node as root in its formula tree
       *
       * @param formula The Not state formula to check
       * @returns The set of states satisfying the formula, represented by a bit vector
       */
      virtual mrmc::storage::BitVector checkNot(mrmc::formula::Not<T>*) = 0;

      /*!
       * The check method for a state formula with an Or node as root in its formula tree
       *
       * @param formula The Or state formula to check
       * @returns The set of states satisfying the formula, represented by a bit vector
       */
      virtual mrmc::storage::BitVector checkOr(mrmc::formula::Or<T>*) = 0;

      /*!
       * The check method for a state formula with a probabilistic operator node as root in its formula tree
       *
       * @param formula The state formula to check
       * @returns The set of states satisfying the formula, represented by a bit vector
       */
      virtual mrmc::storage::BitVector checkProbabilisticOperator(mrmc::formula::ProbabilisticOperator<T>*) = 0;

      /*!
       * The check method for a path formula; Will infer the actual type of formula and delegate it
       * to the specialized method
       *
       * @param formula The path formula to check
       * @returns for each state the probability that the path formula holds.
       */
      virtual std::vector<T> checkPathFormula(mrmc::formula::PCTLPathFormula<T>* formula) {
         return formula->check(this);
      }

      /*!
       * The check method for a path formula with a Bounded Until operator node as root in its formula tree
       *
       * @param formula The Bounded Until path formula to check
       * @returns for each state the probability that the path formula holds.
       */
      virtual std::vector<T> checkBoundedUntil(mrmc::formula::BoundedUntil<T>*) = 0;

      /*!
       * The check method for a path formula with a Next operator node as root in its formula tree
       *
       * @param formula The Next path formula to check
       * @returns for each state the probability that the path formula holds.
       */
      virtual std::vector<T> checkNext(mrmc::formula::Next<T>*) = 0;

      /*!
       * The check method for a path formula with an Until operator node as root in its formula tree
       *
       * @param formula The Until path formula to check
       * @returns for each state the probability that the path formula holds.
       */
      virtual std::vector<T> checkUntil(mrmc::formula::Until<T>*) = 0;
};

} //namespace modelChecker

} //namespace mrmc

#endif /* DTMCPRCTLMODELCHECKER_H_ */
