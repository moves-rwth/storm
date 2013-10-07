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
        
        TransitionReward::TransitionReward(std::string const& commandName, std::unique_ptr<storm::ir::expressions::BaseExpression>&& statePredicate, std::unique_ptr<storm::ir::expressions::BaseExpression>&& rewardValue) : commandName(commandName), statePredicate(std::move(statePredicate)), rewardValue(std::move(rewardValue)) {
            // Nothing to do here.
        }
        
        TransitionReward::TransitionReward(TransitionReward const& otherReward) : commandName(otherReward.commandName), statePredicate(), rewardValue() {
            if (otherReward.statePredicate != nullptr) {
                statePredicate = otherReward.statePredicate->clone();
            }
            if (otherReward.rewardValue != nullptr) {
                rewardValue = otherReward.rewardValue->clone();
            }
        }
        
        TransitionReward& TransitionReward::operator=(TransitionReward const& otherReward) {
            if (this != &otherReward) {
                this->commandName = otherReward.commandName;
                if (otherReward.statePredicate != nullptr) {
                    this->statePredicate = otherReward.statePredicate->clone();
                }
                if (otherReward.rewardValue != nullptr) {
                    this->rewardValue = otherReward.rewardValue->clone();
                }
            }
            
            return *this;
        }
        
        std::string TransitionReward::toString() const {
            std::stringstream result;
            result << "\t[" << commandName << "] " << statePredicate->toString() << ": " << rewardValue->toString() << ";";
            return result.str();
        }
        
        std::string const& TransitionReward::getActionName() const {
            return this->commandName;
        }
        
        std::unique_ptr<storm::ir::expressions::BaseExpression> const& TransitionReward::getStatePredicate() const {
            return this->statePredicate;
        }
        
        std::unique_ptr<storm::ir::expressions::BaseExpression> const& TransitionReward::getRewardValue() const {
            return this->rewardValue;
        }
        
    } // namespace ir
} // namespace storm
