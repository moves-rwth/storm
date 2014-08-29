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
namespace csl {

/*!
 * This is the Csl specific filter.
 *
 * It maintains a Csl formula which can be checked against a given model by either calling evaluate() or check().
 * Additionally it maintains a list of filter actions that are used to further manipulate the modelchecking results and prepare them for output.
 */
template <class T>
class CslFilter : public storm::property::AbstractFilter<T> {

	// Convenience typedef to make the code more readable.
	typedef typename storm::property::action::AbstractAction<T>::Result Result;

public:

	/*!
	 * Creates an empty CslFilter, maintaining no Csl formula.
	 *
	 * Calling check or evaluate will return an empty result.
	 */
	CslFilter() : AbstractFilter<T>(UNDEFINED), child(nullptr), steadyStateQuery(false) {
		// Intentionally left empty.
	}

	/*!
	 * Creates a CslFilter maintaining a Csl formula but containing no actions.
	 *
	 * The modelchecking result will be returned or printed as is.
	 *
	 * @param child The Csl formula to be maintained.
	 * @param opt An enum value indicating which kind of scheduler shall be used for path formulas on nondeterministic models.
	 * @param steadyStateQuery A flag indicating whether this is a steady state query.
	 */
	CslFilter(std::shared_ptr<AbstractCslFormula<T>> const & child, OptimizingOperator opt = UNDEFINED, bool steadyStateQuery = false) : AbstractFilter<T>(opt), child(child), steadyStateQuery(steadyStateQuery) {
		// Intentionally left empty.
	}

	/*!
	 * Creates a CslFilter maintaining a Csl formula and containing a single action.
	 *
	 * The given action will be applied to the modelchecking result during evaluation.
	 * Further actions can be added later.
	 *
	 * @param child The Csl formula to be maintained.
	 * @param action The single action to be executed during evaluation.
	 * @param opt An enum value indicating which kind of scheduler shall be used for path formulas on nondeterministic models.
	 * @param steadyStateQuery A flag indicating whether this is a steady state query.
	 */
	CslFilter(std::shared_ptr<AbstractCslFormula<T>> const & child, std::shared_ptr<action::AbstractAction<T>> const & action, OptimizingOperator opt = UNDEFINED, bool steadyStateQuery = false) : AbstractFilter<T>(action, opt), child(child), steadyStateQuery(steadyStateQuery) {
		// Intentionally left empty
	}

	/*!
	 * Creates a CslFilter using the given parameters.
	 *
	 * The given actions will be applied to the modelchecking result in ascending index order during evaluation.
	 * Further actions can be added later.
	 *
	 * @param child The Csl formula to be maintained.
	 * @param actions A vector conatining the actions that are to be executed during evaluation.
	 * @param opt An enum value indicating which kind of scheduler shall be used for path formulas on nondeterministic models.
	 * @param steadyStateQuery A flag indicating whether this is a steady state query.
	 */
	CslFilter(std::shared_ptr<AbstractCslFormula<T>> const & child, std::vector<std::shared_ptr<action::AbstractAction<T>>> const & actions, OptimizingOperator opt = UNDEFINED, bool steadyStateQuery = false) : AbstractFilter<T>(actions, opt), child(child), steadyStateQuery(steadyStateQuery) {
		// Intentionally left empty.
	}

	/*!
	 * Empty virtual destructor.
	 */
	virtual ~CslFilter() {
		// Intentionally left empty.
	}

	/*!
	 * Calls the modelchecker, retrieves the modelchecking result, applies the filter action one by one and prints out the result.
	 *
	 * @param modelchecker The modelchecker to be called.
	 */
	void check(storm::modelchecker::csl::AbstractModelChecker<T> const & modelchecker) const {

		// Write out the formula to be checked.
		std::cout << std::endl;
		LOG4CPLUS_INFO(logger, "Model checking formula\t" << this->toString());
		std::cout << "Model checking formula:\t" << this->toString() << std::endl;

		writeOut(evaluate(modelchecker), modelchecker);

	}

	/*!
	 * Calls the modelchecker, retrieves the modelchecking result, applies the filter action one by one and returns the result.
	 *
	 * @param modelchecker The modelchecker to be called.
	 * @returns The result of the sequential application of the filter actions to the modelchecking result.
	 */
	Result evaluate(storm::modelchecker::csl::AbstractModelChecker<T> const & modelchecker) const {
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
		}

		return result;
	}

