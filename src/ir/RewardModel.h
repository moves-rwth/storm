/*
 * RewardModel.h
 *
 *  Created on: 04.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_REWARDMODEL_H_
#define STORM_IR_REWARDMODEL_H_

#include "StateReward.h"
#include "TransitionReward.h"

#include <string>
#include <vector>

namespace storm {

namespace ir {

/*!
 * A class representing a reward model.
 */
class RewardModel {
public:
	/*!
	 * Default constructor. Creates an empty reward model.
	 */
	RewardModel();

	/*!
	 * Creates a reward module with the given name, state and transition rewards.
	 * @param rewardModelName the name of the reward model.
	 * @param stateRewards A vector of state-based reward.
	 * @param transitionRewards A vector of transition-based reward.
	 */
	RewardModel(std::string rewardModelName, std::vector<std::shared_ptr<storm::ir::StateReward>> stateRewards, std::vector<std::shared_ptr<storm::ir::TransitionReward>> transitionRewards);

	/*!
	 * Retrieves a string representation of this variable.
	 * @returns a string representation of this variable.
	 */
	std::string toString() const;

	/*!
	 * Check, if there are any state rewards.
	 * @return True, iff there are any state rewards.
	 */
	bool hasStateRewards() const;

	/*!
	 * Retrieve state rewards.
	 * @return State rewards.
	 */
	std::vector<std::shared_ptr<storm::ir::StateReward>> getStateRewards() const;

	/*!
	 * Check, if there are any transition rewards.
	 * @return True, iff there are any transition rewards.
	 */
	bool hasTransitionRewards() const;

	/*!
	 * Retrieve transition rewards.
	 * @return Transition rewards.
	 */
	std::vector<std::shared_ptr<storm::ir::TransitionReward>> getTransitionRewards() const;

private:
	// The name of the reward model.
	std::string rewardModelName;

	// The state-based rewards associated with this reward model.
	std::vector<std::shared_ptr<storm::ir::StateReward>> stateRewards;

	// The transition-based rewards associated with this reward model.
	std::vector<std::shared_ptr<storm::ir::TransitionReward>> transitionRewards;
};

} // namespace ir

} // namespace storm

#endif /* STORM_IR_REWARDMODEL_H_ */
