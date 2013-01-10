/*
 * RewardModel.h
 *
 *  Created on: 04.01.2013
 *      Author: chris
 */

#ifndef REWARDMODEL_H_
#define REWARDMODEL_H_

namespace storm {

namespace ir {

class RewardModel {
public:
	RewardModel() : rewardModelName(""), stateRewards(), transitionRewards() {

	}

	RewardModel(std::string rewardModelName, std::vector<storm::ir::StateReward> stateRewards, std::vector<storm::ir::TransitionReward> transitionRewards) : rewardModelName(rewardModelName), stateRewards(stateRewards), transitionRewards(transitionRewards) {

	}

	std::string toString() {
		std::string result = "rewards \"" + rewardModelName + "\n";
		for (auto reward : stateRewards) {
			result += reward.toString() + "\n";
		}
		for (auto reward : transitionRewards) {
			result += reward.toString() + "\n";
		}
		result += "endrewards\n";
		return result;
	}

private:
	std::string rewardModelName;
	std::vector<storm::ir::StateReward> stateRewards;
	std::vector<storm::ir::TransitionReward> transitionRewards;
};

}

}

#endif /* REWARDMODEL_H_ */
