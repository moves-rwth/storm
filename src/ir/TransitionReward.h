/*
 * TransitionReward.h
 *
 *  Created on: Jan 10, 2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_TRANSITIONREWARD_H_
#define STORM_IR_TRANSITIONREWARD_H_

#include <memory>

#include "expressions/BaseExpression.h"

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
	TransitionReward(std::string const& commandName, std::shared_ptr<storm::ir::expressions::BaseExpression> const& statePredicate, std::shared_ptr<storm::ir::expressions::BaseExpression> const& rewardValue);

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
    std::shared_ptr<storm::ir::expressions::BaseExpression> getStatePredicate() const;
    
    /*!
     * Retrieves the reward value associated with this state reward.
     *
     * @return The reward value associated with this state reward.
     */
    std::shared_ptr<storm::ir::expressions::BaseExpression> getRewardValue() const;

private:
	// The name of the command this transition-based reward is attached to.
	std::string commandName;

	// A predicate that needs to be satisfied by states for the reward to be obtained (by taking
	// a corresponding command transition).
	std::shared_ptr<storm::ir::expressions::BaseExpression> statePredicate;

	// The expression specifying the value of the reward obtained along the transitions.
	std::shared_ptr<storm::ir::expressions::BaseExpression> rewardValue;
};

} // namespace ir

} // namespace storm

#endif /* STORM_IR_TRANSITIONREWARD_H_ */
