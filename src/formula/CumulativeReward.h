/*
 * InstantaneousReward.h
 *
 *  Created on: 26.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_FORMULA_CUMULATIVEREWARD_H_
#define STORM_FORMULA_CUMULATIVEREWARD_H_

#include "AbstractPathFormula.h"
#include "AbstractStateFormula.h"
#include "src/formula/AbstractFormulaChecker.h"
#include "boost/integer/integer_mask.hpp"
#include <string>

namespace storm {

namespace formula {

template <class T> class CumulativeReward;

/*!
 *  @brief Interface class for model checkers that support CumulativeReward.
 *
 *  All model checkers that support the formula class CumulativeReward must inherit
 *  this pure virtual class.
 */
template <class T>
class ICumulativeRewardModelChecker {
    public:
		/*!
         *  @brief Evaluates CumulativeReward formula within a model checker.
         *
         *  @param obj Formula object with subformulas.
         *  @return Result of the formula for every node.
         */
        virtual std::vector<T>* checkCumulativeReward(const CumulativeReward<T>& obj) const = 0;
};

/*!
 * @brief
 * Class for a Abstract (path) formula tree with a Cumulative Reward node as root.
 *
 * The subtrees are seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @see AbstractPathFormula
 * @see AbstractFormula
 */
template <class T>
class CumulativeReward : public AbstractPathFormula<T> {

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
	CumulativeReward(uint_fast64_t bound) {
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
	virtual std::string toString() const {
		std::string result = "C<=";
		result += std::to_string(bound);
		return result;
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new CumulativeReward-object that is identical the called object.
	 */
	virtual AbstractPathFormula<T>* clone() const {
		return new CumulativeReward(bound);
	}


	/*!
	 * Calls the model checker to check this formula.
	 * Needed to infer the correct type of formula class.
	 *
	 * @note This function should only be called in a generic check function of a model checker class. For other uses,
	 *       the methods of the model checker should be used.
	 *
	 * @returns A vector indicating the probability that the formula holds for each state.
	 */
	virtual std::vector<T> *check(const storm::modelChecker::AbstractModelChecker<T>& modelChecker) const {
		return modelChecker.template as<ICumulativeRewardModelChecker>()->checkCumulativeReward(*this);
	}
	
	/*!
     *  @brief Checks if all subtrees conform to some logic.
     *  
     *  As CumulativeReward objects have no subformulas, we return true here.
     * 
     *  @param checker Formula checker object.
     *  @return true
     */	
	virtual bool conforms(const AbstractFormulaChecker<T>& checker) const {
		return true;
	}

private:
	uint_fast64_t bound;
};

} //namespace formula

} //namespace storm

#endif /* STORM_FORMULA_INSTANTANEOUSREWARD_H_ */
