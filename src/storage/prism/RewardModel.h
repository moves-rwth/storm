#ifndef STORM_STORAGE_PRISM_REWARDMODEL_H_
#define STORM_STORAGE_PRISM_REWARDMODEL_H_

#include <string>
#include <vector>

#include "src/storage/prism/StateReward.h"
#include "src/storage/prism/TransitionReward.h"

namespace storm {
    namespace prism {
        class RewardModel {
        public:
            /*!
             * Creates a reward model with the given name, state and transition rewards.
             *
             * @param rewardModelName The name of the reward model.
             * @param stateRewards A vector of state-based rewards.
             * @param transitionRewards A vector of transition-based rewards.
             */
            RewardModel(std::string const& rewardModelName, std::vector<storm::prism::StateReward> const& stateRewards, std::vector<storm::prism::TransitionReward> const& transitionRewards);
            
            // Create default implementations of constructors/assignment.
            RewardModel() = default;
            RewardModel(RewardModel const& otherVariable) = default;
            RewardModel& operator=(RewardModel const& otherVariable)= default;
            RewardModel(RewardModel&& otherVariable) = default;
            RewardModel& operator=(RewardModel&& otherVariable) = default;
            
            /*!
             * Retrieves the name of the reward model.
             *
             * @return The name of the reward model.
             */
            std::string const& getName() const;
            
            /*!
             * Retrieves whether there are any state rewards.
             *
             * @return True iff there are any state rewards.
             */
            bool hasStateRewards() const;
            
            /*!
             * Retrieves all state rewards associated with this reward model.
             *
             * @return The state rewards associated with this reward model.
             */
            std::vector<storm::prism::StateReward> const& getStateRewards() const;
            
            /*!
             * Retrieves whether there are any transition rewards.
             *
             * @return True iff there are any transition rewards.
             */
            bool hasTransitionRewards() const;
            
            /*!
             * Retrieves all transition rewards associated with this reward model.
             *
             * @return The transition rewards associated with this reward model.
             */
            std::vector<storm::prism::TransitionReward> const& getTransitionRewards() const;
            
            friend std::ostream& operator<<(std::ostream& stream, RewardModel const& rewardModel);

        private:
            // The name of the reward model.
            std::string rewardModelName;
            
            // The state-based rewards associated with this reward model.
            std::vector<storm::prism::StateReward> stateRewards;
            
            // The transition-based rewards associated with this reward model.
            std::vector<storm::prism::TransitionReward> transitionRewards;
        };
        
    } // namespace prism
} // namespace storm

#endif /* STORM_STORAGE_PRISM_REWARDMODEL_H_ */
