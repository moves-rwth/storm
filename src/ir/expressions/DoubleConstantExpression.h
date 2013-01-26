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
	DoubleConstantExpression(std::string constantName) : ConstantExpression(double_, constantName), defined(false), value(0) {

	}

	virtual ~DoubleConstantExpression() {

	}

	virtual double getValueAsDouble(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
		if (!defined) {
			throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
					<< "Double constant '" << this->getConstantName() << "' is undefined.";
		} else {
			return value;
		}
	}

	virtual ADD* toAdd() const {
		if (!defined) {
			throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
					<< "Double constant '" << this->getConstantName() << "' is undefined.";
		}

		storm::utility::CuddUtility* cuddUtility = storm::utility::cuddUtilityInstance();
		return new ADD(*cuddUtility->getConstant(value));
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

private:
	bool defined;
	double value;
};

}

}

}

#endif /* DOUBLECONSTANTEXPRESSION_H_ */
