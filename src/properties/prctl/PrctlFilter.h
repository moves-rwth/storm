#ifndef STORM_PROPERTIES_PRCTL_PRCTLFILTER_H_
#define STORM_PROPERTIES_PRCTL_PRCTLFILTER_H_

#include "src/properties/AbstractFilter.h"
#include "src/properties/prctl/AbstractPrctlFormula.h"
#include "src/properties/prctl/AbstractPathFormula.h"
#include "src/properties/prctl/AbstractStateFormula.h"
#include "src/modelchecker/prctl/AbstractModelChecker.h"
#include "src/properties/actions/AbstractAction.h"

#include <algorithm>
#include <memory>

namespace storm {
	namespace properties {
		namespace prctl {

			/*!
			 * This is the Prctl specific filter.
			 *
			 * It maintains a Prctl formula which can be checked against a given model by either calling evaluate() or check().
			 * Additionally it maintains a list of filter actions that are used to further manipulate the modelchecking results and prepare them for output.
			 */
			template <class T>
			class PrctlFilter : public storm::properties::AbstractFilter<T> {

				// Convenience typedef to make the code more readable.
				typedef typename storm::properties::action::AbstractAction<T>::Result Result;

			public:

				/*!
				 * Creates an empty PrctlFilter, maintaining no Prctl formula.
				 *
				 * Calling check or evaluate will return an empty result.
				 */
				PrctlFilter() : AbstractFilter<T>(UNDEFINED), child(nullptr) {
					// Intentionally left empty.
				}

				/*!
				 * Creates a PrctlFilter maintaining a Prctl formula but containing no actions.
				 *
				 * The modelchecking result will be returned or printed as is.
				 *
				 * @param child The Prctl formula to be maintained.
				 * @param opt An enum value indicating which kind of scheduler shall be used for path formulas on nondeterministic models.
				 */
				PrctlFilter(std::shared_ptr<AbstractPrctlFormula<T>> const & child, OptimizingOperator opt = UNDEFINED) : AbstractFilter<T>(opt), child(child) {
					// Intentionally left empty.
				}

				/*!
				 * Creates a PrctlFilter maintaining a Prctl formula and containing a single action.
				 *
				 * The given action will be applied to the modelchecking result during evaluation.
				 * Further actions can be added later.
				 *
				 * @param child The Prctl formula to be maintained.
				 * @param action The single action to be executed during evaluation.
				 * @param opt An enum value indicating which kind of scheduler shall be used for path formulas on nondeterministic models.
				 */
				PrctlFilter(std::shared_ptr<AbstractPrctlFormula<T>> const & child, std::shared_ptr<action::AbstractAction<T>> const & action, OptimizingOperator opt = UNDEFINED) : AbstractFilter<T>(action, opt), child(child) {
					// Intentionally left empty.
				}

				/*!
				 * Creates a PrctlFilter using the given parameters.
				 *
				 * The given actions will be applied to the modelchecking result in ascending index order during evaluation.
				 * Further actions can be added later.
				 *
				 * @param child The Prctl formula to be maintained.
				 * @param actions A vector conatining the actions that are to be executed during evaluation.
				 * @param opt An enum value indicating which kind of scheduler shall be used for path formulas on nondeterministic models.
				 */
				PrctlFilter(std::shared_ptr<AbstractPrctlFormula<T>> const & child, std::vector<std::shared_ptr<action::AbstractAction<T>>> const & actions, OptimizingOperator opt = UNDEFINED) : AbstractFilter<T>(actions, opt), child(child) {
					// Intentionally left empty.
				}

				/*!
				 * Empty virtual destructor.
				 */
				virtual ~PrctlFilter() {
					// Intentionally left empty.
				}

