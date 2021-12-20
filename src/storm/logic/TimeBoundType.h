#pragma once

#include <boost/optional.hpp>

#include "storm/logic/RewardAccumulation.h"

namespace storm {
namespace logic {

enum class TimeBoundType { Steps, Time, Reward };

class TimeBoundReference {
    TimeBoundType type;
    std::string rewardName;
    boost::optional<RewardAccumulation> rewardAccumulation;

   public:
    explicit TimeBoundReference(TimeBoundType t) : type(t) {
        // For rewards, use the other constructor.
        assert(t != TimeBoundType::Reward);
    }

    explicit TimeBoundReference(std::string const& rewardName, boost::optional<RewardAccumulation> rewardAccumulation = boost::none)
        : type(TimeBoundType::Reward), rewardName(rewardName), rewardAccumulation(rewardAccumulation) {
        assert(rewardName != "");  // Empty reward name is reserved.
    }

    TimeBoundReference(TimeBoundReference const& other) = default;

    TimeBoundType getType() const {
        return type;
    }

    bool isStepBound() const {
        return type == TimeBoundType::Steps;
    }

    bool isTimeBound() const {
        return type == TimeBoundType::Time;
    }

    bool isRewardBound() const {
        return type == TimeBoundType::Reward;
    }

    std::string const& getRewardName() const {
        assert(isRewardBound());
        return rewardName;
    }

    bool hasRewardAccumulation() const {
        return rewardAccumulation.is_initialized();
    }

    RewardAccumulation const& getRewardAccumulation() const {
        assert(isRewardBound());
        return rewardAccumulation.get();
    }

    boost::optional<RewardAccumulation> const& getOptionalRewardAccumulation() const {
        assert(isRewardBound());
        return rewardAccumulation;
    }
};

}  // namespace logic
}  // namespace storm
