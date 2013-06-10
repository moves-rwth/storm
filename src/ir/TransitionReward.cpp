/*
 * TransitionReward.cpp
 *
 *  Created on: 12.01.2013
 *      Author: Christian Dehnert
 */

#include <sstream>

#include "TransitionReward.h"

namespace storm {

namespace ir {

TransitionReward::TransitionReward() : commandName(), statePredicate(), rewardValue() {
	// Nothing to do here.
}

TransitionReward::TransitionReward(std::string const& commandName, std::shared_ptr<storm::ir::expressions::BaseExpression> const& statePredicate, std::shared_ptr<storm::ir::expressions::BaseExpression> const& rewardValue) : commandName(commandName), statePredicate(statePredicate), rewardValue(rewardValue) {
	// Nothing to do here.
}

std::string TransitionReward::toString() const {
	std::stringstream result;
	result << "\t[" << commandName << "] " << statePredicate->toString() << ": " << rewardValue->toString() << ";";
	return result.str();
}

std::string const& TransitionReward::getActionName() const {
    return this->commandName;
}
    
std::shared_ptr<storm::ir::expressions::BaseExpression> TransitionReward::getStatePredicate() const {
    return this->statePredicate;
}
    
std::shared_ptr<storm::ir::expressions::BaseExpression> TransitionReward::getRewardValue() const {
    return this->rewardValue;
}
    
} // namespace ir

} // namespace storm
