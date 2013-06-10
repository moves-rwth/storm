/*
 * RewardModel.cpp
 *
 *  Created on: 12.01.2013
 *      Author: Christian Dehnert
 */

#include <sstream>

#include "RewardModel.h"

namespace storm {
    namespace ir {
        
        RewardModel::RewardModel() : rewardModelName(), stateRewards(), transitionRewards() {
            // Nothing to do here.
        }
        
        RewardModel::RewardModel(std::string const& rewardModelName, std::vector<storm::ir::StateReward> const& stateRewards, std::vector<storm::ir::TransitionReward> const& transitionRewards) : rewardModelName(rewardModelName), stateRewards(stateRewards), transitionRewards(transitionRewards) {
            // Nothing to do here.
        }
        
        std::string RewardModel::toString() const {
            std::stringstream result;
            result << "rewards \"" << rewardModelName << "\"" << std::endl;
            for (auto const& reward : stateRewards) {
                result << reward.toString() << std::endl;
            }
            for (auto const& reward : transitionRewards) {
                result << reward.toString() << std::endl;
            }
            result << "endrewards" << std::endl;
            return result.str();
        }
        
        bool RewardModel::hasStateRewards() const {
            return this->stateRewards.size() > 0;
        }
        
        std::vector<storm::ir::StateReward> const& RewardModel::getStateRewards() const {
            return this->stateRewards;
        }
        
        bool RewardModel::hasTransitionRewards() const {
            return this->transitionRewards.size() > 0;
        }
        
        std::vector<storm::ir::TransitionReward> const& RewardModel::getTransitionRewards() const {
            return this->transitionRewards;
        }
        
    } // namespace ir
} // namespace storm
