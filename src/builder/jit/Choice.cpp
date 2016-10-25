#include "src/builder/jit/Choice.h"

namespace storm {
    namespace builder {
        namespace jit {
            
            template <typename IndexType, typename ValueType>
            void Choice<IndexType, ValueType>::add(DistributionEntry<IndexType, ValueType> const& entry) {
                distribution.add(entry);
            }

            template <typename IndexType, typename ValueType>
            Distribution<IndexType, ValueType> const& Choice<IndexType, ValueType>::getDistribution() const {
                return distribution;
            }
            
        }
    }
}
