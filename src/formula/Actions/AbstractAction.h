/*
 * AbstractAction.h
 *
 *  Created on: Apr 26, 2014
 *      Author: Manuel Sascha Weiand
 */

#ifndef STORM_FORMULA_ACTION_ABSTRACTACTION_H_
#define STORM_FORMULA_ACTION_ABSTRACTACTION_H_

#include <vector>
#include "src/storage/BitVector.h"

namespace storm {
namespace property {
namespace action {

template <class T>
class AbstractAction {

public:

	/*!
	 * Virtual destructor
	 * To ensure that the right destructor is called
	 */
	virtual ~AbstractAction() {
		//Intentionally left empty
	}

	/*!
	 *
	 */
	virtual std::vector<T> evaluate(std::vector<T> input) const {
		return input;
	}

	/*!
	 *
	 */
	virtual storm::storage::BitVector evaluate(storm::storage::BitVector input) const {
		return input;
	}

	/*!
	 *
	 */
	virtual std::string toString() const = 0;

	/*!
	 *
	 */
	virtual std::string toFormulaString() const = 0;
};

} //namespace action
} //namespace property
} //namespace storm


#endif /* STORM_FORMULA_ACTION_ABSTRACTACTION_H_ */
