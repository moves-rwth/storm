#ifndef STORM_PROPERTIES_PRCTL_INSTANTANEOUSREWARD_H_
#define STORM_PROPERTIES_PRCTL_INSTANTANEOUSREWARD_H_

#include "AbstractRewardPathFormula.h"
#include <cstdint>
#include <string>

namespace storm {
	namespace properties {
		namespace prctl {

			// Forward declaration for the interface class.
			template <class T> class InstantaneousReward;

			/*!
			 * Interface class for model checkers that support InstantaneousReward.
			 *
			 * All model checkers that support the formula class InstantaneousReward must inherit
			 * this pure virtual class.
			 */
			template <class T>
			class IInstantaneousRewardModelChecker {
				public:

					/*!
					 * Empty virtual destructor.
					 */
					virtual ~IInstantaneousRewardModelChecker() {
						// Intentionally left empty
					}

					/*!
					 * Evaluates an InstantaneousReward formula within a model checker.
					 *
					 * @param obj Formula object with subformulas.
					 * @param qualitative A flag indicating whether the formula only needs to be evaluated qualitatively, i.e. if the
					 *                    results are only compared against the bounds 0 and 1.
					 * @return The modelchecking result of the formula for every state.
					 */
					virtual std::vector<T> checkInstantaneousReward(InstantaneousReward<T> const & obj, bool qualitative) const = 0;
			};

			/*!
			 * Class for an Instantaneous Reward formula.
			 * This class represents a possible leaf in a reward formula tree.
			 *
			 * Given a path of finite length.
			 * The reward received upon entering the last state of the path is the instantaneous reward of the path.
			 * The instantaneous reward for a state s at time \e bound is the expected instantaneous reward of a path of length \e bound starting in s.
			 *
			 * @see AbstractPathFormula
			 * @see AbstractPrctlFormula
			 */
			template <class T>
			class InstantaneousReward : public AbstractRewardPathFormula<T> {

			public:

				/*!
				 * Creates an InstantaneousReward node with the given bound.
				 *
				 * If no bound is given it defaults to 0, referencing the state reward received upon entering the state s itself.
				 *
				 * @param bound The time instance of the reward formula.
				 */
				InstantaneousReward(uint_fast64_t bound = 0) : bound(bound) {
					// Intentionally left empty.
				}

				/*!
				 * Empty virtual destructor.
				 */
				virtual ~InstantaneousReward() {
					// Intentionally left empty.
				}

				/*!
				 * Clones the called object.
				 *
				 * Performs a "deep copy", i.e. the subnodes of the new object are clones of the original ones.
				 *
				 * @returns A new InstantaneousReward object that is a deep copy of the called object.
				 */
				virtual std::shared_ptr<AbstractRewardPathFormula<T>> clone() const override {
					auto result = std::make_shared<InstantaneousReward<T>>(bound);
					return result;
				}


				/*!
				 * Calls the model checker to check this formula.
				 * Needed to infer the correct type of formula class.
				 *
				 * @note This function should only be called in a generic check function of a model checker class. For other uses,
				 *       the methods of the model checker should be used.
				 *
				 * @returns A vector indicating the probability that the formula holds for each state.
				 */
				virtual std::vector<T> check(storm::modelchecker::prctl::AbstractModelChecker<T> const & modelChecker, bool qualitative) const override {
					return modelChecker.template as<IInstantaneousRewardModelChecker>()->checkInstantaneousReward(*this, qualitative);
				}

				/*!
				 * Returns a textual representation of the formula tree with this node as root.
				 *
				 * @returns A string representing the formula tree.
				 */
				virtual std::string toString() const override {
					std::string result = "I=";
					result += std::to_string(bound);
					return result;
				}

				/*!
				 * Gets the time instance for the instantaneous reward operator.
				 *
				 * @returns The time instance for the instantaneous reward operator.
				 */
				uint_fast64_t getBound() const {
					return bound;
				}

				/*!
				 * Sets the the time instance for the instantaneous reward operator.
				 *
				 * @param bound The new time instance.
				 */
				void setBound(uint_fast64_t bound) {
					this->bound = bound;
				}

			private:

				// The time instance of the reward formula.
				uint_fast64_t bound;
			};

		} // namespace prctl
	} // namespace properties
} // namespace storm

#endif /* STORM_PROPERTIES_PRCTL_INSTANTANEOUSREWARD_H_ */
