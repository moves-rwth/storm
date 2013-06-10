/*
 * RewardModel.h
 *
 *  Created on: 04.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_REWARDMODEL_H_
#define STORM_IR_REWARDMODEL_H_

#include <string>
#include <vector>

#include "StateReward.h"
#include "TransitionReward.h"

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
             *
             * @param rewardModelName The name of the reward model.
             * @param stateRewards A vector of state-based rewards.
             * @param transitionRewards A vector of transition-based rewards.
             */
            RewardModel(std::string const& rewardModelName, std::vector<storm::ir::StateReward> const& stateRewards, std::vector<storm::ir::TransitionReward> const& transitionRewards);
            
            /*!
             * Retrieves a string representation of this reward model.
             *
             * @return a string representation of this reward model.
             */
            std::string toString() const;
            
            /*!
             * Check, if there are any state rewards.
             *
             * @return True, iff there are any state rewards.
             */
            bool hasStateRewards() const;
            
            /*!
             * Retrieves a vector of state rewards associated with this reward model.
             *
             * @return A vector containing the state rewards associated with this reward model.
             */
            std::vector<storm::ir::StateReward> const& getStateRewards() const;
            
            /*!
             * Check, if there are any transition rewards.
             *
             * @return True, iff there are any transition rewards associated with this reward model.
             */
            bool hasTransitionRewards() const;
            
            /*!
             * Retrieves a vector of transition rewards associated with this reward model.
             *
             * @return A vector of transition rewards associated with this reward model.
             */
            std::vector<storm::ir::TransitionReward> const& getTransitionRewards() const;
            
        private:
            // The name of the reward model.
            std::string rewardModelName;
            
            // The state-based rewards associated with this reward model.
            std::vector<storm::ir::StateReward> stateRewards;
            
            // The transition-based rewards associated with this reward model.
            std::vector<storm::ir::TransitionReward> transitionRewards;
        };
        
    } // namespace ir
} // namespace storm

#endif /* STORM_IR_REWARDMODEL_H_ */
