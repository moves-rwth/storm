/*
 * IntegerVariable.h
 *
 *  Created on: 08.01.2013
 *      Author: chris
 */

#ifndef INTEGERVARIABLE_H_
#define INTEGERVARIABLE_H_

#include "Variable.h"

namespace storm {

namespace ir {

class IntegerVariable : public Variable {
public:
	IntegerVariable() {

	}

	IntegerVariable(std::string variableName, std::shared_ptr<storm::ir::expressions::BaseExpression> lowerBound, std::shared_ptr<storm::ir::expressions::BaseExpression> upperBound) : Variable(variableName), lowerBound(lowerBound), upperBound(upperBound) {

	}

	virtual ~IntegerVariable() {

	}

	virtual std::string toString() {
		return variableName + ": int[];";
	}

	void setLowerBound(std::shared_ptr<storm::ir::expressions::BaseExpression>& lowerBound) {
		this->lowerBound = lowerBound;
	}

	void setUpperBound(std::shared_ptr<storm::ir::expressions::BaseExpression>& upperBound) {
		this->upperBound = upperBound;
	}

	std::shared_ptr<storm::ir::expressions::BaseExpression> lowerBound;
	std::shared_ptr<storm::ir::expressions::BaseExpression> upperBound;
};

}

}

#endif /* INTEGERVARIABLE_H_ */
