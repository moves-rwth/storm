/*
 * SteadyStateReward.h
 *
 *  Created on: 08.04.2013
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_ABSTRACT_STEADYSTATEREWARD_H_
#define STORM_FORMULA_ABSTRACT_STEADYSTATEREWARD_H_

#include "AbstractFormula.h"
#include "src/formula/AbstractFormulaChecker.h"
#include <string>

namespace storm {
namespace formula {
namespace abstract {

/*!
 * @brief
 * Class for an abstract (path) formula tree with a Steady State Reward node as root.
 *
 * @see AbstractFormula
 */
template <class T>
class SteadyStateReward: public virtual AbstractFormula<T> {
public:
	/*!
	 * Empty constructor
	 */
	SteadyStateReward() {
		// Intentionally left empty

	}
	virtual ~SteadyStateReward() {
		// Intentionally left empty
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const {
		return "S";
	}

	/*!
     *  @brief Checks if all subtrees conform to some logic.
     *
     *  As SteadyStateReward objects have no subformulas, we return true here.
     *
     *  @param checker Formula checker object.
     *  @return true
     */
	virtual bool conforms(const AbstractFormulaChecker<T>& checker) const {
		return true;
	}
};

} //namespace abstract
} //namespace formula
} //namespace storm
#endif /* STORM_FORMULA_ABSTRACT_STEADYSTATEREWARD_H_ */
