/*
 * InvertAction.h
 *
 *  Created on: Jun 22, 2014
 *      Author: Manuel Sascha Weiand
 */

#ifndef STORM_FORMULA_ACTION_INVERTACTION_H_
#define STORM_FORMULA_ACTION_INVERTACTION_H_

#include "src/formula/actions/AbstractAction.h"

namespace storm {
namespace property {
namespace action {

/*!
 * The InvertAction inverts the selection, selecting precisely the unselected states.
 */
template <class T>
class InvertAction : public AbstractAction<T> {

	// Convenience typedef to make the code more readable.
	typedef typename AbstractAction<T>::Result Result;

public:

	/*!
	 * Constructs an InvertAction.
	 *
	 * As the simple inversion of the selection does not need any parameters, the empty constructor is the only one needed.
	 */
	InvertAction() {
		//Intentionally left empty.
	}

	/*!
	 * The virtual destructor.
	 * To ensure that the right destructor is called.
	 */
	virtual ~InvertAction() {
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
		return "invert";
	}

private:

	/*!
	 * Evaluate the action.
	 *
	 * As the InvertAction does not depend on the model or the formula for which the modelchecking result was computed,
	 * it does not depend on the modelchecker at all. This internal version of the evaluate method therefore only needs the
	 * modelchecking result as input.
	 *
	 * @param result The Result struct on which the action is to be evaluated.
	 * @returns A Result struct containing the output of the action.
	 */
	virtual Result evaluate(Result const & result) const {
		storm::storage::BitVector out(result.selection);
		return Result(~out, result.stateMap, result.pathResult, result.stateResult);
	}
};

} //namespace action
} //namespace property
} //namespace storm


#endif /* STORM_FORMULA_ACTION_INVERTACTION_H_ */
