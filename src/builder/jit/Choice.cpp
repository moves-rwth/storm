#include "src/builder/jit/Choice.h"

#include "src/adapters/CarlAdapter.h"

namespace storm {
    namespace builder {
        namespace jit {
            
            template <typename IndexType, typename ValueType>
            Choice<IndexType, ValueType>::Choice() : compressed(true) {
                // Intentionally left empty.
            }
            
            template <typename IndexType, typename ValueType>
            void Choice<IndexType, ValueType>::add(DistributionEntry<IndexType, ValueType> const& entry) {
                distribution.add(entry);
            }
            
            template <typename IndexType, typename ValueType>
            void Choice<IndexType, ValueType>::add(IndexType const& index, ValueType const& value) {
                distribution.add(index, value);
            }

            template <typename IndexType, typename ValueType>
            void Choice<IndexType, ValueType>::add(Choice<IndexType, ValueType>&& choice) {
                distribution.add(std::move(choice.getDistribution()));
            }
            
            template <typename IndexType, typename ValueType>
            Distribution<IndexType, ValueType> const& Choice<IndexType, ValueType>::getDistribution() const {
                return distribution;
            }
            
            template <typename IndexType, typename ValueType>
            void Choice<IndexType, ValueType>::divideDistribution(ValueType const& value) {
                distribution.divide(value);
            }

            template <typename IndexType, typename ValueType>
            void Choice<IndexType, ValueType>::compress() {
                if (!compressed) {
                    distribution.compress();
                    compressed = true;
                }
            }

            template <typename IndexType, typename ValueType>
            Distribution<IndexType, ValueType>& Choice<IndexType, ValueType>::getDistribution() {
                return distribution;
            }
            
            template class Choice<uint32_t, double>;
            template class Choice<uint32_t, storm::RationalNumber>;
            template class Choice<uint32_t, storm::RationalFunction>;

        }
    }
}
