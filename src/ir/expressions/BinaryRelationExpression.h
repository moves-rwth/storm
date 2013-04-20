/*
 * BinaryRelationExpression.h
 *
 *  Created on: 03.01.2013
 *      Author: chris
 */

#ifndef BINARYRELATIONEXPRESSION_H_
#define BINARYRELATIONEXPRESSION_H_

#include "src/ir/expressions/BinaryExpression.h"
#include <iostream>

namespace storm {

namespace ir {

namespace expressions {

class BinaryRelationExpression : public BinaryExpression {
public:
	enum RelationType {EQUAL, NOT_EQUAL, LESS, LESS_OR_EQUAL, GREATER, GREATER_OR_EQUAL};

	BinaryRelationExpression(std::shared_ptr<BaseExpression> left, std::shared_ptr<BaseExpression> right, RelationType relationType) : BinaryExpression(bool_, left, right), relationType(relationType) {
		std::cerr << "BinaryRelationExpression: " << left.get() << " " << relationType << " " << right.get() << " ?" << std::endl;
	}

	virtual ~BinaryRelationExpression() {

	}

	virtual std::shared_ptr<BaseExpression> clone(const std::map<std::string, std::string>& renaming, const std::map<std::string, uint_fast64_t>& bools, const std::map<std::string, uint_fast64_t>& ints) {
		auto res = std::shared_ptr<BaseExpression>(new BinaryRelationExpression(this->getLeft()->clone(renaming, bools, ints), this->getRight()->clone(renaming, bools, ints), this->relationType));
		std::cout << "Cloning " << this->toString() << " to " << res->toString() << std::endl;
		return res;
	}

	virtual bool getValueAsBool(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
		int_fast64_t resultLeft = this->getLeft()->getValueAsInt(variableValues);
		int_fast64_t resultRight = this->getRight()->getValueAsInt(variableValues);
		switch(relationType) {
		case EQUAL: return resultLeft == resultRight; break;
		case NOT_EQUAL: return resultLeft != resultRight; break;
		case LESS: return resultLeft < resultRight; break;
		case LESS_OR_EQUAL: return resultLeft <= resultRight; break;
		case GREATER: return resultLeft > resultRight; break;
		case GREATER_OR_EQUAL: return resultLeft >= resultRight; break;
		default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
				<< "Unknown boolean binary relation: '" << relationType << "'.";
		}
	}

	RelationType getRelationType() const {
		return relationType;
	}

	virtual void accept(ExpressionVisitor* visitor) {
		visitor->visit(this);
	}

	virtual std::string toString() const {
		std::string result = this->getLeft()->toString();
		switch (relationType) {
		case EQUAL: result += " = "; break;
		case NOT_EQUAL: result += " != "; break;
		case LESS: result += " < "; break;
		case LESS_OR_EQUAL: result += " <= "; break;
		case GREATER: result += " > "; break;
		case GREATER_OR_EQUAL: result += " >= "; break;
		}
		result += this->getRight()->toString();

		return result;
	}
	virtual std::string dump(std::string prefix) const {
        std::stringstream result;
        result << prefix << "BinaryRelationExpression" << std::endl;
        result << this->getLeft()->dump(prefix + "\t");
        switch (relationType) {
        case EQUAL: result << prefix << "=" << std::endl; break;
        case NOT_EQUAL: result << prefix << "!=" << std::endl; break;
        case LESS: result << prefix << "<" << std::endl; break;
        case LESS_OR_EQUAL: result << prefix << "<=" << std::endl; break;
        case GREATER: result << prefix << ">" << std::endl; break;
        case GREATER_OR_EQUAL: result << prefix << ">=" << std::endl; break;
        }
        result << this->getRight()->dump(prefix + "\t");
        return result.str();
    }

private:
	RelationType relationType;
};

}

}

}

#endif /* BINARYRELATIONEXPRESSION_H_ */
