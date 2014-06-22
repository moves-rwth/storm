/*
 * CslFilter.h
 *
 *  Created on: May 7, 2014
 *      Author: Manuel Sascha Weiand
 */

#ifndef STORM_FORMULA_PRCTL_CSLFILTER_H_
#define STORM_FORMULA_PRCTL_CSLFILTER_H_

#include "src/formula/AbstractFilter.h"
#include "src/formula/Csl/AbstractCslFormula.h"
#include "src/formula/Csl/AbstractPathFormula.h"
#include "src/formula/Csl/AbstractStateFormula.h"
#include "src/modelchecker/csl/AbstractModelChecker.h"

namespace storm {
namespace property {
namespace csl {

template <class T>
class CslFilter : public storm::property::AbstractFilter<T> {

public:

	CslFilter() : AbstractFilter<T>(UNDEFINED), child(nullptr), steadyStateQuery(false) {
		// Intentionally left empty.
	}

	CslFilter(AbstractCslFormula<T>* child, OptimizingOperator opt = UNDEFINED, bool steadyStateQuery = false) : AbstractFilter<T>(opt), child(child), steadyStateQuery(steadyStateQuery) {
		// Intentionally left empty.
	}

	CslFilter(AbstractCslFormula<T>* child, action::AbstractAction<T>* action, OptimizingOperator opt = UNDEFINED, bool steadyStateQuery = false) : AbstractFilter<T>(action, opt), child(child), steadyStateQuery(steadyStateQuery) {
		// Intentionally left empty
	}

	CslFilter(AbstractCslFormula<T>* child, std::vector<action::AbstractAction<T>*> actions, OptimizingOperator opt = UNDEFINED, bool steadyStateQuery = false) : AbstractFilter<T>(actions, opt), child(child), steadyStateQuery(steadyStateQuery) {
		// Intentionally left empty.
	}

	virtual ~CslFilter() {
		this->actions.clear();
		delete child;
	}

	void check(storm::modelchecker::csl::AbstractModelChecker<T> const & modelchecker) const {

		// Write out the formula to be checked.
		std::cout << std::endl;
		LOG4CPLUS_INFO(logger, "Model checking formula\t" << this->toFormulaString());
		std::cout << "Model checking formula:\t" << this->toFormulaString() << std::endl;

		// Do a dynamic cast to test for the actual formula type and call the correct evaluation function.
		if(dynamic_cast<AbstractStateFormula<T>*>(child) != nullptr) {

			// Check the formula and apply the filter actions.
			storm::storage::BitVector result;

			try {
				result = evaluate(modelchecker, static_cast<AbstractStateFormula<T>*>(child));
			} catch (std::exception& e) {
				std::cout << "Error during computation: " << e.what() << "Skipping property." << std::endl;
				LOG4CPLUS_ERROR(logger, "Error during computation: " << e.what() << "Skipping property.");
				std::cout << std::endl << "-------------------------------------------" << std::endl;

				return;
			}

			// Now write out the result.

			if(this->actions.empty()) {

				// There is no filter action given. So provide legacy support:
				// Return the results for all states labeled with "init".
				LOG4CPLUS_INFO(logger, "Result for initial states:");
				std::cout << "Result for initial states:" << std::endl;
				for (auto initialState : modelchecker.getModel().getInitialStates()) {
					LOG4CPLUS_INFO(logger, "\t" << initialState << ": " << (result.get(initialState) ? "satisfied" : "not satisfied"));
					std::cout << "\t" << initialState << ": " << result.get(initialState) << std::endl;
				}
			}

			std::cout << std::endl << "-------------------------------------------" << std::endl;

		}
		else if (dynamic_cast<AbstractPathFormula<T>*>(child) != nullptr) {

			// Check the formula and apply the filter actions.
			std::vector<T> result;

			try {
				result = evaluate(modelchecker, static_cast<AbstractPathFormula<T>*>(child));
			} catch (std::exception& e) {
				std::cout << "Error during computation: " << e.what() << "Skipping property." << std::endl;
				LOG4CPLUS_ERROR(logger, "Error during computation: " << e.what() << "Skipping property.");
				std::cout << std::endl << "-------------------------------------------" << std::endl;

				return;
			}

			// Now write out the result.

			if(this->actions.empty()) {

				// There is no filter action given. So provide legacy support:
				// Return the results for all states labeled with "init".
				LOG4CPLUS_INFO(logger, "Result for initial states:");
				std::cout << "Result for initial states:" << std::endl;
				for (auto initialState : modelchecker.getModel().getInitialStates()) {
					LOG4CPLUS_INFO(logger, "\t" << initialState << ": " << result[initialState]);
					std::cout << "\t" << initialState << ": " << result[initialState] << std::endl;
				}
			}

			std::cout << std::endl << "-------------------------------------------" << std::endl;

		}
		else {
			// This branch should be unreachable. If you ended up here, something strange has happened.
			//TODO: Error here.
		}
	}

	virtual std::string toString() const override {
		std::string desc = "";

		if(dynamic_cast<AbstractStateFormula<T>*>(child) == nullptr) {

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

	void setChild(AbstractCslFormula<T>* child) {
		this->child = child;
	}

	AbstractCslFormula<T>* getChild() const {
		return child;
	}

private:

	storm::storage::BitVector evaluate(storm::modelchecker::csl::AbstractModelChecker<T> const & modelchecker, AbstractStateFormula<T>* formula) const {
		// First, get the model checking result.
		storm::storage::BitVector result = modelchecker.checkMinMaxOperator(formula);

		if(this->opt != UNDEFINED) {
			// If there is an action specifying that min/max probabilities should be computed, call the appropriate method of the model checker.
			result = modelchecker.checkMinMaxOperator(formula, this->opt == MINIMIZE ? true : false);
		} else {
			result = formula->check(modelchecker);
		}


		// Now apply all filter actions and return the result.
		for(auto action : this->actions) {
			result = action->evaluate(result);
		}
		return result;
	}

	std::vector<T> evaluate(storm::modelchecker::csl::AbstractModelChecker<T> const & modelchecker, AbstractPathFormula<T>* formula) const {
		// First, get the model checking result.
		std::vector<T> result;

		if(this->opt != UNDEFINED) {
			// If there is an action specifying that min/max probabilities should be computed, call the appropriate method of the model checker.
			result = modelchecker.checkMinMaxOperator(formula, this->opt == MINIMIZE ? true : false);
		} else {
			result = formula->check(modelchecker, false);
		}

		// Now apply all filter actions and return the result.
		for(auto action : this->actions) {
			result = action->evaluate(result);
		}
		return result;
	}

	AbstractCslFormula<T>* child;

	bool steadyStateQuery;
};

} //namespace csl
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_CSL_CSLFILTER_H_ */
