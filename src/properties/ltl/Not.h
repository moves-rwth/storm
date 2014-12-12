#ifndef STORM_PROPERTIES_LTL_NOT_H_
#define STORM_PROPERTIES_LTL_NOT_H_

#include "AbstractLtlFormula.h"

namespace storm {
	namespace properties {
		namespace ltl {

			// Forward declaration for the interface class.
			template <class T> class Not;

			/*!
			 * Interface class for model checkers that support Not.
			 *
			 * All model checkers that support the formula class Not must inherit
			 * this pure virtual class.
			 */
			template <class T>
			class INotModelChecker {
				public:

					/*!
					 * Empty virtual destructor.
					 */
					virtual ~INotModelChecker() {
						// Intentionally left empty
					}

					/*!
					 * Evaluates Not formula within a model checker.
					 *
					 * @param obj Formula object with subformulas.
					 * @return Result of the formula for every node.
					 */
					virtual std::vector<T> checkNot(Not<T> const & obj) const = 0;
			};

			/*!
			 * Class for an Ltl formula tree with a Not node as root.
			 *
			 * Has one Ltl formula as sub formula/tree.
			 *
			 * The object has shared ownership of its subtree. If this object is deleted and no other object has a shared
			 * ownership of the subtree it will be deleted as well.
			 *
			 * @see AbstractLtlFormula
			 */
			template <class T>
			class Not : public AbstractLtlFormula<T> {

			public:

				/*!
				 * Creates a Not node without a subnode.
				 * The resulting object will not represent a complete formula!
				 */
				Not() : child(nullptr) {
					// Intentionally left empty.
				}

				/*!
				 * Creates a Not node using the given parameter.
				 *
				 * @param child The child formula subtree.
				 */
				Not(std::shared_ptr<AbstractLtlFormula<T>> const & child) : child(child) {
					// Intentionally left empty.
				}

				/*!
				 * Empty virtual destructor.
				 */
				virtual ~Not() {
					// Intentionally left empty.
				}

				/*!
				 * Clones the called object.
				 *
				 * Performs a "deep copy", i.e. the subnodes of the new object are clones of the original ones.
				 *
				 * @returns A new Not object that is a deep copy of the called object.
				 */
				virtual std::shared_ptr<AbstractLtlFormula<T>> clone() const override {
					auto result = std::make_shared<Not<T>>();
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
				 * @returns A bit vector indicating all states that satisfy the formula represented by the called object.
				 */
				virtual std::vector<T> check(storm::modelchecker::ltl::AbstractModelChecker<T> const & modelChecker) const override {
					return modelChecker.template as<INotModelChecker>()->checkNot(*this);
				}

				/*!
				 * Returns a textual representation of the formula tree with this node as root.
				 *
				 * @returns A string representing the formula tree.
				 */
				virtual std::string toString() const override {
					std::string result = "!";
					result += child->toString();
					return result;
				}

				/*!
				 * Returns whether the formula is a propositional logic formula.
				 * That is, this formula and all its subformulas consist only of And, Or, Not and AP.
				 *
				 * @return True iff this is a propositional logic formula.
				 */
				virtual bool isPropositional() const override {
					return child->isPropositional();
				}

				/*!
				 * Gets the child node.
				 *
				 * @returns The child node.
				 */
				std::shared_ptr<AbstractLtlFormula<T>> const & getChild() const {
					return child;
				}

				/*!
				 * Sets the subtree.
				 *
				 * @param child The new child.
				 */
				void setChild(std::shared_ptr<AbstractLtlFormula<T>> const & child) {
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

				// The child node.
				std::shared_ptr<AbstractLtlFormula<T>> child;
			};

		} // namespace ltl
	} // namespace properties
} // namespace storm

#endif /* STORM_PROPERTIES_LTL_NOT_H_ */
