#ifndef STORM_PROPERTIES_CSL_PROBABILISTICBOUNDOPERATOR_H_
#define STORM_PROPERTIES_CSL_PROBABILISTICBOUNDOPERATOR_H_

#include "src/properties/csl/AbstractStateFormula.h"
#include "src/properties/csl/AbstractPathFormula.h"
#include "src/properties/ComparisonType.h"
#include "utility/constants.h"

namespace storm {
	namespace properties {
		namespace csl {

			// Forward declaration for the interface class.
			template <class T> class ProbabilisticBoundOperator;

			/*!
			 * Interface class for model checkers that support ProbabilisticBoundOperator.
			 *
			 * All model checkers that support the formula class PathBoundOperator must inherit
			 * this pure virtual class.
			 */
			template <class T>
			class IProbabilisticBoundOperatorModelChecker {
				public:

					/*!
					 * Empty virtual destructor.
					 */
					virtual ~IProbabilisticBoundOperatorModelChecker() {
						// Intentionally left empty
					}

					/*!
					 * Evaluates a ProbabilisticBoundOperator within a model checker.
					 *
					 * @param obj Formula object with subformulas.
					 * @return The modelchecking result of the formula for every state.
					 */
					virtual storm::storage::BitVector checkProbabilisticBoundOperator(ProbabilisticBoundOperator<T> const & obj) const = 0;
			};

			/*!
			 * Class for a Csl formula tree with a P (probablistic) bound operator node as root.
			 *
			 * Has one path formula as sub formula/tree.
			 *
			 * @par Semantics
			 * 	  The formula holds iff the probability that the path formula holds meets the bound
			 * 	  specified in this operator
			 *
			 * The object has shared ownership of its subtree. If this object is deleted and no other object has a shared
			 * ownership of the subtree it will be deleted as well.
			 *
			 * @see AbstractStateFormula
			 * @see AbstractPathFormula
			 * @see AbstractCslFormula
			 */
			template<class T>
			class ProbabilisticBoundOperator : public AbstractStateFormula<T> {

			public:

				/*!
				 * Creates a ProbabilisticBoundOperator node without a subnode.
				 * The resulting object will not represent a complete formula!
				 */
				ProbabilisticBoundOperator() : comparisonOperator(LESS), bound(0), child(nullptr) {
					// Intentionally left empty.
				}

				/*!
				 * Creates a ProbabilisticBoundOperator node using the given parameters.
				 *
				 * @param comparisonOperator The relation for the bound.
				 * @param bound The bound for the probability.
				 * @param child The child formula subtree.
				 */
				ProbabilisticBoundOperator(storm::properties::ComparisonType comparisonOperator, T bound, std::shared_ptr<AbstractPathFormula<T>> const & child)
					: comparisonOperator(comparisonOperator), bound(bound), child(child) {
					// Intentionally left empty.
				}

				/*!
				 * Empty virtual destructor.
				 */
				virtual ~ProbabilisticBoundOperator() {
					// Intentionally left empty.
				}

				/*!
				 * Clones the called object.
				 *
				 * Performs a "deep copy", i.e. the subnodes of the new object are clones of the original ones.
				 *
				 * @returns A new ProbabilisticBoundOperator object that is a deep copy of the called object.
				 */
				virtual std::shared_ptr<AbstractStateFormula<T>> clone() const override {
					auto result = std::make_shared<ProbabilisticBoundOperator<T>>();
					result->setComparisonOperator(comparisonOperator);
					result->setBound(bound);
					result->setChild(child->clone());
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
				virtual storm::storage::BitVector check(storm::modelchecker::csl::AbstractModelChecker<T> const & modelChecker) const override {
					return modelChecker.template as<IProbabilisticBoundOperatorModelChecker>()->checkProbabilisticBoundOperator(*this);
				}

				/*!
				 * Returns a textual representation of the formula tree with this node as root.
				 *
				 * @returns A string representing the formula tree.
				 */
				virtual std::string toString() const override {
					std::string result = "P ";
					switch (comparisonOperator) {
					case LESS: result += "<"; break;
					case LESS_EQUAL: result += "<="; break;
					case GREATER: result += ">"; break;
					case GREATER_EQUAL: result += ">="; break;
					}
					result += " ";
					result += std::to_string(bound);
					result += " (";
					result += child->toString();
					result += ")";
					return result;
				}

				/*!
				 * Gets the child node.
				 *
				 * @returns The child node.
				 */
				std::shared_ptr<AbstractPathFormula<T>> const & getChild () const {
					return child;
				}

				/*!
				 * Sets the subtree.
				 *
				 * @param child The new child.
				 */
				void setChild(std::shared_ptr<AbstractPathFormula<T>> const & child) {
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
				 * Gets the comparison operator.
				 *
				 * @returns An enum value representing the comparison relation.
				 */
				storm::properties::ComparisonType const getComparisonOperator() const {
					return comparisonOperator;
				}

				/*!
				 * Sets the comparison operator.
				 *
				 * @param comparisonOperator An enum value representing the new comparison relation.
				 */
				void setComparisonOperator(storm::properties::ComparisonType comparisonOperator) {
					this->comparisonOperator = comparisonOperator;
				}

				/*!
				 * Gets the bound which the probability that the path formula holds has to obey.
				 *
				 * @returns The probability bound.
				 */
				T const & getBound() const {
					return bound;
				}

				/*!
				 * Sets the bound which the probability that the path formula holds has to obey.
				 *
				 * @param bound The new probability bound.
				 */
				void setBound(T const & bound) {
					this->bound = bound;
				}

				/*!
				 * Checks if the bound is met by the given value.
				 *
				 * @param value The value to test against the bound.
				 * @returns True iff value <comparisonOperator> bound holds.
				 */
				bool meetsBound(T const & value) const {
					switch (comparisonOperator) {
					case LESS: return value < bound; break;
					case LESS_EQUAL: return value <= bound; break;
					case GREATER: return value > bound; break;
					case GREATER_EQUAL: return value >= bound; break;
					default: return false;
					}
				}

			private:

				// The operator used to indicate the kind of bound that is to be met.
				storm::properties::ComparisonType comparisonOperator;

				// The probability bound.
				T bound;

				// The path formula for which the probability to be satisfied has to meet the bound.
				std::shared_ptr<AbstractPathFormula<T>> child;
			};

		} // namespace csl
	} // namespace properties
} // namespace storm

#endif /* STORM_PROPERTIES_CSL_PROBABILISTICBOUNDOPERATOR_H_ */
