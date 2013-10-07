/*
 * StateReward.h
 *
 *  Created on: Jan 10, 2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_STATEREWARD_H_
#define STORM_IR_STATEREWARD_H_

#include <memory>

#include "expressions/BaseExpression.h"

namespace storm {
    namespace ir {
        
        /*!
         * A class representing a state reward.
         */
        class StateReward {
        public:
            /*!
             * Default constructor. Creates an empty state reward.
             */
            StateReward();
            
            /*!
             * Creates a state reward for the states satisfying the given expression with the value given
             * by a second expression.
             *
             * @param statePredicate The predicate that states earning this state-based reward need to
             * satisfy.
             * @param rewardValue An expression specifying the values of the rewards to attach to the
             * states.
             */
            StateReward(std::unique_ptr<storm::ir::expressions::BaseExpression>&& statePredicate, std::unique_ptr<storm::ir::expressions::BaseExpression>&& rewardValue);
            
            /*!
             * Performs a deep-copy of the given reward.
             *
             * @param otherReward The reward to copy.
             */
            StateReward(StateReward const& otherReward);

            /*!
             * Performs a deep-copy of the given reward and assigns it to the current one.
             *
             * @param otherReward The reward to assign.
             */
            StateReward& operator=(StateReward const& otherReward);
            
            /*!
             * Retrieves a string representation of this state reward.
             *
             * @return A string representation of this state reward.
             */
            std::string toString() const;
            
            /*!
             * Retrieves the state predicate that is associated with this state reward.
             *
             * @return The state predicate that is associated with this state reward.
             */
            std::unique_ptr<storm::ir::expressions::BaseExpression> const& getStatePredicate() const;
            
            /*!
             * Retrieves the reward value associated with this state reward.
             *
             * @return The reward value associated with this state reward.
             */
            std::unique_ptr<storm::ir::expressions::BaseExpression> const& getRewardValue() const;
            
        private:
            // The predicate that characterizes the states that obtain this reward.
            std::unique_ptr<storm::ir::expressions::BaseExpression> statePredicate;
            
            // The expression that specifies the value of the reward obtained.
            std::unique_ptr<storm::ir::expressions::BaseExpression> rewardValue;
        };
        
    } // namespace ir
} // namespace storm

#endif /* STORM_IR_STATEREWARD_H_ */
