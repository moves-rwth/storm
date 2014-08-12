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
#include "src/formula/actions/AbstractAction.h"

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

	AbstractFilter(std::shared_ptr<action::AbstractAction<T>> const & action, OptimizingOperator opt = UNDEFINED) : opt(opt) {
		if(action.get() != nullptr) {
			actions.push_back(action);
		}
	}

	AbstractFilter(std::vector<std::shared_ptr<action::AbstractAction<T>>> const & actions, OptimizingOperator opt = UNDEFINED) {
		// Filter out all nullptr actions.
		// First detect that there is at least one.
		uint_fast64_t emptyCount = 0;
		for(uint_fast64_t i = 0; i < actions.size(); i++) {
			if (actions[i].get() == nullptr) {
				emptyCount++;
			}
		}

		if(emptyCount > 0) {
			// There is at least one nullptr action.
			// Allocate space for the non null actions.
			this->actions.reserve(actions.size() - emptyCount);

			// Fill the vector. Note: For most implementations of the standard there will be no reallocation in the vector while doing this.
			for(auto action : actions){
				if(action.get() != nullptr) {
					this->actions.push_back(action);
				}
			}
		} else {
			this->actions = actions;
		}

		this->opt = opt;
	}

	virtual ~AbstractFilter() {
		// Intentionally left empty.
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

	void addAction(std::shared_ptr<action::AbstractAction<T>> const & action) {
		if(action.get() != nullptr) {
			actions.push_back(action);
		}
	}

	void removeAction() {
		actions.pop_back();
	}

	std::shared_ptr<action::AbstractAction<T>> getAction(uint_fast64_t position) {
		// Make sure the chosen position is not beyond the end of the vector.
		// If it is so return the last element.
		if(position < actions.size()) {
			return actions[position];
		} else {
			return actions[actions.size()-1];
		}
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

	std::vector<std::shared_ptr<action::AbstractAction<T>>> actions;

	OptimizingOperator opt;
};

} //namespace property
} //namespace storm



#endif /* STORM_FORMULA_ABSTRACTFILTER_H_ */
