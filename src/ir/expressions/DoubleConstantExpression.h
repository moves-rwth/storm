/*
 * DoubleConstantExpression.h
 *
 *  Created on: 04.01.2013
 *      Author: chris
 */

#ifndef DOUBLECONSTANTEXPRESSION_H_
#define DOUBLECONSTANTEXPRESSION_H_

#include "ConstantExpression.h"

namespace storm {

namespace ir {

namespace expressions {

class DoubleConstantExpression : public ConstantExpression {
public:
	DoubleConstantExpression(std::string constantName) : ConstantExpression(constantName) {
		defined = false;
		value = 0.0;
	}

	virtual ~DoubleConstantExpression() {

	}

	virtual std::string toString() const {
		std::string result = this->constantName;
		if (defined) {
			result += "[" + boost::lexical_cast<std::string>(value) + "]";
		}
		return result;
	}

	bool isDefined() {
		return defined;
	}

	double getValue() {
		return value;
	}

	void define(double value) {
		defined = true;
		this->value = value;
	}

	double value;
	bool defined;
};

}

}

}

#endif /* DOUBLECONSTANTEXPRESSION_H_ */
