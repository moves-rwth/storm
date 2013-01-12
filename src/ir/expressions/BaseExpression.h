/*
 * Expression.h
 *
 *  Created on: 03.01.2013
 *      Author: chris
 */

#ifndef EXPRESSION_H_
#define EXPRESSION_H_

#include <string>

namespace storm {

namespace ir {

namespace expressions {

class BaseExpression {

public:
	virtual ~BaseExpression() {

	}

	virtual std::string toString() const {
		return "expr here!";
	}
};

}

}

}

#endif /* EXPRESSION_H_ */
