/*
 * Filter.h
 *
 *  Created on: Apr 26, 2014
 *      Author: Manuel Sascha Weiand
 */

#ifndef STORM_FORMULA_ABSTRACTFILTER_H_
#define STORM_FORMULA_ABSTRACTFILTER_H_

#include <vector>
#include <string>
#include "src/formula/AbstractFormula.h"
#include "src/formula/Actions/AbstractAction.h"

namespace storm {
namespace property {

/*!
 *
 */
enum OptimizingOperator {MINIMIZE, MAXIMIZE, UNDEFINED};

template <class T>
class AbstractFilter {

public:

	AbstractFilter(OptimizingOperator opt = UNDEFINED) : opt(opt) {
		// Intentionally left empty.
	}

	AbstractFilter(action::AbstractAction<T>* action, OptimizingOperator opt = UNDEFINED) : opt(opt) {
		actions.push_back(action);
	}

	AbstractFilter(std::vector<action::AbstractAction<T>*> actions, OptimizingOperator opt = UNDEFINED) : actions(actions), opt(opt) {
		// Intentionally left empty.
	}

	virtual ~AbstractFilter() {
		actions.clear();
	}

	virtual std::string toString() const {
		std::string desc = "filter(";

		for(auto action : actions) {
			desc += action->toString();
			desc += ", ";
		}

		// Remove the last ", ".
		if(!actions.empty()) {
			desc.pop_back();
			desc.pop_back();
		}

		desc += ")";

		return desc;
	}

	virtual std::string toPrettyString() const {
		std::string desc = "Filter: ";
		desc += "\nActions:";
		for(auto action : actions) {
			desc += "\n\t" + action->toString();
		}
		return desc;
	}

	void addAction(action::AbstractAction<T>* action) {
		actions.push_back(action);
	}

	void removeAction() {
		actions.pop_back();
	}

	action::AbstractAction<T>* getAction(uint_fast64_t pos) const {
		return actions[pos];
	}

	uint_fast64_t getActionCount() const {
		return actions.size();
	}

	void setOptimizingOperator(OptimizingOperator opt) {
		this->opt = opt;
	}

	OptimizingOperator getOptimizingOperator() const {
		return opt;
	}

protected:

	std::vector<action::AbstractAction<T>*> actions;

	OptimizingOperator opt;
};

} //namespace property
} //namespace storm



#endif /* STORM_FORMULA_ABSTRACTFILTER_H_ */
