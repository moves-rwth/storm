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
        
        StateReward::StateReward(std::unique_ptr<storm::ir::expressions::BaseExpression>&& statePredicate, std::unique_ptr<storm::ir::expressions::BaseExpression>&& rewardValue) : statePredicate(std::move(statePredicate)), rewardValue(std::move(rewardValue)) {
            // Nothing to do here.
        }
        
        StateReward::StateReward(StateReward const& otherReward) {
            if (otherReward.statePredicate != nullptr) {
                statePredicate = otherReward.statePredicate->clone();
            }
            if (otherReward.rewardValue != nullptr) {
                rewardValue = otherReward.rewardValue->clone();
            }
        }
        
        StateReward& StateReward::operator=(StateReward const& otherReward) {
            if (this != & otherReward) {
                if (otherReward.statePredicate != nullptr) {
                    this->statePredicate = otherReward.statePredicate->clone();
                }
                if (otherReward.rewardValue != nullptr) {
                    this->rewardValue = otherReward.rewardValue->clone();
                }
            }
            
            return *this;
        }
        
        std::string StateReward::toString() const {
            std::stringstream result;
            result << "\t" << statePredicate->toString() << ": " << rewardValue->toString() << ";";
            return result.str();
        }
        
        std::unique_ptr<storm::ir::expressions::BaseExpression> const& StateReward::getStatePredicate() const {
            return this->statePredicate;
        }
        
        std::unique_ptr<storm::ir::expressions::BaseExpression> const& StateReward::getRewardValue() const {
            return this->rewardValue;
        }
        
    } // namespace ir
} // namespace storm
