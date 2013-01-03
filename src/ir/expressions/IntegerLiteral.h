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
	int value;

	IntegerLiteral(int value) {
		this->value = value;
	}

	virtual ~IntegerLiteral() {

	}

	virtual std::string toString() const {
		return boost::lexical_cast<std::string>(value);
	}
};

}

}

}

BOOST_FUSION_ADAPT_STRUCT(
    storm::ir::expressions::IntegerLiteral,
    (int, value)
)

#endif /* INTEGERLITERAL_H_ */
