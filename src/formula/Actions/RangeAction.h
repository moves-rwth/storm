/*
 * RangeAction.h
 *
 *  Created on: Apr 26, 2014
 *      Author: Manuel Sascha Weiand
 */

#ifndef STORM_FORMULA_ACTION_RANGEACTION_H_
#define STORM_FORMULA_ACTION_RANGEACTION_H_

#include "src/formula/Actions/AbstractAction.h"

#include <string>

namespace storm {
namespace property {
namespace action {

template <class T>
class RangeAction : public AbstractAction<T> {

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
	virtual storm::storage::BitVector evaluate(storm::storage::BitVector input) const override {
		//Initialize the output vector.
		storm::storage::BitVector out(to - from  + 1);

		storm::storage::BitVector::const_iterator it = input.begin();

		//Proceed the set index iterator to the first set bit within the range.
		for(; *it < from; ++it);

		//Fill the output vector.
		for(; *it <= to; ++it) {
			out.set(*it-from);
		}
		return out;
	}

	/*!
	 *
	 */
	virtual std::string toString() const override {
		std::string out = "range, ";
		out += std::to_string(from);
		out += ", ";
		out += std::to_string(to);
		return out;
	}

	/*!
	 *
	 */
	virtual std::string toFormulaString() const override {
		std::string out = "\"range\", ";
		out += std::to_string(from);
		out += ", ";
		out += std::to_string(to);
		return out;
	}

private:
	uint_fast64_t from;
	uint_fast64_t to;

};

} //namespace action
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_ACTION_RANGEACTION_H_ */
