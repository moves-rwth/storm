/*
 * Filter.h
 *
 *  Created on: Apr 26, 2014
 *      Author: Manuel Sascha Weiand
 */

#ifndef STORM_FORMULA_ABSTRACTFILTER_H_
#define STORM_FORMULA_ABSTRACTFILTER_H_

#include <vector>
#include "src/formula/AbstractFormula.h"
#include "src/formula/Actions/Action.h"

namespace storm {
namespace property {

template <class T>
class AbstractFilter {

public:

	AbstractFilter() {
		// Intentionally left empty.
	}

	AbstractFilter(action::Action<T>* action) {
		actions.push_back(action);
	}

	AbstractFilter(std::vector<action::Action<T>*> actions) : actions(actions) {
		// Intentionally left empty.
	}

	virtual ~AbstractFilter() {
		actions.clear();
	}

	std::string toFormulaString() const {
		std::string desc = "filter(";
		return desc;
	}

	std::string toString() const {
		std::string desc = "Filter: ";
		desc += "\nActions:";
		for(auto action : actions) {
			desc += "\n\t" + action.toString();
		}
		return desc;
	}

	void addAction(action::Action<T>* action) {
		actions.push_back(action);
	}

	void removeAction() {
		actions.pop_back();
	}

	action::Action<T>* getAction(uint_fast64_t pos) const {
		return actions[pos];
	}

	uint_fast64_t getActionCount() const {
		return actions.size();
	}

protected:

	std::vector<action::Action<T>*> actions;
};

} //namespace property
} //namespace storm



#endif /* STORM_FORMULA_ABSTRACTFILTER_H_ */
