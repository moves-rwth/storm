/*
 * RewardModel.cpp
 *
 *  Created on: 12.01.2013
 *      Author: Christian Dehnert
 */

#include "RewardModel.h"

#include <sstream>

namespace storm {

namespace ir {

// Initializes all members with their default constructors.
RewardModel::RewardModel() : rewardModelName(), stateRewards(), transitionRewards() {
	// Nothing to do here.
}

// Initializes all members according to the given values.
RewardModel::RewardModel(std::string rewardModelName, std::vector<std::shared_ptr<storm::ir::StateReward>> stateRewards, std::vector<std::shared_ptr<storm::ir::TransitionReward>> transitionRewards) : rewardModelName(rewardModelName), stateRewards(stateRewards), transitionRewards(transitionRewards) {
	// Nothing to do here.
}

// Build a string representation of the reward model.
std::string RewardModel::toString() const {
	std::stringstream result;
	result << "rewards \"" << rewardModelName << "\"" << std::endl;
	for (auto reward : stateRewards) {
		result << reward->toString() << std::endl;
	}
	for (auto reward : transitionRewards) {
		result << reward->toString() << std::endl;
	}
	result << "endrewards" << std::endl;
	return result.str();
}

bool RewardModel::hasStateRewards() const {
	return this->stateRewards.size() > 0;
}

std::vector<std::shared_ptr<storm::ir::StateReward>> RewardModel::getStateRewards() const {
	return this->stateRewards;
}

bool RewardModel::hasTransitionRewards() const {
	return this->transitionRewards.size() > 0;
}

std::vector<std::shared_ptr<storm::ir::TransitionReward>> RewardModel::getTransitionRewards() const {
	return this->transitionRewards;
}

} // namespace ir

} // namespace storm
