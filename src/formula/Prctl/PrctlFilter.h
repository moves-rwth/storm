/*
 * PrctlFilter.h
 *
 *  Created on: Apr 26, 2014
 *      Author: Manuel Sascha Weiand
 */

#ifndef STORM_FORMULA_PRCTL_PRCTLFILTER_H_
#define STORM_FORMULA_PRCTL_PRCTLFILTER_H_

#include "src/formula/AbstractFilter.h"
#include "src/formula/Prctl/AbstractPrctlFormula.h"
#include "src/formula/Prctl/AbstractPathFormula.h"
#include "src/formula/Prctl/AbstractStateFormula.h"
#include "src/modelchecker/prctl/AbstractModelChecker.h"

namespace storm {
namespace property {
namespace prctl {

template <class T>
class PrctlFilter : public storm::property::AbstractFilter<T> {

public:

	PrctlFilter() : AbstractFilter<T>(UNDEFINED), child(nullptr) {
		// Intentionally left empty.
	}

	PrctlFilter(AbstractPrctlFormula<T>* child, OptimizingOperator opt = UNDEFINED) : AbstractFilter<T>(opt), child(child) {
		// Intentionally left empty.
	}

	PrctlFilter(AbstractPrctlFormula<T>* child, action::AbstractAction<T>* action, OptimizingOperator opt = UNDEFINED) : AbstractFilter<T>(action, opt), child(child) {
		// Intentionally left empty.
	}

	PrctlFilter(AbstractPrctlFormula<T>* child, std::vector<action::AbstractAction<T>*> actions, OptimizingOperator opt = UNDEFINED) : AbstractFilter<T>(actions, opt), child(child) {
		// Intentionally left empty.
	}

	virtual ~PrctlFilter() {
		this->actions.clear();
		delete child;
	}

	void check(storm::modelchecker::prctl::AbstractModelChecker<T> const & modelchecker) const {

		// Write out the formula to be checked.
		std::cout << std::endl;
		LOG4CPLUS_INFO(logger, "Model checking formula\t" << this->toString());
		std::cout << "Model checking formula:\t" << this->toString() << std::endl;

		// Do a dynamic cast to test for the actual formula type and call the correct evaluation function.
		if(dynamic_cast<AbstractStateFormula<T>*>(child) != nullptr) {

			// Check the formula and apply the filter actions.
			storm::storage::BitVector result;

			try {
				result = evaluate(modelchecker, dynamic_cast<AbstractStateFormula<T>*>(child));
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
				for (auto initialState : modelchecker.template getModel<storm::models::AbstractModel<T>>().getInitialStates()) {
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
				result = evaluate(modelchecker, dynamic_cast<AbstractPathFormula<T>*>(child));
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
				for (auto initialState : modelchecker.template getModel<storm::models::AbstractModel<T>>().getInitialStates()) {
					LOG4CPLUS_INFO(logger, "\t" << initialState << ": " << result[initialState]);
					std::cout << "\t" << initialState << ": " << result[initialState] << std::endl;
				}
			}

			std::cout << std::endl << "-------------------------------------------" << std::endl;

		}
		else if (dynamic_cast<AbstractRewardPathFormula<T>*>(child) != nullptr) {

			// Check the formula and apply the filter actions.
			std::vector<T> result;

			try {
				result = evaluate(modelchecker, dynamic_cast<AbstractRewardPathFormula<T>*>(child));
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
				for (auto initialState : modelchecker.template getModel<storm::models::AbstractModel<T>>().getInitialStates()) {
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

	bool validate() const {
		// Test whether the stored filter actions are consistent in relation to themselves and to the ingoing modelchecking result.

		// Do a dynamic cast to test for the actual formula type.
		if(dynamic_cast<AbstractStateFormula<T>*>(child) != nullptr) {
			//TODO: Actual validation.
		}
		else if (dynamic_cast<AbstractPathFormula<T>*>(child) != nullptr) {
			//TODO: Actual validation.
		}
		else {
			// This branch should be unreachable. If you ended up here, something strange has happened.
			//TODO: Error here.
		}

		return true;
	}

	virtual std::string toString() const override {
		std::string desc = "";

		if(dynamic_cast<AbstractStateFormula<T>*>(child) == nullptr) {

			// The formula is not a state formula so we either have a probability query or a reward query.
			if(this->actions.empty()){

				// There is exactly one action in the list, the minmax action. Again, we do legacy support-

				if(dynamic_cast<AbstractPathFormula<T>*>(child) != nullptr) {
					// It is a probability query.
					desc += "P ";

				} else {
					// It is a reward query.
					desc += "R ";
				}

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
						desc += " min, ";
						break;
					case MAXIMIZE:
						desc += " max, ";
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

				// There are no filter actions but only the raw state formula. So just print that.
				return child->toString();
			}

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

	void setChild(AbstractPrctlFormula<T>* child) {
		this->child = child;
	}

	AbstractPrctlFormula<T>* getChild() const {
		return child;
	}

private:

	storm::storage::BitVector evaluate(storm::modelchecker::prctl::AbstractModelChecker<T> const & modelchecker, AbstractStateFormula<T>* formula) const {
		// First, get the model checking result.
		storm::storage::BitVector result;

		if(this->opt != UNDEFINED) {
			// If it is specified that min/max probabilities/rewards should be computed, call the appropriate method of the model checker.
			result = modelchecker.checkOptimizingOperator(*formula, this->opt == storm::property::MINIMIZE ? true : false);
		} else {
			result = formula->check(modelchecker);
		}


		// Now apply all filter actions and return the result.
		for(auto action : this->actions) {
			result = action->evaluate(result);
		}
		return result;
	}

	std::vector<T> evaluate(storm::modelchecker::prctl::AbstractModelChecker<T> const & modelchecker, AbstractPathFormula<T>* formula) const {
		// First, get the model checking result.
		std::vector<T> result;

		if(this->opt != UNDEFINED) {
			// If it is specified that min/max probabilities/rewards should be computed, call the appropriate method of the model checker.
			result = modelchecker.checkOptimizingOperator(*formula, this->opt == storm::property::MINIMIZE ? true : false);
		} else {
			result = formula->check(modelchecker, false);
		}

		// Now apply all filter actions and return the result.
		for(auto action : this->actions) {
			result = action->evaluate(result);
		}
		return result;
	}

	std::vector<T> evaluate(storm::modelchecker::prctl::AbstractModelChecker<T> const & modelchecker, AbstractRewardPathFormula<T>* formula) const {
		// First, get the model checking result.
		std::vector<T> result;

		if(this->opt != UNDEFINED) {
			// If it is specified that min/max probabilities/rewards should be computed, call the appropriate method of the model checker.
			result = modelchecker.checkOptimizingOperator(*formula, this->opt == storm::property::MINIMIZE ? true : false);
		} else {
			result = formula->check(modelchecker, false);
		}

		// Now apply all filter actions and return the result.
		for(auto action : this->actions) {
			result = action->evaluate(result);
		}
		return result;
	}

	AbstractPrctlFormula<T>* child;
};

} //namespace prctl
} //namespace property
} //namespace storm



#endif /* STORM_FORMULA_PRCTL_PRCTLFILTER_H_ */
