/*
 * ConstantExpression.h
 *
 *  Created on: 03.01.2013
 *      Author: chris
 */

#ifndef CONSTANTEXPRESSION_H_
#define CONSTANTEXPRESSION_H_

#include "src/ir/expressions/BaseExpression.h"

namespace storm {

namespace ir {

namespace expressions {

class ConstantExpression : public BaseExpression {
public:
	std::string constantName;

	ConstantExpression(ReturnType type, std::string constantName) : BaseExpression(type), constantName(constantName) {
	}
	ConstantExpression(const ConstantExpression& ce)
		: BaseExpression(ce), constantName(ce.constantName) {

	}

	virtual ~ConstantExpression() {

	}

	std::string const& getConstantName() const {
		return constantName;
	}

	virtual std::string toString() const {
		return constantName;
	}
	
	virtual std::string dump(std::string prefix) const {
		std::stringstream result;
		result << prefix << "ConstantExpression " << this->toString() << std::endl;
		return result.str();
	}
};

}

}

}

#endif /* CONSTANTEXPRESSION_H_ */
