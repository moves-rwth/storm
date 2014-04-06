/*
 * StateReward.cpp
 *
 *  Created on: 12.01.2013
 *      Author: Christian Dehnert
 */

#include <sstream>

#include "StateReward.h"

namespace storm {
    namespace ir {
        
        StateReward::StateReward() : statePredicate(), rewardValue() {
            // Nothing to do here.
        }
        
        StateReward::StateReward(storm::expressions::Expression const& statePredicate, storm::expressions::Expression const& rewardValue) : statePredicate(statePredicate), rewardValue(rewardValue) {
            // Nothing to do here.
        }
        
        std::string StateReward::toString() const {
            std::stringstream result;
            result << "\t" << statePredicate << ": " << rewardValue << ";";
            return result.str();
        }
        
        storm::expressions::Expression const& StateReward::getStatePredicate() const {
            return this->statePredicate;
        }
        
        storm::expressions::Expression const& StateReward::getRewardValue() const {
            return this->rewardValue;
        }
        
    } // namespace ir
} // namespace storm
