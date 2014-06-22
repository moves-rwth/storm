/*
 * InvertAction.h
 *
 *  Created on: Jun 22, 2014
 *      Author: Manuel Sascha Weiand
 */

#ifndef STORM_FORMULA_ACTION_INVERTACTION_H_
#define STORM_FORMULA_ACTION_INVERTACTION_H_

#include "src/formula/Actions/AbstractAction.h"

namespace storm {
namespace property {
namespace action {

template <class T>
class InvertAction : public AbstractAction<T> {

	typedef typename AbstractAction<T>::Result Result;

public:

	InvertAction() {
		//Intentionally left empty.
	}

	/*!
	 * Virtual destructor
	 * To ensure that the right destructor is called
	 */
	virtual ~InvertAction() {
		//Intentionally left empty
	}

	/*!
	 *
	 */
	virtual Result evaluate(Result const & result, storm::modelchecker::prctl::AbstractModelChecker<T> const & mc) const override {
		return evaluate(result);
	}

	/*!
	 *
	 */
	virtual Result evaluate(Result const & result, storm::modelchecker::csl::AbstractModelChecker<T> const & mc) const override {
		return evaluate(result);
	}

	/*!
	 *
	 */
	virtual Result evaluate(Result const & result, storm::modelchecker::ltl::AbstractModelChecker<T> const & mc) const override {
		return evaluate(result);
	}

	/*!
	 *
	 */
	virtual std::string toString() const override {
		return "invert";
	}

private:

	/*!
	 *
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
