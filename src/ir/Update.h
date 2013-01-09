/*
 * Update.h
 *
 *  Created on: 06.01.2013
 *      Author: chris
 */

#ifndef UPDATE_H_
#define UPDATE_H_

#include "IR.h"

namespace storm {

namespace ir {

class Update {
public:
	Update() : likelihoodExpression(nullptr), assignments() {

	}

	Update(std::shared_ptr<storm::ir::expressions::BaseExpression> likelihoodExpression, std::vector<storm::ir::Assignment> assignments)
		: likelihoodExpression(likelihoodExpression), assignments(assignments) {

	}

	std::string toString() {
		std::string result = likelihoodExpression->toString() + " : ";
		for (uint_fast64_t i = 0; i < assignments.size(); ++i) {
			result += assignments[i].toString();
			if (i < assignments.size() - 1) {
				result += " & ";
			}
		}
		return result;
	}

private:
	std::shared_ptr<storm::ir::expressions::BaseExpression> likelihoodExpression;
	std::vector<storm::ir::Assignment> assignments;
};

}

}

#endif /* UPDATE_H_ */
