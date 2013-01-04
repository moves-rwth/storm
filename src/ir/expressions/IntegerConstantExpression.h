/*
 * IntegerConstantExpression.h
 *
 *  Created on: 04.01.2013
 *      Author: chris
 */

#ifndef INTEGERCONSTANTEXPRESSION_H_
#define INTEGERCONSTANTEXPRESSION_H_

#include "ConstantExpression.h"

namespace storm {

namespace ir {

namespace expressions {

class IntegerConstantExpression : public ConstantExpression {
public:
	IntegerConstantExpression(std::string constantName) : ConstantExpression(constantName) {
		defined = false;
		value = 0;
	}

	virtual ~IntegerConstantExpression() {

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

	int getValue() {
		return value;
	}

	void define(int value) {
		defined = true;
		this->value = value;
	}

	int value;
	bool defined;
};

}

}

}

BOOST_FUSION_ADAPT_STRUCT(
    storm::ir::expressions::IntegerConstantExpression,
    (std::string, constantName)
)

#endif /* INTEGERCONSTANTEXPRESSION_H_ */
