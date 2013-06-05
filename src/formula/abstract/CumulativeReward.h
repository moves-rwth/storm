/*
 * InstantaneousReward.h
 *
 *  Created on: 26.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_FORMULA_ABSTRACT_CUMULATIVEREWARD_H_
#define STORM_FORMULA_ABSTRACT_CUMULATIVEREWARD_H_

#include "AbstractFormula.h"
#include "src/formula/AbstractFormulaChecker.h"
#include <string>

namespace storm {
namespace property {
namespace abstract {

/*!
 * @brief
 * Class for an abstract (path) formula tree with a Cumulative Reward node as root.
 *
 * The subtrees are seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @see AbstractPathFormula
 * @see AbstractFormula
 */
template <class T>
class CumulativeReward : public virtual AbstractFormula<T> {

public:
	/*!
	 * Empty constructor
	 */
	CumulativeReward() {
		bound = 0;
	}

	/*!
	 * Constructor
	 *
	 * @param bound The time bound of the reward formula
	 */
	CumulativeReward(T bound) {
		this->bound = bound;
	}

	/*!
	 * Empty destructor.
	 */
	virtual ~CumulativeReward() {
		// Intentionally left empty.
	}

	/*!
	 * @returns the time instance for the instantaneous reward operator
	 */
	T getBound() const {
		return bound;
	}

	/*!
	 * Sets the the time instance for the instantaneous reward operator
	 *
	 * @param bound the new bound.
	 */
	void setBound(T bound) {
		this->bound = bound;
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const override {
		std::string result = "C <= ";
		result += std::to_string(bound);
		return result;
	}
	
	/*!
     *  @brief Checks if all subtrees conform to some logic.
     *  
     *  As CumulativeReward objects have no subformulas, we return true here.
     * 
     *  @param checker Formula checker object.
     *  @return true
     */	
	virtual bool validate(const AbstractFormulaChecker<T>& checker) const override {
		return true;
	}

private:
	T bound;
};

} //namespace abstract
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_ABSTRACT_INSTANTANEOUSREWARD_H_ */
