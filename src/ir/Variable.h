/*
 * Variable.h
 *
 *  Created on: 06.01.2013
 *      Author: chris
 */

#ifndef VARIABLE_H_
#define VARIABLE_H_

namespace storm {

namespace ir {

class Variable {
public:
	Variable() : variableName(""), initialValue(nullptr) {

	}

	Variable(std::string variableName, std::shared_ptr<storm::ir::expressions::BaseExpression> initialValue = nullptr) : variableName(variableName), initialValue(initialValue) {

	}

	virtual ~Variable() {

	}

	virtual std::string toString() {
		return variableName;
	}

	std::string getVariableName() {
		return variableName;
	}

	std::shared_ptr<storm::ir::expressions::BaseExpression> getInitialValue() {
		return initialValue;
	}

private:
	std::string variableName;
	std::shared_ptr<storm::ir::expressions::BaseExpression> initialValue;
};

}

}


#endif /* VARIABLE_H_ */
