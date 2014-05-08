/*
 * RangeAction.h
 *
 *  Created on: Apr 26, 2014
 *      Author: Manuel Sascha Weiand
 */

#ifndef STORM_FORMULA_ACTION_RANGEACTION_H_
#define STORM_FORMULA_ACTION_RANGEACTION_H_

#include "src/formula/Actions/AbstractAction.h"

namespace storm {
namespace property {
namespace action {

template <class T>
class RangeAction : AbstractAction<T> {

public:

	RangeAction() : from(0), to(0) {
		//Intentionally left empty.
	}

	RangeAction(uint_fast64_t from, uint_fast64_t to) : from(from), to(to) {
		//Intentionally left empty.
	}

	/*!
	 * Virtual destructor
	 * To ensure that the right destructor is called
	 */
	virtual ~RangeAction() {
		//Intentionally left empty
	}

	/*!
	 *
	 */
	virtual std::vector<T> evaluate(std::vector<T> input) const override {
		// The range constructor is used here instead of manipulating the incoming vector.
		// While deleting the element at the end of the vector is efficient, deleting elements at any position but the end is not.
		// Also this leaves the incoming vector unchanged.
		std::vector<T> out(input.begin() + from, input.begin() + to);
		return out;
	}

	/*!
	 *
	 */
	virtual storm::storage::BitVector<T> evaluate(storm::storage::BitVector<T> input) const override {
		storm::storage::BitVector<T> out(to - from  + 1, input.begin() + from, input.begin() + to);
		return out;
	}

	/*!
	 *
	 */
	virtual std::string toString() const override {
		return "range, " + from + ", " + to;
	}

	/*!
	 *
	 */
	virtual std::string toFormulaString() const override {
		return "\"range\", " + from + ", " + to;
	}

private:
	uint_fast64_t from;
	uint_fast64_t to;

};

} //namespace action
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_ACTION_RANGEACTION_H_ */
