/*
 * TransitionReward.cpp
 *
 *  Created on: 12.01.2013
 *      Author: Christian Dehnert
 */

#include "TransitionReward.h"

#include <sstream>

namespace storm {

namespace ir {

// Initializes all members with their default constructors.
TransitionReward::TransitionReward() : commandName(), statePredicate(), rewardValue() {
	// Nothing to do here.
}

// Initializes all members according to the given values.
TransitionReward::TransitionReward(std::string commandName, std::shared_ptr<storm::ir::expressions::BaseExpression> statePredicate, std::shared_ptr<storm::ir::expressions::BaseExpression> rewardValue) : commandName(commandName), statePredicate(statePredicate), rewardValue(rewardValue) {
	// Nothing to do here.
}

// Build a string representation of the transition reward.
std::string TransitionReward::toString() const {
	std::stringstream result;
	result << "\t[" << commandName << "] " << statePredicate->toString() << ": " << rewardValue->toString() << ";";
	return result.str();
}

double TransitionReward::getReward(std::string const & label, std::pair<std::vector<bool>, std::vector<int_fast64_t>> const * state) const {
	if (this->commandName != label) return 0;
	if (this->statePredicate->getValueAsBool(state)) {
		return this->rewardValue->getValueAsDouble(state);
	}
	return 0;
}

} // namespace ir

} // namespace storm
