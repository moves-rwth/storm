/*
 * RangeAction.h
 *
 *  Created on: Apr 26, 2014
 *      Author: Manuel Sascha Weiand
 */

#ifndef STORM_FORMULA_ACTION_RANGEACTION_H_
#define STORM_FORMULA_ACTION_RANGEACTION_H_

#include "src/properties/actions/AbstractAction.h"

namespace storm {
namespace properties {
namespace action {

/*!
 * The RangeAction deselects all states that are not within the given range.
 *
 * The bounds of the range given are indices in the result ordering.
 * Thus, if the lower bound is 0 and the upper bound is 5 then all states that are not
 * at index 0 through 5 (including) in the state ordering will be deselected.
 * This action is thought to be used in connection with the SortAction.
 */
template <class T>
class RangeAction : public AbstractAction<T> {

	// Convenience typedef to make the code more readable.
	typedef typename AbstractAction<T>::Result Result;

public:

	/*!
	 *  Construct a RangeAction using the given values.
	 *
	 *  If no values are given they default to [0,UINT_FAST64_MAX] thus deselecting no state at evaluation.
	 *
	 *	@note The interval bounds are included in the interval.
	 */
	RangeAction(uint_fast64_t from = 0, uint_fast64_t to = UINT_FAST64_MAX) : from(from), to(to) {
		if(from > to) {
			throw storm::exceptions::IllegalArgumentValueException() << "The end of the range is lower than its beginning";
		}
	}

	/*!
	 * The virtual destructor.
	 * To ensure that the right destructor is called.
	 */
	virtual ~RangeAction() {
		//Intentionally left empty
	}

	/*!
	 * Evaluate the action for a Prctl formula.
	 *
	 * @param result The Result struct on which the action is to be evaluated.
	 * @param mc The Prctl modelchecker that computed the path- or stateResult contained in the Result struct.
	 * @returns A Result struct containing the output of the action.
	 */
	virtual Result evaluate(Result const & result, storm::modelchecker::prctl::AbstractModelChecker<T> const & mc) const override {
		return evaluate(result);
	}

	/*!
	 * Evaluate the action for a Csl formula.
	 *
	 * @param result The Result struct on which the action is to be evaluated.
	 * @param mc The Csl modelchecker that computed the path- or stateResult contained in the Result struct.
	 * @returns A Result struct containing the output of the action.
	 */
	virtual Result evaluate(Result const & result, storm::modelchecker::csl::AbstractModelChecker<T> const & mc) const override {
		return evaluate(result);
	}

	/*!
	 * Evaluate the action for a Ltl formula.
	 *
	 * @param result The Result struct on which the action is to be evaluated.
	 * @param mc The Ltl modelchecker that computed the path- or stateResult contained in the Result struct.
	 * @returns A Result struct containing the output of the action.
	 */
	virtual Result evaluate(Result const & result, storm::modelchecker::ltl::AbstractModelChecker<T> const & mc) const override {
		return evaluate(result);
	}

	/*!
	 * Returns a string representation of this action.
	 *
	 * @returns A string representing this action.
	 */
	virtual std::string toString() const override {
		std::string out = "range(";
		out += std::to_string(from);
		out += ", ";
		out += std::to_string(to);
		out += ")";
		return out;
	}

private:

	/*!
	 * Evaluate the action.
	 *
	 * As the RangeAction does not depend on the model or the formula for which the modelchecking result was computed,
	 * it does not depend on the modelchecker at all. This internal version of the evaluate method therefore only needs the
	 * modelchecking result as input.
	 *
	 * @param result The Result struct on which the action is to be evaluated.
	 * @returns A Result struct containing the output of the action.
	 */
	virtual Result evaluate(Result const & result) const {
		//Initialize the output vector.
		storm::storage::BitVector out(result.selection.size());

		uint_fast64_t end = to - from;

		// Safety check for access bounds.
		if(from >= result.stateMap.size()) {
			LOG4CPLUS_WARN(logger, "Range begins behind the end of the states by " << to - result.stateMap.size() << ". No state was selected.");
			std::cout << "Range begins behind the end of the states by " << to - result.stateMap.size() << ". No state was selected." << std::endl;

			return Result(out, result.stateMap, result.pathResult, result.stateResult);
		}

		if(to >= result.stateMap.size()) {

			end = result.selection.size() - from - 1;

			LOG4CPLUS_WARN(logger, "Range ends behind the end of the states by " << to - result.stateMap.size() << ". The range has been cut at the last state.");
			std::cout << "Range ends behind the end of the states by " << to - result.stateMap.size() << ". The range has been cut at the last state." << std::endl;
		}

		//Fill the output vector.
		for(uint_fast64_t i=0; i <= end; i++) {
			out.set(result.stateMap[from + i], result.selection[result.stateMap[from + i]]);
		}

		return Result(out, result.stateMap, result.pathResult, result.stateResult);
	}

	// The lower bound of the interval of states not deselected.
	uint_fast64_t from;

	// The upper bound of the interval of states not deselected.
	uint_fast64_t to;

};

} //namespace action
} //namespace properties
} //namespace storm

#endif /* STORM_FORMULA_ACTION_RANGEACTION_H_ */
