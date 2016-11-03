#pragma once

#include "src/builder/jit/Distribution.h"

namespace storm {
    namespace builder {
        namespace jit {
            
            template <typename IndexType, typename ValueType>
            class Choice {
            public:
                Choice();
                
                void add(DistributionEntry<IndexType, ValueType> const& entry);
                void add(IndexType const& index, ValueType const& value);
                void add(Choice<IndexType, ValueType>&& choice);
                
                Distribution<IndexType, ValueType> const& getDistribution() const;
                void divideDistribution(ValueType const& value);
                
                /*!
                 * Adds the given value to the reward associated with this choice.
                 */
                void addReward(ValueType const& value);
                
                /*!
                 * Adds the given choices rewards to this choice.
                 */
                void addRewards(std::vector<ValueType>&& values);
                
                /*!
                 * Retrieves the rewards for this choice.
                 */
                std::vector<ValueType> const& getRewards() const;
                
                /*!
                 * Compresses the underlying distribution.
                 */
                void compress();
                
            private:
                Distribution<IndexType, ValueType>& getDistribution();

                /// The distribution of this choice.
                Distribution<IndexType, ValueType> distribution;
                
                /// The reward values associated with this choice.
                std::vector<ValueType> rewards;
            };
            
        }
    }
}
