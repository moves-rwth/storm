#pragma once

#include <cstdint>

#include "src/storage/expressions/Expression.h"

namespace storm {
    namespace jani {
        
        class RewardIncrement {
        public:
            /*!
             * Creates an increment of a reward (given by its index) by the given expression.
             *
             * @param rewardIndex The index of the reward to increment.
             * @param value The expression defining the amount the reward is the incremented.
             */
            RewardIncrement(uint64_t rewardIndex, storm::expressions::Expression const& value);

            /*!
             * Retrieves the index of the reward to increment.
             */
            uint64_t getRewardIndex() const;
            
            /*!
             * Retrieves the expression defining the amount by which the reward is to be incremented.
             */
            storm::expressions::Expression const& getValue() const;
            
        private:
            // The index of the reward that is to be incremented.
            uint64_t rewardIndex;
            
            // The expression defining the amount the reward is to be incremented.
            storm::expressions::Expression value;
        };
        
    }
}