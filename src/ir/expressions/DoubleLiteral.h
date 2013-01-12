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

	DoubleLiteral(double value) {
		this->value = value;
	}

	virtual ~DoubleLiteral() {

	}

	virtual std::string toString() const {
		return boost::lexical_cast<std::string>(value);
	}
};

}

}

}

#endif /* DOUBLELITERAL_H_ */
