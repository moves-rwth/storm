/*
 * IntegerLiteral.h
 *
 *  Created on: 03.01.2013
 *      Author: chris
 */

#ifndef INTEGERLITERAL_H_
#define INTEGERLITERAL_H_

#include "src/ir/expressions/BaseExpression.h"

namespace storm {

namespace ir {

namespace expressions {

class IntegerLiteral : public BaseExpression {
public:
	int_fast64_t value;

	IntegerLiteral(int_fast64_t value) : BaseExpression(int_), value(value) {

	}

	virtual ~IntegerLiteral() {

	}

	virtual int_fast64_t getValueAsInt(std::vector<bool> const& booleanVariableValues, std::vector<int_fast64_t> const& integerVariableValues) const {
		return value;
	}

	virtual std::string toString() const {
		return boost::lexical_cast<std::string>(value);
	}
};

}

}

}

#endif /* INTEGERLITERAL_H_ */
