#include "storm/builder/jit/DistributionEntry.h"

#include "storm/adapters/RationalFunctionAdapter.h"

namespace storm {
    namespace builder {
        namespace jit {
            
            template <typename IndexType, typename ValueType>
            DistributionEntry<IndexType, ValueType>::DistributionEntry() : index(0), value(0) {
                // Intentionally left empty.
            }
            
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
            
            template <typename IndexType, typename ValueType>
            void DistributionEntry<IndexType, ValueType>::addToValue(ValueType const& value) {
                this->value += value;
            }
            
            template <typename IndexType, typename ValueType>
            void DistributionEntry<IndexType, ValueType>::divide(ValueType const& value) {
                this->value /= value;
            }
         
            template class DistributionEntry<uint32_t, double>;
            template class DistributionEntry<uint32_t, storm::RationalNumber>;
            template class DistributionEntry<uint32_t, storm::RationalFunction>;

        }
    }
}
