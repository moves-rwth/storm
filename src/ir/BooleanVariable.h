/*
 * BooleanVariable.h
 *
 *  Created on: 08.01.2013
 *      Author: chris
 */

#ifndef BOOLEANVARIABLE_H_
#define BOOLEANVARIABLE_H_

namespace storm {

namespace ir {

class BooleanVariable : public Variable {
public:
	BooleanVariable() {

	}

	BooleanVariable(std::string variableName, std::shared_ptr<storm::ir::expressions::BaseExpression> initialValue = nullptr) : Variable(variableName,  initialValue) {

	}

	virtual ~BooleanVariable() {

	}

	virtual std::string toString() {
		std::string result = getVariableName() + ": bool";
		if (this->getInitialValue() != nullptr) {
			result += " init " + this->getInitialValue()->toString();
		}
		result += ";";
		return result;
	}
};

}

}

#endif /* BOOLEANVARIABLE_H_ */
