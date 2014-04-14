#include "src/storage/prism/StateReward.h"

namespace storm {
    namespace prism {
        StateReward::StateReward(storm::expressions::Expression const& statePredicateExpression, storm::expressions::Expression const& rewardValueExpression, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), statePredicateExpression(statePredicateExpression), rewardValueExpression(rewardValueExpression) {
            // Nothing to do here.
        }
        
        storm::expressions::Expression const& StateReward::getStatePredicateExpression() const {
            return this->statePredicateExpression;
        }
        
        storm::expressions::Expression const& StateReward::getRewardValueExpression() const {
            return this->rewardValueExpression;
        }
        
        StateReward StateReward::substitute(std::map<std::string, storm::expressions::Expression> const& substitution) const {
            return StateReward(this->getStatePredicateExpression().substitute<std::map>(substitution), this->getRewardValueExpression().substitute<std::map>(substitution), this->getFilename(), this->getLineNumber());
        }
        
        std::ostream& operator<<(std::ostream& stream, StateReward const& stateReward) {
            stream << "\t" << stateReward.getStatePredicateExpression() << ": " << stateReward.getRewardValueExpression() << ";";
            return stream;
        }
    } // namespace prism
} // namespace storm
