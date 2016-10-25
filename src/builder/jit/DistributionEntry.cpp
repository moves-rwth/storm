#include "src/builder/jit/DistributionEntry.h"

namespace storm {
    namespace builder {
        namespace jit {
            
            template <typename IndexType, typename ValueType>
            DistributionEntry<IndexType, ValueType>::DistributionEntry(IndexType const& index, ValueType const& value) : index(index), value(value) {
                // Intentionally left empty.
            }
            
            template <typename IndexType, typename ValueType>
            IndexType const& DistributionEntry<IndexType, ValueType>::getIndex() const {
                return index;
            }
            
            template <typename IndexType, typename ValueType>
            ValueType const& DistributionEntry<IndexType, ValueType>::getValue() const {
                return value;
            }
            
        }
    }
}
