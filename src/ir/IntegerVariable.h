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
		return getVariableName() + ": int[" + lowerBound->toString() + ".." + upperBound->toString() + "];";
	}

private:
	std::shared_ptr<storm::ir::expressions::BaseExpression> lowerBound;
	std::shared_ptr<storm::ir::expressions::BaseExpression> upperBound;
};

}

}

#endif /* INTEGERVARIABLE_H_ */
