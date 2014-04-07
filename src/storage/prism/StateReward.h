#ifndef STORM_STORAGE_PRISM_STATEREWARD_H_
#define STORM_STORAGE_PRISM_STATEREWARD_H_

#include "src/storage/expressions/Expression.h"

namespace storm {
    namespace prism {
        class StateReward {
        public:
            /*!
             * Creates a state reward for the states satisfying the given expression with the value given by a second
             * expression.
             *
             * @param statePredicateExpression The predicate that states earning this state-based reward need to satisfy.
             * @param rewardValueExpression An expression specifying the values of the rewards to attach to the states.
             */
            StateReward(storm::expressions::Expression const& statePredicateExpression, storm::expressions::Expression const& rewardValueExpression);
            
            // Create default implementations of constructors/assignment.
            StateReward() = default;
            StateReward(StateReward const& otherVariable) = default;
            StateReward& operator=(StateReward const& otherVariable)= default;
            StateReward(StateReward&& otherVariable) = default;
            StateReward& operator=(StateReward&& otherVariable) = default;
            
            /*!
             * Retrieves the state predicate that is associated with this state reward.
             *
             * @return The state predicate that is associated with this state reward.
             */
            storm::expressions::Expression const& getStatePredicateExpression() const;
            
            /*!
             * Retrieves the reward value associated with this state reward.
             *
             * @return The reward value associated with this state reward.
             */
            storm::expressions::Expression const& getRewardValueExpression() const;
            
            friend std::ostream& operator<<(std::ostream& stream, StateReward const& stateReward);

        private:
            // The predicate that characterizes the states that obtain this reward.
            storm::expressions::Expression statePredicateExpression;
            
            // The expression that specifies the value of the reward obtained.
            storm::expressions::Expression rewardValueExpression;
        };
        
    } // namespace prism
} // namespace storm

#endif /* STORM_STORAGE_PRISM_STATEREWARD_H_ */
