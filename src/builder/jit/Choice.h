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
                
                void compress();
                
            private:
                Distribution<IndexType, ValueType>& getDistribution();

                Distribution<IndexType, ValueType> distribution;
                bool compressed;
            };
            
        }
    }
}
