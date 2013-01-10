/*
 * StateReward.h
 *
 *  Created on: Jan 10, 2013
 *      Author: chris
 */

#ifndef STATEREWARD_H_
#define STATEREWARD_H_

#include "expressions/BaseExpression.h"

namespace storm {

namespace ir {

class StateReward {
public:
	StateReward() : statePredicate(nullptr), rewardValue(nullptr) {

	}

	StateReward(std::shared_ptr<storm::ir::expressions::BaseExpression> statePredicate, std::shared_ptr<storm::ir::expressions::BaseExpression> rewardValue) : statePredicate(statePredicate), rewardValue(rewardValue) {

	}

	std::string toString() {
		return statePredicate->toString() + ": " + rewardValue->toString() + ";";
	}

private:
	std::shared_ptr<storm::ir::expressions::BaseExpression> statePredicate;
	std::shared_ptr<storm::ir::expressions::BaseExpression> rewardValue;
};

}

}

#endif /* STATEREWARD_H_ */
