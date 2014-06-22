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

template <class T>
class AbstractAction {

public:

	struct Result {

		/*!
		 *
		 */
		Result() : selection(), stateMap(), pathResult(), stateResult(){
			// Intentionally left empty.
		}

		/*!
		 *
		 */
		Result(Result const & other) : selection(other.selection), stateMap(other.stateMap), pathResult(other.pathResult), stateResult(other.stateResult) {
			// Intentionally left empty.
		}

		/*!
		 *
		 */
		Result(storm::storage::BitVector const & selection, std::vector<uint_fast64_t> const & stateMap, std::vector<T> const & pathResult, storm::storage::BitVector const & stateResult) : selection(selection), stateMap(stateMap), pathResult(pathResult), stateResult(stateResult) {
			// Intentionally left empty.
		}

		//!
		storm::storage::BitVector selection;

		//!
		std::vector<uint_fast64_t> stateMap;

		//!
		std::vector<T> pathResult;

		//!
		storm::storage::BitVector stateResult;
	};

	/*!
	 * Virtual destructor
	 * To ensure that the right destructor is called
	 */
	virtual ~AbstractAction() {
		//Intentionally left empty
	}

	/*
	 *
	 */
	virtual Result evaluate(Result const & filterResult, storm::modelchecker::prctl::AbstractModelChecker<T> const & mc) const {
		return Result();
	}

	/*
	 *
	 */
	virtual Result evaluate(Result const & filterResult, storm::modelchecker::csl::AbstractModelChecker<T> const & mc) const {
		return Result();
	}

	/*
	 *
	 */
	virtual Result evaluate(Result const & filterResult, storm::modelchecker::ltl::AbstractModelChecker<T> const & mc) const {
		return Result();
	}

	/*!
	 *
	 */
	virtual std::string toString() const = 0;

};

} //namespace action
} //namespace property
} //namespace storm


#endif /* STORM_FORMULA_ACTION_ABSTRACTACTION_H_ */
