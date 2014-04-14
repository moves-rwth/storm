#include "src/storage/prism/RewardModel.h"

namespace storm {
    namespace prism {
        RewardModel::RewardModel(std::string const& rewardModelName, std::vector<storm::prism::StateReward> const& stateRewards, std::vector<storm::prism::TransitionReward> const& transitionRewards, std::string const& filename, uint_fast64_t lineNumber) : LocatedInformation(filename, lineNumber), rewardModelName(rewardModelName), stateRewards(stateRewards), transitionRewards(transitionRewards) {
            // Nothing to do here.
        }
        
        std::string const& RewardModel::getName() const {
            return this->rewardModelName;
        }
        
        bool RewardModel::hasStateRewards() const {
            return this->stateRewards.size() > 0;
        }
        
        std::vector<storm::prism::StateReward> const& RewardModel::getStateRewards() const {
            return this->stateRewards;
        }
        
        bool RewardModel::hasTransitionRewards() const {
            return this->transitionRewards.size() > 0;
        }
        
        std::vector<storm::prism::TransitionReward> const& RewardModel::getTransitionRewards() const {
            return this->transitionRewards;
        }
        
        RewardModel RewardModel::substitute(std::map<std::string, storm::expressions::Expression> const& substitution) const {
            std::vector<StateReward> newStateRewards;
            newStateRewards.reserve(this->getStateRewards().size());
            for (auto const& stateReward : this->getStateRewards()) {
                newStateRewards.emplace_back(stateReward.substitute(substitution));
            }
            
            std::vector<TransitionReward> newTransitionRewards;
            newTransitionRewards.reserve(this->getTransitionRewards().size());
            for (auto const& transitionReward : this->getTransitionRewards()) {
                newTransitionRewards.emplace_back(transitionReward.substitute(substitution));
            }
            return RewardModel(this->getName(), newStateRewards, newTransitionRewards, this->getFilename(), this->getLineNumber());
        }
        
        std::ostream& operator<<(std::ostream& stream, RewardModel const& rewardModel) {
            stream << "rewards \"" << rewardModel.getName() << "\"" << std::endl;
            for (auto const& reward : rewardModel.getStateRewards()) {
                stream << reward << std::endl;
            }
            for (auto const& reward : rewardModel.getTransitionRewards()) {
                stream << reward << std::endl;
            }
            stream << "endrewards" << std::endl;
            return stream;
        }
        
    } // namespace prism
} // namespace storm
