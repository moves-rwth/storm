/*
 * BooleanLiteral.h
 *
 *  Created on: 03.01.2013
 *      Author: chris
 */

#ifndef BOOLEANLITERAL_H_
#define BOOLEANLITERAL_H_

#include "src/ir/expressions/BaseExpression.h"

namespace storm {

namespace ir {

namespace expressions {

class BooleanLiteral : public BaseExpression {
public:
	bool value;

	BooleanLiteral(bool value) : BaseExpression(bool_), value(value) {

	}

	virtual ~BooleanLiteral() {

	}

	virtual bool getValueAsBool(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
		return value;
	}

	virtual void accept(ExpressionVisitor* visitor) {
		visitor->visit(this);
	}

	virtual std::string toString() const {
		if (value) {
			return std::string("true");
		} else {
			return std::string("false");
		}
	}
};

}

}

}

#endif /* BOOLEANLITERAL_H_ */
