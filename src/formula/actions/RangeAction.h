/*
 * RangeAction.h
 *
 *  Created on: Apr 26, 2014
 *      Author: Manuel Sascha Weiand
 */

#ifndef STORM_FORMULA_ACTION_RANGEACTION_H_
#define STORM_FORMULA_ACTION_RANGEACTION_H_

#include "src/formula/actions/AbstractAction.h"

namespace storm {
namespace property {
namespace action {

template <class T>
class RangeAction : public AbstractAction<T> {

	typedef typename AbstractAction<T>::Result Result;

public:

	RangeAction() : from(0), to(0) {
		//Intentionally left empty.
	}

	/*!
	 *	Including the state with position to.
	 */
	RangeAction(uint_fast64_t from, uint_fast64_t to) : from(from), to(to) {
		if(from > to) {
			throw storm::exceptions::IllegalArgumentValueException() << "The end of the range is lower than its beginning";
		}
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
		std::string out = "range(";
		out += std::to_string(from);
		out += ", ";
		out += std::to_string(to);
		out += ")";
		return out;
	}

private:

	/*!
	 *
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

	uint_fast64_t from;
	uint_fast64_t to;

};

} //namespace action
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_ACTION_RANGEACTION_H_ */
