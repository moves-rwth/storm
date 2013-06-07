/*
 * StateReward.cpp
 *
 *  Created on: 12.01.2013
 *      Author: Christian Dehnert
 */

#include <sstream>

#include "StateReward.h"

namespace storm {

namespace ir {

StateReward::StateReward() : statePredicate(), rewardValue() {
	// Nothing to do here.
}

StateReward::StateReward(std::shared_ptr<storm::ir::expressions::BaseExpression> statePredicate, std::shared_ptr<storm::ir::expressions::BaseExpression> rewardValue) : statePredicate(statePredicate), rewardValue(rewardValue) {
	// Nothing to do here.
}

std::string StateReward::toString() const {
	std::stringstream result;
	result << "\t" << statePredicate->toString() << ": " << rewardValue->toString() << ";";
	return result.str();
}

} // namespace ir

} // namespace storm
