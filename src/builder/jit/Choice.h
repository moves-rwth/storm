#pragma once

#include "src/builder/jit/Distribution.h"

namespace storm {
    namespace builder {
        namespace jit {
            
            template <typename IndexType, typename ValueType>
            class Choice {
            public:
                void add(DistributionEntry<IndexType, ValueType> const& entry);
                
                Distribution<IndexType, ValueType> const& getDistribution() const;
                
            private:
                Distribution<IndexType, ValueType> distribution;
            };
            
        }
    }
}
