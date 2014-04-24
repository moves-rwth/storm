#include "src/storage/prism/TransitionReward.h"

namespace storm {
    namespace prism {
        TransitionReward::TransitionReward(std::string const& commandName, storm::expressions::Expression const& statePredicateExpression, storm::expressions::Expression const& rewardValueExpression, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), commandName(commandName), statePredicateExpression(statePredicateExpression), rewardValueExpression(rewardValueExpression) {
            // Nothing to do here.
        }
        
        std::string const& TransitionReward::getActionName() const {
            return this->commandName;
        }
        
        storm::expressions::Expression const& TransitionReward::getStatePredicateExpression() const {
            return this->statePredicateExpression;
        }
        
        storm::expressions::Expression const& TransitionReward::getRewardValueExpression() const {
            return this->rewardValueExpression;
        }
        
        TransitionReward TransitionReward::substitute(std::map<std::string, storm::expressions::Expression> const& substitution) const {
            return TransitionReward(this->getActionName(), this->getStatePredicateExpression().substitute(substitution), this->getRewardValueExpression().substitute(substitution), this->getFilename(), this->getLineNumber());
        }
        
        std::ostream& operator<<(std::ostream& stream, TransitionReward const& transitionReward) {
            stream << "\t[" << transitionReward.getActionName() << "] " << transitionReward.getStatePredicateExpression() << ": " << transitionReward.getRewardValueExpression() << ";";
            return stream;
        }
        
    } // namespace prism
} // namespace storm
