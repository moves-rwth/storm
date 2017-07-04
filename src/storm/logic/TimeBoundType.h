#pragma once

namespace storm {
    namespace logic {

        enum class TimeBoundType {
            Steps,
            Time,
            Reward
        };


        class TimeBoundReference {
            TimeBoundType type;
            std::string rewardName;

        public:
            explicit TimeBoundReference(TimeBoundType t) : type(t) {
                // For rewards, use the other constructor.
                assert(t != TimeBoundType::Reward);
            }

            explicit TimeBoundReference(std::string const& rewardName) : type(TimeBoundType::Reward), rewardName(rewardName) {
                assert(rewardName != ""); // Empty reward name is reserved.
            }


            bool isStepBound() const {
                return type ==  TimeBoundType::Steps;
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
        };


    }
}
