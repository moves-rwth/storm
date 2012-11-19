/*
 * formulaTypes.h
 *
 *  Created on: 21.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef FORMULATYPES_H_
#define FORMULATYPES_H_

enum stateFormulaTypes {
      AND,
      AP,
      NOT,
      OR,
      PROBABILISTIC
};

enum pathFormulaTypes {
      NEXT,
      UNTIL,
      BOUNDED_UNTIL,
      EVENTUALLY,
      ALWAYS
};

#endif /* FORMULATYPES_H_ */
