/*
 * BooleanConstantExpression.h
 *
 *  Created on: 04.01.2013
 *      Author: chris
 */

#ifndef BOOLEANCONSTANTEXPRESSION_H_
#define BOOLEANCONSTANTEXPRESSION_H_

#include "ConstantExpression.h"

#include <boost/lexical_cast.hpp>

namespace storm {

namespace ir {

namespace expressions {

class BooleanConstantExpression : public ConstantExpression {
public:
	BooleanConstantExpression(std::string constantName) : ConstantExpression(bool_, constantName) {
		defined = false;
		value = false;
	}

	virtual ~BooleanConstantExpression() {

	}

	virtual bool getValueAsBool(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
		if (!defined) {
			throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
					<< "Boolean constant '" << this->getConstantName() << "' is undefined.";
		} else {
			return value;
		}
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

#endif /* BOOLEANCONSTANTEXPRESSION_H_ */
