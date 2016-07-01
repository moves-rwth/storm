#include "src/storage/jani/RewardIncrement.h"

namespace storm {
    namespace jani {
        
        RewardIncrement::RewardIncrement(uint64_t rewardIndex, storm::expressions::Expression const& value) : rewardIndex(rewardIndex), value(value) {
            // Intentionally left empty.
        }
        
        uint64_t RewardIncrement::getRewardIndex() const {
            return rewardIndex;
        }
        
        storm::expressions::Expression const& RewardIncrement::getValue() const {
            return value;
        }
        
    }
}