				/*!
				 * Calls the modelchecker, retrieves the modelchecking result, applies the filter action one by one and prints out the result.
				 *
				 * @param modelchecker The modelchecker to be called.
				 */
				void check(storm::modelchecker::prctl::AbstractModelChecker<T> const & modelchecker) const {

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
				Result evaluate(storm::modelchecker::prctl::AbstractModelChecker<T> const & modelchecker) const {

					Result result;

					try {
						if(dynamic_cast<AbstractStateFormula<T> *>(child.get()) != nullptr) {
							result = evaluate(modelchecker, std::dynamic_pointer_cast<AbstractStateFormula<T>>(child));
						} else if (dynamic_cast<AbstractPathFormula<T> *>(child.get()) != nullptr) {
							result = evaluate(modelchecker, std::dynamic_pointer_cast<AbstractPathFormula<T>>(child));
						} else if (dynamic_cast<AbstractRewardPathFormula<T> *>(child.get()) != nullptr) {
							result = evaluate(modelchecker, std::dynamic_pointer_cast<AbstractRewardPathFormula<T>>(child));
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

						// The formula is not a state formula so we either have a probability query or a reward query.
						if(this->actions.empty()){

							// There is exactly one action in the list, the minmax action. Again, we do legacy support-

							if(std::dynamic_pointer_cast<AbstractPathFormula<T>>(child)) {
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
							// There are no filter actions but only the raw state formula. So just print that.
							return child->toString();
						}

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
				std::shared_ptr<AbstractPrctlFormula<T>> const & getChild() const {
					return child;
				}

				/*!
				 * Sets the subtree.
				 *
				 * @param child The new child.
				 */
				void setChild(std::shared_ptr<AbstractPrctlFormula<T>> const & child) {
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
				 * This an internal version of the evaluate method overloading it for the different Prctl formula types.
				 *
				 * @param modelchecker The modelchecker to be called.
				 * @param formula The state formula for which the modelchecker will be called.
				 * @returns The result of the sequential application of the filter actions to the modelchecking result.
				 */
				Result evaluate(storm::modelchecker::prctl::AbstractModelChecker<T> const & modelchecker, std::shared_ptr<AbstractStateFormula<T>> const & formula) const {
					// First, get the model checking result.
					Result result;

					if(this->opt != UNDEFINED) {
						// If it is specified that min/max probabilities/rewards should be computed, call the appropriate method of the model checker.
						result.stateResult = modelchecker.checkOptimizingOperator(*formula, this->opt == storm::properties::MINIMIZE ? true : false);
					} else {
						result.stateResult = formula->check(modelchecker);
					}

					// Now apply all filter actions and return the result.
					return evaluateActions(result, modelchecker);
				}

				/*!
				 * Calls the modelchecker for a path formula, retrieves the modelchecking result, applies the filter action one by one and returns the result.
				 *
				 * This an internal version of the evaluate method overloading it for the different Prctl formula types.
				 *
				 * @param modelchecker The modelchecker to be called.
				 * @param formula The path formula for which the modelchecker will be called.
				 * @returns The result of the sequential application of the filter actions to the modelchecking result.
				 */
				Result evaluate(storm::modelchecker::prctl::AbstractModelChecker<T> const & modelchecker, std::shared_ptr<AbstractPathFormula<T>> const & formula) const {
					// First, get the model checking result.
					Result result;

					if(this->opt != UNDEFINED) {
						// If it is specified that min/max probabilities/rewards should be computed, call the appropriate method of the model checker.
						result.pathResult = modelchecker.checkOptimizingOperator(*formula, this->opt == storm::properties::MINIMIZE ? true : false);
					} else {
						result.pathResult = formula->check(modelchecker, false);
					}

					// Now apply all filter actions and return the result.
					return evaluateActions(result, modelchecker);
				}

				/*!
				 * Calls the modelchecker for a reward formula, retrieves the modelchecking result, applies the filter action one by one and returns the result.
				 *
				 * This an internal version of the evaluate method overloading it for the different Prctl formula types.
				 *
				 * @param modelchecker The modelchecker to be called.
				 * @param formula The reward formula for which the modelchecker will be called.
				 * @returns The result of the sequential application of the filter actions to the modelchecking result.
				 */
				Result evaluate(storm::modelchecker::prctl::AbstractModelChecker<T> const & modelchecker, std::shared_ptr<AbstractRewardPathFormula<T>> const & formula) const {
					// First, get the model checking result.
					Result result;

					if(this->opt != UNDEFINED) {
						// If it is specified that min/max probabilities/rewards should be computed, call the appropriate method of the model checker.
						result.pathResult = modelchecker.checkOptimizingOperator(*formula, this->opt == storm::properties::MINIMIZE ? true : false);
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
				Result evaluateActions(Result const & input, storm::modelchecker::prctl::AbstractModelChecker<T> const & modelchecker) const {

					// Init the state selection and state map vectors.
					Result result = input;
					uint_fast64_t size = result.stateResult.size() == 0 ? result.pathResult.size() : result.stateResult.size();
					result.selection = storm::storage::BitVector(size, true);
					result.stateMap = std::vector<uint_fast64_t>(size);
					for(uint_fast64_t i = 0; i < size; i++) {
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
				void writeOut(Result const & result, storm::modelchecker::prctl::AbstractModelChecker<T> const & modelchecker) const {

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

				// The Prctl formula maintained by this filter.
				std::shared_ptr<AbstractPrctlFormula<T>> child;
			};

		} // namespace prctl
	} // namespace properties
} // namespace storm

#endif /* STORM_PROPERTIES_PRCTL_PRCTLFILTER_H_ */
