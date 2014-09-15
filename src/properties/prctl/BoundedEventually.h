#ifndef STORM_PROPERTIES_PRCTL_BOUNDEDEVENTUALLY_H_
#define STORM_PROPERTIES_PRCTL_BOUNDEDEVENTUALLY_H_

#include "src/properties/prctl/AbstractPathFormula.h"
#include "src/properties/prctl/AbstractStateFormula.h"
#include <cstdint>
#include <string>
#include "src/modelchecker/prctl/ForwardDeclarations.h"

namespace storm {
	namespace properties {
		namespace prctl {

			// Forward declaration for the interface class.
			template <class T> class BoundedEventually;

			/*!
			 * Interface class for model checkers that support BoundedEventually.
			 *
			 * All model checkers that support the formula class BoundedEventually must inherit
			 * this pure virtual class.
			 */
			template <class T>
			class IBoundedEventuallyModelChecker {
				public:

					/*!
					 * Empty virtual destructor.
					 */
					virtual ~IBoundedEventuallyModelChecker() {
						// Intentionally left empty
					}

					/*!
					 * Evaluates a BoundedEventually formula within a model checker.
					 *
					 * @param obj Formula object with subformulas.
					 * @param qualitative A flag indicating whether the formula only needs to be evaluated qualitatively, i.e. if the
					 *                    results are only compared against the bounds 0 and 1.
					 * @return The modelchecking result of the formula for every state.
					 */
					virtual std::vector<T> checkBoundedEventually(BoundedEventually<T> const & obj, bool qualitative) const = 0;
			};

			/*!
			 * Class for a Prctl (path) formula tree with a BoundedEventually node as root.
			 *
			 * Has one state formula as subformula/tree.
			 *
			 * @par Semantics
			 * The formula holds iff in at most \e bound steps, formula \e child holds.
			 *
			 * The object has shared ownership of its subtree. If this object is deleted and no other object has a shared
			 * ownership of the subtree it will be deleted as well.
			 *
			 * @see AbstractPathFormula
			 * @see AbstractPrctlFormula
			 */
			template <class T>
			class BoundedEventually : public AbstractPathFormula<T> {

			public:

				/*!
				 * Creates a BoundedEventually node without a subnode.
				 * The resulting object will not represent a complete formula!
				 */
				BoundedEventually() : child(nullptr), bound(0) {
					// Intentionally left empty.
				}

				/*!
				 * Creates a BoundedEventually node using the given parameters.
				 *
				 * @param child The child formula subtree.
				 * @param bound The maximal number of steps within which the subformula must hold.
				 */
				BoundedEventually(std::shared_ptr<AbstractStateFormula<T>> child, uint_fast64_t bound) : child(child), bound(bound) {
					// Intentionally left empty.
				}

				/*!
				 * Empty virtual destructor.
				 */
				virtual ~BoundedEventually() {
				  // Intentionally left empty.
				}

				/*!
				 * Clones the called object.
				 *
				 * Performs a "deep copy", i.e. the subnodes of the new object are clones of the original ones.
				 *
				 * @returns A new BoundedEventually object that is a deep copy of the called object.
				 */
				virtual std::shared_ptr<AbstractPathFormula<T>> clone() const override {
					auto result = std::make_shared<BoundedEventually<T>>();
					result->setBound(bound);
					if (this->isChildSet()) {
						result->setChild(child->clone());
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
					return modelChecker.template as<IBoundedEventuallyModelChecker>()->checkBoundedEventually(*this, qualitative);
				}

				/*!
				 * Returns a textual representation of the formula tree with this node as root.
				 *
				 * @returns A string representing the formula tree.
				 */
				virtual std::string toString() const override {
					std::string result = "F<=";
					result += std::to_string(bound);
					result += " ";
					result += child->toString();
					return result;
				}

				/*!
				 * Gets the child node.
				 *
				 * @returns The child node.
				 */
				std::shared_ptr<AbstractStateFormula<T>> const & getChild() const {
					return child;
				}

				/*!
				 * Sets the subtree.
				 *
				 * @param child The new child.
				 */
				void setChild(std::shared_ptr<AbstractStateFormula<T>> const & child) {
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

				/*!
				 * Gets the maximally allowed number of steps for the bounded eventually operator.
				 *
				 * @returns The bound.
				 */
				uint_fast64_t getBound() const {
					return bound;
				}

				/*!
				 * Sets the maximally allowed number of steps for the bounded eventually operator.
				 *
				 * @param bound The new bound.
				 */
				void setBound(uint_fast64_t bound) {
					this->bound = bound;
				}

			private:

				// The child node.
				std::shared_ptr<AbstractStateFormula<T>> child;

				// The maximal number of steps within which the subformula must hold.
				uint_fast64_t bound;
			};

		} // namespace prctl
	} // namespace properties
} // namespace storm

#endif /* STORM_PROPERTIES_PRCTL_BOUNDEDEVENTUALLY_H_ */
