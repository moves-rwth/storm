/*
 * SortAction.h
 *
 *  Created on: Jun 22, 2014
 *      Author: Manuel Sascha Weiand
 */

#ifndef STORM_FORMULA_ACTION_SORTACTION_H_
#define STORM_FORMULA_ACTION_SORTACTION_H_

#include "src/formula/actions/AbstractAction.h"
#include <cctype>

namespace storm {
namespace property {
namespace action {

template <class T>
class SortAction : public AbstractAction<T> {

	typedef typename AbstractAction<T>::Result Result;

public:

	enum SortingCategory {INDEX, VALUE};

	SortAction() : category(INDEX), ascending(true) {
		//Intentionally left empty.
	}

	SortAction(SortingCategory category, bool ascending = true) : category(category), ascending(ascending) {
		//Intentionally left empty.
	}

	/*!
	 * Virtual destructor
	 * To ensure that the right destructor is called
	 */
	virtual ~SortAction() {
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
		std::string out = "sort, ";
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
		out += ascending ? "ascending" : "descending";
		return out;
	}


private:

	/*!
	 *
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
	 *
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
	 *
	 */
	std::vector<uint_fast64_t> sort(std::vector<uint_fast64_t> const & stateMap, std::vector<T> const & values) const {

		// Prepare the new state map.
		std::vector<uint_fast64_t> outMap(stateMap);

		// Sort the state map.
		if(ascending) {
			std::sort(outMap.begin(), outMap.end(), [&] (uint_fast64_t a, uint_fast64_t b) -> bool { return values[a] <= values[b]; });
		} else {
			std::sort(outMap.begin(), outMap.end(), [&] (uint_fast64_t a, uint_fast64_t b) -> bool { return values[a] >= values[b]; });
		}

		return outMap;
	}

	/*!
	 *
	 */
	std::vector<uint_fast64_t> sort(std::vector<uint_fast64_t> const & stateMap, storm::storage::BitVector const & values) const {

		// Prepare the new state map.
		std::vector<uint_fast64_t> outMap(stateMap);

		// Sort the state map.
		if(ascending) {
			std::sort(outMap.begin(), outMap.end(), [&] (uint_fast64_t a, uint_fast64_t b) -> bool { return values[a] <= values[b]; });
		} else {
			std::sort(outMap.begin(), outMap.end(), [&] (uint_fast64_t a, uint_fast64_t b) -> bool { return values[a] >= values[b]; });
		}

		return outMap;
	}

	SortingCategory category;
	bool ascending;
};

} //namespace action
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_ACTION_SORTACTION_H_ */