	/*!
	 * Returns a textual representation of the filter.
	 *
	 * That includes the actions as well as the maintained formula.
	 *
	 * @returns A string representing the filter.
	 */
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
						desc += "min; ";
						break;
					case MAXIMIZE:
						desc += "max; ";
						break;
					default:
						break;
				}

				for(auto action : this->actions) {
					desc += action->toString();
					desc += "; ";
				}

				// Remove the last "; ".
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
					desc += "; ";
				}

				// Remove the last "; ".
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

	/*!
	 * Gets the child node.
	 *
	 * @returns The child node.
	 */
	std::shared_ptr<AbstractCslFormula<T>> const & getChild() const {
		return child;
	}

	/*!
	 * Sets the subtree.
	 *
	 * @param child The new child.
	 */
	void setChild(std::shared_ptr<AbstractCslFormula<T>> const & child) {
		this->child = child;
	}

	/*!
	 * Checks if the child is set, i.e. it does not point to null.
	 *
	 * @return True iff the child is set.
	 */
	bool isChildSet() const {
		return child.get() != nullptr;
	}

private:

	/*!
	 * Calls the modelchecker for a state formula, retrieves the modelchecking result, applies the filter action one by one and returns the result.
	 *
	 * This an internal version of the evaluate method overloading it for the different Csl formula types.
	 *
	 * @param modelchecker The modelchecker to be called.
	 * @param formula The state formula for which the modelchecker will be called.
	 * @returns The result of the sequential application of the filter actions to the modelchecking result.
	 */
	Result evaluate(storm::modelchecker::csl::AbstractModelChecker<T> const & modelchecker, std::shared_ptr<AbstractStateFormula<T>> const & formula) const {
		// First, get the model checking result.
		Result result;

		//TODO: Once a modelchecker supporting steady state formulas is implemented, call it here in case steadyStateQuery is set.

		if(this->opt != UNDEFINED) {
			// If there is an action specifying that min/max probabilities should be computed, call the appropriate method of the model checker.
			result.stateResult = modelchecker.checkMinMaxOperator(*formula, this->opt == MINIMIZE ? true : false);
		} else {
			result.stateResult = formula->check(modelchecker);
		}


		// Now apply all filter actions and return the result.
		return evaluateActions(result, modelchecker);
	}

	/*!
	 * Calls the modelchecker for a path formula, retrieves the modelchecking result, applies the filter action one by one and returns the result.
	 *
	 * This an internal version of the evaluate method overloading it for the different Csl formula types.
	 *
	 * @param modelchecker The modelchecker to be called.
	 * @param formula The path formula for which the modelchecker will be called.
	 * @returns The result of the sequential application of the filter actions to the modelchecking result.
	 */
	Result evaluate(storm::modelchecker::csl::AbstractModelChecker<T> const & modelchecker, std::shared_ptr<AbstractPathFormula<T>> const & formula) const {
		// First, get the model checking result.
		Result result;

		if(this->opt != UNDEFINED) {
			// If there is an action specifying that min/max probabilities should be computed, call the appropriate method of the model checker.
			result.pathResult = modelchecker.checkMinMaxOperator(*formula, this->opt == MINIMIZE ? true : false);
		} else {
			result.pathResult = formula->check(modelchecker, false);
		}

		// Now apply all filter actions and return the result.
		return evaluateActions(result, modelchecker);
	}

	/*!
	 * Evaluates the filter actions by calling them one by one using the output of each action as the input for the next one.
	 *
	 * @param input The modelchecking result in form of a Result struct.
	 * @param modelchecker The modelchecker that was called to generate the modelchecking result. Needed by some actions.
	 * @returns The result of the sequential application of the filter actions to the modelchecking result.
	 */
	Result evaluateActions(Result result, storm::modelchecker::csl::AbstractModelChecker<T> const & modelchecker) const {

		// Init the state selection and state map vectors.
		uint_fast64_t size = result.stateResult.size() == 0 ? result.pathResult.size() : result.stateResult.size();
		result.selection = storm::storage::BitVector(size, true);
		result.stateMap = std::vector<uint_fast64_t>(size);
		for(uint_fast64_t i = 0; i < result.selection.size(); i++) {
			result.stateMap[i] = i;
		}

		// Now apply all filter actions and return the result.
		for(auto action : this->actions) {
			result = action->evaluate(result, modelchecker);
		}
		return result;
	}

	/*!
	 * Writes out the given result.
	 *
	 * @param result The result of the sequential application of the filter actions to a modelchecking result.
	 * @param modelchecker The modelchecker that was called to generate the modelchecking result. Needed for legacy support.
	 */
	void writeOut(Result const & result, storm::modelchecker::csl::AbstractModelChecker<T> const & modelchecker) const {

		// Test if there is anything to write out.
		// The selection size should only be 0 if an error occurred during the evaluation (since a model with 0 states is invalid).
		if(result.selection.size() == 0) {
			std::cout << std::endl << "-------------------------------------------" << std::endl;
			return;
		}

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

	// The Csl formula maintained by this filter.
	std::shared_ptr<AbstractCslFormula<T>> child;

	// A flag indicating whether this is a steady state query.
	bool steadyStateQuery;
};

} //namespace csl
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_CSL_CSLFILTER_H_ */
