/*
 * TransitionReward.h
 *
 *  Created on: Jan 10, 2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_TRANSITIONREWARD_H_
#define STORM_IR_TRANSITIONREWARD_H_

#include <memory>

#include "src/storage/expressions/Expression.h"

namespace storm {

namespace ir {

/*!
 * A class representing a transition reward.
 */
class TransitionReward {
public:
	/*!
	 * Default constructor. Creates an empty transition reward.
	 */
	TransitionReward();

	/*!
	 * Creates a transition reward for the transitions with the given name emanating from states
	 * satisfying the given expression with the value given by another expression.
     *
	 * @param commandName The name of the command that obtains this reward.
	 * @param statePredicate The predicate that needs to hold before taking a transition with the
	 * previously specified name in order to obtain the reward.
	 * @param rewardValue An expression specifying the values of the rewards to attach to the
	 * transitions.
	 */
	TransitionReward(std::string const& commandName, storm::expressions::Expression const& statePredicate, storm::expressions::Expression const& rewardValue);

    /*!
     * Performs a deep-copy of the given transition reward.
     *
     * @param otherReward The transition reward to copy.
     */
    TransitionReward(TransitionReward const& otherReward) = default;
    
    /*!
     * Performs a deep-copy of the given transition reward and assigns it to the current one.
     *
     * @param otherReward The reward to assign.
     */
    TransitionReward& operator=(TransitionReward const& otherReward) = default;
    
	/*!
	 * Retrieves a string representation of this transition reward.
     *
	 * @return A string representation of this transition reward.
	 */
	std::string toString() const;
    
    /*!
     * Retrieves the action name that is associated with this transition reward.
     *
     * @return The action name that is associated with this transition reward.
     */
    std::string const& getActionName() const;
    
    /*!
     * Retrieves the state predicate that is associated with this state reward.
     *
     * @return The state predicate that is associated with this state reward.
     */
    storm::expressions::Expression const& getStatePredicate() const;
    
    /*!
     * Retrieves the reward value associated with this state reward.
     *
     * @return The reward value associated with this state reward.
     */
    storm::expressions::Expression const& getRewardValue() const;

private:
	// The name of the command this transition-based reward is attached to.
	std::string commandName;

	// A predicate that needs to be satisfied by states for the reward to be obtained (by taking
	// a corresponding command transition).
	storm::expressions::Expression statePredicate;

	// The expression specifying the value of the reward obtained along the transitions.
	storm::expressions::Expression rewardValue;
};

} // namespace ir

} // namespace storm

#endif /* STORM_IR_TRANSITIONREWARD_H_ */
