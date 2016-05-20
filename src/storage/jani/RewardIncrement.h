#pragma once

#include <cstdint>

#include "src/storage/expressions/Expression.h"

namespace storm {
    namespace jani {
        
        class RewardIncrement {
        public:
            /*!
             * Creates an increment of a reward (given by its id) by the given expression.
             *
             * @param rewardId The id of the reward to increment.
             * @param value The expression defining the amount the reward is the incremented.
             */
            RewardIncrement(uint64_t rewardId, storm::expressions::Expression const& value);
            
        private:
            // The id of the reward that is to be incremented.
            uint64_t rewardId;
            
            // The expression defining the amount the reward is to be incremented.
            storm::expressions::Expression value;
        };
        
    }
}