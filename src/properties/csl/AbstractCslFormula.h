#ifndef STORM_PROPERTIES_CSL_ABSTRACTCSLFORMULA_H_
#define STORM_PROPERTIES_CSL_ABSTRACTCSLFORMULA_H_

#include "src/properties/AbstractFormula.h"

namespace storm {
	namespace properties {
		namespace csl {

			// Forward declarations.
			template <class T> class ProbabilisticBoundOperator;
			template <class T> class Eventually;
			template <class T> class Until;

			/*!
			 * This is the abstract base class for all Csl formulas.
			 *
			 * @note While formula classes do have copy constructors using a copy constructor
			 *       will yield a formula objects whose formula subtree consists of the same objects
			 *       as the original formula. The ownership of the formula tree will be shared between
			 *       the original and the copy.
			 */
			template <class T>
			class AbstractCslFormula : public virtual storm::properties::AbstractFormula<T>{
			public:

				/*!
				 * The virtual destructor.
				 */
				virtual ~AbstractCslFormula() {
					// Intentionally left empty
				}

				/*!
				 * Checks whether the formula is a probabilistic bound reachability formula.
				 * Returns true iff the formula conforms to the following pattern.
				 * Pattern: P[<,<=,>,>=]p ([psi U, E] phi) whith psi, phi propositional logic formulas (consisiting only of And, Or, Not and AP).
				 * That is, a probabilistic bound operator as root with a single until or eventually formula directly below it, whose subformulas are propositional
				 * (denoting some set of atomic propositions).
				 *
				 * @return True iff this is a probabilistic bound reachability formula.
				 */
				bool isProbEventuallyAP() const {

					// Test if a probabilistic bound operator is at the root.
					if(dynamic_cast<storm::properties::csl::ProbabilisticBoundOperator<T> const *>(this) == nullptr) {
						return false;
					}

					auto probFormula = dynamic_cast<storm::properties::csl::ProbabilisticBoundOperator<T> const *>(this);

					// Check if the direct subformula of the probabilistic bound operator is an eventually or until formula.
					if(std::dynamic_pointer_cast<storm::properties::csl::Eventually<T>>(probFormula->getChild()).get() != nullptr) {

						// Get the subformula and check if its subformulas are propositional.
						auto eventuallyFormula = std::dynamic_pointer_cast<storm::properties::csl::Eventually<T>>(probFormula->getChild());
						return eventuallyFormula->getChild()->isPropositional();
					} else if(std::dynamic_pointer_cast<storm::properties::csl::Until<T>>(probFormula->getChild()).get() != nullptr) {

						// Get the subformula and check if its subformulas are propositional.
						auto untilFormula = std::dynamic_pointer_cast<storm::properties::csl::Until<T>>(probFormula->getChild());
						return untilFormula->getLeft()->isPropositional() && untilFormula->getRight()->isPropositional();
					}

					return false;
				}
			};

		} // namespace csl
	} // namespace properties
} // namespace storm

#endif /* STORM_PROPERTIES_CSL_ABSTRACTCSLFORMULA_H_ */
