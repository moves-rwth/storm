/*
 * SortAction.h
 *
 *  Created on: Jun 22, 2014
 *      Author: Manuel Sascha Weiand
 */

#ifndef STORM_FORMULA_ACTION_SORTACTION_H_
#define STORM_FORMULA_ACTION_SORTACTION_H_

#include "src/properties/actions/AbstractAction.h"
#include <cctype>

namespace storm {
namespace properties {
namespace action {

/*!
 * This action manipulates the state ordering by sorting the states by some category.
 *
 * Currently the states can be sorted either by index or by value; ascending or descending.
 * This is done using the standard libraries sort function, thus the action evaluates in O(n*log n) where n is the number of states.
 */
template <class T>
class SortAction : public AbstractAction<T> {

	// Convenience typedef to make the code more readable.
	typedef typename AbstractAction<T>::Result Result;

public:

	//! Enum defining the categories in relation to which the states can be sorted.
	enum SortingCategory {INDEX, VALUE};

	/*!
	 * Construct a SortAction using the given values.
	 *
	 * If no values are given the action will sort by ascending state index.
	 *
	 * @param category An enum value identifying the category by which the states are to be ordered.
	 * @param ascending Determines whether the values are to be sorted in ascending or descending order.
	 *                  The parameter is to be set to true iff the values are to be sorted in ascending order.
	 */
	SortAction(SortingCategory category = INDEX, bool ascending = true) : category(category), ascending(ascending) {
		//Intentionally left empty.
	}

	/*!
	 * The virtual destructor.
	 * To ensure that the right destructor is called.
	 */
	virtual ~SortAction() {
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
		std::string out = "sort(";
		switch (category) {
			case INDEX:
				out += "index";
				break;
			case VALUE:
				out += "value";
				break;
			default:
				LOG4CPLUS_INFO(logger, "Unknown sorting category of value " << category << ".");
				std::cout << "Unknown sorting category of value " << category << "." << std::endl;
				break;
		}
		out += ", ";
		out += ascending ? "ascending)" : "descending)";
		return out;
	}


private:

	/*!
	 * Evaluate the action.
	 *
	 * As the SortAction does not depend on the model or the formula for which the modelchecking result was computed,
	 * it does not depend on the modelchecker at all. This internal version of the evaluate method therefore only needs the
	 * modelchecking result as input.
	 *
	 * @param result The Result struct on which the action is to be evaluated.
	 * @returns A Result struct containing the output of the action.
	 */
	Result evaluate(Result const & result) const {

		if(category == VALUE) {
			//TODO

			if(result.pathResult.size() != 0) {
				return Result(result.selection, sort(result.stateMap, result.pathResult), result.pathResult, result.stateResult);
			} else {
				return Result(result.selection, sort(result.stateMap, result.stateResult), result.pathResult, result.stateResult);
			}

		} else {
			return Result(result.selection, sort(result.stateMap.size()), result.pathResult, result.stateResult);
		}
	}

	/*!
	 * This method returns a vector of the given length filled with the numbers 0 to length-1 in ascending or descending order,
	 * depending on the value of the member variable ascending. Thus it sorts by state index.
	 *
	 * @param length The length of the generated vector.
	 * @returns A vector of unsigned integers from 0 to length-1 in ascending or descending order.
	 */
	std::vector<uint_fast64_t> sort(uint_fast64_t length) const {

		// Project the vector down to its first component.
		std::vector<uint_fast64_t> outMap(length);

		// Sort the combined vector.
		if(ascending) {
			for(uint_fast64_t i = 0; i < length; i++) {
				outMap[i] = i;
			}
		} else {
			for(uint_fast64_t i = 0; i < length; i++) {
				outMap[i] = length - i - 1;
			}
		}

		return outMap;
	}

	/*!
	 * Sort the stateMap vector representing the current state ordering by the values in the values vector.
	 *
	 * Here the entries in the values vector are assumed to be the modelchecking results of a path formula.
	 * Hence, the value at index i is associated with state i, i.e the value i in the stateMap.
	 * The ordering direction (ascending/decending) is given by the member variable ascending, set in the constructor.
	 *
	 * @param stateMap A vector representing the current state ordering.
	 * @param values A vector containing the values by which the stateMap is to be ordered.
	 * @returns A vector containing the reordered entries of the stateMap.
	 */
	std::vector<uint_fast64_t> sort(std::vector<uint_fast64_t> const & stateMap, std::vector<T> const & values) const {

		// Prepare the new state map.
		std::vector<uint_fast64_t> outMap(stateMap);

		// Sort the state map.
		if(ascending) {
			std::sort(outMap.begin(), outMap.end(), [&] (uint_fast64_t a, uint_fast64_t b) -> bool { return values[a] == values[b] ? a < b : values[a] < values[b]; });
		} else {
			std::sort(outMap.begin(), outMap.end(), [&] (uint_fast64_t a, uint_fast64_t b) -> bool { return values[a] == values[b] ? a < b : values[a] > values[b]; });
		}

		return outMap;
	}

	/*!
	 * Sort the stateMap vector representing the current state ordering by the values in the values vector.
	 *
	 * Here the entries in the values vector are assumed to be the modelchecking results of a state formula.
	 * Hence, the value at index i is associated with state i, i.e the value i in the stateMap.
	 * The ordering direction (ascending/decending) is given by the member variable ascending, set in the constructor.
	 *
	 * @param stateMap A vector representing the current state ordering.
	 * @param values A vector containing the values by which the stateMap is to be ordered.
	 * @returns A vector containing the reordered entries of the stateMap.
	 */
	std::vector<uint_fast64_t> sort(std::vector<uint_fast64_t> const & stateMap, storm::storage::BitVector const & values) const {

		// Prepare the new state map.
		std::vector<uint_fast64_t> outMap(stateMap);

		// Sort the state map.
		if(ascending) {
			std::sort(outMap.begin(), outMap.end(), [&] (uint_fast64_t a, uint_fast64_t b) -> bool { return values[a] == values[b] ? a < b : values[a] < values[b]; });
		} else {
			std::sort(outMap.begin(), outMap.end(), [&] (uint_fast64_t a, uint_fast64_t b) -> bool { return values[a] == values[b] ? a < b : values[a] > values[b]; });
		}

		return outMap;
	}

	// The category by which the states are to be ordered.
	SortingCategory category;

	// Determines whether the values are to be sorted in ascending or descending order.
	bool ascending;
};

} //namespace action
} //namespace properties
} //namespace storm

#endif /* STORM_FORMULA_ACTION_SORTACTION_H_ */
