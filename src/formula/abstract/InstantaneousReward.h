/*
 * InstantaneousReward.h
 *
 *  Created on: 26.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_FORMULA_ABSTRACT_INSTANTANEOUSREWARD_H_
#define STORM_FORMULA_ABSTRACT_INSTANTANEOUSREWARD_H_

#include "AbstractFormula.h"
#include "src/formula/AbstractFormulaChecker.h"
#include <cstdint>
#include <string>

namespace storm {
namespace property {
namespace abstract {

/*!
 * @brief
 * Class for an abstract (path) formula tree with a Instantaneous Reward node as root.
 *
 * The subtrees are seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @see AbstractFormula
 */
template <class T>
class InstantaneousReward : public virtual AbstractFormula<T> {

public:
	/*!
	 * Empty constructor
	 */
	InstantaneousReward() {
		bound = 0;
	}

	/*!
	 * Constructor
	 *
	 * @param bound The time instance of the reward formula
	 */
	InstantaneousReward(uint_fast64_t bound) {
		this->bound = bound;
	}

	/*!
	 * Empty destructor.
	 */
	virtual ~InstantaneousReward() {
		// Intentionally left empty.
	}

	/*!
	 * @returns the time instance for the instantaneous reward operator
	 */
	uint_fast64_t getBound() const {
		return bound;
	}

	/*!
	 * Sets the the time instance for the instantaneous reward operator
	 *
	 * @param bound the new bound.
	 */
	void setBound(uint_fast64_t bound) {
		this->bound = bound;
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const override {
		std::string result = "I=";
		result += std::to_string(bound);
		return result;
	}
	
	/*!
     *  @brief Checks if all subtrees conform to some logic.
     *  
     *  As InstantaneousReward formulas have no subformulas, we return true here.
     * 
     *  @param checker Formula checker object.
     *  @return true
     */
	virtual bool validate(const AbstractFormulaChecker<T>& checker) const override {
		return true;
	}

private:
	uint_fast64_t bound;
};

} //namespace abstract
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_ABSTRACT_INSTANTANEOUSREWARD_H_ */
