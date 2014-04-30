/*
 * MinMaxAction.h
 *
 *  Created on: Apr 30, 2014
 *      Author: Manuel Sascha Weiand
 */

#ifndef STORM_FORMULA_ACTION_MINMAXACTION_H_
#define STORM_FORMULA_ACTION_MINMAXACTION_H_

#include "src/formula/Actions/Action.h"

namespace storm {
namespace property {
namespace action {

template <class T>
class MinMaxAction : Action<T> {

public:

	MinMaxAction() : minimize(true) {
		//Intentionally left empty.
	}

	explicit MinMaxAction(bool minimize) : minimize(minimize) {
		//Intentionally left empty.
	}

	/*!
	 *
	 */
	virtual std::string toString() const override {
		return minimize ? "min" : "max";
	}

	/*!
	 *
	 */
	virtual std::string toFormulaString() const override {
		return minimize ? "min" : "max";
	}

	/*!
	 *
	 */
	bool getMinimize() {
		return minimize;
	}

private:
	bool minimize;

};

} //namespace action
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_ACTION_MINMAXACTION_H_ */
