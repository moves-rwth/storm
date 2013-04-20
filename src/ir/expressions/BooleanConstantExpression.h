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

	virtual std::shared_ptr<BaseExpression> clone(const std::map<std::string, std::string>& renaming, const std::map<std::string, uint_fast64_t>& bools, const std::map<std::string, uint_fast64_t>& ints) {
		return std::shared_ptr<BaseExpression>(this);
	}

	virtual bool getValueAsBool(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
		if (!defined) {
			throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
					<< "Boolean constant '" << this->getConstantName() << "' is undefined.";
		} else {
			return value;
		}
	}

	virtual void accept(ExpressionVisitor* visitor) {
		visitor->visit(this);
	}

	virtual std::string toString() const {
		std::string result = this->constantName;
		if (defined) {
			result += "[" + boost::lexical_cast<std::string>(value) + "]";
		}
		return result;
	}
	
	virtual std::string dump(std::string prefix) const {
		std::stringstream result;
		result << prefix << "BooleanConstantExpression " << this->toString() << std::endl;
		return result.str();
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
