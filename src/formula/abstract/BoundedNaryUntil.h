/*
 * BoundedNaryUntil.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_ABSTRACT_BOUNDEDNARYUNTIL_H_
#define STORM_FORMULA_ABSTRACT_BOUNDEDNARYUNTIL_H_

#include "src/formula/abstract/AbstractFormula.h"
#include "boost/integer/integer_mask.hpp"
#include <string>
#include <vector>
#include <tuple>
#include <sstream>
#include "src/modelchecker/ForwardDeclarations.h"

namespace storm {
namespace property {
namespace abstract {

/*!
 * @brief
 * Class for an abstract (path) formula tree with a BoundedNaryUntil node as root.
 *
 * Has at least two formulas as sub formulas and an interval
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
 * @tparam FormulaType The type of the subformula.
 * 		  The instantiation of FormulaType should be a subclass of AbstractFormula, as the functions
 * 		  "toString" and "conforms" of the subformulas are needed.
 *
 * @see AbstractFormula
 */
template <class T, class FormulaType>
class BoundedNaryUntil : public virtual AbstractFormula<T> {

	// Throw a compiler error when FormulaType is not a subclass of AbstractFormula.
	static_assert(std::is_base_of<AbstractFormula<T>, FormulaType>::value,
				  "Instantiaton of FormulaType for storm::property::abstract::BoundedNaryUntil<T,FormulaType> has to be a subtype of storm::property::abstract::AbstractFormula<T>");

public:
	/*!
	 * Empty constructor
	 */
	BoundedNaryUntil() {
		this->left = nullptr;
		this->right = new std::vector<std::tuple<FormulaType*,T,T>>();
	}

	/*!
	 * Constructor
	 *
	 * @param left The left formula subtree
	 * @param right The left formula subtree
	 */
	BoundedNaryUntil(FormulaType* left, std::vector<std::tuple<FormulaType*,T,T>>* right) {
		this->left = left;
		this->right = right;
	}

	/*!
	 * Destructor.
	 *
	 * Also deletes the subtrees.
	 * (this behaviour can be prevented by setting the subtrees to NULL before deletion)
	 */
	virtual ~BoundedNaryUntil() {
	  if (left != nullptr) {
		  delete left;
	  }
	  if (right != nullptr) {
		  delete right;
	  }
	}

	/*!
	 * Sets the left child node.
	 *
	 * @param newLeft the new left child.
	 */
	void setLeft(FormulaType* newLeft) {
		left = newLeft;
	}

	void setRight(std::vector<std::tuple<FormulaType*,T,T>>* newRight) {
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
	 * @return True if the right child is set, i.e. it does not point to nullptr; false otherwise
	 */
	bool rightIsSet() const {
		return right != nullptr;
	}

	/*!
	 * Sets the right child node.
	 *
	 * @param newRight the new right child.
	 */
	void addRight(FormulaType* newRight, T upperBound, T lowerBound) {
		this->right->push_back(std::tuple<FormulaType*,T,T>(newRight, upperBound, lowerBound));
	}

	/*!
	 * @returns a pointer to the left child node
	 */
	const FormulaType& getLeft() const {
		return *left;
	}

	/*!
	 * @returns a pointer to the right child nodes.
	 */
	const std::vector<std::tuple<FormulaType*,T,T>>& getRight() const {
		return *right;
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const {
		std::stringstream result;
		result << "( " << left->toString();
		for (auto it = this->right->begin(); it != this->right->end(); ++it) {
			result << " U[" << std::get<1>(*it) << "," << std::get<2>(*it) << "] " << std::get<0>(*it)->toString();
		}
		result << ")";
		return result.str();
	}

	/*!
     *  @brief Checks if all subtrees conform to some logic.
     *
     *  @param checker Formula checker object.
     *  @return true iff all subtrees conform to some logic.
     */
	virtual bool validate(const AbstractFormulaChecker<T>& checker) const {
		bool res = checker.validate(this->left);
		for (auto it = this->right->begin(); it != this->right->end(); ++it) {
			res &= checker.validate(std::get<0>(*it));
		}
		return res;
	}


private:
	FormulaType* left;
	std::vector<std::tuple<FormulaType*,T,T>>* right;
};

} //namespace abstract
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_ABSTRACT_BOUNDEDNARYUNTIL_H_ */
