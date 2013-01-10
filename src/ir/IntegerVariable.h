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
	IntegerVariable() : lowerBound(nullptr), upperBound(nullptr) {

	}

	IntegerVariable(std::string variableName, std::shared_ptr<storm::ir::expressions::BaseExpression> lowerBound, std::shared_ptr<storm::ir::expressions::BaseExpression> upperBound, std::shared_ptr<storm::ir::expressions::BaseExpression> initialValue = nullptr) : Variable(variableName, initialValue), lowerBound(lowerBound), upperBound(upperBound) {

	}

	virtual ~IntegerVariable() {

	}

	virtual std::string toString() {
		std::string result = getVariableName() + ": [" + lowerBound->toString() + ".." + upperBound->toString() + "]";
		if (this->getInitialValue() != nullptr) {
			result += " init " + this->getInitialValue()->toString();
		}
		result += ";";
		return result;
	}

private:
	std::shared_ptr<storm::ir::expressions::BaseExpression> lowerBound;
	std::shared_ptr<storm::ir::expressions::BaseExpression> upperBound;
};

}

}

#endif /* INTEGERVARIABLE_H_ */
