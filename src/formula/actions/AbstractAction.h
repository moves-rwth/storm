/*
 * AbstractAction.h
 *
 *  Created on: Apr 26, 2014
 *      Author: Manuel Sascha Weiand
 */

#ifndef STORM_FORMULA_ACTION_ABSTRACTACTION_H_
#define STORM_FORMULA_ACTION_ABSTRACTACTION_H_

#include <vector>
#include <utility>
#include "src/storage/BitVector.h"
#include "src/modelchecker/prctl/AbstractModelChecker.h"
#include "src/modelchecker/csl/AbstractModelChecker.h"
#include "src/modelchecker/ltl/AbstractModelChecker.h"

namespace storm {
namespace property {
namespace action {

/*!
 * This is the abstract base class for all filter actions.
 *
 * Each action implements a distinct function which executed each time evaluate() is called.
 * The input and output for each action is an instance of the Result struct.
 * Thus the action is able to manipulate both the selection of output states and the order in which they are returned.
 */
template <class T>
class AbstractAction {

public:

	/*!
	 * This is the struct used by all actions to pass along information.
	 *
	 * It contains fields for the two possible data structures containing the modelchecking results.
	 * A value vector for path formulas and a bit vector for state formulas.
	 * The two other fields are used to describe a selection as well as an ordering of states within the modelchecking results.
	 *
	 * @note For each formula the modelchecker will return either a BitVector or a std::vector.
	 *       Thus, either the state- or the pathResult should be set to be empty.
	 *       If both contain a value for at least one state the pathResult will be used.
	 *
	 * @note The four vectors should always have the same number of entries.
	 */
	struct Result {

		/*!
		 * Constructs an empty Result.
		 */
		Result() : selection(), stateMap(), pathResult(), stateResult(){
			// Intentionally left empty.
		}

		/*!
		 * Copy constructs a Result.
		 *
		 * @param other The Result from which the fields are copied.
		 */
		Result(Result const & other) : selection(other.selection), stateMap(other.stateMap), pathResult(other.pathResult), stateResult(other.stateResult) {
			// Intentionally left empty.
		}

		/*!
		 * Constructs a Result using values for each field.
		 *
		 * @param selection A BitVector describing which states are currently selected.
		 * @param stateMap A vector representing an ordering on the states within the modelchecking results.
		 * @param pathResult The modelchecking result for a path formula.
		 * @param stateResult The modelchecking result for a state formula.
		 */
		Result(storm::storage::BitVector const & selection, std::vector<uint_fast64_t> const & stateMap, std::vector<T> const & pathResult, storm::storage::BitVector const & stateResult) : selection(selection), stateMap(stateMap), pathResult(pathResult), stateResult(stateResult) {
			// Intentionally left empty.
		}

		//! A BitVector describing which states are currently selected.
		//! For state i the i-th bit is true iff state i is selected.
		storm::storage::BitVector selection;

		//! A vector representing an ordering on the states within the modelchecking results.
		//! The first value of the vector is the index of the state to be returned first.
		//! Thus, stateMap[i] is the index of the state that is to be returned at position i.
		std::vector<uint_fast64_t> stateMap;

		//! The modelchecking result for a path formula.
		std::vector<T> pathResult;

		//! The modelchecking result for a state formula.
		storm::storage::BitVector stateResult;
	};

	/*!
	 * The virtual destructor.
	 * To ensure that the right destructor is called.
	 */
	virtual ~AbstractAction() {
		//Intentionally left empty
	}

	/*!
	 * Evaluate the action for a Prctl formula.
	 *
	 * @param result The Result struct on which the action is to be evaluated.
	 * @param mc The Prctl modelchecker that computed the path- or stateResult contained in the Result struct.
	 * @returns A Result struct containing the output of the action.
	 */
	virtual Result evaluate(Result const & result, storm::modelchecker::prctl::AbstractModelChecker<T> const & mc) const {
		return Result();
	}

	/*!
	 * Evaluate the action for a Csl formula.
	 *
	 * @param result The Result struct on which the action is to be evaluated.
	 * @param mc The Csl modelchecker that computed the path- or stateResult contained in the Result struct.
	 * @returns A Result struct containing the output of the action.
	 */
	virtual Result evaluate(Result const & result, storm::modelchecker::csl::AbstractModelChecker<T> const & mc) const {
		return Result();
	}

	/*!
	 * Evaluate the action for an Ltl formula.
	 *
	 * @param result The Result struct on which the action is to be evaluated.
	 * @param mc The Ltl modelchecker that computed the path- or stateResult contained in the Result struct.
	 * @returns A Result struct containing the output of the action.
	 */
	virtual Result evaluate(Result const & result, storm::modelchecker::ltl::AbstractModelChecker<T> const & mc) const {
		return Result();
	}

	/*!
	 * Returns a string representation of the action.
	 *
	 * @returns A string representing the action.
	 */
	virtual std::string toString() const = 0;

};

} //namespace action
} //namespace property
} //namespace storm


#endif /* STORM_FORMULA_ACTION_ABSTRACTACTION_H_ */
