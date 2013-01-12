/*
 * StateReward.cpp
 *
 *  Created on: 12.01.2013
 *      Author: Christian Dehnert
 */

#include "StateReward.h"

#include <sstream>

namespace storm {

namespace ir {

// Initializes all members with their default constructors.
StateReward::StateReward() : statePredicate(), rewardValue() {
	// Nothing to do here.
}

// Initializes all members according to the given values.
StateReward::StateReward(std::shared_ptr<storm::ir::expressions::BaseExpression> statePredicate, std::shared_ptr<storm::ir::expressions::BaseExpression> rewardValue) : statePredicate(statePredicate), rewardValue(rewardValue) {
	// Nothing to do here.
}

// Build a string representation of the state reward.
std::string StateReward::toString() const {
	std::stringstream result;
	result << "\t" << statePredicate->toString() << ": " << rewardValue->toString() << ";";
	return result.str();
}

} // namespace ir

} // namespace storm
