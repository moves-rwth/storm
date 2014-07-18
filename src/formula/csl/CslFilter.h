/*
 * CslFilter.h
 *
 *  Created on: May 7, 2014
 *      Author: Manuel Sascha Weiand
 */

#ifndef STORM_FORMULA_PRCTL_CSLFILTER_H_
#define STORM_FORMULA_PRCTL_CSLFILTER_H_

#include "src/formula/AbstractFilter.h"
#include "src/formula/csl/AbstractCslFormula.h"
#include "src/formula/csl/AbstractPathFormula.h"
#include "src/formula/csl/AbstractStateFormula.h"
#include "src/modelchecker/csl/AbstractModelChecker.h"

#include "src/formula/actions/AbstractAction.h"

namespace storm {
namespace property {
namespace action {
 template <typename T> class AbstractAction;
}
}
}

namespace storm {
namespace property {
namespace csl {

template <class T>
class CslFilter : public storm::property::AbstractFilter<T> {

	typedef typename storm::property::action::AbstractAction<T>::Result Result;

public:

	CslFilter() : AbstractFilter<T>(UNDEFINED), child(nullptr), steadyStateQuery(false) {
		// Intentionally left empty.
	}

	CslFilter(std::shared_ptr<AbstractCslFormula<T>> const & child, OptimizingOperator opt = UNDEFINED, bool steadyStateQuery = false) : AbstractFilter<T>(opt), child(child), steadyStateQuery(steadyStateQuery) {
		// Intentionally left empty.
	}

	CslFilter(std::shared_ptr<AbstractCslFormula<T>> const & child, action::AbstractAction<T>* action, OptimizingOperator opt = UNDEFINED, bool steadyStateQuery = false) : AbstractFilter<T>(action, opt), child(child), steadyStateQuery(steadyStateQuery) {
		// Intentionally left empty
	}

	CslFilter(std::shared_ptr<AbstractCslFormula<T>> const & child, std::vector<action::AbstractAction<T>*> actions, OptimizingOperator opt = UNDEFINED, bool steadyStateQuery = false) : AbstractFilter<T>(actions, opt), child(child), steadyStateQuery(steadyStateQuery) {
		// Intentionally left empty.
	}

	virtual ~CslFilter() {
		this->actions.clear();
	}

	void check(storm::modelchecker::prctl::AbstractModelChecker<T> const & modelchecker) const {

		// Write out the formula to be checked.
		std::cout << std::endl;
		LOG4CPLUS_INFO(logger, "Model checking formula\t" << this->toString());
		std::cout << "Model checking formula:\t" << this->toString() << std::endl;

		Result result;

		try {
			if(std::dynamic_pointer_cast<AbstractStateFormula<T>>(child).get() != nullptr) {
				result = evaluate(modelchecker, std::dynamic_pointer_cast<AbstractStateFormula<T>>(child));
			} else if (std::dynamic_pointer_cast<AbstractPathFormula<T>>(child).get() != nullptr) {
				result = evaluate(modelchecker, std::dynamic_pointer_cast<AbstractPathFormula<T>>(child));
			}
		} catch (std::exception& e) {
			std::cout << "Error during computation: " << e.what() << "Skipping property." << std::endl;
			LOG4CPLUS_ERROR(logger, "Error during computation: " << e.what() << "Skipping property.");
			std::cout << std::endl << "-------------------------------------------" << std::endl;

			return;
		}

		writeOut(result, modelchecker);

	}

	virtual std::string toString() const override {
		std::string desc = "";

		if(!std::dynamic_pointer_cast<AbstractStateFormula<T>>(child)) {

			// The formula is not a state formula so we have a probability query.
			if(this->actions.empty()){

				// Since there are no actions given we do legacy support.

				desc += "P ";
				switch(this->opt) {
					case MINIMIZE:
						desc += "min ";
						break;
					case MAXIMIZE:
						desc += "max ";
						break;
					default:
						break;
				}
				desc += "= ? ";

			} else {
				desc = "filter[";

				switch(this->opt) {
					case MINIMIZE:
						desc += "min, ";
						break;
					case MAXIMIZE:
						desc += "max, ";
						break;
					default:
						break;
				}

				for(auto action : this->actions) {
					desc += action->toString();
					desc += ", ";
				}

				// Remove the last ", ".
				desc.pop_back();
				desc.pop_back();

				desc += "]";
			}

		} else {

			if(this->actions.empty()) {

				if(steadyStateQuery) {

					// Legacy support for the steady state query.
					desc += "S = ? ";

				} else {

					// There are no filter actions but only the raw state formula. So just print that.
					return child->toString();
				}
			} else {

				desc = "filter[";

				for(auto action : this->actions) {
					desc += action->toString();
					desc += ", ";
				}

				// Remove the last ", ".
				desc.pop_back();
				desc.pop_back();

				desc += "]";
			}
		}

		desc += "(";
		desc += child->toString();
		desc += ")";

		return desc;
	}

	virtual std::string toPrettyString() const override{
		std::string desc = "Filter: ";
		desc += "\nActions:";
		for(auto action : this->actions) {
			desc += "\n\t" + action->toString();
		}
		desc += "\nFormula:\n\t" + child->toString();
		return desc;
	}

	void setChild(std::shared_ptr<AbstractCslFormula<T>> const & child) {
		this->child = child;
	}

