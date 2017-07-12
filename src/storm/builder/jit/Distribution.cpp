#include "storm/builder/jit/Distribution.h"

#include "storm/adapters/RationalFunctionAdapter.h"

namespace storm {
    namespace builder {
        namespace jit {
            
            template <typename IndexType, typename ValueType>
            Distribution<IndexType, ValueType>::Distribution() : compressed(true) {
                // Intentionally left empty.
            }
            
            template <typename IndexType, typename ValueType>
            void Distribution<IndexType, ValueType>::add(DistributionEntry<IndexType, ValueType> const& entry) {
                storage.push_back(entry);
                compressed &= storage.back().getIndex() < entry.getIndex();
            }

            template <typename IndexType, typename ValueType>
            void Distribution<IndexType, ValueType>::add(IndexType const& index, ValueType const& value) {
                storage.emplace_back(index, value);
                compressed &= storage.back().getIndex() < index;
            }

            template <typename IndexType, typename ValueType>
            void Distribution<IndexType, ValueType>::add(Distribution&& distribution) {
                storage.insert(storage.end(), std::make_move_iterator(distribution.begin()), std::make_move_iterator(distribution.end()));
                compressed = false;
            }
            
            template <typename IndexType, typename ValueType>
            void Distribution<IndexType, ValueType>::compress() {
                if (!compressed) {
                    std::sort(storage.begin(), storage.end(),
                              [] (DistributionEntry<IndexType, ValueType> const& a, DistributionEntry<IndexType, ValueType> const& b) {
                                  return a.getIndex() < b.getIndex();
                              }
                              );
                    
                    // Code taken from std::unique and modified to fit needs.
                    auto first = storage.begin();
                    auto last = storage.end();
                    
                    if (first != last) {
                        auto result = first;
                        while (++first != last) {
                            if (!(result->getIndex() == first->getIndex())) {
                                if (++result != first) {
                                    *result = std::move(*first);
                                }
                            } else {
                                result->addToValue(first->getValue());
                            }
                        }
                        ++result;
                        
                        storage.resize(std::distance(storage.begin(), result));
                    }
                    compressed = true;
                }
            }
            
            template <typename IndexType, typename ValueType>
            void Distribution<IndexType, ValueType>::divide(ValueType const& value) {
                for (auto& entry : storage) {
                    entry.divide(value);
                }
            }
            
            template <typename IndexType, typename ValueType>
            typename Distribution<IndexType, ValueType>::ContainerType::iterator Distribution<IndexType, ValueType>::begin() {
                return storage.begin();
            }
            
            template <typename IndexType, typename ValueType>
            typename Distribution<IndexType, ValueType>::ContainerType::const_iterator Distribution<IndexType, ValueType>::begin() const {
                return storage.begin();
            }
            
            template <typename IndexType, typename ValueType>
            typename Distribution<IndexType, ValueType>::ContainerType::iterator Distribution<IndexType, ValueType>::end() {
                return storage.end();
            }
            
            template <typename IndexType, typename ValueType>
            typename Distribution<IndexType, ValueType>::ContainerType::const_iterator Distribution<IndexType, ValueType>::end() const {
                return storage.end();
            }
            
            template class Distribution<uint32_t, double>;
            template class Distribution<uint32_t, storm::RationalNumber>;
            template class Distribution<uint32_t, storm::RationalFunction>;
            
        }
    }
}
