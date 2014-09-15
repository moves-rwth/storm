#ifndef STORM_PROPERTIES_ABSTRACTFORMULA_H_
#define STORM_PROPERTIES_ABSTRACTFORMULA_H_

#include <string>
#include <memory>

namespace storm {
	namespace properties {

		// Forward declaration.
		template <class T> class AbstractFormula;

		/*!
		 * This is the abstract base class for every formula class in every logic.
		 *
		 * There are currently three implemented logics Ltl, Csl and Pctl.
		 * The implementation of these logics can be found in the namespaces storm::properties::<logic>
		 * where <logic> is one of ltl, pctl and csl.
		 *
		 * @note While formula classes do have copy constructors using a copy constructor
		 *       will yield a formula objects whose formula subtree consists of the same objects
		 *       as the original formula. The ownership of the formula tree will be shared between
		 *       the original and the copy.
		 */
		template <class T>
		class AbstractFormula {

		public:

			/*!
			 * The virtual destructor.
			 */
			virtual ~AbstractFormula() {
				// Intentionally left empty.
			}

			/*!
			 *	Return string representation of this formula.
			 *
			 *	@note Every subclass must implement this method.
			 *
			 *	@returns A string representation of the formula.
			 */
			virtual std::string toString() const = 0;

			/*!
			 * Returns whether the formula is a propositional logic formula.
			 * That is, this formula and all its subformulas consist only of And, Or, Not and AP.
			 *
			 * @return True iff this is a propositional logic formula.
			 */
			virtual bool isPropositional() const {
				return false;
			}
		};

		/*!
		 * Overloads the stream operator for AbstractFormulas and thus all formula classes.
		 *
		 * @param os The output stream to which the string representation of the formula is to be appended.
		 * @param formula The formula whose string representation is to be appended to the given output stream.
		 * @returns A reference to an output stream containing the contents of the output stream given as input,
		 *          appended with the string representation of the given formula.
		 */
		template <class T>
		std::ostream & operator<<(std::ostream& os, AbstractFormula<T> const & formula) {

			os << formula.toString();
			return os;
		}

	} // namespace properties
} // namespace storm

#endif /* STORM_PROPERTIES_ABSTRACTFORMULA_H_ */