	std::shared_ptr<AbstractCslFormula<T>> const & getChild() const {
		return child;
	}

private:

	storm::storage::BitVector evaluate(storm::modelchecker::csl::AbstractModelChecker<T> const & modelchecker, std::shared_ptr<AbstractStateFormula<T>> const & formula) const {
		// First, get the model checking result.
		storm::storage::BitVector result = modelchecker.checkMinMaxOperator(*formula);

		if(this->opt != UNDEFINED) {
			// If there is an action specifying that min/max probabilities should be computed, call the appropriate method of the model checker.
			result = modelchecker.checkMinMaxOperator(formula, this->opt == MINIMIZE ? true : false);
		} else {
			result = formula->check(modelchecker);
		}


		// Now apply all filter actions and return the result.
		return evaluateActions(result, modelchecker);
	}

	std::vector<T> evaluate(storm::modelchecker::csl::AbstractModelChecker<T> const & modelchecker, std::shared_ptr<AbstractPathFormula<T>> const & formula) const {
		// First, get the model checking result.
		std::vector<T> result;

		if(this->opt != UNDEFINED) {
			// If there is an action specifying that min/max probabilities should be computed, call the appropriate method of the model checker.
			result = modelchecker.checkMinMaxOperator(*formula, this->opt == MINIMIZE ? true : false);
		} else {
			result = formula->check(modelchecker, false);
		}

		// Now apply all filter actions and return the result.
		return evaluateActions(result, modelchecker);
	}

	Result evaluateActions(Result result, storm::modelchecker::csl::AbstractModelChecker<T> const & modelchecker) const {

		// Init the state selection and state map vectors.
		result.selection = storm::storage::BitVector(result.stateResult.size(), true);
		result.stateMap = std::vector<uint_fast64_t>(result.selection.size());
		for(uint_fast64_t i = 0; i < result.selection.size(); i++) {
			result.stateMap[i] = i;
		}

		// Now apply all filter actions and return the result.
		for(auto action : this->actions) {
			result = action->evaluate(result, modelchecker);
		}
		return result;
	}

	void writeOut(Result const & result, storm::modelchecker::prctl::AbstractModelChecker<T> const & modelchecker) const {

		// Test for the kind of result. Values or states.
		if(!result.pathResult.empty()) {

			// Write out the selected value results in the order given by the stateMap.
			if(this->actions.empty()) {

				// There is no filter action given. So provide legacy support:
				// Return the results for all states labeled with "init".
				LOG4CPLUS_INFO(logger, "Result for initial states:");
				std::cout << "Result for initial states:" << std::endl;
				for (auto initialState : modelchecker.template getModel<storm::models::AbstractModel<T>>().getInitialStates()) {
					LOG4CPLUS_INFO(logger, "\t" << initialState << ": " << result.pathResult[initialState]);
					std::cout << "\t" << initialState << ": " << result.pathResult[initialState] << std::endl;
				}
			} else {
				LOG4CPLUS_INFO(logger, "Result for " << result.selection.getNumberOfSetBits() << " selected states:");
				std::cout << "Result for " << result.selection.getNumberOfSetBits() << " selected states:" << std::endl;

				for(uint_fast64_t i = 0; i < result.stateMap.size(); i++) {
					if(result.selection.get(result.stateMap[i])) {
						LOG4CPLUS_INFO(logger, "\t" << result.stateMap[i] << ": " << result.pathResult[result.stateMap[i]]);
						std::cout << "\t" << result.stateMap[i] << ": " << result.pathResult[result.stateMap[i]] << std::endl;
					}
				}
			}

		} else {

			// Write out the selected state results in the order given by the stateMap.
			if(this->actions.empty()) {

				// There is no filter action given. So provide legacy support:
				// Return the results for all states labeled with "init".
				LOG4CPLUS_INFO(logger, "Result for initial states:");
				std::cout << "Result for initial states:" << std::endl;
				for (auto initialState : modelchecker.template getModel<storm::models::AbstractModel<T>>().getInitialStates()) {
					LOG4CPLUS_INFO(logger, "\t" << initialState << ": " << (result.stateResult[initialState] ? "satisfied" : "not satisfied"));
					std::cout << "\t" << initialState << ": " << result.stateResult[initialState] << std::endl;
				}
			} else {
				LOG4CPLUS_INFO(logger, "Result for " << result.selection.getNumberOfSetBits() << " selected states:");
				std::cout << "Result for " << result.selection.getNumberOfSetBits() << " selected states:" << std::endl;

				for(uint_fast64_t i = 0; i < result.stateMap.size(); i++) {
					if(result.selection.get(result.stateMap[i])) {
						LOG4CPLUS_INFO(logger, "\t" << result.stateMap[i] << ": " << (result.stateResult[result.stateMap[i]] ? "satisfied" : "not satisfied"));
						std::cout << "\t" << result.stateMap[i] << ": " << (result.stateResult[result.stateMap[i]] ? "satisfied" : "not satisfied") << std::endl;
					}
				}
			}
		}

		std::cout << std::endl << "-------------------------------------------" << std::endl;
	}

	std::shared_ptr<AbstractCslFormula<T>> child;

	bool steadyStateQuery;
};

} //namespace csl
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_CSL_CSLFILTER_H_ */
