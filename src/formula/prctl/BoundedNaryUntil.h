/*
 * BoundedNaryUntil.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_PRCTL_BOUNDEDNARYUNTIL_H_
#define STORM_FORMULA_PRCTL_BOUNDEDNARYUNTIL_H_

#include "src/formula/prctl/AbstractPathFormula.h"
#include "src/formula/prctl/AbstractStateFormula.h"
#include <cstdint>
#include <string>
#include <vector>
#include <tuple>
#include <sstream>
#include "src/modelchecker/prctl/ForwardDeclarations.h"

namespace storm {
namespace property {
namespace prctl {


template <class T> class BoundedNaryUntil;

/*!
 *  @brief Interface class for model checkers that support BoundedNaryUntil.
 *   
 *  All model checkers that support the formula class BoundedNaryUntil must inherit
 *  this pure virtual class.
 */
template <class T>
class IBoundedNaryUntilModelChecker {
    public:
		/*!
         *  @brief Evaluates BoundedNaryUntil formula within a model checker.
         *
         *  @param obj Formula object with subformulas.
         *  @return Result of the formula for every node.
         */
        virtual std::vector<T> checkBoundedNaryUntil(const BoundedNaryUntil<T>& obj, bool qualitative) const = 0;
};

/*!
 * @brief
 * Class for an abstract (path) formula tree with a BoundedNaryUntil node as root.
 *
 * Has at least two Abstract state formulas as sub formulas and an interval
 * associated with all but the first sub formula. We'll call the first one
 * \e left and all other one \e right.
 *
 * @par Semantics
 * The formula holds iff \e left holds until eventually any of the \e right
 * formulas holds after a number of steps contained in the interval
 * associated with this formula.
 *
 * The subtrees are seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @see AbstractPathFormula
 * @see AbstractPrctlFormula
 */
template <class T>
class BoundedNaryUntil : public AbstractPathFormula<T> {

public:

	/*!
	 * Empty constructor
	 */
	BoundedNaryUntil() : left(nullptr), right() {
		// Intentionally left empty.
	}

	/*!
	 * Constructor
	 *
	 * @param left The left formula subtree
	 * @param right The left formula subtree
	 */
	BoundedNaryUntil(std::shared_ptr<AbstractStateFormula<T>> const & left, std::vector<std::tuple<std::shared_ptr<AbstractStateFormula<T>>,T,T>> const & right) : left(left), right(right) {
		// Intentionally left empty.
	}

	/*!
	 * Destructor.
	 *
	 * Also deletes the subtrees.
	 * (this behaviour can be prevented by setting the subtrees to NULL before deletion)
	 */
	virtual ~BoundedNaryUntil() {
	  // Intentionally left empty.
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new BoundedNaryUntil-object that is identical the called object.
	 */
	virtual std::shared_ptr<AbstractPathFormula<T>> clone() const override {
		std::shared_ptr<BoundedNaryUntil<T>> result(new BoundedNaryUntil<T>());
		if (this->leftIsSet()) {
			result->setLeft(left->clone());
		}
		if (this->rightIsSet()) {
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
	virtual std::vector<T> check(const storm::modelchecker::prctl::AbstractModelChecker<T>& modelChecker, bool qualitative) const override {
		return modelChecker.template as<IBoundedNaryUntilModelChecker>()->checkBoundedNaryUntil(*this, qualitative);
	}

	/*!
	 * @returns a string representation of the formula
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
	 * Sets the left child node.
	 *
	 * @param newLeft the new left child.
	 */
	void setLeft(std::shared_ptr<AbstractStateFormula<T>> const & newLeft) {
		left = newLeft;
	}

	void setRight(std::vector<std::tuple<std::shared_ptr<AbstractStateFormula<T>>,T,T>> const & newRight) {
		right = newRight;
	}

	/*!
	 *
	 * @return True if the left child is set, i.e. it does not point to nullptr; false otherwise
	 */
	bool leftIsSet() const {
		return left != nullptr;
	}

	/*!
	 *
	 * @return True if the right child is set, i.e. it is not empty; false otherwise
	 */
	bool rightIsSet() const {
		return !(right.empty());
	}

	/*!
	 * Sets the right child node.
	 *
	 * @param newRight the new right child.
	 */
	void addRight(std::shared_ptr<AbstractStateFormula<T>> const & newRight, T upperBound, T lowerBound) {
		right.push_back(std::tuple<std::shared_ptr<AbstractStateFormula<T>>,T,T>(newRight, upperBound, lowerBound));
	}

	/*!
	 * @returns a pointer to the left child node
	 */
	std::shared_ptr<AbstractStateFormula<T>> const & getLeft() const {
		return left;
	}

	/*!
	 * @returns a pointer to the right child nodes.
	 */
	std::vector<std::tuple<std::shared_ptr<AbstractStateFormula<T>>,T,T>> const & getRight() const {
		return right;
	}


private:
	std::shared_ptr<AbstractStateFormula<T>> left;
	std::vector<std::tuple<std::shared_ptr<AbstractStateFormula<T>>,T,T>> right;
};

} //namespace prctl
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_PRCTL_BOUNDEDNARYUNTIL_H_ */
