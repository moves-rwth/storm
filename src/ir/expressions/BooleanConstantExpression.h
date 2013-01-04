/*
 * BooleanConstantExpression.h
 *
 *  Created on: 04.01.2013
 *      Author: chris
 */

#ifndef BOOLEANCONSTANTEXPRESSION_H_
#define BOOLEANCONSTANTEXPRESSION_H_

#include "ConstantExpression.h"

namespace storm {

namespace ir {

namespace expressions {

class BooleanConstantExpression : public ConstantExpression {
public:
	BooleanConstantExpression(std::string constantName) : ConstantExpression(constantName) {
		defined = false;
		value = false;
	}

	virtual ~BooleanConstantExpression() {

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

	bool getValue() {
		return value;
	}

	void define(bool value) {
		defined = true;
		this->value = value;
	}

	bool value;
	bool defined;
};

}

}

}

BOOST_FUSION_ADAPT_STRUCT(
    storm::ir::expressions::BooleanConstantExpression,
    (std::string, constantName)
)

#endif /* BOOLEANCONSTANTEXPRESSION_H_ */
