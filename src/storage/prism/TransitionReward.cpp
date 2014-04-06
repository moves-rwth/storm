/*
 * TransitionReward.cpp
 *
 *  Created on: 12.01.2013
 *      Author: Christian Dehnert
 */

#include <sstream>

#include "TransitionReward.h"

namespace storm {
    namespace ir {
        
        TransitionReward::TransitionReward() : commandName(), statePredicate(), rewardValue() {
            // Nothing to do here.
        }
        
        TransitionReward::TransitionReward(std::string const& commandName, storm::expressions::Expression const& statePredicate, storm::expressions::Expression const& rewardValue) : commandName(commandName), statePredicate(statePredicate), rewardValue(rewardValue) {
            // Nothing to do here.
        }
        
        std::string TransitionReward::toString() const {
            std::stringstream result;
            result << "\t[" << commandName << "] " << statePredicate << ": " << rewardValue << ";";
            return result.str();
        }
        
        std::string const& TransitionReward::getActionName() const {
            return this->commandName;
        }
        
        storm::expressions::Expression const& TransitionReward::getStatePredicate() const {
            return this->statePredicate;
        }
        
        storm::expressions::Expression const& TransitionReward::getRewardValue() const {
            return this->rewardValue;
        }
        
    } // namespace ir
} // namespace storm
