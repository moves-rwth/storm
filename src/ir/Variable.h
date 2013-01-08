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
	std::string variableName;

	Variable() {

	}

	Variable(std::string variableName) : variableName(variableName) {

	}

	virtual ~Variable() {

	}

	virtual std::string toString() {
		return variableName;
	}

	void setVariableName(std::string variableName) {
		this->variableName = variableName;
	}
};

}

}


#endif /* VARIABLE_H_ */
