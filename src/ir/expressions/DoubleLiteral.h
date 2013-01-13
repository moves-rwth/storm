/*
 * DoubleLiteral.h
 *
 *  Created on: 03.01.2013
 *      Author: chris
 */

#ifndef DOUBLELITERAL_H_
#define DOUBLELITERAL_H_

#include "src/ir/expressions/BaseExpression.h"

#include "boost/lexical_cast.hpp"

namespace storm {

namespace ir {

namespace expressions {

class DoubleLiteral : public BaseExpression {
public:
	double value;

	DoubleLiteral(double value) : BaseExpression(double_), value(value) {

	}

	virtual ~DoubleLiteral() {

	}

	virtual double getValueAsDouble(std::vector<bool> const& booleanVariableValues, std::vector<int_fast64_t> const& integerVariableValues) const {
		return value;
	}

	virtual std::string toString() const {
		return boost::lexical_cast<std::string>(value);
	}
};

}

}

}

#endif /* DOUBLELITERAL_H_ */
