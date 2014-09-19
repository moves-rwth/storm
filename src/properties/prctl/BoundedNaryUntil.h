#ifndef STORM_PROPERTIES_PRCTL_BOUNDEDNARYUNTIL_H_
#define STORM_PROPERTIES_PRCTL_BOUNDEDNARYUNTIL_H_

#include "src/properties/prctl/AbstractPathFormula.h"
#include "src/properties/prctl/AbstractStateFormula.h"
#include <cstdint>
#include <string>
#include <vector>
#include <tuple>
#include <sstream>
#include "src/modelchecker/prctl/ForwardDeclarations.h"

namespace storm {
	namespace properties {
		namespace prctl {

			// Forward declaration for the interface class.
			template <class T> class BoundedNaryUntil;

			/*!
			 * Interface class for model checkers that support BoundedNaryUntil.
			 *
			 * All model checkers that support the formula class BoundedNaryUntil must inherit
			 * this pure virtual class.
			 */
			template <class T>
			class IBoundedNaryUntilModelChecker {
				public:

					/*!
					 * Empty virtual destructor.
					 */
					virtual ~IBoundedNaryUntilModelChecker() {
						// Intentionally left empty
					}

					/*!
					 * Evaluates BoundedNaryUntil formula within a model checker.
					 *
					 * @param obj Formula object with subformulas.
					 * @param qualitative A flag indicating whether the formula only needs to be evaluated qualitatively, i.e. if the
					 *                    results are only compared against the bounds 0 and 1.
					 * @return Result of the formula for every state.
					 */
					virtual std::vector<T> checkBoundedNaryUntil(BoundedNaryUntil<T> const & obj, bool qualitative) const = 0;
			};

			/*!
			 * Class for a Prctl (path) formula tree with a BoundedNaryUntil node as root.
			 *
			 * Has at least two state formulas as sub formulas and an interval
			 * associated with all but the first sub formula. We will call the first one
			 * \e left and all other one \e right.
			 *
			 * @par Semantics
			 * The formula holds iff \e left holds until eventually any of the \e right
			 * formulas holds after a number of steps contained in the interval
			 * associated with this formula.
			 *
			 * The object has shared ownership of its subtrees. If this object is deleted and no other object has a shared
			 * ownership of the subtrees they will be deleted as well.
			 *
			 * @see AbstractPathFormula
			 * @see AbstractPrctlFormula
			 */
			template <class T>
			class BoundedNaryUntil : public AbstractPathFormula<T> {

			public:

				/*!
				 * Creates a BoundedNaryUntil node without subnodes.
				 * The resulting object will not represent a complete formula!
				 */
				BoundedNaryUntil() : left(nullptr), right() {
					// Intentionally left empty.
				}

				/*!
				 * Creates a BoundedNaryUntil node with the parameters as subtrees.
				 *
				 * @param left The left formula subtree.
				 * @param right The right formula subtrees with their associated intervals.
				 */
				BoundedNaryUntil(std::shared_ptr<AbstractStateFormula<T>> const & left, std::vector<std::tuple<std::shared_ptr<AbstractStateFormula<T>>,T,T>> const & right) : left(left), right(right) {
					// Intentionally left empty.
				}

				/*!
				 * Empty virtual destructor.
				 */
				virtual ~BoundedNaryUntil() {
				  // Intentionally left empty.
				}

				/*!
				 * Clones the called object.
				 *
				 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones.
				 *
				 * @returns A new BoundedNaryUntil object that is a deep copy of the called object.
				 */
				virtual std::shared_ptr<AbstractPathFormula<T>> clone() const override {

					auto result = std::make_shared<BoundedNaryUntil<T>>();
					if (this->isLeftSet()) {
						result->setLeft(left->clone());
					}
					if (this->isRightSet()) {
						std::vector<std::tuple<std::shared_ptr<AbstractStateFormula<T>>,T,T>> newright;
						for (auto it = right->begin(); it != right->end(); ++it) {
							newright.push_back(std::tuple<std::shared_ptr<AbstractStateFormula<T>>,T,T>(std::get<0>(*it)->clone(), std::get<1>(*it), std::get<2>(*it)));
						}
						result->setRight(newright);
					}
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
					return modelChecker.template as<IBoundedNaryUntilModelChecker>()->checkBoundedNaryUntil(*this, qualitative);
				}

				/*!
				 * Returns a textual representation of the formula tree with this node as root.
				 *
				 * @returns A string representing the formula tree.
				 */
				virtual std::string toString() const override {
					std::stringstream result;
					result << "( " << left->toString();
					for (auto it = right->begin(); it != right->end(); ++it) {
						result << " U(" << std::get<1>(*it) << "," << std::get<2>(*it) << ") " << std::get<0>(*it)->toString();
					}
					result << ")";
					return result.str();
				}

				/*!
				 * Gets the left child node.
				 *
				 * @returns The left child node.
				 */
				std::shared_ptr<AbstractStateFormula<T>> const & getLeft() const {
					return left;
				}

				/*!
				 * Gets the right child nodes and their associated intervals.
				 *
				 * @returns A vector containing the right children as well as the associated intervals.
				 */
				std::vector<std::tuple<std::shared_ptr<AbstractStateFormula<T>>,T,T>> const & getRight() const {
					return right;
				}

				/*!
				 * Sets the left child node.
				 *
				 * @param newLeft The new left child.
				 */
				void setLeft(std::shared_ptr<AbstractStateFormula<T>> const & newLeft) {
					left = newLeft;
				}

				/*!
				 * Sets the right child nodes.
				 *
				 * @param newRight A vector containing the new right children as well as the associated intervals.
				 */
				void setRight(std::vector<std::tuple<std::shared_ptr<AbstractStateFormula<T>>,T,T>> const & newRight) {
					right = newRight;
				}

				/*!
				 * Adds a new rightmost child node.
				 *
				 * @param newRight The new child.
				 * @param upperBound The upper bound of the associated interval.
				 * @param lowerBound The lower bound of the associated interval.
				 */
				void addRight(std::shared_ptr<AbstractStateFormula<T>> const & newRight, T upperBound, T lowerBound) {
					right.push_back(std::tuple<std::shared_ptr<AbstractStateFormula<T>>,T,T>(newRight, upperBound, lowerBound));
				}

				/*!
				 * Checks if the left child is set, i.e. it does not point to null.
				 *
				 * @return True iff the left child is set.
				 */
				bool isLeftSet() const {
					return left != nullptr;
				}

				/*!
				 * Checks if the right child is set, i.e. it contains at least one entry.
				 *
				 * @return True iff the right child is set.
				 */
				bool isRightSet() const {
					return !(right.empty());
				}

			private:

				// The left formula subtree.
				std::shared_ptr<AbstractStateFormula<T>> left;

				// The right formula subtrees with their associated intervals.
				std::vector<std::tuple<std::shared_ptr<AbstractStateFormula<T>>,T,T>> right;
			};

		} // namespace prctl
	} // namespace properties
} // namespace storm

#endif /* STORM_PROPERTIES_PRCTL_BOUNDEDNARYUNTIL_H_ */
