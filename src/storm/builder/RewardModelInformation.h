#pragma once

#include <string>

namespace storm {
namespace builder {

class RewardModelInformation {
   public:
    RewardModelInformation(std::string const& name, bool stateRewards, bool stateActionRewards, bool transitionRewards);

    std::string const& getName() const;
    bool hasStateRewards() const;
    bool hasStateActionRewards() const;
    bool hasTransitionRewards() const;

    void setHasStateRewards();
    void setHasStateActionRewards();
    void setHasTransitionRewards();

   private:
    std::string name;
    bool stateRewards;
    bool stateActionRewards;
    bool transitionRewards;
};

}  // namespace builder
}  // namespace storm
