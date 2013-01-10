/*
 * TransitionReward.h
 *
 *  Created on: Jan 10, 2013
 *      Author: chris
 */

#ifndef TRANSITIONREWARD_H_
#define TRANSITIONREWARD_H_

#include "expressions/BaseExpression.h"

namespace storm {

namespace ir {

class TransitionReward {
public:
	TransitionReward() : commandName(""), statePredicate(nullptr), rewardValue(nullptr) {

	}

	TransitionReward(std::string commandName, std::shared_ptr<storm::ir::expressions::BaseExpression> statePredicate, std::shared_ptr<storm::ir::expressions::BaseExpression> rewardValue) : commandName(commandName), statePredicate(statePredicate), rewardValue(rewardValue) {

	}

	std::string toString() {
		return "[" + commandName + "] " + statePredicate->toString() + ": " + rewardValue->toString() + ";";
	}

private:
	std::string commandName;
	std::shared_ptr<storm::ir::expressions::BaseExpression> statePredicate;
	std::shared_ptr<storm::ir::expressions::BaseExpression> rewardValue;
};

}

}

#endif /* TRANSITIONREWARD_H_ */
