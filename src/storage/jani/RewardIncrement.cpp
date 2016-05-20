#include "src/storage/jani/RewardIncrement.h"

namespace storm {
    namespace jani {
        
        RewardIncrement::RewardIncrement(uint64_t rewardId, storm::expressions::Expression const& value) : rewardId(rewardId), value(value) {
            // Intentionally left empty.
        }
        
    }
}